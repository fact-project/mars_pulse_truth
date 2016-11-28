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
!   Author(s): Markus Gaug 02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MHCalibrationChargePix
//
//  Histogram class for the charge calibration. 
//  Stores and fits the charges and stores the location of the maximum FADC 
//  slice. Charges are taken from MExtractedSignalPix.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationChargePix.h"

#include <TH1.h>
#include <TF1.h>

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHCalibrationChargePix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - the default x-axis title for fHAbsTime ("Absolute Arrival Time [FADC slice nr]")
// - the default y-axis title for fHAbsTime ("Nr. of events"); 
// - the default directory of the fHAbsTime (NULL)
// - the current style for fHAbsTime
//
// Initializes:
// - fHAbsTime()
//
// Calls:
// - Clear();
//
MHCalibrationChargePix::MHCalibrationChargePix(const char *name, const char *title)
    : fHAbsTime(), 
      fAbsTimeNbins(1), fAbsTimeFirst(0.), fAbsTimeLast(1.)
{ 
  
  fName  = name  ? name  : "MHCalibrationChargePix";
  fTitle = title ? title : "Statistics of the FADC sums of calibration events";

  fHAbsTime.UseCurrentStyle();
  fHAbsTime.SetDirectory(NULL); 

  Clear();
  
}

// --------------------------------------------------------------------------
//
// Sets:
// - fHGausHist.SetBins(fNbins,fFirst,fLast);
// - fHAbsTime.SetBins(fAbsTimeNbins,fAbsTimeFirst,fAbsTimeLast);
//
void MHCalibrationChargePix::InitBins()
{
  MHGausEvents::InitBins();
  fHAbsTime.SetBins(fAbsTimeNbins,fAbsTimeFirst,fAbsTimeLast);
}

// --------------------------------------------------------------------------
//
// Empty function to overload MHGausEvents::Reset()
//
void MHCalibrationChargePix::Reset()
{
  MHGausEvents::Reset();
  fHAbsTime.Reset();
}

// --------------------------------------------------------------------------
//
// returns fHGausHist.Integral("width")
//
const Float_t MHCalibrationChargePix::GetIntegral() const 
{ 
   return fHGausHist.Integral("width");  
}

// --------------------------------------------------------------------------
//
// returns fHAbsTime.GetMean()
//
const Float_t MHCalibrationChargePix::GetAbsTimeMean() const 
{
  return fHAbsTime.GetMean();
}

// --------------------------------------------------------------------------
//
// returns fHAbsTime.GetRMS()
//
const Float_t MHCalibrationChargePix::GetAbsTimeRms()  const 
{
  return fHAbsTime.GetRMS();
}

// --------------------------------------------------------------------------
//
// Fills fHAbsTime with t
// Returns kFALSE, if overflow or underflow occurred, else kTRUE
//
Bool_t MHCalibrationChargePix::FillAbsTime(Float_t t)
{
  return fHAbsTime.Fill(t) > -1;
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// The following options can be chosen:
//
// "": displays the fHGausHist and the fHAbsTime
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
<img src="images/MHCalibrationChargePixDraw.gif">
*/
//End_Html
//
void MHCalibrationChargePix::Draw(const Option_t *opt)
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

  fHAbsTime.GetXaxis()->SetLabelSize(0.06);
  fHAbsTime.GetYaxis()->SetLabelSize(0.07);
  fHAbsTime.GetXaxis()->SetLabelOffset(0.01);
  fHAbsTime.GetYaxis()->SetLabelOffset(0.01);
  fHAbsTime.GetXaxis()->SetTitleSize(0.065);
  fHAbsTime.GetYaxis()->SetTitleSize(0.07);
  fHAbsTime.GetXaxis()->SetTitleOffset(0.6);
  fHAbsTime.GetYaxis()->SetTitleOffset(0.6);
  fHAbsTime.Draw();

  if (win < 2)
      return;

  oldpad->cd(2);
  MHCalibrationPix::Draw("fourierevents");

}
