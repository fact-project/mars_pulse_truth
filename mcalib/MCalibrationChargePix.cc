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
!   Author(s): Markus Gaug   02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         
// MCalibrationChargePix                                                   
//                                                                         
// Storage container of the calibrated Charge of one pixel.
//                                                                         
// The following values are initialized to meaningful values:
//
// - The Electronic Rms to 1.5 per FADC slice
// - The uncertainty about the Electronic RMS to 0.3 per slice
// - The F-Factor is assumed to have been measured in Munich to 1.13 - 1.17.
//   with the Munich definition of the F-Factor, thus:
//   F = Sigma(Out)/Mean(Out) * Mean(In)/Sigma(In)
//   Mean F-Factor (gkFFactor)     = 1.15
//   Error F-Factor (gkFFactorErr) = 0.02
//
// The following variables are calculated inside this class:
// -  fLoGainPedRmsSquare and fLoGainPedRmsSquareVar (see CalcLoGainPedestal())
// -  fRSigmaSquare and fRSigmaSquareVar             (see CalcReducedSigma()  )
// -  fPheFFactorMethod and fPheFFactorMethodVar     (see CalcFFactor()       )
// -  fMeanConvFADC2Phe and fMeanConvFADC2PheVar     (see CalcConvFFactor()  )
//
// The following variables are set by MHCalibrationChargeCam:
// -  fAbsTimeMean and fAbsTimeRms
// -  all variables in MCalibrationPix
//
// The following variables are set by MCalibrationChargeCalc:
// - fPed, fPedVar and fPedRms                         
// - fMeanConvFADC2Phe
// - fConvFADC2PheVar 
// - fSigmaConvFADC2Phe
// - fTotalFFactorFFactorMethod 
// - fTotalFFactorFFactorMethodVar 
//
// The following variables are not yet implemented:
// - fConversionHiLo and fConversionHiLoVar (now set fixed to 10. +- 2.5)
//
//  Error of all variables are calculated by error-propagation. Note that internally, 
//  all error variables contain Variances in order to save the CPU-intensive square rooting 
// 
//  Low-Gain variables are stored internally unconverted, i.e. directly from the summed 
//  FADC slices extraction results, but can be retrieved converted to High-Gain amplifications 
//  by calls to: GetConvertedMean() or GetConvertedSigma()
//
// Inline Functions:
// -----------------
//
// GetPedRms(): Get the pedestals RMS: Test bit kHiGainSaturation:
//  If yes, return square root of fLoGainPedRmsSquare (if greater than 0,
//  otherwise -1.), If no,  return fPedRms
//
// GetConvertedMean(): Get the Low Gain Mean Charge converted to High Gain
//  amplification: Returns fLoGainMean multiplied with fConversionHiLo if
//  IsHiGainSaturation(), else return fHiGainMean
//
// GetConvertedSigma(): Get the Low Gain Sigma converted to High Gain
//  amplification: Returns fLoGainSigma multiplied with fConversionHiLo
//  if IsHiGainSaturation() else return fHiGainSigma
//
// GetConvertedRSigmaSquare(): Get the reduced Sigma Square:
//  If fRSigmaSquare is smaller than 0 (i.e. has not yet been set), return -1.
//  Test bit kHiGainSaturation:
//  If yes, return fRSigmaSquare, multiplied with fConversionHiLo^2,
//  If no , return fRSigmaSquare
//
// GetPheFFactorMethodRelVar(): Get the relative variance on the number of
//  photo-electrons (F-Factor Method): If fPheFFactorMethodVar is smaller
//  than 0 (i.e. has not yet been set), return -1. If fPheFFactorMethod
//  is 0, return -1. Else returns fPheFFactorMethodVar / fPheFFactorMethod^2
//
// GetMeanConvFADC2PheErr(): Get the error on the mean conversion factor
//  (FFactor  Method): If fMeanConvFADC2PheVar is smaller than 0 (i.e. has
//  not yet been set), return -1. Else returns the square root of
//  fMeanConvFADC2PheVar
//
// 
// See also: MCalibrationChargeCam, MCalibrationChargeCalc,
//           MHCalibrationChargeCam, MHCalibrationChargePix
//
//
// ClassVersion 3:
//  + fNumSaturated
//
// Class Version 4:
//  +  Float_t fConversionHiLoSigma;             // Sigma of conversion factor betw. Hi and Lo Gain
//  -  Float_t fMeanConvFADC2PheVar;             // Variance conversion factor (F-factor method)
//  +  Float_t fMeanConvFADC2PheStatVar;         // Variance conversion factor, only stat. error
//  +  Float_t fMeanConvFADC2PheSystVar;         // Variance conversion factor, only syst. error
//  -  Float_t fMeanFFactorFADC2PhotVar;         // Variance mean F-Factor photons (F-factor method)
//  +  Float_t fMeanFFactorFADC2PhotVar;         // Variance mean F-Factor photons, only stat. error
//  -  Float_t fPheFFactorMethod;                // Number Phe's calculated (F-factor method)
//  -  Float_t fPheFFactorMethodVar;             // Variance number of Phe's (F-factor method)
//  +  Float_t fPheFFactorMethod;                // Number Phe's calculated  with F-factor method)
//  +  Float_t fPheFFactorMethodStatVar;         // Variance number of Phe's, only stat. error
//  +  Float_t fPheFFactorMethodSystVar;         // Variance number of Phe's, only syst. error
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationChargePix.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MBadPixelsPix.h"

