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
!   Author(s): Javier Lopez 05/2001 <mailto:jlopez@ifae.es>
!   Author(s): Thomas Bretz 05/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MHMcEfficiencyEnergy
//
// This class holds the information (histogram and fit function)
// about the energy threshold for a particular trigger condition.
//
////////////////////////////////////////////////////////////////////////////
#include "MHMcEfficiencyEnergy.h" 

#include <math.h>

#include <TH2.h>
#include <TCanvas.h>

#include "MH.h"
#include "MBinning.h"

#include "MHMcEnergyImpact.h"

ClassImp(MHMcEfficiencyEnergy);

// -------------------------------------------------------------------------
//
//  Default Constructor.
//
MHMcEfficiencyEnergy::MHMcEfficiencyEnergy(const char *name, const char *title)
    : fHist()
{
    fName  = name  ? name  : "MHMcEfficiencyEnergy";
    fTitle = title ? title : "Trigger Efficieny vs. Energy";

    //  - we initialize the histogram
    //  - we have to give diferent names for the diferent histograms because
    //    root don't allow us to have diferent histograms with the same name

    fHist.SetName(fName);
    fHist.SetTitle(fTitle);

    fHist.SetDirectory(NULL);

    fHist.SetXTitle("E [GeV]");
    fHist.SetYTitle("Trig. Eff. [1]");

    MBinning binse;
    binse.SetEdgesLog(10, 1, 100000); // [GeV]
    MH::SetBinning(&fHist, &binse);
}

void MHMcEfficiencyEnergy::SetName(const char *name)
{
    fName = name;
    fHist.SetName(name);
    fHist.SetDirectory(NULL);
}

void MHMcEfficiencyEnergy::SetTitle(const char *title)
{
    fTitle = title;
    fHist.SetTitle(title);
}

// ------------------------------------------------------------------------
// 
// Drawing function. It creates its own canvas.
//
void MHMcEfficiencyEnergy::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->SetLogx();

    fHist.Draw(option);

    pad->Modified();
    pad->Update();
}

void MHMcEfficiencyEnergy::Calc(const TH2D &hsel, const TH2D &hall)
{
    //
    // Set the binning from the two axis of one of the two histograms
    //
    MH::SetBinning(&fHist, ((TH2D&)hsel).GetXaxis());

    //
    // This is necessary to initialize thze error calculation correctly
    // (Nothing important: The histogram set the size of its internal
    // array storing the errors to the correct size)
    //
    fHist.Sumw2();

    //
    // Calculate the efficiency by dividing the number of selected
    // (eg. triggered) showers by the number of all showers per bin.
    // Both histograms are weighted with weight 1, and for the error
    // calculation we assume a binomial error calculation.
    //
    TH1D &tsel = *((TH2D&)hsel).ProjectionX();
    TH1D &tall = *((TH2D&)hall).ProjectionX();
    fHist.Divide(&tsel, &tall, 1, 1);
    delete &tsel;
    delete &tall;

    SetReadyToSave();
}

// --------------------------------------------------------------------------
//
//  Calculate the EfficiencyEnergy and set the 'ReadyToSave' flag.
//  The EfficiencyEnergy is calculated as the number of selected showers
//  (eg. triggered ones) divided by the number of all showers.
//  For error calculation Binomial errors are computed.
//
void MHMcEfficiencyEnergy::Calc(const MHMcEnergyImpact &mcsel, const MHMcEnergyImpact &mcall)
{
    Calc(*mcsel.GetHist(), *mcall.GetHist());
}
