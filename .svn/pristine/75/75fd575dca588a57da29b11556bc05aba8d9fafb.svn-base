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
//  MHMcIntRate
//
// This class holds the information (histogram and fit function)
// about the energy threshold for a particular trigger condition.
//
////////////////////////////////////////////////////////////////////////////
#include "MHMcIntRate.h" 

#include <math.h>

#include <TCanvas.h>

#include "MH.h"
#include "MBinning.h"

#include "MHMcDifRate.h"

ClassImp(MHMcIntRate);

using namespace std;

// -------------------------------------------------------------------------
//
//  Default Constructor.
//
MHMcIntRate::MHMcIntRate(const char *name, const char *title)
    : fHist()
{
    fName  = name  ? name  : "MHMcIntRate";
    fTitle = title ? title : "Integral Trigger Rate";

    //  - we initialize the histogram
    //  - we have to give diferent names for the diferent histograms because
    //    root don't allow us to have diferent histograms with the same name

    fHist.SetName(fName);
    fHist.SetTitle(fTitle);

    fHist.SetDirectory(NULL);

    fHist.SetXTitle("E [GeV]");
    fHist.SetYTitle("Rate [Hz]");
}

void MHMcIntRate::SetName(const char *name)
{
    fName = name;
    fHist.SetName(name);
    fHist.SetDirectory(NULL);
}

void MHMcIntRate::SetTitle(const char *title)
{
    fTitle = title;
    fHist.SetTitle(title);
}

// ------------------------------------------------------------------------
// 
// Drawing function. It creates its own canvas.
//
void MHMcIntRate::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->SetLogx();

    fHist.Draw(option);

    pad->Modified();
    pad->Update();
}

// --------------------------------------------------------------------------
//
//  Calculate the IntRate and set the 'ReadyToSave' flag.
//  The IntRate is calculated as the number of selected showers
//  (eg. triggered ones) divided by the number of all showers.
//  For error calculation Binomial errors are computed.
//
void MHMcIntRate::Calc(const MHMcDifRate &rate)
{
    /*const*/ TH1D  &hist = (TH1D&)*rate.GetHist();
    const TAxis &axis = *hist.GetXaxis();

    //
    // Set the binning from the two axis of one of the two histograms
    //
    MH::SetBinning(&fHist, &axis);

    const Int_t nbinsx = axis.GetNbins();

    for (Int_t i=1; i<=nbinsx; i++)
    {
        fHist.SetBinContent(i, hist.Integral(i, nbinsx, "width"));
        fHist.SetBinError(i, hist.GetBinError(i)*axis.GetBinWidth(i));
    }

    fHist.SetEntries(hist.GetEntries());

    SetReadyToSave();
}
