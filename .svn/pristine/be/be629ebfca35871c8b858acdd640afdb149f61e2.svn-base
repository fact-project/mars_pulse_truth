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
//  MHMcEnergy
//
// This class holds the information (histogram and fit function)
// about the energy threshold for a particular trigger condition.
//
////////////////////////////////////////////////////////////////////////////
#include "MHMcEnergy.h" 

#include <stdlib.h>
#include <iostream>

#include <TH1.h> 
#include <TF1.h> 
#include <TCanvas.h>
#include <TPaveLabel.h> 

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHMcEnergy);

using namespace std;

// -------------------------------------------------------------------------
//
//  Default Constructor.
//
MHMcEnergy::MHMcEnergy(const char *name, const char *title)
{ 
    fTitle = title ? title : "Container for an energy distribution histogram";

    //  - we initialize the histogram
    //  - we have to give diferent names for the diferent histograms because
    //    root don't allow us to have diferent histograms with the same name

    fHist = new TH1F("", "", 20, 0.5, 4.5);

    fHist->SetDirectory(NULL);
    fHist->SetXTitle("log(E/GeV)");
    fHist->SetYTitle("dN/dE");

    SetName(name ? name : "MHMcEnergy");
}

// -------------------------------------------------------------------------
//
//  This doesn't only set the name. It tries to get the number from the
//  name and creates also name and title of the histogram.
//
//  This is necessary for example if a list of such MHMcEnergy histograms
//  is created (s. MParList::CreateObjList)
//
void MHMcEnergy::SetName(const char *name)
{
    TString cname(name);
    const char *semicolon = strrchr(cname, ';');

    UInt_t idx = semicolon ? atoi(semicolon+1) : 0;

    fName = cname;

    char text[256];
    if (idx>0)
        sprintf(text, "Energy Distribution for trigger condition #%i", idx);
    else
        sprintf(text, "Energy Distribution");

    char aux[256];
    strcpy(aux, "Threshold");

    if (idx>0)
        sprintf(aux+strlen(aux), " #%i", idx);

    fHist->SetName(aux);
    fHist->SetTitle(text);
}

//-------------------------------------------------------------------------
//
//  Defualt Destructor
//
MHMcEnergy::~MHMcEnergy()
{
    delete fHist;
}

//--------------------------------------------------------------------------
//
//  Fill the histogram with the log10 of the energy for triggered events.
//
void MHMcEnergy::Fill(Float_t log10E, Float_t w)
{
    fHist->Fill(log10E, w);
}

// -------------------------------------------------------------------------
//
// Fitting function
//
void MHMcEnergy::Fit(Axis_t xxmin, Axis_t xxmax)
{
    //
    // 0: don't draw the function (it is drawn together with the histogram)
    // Q: quiet mode
    //
    fHist->Fit("gaus", "Q0", "", xxmin, xxmax);

    TF1 *result = fHist->GetFunction("gaus");

    fThreshold    = CalcThreshold(result);
    fThresholdErr = CalcThresholdErr(result);
    fGaussPeak    = CalcGaussPeak(result);
    fGaussSigma   = CalcGaussSigma(result);
}

// ------------------------------------------------------------------------
// 
//  Helper function for Draw() and DrawClone() which adds some useful
//  information to the plot.
//
void MHMcEnergy::DrawLegend() const
{
    char text[256];

    const Float_t min = fHist->GetMinimum();
    const Float_t max = fHist->GetMaximum();
    const Float_t sum = min+max;

    sprintf(text, "Energy Threshold = %4.1f +- %4.1f GeV",
            fThreshold, fThresholdErr);

    TPaveLabel* label = new TPaveLabel(2.2, 0.75*sum, 4.4, 0.90*sum, text);
    label->SetFillColor(10);
    label->SetTextSize(0.3);
    label->SetBit(kCanDelete);
    label->Draw();
}

// ------------------------------------------------------------------------
// 
// Drawing function. It creates its own canvas.
//
void MHMcEnergy::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    fHist->Draw(option);

    DrawLegend();

    pad->Modified();
    pad->Update();
}

// --------------------------------------------------------------------------
//
// Set the number of bins in the histogran.
//
void MHMcEnergy::SetNumBins(Int_t nbins)
{
    fHist->SetBins(nbins, 0.5, 4.5);
}
// --------------------------------------------------------------------------
//
// Write the threshold and its error in the standard output
//
void MHMcEnergy::Print(Option_t*) const
{
    *fLog << all << "Threshold: " << fThreshold << " +- " << fThresholdErr << endl;
}

// -------------------------------------------------------------------------
//
//  Return the threshold
//
Float_t MHMcEnergy::CalcThreshold(TF1 *gauss)
{
    const Float_t p1 = gauss->GetParameter(1);

    return pow(10, p1);
}

// -------------------------------------------------------------------------
//
// Return the error of the threshold.
//
Float_t MHMcEnergy::CalcThresholdErr(TF1 *gauss)
{
    const Float_t lg10  = log(10.);
    const Float_t p1    = gauss->GetParameter(1);
    const Float_t p1err = gauss->GetParError(1);

    // The error has into accuont the error in the fit
    return pow(10, p1) * p1err * lg10;
}

// -------------------------------------------------------------------------
//
// Return the peak of the fitted gaussan function.
//
Float_t MHMcEnergy::CalcGaussPeak(TF1 *gauss)
{
    return gauss->GetParameter(1);
}

// -------------------------------------------------------------------------
//
// Return the sigma of the fitted gaussan function.
//
Float_t MHMcEnergy::CalcGaussSigma(TF1 *gauss)
{
    return gauss->GetParameter(2);
}

TH1 *MHMcEnergy::GetHistByName(const TString name)
{
    return fHist;
}
