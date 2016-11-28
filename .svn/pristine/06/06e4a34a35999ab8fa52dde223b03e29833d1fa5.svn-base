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
//  MHCalibrationPix
//
//  A base class for events which are believed to follow a Gaussian distribution 
//  with time, e.g. calibration events, observables containing white noise, ...
//
//  MHCalibrationPix derives from MHGausEvents, thus all features of 
//  MHGausEvents can be used by a class deriving from MHCalibrationPix
//
//  As an additional feature to MHGausEvents, this class offers to skip the fitting 
//  to set mean, sigma and its errors directly from the histograms with the function 
//  BypassFit()
//
//  See also: MHGausEvents
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationPix.h"

#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHCalibrationPix);

using namespace std;

const Float_t  MHCalibrationPix::fgBlackoutLimit        = 5.;
const Float_t  MHCalibrationPix::fgPickupLimit          = 5.;
// --------------------------------------------------------------------------
//
// Default Constructor. 
// Sets: 
// - the default number for fPickupLimit           (fgPickupLimit)
// - the default number for fBlackoutLimit         (fgBlackoutLimit)
//
// Initializes:
// - all variables to 0.
//
MHCalibrationPix::MHCalibrationPix(const char *name, const char *title)
{ 

  fName  = name  ? name  : "MHCalibrationPix";
  fTitle = title ? title : "Calibration histogram events";

  Clear();
  
  SetBlackoutLimit();
  SetPickupLimit();
}

// --------------------------------------------------------------------------
//
// Default Clear(), can be overloaded.
//
// Sets:
// - all other pointers to NULL
// - all variables to 0., except fPixId to -1 
// - all flags to kFALSE
// 
// - all pointers
//
void MHCalibrationPix::Clear(Option_t *o)
{

  MHGausEvents::Clear();
  fSaturated         = 0;
}

void MHCalibrationPix::Reset()
{
  
  MHGausEvents::Reset();
  fSaturated = 0;
}


// -----------------------------------------------------------------------------
// 
// Bypasses the Gauss fit by taking mean and RMS from the histogram
//
// Errors are determined in the following way:
// MeanErr  = RMS / Sqrt(entries)
// SigmaErr = RMS / (2.*Sqrt(entries) )
//
void MHCalibrationPix::BypassFit()
{

  const Stat_t entries = fHGausHist.GetEntries();
  
  if (entries <= 0.)
    {
      *fLog << warn << GetDescriptor() 
            << ": Cannot bypass fit. Number of entries smaller or equal 0 in pixel: " << GetName() << endl;
      return;
    }
  
  fMean     = fHGausHist.GetMean();
  fMeanErr  = fHGausHist.GetRMS() / TMath::Sqrt(entries);
  fSigma    = fHGausHist.GetRMS() ;
  fSigmaErr = fHGausHist.GetRMS() / TMath::Sqrt(entries) / 2.;
}

// -------------------------------------------------------------------------------
//
// Return the number of "blackout" events, which are events with values higher 
// than fBlackoutLimit sigmas from the mean
//
//
const Double_t MHCalibrationPix::GetBlackout() const 
{
  
  if ((fMean == 0.) && (fSigma == 0.))
    return -1.;

  const Int_t first = fHGausHist.GetXaxis()->GetFirst();
  const Int_t last  = fHGausHist.GetXaxis()->FindBin(fMean-fBlackoutLimit*fSigma);

  if (first >= last)
    return 0.;
  
  return fHGausHist.Integral(first, last);
}


// -------------------------------------------------------------------------------
//
// Return the number of "pickup" events, which are events with values higher 
// than fPickupLimit sigmas from the mean
//
//
const Double_t MHCalibrationPix::GetPickup() const 
{
  if (!IsValid())
    return -1.;

  const Int_t first = fHGausHist.GetXaxis()->FindBin(fMean+fPickupLimit*fSigma);
  const Int_t last  = fHGausHist.GetXaxis()->GetLast();

  if (first >= last)
    return 0.;
  
  return fHGausHist.Integral(first, last);
}

// -----------------------------------------------------------------------------
//
// If flag IsGausFitOK() is set (histogram already successfully fitted), 
// returns kTRUE
// 
// If both fMean and fSigma are still zero, call FitGaus() 
// 
// Repeats the Gauss fit in a smaller range, defined by: 
// 
// min = GetMean() - fBlackoutLimit * GetSigma();
// max = GetMean() + fPickupLimit   * GetSigma();
//
// The fit results are retrieved and stored in class-own variables.  
//
// A flag IsGausFitOK() is set according to whether the fit probability 
// is smaller or bigger than fProbLimit, whether the NDF is bigger than 
// fNDFLimit and whether results are NaNs.
//
Bool_t MHCalibrationPix::RepeatFit(const Option_t *option)
{

  if (IsGausFitOK())
    return kTRUE;

  if (!IsValid())
    return FitGaus();

  //
  // Get new fitting ranges
  //
  Axis_t rmin = GetMean() - fBlackoutLimit * GetSigma();
  Axis_t rmax = GetMean() + fPickupLimit   * GetSigma();

  Axis_t hmin = fHGausHist.GetBinCenter(fHGausHist.GetXaxis()->GetFirst());
  Axis_t hmax = fHGausHist.GetBinCenter(fHGausHist.GetXaxis()->GetLast()) ;

  fFGausFit->SetRange(hmin < rmin ? rmin : hmin , hmax > rmax ? rmax : hmax);

  fHGausHist.Fit(fFGausFit,option);

  fMean     = fFGausFit->GetParameter(1);
  fSigma    = fFGausFit->GetParameter(2);
  fMeanErr  = fFGausFit->GetParError(1) ; 
  fSigmaErr = fFGausFit->GetParError(2) ; 
  fProb     = fFGausFit->GetProb()      ;      

  //
  // The fit result is accepted under condition:
  // 1) The results are not nan's
  // 2) The NDF is not smaller than fNDFLimit (default: fgNDFLimit)
  // 3) The Probability is greater than fProbLimit (default: fgProbLimit)
  //
  if (   !TMath::Finite( fMean     )
      || !TMath::Finite( fMeanErr  )
      || !TMath::Finite( fProb     )
      || !TMath::Finite( fSigma    )
      || !TMath::Finite( fSigmaErr )
      || fFGausFit->GetNDF() < GetNDFLimit()
      || fProb < GetProbLimit() )
    return kFALSE;
  
  SetGausFitOK(kTRUE);
  return kTRUE;

}