ClassImp(MCalibrationChargePix);

using namespace std;

const Float_t MCalibrationChargePix::gkElectronicPedRms    = 1.0;
const Float_t MCalibrationChargePix::gkElectronicPedRmsErr = 0.35;
const Float_t MCalibrationChargePix::gkFFactor             = 1.15;
const Float_t MCalibrationChargePix::gkFFactorErr          = 0.02;

const Float_t MCalibrationChargePix::fgConversionHiLo         = 10.;
const Float_t MCalibrationChargePix::fgConversionHiLoErr      = 0.05;
const Float_t MCalibrationChargePix::fgConversionHiLoSigma    = 2.5;
const Float_t MCalibrationChargePix::fgPheFFactorMethodLimit  = 1.;
const Float_t MCalibrationChargePix::fgConvFFactorRelErrLimit = 0.85;

// --------------------------------------------------------------------------
//
// Default Constructor: 
//
// Sets:
// - fCalibFlags to 0
// - fConversionHiLo to fgConversionHiLo
// - fConversionHiLoVar to square of fgConversionHiLoErr
// - fConvFFactorelErrLimit to fgConvFFactorRelErrLimit*fgConvFFactorelErrLimit
// - fPheFFactorLimit to fgPheFFactorLimit
// 
// Calls:
// - Clear()
//
MCalibrationChargePix::MCalibrationChargePix(const char *name, const char *title)
    : fCalibFlags(0), fNumSaturated(0)
{

  fName  = name  ? name  : "MCalibrationChargePix";
  fTitle = title ? title : "Container of the fit results of MHCalibrationChargePixs ";

  //
  // At the moment, we don't have a database, yet, 
  // so we get it from the configuration file
  //
  SetConversionHiLo();
  SetConversionHiLoErr();

  SetPheFFactorMethodLimit();
  SetConvFFactorRelErrLimit();
  
  Clear();
}

// ------------------------------------------------------------------------
//
// Sets:
// - all flags to kFALSE
// - all variables to -1.
//
// Calls: 
// - MCalibrationPix::Clear()
//
void MCalibrationChargePix::Clear(Option_t *o)
{

  SetFFactorMethodValid     ( kFALSE );

  fRSigmaSquare                     =  -1.;
  fRSigmaSquareVar                  =  -1.;
  
  fPed                              =  -1.;
  fPedRms                           =  -1.;
  fPedVar                           =  -1.;

  fLoGainPedRmsSquare               =  -1.;
  fLoGainPedRmsSquareVar            =  -1.;

  fAbsTimeMean                      =  -1.;
  fAbsTimeRms                       =  -1.;

  fPheFFactorMethod                 =  -1.;
  fPheFFactorMethodStatVar          =  -1.;
  fPheFFactorMethodSystVar          =  -1.;

  fMeanConvFADC2Phe                 =  -1.;
  fMeanConvFADC2PheStatVar          =  -1.;
  fMeanConvFADC2PheSystVar          =  -1.;
  fMeanFFactorFADC2Phot             =  -1.;
  fMeanFFactorFADC2PhotVar          =  -1.;  

  fNumSaturated                     =   0;

  MCalibrationPix::Clear();
}

// -----------------------------------------------------
//
void MCalibrationChargePix::Copy(TObject& object) const
{
  MCalibrationChargePix &pix = (MCalibrationChargePix&)object;
  
  MCalibrationPix::Copy(pix);

  //
  // Copy the data members
  //
  pix.fAbsTimeMean                = fAbsTimeMean              ;
  pix.fAbsTimeRms                 = fAbsTimeRms               ;
  pix.fCalibFlags                 = fCalibFlags               ;
  pix.fConversionHiLo             = fConversionHiLo           ;
  pix.fConversionHiLoVar          = fConversionHiLoVar        ;
  pix.fConversionHiLoSigma        = fConversionHiLoSigma      ;
  pix.fConvFFactorRelVarLimit     = fConvFFactorRelVarLimit   ;
  pix.fLoGainPedRmsSquare         = fLoGainPedRmsSquare       ;
  pix.fLoGainPedRmsSquareVar      = fLoGainPedRmsSquareVar    ;
  pix.fMeanConvFADC2Phe           = fMeanConvFADC2Phe         ;
  pix.fMeanConvFADC2PheStatVar    = fMeanConvFADC2PheStatVar  ;
  pix.fMeanConvFADC2PheSystVar    = fMeanConvFADC2PheSystVar  ;
  pix.fMeanFFactorFADC2Phot       = fMeanFFactorFADC2Phot     ;
  pix.fMeanFFactorFADC2PhotVar    = fMeanFFactorFADC2PhotVar  ;
  pix.fPed                        = fPed                      ;
  pix.fPedVar                     = fPedVar                   ;
  pix.fPedRms                     = fPedRms                     ;
  pix.fPedRmsVar                  = fPedRmsVar                  ;
  pix.fPheFFactorMethod           = fPheFFactorMethod           ;
  pix.fPheFFactorMethodStatVar    = fPheFFactorMethodStatVar    ;
  pix.fPheFFactorMethodSystVar    = fPheFFactorMethodSystVar    ;
  pix.fPheFFactorMethodLimit      = fPheFFactorMethodLimit      ;
  pix.fRSigmaSquare               = fRSigmaSquare               ;
  pix.fRSigmaSquareVar            = fRSigmaSquareVar            ;
  pix.fNumSaturated               = fNumSaturated               ;
}                                  


