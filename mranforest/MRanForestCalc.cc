/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Hengstebeck 2/2005 <mailto:hengsteb@physik.hu-berlin.de>
!   Author(s): Thomas Bretz 8/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRanForestCalc
//
//
////////////////////////////////////////////////////////////////////////////
#include "MRanForestCalc.h"

#include <stdlib.h> // Needed for atof in some cases

#include <TMath.h>

#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TVector.h>

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MData.h"
#include "MDataArray.h"

#include "MRanForest.h"
#include "MParameters.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"
#include "MRanForestGrow.h"
#include "MFillH.h"

ClassImp(MRanForestCalc);

using namespace std;

const TString MRanForestCalc::gsDefName    = "MRanForestCalc";
const TString MRanForestCalc::gsDefTitle   = "RF for energy estimation";

const TString MRanForestCalc::gsNameOutput   = "RanForestOut";
const TString MRanForestCalc::gsNameEvalFunc = "EvalFunction";

MRanForestCalc::MRanForestCalc(const char *name, const char *title)
    : fData(0), fRFOut(0), fTestMatrix(0), fFunc("x"),
    fNumTrees(-1), fNumTry(-1), fNdSize(-1), fNumObsoleteVariables(1),
    fLastDataColumnHasWeights(kFALSE),
    fNameOutput(gsNameOutput), fDebug(kFALSE), fEstimationMode(kMean)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    // FIXME:
    fNumTrees = 100; //100
    fNumTry   = 0;   //3   0 means: in MRanForest estimated best value will be calculated
    fNdSize   = 1;   //1   
}

MRanForestCalc::~MRanForestCalc()
{
    fEForests.Delete();
}

// --------------------------------------------------------------------------
//
// Set a function which is applied to the output of the random forest
//
Bool_t MRanForestCalc::SetFunction(const char *func)
{
    return !fFunc.SetRule(func);
}

