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
!   Author(s): Markus Gaug 06/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MHCalibrationHiLoPix
//
//  Histogram class for the charge calibration. 
//  Stores and fits the charges and stores the location of the maximum FADC 
//  slice. HiLos are taken from MExtractedSignalPix.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationHiLoPix.h"

#include <TF1.h>

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>
#include <TStyle.h>

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHCalibrationHiLoPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - the default x-axis title for fHivsLo ("High-gain Charge [FADC slices]")
// - the default y-axis title for fHivsLo ("Low-gain Charge [FADC slices]"); 
// - the default directory of the fHivsLo (NULL)
// - the current style for fHivsLo
//
// Initializes:
// - fHivsLo()
//
// Calls:
// - Clear();
//
MHCalibrationHiLoPix::MHCalibrationHiLoPix(const char *name, const char *title)
    : fHivsLo(), 
      fHivsLoNbins(1), fHivsLoFirst(0.), fHivsLoLast(1.)
{ 
  
  fName  = name  ? name  : "MHCalibrationHiLoPix";
  fTitle = title ? title : "High-gain vs. Low-gain intercalibration";

  fHivsLo.UseCurrentStyle();
  fHivsLo.SetDirectory(NULL); 

  Clear();
  
}

// --------------------------------------------------------------------------
//
// Sets:
// - fHGausHist.SetBins(fNbins,fFirst,fLast);
// - fHivsLo.SetBins(fHivsLoNbins,fHivsLoFirst,fHivsLoLast);
//
void MHCalibrationHiLoPix::InitBins()
{
  MHGausEvents::InitBins();
  fHivsLo.SetBins(fHivsLoNbins,fHivsLoFirst,fHivsLoLast);
}

// --------------------------------------------------------------------------
//
// Empty function to overload MHGausEvents::Reset()
//
void MHCalibrationHiLoPix::Reset()
{
  MHGausEvents::Reset();
  fHivsLo.Reset();
}

// --------------------------------------------------------------------------
//
// returns probability of fit. If fit does not exist, 
// returns -1. 
//
const Float_t MHCalibrationHiLoPix::GetHivsLoProb() const 
{ 
  const TF1 *fit = fHivsLo.GetFunction("pol1");
  return fit ? fit->GetProb() : -1.;  
}

// --------------------------------------------------------------------------
//
// returns parameter 0 of fit. If fit does not exist,
// returns -1.
//
const Float_t MHCalibrationHiLoPix::GetHivsLoP0() const 
{
  const TF1 *fit = fHivsLo.GetFunction("pol1");
  return fit ? fit->GetParameter(0) : -1.;  
}

// --------------------------------------------------------------------------
//
// returns parameter 1 of fit. If fit does not exist,
// returns -1.
//
const Float_t MHCalibrationHiLoPix::GetHivsLoP1() const 
{
  const TF1 *fit = fHivsLo.GetFunction("pol1");
  return fit ? fit->GetParameter(1) : -1.;  
}

// --------------------------------------------------------------------------
//
// Fills fHivsLo with t
// Returns kFALSE, if overflow or underflow occurred, else kTRUE
//
Bool_t MHCalibrationHiLoPix::FillHivsLo(Float_t t,Float_t w)
{
  return fHivsLo.Fill(t,w) > -1;
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// The following options can be chosen:
//
// "": displays the fHGausHist and the fHivsLo
// "all": executes additionally MHCalibrationPix::Draw(), with options
//
// The following picture shows a typical outcome of call to Draw("all"): 
// One the left side:
// - The distribution of the values with the Gauss fit are shown (points connected 
//   with each other). The line is green, thus the Gauss fit has a probability higher 
//   than 0.5%.
// - The distribution of the positions of the maximum (abs. arrival times) 
//   is displayed. Most of the events have an arrival time of slice 7 (==hardware:8)
//
// On the right side:
// - The first plot shows the distribution of the values with the Gauss fit
//   with error bars
// - The second plot shows the TGraph with the events vs. time
// - The third plot shows the fourier transform and a peak at 100 Hz.
// - The fourth plot shows the projection of the fourier components and an exponential 
//   fit, with the result that the observed deviation is not statistical, but signficant with a 
//   probability smaller than 0.5%. 
//
//Begin_Html
/*
<img src="images/MHCalibrationHiLoPixDraw.gif">
*/
//End_Html
//
void MHCalibrationHiLoPix::Draw(const Option_t *opt)
{

  TString option(opt);
  option.ToLower();
  
  Int_t win = 1;
  
  TVirtualPad *oldpad = gPad ? gPad : MH::MakeDefCanvas(this,600, 600);
  TVirtualPad *pad    = NULL;

  if (option.Contains("all"))
  {
      option.ReplaceAll("all","");
      oldpad->Divide(2,1);
      win = 2;
      oldpad->cd(1);
      TVirtualPad *newpad = gPad;
      pad = newpad;
      pad->Divide(1,2);
      pad->cd(1);
  }
  else if (option.Contains("datacheck"))
    {
      MHCalibrationPix::Draw("events");
      return;
    }
  else 
  {
      pad = oldpad;
      pad->Divide(1,2);
      pad->cd(1);
  }
  /*
  else
    {
      option.ReplaceAll("time","");
      pad = oldpad;
      pad->Divide(1,2);
      pad->cd(1);
    }
  */
  if (!IsEmpty() && !IsOnlyOverflow() && !IsOnlyUnderflow())
    gPad->SetLogy();

  gPad->SetTicks();

  fHGausHist.GetXaxis()->SetLabelSize(0.06);
  fHGausHist.GetYaxis()->SetLabelSize(0.07);
  fHGausHist.GetXaxis()->SetLabelOffset(0.01);
  fHGausHist.GetYaxis()->SetLabelOffset(0.01);
  fHGausHist.GetXaxis()->SetTitleSize(0.065);
  fHGausHist.GetYaxis()->SetTitleSize(0.07);
  fHGausHist.GetXaxis()->SetTitleOffset(0.6);
  fHGausHist.GetYaxis()->SetTitleOffset(0.6);
  fHGausHist.Draw(); 
  if (fFGausFit)
  {
      fFGausFit->SetLineColor(IsGausFitOK() ? kGreen : kRed);
      fFGausFit->Draw("same");
  }

  pad->cd(2);
  gPad->SetTicks();
  gStyle->SetOptFit();

  fHivsLo.GetXaxis()->SetLabelSize(0.06);
  fHivsLo.GetYaxis()->SetLabelSize(0.07);
  fHivsLo.GetXaxis()->SetLabelOffset(0.01);
  fHivsLo.GetYaxis()->SetLabelOffset(0.01);
  fHivsLo.GetXaxis()->SetTitleSize(0.065);
  fHivsLo.GetYaxis()->SetTitleSize(0.07);
  fHivsLo.GetXaxis()->SetTitleOffset(0.6);
  fHivsLo.GetYaxis()->SetTitleOffset(0.6);
  fHivsLo.Draw();

  if (win < 2)
      return;

  oldpad->cd(2);
  MHCalibrationPix::Draw("fourierevents");
}
