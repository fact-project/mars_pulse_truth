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
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                     
// MCalibrationChargePINDiode       
//                                  
// Storage container of the fit results of the PIN Diode signal 
// (from MHCalibrationChargePINDiode). The PIN Diode is not yet working, so 
// is the documentation for the moment.
//
// The Flux is calculated in photons per mm^2 in the camera plane. 
//
// Currently, the following numbers are implemented:
//
// Area of PIN Diode   Ap: 63.5 +- 0.1 mm^2 
// Transparency of window T: 0.88 +- 0.05
//
// Ratio of areas: 
//
// Distance of PIN Diode to pulser D1:   1.139  +- 0.005 m
// Distance of Inner Pixel to pulser D2: 18.135 +- 0.015 m
//
//                          D1*D1
// gkSolidAngleRatio   =   -------- = 7.06 * 10^-5
//                         Ap*T*D2*D2
//
// gkSolidAngleRatioErr = 0.4 * 10^-5
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationChargePINDiode.h"
#include "MCalibrationChargePix.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCalibrationChargePINDiode);

using namespace std;

const Float_t MCalibrationChargePINDiode::fgChargeToPhotons    = -1.; 
const Float_t MCalibrationChargePINDiode::fgChargeToPhotonsErr = -1.; 
const Float_t MCalibrationChargePINDiode::gkAbsorptionWindow   = 0.88; 
const Float_t MCalibrationChargePINDiode::gkSolidAngleRatio    = 0.0000705;
const Float_t MCalibrationChargePINDiode::gkSolidAngleRatioErr = 0.000004;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEGreen    = 0.84;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEBlue     = 0.74;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEUV       = 0.40;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQECT1      = 0.40;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEGreenErr = 0.015;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEBlueErr  = 0.015;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQEUVErr    = 0.015;
const Float_t MCalibrationChargePINDiode::gkPINDiodeQECT1Err   = 0.015;
// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets:
// - fCalibFlags to 0
// - fChargeToPhotons to fgChargeToPhotons
// - fChargeToPhotonsVar to fgChargeToPhotonsErr*fgChargeToPhotonsErr
// 
// Calls:
// - Clear()
//
MCalibrationChargePINDiode::MCalibrationChargePINDiode(const char *name, const char *title)
    : fCalibFlags(0)
{

  fName  = name  ? name  : "MCalibrationChargePINDiode";
  fTitle = title ? title : "Container of the fit results of MHCalibrationChargePINDiode";

  SetChargeToPhotons();
  SetChargeToPhotonsErr();

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
void MCalibrationChargePINDiode::Clear(Option_t *o)
{

  SetOscillating        ( kFALSE );
  SetChargeFitValid     ( kFALSE );
  SetTimeFitValid       ( kFALSE );
  SetColor              ( MCalibrationCam::kNONE );
    
  fAbsTimeMean              =  -1.;
  fAbsTimeRms               =  -1.;
  fFluxOutsidePlexiglass    =  -1.;  
  fFluxOutsidePlexiglassVar =  -1.;
  fNumPhotons               =  -1.;
  fNumPhotonsVar            =  -1.;
  fPed                      =  -1.;
  fPedRms                   =  -1.;
  fRmsChargeMean            =  -1.;
  fRmsChargeMeanErr         =  -1.;
  fRmsChargeSigma           =  -1.;  
  fRmsChargeSigmaErr        =  -1.;

  MCalibrationPix::Clear();
}


// --------------------------------------------------------------------------
//
// Set the pedestals from outside
//
void MCalibrationChargePINDiode::SetPedestal(Float_t ped, Float_t pedrms)
{

  fPed    = ped;    
  fPedRms = pedrms;
  
}

// --------------------------------------------------------------------------
//
// Set the Oscillating Bit from outside 
//
void  MCalibrationChargePINDiode::SetOscillating( const Bool_t b)
{
    b ? SETBIT(fCalibFlags,kOscillating) : CLRBIT(fCalibFlags,kOscillating);
}


// --------------------------------------------------------------------------
//
// Set the ChargeFitValid Bit from outside 
//
void MCalibrationChargePINDiode::SetChargeFitValid(Bool_t b )    
{ 
  b ?  SETBIT(fCalibFlags, kChargeFitValid) : CLRBIT(fCalibFlags, kChargeFitValid); 
}

// --------------------------------------------------------------------------
//
// Set the TimeFitValid Bit from outside 
//
void MCalibrationChargePINDiode::SetTimeFitValid(Bool_t b )    
{ 
  b ?  SETBIT(fCalibFlags, kTimeFitValid) : CLRBIT(fCalibFlags, kTimeFitValid); 
}

// --------------------------------------------------------------------------
//
// Set the FluxOutsidePlexiglassAvailable Bit from outside 
//
void MCalibrationChargePINDiode::SetFluxOutsidePlexiglassAvailable (const Bool_t b)
{
  b ?  SETBIT(fCalibFlags, kFluxOutsidePlexiglassAvailable) 
    : CLRBIT(fCalibFlags, kFluxOutsidePlexiglassAvailable); 
}

// --------------------------------------------------------------------------
//
// Return -1 if fFluxOutsidePlexiglassVar is smaller than 0.
// Return square root of fFluxOutsidePlexiglassVar
// 
Float_t MCalibrationChargePINDiode::GetFluxOutsidePlexiglassErr() const
{
  if (fFluxOutsidePlexiglassVar < 0.)
    return -1.;
  
  return TMath::Sqrt(fFluxOutsidePlexiglassVar);
}

// --------------------------------------------------------------------------
//
// Return -1 if fFluxOutsidePlexiglassVar is smaller than 0.
// Return -1 if fFluxOutsidePlexiglass is 0.
// Return  fFluxOutsidePlexiglassVar / fFluxOutsidePlexiglass^2
// 
Float_t MCalibrationChargePINDiode::GetFluxOutsidePlexiglassRelVar() const
{
  if (fFluxOutsidePlexiglassVar < 0.)
    return -1.;
  
  if (fFluxOutsidePlexiglass == 0.)
    return -1.;
  
  return fFluxOutsidePlexiglassVar / (fFluxOutsidePlexiglass * fFluxOutsidePlexiglass );
}

// --------------------------------------------------------------------------
//
// Return -1 if fNumPhotonsVar is smaller than 0.
// Return square root of fNumPhotonsVar
// 
Float_t MCalibrationChargePINDiode::GetNumPhotonsErr() const
{
  if (fNumPhotonsVar < 0.)
    return -1.;
  
  return TMath::Sqrt(fNumPhotonsVar);
}

// --------------------------------------------------------------------------
//
// Return -1 if fNumPhotonsVar is smaller than 0.
// Return -1 if fNumPhotons    is 0.
// Return fNumPhotonsVar / (fNumPhotons^2 )
// 
Float_t MCalibrationChargePINDiode::GetNumPhotonsRelVar() const
{
  if (fNumPhotonsVar < 0.)
    return -1.;
  
  if (fNumPhotons  == 0.)
    return -1.;
  
  return  fNumPhotonsVar / fNumPhotons / fNumPhotons ;
}


// --------------------------------------------------------------------------
//
// Return -1 if gkPINDiodeQEGreenErr is smaller than 0.
// Return -1 if gkPINDiodeQEGreen    is 0.
// Return gkPINDiodeQEGreenErr^2 / (gkPINDiodeQEGreen^2 )
// 
const Float_t MCalibrationChargePINDiode::GetPINDiodeQEGreenRelVar() const
{
  if (gkPINDiodeQEGreenErr < 0.)
    return -1.;
  
  if (gkPINDiodeQEGreen  == 0.)
    return -1.;
  
  return gkPINDiodeQEGreenErr * gkPINDiodeQEGreenErr / gkPINDiodeQEGreen / gkPINDiodeQEGreen ;
}

// --------------------------------------------------------------------------
//
// Return -1 if gkPINDiodeQEBlueErr is smaller than 0.
// Return -1 if gkPINDiodeQEBlue    is 0.
// Return gkPINDiodeQEBlueErr^2 / gkPINDiodeQEBlue^2
// 
const Float_t MCalibrationChargePINDiode::GetPINDiodeQEBlueRelVar() const
{
  if (gkPINDiodeQEBlueErr < 0.)
    return -1.;
  
  if (gkPINDiodeQEBlue  == 0.)
    return -1.;
  
  return gkPINDiodeQEBlueErr * gkPINDiodeQEBlueErr / gkPINDiodeQEBlue / gkPINDiodeQEBlue ;
}

// --------------------------------------------------------------------------
//
// Return -1 if gkPINDiodeQEUVErr is smaller than 0.
// Return -1 if gkPINDiodeQEUV    is 0.
// Return gkPINDiodeQEUVErr ^2 / gkPINDiodeQEUV^2 
// 
const Float_t MCalibrationChargePINDiode::GetPINDiodeQEUVRelVar() const
{
  if (gkPINDiodeQEUVErr < 0.)
    return -1.;
  
  if (gkPINDiodeQEUV  == 0.)
    return -1.;
  
  return gkPINDiodeQEUVErr * gkPINDiodeQEUVErr / gkPINDiodeQEUV / gkPINDiodeQEUV ;
}

// --------------------------------------------------------------------------
//
// Return -1 if gkPINDiodeQECT1Err is smaller than 0.
// Return -1 if gkPINDiodeQECT1    is 0.
// Return gkPINDiodeQECT1Err ^2 / gkPINDiodeQECT1^2
// 
const Float_t MCalibrationChargePINDiode::GetPINDiodeQECT1RelVar() const
{
  if (gkPINDiodeQECT1Err < 0.)
    return -1.;
  
  if (gkPINDiodeQECT1  == 0.)
    return -1.;
  
  return gkPINDiodeQECT1Err * gkPINDiodeQECT1Err / gkPINDiodeQECT1 / gkPINDiodeQECT1 ;
}

// --------------------------------------------------------------------------
//
// Return -1 if gkSolidAngleRatioErr is smaller than 0.
// Return -1 if gkSolidAngleRatio    is 0.
// Return gkSolidAngleRatioErr ^2 / gkSolidAngleRatio^2
// 
const Float_t MCalibrationChargePINDiode::GetSolidAngleRatioRelVar() const
{
  if (gkSolidAngleRatioErr < 0.)
    return -1.;
  
  if (gkSolidAngleRatio  == 0.)
    return -1.;
  
  return gkSolidAngleRatioErr * gkSolidAngleRatioErr / gkSolidAngleRatio / gkSolidAngleRatio ;
}


// --------------------------------------------------------------------------
//
// Test bit kChargeFitValid
//
Bool_t MCalibrationChargePINDiode::IsChargeFitValid() const 
{
  return TESTBIT(fCalibFlags, kChargeFitValid);  
}

// --------------------------------------------------------------------------
//
// Test bit kFluxOutsidePlexiglassAvailable
//
Bool_t  MCalibrationChargePINDiode::IsFluxOutsidePlexiglassAvailable()   const
{
  return TESTBIT(fFlags,kFluxOutsidePlexiglassAvailable);
}


// --------------------------------------------------------------------------
//
// Test bit kTimeFitValid
//
Bool_t MCalibrationChargePINDiode::IsTimeFitValid()   const 
{
  return TESTBIT(fCalibFlags, kTimeFitValid);  
}

// --------------------------------------------------------------------------
//
// Test bit kOscillating
//
Bool_t MCalibrationChargePINDiode::IsOscillating()   const 
{
  return TESTBIT(fCalibFlags, kOscillating);  
}

// --------------------------------------------------------------------------
//
// Return kFALSE if IsChargeFitValid() is kFALSE
//
// Calculate fFluxOutsidePlexiglass with the formula:
// - fFluxOutsidePlexiglass    = fNumPhotons*gkSolidAngleRatio / gkPINDiodeQE (of the corr. colour) 
// - fFluxOutsidePlexiglassVar = sqrt( fNumPhotonsVar / ( fNumPhotons * fNumPhotons ) 
//                             + ( gkSolidAngleRatioErr * gkSolidAngleRatioErr / gkSolidAngleRatio / gkSolidAngleRatio )
//                             + ( gkPINDiodeQEErr * gkPINDiodeQEErr / gkPINDiodeQE / gkPINDiodeQE )
//                               ) * fFluxOutsidePlexiglass * * fFluxOutsidePlexiglass
//
// If the fFluxOutsidePlexiglass is smaller than 0., return kFALSE
// If the Variance is smaller than 0., return kFALSE
//
// SetFluxOutsidePlexiglassAvailable() and return kTRUE
//
Bool_t MCalibrationChargePINDiode::CalcFluxOutsidePlexiglass()
{

  if (IsChargeFitValid())
    return kFALSE;
  
  //
  // Start calculation of number of photons per mm^2 on an inner pixel:
  // Correct for the distance between camera and PIN Diode and for different areas.
  //
  switch (fColor)
    {
    case MCalibrationCam::kGREEN:
      fFluxOutsidePlexiglass    = fNumPhotons           * gkSolidAngleRatio           * gkPINDiodeQEGreen;
      fFluxOutsidePlexiglassVar = GetNumPhotonsRelVar() + GetSolidAngleRatioRelVar()  + GetPINDiodeQEGreenRelVar();
      break;
    case MCalibrationCam::kBLUE:
      fFluxOutsidePlexiglass    = fNumPhotons           * gkSolidAngleRatio           * gkPINDiodeQEBlue;
      fFluxOutsidePlexiglassVar = GetNumPhotonsRelVar() + GetSolidAngleRatioRelVar()  + GetPINDiodeQEBlueRelVar();
      break; 
    case MCalibrationCam::kUV:
      fFluxOutsidePlexiglass    = fNumPhotons           * gkSolidAngleRatio          * gkPINDiodeQEUV;
      fFluxOutsidePlexiglassVar = GetNumPhotonsRelVar() + GetSolidAngleRatioRelVar() + GetPINDiodeQEUVRelVar();
      break;
    case MCalibrationCam::kCT1:
    default:
      fFluxOutsidePlexiglass    = fNumPhotons           * gkSolidAngleRatio          * gkPINDiodeQECT1;
      fFluxOutsidePlexiglassVar = GetNumPhotonsRelVar() + GetSolidAngleRatioRelVar() + GetPINDiodeQECT1RelVar();
      break;
    }

  //
  // Finish calculation of errors -> convert from relative variance to absolute variance
  //
  fFluxOutsidePlexiglassVar *= fFluxOutsidePlexiglass * fFluxOutsidePlexiglass;

  if (fFluxOutsidePlexiglass < 0.)
      return kFALSE;

  if (fFluxOutsidePlexiglassVar < 0.)
      return kFALSE;

  SetFluxOutsidePlexiglassAvailable();  

  *fLog << inf << endl;
  *fLog << inf << " Mean Photon flux [ph/mm^2] outside Plexiglass: " 
        << Form("%5.3f%s%5.3f",fFluxOutsidePlexiglass," +- ",GetFluxOutsidePlexiglassErr()) << endl;

  return kTRUE;
}
