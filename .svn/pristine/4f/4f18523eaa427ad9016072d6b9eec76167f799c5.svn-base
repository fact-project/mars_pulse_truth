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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHRanForest
//
// This histogram shows a measure of variable importance (mean decrease in
// Gini-index)
//
////////////////////////////////////////////////////////////////////////////
#include "MHRanForestGini.h"

#include <TH1.h>
#include <TPad.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMarker.h>

#include "MParList.h"
#include "MBinning.h"
#include "MRanTree.h"
#include "MRanForest.h"
#include "MDataArray.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHRanForestGini);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms, nbins is the number of bins used for the evaluation.
// The default is 100 bins.
//
MHRanForestGini::MHRanForestGini(Int_t nbins, const char *name, const char *title)
    : fRules(0.01, 0.01, 0.99, 0.99)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHRanForestGini";
    fTitle = title ? title : "Measure of importance of Random Forest-input parameters";

    fGraphGini.SetNameTitle("Gini", "Importance of RF-input parameters measured by mean Gini decrease");
    fGraphGini.SetMarkerStyle(28);
    fGraphGini.SetFillColor(38);

    fGraphError.SetNameTitle("ResErr", "Resolution/Error versus train step");
    fGraphError.SetMarkerStyle(kFullDotMedium);

    fGraphNodes.SetNameTitle("Nodes", "Number of nodes versus train step");
    fGraphNodes.SetMarkerStyle(kFullDotMedium);

    fRules.SetTextAlign(13);
    fRules.SetTextSize(0.05);
}

// --------------------------------------------------------------------------
//
// Setup Filling of the histograms. It needs:
//  MMcEvt and MRanForest
//
Bool_t MHRanForestGini::SetupFill(const MParList *plist)
{
    fRanForest = (MRanForest*)plist->FindObject("MRanForest");
    if (!fRanForest)
    {
        *fLog << err << dbginf << "MRanForest not found... aborting." << endl;
        return kERROR;
    }

    fGini.Set(fRanForest->GetNumDim());
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the RanForest from a MRanForest container into the corresponding
// histogram dependant on the particle id.
//
//
Int_t MHRanForestGini::Fill(const MParContainer *par, const Stat_t w)
{
    MRanTree *t = fRanForest->GetCurTree();

    for (Int_t i=0;i<fRanForest->GetNumDim();i++)
        fGini[i] += t->GetGiniDec(i);

    fGraphError.SetPoint(fGraphError.GetN(), GetNumExecutions(), t->GetError());
    fGraphNodes.SetPoint(fGraphError.GetN(), GetNumExecutions(), t->GetNumEndNodes());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Bool_t MHRanForestGini::Finalize()
{
    // --- Calculate mean decrease of gini index ---
    const Int_t n = fGini.GetSize();

    fGraphGini.Set(n);

    for (Int_t i=0; i<n; i++)
    {
        fGini[i] /= fRanForest->GetNumTrees()*fRanForest->GetNumData();
        fGraphGini.SetPoint(i, i+1, fGini[i]);
    }

    // --- Produce some text information ---
    fRules.AddText("");
    fRules.AddText(Form("%s w/ %d trees of node size %d trained by %d evts",
                        fRanForest->IsClassify()?"Classification":"Regression",
                        fRanForest->GetNumTrees(),
                        fRanForest->GetNdSize(),
                        fRanForest->GetNumData()));
    fRules.AddText("---");//Form("---> %s", fRanForest->GetTargetRule().Data()));

    const MDataArray &arr = *fRanForest->GetRules();

    int i;
    for (i=0; i<arr.GetNumEntries(); i++)
        fRules.AddText(Form("%d) %s", i+1, arr.GetRule(i).Data()));

    for (; i<20; i++)
        fRules.AddText("");

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draw histogram. (For the Meaning see class description)
//
void MHRanForestGini::Draw(Option_t *)
{
    if (fGraphGini.GetN()==0)
        return;

    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fGraphGini.Draw("AB");

    TH1 *h = fGraphGini.GetHistogram();
    if (h)
    {
        h->SetXTitle("No.of RF-input parameter");
        h->SetYTitle("Mean decrease in Gini-index [au]");
        h->GetXaxis()->SetNdivisions(10);
        h->SetMinimum(0);
    }

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridy();
    fGraphError.Draw("ALP");
    h = fGraphError.GetHistogram();
    if (h)
    {
        h->SetXTitle("Train step/Tree number");
        h->SetYTitle("Error/Resolution");
        h->SetMinimum(0);
    }

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridy();
    fGraphNodes.Draw("ALP");
    h = fGraphNodes.GetHistogram();
    if (h)
    {
        h->SetXTitle("Train step/Tree number");
        h->SetYTitle("Number of end nodes");
        h->SetMinimum(0);
    }

    pad->cd(4);
    gPad->SetBorderMode(0);
    fRules.Draw();
}
