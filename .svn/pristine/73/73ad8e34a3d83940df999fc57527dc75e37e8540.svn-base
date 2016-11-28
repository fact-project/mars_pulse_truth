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
//  MHCalibrationChargePINDiode
//
//  Histogram class for the charge calibration of the PIN Diode. 
//  Stores and fits the charges, the RMS of the charges  and stores the 
//  location of the maximum FADC slice. Charges are taken from MExtractedSignalPINDiode.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationChargePINDiode.h"

#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MExtractedSignalPINDiode.h"
#include "MCalibrationChargePINDiode.h"

ClassImp(MHCalibrationChargePINDiode);

using namespace std;

const Axis_t  MHCalibrationChargePINDiode::fgAbsTimeFirst    = -0.5;
const Axis_t  MHCalibrationChargePINDiode::fgAbsTimeLast     = 29.5;
const Int_t   MHCalibrationChargePINDiode::fgAbsTimeNbins    = 30;
const Axis_t  MHCalibrationChargePINDiode::fgChargeFirst     = -0.5;
const Axis_t  MHCalibrationChargePINDiode::fgChargeLast      = 1999.5;
const Int_t   MHCalibrationChargePINDiode::fgChargeNbins     = 2000;
const Int_t   MHCalibrationChargePINDiode::fgRmsChargeNbins  = 200;
const Axis_t  MHCalibrationChargePINDiode::fgRmsChargeFirst  = 0.;
const Axis_t  MHCalibrationChargePINDiode::fgRmsChargeLast   = 200.;  
const Float_t MHCalibrationChargePINDiode::fgTimeLowerLimit  = 3.;
const Float_t MHCalibrationChargePINDiode::fgTimeUpperLimit  = 4.;
const TString MHCalibrationChargePINDiode::gsHistName          = "Charge";
const TString MHCalibrationChargePINDiode::gsHistTitle         = "Signals";
const TString MHCalibrationChargePINDiode::gsHistXTitle        = "Signal [FADC counts]";
const TString MHCalibrationChargePINDiode::gsHistYTitle        = "Nr. events";
const TString MHCalibrationChargePINDiode::gsAbsHistName       = "AbsTime";
const TString MHCalibrationChargePINDiode::gsAbsHistTitle      = "Abs. Arr. Times";
const TString MHCalibrationChargePINDiode::gsAbsHistXTitle     = "Time [FADC slices]";
const TString MHCalibrationChargePINDiode::gsAbsHistYTitle     = "Nr. events";

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - the default number for fAbsTimeFirst        (fgAbsTimeFirst)
// - the default number for fAbsTimeLast         (fgAbsTimeLast)
// - the default number for fAbsTimeNbins        (fgAbsTimeNbins)
// - the default number for MHGausEvents::fNbins (fgChargeNbins)
// - the default number for MHGausEvents::fFirst (fgChargeFirst)
// - the default number for MHGausEvents::fLast  (fgChargeLast)
// - the default number for fRmsChargeNbins      (fgRmsChargeNbins)
// - the default number for fRmsChargeFirst      (fgRmsChargeFirst)
// - the default number for fRmsChargeLast       (fgRmsChargeLast)
// - the default number for fTimeLowerLimit      (fgTimeLowerLimit)
// - the default number for fTimeUpperLimit      (fgTimeUpperLimit)
//
// - the default name of the  fHGausHist      ("HCalibrationChargePINDiode")
// - the default title of the fHGausHist      ("Distribution of Summed FADC slices PIN Diode")
// - the default x-axis title for fHGausHist  ("Sum FADC Slices")
// - the default y-axis title for fHGausHist  ("Nr. of events")
// - the default name of the  fHAbsTime       ("HAbsTimePINDiode")
// - the default title of the fHAbsTime       ("Distribution of Absolute Arrival Times PIN Diode")
// - the default x-axis title for fHAbsTime   ("Absolute Arrival Time [FADC slice nr]")
// - the default y-axis title for fHAbsTime   ("Nr. of events")
// - the default name of the  fHRmsCharge     ("HRmsChargePINDiode")
// - the default title of the fHRmsCharge     ("Distribution of Variances of summed FADC slices PIN Diode")
// - the default x-axis title for fHRmsCharge ("RMS (sum) [FADC slices]")
// - the default y-axis title for fHRmsCharge ("Nr. of events")
// - the default directory of the fHRmsCharge (NULL)
// - the current style for fHRmsCharge (NULL)
//
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
// - fAbsHistName   to gsAbsHistName  
// - fAbsHistTitle  to gsAbsHistTitle 
// - fAbsHistXTitle to gsAbsHistXTitle
// - fAbsHistYTitle to gsAbsHistYTitle
//
// Initializes:
// - fHRmsCharge()
// - all pointers to NULL
//
// Calls:
// - Clear()
//
MHCalibrationChargePINDiode::MHCalibrationChargePINDiode(const char *name, const char *title)
    : fPINDiode(NULL), fSigPIN(NULL), fHRmsCharge()
{

  fName  = name  ? name  : "MHCalibrationChargePINDiode";
  fTitle = title ? title : "Fill the FADC sums of the PINDiode events and perform the fits";

  SetBinning(fgChargeNbins, fgChargeFirst, fgChargeLast);
  SetBinningAbsTime(fgAbsTimeNbins, fgAbsTimeFirst, fgAbsTimeLast);
  SetBinningRmsCharge(fgRmsChargeNbins, fgRmsChargeFirst, fgRmsChargeLast);

  SetTimeLowerLimit();
  SetTimeUpperLimit();

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetAbsHistName  (gsAbsHistName  .Data());
  SetAbsHistTitle (gsAbsHistTitle .Data());
  SetAbsHistXTitle(gsAbsHistXTitle.Data());
  SetAbsHistYTitle(gsAbsHistYTitle.Data());

  fHRmsCharge.SetName("HRmsChargePINDiode");
  fHRmsCharge.SetTitle("Distribution of Variances of summed FADC slices PIN Diode");  
  fHRmsCharge.SetXTitle("RMS (sum) [FADC slices]");
  fHRmsCharge.SetYTitle("Nr. of events");
  fHRmsCharge.UseCurrentStyle();
  fHRmsCharge.SetDirectory(NULL); 

  Clear();
}

