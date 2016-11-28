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
//  MHMcDifRate
//
// This class holds the information (histogram and fit function)
// about the energy threshold for a particular trigger condition.
//
////////////////////////////////////////////////////////////////////////////
#include "MHMcDifRate.h" 

#include <math.h>

#include <TCanvas.h>

#include "MH.h"
#include "MBinning.h"

#include "MHMcCollectionArea.h"

ClassImp(MHMcDifRate);

// -------------------------------------------------------------------------
//
//  Default Constructor.
//
MHMcDifRate::MHMcDifRate(const char *name, const char *title)
    : fHist()
{
    fName  = name  ? name  : "MHMcDifRate";
    fTitle = title ? title : "Differential Trigger Rate";

    //  - we initialize the histogram
    //  - we have to give diferent names for the diferent histograms because
    //    root don't allow us to have diferent histograms with the same name

    fHist.SetName(fName);
    fHist.SetTitle(fTitle);


    fHist.SetDirectory(NULL);

    fHist.SetXTitle("E [GeV]");
    fHist.SetYTitle("dR/dE [Hz/GeV]");
}

void MHMcDifRate::SetName(const char *name)
{
    fName = name;
    fHist.SetName(name);
    fHist.SetDirectory(NULL);
}

void MHMcDifRate::SetTitle(const char *title)
{
    fTitle = title;
    fHist.SetTitle(title);
}

// ------------------------------------------------------------------------
// 
// Drawing function. It creates its own canvas.
//
void MHMcDifRate::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->SetLogx();

    fHist.Draw(option);

    pad->Modified();
    pad->Update();
}
/*
void MHMcDifRate::Calc(const TH2D &hsel, const TH2D &hall)
{
    //
    // Set the binning from the two axis of one of the two histograms
    //
    MH::SetBinning(&fHist, ((TH2D&)hsel).GetXaxis(), ((TH2D&)hsel).GetYaxis());

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
    fHist.Divide((TH2D*)&hsel, (TH2D*)&hall, 1, 1, "B");

    SetReadyToSave();
}
*/

// --------------------------------------------------------------------------
//
//  Calculate the DifRate and set the 'ReadyToSave' flag.
//  The DifRate is calculated as the number of selected showers
//  (eg. triggered ones) divided by the number of all showers.
//  For error calculation Binomial errors are computed.
//
void MHMcDifRate::Calc(const MHMcCollectionArea &cola, const TF1 &spect)
{
    /*const*/ TH1D &hcol = (TH1D&)*cola.GetHist();

    fHist.Reset();

    //
    // Set the binning from the two axis of one of the two histograms
    //
    MH::SetBinning(&fHist, hcol.GetXaxis());

    //
    // This is necessary to initialize thze error calculation correctly
    // (Nothing important: The histogram set the size of its internal
    // array storing the errors to the correct size)
    //
    fHist.Sumw2();

    fHist.Add(&hcol);
    fHist.Multiply((TF1*)&spect);

    SetReadyToSave();
}
