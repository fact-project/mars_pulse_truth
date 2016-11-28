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
!   Author(s): Thomas Hengstebeck 3/2003 <mailto:hengsteb@alwa02.physik.uni-siegen.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHRanForest
//
// This histogram shows the evolution of the standard deviation 
// of est. hadronness as the number of trees increases. 
// It helps you to find out how many trees are really needed in g/h-sep.
//
////////////////////////////////////////////////////////////////////////////
#include "MHRanForest.h"

#include <TH1.h>
#include <TPad.h>
#include <TMath.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMarker.h>

#include "MParList.h"
#include "MBinning.h"
#include "MRanForest.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"

ClassImp(MHRanForest);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms, nbins is the number of bins used for the evaluation.
// The default is 100 bins.
//
MHRanForest::MHRanForest(Int_t nbins, const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHRanForest";
    fTitle = title ? title : "Histogram showing Standard deviation of estimated hadronness";

    fGraphSigma = new TGraph;
    fGraphSigma->SetTitle("Evolution of Standard deviation of estimated hadronness in tree combination");
    fGraphSigma->SetMarkerStyle(kFullDotSmall);
}

// --------------------------------------------------------------------------
//
// Delete the histograms.
//
MHRanForest::~MHRanForest()
{
    delete fGraphSigma;
}

// --------------------------------------------------------------------------
//
// Setup Filling of the histograms. It needs:
//  MMcEvt and MRanForest
//
Bool_t MHRanForest::SetupFill(const MParList *plist)
{
    fMcEvt = (MMcEvt*)plist->FindObject(AddSerialNumber("MMcEvt"));
    if (!fMcEvt)
    {
        *fLog << err << AddSerialNumber("MMcEvt") << " not found... aborting." << endl;
        return kFALSE;
    }

    fRanForest = (MRanForest*)plist->FindObject("MRanForest");
    if (!fRanForest)
    {
        *fLog << err << "MRanForest not found... aborting." << endl;
        return kFALSE;
    }

    fSigma.Set(fRanForest->GetNumTrees());
    fNumEvent=0;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MHRanForest::Fill(const MParContainer *par, const Stat_t w)
{
    fNumEvent++;

    Double_t hest=0;
    Double_t htrue=fMcEvt->GetPartId()==MMcEvt::kGAMMA ? 0. : 1.;

    Int_t ntrees=fRanForest->GetNumTrees();

    for (Int_t i=0;i<ntrees;i++)
    {
        for(Int_t j=0;j<=i;j++)
            hest+=fRanForest->GetTreeHad(j);

        hest/=i+1;

        const Double_t val = htrue-hest;
        fSigma[i] += val*val;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Finalize the histogram:
// calculate standard deviation and set histogram max and min
//
Bool_t MHRanForest::Finalize()
{
    Int_t n = fSigma.GetSize();

    fGraphSigma->Set(n);

    Stat_t max=0.;
    Stat_t min=0.;
    for (Int_t i=0; i<n; i++)
    {
	fSigma[i] = TMath::Sqrt(fSigma[i]/fNumEvent);

        const Stat_t ig = fSigma[i];
        if (ig>max) max = ig;
        if (ig<min) min = ig;
        fGraphSigma->SetPoint(i, i+1, ig);
    }

    // This is used in root>3.04/? so that SetMaximum/Minimum can succeed
    fGraphSigma->GetHistogram();

    fGraphSigma->SetMaximum(1.05*max);
    fGraphSigma->SetMinimum(0.95*min);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draw histogram. (For the Meaning see class description)
//
void MHRanForest::Draw(Option_t *)
{
   if (fGraphSigma->GetN()==0)
        return;

    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    fGraphSigma->Draw("ALP");
    pad->Modified();
    pad->Update();

    TH1 *h=fGraphSigma->GetHistogram();
    if (!h)
        return;

    //h->GetXaxis()->SetRangeUser(0, fNumEvent+1);
    h->SetXTitle("No.of Trees");
    h->SetYTitle("\\sigma of est.hadronness");

    pad->Modified();
    pad->Update();
}