// --------------------------------------------------------------------------
//
// ver=0: One yes/no-classification forest is trained for each bin.
//        the yes/no classification is done using the grid
// ver=1: One classification forest is trained. The last column contains a
//        value which is turned into a classifier by rf itself using the grid
// ver=2: One classification forest is trained. The last column already contains
//        the classifier
// ver=3: A regression forest is trained. The last column contains the
//        classifier
//
Int_t MRanForestCalc::Train(const MHMatrix &matrixtrain, const TArrayD &grid, Int_t ver)
{
    gLog.Separator("MRanForestCalc - Train");

    if (!matrixtrain.GetColumns())
    {
        *fLog << err << "ERROR - MHMatrix does not contain rules... abort." << endl;
        return kFALSE;
    }

    const Int_t ncols = matrixtrain.GetM().GetNcols();
    const Int_t nrows = matrixtrain.GetM().GetNrows();
    if (ncols<=0 || nrows <=0)
    {
        *fLog << err << "ERROR - No. of columns or no. of rows of matrixtrain equal 0 ... abort." << endl;
        return kFALSE;
    }

    // rules (= combination of image par) to be used for energy estimation
    TFile fileRF(fFileName, "recreate");
    if (!fileRF.IsOpen())
    {
        *fLog << err << "ERROR - File to store RFs could not be opened... abort." << endl;
        return kFALSE;
    }

    // The number of columns which have to be removed for the training
    // The last data column may contain weight which also have to be removed
    const Int_t nobs = fNumObsoleteVariables + (fLastDataColumnHasWeights?1:0); // Number of obsolete columns

    const MDataArray &dcol = *matrixtrain.GetColumns();

    // Make a copy of the rules for accessing the train-data
    MDataArray usedrules;
    for (Int_t i=0; i<ncols; i++)
        if (i<ncols-nobs)  // -3 is important!!!
            usedrules.AddEntry(dcol[i].GetRule());
        else
            *fLog << inf << "Skipping " << dcol[i].GetRule() << " for training" << endl;

    // In the case of regression store the rule to be regessed in the
    // last entry of your rules
    MDataArray rules(usedrules);
    rules.AddEntry(ver<3?"Classification.fVal":dcol[ncols-1].GetRule().Data());

    // prepare train-matrix finally used
    TMatrix mat(matrixtrain.GetM());

    // Resize it such that the obsolete columns are removed
    mat.ResizeTo(nrows, ncols-nobs+1);

    if (fDebug)
        gLog.SetNullOutput(kTRUE);

    // In the case one independant RF is trained for each bin (e.g.
    // energy-bin) train all of them
    const Int_t nbins = ver>0 ? 1 : grid.GetSize()-1;
    for (Int_t ie=0; ie<nbins; ie++)
    {
        // In the case weights should be used initialize the
        // corresponding array
        Double_t sum = 0;

        TArrayF weights(nrows);
        if (fLastDataColumnHasWeights)
        {
            for (Int_t j=0; j<nrows; j++)
            {
                weights[j] = matrixtrain.GetM()(j, ncols-nobs);
                sum += weights[j];
            }
        }

        *fLog << inf << "MRanForestCalc::Train: Sum of weights " << sum << endl;

        // Setup the matrix such that the last comlumn contains
        // the classifier or the regeression target value
        switch (ver)
        {
        case 0: // Replace last column by a classification which is 1 in
                // the case the event belongs to this bin, 0 otherwise
            {
                Int_t irows=0;
                for (Int_t j=0; j<nrows; j++)
                {
                    const Double_t value  = matrixtrain.GetM()(j,ncols-1);
                    const Bool_t   inside = value>grid[ie] && value<=grid[ie+1];

                    mat(j, ncols-nobs) = inside ? 1 : 0;

                    if (inside)
                        irows++;
                }
                if (irows==0)
                    *fLog << warn << "WARNING - Skipping";
                else
                    *fLog << inf << "Training RF for";

                *fLog << " bin " << ie << " (" << grid[ie] << ", " << grid[ie+1] << ") " << irows << "/" << nrows << endl;

                if (irows==0)
                    continue;
            }
            break;

        case 1: // Use last column as classifier or for regression
        case 2:
        case 3:
            for (Int_t j=0; j<nrows; j++)
                mat(j, ncols-nobs) = matrixtrain.GetM()(j,ncols-1);
            break;
        }

        MHMatrix matrix(mat, &rules, "MatrixTrain");

        MParList plist;
        MTaskList tlist;
        plist.AddToList(&tlist);
        plist.AddToList(&matrix);

        MRanForest rf;
        rf.SetNumTrees(fNumTrees);
        rf.SetNumTry(fNumTry);
        rf.SetNdSize(fNdSize);
        rf.SetClassify(ver<3 ? kTRUE : kFALSE);
        if (ver==1)
            rf.SetGrid(grid);
        if (fLastDataColumnHasWeights)
            rf.SetWeights(weights);

        plist.AddToList(&rf);

        MRanForestGrow rfgrow;
        tlist.AddToList(&rfgrow);

        MFillH fillh("MHRanForestGini");
        tlist.AddToList(&fillh);

        MEvtLoop evtloop(fTitle);
        evtloop.SetParList(&plist);
        evtloop.SetDisplay(fDisplay);
        evtloop.SetLogStream(fLog);

        if (!evtloop.Eventloop())
            return kFALSE;

        if (fDebug)
            gLog.SetNullOutput(kFALSE);

        if (ver==0)
        {
            // Calculate bin center
            const Double_t E = (TMath::Log10(grid[ie])+TMath::Log10(grid[ie+1]))/2;

            // save whole forest
            rf.SetUserVal(E);
            rf.SetName(Form("%.10f", E));
        }

        rf.Write();
    }

    // save rules
    usedrules.Write("rules");

    fFunc.Write(gsNameEvalFunc);

    return kTRUE;
}

Int_t MRanForestCalc::ReadForests(MParList &plist)
{
    TFile fileRF(fFileName, "read");
    if (!fileRF.IsOpen())
    {
        *fLog << err << dbginf << "File containing RFs could not be opened... aborting." << endl;
        return kFALSE;
    }

    fEForests.Delete();

    TIter Next(fileRF.GetListOfKeys());
    TObject *o=0;
    while ((o=Next()))
    {
        MRanForest *forest=0;
        fileRF.GetObject(o->GetName(), forest);
        if (!forest)
            continue;

        forest->SetUserVal(atof(o->GetName()));

        fEForests.Add(forest);
    }

    // Maybe fEForests[0].fRules could be used instead?
    if (fData->Read("rules")<=0)
    {
        *fLog << err << "ERROR - Reading 'rules' from file " << fFileName << endl;
        return kFALSE;
    }

    if (fileRF.GetListOfKeys()->FindObject(gsNameEvalFunc))
    {
        if (fFunc.Read(gsNameEvalFunc)<=0)
        {
            *fLog << err << "ERROR - Reading '" << gsNameEvalFunc << "' from file " << fFileName << endl;
            return kFALSE;
        }

        *fLog << inf << "Evaluation function found in file: " << fFunc.GetRule() << endl;
    }

    return kTRUE;
}

