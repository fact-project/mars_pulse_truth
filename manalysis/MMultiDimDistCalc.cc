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
!   Author(s): Thomas Bretz, 5/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Rudy Bock, 5/2002 <mailto:rkb@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMultiDimDistCalc
//
//  Calculated a multidimensional distance. It calculates the distance to
//  all vectors in a given matrix describing Gammas and another one
//  describing Hadrons (non gammas). The shortest distances are avaraged.
//  How many distances are used for avaraging can be specified in the
//  constructor.
//
//  * If you want to use the nearest neighbor function for calculation use:
//      MMultiDimDistCalc::SetUseKernelMethod(kFALSE);
//  * If you want to use the kernel function for calculation use:
//      MMultiDimDistCalc::SetUseKernelMethod(kTRUE); <default>
//  * To use only the n next neighbors for your calculation use:
//      MMultiDimDistCalc::SetUseNumRows(n);
//  * To use all reference events set the number to 0 <default>
//
////////////////////////////////////////////////////////////////////////////
#include "MMultiDimDistCalc.h"

#include <fstream>

#include <math.h>

#include <TVector.h>

#include "MHMatrix.h" // must be before MLogManip.h

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
//#include "MDataPhrase.h"
#include "MDataArray.h"

#include "MParameters.h"

ClassImp(MMultiDimDistCalc);

using namespace std;

static const TString gsDefName  = "MMultiDimDistCalc";
static const TString gsDefTitle = "Calculate Hadronness with Nearest Neighbor/Kernel Method";

// --------------------------------------------------------------------------
//
// Setup histograms and the number of distances which are used for
// avaraging in CalcDist
//
MMultiDimDistCalc::MMultiDimDistCalc(const char *name, const char *title)
    : fNum(0), fUseKernel(kTRUE), fHadronnessName("MHadronness"), fData(NULL)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    /*
     fData = new TList;
     fData->SetOwner();
     */
}

// --------------------------------------------------------------------------
//
// Needs:
//  - MatrixGammas  [MHMatrix]
//  - MatrixHadrons {MHMatrix]
//  - MHadroness
//  - all data containers used to build the matrixes
//
// The matrix object can be filles using MFillH. And must be of the same
// number of columns (with the same meaning).
//
Int_t MMultiDimDistCalc::PreProcess(MParList *plist)
{
    fMGammas = (MHMatrix*)plist->FindObject("MatrixGammas", "MHMatrix");
    if (!fMGammas)
    {
        *fLog << err << dbginf << "MatrixGammas [MHMatrix] not found... aborting." << endl;
        return kFALSE;
    }

    fMHadrons = (MHMatrix*)plist->FindObject("MatrixHadrons", "MHMatrix");
    if (!fMHadrons)
    {
        *fLog << err << dbginf << "MatrixHadrons [MHMatrix] not found... aborting." << endl;
        return kFALSE;
    }

    if (fMGammas->GetM().GetNcols() != fMHadrons->GetM().GetNcols())
    {
        *fLog << err << dbginf << "Error matrices have different numbers of columns... aborting." << endl;
        return kFALSE;
    }

    /*
    TIter Next(fMGammas->GetRules());
    TObject *data=NULL;
    while ((data=Next()))
    {
        MDataPhrase *chain = new MDataPhrase(data->GetName());
        if (!chain->PreProcess(plist))
        {
            delete chain;
            return kFALSE;
        }
        fData->Add(chain);
    }
    */
    fData = fMGammas->GetColumns();
    if (!fData)
    {
        *fLog << err << dbginf << "Error matrix doesn't contain columns... aborting." << endl;
        return kFALSE;
    }

    if (!fData->PreProcess(plist))
    {
        *fLog << err << dbginf << "PreProcessing of the MDataArray failed for the columns failed... aborting." << endl;
        return kFALSE;
    }

    fHadronness = (MParameterD*)plist->FindCreateObj("MParameterD", fHadronnessName);
    if (!fHadronness)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluates the avarage over the fNum shortest distances in a
// multidimensional space to a matrix (set of vectors) describing
// gammas and hadrons.
// The hadroness of the event is defines as the avarage distance to the
// set of gammas (dg) divided by the avarage distance to the set of
// hadrons (dh). Because this value is in teh range [0, inf] it is
// transformed into [0,1] by:
//  H = exp(-dh/dg);
//
Int_t MMultiDimDistCalc::Process()
{
    // first copy the data from the data array to a vector event
    TVector event;
    *fData >> event;

    Int_t numg = fNum;
    Int_t numh = fNum;
    if (fNum==0)
    {
        numg = fMGammas->GetM().GetNrows();
        numh = fMHadrons->GetM().GetNrows();
    }
    if (fUseKernel)
    {
        numg = -numg;
        numh = -numh;
    }

    const Double_t dg = fMGammas->CalcDist(event, numg);
    const Double_t dh = fMHadrons->CalcDist(event, numh);

    if (dg<0 || dh<0)
    {
        *fLog << err << "MHMatrix::CalcDist failed (dg=" << dg << ", dh=" << dh << ")... aborting" << endl;
        return kERROR;
    }

    fHadronness->SetVal(dg==0 ? 0 : exp(-dh/dg));
    fHadronness->SetReadyToSave();

    return kTRUE;
}

void MMultiDimDistCalc::StreamPrimitive(ostream &out) const
{
    out << "   MMultiDimDist " << GetUniqueName();

    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\")";
    }
    out << ";" << endl;

    if (fHadronnessName!="MHadronness")
        out << "   " << GetUniqueName() << ".SetHadronnessName(\"" << fHadronnessName << "\");" << endl;
    if (fNum!=0)
        out << "   " << GetUniqueName() << ".SetUseNumRows(" << fNum << ");" << endl;
    if (fUseKernel)
        out << "   " << GetUniqueName() << ".SetUseKernelMethod();" << endl;
}