// --------------------------------------------------------------------------
//
// Set F-Factor Method Validity Bit from outside 
//
void MCalibrationChargePix::SetFFactorMethodValid(const Bool_t b )
{ 
  b ?  SETBIT(fCalibFlags, kFFactorMethodValid) : CLRBIT(fCalibFlags, kFFactorMethodValid); 
}    

// --------------------------------------------------------------------------
//
// Set pedestals from outside (done by MCalibrationChargeCalc)
//
void MCalibrationChargePix::SetPedestal(const Float_t ped, const Float_t pedrms, const Float_t pederr)
{
  fPed       = ped;    
  fPedRms    = pedrms;
  fPedVar    = pederr*pederr;
}

// --------------------------------------------------------------------------
//
// Set pedestals from outside (done by MCalibrationChargeCalc)
//
void MCalibrationChargePix::SetPed(const Float_t ped, const Float_t pederr)
{
  fPed       = ped;    
  fPedVar    = pederr*pederr;
}

// --------------------------------------------------------------------------
//
// Set pedestals RMS from outside (done by MHCalibrationChargeCam)
//
void MCalibrationChargePix::SetPedRMS( const Float_t pedrms, const Float_t pedrmserr)
{
  fPedRms    = pedrms;
  fPedRmsVar = pedrmserr*pedrmserr;
}

// --------------------------------------------------------------------------
//
//  Get the conversion Error Hi-Gain to Low-Gain:
//  If fConversionHiLoVar is smaller than 0 (i.e. has not yet
//  been set), return -1.
//  
Float_t MCalibrationChargePix::GetConversionHiLoErr() const
{
    return fConversionHiLoVar<0 ? -1 : TMath::Sqrt(fConversionHiLoVar);
}

// --------------------------------------------------------------------------
//
// Get the Error of the converted Low Gain Mean: Returns -1 if the
// variable fLoGainMean or fLoGainMeanVar are smaller than 0. Returns the
// square root of the quadratic sum of the relative variances of the
// fLoGainMean and fConversionHiLo, mulitplied with GetConvertedMean() in
// case of HiGain Saturation, else return GetMeanErr()
//
Float_t MCalibrationChargePix::GetConvertedMeanErr() const
{
    if (!IsHiGainSaturation())
        return GetMeanErr();

    const Float_t logainrelvar = GetLoGainMeanRelVar();
    return logainrelvar<0 ? -1 : TMath::Sqrt(logainrelvar + GetConversionHiLoRelVar()) * GetConvertedMean();
}

// --------------------------------------------------------------------------
//
// Get the Error of the converted Sigma: Returns -1 if the variable
// fLoGainSigma or fLoGainSigmaVar are smaller than 0. if
// IsHiGainSaturatio() returns the square root of the quadratic sum of the
// relative variances of the fLoGainSigma and fConversionHiLo, mulitplied
// with GetConvertedSigma() else returns GetSigmaErr()
//
Float_t MCalibrationChargePix::GetConvertedSigmaErr() const
{
    if (!IsHiGainSaturation())
        return GetSigmaErr();

    if (fLoGainSigmaVar<0 || fLoGainSigma<0)
        return -1.;

    const Float_t sigmaRelVar = fLoGainSigmaVar/(fLoGainSigma*fLoGainSigma);
    return TMath::Sqrt(sigmaRelVar+GetConversionHiLoRelVar()) * GetConvertedSigma();
}