Int_t MRanForestCalc::PreProcess(MParList *plist)
{
    fRFOut = (MParameterD*)plist->FindCreateObj("MParameterD", fNameOutput);
    if (!fRFOut)
        return kFALSE;

    fData = (MDataArray*)plist->FindCreateObj("MDataArray");
    if (!fData)
        return kFALSE;

    if (!ReadForests(*plist))
    {
        *fLog << err << "Reading RFs failed... aborting." << endl;
        return kFALSE;
    }

    *fLog << inf << "RF read from " << fFileName << endl;

    if (!fFunc.PreProcess(plist))
    {
        *fLog << err << "PreProcessing of evaluation function failed... aborting." << endl;
        return kFALSE;
    }

    if (fTestMatrix)
        return kTRUE;

    fData->Print();

    if (!fData->PreProcess(plist))
    {
        *fLog << err << "PreProcessing of the MDataArray failed... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

Double_t MRanForestCalc::Eval() const
{
    TVector event;
    if (fTestMatrix)
        *fTestMatrix >> event;
    else
        *fData >> event;

    // --------------- Single Tree RF -------------------
    if (fEForests.GetEntriesFast()==1)
    {
        MRanForest *rf = static_cast<MRanForest*>(fEForests.UncheckedAt(0));
        return rf->CalcHadroness(event);
    }

    // --------------- Multi Tree RF -------------------
    static TF1 f1("f1", "gaus");

    Double_t sume = 0;
    Double_t sumh = 0;
    Double_t maxh = 0;
    Double_t maxe = 0;

    Double_t max  = -1e10;
    Double_t min  =  1e10;

    TIter Next(&fEForests);
    MRanForest *rf = 0;

    TGraph g;
    while ((rf=(MRanForest*)Next()))
    {
        const Double_t h = rf->CalcHadroness(event);
        const Double_t e = rf->GetUserVal();

        g.SetPoint(g.GetN(), e, h);

        sume += e*h;
        sumh += h;

        if (h>maxh)
        {
            maxh = h;
            maxe = e;
        }
        if (e>max)
            max = e;
        if (e<min)
            min = e;
    }

    switch (fEstimationMode)
    {
    case kMean:
        return sume/sumh;
    case kMaximum:
        return maxe;
    case kFit:
        f1.SetParameter(0, maxh);
        f1.SetParameter(1, maxe);
        f1.SetParameter(2, 0.125);
        g.Fit(&f1, "Q0N");
        return f1.GetParameter(1);
    }

    return 0;
}

Int_t MRanForestCalc::Process()
{
    const Double_t val = Eval();

    fRFOut->SetVal(fFunc.Eval(val));
    fRFOut->SetReadyToSave();

    return kTRUE;
}

void MRanForestCalc::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << " - Forest ";
    switch (fEForests.GetEntries())
    {
    case 0:  *fLog << "not yet initialized." << endl;     break;
    case 1:  *fLog << "is a single tree forest." << endl; break;
    default: *fLog << "is a multi tree forest." << endl;  break;
    }
    /*
     *fLog << " - Trees: " << fNumTrees << endl;
     *fLog << " - Trys: " << fNumTry << endl;
     *fLog << " - Node Size: " << fNdSize << endl;
     *fLog << " - Node Size: " << fNdSize << endl;
     */
    *fLog << " - FileName: " << fFileName << endl;
    *fLog << " - NameOutput: " << fNameOutput << endl;
}

// --------------------------------------------------------------------------
//
//
Int_t MRanForestCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        SetFileName(GetEnvValue(env, prefix, "FileName", fFileName));
    }
    if (IsEnvDefined(env, prefix, "Debug", print))
    {
        rc = kTRUE;
        SetDebug(GetEnvValue(env, prefix, "Debug", fDebug));
    }
    if (IsEnvDefined(env, prefix, "NameOutput", print))
    {
        rc = kTRUE;
        SetNameOutput(GetEnvValue(env, prefix, "NameOutput", fNameOutput));
    }
    if (IsEnvDefined(env, prefix, "EstimationMode", print))
    {
        TString txt = GetEnvValue(env, prefix, "EstimationMode", "");
        txt = txt.Strip(TString::kBoth);
        txt.ToLower();
        if (txt==(TString)"mean")
            fEstimationMode = kMean;
        if (txt==(TString)"maximum")
            fEstimationMode = kMaximum;
        if (txt==(TString)"fit")
            fEstimationMode = kFit;
        rc = kTRUE;
    }
    return rc;
}
