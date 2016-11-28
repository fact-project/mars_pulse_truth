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
!   Author(s): Markus Gaug 02/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHPedestalPix
//
//  A base class for events which are believed to follow a Gaussian distribution 
//  with time, e.g. calibration events, observables containing white noise, ...
//
//  MHPedestalPix derives from MHGausEvents, thus all features of 
//  MHGausEvents can be used by a class deriving from MHPedestalPix
//
//  As an additional feature to MHGausEvents, this class offers to skip the fitting 
//  to set mean, sigma and its errors directly from the histograms with the function 
//  BypassFit()
//
//  See also: MHGausEvents
//
//////////////////////////////////////////////////////////////////////////////
#include "MHPedestalPix.h"

#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHPedestalPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
MHPedestalPix::MHPedestalPix(const char *name, const char *title)
{ 

  fName  = name  ? name  : "MHPedestalPix";
  fTitle = title ? title : "Pedestal histogram events";

}

// -------------------------------------------------------------------------------
//
// Fits the histogram to a double Gauss.
//
//
Bool_t MHPedestalPix::FitDoubleGaus(const Double_t xmin, const Double_t xmax, Option_t *option)
{

  if (IsGausFitOK())
    return kTRUE;

  StripZeros(&fHGausHist,0);
  
  TAxis *axe = fHGausHist.GetXaxis();
  //
  // Get the fitting ranges
  //
  Axis_t rmin = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetFirst()) : xmin;
  Axis_t rmax = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetLast())  : xmax;

  //
  // First guesses for the fit (should be as close to reality as possible, 
  //
  const Stat_t   entries     = fHGausHist.Integral(axe->FindBin(rmin),axe->FindBin(rmax),"width");
  const Double_t sigma_guess = fHGausHist.GetRMS();
  const Double_t area_guess  = entries/TMath::Sqrt(TMath::TwoPi())/sigma_guess;

  fFGausFit = new TF1("GausFit","gaus(0)+gaus(3)",rmin,rmax);

  if (!fFGausFit) 
    {
    *fLog << warn << dbginf << "WARNING: Could not create fit function for Gauss fit " 
          << "in: " << fName << endl;
    return kFALSE;
    }
  
  // 
  // For the fits, we have to take special care since ROOT 
  // has stored the function pointer in a global list which 
  // lead to removing the object twice. We have to take out 
  // the following functions of the global list of functions 
  // as well:
  //
  gROOT->GetListOfFunctions()->Remove(fFGausFit);

  fFGausFit->SetParameters(area_guess/2.,0.,sigma_guess/2.,area_guess/2.,25.,sigma_guess/2.);
  fFGausFit->SetParNames("Area_{0}","#mu_{0}","#sigma_{0}","Area_{1}","#mu_{1}","#sigma_{1}");
  fFGausFit->SetParLimits(0,0.,area_guess*5.);
  fFGausFit->SetParLimits(1,rmin,0.);
  fFGausFit->SetParLimits(2,0.,rmax-rmin);
  fFGausFit->SetParLimits(3,0.,area_guess*10.);
  fFGausFit->SetParLimits(4,0.,rmax/2.);
  fFGausFit->SetParLimits(5,0.,rmax-rmin);
  fFGausFit->SetRange(rmin,rmax);

  fHGausHist.Fit(fFGausFit,option);

  SetMean     (fFGausFit->GetParameter(4)-fFGausFit->GetParameter(1));
  SetSigma    (TMath::Sqrt(fFGausFit->GetParameter(5)*fFGausFit->GetParameter(5)
                           +fFGausFit->GetParameter(2)*fFGausFit->GetParameter(2)));
  SetMeanErr  (TMath::Sqrt(fFGausFit->GetParError(4)*fFGausFit->GetParError(4)
                           +fFGausFit->GetParError(1)*fFGausFit->GetParError(1)));
  SetSigmaErr (TMath::Sqrt(fFGausFit->GetParError(5)*fFGausFit->GetParError(5)
                           +fFGausFit->GetParError(2)*fFGausFit->GetParError(2)));
  SetProb     (fFGausFit->GetProb());
  //
  // The fit result is accepted under condition:
  // 1) The results are not nan's
  // 2) The NDF is not smaller than fNDFLimit (default: fgNDFLimit)
  // 3) The Probability is greater than fProbLimit (default: fgProbLimit)
  //
  // !Finitite means either infinite or not-a-number
  if (   !TMath::Finite(GetMean())
      || !TMath::Finite(GetMeanErr())
      || !TMath::Finite(GetProb())
      || !TMath::Finite(GetSigma())
      || !TMath::Finite(GetSigmaErr())
      || fProb < GetProbLimit())
    return kFALSE;
  
  SetGausFitOK(kTRUE);
  return kTRUE;
}

  
// -------------------------------------------------------------------------------
//
// Fits the histogram to a triple Gauss.
//
Bool_t MHPedestalPix::FitTripleGaus(const Double_t xmin, const Double_t xmax, Option_t *option)
{

  if (IsGausFitOK())
    return kTRUE;

  StripZeros(&fHGausHist,0);
  
  TAxis *axe = fHGausHist.GetXaxis();
  //
  // Get the fitting ranges
  //
  Axis_t rmin = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetFirst()) : xmin;
  Axis_t rmax = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetLast())  : xmax;

  //
  // First guesses for the fit (should be as close to reality as possible, 
  //
  const Stat_t   entries     = fHGausHist.Integral(axe->FindBin(rmin),axe->FindBin(rmax),"width");
  const Double_t sigma_guess = fHGausHist.GetRMS();
  const Double_t area_guess  = entries/TMath::Sqrt(TMath::TwoPi())/sigma_guess;

  fFGausFit = new TF1("GausFit","gaus(0)+gaus(3)+gaus(6)",rmin,rmax);

  if (!fFGausFit) 
    {
    *fLog << warn << dbginf << "WARNING: Could not create fit function for Gauss fit " 
          << "in: " << fName << endl;
    return kFALSE;
    }
  
  // 
  // For the fits, we have to take special care since ROOT 
  // has stored the function pointer in a global list which 
  // lead to removing the object twice. We have to take out 
  // the following functions of the global list of functions 
  // as well:
  //
  gROOT->GetListOfFunctions()->Remove(fFGausFit);

  fFGausFit->SetParameters(10.,-4.0,1.5,70.,1.5,6.,5.,7.,7.);
  fFGausFit->SetParNames("Area_{0}","#mu_{0}","#sigma_{0}","Area_{1}","#mu_{1}","#sigma_{1}","Area_{2}","#mu_{2}","#sigma_{2}");
  fFGausFit->SetParLimits(0,0.,area_guess*2.5);
  fFGausFit->SetParLimits(1,-9.0,-2.2);
  fFGausFit->SetParLimits(2,-1.0,15.);
  fFGausFit->SetParLimits(3,0.,area_guess*10.);
  fFGausFit->SetParLimits(4,-4.5,2.);
  fFGausFit->SetParLimits(5,0.,(rmax-rmin)/3.);
  fFGausFit->SetParLimits(6,0.,area_guess*5.);
  fFGausFit->SetParLimits(7,6.,20.);
  fFGausFit->SetParLimits(8,5.,40.);
  fFGausFit->SetRange(rmin,rmax);

  fHGausHist.Fit(fFGausFit,option);

  SetMean     (fFGausFit->GetParameter(4)-fFGausFit->GetParameter(1));
  SetSigma    (TMath::Sqrt(fFGausFit->GetParameter(5)*fFGausFit->GetParameter(5)
                           +fFGausFit->GetParameter(2)*fFGausFit->GetParameter(2)));
  SetMeanErr  (TMath::Sqrt(fFGausFit->GetParError(4)*fFGausFit->GetParError(4)
                           +fFGausFit->GetParError(1)*fFGausFit->GetParError(1)));
  SetSigmaErr (TMath::Sqrt(fFGausFit->GetParError(5)*fFGausFit->GetParError(5)
                           +fFGausFit->GetParError(2)*fFGausFit->GetParError(2)));
  SetProb     (fFGausFit->GetProb());
  //
  // The fit result is accepted under condition:
  // 1) The results are not nan's
  // 2) The NDF is not smaller than fNDFLimit (default: fgNDFLimit)
  // 3) The Probability is greater than fProbLimit (default: fgProbLimit)
  //
  // !Finitite means either infinite or not-a-number
  if (   !TMath::Finite(GetMean())
      || !TMath::Finite(GetMeanErr())
      || !TMath::Finite(GetProb())
      || !TMath::Finite(GetSigma())
      || !TMath::Finite(GetSigmaErr())
      || fProb < GetProbLimit() )
    return kFALSE;
  
  SetGausFitOK(kTRUE);
  return kTRUE;
}

  