// --------------------------------------------------------------------------
//
// Initializes Binning of the following histograms:
// - fHGausHist.SetBins(fNbins,fFirst,fLast);
// - fHAbsTime.SetBins(fAbsTimeNbins,fAbsTimeFirst,fAbsTimeLast);
// - fHRmsCharge.SetBins(fRmsChargeNbins,fRmsChargeFirst,fRmsChargeLast);
//
Bool_t MHCalibrationChargePINDiode::SetupFill(const MParList *pList) 
{

  TH1F *h = GetHGausHist();

  h->SetName (fHistName.Data());
  h->SetTitle(fHistTitle.Data());  
  h->SetXTitle(fHistXTitle.Data());
  h->SetYTitle(fHistYTitle.Data());
  
  h = GetHAbsTime();

  h->SetName (fAbsHistName.Data());
  h->SetTitle(fAbsHistTitle.Data());
  h->SetXTitle(fAbsHistXTitle.Data());
  h->SetYTitle(fAbsHistYTitle.Data());
  
  MHCalibrationPix::InitBins();

  fHAbsTime.  SetBins(fAbsTimeNbins,  fAbsTimeFirst,  fAbsTimeLast);
  fHRmsCharge.SetBins(fRmsChargeNbins,fRmsChargeFirst,fRmsChargeLast);

  fExclusionMean  = 0;
  fExclusionSigma = 0;  
  fExclusionChi2  = 0;

  return kTRUE;

}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MExtractedSignalPINDiode
// - MCalibrationChargePINDiode
//
Bool_t MHCalibrationChargePINDiode::ReInit(MParList *pList)
{
  
  fSigPIN = (MExtractedSignalPINDiode*)pList->FindCreateObj("MExtractedSignalPINDiode");
  if (!fSigPIN)
      return kFALSE;

  fPINDiode = (MCalibrationChargePINDiode*)pList->FindCreateObj("MCalibrationChargePINDiode");
  if (!fPINDiode)
      return kFALSE;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Retrieves from MExtractedSignalPINDiode:
// - Number of used FADC samples via MExtractedSignalPINDiode::GetNumFADCSamples()
// - Extracted signal via            MExtractedSignalPINDiode::GetExtractedSignal()
// - Signal Rms                      MExtractedSignalPINDiode::GetExtractedRms()
// - Arrival Time                    MExtractedSignalPINDiode::GetExtractedTime()
//
// Fills the following histograms:
// - MHGausEvents::FillHistAndArray(signal)
// - MHCalibrationChargePix::FillAbsTime(time);
// - FillRmsCharge(rms);
//
Int_t MHCalibrationChargePINDiode::Fill(const MParContainer *par, const Stat_t w)
{

  const MExtractedSignalPINDiode *extractor = dynamic_cast<const MExtractedSignalPINDiode*>(par);
  if (!extractor)
    {
      *fLog << err << "No argument in MExtractedSignalPINDiode::Fill... abort." << endl;
      return kERROR;
    }
  
  const Float_t signal = (float)extractor->GetExtractedSignal();
  const Float_t time   = extractor->GetExtractedTime();
  const Float_t sigma  = extractor->GetExtractedSigma();
  const Float_t chi2   = extractor->GetExtractedChi2();

  if (time < 3. || time > 24.)
    {
      fExclusionMean++;
      return kTRUE;
    }
  
  if (sigma < 5. || sigma > 18.)
    {
      fExclusionSigma++;
      return kTRUE;
    }
  
  if (chi2  > 0.35)
    {
      fExclusionChi2++;
      return kTRUE;
    }
  
  FillHistAndArray(signal);
  FillAbsTime(time);
  FillRmsCharge(sigma);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns kTRUE, if empty
//
// Performs the following fits:
// - MHGausEvents::FitGaus()
// - FitRmsCharge()
//
// Creates the fourier spectrum (MHGausEvents::CreateFourierSpectrum() 
// and sets bit MCalibrationChargePINDiode::SetOscillating( MHGausEvents::IsFourierSpectrumOK() )
// Retrieves the results of the following fits and stores them in MCalibrationChargePINDiode:
// - Mean Charge and Error 
// - Sigma Charge and Error
// - Fit Probability
// - Abs Time Mean
// - Abs Time Rms
// - Rms Charge Mean and Error
// - Rms Charge Sigma and Error
// 
// Performs one consistency check on the arrival time: 
// The check returns kFALSE if:
//
// -The mean arrival time is in fTimeLowerLimit slices from the lower edge 
//  and fUpperLimit slices from the upper edge
//
Bool_t MHCalibrationChargePINDiode::Finalize() 
{
  
  if (IsGausFitOK() || IsEmpty())
    return kTRUE;
    
  FitGaus();
  FitRmsCharge();

  fPINDiode->SetMean     (  fMean     );
  fPINDiode->SetMeanVar  (  fMeanErr  * fMeanErr );
  fPINDiode->SetSigma    (  fSigma    );
  fPINDiode->SetSigmaVar (  fSigmaErr * fMeanErr );
  fPINDiode->SetProb     (  fProb     );

  fPINDiode->SetAbsTimeMean(    GetAbsTimeMean() );
  fPINDiode->SetAbsTimeRms(     GetAbsTimeRms()  );

  fPINDiode->SetRmsChargeMean(     GetRmsChargeMean()        );
  fPINDiode->SetRmsChargeMeanErr(  GetRmsChargeMeanErr()     );
  fPINDiode->SetRmsChargeSigma(    GetRmsChargeSigma()       );
  fPINDiode->SetRmsChargeSigmaErr( GetRmsChargeSigmaErr()    );

  fPINDiode->SetValid(kTRUE);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fills fHRmsCharge with q
// Returns kFALSE, if overflow or underflow occurred, else kTRUE
//
Bool_t MHCalibrationChargePINDiode::FillRmsCharge(const Float_t q)
{
  return fHRmsCharge.Fill(q) > -1;
}

// -----------------------------------------------------------
//
// Fits -- not yet implemented
//
Bool_t MHCalibrationChargePINDiode::FitRmsCharge(Option_t *option)
{
  return 1;
}


// -------------------------------------------------------------------------
//
// Draw the histogram
//
// The following options can be chosen:
//
// "": displays the fHGausHist with fits and fHRmsCharge
// "all": executes additionally MHCalibrationPix::Draw(), with option "fourierevents"
//
void MHCalibrationChargePINDiode::Draw(const Option_t *opt)
{

  TString option(opt);
  option.ToLower();
  
//  Int_t win = 1;

  TVirtualPad *oldpad = gPad ? gPad : MH::MakeDefCanvas(this,900, 600);
  TVirtualPad *pad    = NULL;

  oldpad->SetBorderMode(0);
    
  if (option.Contains("all"))
  {
      option.ReplaceAll("all","");
      oldpad->Divide(2,1);
//      win = 2;
      oldpad->cd(1);
      TVirtualPad *newpad = gPad;
      pad = newpad;
      pad->Divide(1,2);
      pad->cd(1);
  }
  else
  {
      pad = oldpad;
      pad->Divide(1,2);
      pad->cd(1);
  }

  if (IsEmpty())
    return;

  if (!IsOnlyOverflow() && !IsOnlyUnderflow())
    gPad->SetLogy();

  gPad->SetTicks();

  fHGausHist.Draw(opt); 
  if (fFGausFit)
  {
      fFGausFit->SetLineColor(IsGausFitOK() ? kGreen : kRed);
      fFGausFit->Draw("same");
  }

  pad->cd(2);
  fHRmsCharge.Draw(opt);

  oldpad->cd(2);
  MHCalibrationPix::Draw("fourierevents");
}