// --------------------------------------------------------------------------
//
// Get the converted reduced Sigma: If fRSigmaSquare is smaller than 0
// (i.e. has not yet been set), return -1. Test bit kHiGainSaturation: If
// yes, return square root of fRSigmaSquare, multiplied with
// fConversionHiLo, If no , return square root of fRSigmaSquare
//
Float_t MCalibrationChargePix::GetConvertedRSigma() const
{
    if (fRSigmaSquare < 0)
        return -1;

    const Float_t rsigma = TMath::Sqrt(fRSigmaSquare);
    return IsHiGainSaturation() ? rsigma*fConversionHiLo : rsigma ;
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetMeanConvFADC2PheErr() const
{
    return fMeanConvFADC2PheStatVar<0 ? -1 : TMath::Sqrt(fMeanConvFADC2PheStatVar);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetMeanConvFADC2PheSystErr() const
{
    return fMeanConvFADC2PheSystVar<0 ? -1 : TMath::Sqrt(fMeanConvFADC2PheSystVar);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetMeanConvFADC2PheTotErr() const
{
    if (fMeanConvFADC2PheSystVar<0 || fMeanConvFADC2PheStatVar<0)
        return -1.;

    return TMath::Sqrt(fMeanConvFADC2PheSystVar+fMeanConvFADC2PheStatVar);
}

// --------------------------------------------------------------------------
//
//  Get the error on the mean total F-Factor of the signal readout
//  (F-Factor Method): If fMeanFFactorFADC2PhotVar is smaller than 0
//  (i.e. has not yet been set), return -1. Else returns the square
//  root of fMeanFFactorFADC2PhotVar
//
Float_t MCalibrationChargePix::GetMeanFFactorFADC2PhotErr() const
{
    return fMeanFFactorFADC2PhotVar<0 ? -1. : TMath::Sqrt(fMeanFFactorFADC2PhotVar);
}

// --------------------------------------------------------------------------
//
// Get the relative variance of the conversion factor between higain and logain:
// - If fConversionHiLo    is 0,              return -1.
// - If fConversionHiLoVar is smaller than 0, return -1.
// - Else returns: fConversionHiLoVar / fConversionHiLo^2
//
const Float_t MCalibrationChargePix::GetConversionHiLoRelVar() const 
{

  if (fConversionHiLoVar < 0.)
    return -1.;

  if (fConversionHiLo == 0.)
    return -1.;

  return fConversionHiLoVar / (fConversionHiLo * fConversionHiLo);
}

// --------------------------------------------------------------------------
//
// Get the relative variance of the electronics pedestal RMS
// - returns rel. variance of gkElectronicPedRms
//
const Float_t MCalibrationChargePix::GetElectronicPedRmsRelVar() const 
{
  return gkElectronicPedRmsErr * gkElectronicPedRmsErr / gkElectronicPedRms / gkElectronicPedRms;
}
 

// --------------------------------------------------------------------------
//
// Get the relative variance of the conversion factor between higain and logain:
// - If gkFFactor    is 0,              return -1.
// - If gkFFactorErr is smaller than 0, return -1.
// - Else returns: gkFFactorErr^2 / gkFFactor*^2
//
const Float_t MCalibrationChargePix::GetFFactorRelVar() const 
{

  if (gkFFactorErr < 0.)
    return -1.;

  if (gkFFactor == 0.)
    return -1.;

  return gkFFactorErr * gkFFactorErr / (gkFFactor * gkFFactor);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetPedErr() const
{
    return TMath::Sqrt(fPedVar);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetPedRms() const
{
    if (!IsHiGainSaturation())
        return fPedRms;

    return fLoGainPedRmsSquare<0 ? -1 : TMath::Sqrt(fLoGainPedRmsSquare);
}

// --------------------------------------------------------------------------
//
// Get the Error of the pedestals RMS: 
// - Test bit kHiGainSaturation: 
//   If yes, return square root of (0.25*fLoGainPedRmsSquareVar/ fLoGainPedRmsSquare) (if greater than 0, otherwise -1.)
//   If no , return square root of (fPedVar) (if greater than 0, otherwise -1.), divided by 2. 
//
Float_t MCalibrationChargePix::GetPedRmsErr()  const
{
  if (IsHiGainSaturation())
      return fLoGainPedRmsSquareVar < 0. ? -1. :TMath::Sqrt(0.25*fLoGainPedRmsSquareVar/fLoGainPedRmsSquare);
  else
      return fPedVar < 0. ? -1. : TMath::Sqrt(fPedVar)/2.;
}

// --------------------------------------------------------------------------
//
// GetPheFFactorMethodErr(): Get the error on the number of photo-electrons
//  (F-Factor Method): If fPheFFactorMethodVar is smaller than 0 (i.e. has
//  not yet been set), return -1. Else returns the square root of
//  fPheFFactorMethodVar
//
Float_t MCalibrationChargePix::GetPheFFactorMethodErr() const
{
    return fPheFFactorMethodStatVar<0 ? -1 : TMath::Sqrt(fPheFFactorMethodStatVar);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetPheFFactorMethodSystErr() const
{
    return fPheFFactorMethodSystVar<0 ? -1 : TMath::Sqrt(fPheFFactorMethodSystVar);
}

// --------------------------------------------------------------------------
//
Float_t MCalibrationChargePix::GetPheFFactorMethodTotErr() const
{
    if (fPheFFactorMethodStatVar<0 || fPheFFactorMethodSystVar<0)
        return -1.;

    return TMath::Sqrt(fPheFFactorMethodStatVar+fPheFFactorMethodSystVar);
}

// --------------------------------------------------------------------------
//
// Get the error of the converted reduced Sigma: 
// - If fRSigmaSquareVar is smaller than 0 (i.e. has not yet been set), return -1.
// - Calculate the absolute variance of the reduced sigma with the formula:
//   reduced sigma variance = 0.25 * fRSigmaSquareVar / fRSigmaSquare
// - Test bit kHiGainSaturation: 
//   If yes, returns the square root of the quadratic sum of the relative variances of the 
//           reduced sigma and fConversionHiLo, mulitplied with GetRSigma()
//   Else returns the square root of rel. (0.25*fRSigmaSquareVar / fRSigmaSquare)
//
Float_t MCalibrationChargePix::GetConvertedRSigmaErr()  const
{

  if (fRSigmaSquareVar < 0)
    return -1;

  //
  // SigmaSquareVar = 4. * Sigma * Sigma * Var(sigma)
  // ==> Var(sigma) = 0.25 * SigmaSquareVar / (Sigma * Sigma)
  //
  const Float_t rsigmaVar = 0.25 * fRSigmaSquareVar / fRSigmaSquare;

  if (IsHiGainSaturation())
    return TMath::Sqrt(rsigmaVar/fRSigmaSquare + GetConversionHiLoRelVar()) * GetRSigma();
  else
    return TMath::Sqrt(rsigmaVar);

}

// --------------------------------------------------------------------------
//
// Get the reduced Sigma:
// If fRSigmaSquare is smaller than 0 (i.e. has not yet been set),
// return -1.
//
Float_t MCalibrationChargePix::GetRSigma() const
{
    return fRSigmaSquare<0 ? -1 : TMath::Sqrt(fRSigmaSquare);
}

// --------------------------------------------------------------------------
//
// Get the error of the reduced Sigma: 
// - If fRSigmaSquareVar is smaller than 0 (i.e. has not yet been set), return -1.
// - Calculate the absolute variance of the reduced sigma with the formula:
//   reduced sigma variance = 0.25 * fRSigmaSquareVar / fRSigmaSquare
//
Float_t MCalibrationChargePix::GetRSigmaErr()  const
{

  if (fRSigmaSquareVar < 0)
    return -1;

  //
  // SigmaSquareVar = 4. * Sigma * Sigma * Var(sigma)
  // ==> Var(sigma) = 0.25 * SigmaSquareVar / (Sigma * Sigma)
  //
  return TMath::Sqrt(0.25 * fRSigmaSquareVar / fRSigmaSquare);

}

// --------------------------------------------------------------------------
//
// Get the reduced Sigma per Charge: 
// - If GetRSigma() is smaller or equal 0. (i.e. has not yet been set), return -1.
// - If GetMean() is 0. or -1. (i.e. has not yet been set), return -1.
// - Return GetRSigma() / GetMean() 
//
Float_t MCalibrationChargePix::GetRSigmaPerCharge()  const 
{
  
  const Float_t rsigma = GetRSigma();

  if (rsigma <= 0)
    return -1.;
  

  const Float_t mean   = GetMean();
  
  if (mean == 0. || mean == -1.)
    return -1.;
  
  return rsigma / mean;
} 


// --------------------------------------------------------------------------
//
// Get the error of the reduced Sigma per Charge: 
// - If GetRSigmaRelVar() is smaller or equal 0. (i.e. has not yet been set), return -1.
// - If GetMeanRelVar() is smaller or equal 0. (i.e. has not yet been set), return -1.
// - Return the propagated error of GetRSigmaPerCharge() 
//
Float_t MCalibrationChargePix::GetRSigmaPerChargeErr()  const 
{
  
  const Float_t rsigmarelvar  = GetRSigmaRelVar();

  if (rsigmarelvar <= 0)
    return -1.;
  

  const Float_t meanrelvar   = GetMeanRelVar();
  
  if (meanrelvar <= 0.)
    return -1.;
  
  return TMath::Sqrt(rsigmarelvar + meanrelvar) * GetRSigmaPerCharge();
} 

// --------------------------------------------------------------------------
//
// Get the relative variance of the reduced Sigma: 
// - If fRSigmaSquareVar is smaller than 0 (i.e. has not yet been set), return -1.
// - Calculate the relative variance of the reduced sigma squares with the formula:
//   reduced sigma rel. variance = 0.25 * fRSigmaSquareVar / fRSigmaSquare / fRSigmaSquare
// - Test bit kHiGainSaturation: 
//   If yes, returns the sum of the relative variances of the reduced sigma and fConversionHiLo
//   Else returns the relative variance of the reduced sigma
//
Float_t MCalibrationChargePix::GetRSigmaRelVar()  const
{

  if (fRSigmaSquareVar < 0)
    return -1;

  //
  // SigmaSquareVar = 4. * Sigma * Sigma * Var(sigma)
  // ==> Var(sigma) = 0.25 * SigmaSquareVar / (Sigma * Sigma)
  //
  return 0.25 * fRSigmaSquareVar / ( fRSigmaSquare * fRSigmaSquare );

}



// --------------------------------------------------------------------------
//
// Test bit kFFactorMethodValid
//
Bool_t MCalibrationChargePix::IsFFactorMethodValid()   const
{ 
  return TESTBIT(fCalibFlags, kFFactorMethodValid);     
}


// ----------------------------------------------------------------------------
// 
// - If fSigma  is smaller than 0 (i.e. has not yet been set), return kFALSE
// - If fPedRms is smaller than 0 (i.e. has not yet been set), return kFALSE
//
// Calculate the reduced sigma of the low-Gain FADC slices:
// - Test bit IsHiGainSaturation() for the Sigma: 
//   If yes, take fLoGainSigma and fLoGainSigmaVar 
//   If no , take fHiGainSigma and fHiGainSigmaVar 
//
// - Test bit IsHiGainSaturation() for the pedRMS: 
//   If yes, take fLoGainPedRmsSquare and fLoGainPedRmsSquareVar
//   If no , take fPedRms and fPedVar
//
// - Calculate the reduced sigma with the formula:
//   fRSigmaSquare = Sigma*Sigma - pedRMS*pedRMS
// 
// - If fRSigmaSquare is smaller than 0, give a warning and return kFALSE
//
// - Calculate the variance of the reduced sigma with the formula:
//   fRSigmaSquareVar = 4.* (sigmaVar*Sigma*Sigma + pedRmsVar*pedRMS*pedRMS)
//
// A back-transformation to the corr. amplification factor of the High-Gain is done 
// in GetRSigma() and GetRSigmaErr()
//
Bool_t MCalibrationChargePix::CalcReducedSigma(const Float_t extractorres)
{

  if (GetSigma() < 0.)
    return kFALSE;
  
  if (GetPedRms() < 0.)
    return kFALSE;

  const Float_t sigma           = IsHiGainSaturation() ? fLoGainSigma           : fHiGainSigma   ;
  const Float_t sigmavar        = IsHiGainSaturation() ? fLoGainSigmaVar        : fHiGainSigmaVar;
  const Float_t pedRmsSquare    = IsHiGainSaturation() ? fLoGainPedRmsSquare    : fPedRms*fPedRms;
  const Float_t pedRmsSquareVar = IsHiGainSaturation() ? fLoGainPedRmsSquareVar : 0.25*fPedVar*pedRmsSquare;
  const Float_t extractorresVar = extractorres * extractorres;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " HiGainSaturation: " << IsHiGainSaturation() 
            << " Sigma: " << sigma 
            << " Var.Sigma: " << sigmavar
            << " PedRmsSquare: " << pedRmsSquare
            << " pedRmsSquareVar: " << pedRmsSquareVar
	    << " extractorresVar: " << extractorresVar
            << endl;
    }
  
  const Float_t sigmaSquare    =      sigma     * sigma;
  const Float_t sigmaSquareVar = 4. * sigmavar  * sigmaSquare;

  //
  // Calculate the reduced sigmas
  //
  fRSigmaSquare = sigmaSquare - pedRmsSquare - extractorresVar;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Red.Sigma Square: " << fRSigmaSquare
            << endl;
    }
  
  if (fRSigmaSquare <= 0.)
    {
      if (IsDebug())
        *fLog << warn 
              << "WARNING: Cannot calculate the reduced sigma: smaller than 0 in pixel " 
              << fPixId << endl;
      return kFALSE;
    }

  
  fRSigmaSquareVar = 4. * (sigmaSquareVar + pedRmsSquareVar);

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Var.Red.Sigma Square: " << fRSigmaSquareVar
            << endl;
    }

  return kTRUE;
}

// ------------------------------------------------------------------
//
// If fRSigmaSquare is smaller than 0 (i.e. has not yet been set),
// return kFALSE
//
// Calculate the number of photo-electrons with the F-Factor method:
// - Test bit IsHiGainSaturation() for the Mean Sum of FADC slices: 
//   If yes, take fLoGainMean and fLoGainMeanVar 
//   If no , take fHiGainMean and fHiGainMeanVar 
//
// - Test bit IsHiGainSaturation() for the pedRMS: 
//   If yes, take fLoGainPedRmsSquare and fLoGainPedRmsSquareVar
//   If no , take fPedRms and fPedVar
//
// - Calculate the number of photo-electrons with the formula:
//   fPheFFactorMethod   = gkFFactor*gkFFactor * Mean * Mean  / fRSigmaSquare
//
// - Calculate the Variance on the photo-electrons with the formula:
//   fPheFFactorMethodVar =  (  4. * gkFFactorErr * gkFFactorErr / ( gkFFactor * gkFFactor ) 
//                            + 4. * Mean Var.   / ( Mean * Mean )
//                            + fRSigmaSquareVar / fRSigmaSquare
//                            ) * fPheFFactorMethod * fPheFFactorMethod
//
// - If fPheFFactorMethod is less than fPheFFactorMethodLimit, 
//   set kFFactorMethodValid to kFALSE and return kFALSE
//
Bool_t MCalibrationChargePix::CalcFFactor()
{
    SetFFactorMethodValid(kFALSE);

    if (fRSigmaSquare < 0.)
        return kFALSE;
  
  //
  // Square all variables in order to avoid applications of square root
  //
  const Float_t meanSquare          =     GetMean()    * GetMean();
  const Float_t meanSquareRelVar    = 4.* GetMeanRelVar();

  const Float_t ffactorsquare       =     gkFFactor    * gkFFactor;
  const Float_t ffactorsquareRelVar = 4.* GetFFactorRelVar();

  const Float_t rsigmaSquareRelVar  =     fRSigmaSquareVar / fRSigmaSquare / fRSigmaSquare;
  //
  // Calculate the number of phe's from the F-Factor method
  // (independent on Hi Gain or Lo Gain)
  //
  fPheFFactorMethod = ffactorsquare * meanSquare / fRSigmaSquare;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " F-Factor Square: " << ffactorsquare
            << " Mean Square: " << meanSquare
            << " Red.Sigma Square: " << fRSigmaSquare
            << " Photo-electrons: " << fPheFFactorMethod
            << endl;
    }

  if (fPheFFactorMethod < fPheFFactorMethodLimit)
    return kFALSE;
  
  //
  // Calculate the Error of Nphe
  //
  const Float_t pheRelVar  = meanSquareRelVar + rsigmaSquareRelVar;
  fPheFFactorMethodStatVar = pheRelVar * fPheFFactorMethod * fPheFFactorMethod;
  fPheFFactorMethodSystVar = ffactorsquareRelVar * fPheFFactorMethod * fPheFFactorMethod;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Rel.Var.F-Factor Square: " << ffactorsquareRelVar
            << " Rel.Var. Mean Square: " << meanSquareRelVar
            << " Rel.Var. Red.Sigma Square: " << rsigmaSquareRelVar
            << " Rel.Var. Photo-electrons: " << pheRelVar
            << endl;
    }

  if (fPheFFactorMethodStatVar < 0. )
    return kFALSE;

  return kTRUE;
}

// ------------------------------------------------------------------
//
// If fPheFFactorMethod is smaller than 0 (i.e. has not yet been set),
// return kFALSE
//
// If GetCovertedMean() is smaller than 0 (i.e. has not yet been set),
// return kFALSE
//
// Calculate fMeanConvFADC2Phe with the following formula:
//
//      fMeanConvFADC2Phe    =  fPheFFactorMethod / GetConvMean();
//
// Calculate the rel. variance of fMeanConvFADC2Phe, taking into account that 
// in the calculation of the number of phe's one mean square has already been used. 
// Now, dividing by another mean, one mean calcels out, one cannot directly propagate
// the errors, but instead havs to take into account this cancellation:
// 
//     convrelvar = ffactorsquareRelVar + GetMeanRelVar() + rsigmaSquareRelVar;
//
// If confrelvar is smaller than 0. or greater than fConvFFactorRelVarLimit, 
// return kFALSE
// 
// Calculate the variance of fMeanConvFADC2Phe with the formula:
//
//    fMeanConvFADC2PheVar =  convrelvar * fMeanConvFADC2Phe * fMeanConvFADC2Phe;
// 
// Set kFFactorMethodValid to kTRUE and 
// return kTRUE
//
Bool_t MCalibrationChargePix::CalcConvFFactor()
{
  
  if (fPheFFactorMethod <= 0.)
    return kFALSE;

  const Float_t convmean = GetConvertedMean();

  if (convmean <= 0.)
    return kFALSE;
  
  fMeanConvFADC2Phe    =  fPheFFactorMethod / convmean;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Converted Mean: " << convmean 
            << " Conversion FADC2Phe: " << fMeanConvFADC2Phe
            << endl;
    }

  const Float_t ffactorsquareRelVar = 4.* GetFFactorRelVar();
  const Float_t rsigmaSquareRelVar  = fRSigmaSquareVar / fRSigmaSquare / fRSigmaSquare;
  //
  // In the calculation of the number of phe's one mean square has already been used. 
  // Now, we divide by another mean, so one mean calcels out, we cannot directly propagate
  // the errors, but have to take account of this cancellation:
  // 
  Float_t convrelvar = GetMeanRelVar() + rsigmaSquareRelVar;
  if (IsHiGainSaturation())
      convrelvar += GetConversionHiLoRelVar();

  const Float_t limit = IsHiGainSaturation() ? fConvFFactorRelVarLimit * 4. : fConvFFactorRelVarLimit;

  //
  // Also have to take into account the pixels labelled MBadPixelsPix::kChargeSigmaNotValid which do not 
  // have a fRSigmaSquareVar, calculate their error directly!
  //
  if (fRSigmaSquareVar < 0.)
    convrelvar = GetMeanRelVar() + GetPheFFactorMethodRelVar();

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Rel.Var.Red.Sigma: " << rsigmaSquareRelVar
            << " Rel.Var.Mean: " << GetMeanRelVar()
            << " Rel.Var.F-Factor: " << ffactorsquareRelVar
            << " Rel.Var.Conversion FADC2Phe: " << convrelvar
            << endl;
    }

  if (convrelvar > limit || convrelvar < 0.)
    {
        *fLog << warn << "pixel  " << setw(4) << fPixId << ": Conv. F-Factor Method Rel. Var.: "
            << Form("%4.3f out of limits: [0,%3.2f]",convrelvar,limit) << endl;
        return kFALSE;
    }
  
  fMeanConvFADC2PheStatVar = convrelvar * fMeanConvFADC2Phe  * fMeanConvFADC2Phe;
  fMeanConvFADC2PheSystVar = ffactorsquareRelVar * fMeanConvFADC2Phe * fMeanConvFADC2Phe;
  
  SetFFactorMethodValid(kTRUE);
  return kTRUE;
}

