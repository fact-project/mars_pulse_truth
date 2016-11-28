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
!   Author(s): Thomas Bretz  12/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHThreshold
//
// Class to calculate the threshold of your analysis.
//
// It fills a histogram dN/dE vs E with a fine binning. The threshold is
// determined as the logarithmic center of the bin with the maximum
// content.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHThreshold.h" 

#include <TMath.h>

#include <TF1.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TPaveStats.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"
#include "MBinning.h"

#include "MMcEvt.hxx"

#include "MParList.h"

ClassImp(MHThreshold);

using namespace std;

// --------------------------------------------------------------------------
//
//  Creates the three necessary histograms:
//   - selected showers (input)
//   - all showers (input)
//   - collection area (result)
//
MHThreshold::MHThreshold(const char *name, const char *title)
{ 
    //   initialize the histogram for the distribution r vs E
    //
    //   we set the energy range from 2 Gev to 20000 GeV (in log 4 orders
    //   of magnitude) and for each order we take 25 subdivision --> 100 xbins
    //
    //   we set the radius range from 0 m to 500 m with 10 m bin --> 50 ybins
    //
    fName  = name  ? name  : "MHThreshold";
    fTitle = title ? title : "Histogram to determin the threshold";

    fHEnergy.SetName("Threshold");
    fHEnergy.SetTitle("Energy Threshold");
    fHEnergy.SetXTitle("E [GeV]");
    fHEnergy.SetYTitle("dN/dE [GeV^{-1}]");
    fHEnergy.SetDirectory(NULL);
    fHEnergy.UseCurrentStyle();

    MBinning binse(50, 20, 50000, "", "log");
    binse.Apply(fHEnergy);

    fHEnergy.Sumw2();
}

Bool_t MHThreshold::SetupFill(const MParList *pl)
{
    fMcEvt = (MMcEvt*)pl->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... abort." << endl;
        return kFALSE;
    }

    ApplyBinning(*pl, "Threshold", fHEnergy);

    return kTRUE;
}

void MHThreshold::Paint(Option_t *option)
{
    if (fHEnergy.GetEntries()==0)
        return;

    TH1D h(fHEnergy);
    h.SetDirectory(0);

#if ROOT_VERSION_CODE<ROOT_VERSION(5,20,00)
    Int_t min=1;
    while (min<h.GetNbinsX() && h.GetBinContent(min)==0)
        min++;
    Int_t max=h.GetNbinsX();
    while (max>min && h.GetBinContent(max)==0)
        max--;

    if (max-min<5)
        return;

    h.Smooth(1, min, max);
#else
    h.Smooth(1);
#endif

    const Int_t bin  =  h.GetMaximumBin();
    const TAxis &axe = *h.GetXaxis();

    // Assume that the energy binning is logarithmic
    const Axis_t maxe = TMath::Sqrt(axe.GetBinLowEdge(bin)*axe.GetBinUpEdge(bin));

    TLatex text(0.30, 0.95, MString::Format("E_{max}=%dGeV (%dGeV,%dGeV)", TMath::Nint(maxe),
                                            TMath::Nint(axe.GetBinLowEdge(bin)),
                                            TMath::Nint(axe.GetBinUpEdge(bin))));
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.04);
    text.Paint();
}

void MHThreshold::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);

    // Do the projection before painting the histograms into
    // the individual pads
    pad->SetBorderMode(0);

    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();

    fHEnergy.Draw();

    AppendPad();
}

Int_t MHThreshold::Fill(const MParContainer *par, const Stat_t weight)
{
    const Double_t energy = fMcEvt->GetEnergy();

    fHEnergy.Fill(energy, weight/energy);

    return kTRUE;
}