// ----------------------------------------------------------------------------------
//
// If photflux is smaller or equal 0, return kFALSE
//
// Calculate the total F-Factor with the formula:
//   fMeanFFactorFADC2Phot = Sqrt ( fRSigmaSquare ) / GetMean()  * sqrt(nphotons)
//
// Calculate the error of the total F-Factor
//
Bool_t MCalibrationChargePix::CalcMeanFFactor( const Float_t nphotons, const Float_t nphotonsrelvar )
{


  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Number photons: " << nphotons
            << " Rel.Var.Number photons: " << nphotonsrelvar
            << " Red.Sigma Square: " << fRSigmaSquare
            << " Mean: " << GetMean()
            << endl;
    }


  if (nphotons <= 0.)
    {
      *fLog << warn << GetDescriptor() << ": Assumed photon flux is smaller or equal 0." << endl;
      return kFALSE;
    }

  if (nphotonsrelvar < 0.)
    {
      *fLog << warn << GetDescriptor() << ": Assumed photon flux variance is smaller than 0." << endl;
      return kFALSE;
    }

  fMeanFFactorFADC2Phot =  TMath::Sqrt(fRSigmaSquare * nphotons) / GetMean() ;
  
  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " F-Factor FADC2Phot: " << fMeanFFactorFADC2Phot
            << endl;
    }

  if (fMeanFFactorFADC2Phot < 0.)
    {
      *fLog << warn << GetDescriptor() << ": F-Factor photons to FADC counts smaller than 0." << endl;
      return kFALSE;
    }
  
  const Float_t ffactorrelvar = 0.25 * fRSigmaSquareVar / ( fRSigmaSquare * fRSigmaSquare) 
                              + GetMeanRelVar() 
                              + 0.25 * nphotonsrelvar;
  
  fMeanFFactorFADC2PhotVar = ffactorrelvar * fMeanFFactorFADC2Phot * fMeanFFactorFADC2Phot;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Rel.Var.Red.Sigma: " << 0.25 * fRSigmaSquareVar / ( fRSigmaSquare * fRSigmaSquare) 
            << " Rel.Var.Mean: " << GetMeanRelVar()
            << " Rel.Var.photons: " << 0.25 * nphotonsrelvar
            << " Rel.Var.F-Factor FADC2Phot: " << ffactorrelvar
            << endl;
    }

  return kTRUE;
}


// ----------------------------------------------------------------------------
// 
// - If fPed    is smaller than 0 (i.e. has not yet been set), return.
// - If fPedVar is smaller than 0 (i.e. has not yet been set), return.
//
// Calculate the electronic pedestal RMS with the formula:
//  - elec. pedestal = gkElectronicPedRms * sqrt(logainsamples)
// 
// Calculate the night sky background ped. RMS contribution ("NSB") in the high-gain 
// from the high gain Pedestal RMS with the formula:
// - HiGain NSB square      = fPedRms * fPedRms - elec.ped.* elec.ped.
// - Var(HiGain NSB square) = fPedVar * fPedRms * fPedRms + 4.*elecPedRmsVar * elec.ped.* elec.ped.
//
// If HiGain NSB square is smaller than 0., set it to zero. (but not the error!)
//
// Convert the NSB ped. RMS contribution to the low-gain with the formula:
// - LoGain NSB square      =  HiGain NSB square / (fConversionHiLo*fConversionHiLo)
// - Var(LoGain NSB square) = ( Var(HiGain NSB square) / (HiGain NSB square * HiGain NSB square)
//                              + GetConversionHiLoRelVar()   
//                            ) * LoGain NSB square * LoGain NSB square
//
// - Low Gain Ped RMS Square       = LoGain NSB square      + elec.ped. square
//   Var (Low Gain Ped RMS Square) = Var(LoGain NSB square) + Var(elec.ped. square)
//
void MCalibrationChargePix::CalcLoGainPedestal(Float_t logainsamples)
{

  if (fPedRms < 0.)
    return;

  if (fPedVar < 0.)
    return;

  const Float_t elecPedRms     = gkElectronicPedRms * TMath::Sqrt(logainsamples);
  const Float_t elecPedRmsVar  = GetElectronicPedRmsRelVar() * elecPedRms * elecPedRms;
  
  Float_t pedRmsSquare         = fPedRms * fPedRms;
  Float_t pedRmsSquareVar      = fPedVar * pedRmsSquare; // fPedRmsErr = fPedErr/2.
  
  //
  // We do not know the Low Gain Pedestal RMS, so we have to retrieve it 
  // from the High Gain:  
  //
  // We extract the pure NSB contribution:
  //
  const Float_t elecRmsSquare    =    elecPedRms    * elecPedRms;
  const Float_t elecRmsSquareVar = 4.*elecPedRmsVar * elecRmsSquare;
  
  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " Ped.Rms Square:  " << pedRmsSquare 
            << " Elec.Rms Square: " << elecRmsSquare 
            << " Ped.Rms.Square Var.: " << pedRmsSquareVar 
            << " Elec.Rms Square Var.: " << elecRmsSquareVar 
            << endl;
    }
  

  Float_t higainNsbSquare     =  pedRmsSquare    - elecRmsSquare;
  Float_t higainNsbSquareVar  = (pedRmsSquareVar + elecRmsSquareVar);

  if (higainNsbSquare < 0.001)
    higainNsbSquare = 0.;
  
  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " HiGain NSB Square:  " << higainNsbSquare
            << " Var.HiGain NSB Square:  " << higainNsbSquareVar
            << endl;
    }
  
  //
  // Now, we divide the NSB by the conversion factor and 
  // add it quadratically to the electronic noise
  //
  const Float_t conversionSquare        =     fConversionHiLo    * fConversionHiLo;
  const Float_t conversionSquareRelVar  = 4.* GetConversionHiLoRelVar();

  const Float_t logainNsbSquare         =   higainNsbSquare       / conversionSquare;
  //
  // Calculation of variance of: c = a/b
  //                    Delta(c)^2 = ( Delta(a)^2 + a^2/b^2*(Delta(b)^2 )  / b^2
  //
  const Float_t logainNsbSquareVar      = ( higainNsbSquareVar 
                                         + conversionSquareRelVar * higainNsbSquare * higainNsbSquare ) 
                                          / conversionSquare / conversionSquare;
    
  fLoGainPedRmsSquare    = logainNsbSquare    + elecRmsSquare;
  fLoGainPedRmsSquareVar = logainNsbSquareVar + elecRmsSquareVar;

  if (IsDebug())
    {
      *fLog << dbginf << "ID: " << GetPixId() 
            << " LoGain Ped Rms Square:  " << fLoGainPedRmsSquare
            << " Var.Ped Rms Square:  " << fLoGainPedRmsSquareVar
            << endl;
    }
  
  
}
 
