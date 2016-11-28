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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                               
// MCalibrationBlindPix    
//
// Storage container of the fit results of the Blind Pixel signal 
// (from MHCalibrationChargeBlindPix). 
//
// The Flux is calculated in photons per mm^2 in the camera plane. 
//
// Currently, the following numbers are implemented:
// - fArea: 100 mm^2
// - Average QE of Blind Pixel: 
//    fQEGreen: 0.154
//    fQEBlue : 0.226
//    fQEUV   : 0.247
//    fQECT1  : 0.247
// - Average QE Error of Blind Pixel: 
//    fQEGreenErr: 0.015;
//    fQEBlueErr : 0.02;
//    fQEUVErr   : 0.02;
//    fQECT1Err  : 0.02;
// - Attenuation factor Blind Pixel:
//    fAttGreen :  1.97;
//    fAttBlue  :  1.96;
//    fAttUV    :  1.95;
//    fAttCT1   :  1.95;
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationBlindPix.h"

#include <TMath.h>
#include <TH1.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCalibrationBlindPix);

using namespace std;

const Float_t MCalibrationBlindPix::fgArea       = 100;
const Float_t MCalibrationBlindPix::fgAttGreen   = 1.97;
const Float_t MCalibrationBlindPix::fgAttBlue    = 1.96;
const Float_t MCalibrationBlindPix::fgAttUV      = 1.95;
const Float_t MCalibrationBlindPix::fgAttCT1     = 1.95;
const Float_t MCalibrationBlindPix::fgAttErr     = 0.01;
const Float_t MCalibrationBlindPix::fgQEGreen    = 0.154;
const Float_t MCalibrationBlindPix::fgQEBlue     = 0.226;
const Float_t MCalibrationBlindPix::fgQEUV       = 0.247;
const Float_t MCalibrationBlindPix::fgQECT1      = 0.247;
const Float_t MCalibrationBlindPix::fgQEErrGreen = 0.005;
const Float_t MCalibrationBlindPix::fgQEErrBlue  = 0.007;
const Float_t MCalibrationBlindPix::fgQEErrUV    = 0.01;
const Float_t MCalibrationBlindPix::fgQEErrCT1   = 0.01;
const Float_t MCalibrationBlindPix::fgCollEffGreen = 0.99; 
const Float_t MCalibrationBlindPix::fgCollEffBlue  = 0.93; 
const Float_t MCalibrationBlindPix::fgCollEffUV    = 0.90; 
const Float_t MCalibrationBlindPix::fgCollEffCT1   = 0.90; 
const Float_t MCalibrationBlindPix::fgCollEffErr   = 0.05; 

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Calls:
// - Clear()
//
// For backward-compatibility reasons, quantum eff., coll. eff. and att. 
// are intialized from the static members. This should, however, be 
// overwritten by a class deriving from MCalibrationBlindCam. 
//
MCalibrationBlindPix::MCalibrationBlindPix(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationBlindPix";
  fTitle = title ? title : "Container of the fit results of the blind pixel";

  Clear();

  fArea = fgArea;
  fAreaErr = 0.;

  Float_t att[MCalibrationCam::gkNumPulserColors];

  att [ MCalibrationCam::kGREEN ] = fgAttGreen; 
  att [ MCalibrationCam::kBLUE  ] = fgAttBlue; 
  att [ MCalibrationCam::kUV    ] = fgAttUV; 
  att [ MCalibrationCam::kCT1   ] = fgAttCT1; 

  SetAtt(MCalibrationCam::gkNumPulserColors,att);

  Float_t atterr[MCalibrationCam::gkNumPulserColors];

  atterr [ MCalibrationCam::kGREEN ] = fgAttErr; 
  atterr [ MCalibrationCam::kBLUE  ] = fgAttErr; 
  atterr [ MCalibrationCam::kUV    ] = fgAttErr; 
  atterr [ MCalibrationCam::kCT1   ] = fgAttErr; 
  
  SetAttErr(MCalibrationCam::gkNumPulserColors,atterr);

  Float_t qe[MCalibrationCam::gkNumPulserColors];

  qe [ MCalibrationCam::kGREEN ] = fgQEGreen; 
  qe [ MCalibrationCam::kBLUE  ] = fgQEBlue; 
  qe [ MCalibrationCam::kUV    ] = fgQEUV; 
  qe [ MCalibrationCam::kCT1   ] = fgQECT1; 

  SetQE(MCalibrationCam::gkNumPulserColors,qe);  

  Float_t qeerr[MCalibrationCam::gkNumPulserColors];

  qeerr [ MCalibrationCam::kGREEN ] = fgQEErrGreen; 
  qeerr [ MCalibrationCam::kBLUE  ] = fgQEErrBlue; 
  qeerr [ MCalibrationCam::kUV    ] = fgQEErrUV; 
  qeerr [ MCalibrationCam::kCT1   ] = fgQEErrCT1; 

  SetQEErr(MCalibrationCam::gkNumPulserColors,qeerr);  

  Float_t colleff[MCalibrationCam::gkNumPulserColors];

  colleff [ MCalibrationCam::kGREEN ] = fgCollEffGreen; 
  colleff [ MCalibrationCam::kBLUE  ] = fgCollEffBlue; 
  colleff [ MCalibrationCam::kUV    ] = fgCollEffUV; 
  colleff [ MCalibrationCam::kCT1   ] = fgCollEffCT1; 

  SetCollEff(MCalibrationCam::gkNumPulserColors,colleff);  

  Float_t collefferr[MCalibrationCam::gkNumPulserColors];

  collefferr [ MCalibrationCam::kGREEN ] = fgCollEffErr; 
  collefferr [ MCalibrationCam::kBLUE  ] = fgCollEffErr; 
  collefferr [ MCalibrationCam::kUV    ] = fgCollEffErr; 
  collefferr [ MCalibrationCam::kCT1   ] = fgCollEffErr; 

  SetCollEffErr(MCalibrationCam::gkNumPulserColors,collefferr);  
}

// ------------------------------------------------------------------------
//
// Sets:
// - all flags to kFALSE
// - all variables to -1.
// - the fColor to MCalibrationCam::kNONE
//
// Calls: 
// - MCalibrationPix::Clear()
//
void MCalibrationBlindPix::Clear(Option_t *o)
{

  fFluxInsidePlexiglass    = -1.;
  fFluxInsidePlexiglassVar = -1.;
  fLambda                  = -1.;
  fLambdaCheck             = -1.;
  fLambdaVar               = -1.;
  fMu0                     = -1.;
  fMu0Err                  = -1.;
  fMu1                     = -1.;
  fMu1Err                  = -1.;
  fSigma0                  = -1.;
  fSigma0Err               = -1.;
  fSigma1                  = -1.;
  fSigma1Err               = -1.;

  SetOscillating                   ( kFALSE );
  SetExcluded                      ( kFALSE );
  SetChargeFitValid                ( kFALSE );
  SetPedestalFitOK                 ( kFALSE );
  SetSinglePheFitOK                ( kFALSE );
  SetFluxInsidePlexiglassAvailable ( kFALSE );
  
  SetColor(MCalibrationCam::kNONE);

  MCalibrationPix::Clear();
}

void  MCalibrationBlindPix::SetFluxInsidePlexiglassAvailable( const Bool_t b)
{
    b ? SETBIT(fFlags,kFluxInsidePlexiglassAvailable) : CLRBIT(fFlags,kFluxInsidePlexiglassAvailable);
}


// --------------------------------------------------------------------------
//
// Set the Oscillating Bit from outside 
//
void  MCalibrationBlindPix::SetOscillating( const Bool_t b)
{
    b ? SETBIT(fFlags,kOscillating) : CLRBIT(fFlags,kOscillating);
}

// -----------------------------------------------------
//
// copy 'constructor'
//
void MCalibrationBlindPix::Copy(TObject& object) const
{

  MCalibrationBlindPix &pix = (MCalibrationBlindPix&)object;

  //
  // Copy the data members
  //
  pix.fPixId      = fPixId;
  pix.fFlags      = fFlags;
  pix.fArea       = fArea;
  pix.fAreaErr    = fAreaErr;                     
  pix.fColor      = fColor;

  //
  // Copy arrays
  //
  pix.fAtt        = fAtt;
  pix.fAttErr     = fAttErr;
  pix.fQE         = fQE;
  pix.fQEErr      = fQEErr;
  pix.fCollEff    = fCollEff;
  pix.fCollEffErr = fCollEffErr;

  pix.fLambda                  = fLambda;
  pix.fLambdaCheck             = fLambdaCheck;
  pix.fLambdaCheckErr          = fLambdaCheckErr;
  pix.fLambdaVar               = fLambdaVar;
  pix.fFluxInsidePlexiglass    = fFluxInsidePlexiglass;
  pix.fFluxInsidePlexiglassVar = fFluxInsidePlexiglassVar;
  pix.fMu0       = fMu0;
  pix.fMu0Err    = fMu0Err;
  pix.fMu1       = fMu1;
  pix.fMu1Err    = fMu1Err;
  pix.fSigma0    = fSigma0;
  pix.fSigma0Err = fSigma0Err;
  pix.fSigma1    = fSigma1;
  pix.fSigma1Err = fSigma1Err;

}


// --------------------------------------------------------------------------
//
// Set the ChargeFitValid Bit from outside 
//
void  MCalibrationBlindPix::SetChargeFitValid( const Bool_t b)
{
    b ? SETBIT(fFlags,kChargeFitValid) : CLRBIT(fFlags,kChargeFitValid);
}

// --------------------------------------------------------------------------
//
// Set the PedestalFitValid Bit from outside 
//
void  MCalibrationBlindPix::SetPedestalFitOK( const Bool_t b)
{
    b ? SETBIT(fFlags,kPedestalFitOK) : CLRBIT(fFlags,kPedestalFitOK);
}

// --------------------------------------------------------------------------
//
// Set the SinglePheFitValid Bit from outside 
//
void  MCalibrationBlindPix::SetSinglePheFitOK( const Bool_t b)
{
    b ? SETBIT(fFlags,kSinglePheFitOK) : CLRBIT(fFlags,kSinglePheFitOK);
}

// --------------------------------------------------------------------------
//
// Return -1 if fFluxInsidePlexiglassVar is smaller than 0.
// Return square root of fFluxInsidePlexiglassVar
// 
const Float_t MCalibrationBlindPix::GetFluxInsidePlexiglassErr() const
{
  if (fFluxInsidePlexiglassVar < 0.)
    return -1.;
  
  return TMath::Sqrt(fFluxInsidePlexiglassVar);
}

// --------------------------------------------------------------------------
//
// Return -1 if fFluxInsidePlexiglassVar is smaller than 0.
// Return -1 if fFluxInsidePlexiglass    is 0.
// Return fFluxInsidePlexiglassVar / fFluxInsidePlexiglass^2
// 
const Float_t MCalibrationBlindPix::GetFluxInsidePlexiglassRelVar() const
{
  if (fFluxInsidePlexiglassVar < 0.)
    return -1.;

  if (fFluxInsidePlexiglass == 0.)
    return -1.;
  
  return fFluxInsidePlexiglassVar / (fFluxInsidePlexiglass * fFluxInsidePlexiglass) ;
}

// --------------------------------------------------------------------------
//
// Return -1 if fLambdaVar is smaller than 0.
// Return square root of fLambdaVar
// 
const Float_t MCalibrationBlindPix::GetLambdaErr() const
{
  if (fLambdaVar < 0.)
    return -1.;
  
  return TMath::Sqrt(fLambdaVar);
}

// --------------------------------------------------------------------------
//
// Return -1 if fLambdaVar is smaller than 0.
// Return -1 if fLambda    is 0.
// Return fLambdaVar / (fLambda * fLambda )
// 
const Float_t  MCalibrationBlindPix::GetLambdaRelVar() const
{
  if (fLambdaVar < 0.)
    return -1.;
  
  if (fLambda  == 0.)
    return -1.;
  
  return fLambdaVar / fLambda / fLambda ;
}

// --------------------------------------------------------------------------
//
// Return fAreaErr^2 / (fArea^2 )
// 
const Float_t MCalibrationBlindPix::GetAreaRelVar() const
{
  return fAreaErr*fAreaErr/fArea/fArea;
}


// --------------------------------------------------------------------------
//
// Return TMath::Power(10,fAtt[fColor])
//
const Float_t MCalibrationBlindPix::GetAtt() const
{
  return TMath::Power(10,fAtt[fColor]);   
}

// --------------------------------------------------------------------------
//
// Return -1 if fAttErr[fColor] is smaller than 0.
// Error of TMath::Power(10,fAtt[fColor]) = TMath::Power(10,fAtt[fColor])*ln(10.)*fAttErr[fColor]
// Return fAttErr^2 / (fAtt^2 )
// 
const Float_t MCalibrationBlindPix::GetAttRelVar() const
{

  const Float_t ferr = fAttErr[fColor];

  if (ferr < 0.)
    return -1.;
  
  return ferr*ferr*2.3;
}

// --------------------------------------------------------------------------
//
// Return fQE[fColor]
//
const Float_t MCalibrationBlindPix::GetQE() const
{
  return fQE[fColor];
}

// --------------------------------------------------------------------------
//
// Return -1 if fQEErr[fColor] is smaller than 0.
// Return fQEErr^2 / (fQE^2 )
// 
const Float_t MCalibrationBlindPix::GetQERelVar() const
{

  if (fQEErr[fColor] < 0.)
    return -1.;
  
  return fQEErr[fColor]* fQEErr[fColor] / GetQE() / GetQE();
}

// --------------------------------------------------------------------------
//
// Return fCollEff[fColor]
//
const Float_t MCalibrationBlindPix::GetCollEff() const
{
  return fCollEff[fColor];
}

// --------------------------------------------------------------------------
//
// Return -1 if fCollEffErr[fColor] is smaller than 0.
// Return fCollEffErr^2 / (fCollEff^2 )
// 
const Float_t MCalibrationBlindPix::GetCollEffRelVar() const
{

  if (fCollEffErr[fColor] < 0.)
    return -1.;
  
  return fCollEffErr[fColor]* fCollEffErr[fColor] / GetCollEff() / GetCollEff();
}

// --------------------------------------------------------------------------
//
// Test bit kChargeFitValid
//
const Bool_t MCalibrationBlindPix::IsChargeFitValid()  const 
{
    return TESTBIT(fFlags,kChargeFitValid);
}

// --------------------------------------------------------------------------
//
// Test bit kOscillating
//
const Bool_t MCalibrationBlindPix::IsOscillating()  const 
{
    return TESTBIT(fFlags,kOscillating);
}

// --------------------------------------------------------------------------
//
// Test bit kPedestalFitValid
//
const Bool_t MCalibrationBlindPix::IsPedestalFitOK()  const 
{
    return TESTBIT(fFlags,kPedestalFitOK);
}

// --------------------------------------------------------------------------
//
// Test bit kSinglePheFitValid
//
const Bool_t MCalibrationBlindPix::IsSinglePheFitOK()  const 
{
    return TESTBIT(fFlags,kSinglePheFitOK);
}

// --------------------------------------------------------------------------
//
// Test bit kFluxInsidePlexiglassAvailable
//
const Bool_t  MCalibrationBlindPix::IsFluxInsidePlexiglassAvailable()   const
{
  return TESTBIT(fFlags,kFluxInsidePlexiglassAvailable);
}


// --------------------------------------------------------------------------
//
// Return kFALSE if IsChargeFitValid() is kFALSE
//
// Calculate fFluxInsidePlexiglass with the formula:
// - fFluxInsidePlexiglass    = fLambda 
//                            / GetCollEff()
//                            / GetQE() 
//                            * GetAtt()
//                            / fArea 
// - fFluxInsidePlexiglassVar = sqrt( fLambdaVar / ( fLambda * fLambda ) 
//                             + GetQERelVar() 
//                             + GetCollEffRelVar()
//                             + GetAttRelVar()
//                               ) * fFluxInsidePlexiglass * * fFluxInsidePlexiglass
//
// If the fFluxInsidePlexiglass is smaller than 0., return kFALSE
// If the Variance is smaller than 0., return kFALSE
//
// SetFluxInsidePlexiglassAvailable() and return kTRUE
//
Bool_t MCalibrationBlindPix::CalcFluxInsidePlexiglass()
{

  if (IsChargeFitValid())
    return kFALSE;
  

  //
  // Start calculation of number of photons 
  // The blind pixel has exactly 100 mm^2 area (with negligible error), 
  //
  fFluxInsidePlexiglass    = fLambda / GetQE() * GetAtt() / GetCollEff() / fArea;   

  if (fFluxInsidePlexiglass < 0.)
      return kFALSE;

  fFluxInsidePlexiglassVar = GetLambdaRelVar() + GetQERelVar() + GetAttRelVar() + GetCollEffRelVar() + GetAreaRelVar();

  //
  // Finish calculation of errors -> convert from relative variance to absolute variance
  //
  fFluxInsidePlexiglassVar *= fFluxInsidePlexiglass * fFluxInsidePlexiglass;

  if (fFluxInsidePlexiglassVar < 0.)
      return kFALSE;

  SetFluxInsidePlexiglassAvailable(kTRUE);  

  *fLog << inf << GetDescriptor() 
      << ": BlindPix #" << fPixId << ": Flux [ph/mm^2] inside Plexiglass: "
      << Form("%5.3f +- %5.3f",fFluxInsidePlexiglass, GetFluxInsidePlexiglassErr()) << endl;

  return kTRUE;
}

void MCalibrationBlindPix::Print(Option_t *opt) const
{
  
  *fLog << all << GetDescriptor() 
        << Form("BlindPixel: %3i"         ,GetPixId())
        << Form("  Lambda: %4.2f +- %4.2f",GetLambda(),GetLambdaErr())
        << Form("  Mu0: %4.2f +- %4.2f"   ,GetMu0(),   GetMu0Err())
        << Form("  Mu1: %4.2f +- %4.2f"   ,GetMu1(),   GetMu1Err())
        << Form("  Sigma0: %4.2f +- %4.2f",GetSigma0(),GetSigma0Err())
        << Form("  Sigma1: %4.2f +- %4.2f",GetSigma1(),GetSigma1Err())
        << endl;
  *fLog << all
        << " Pedestal Fit OK? :" << IsPedestalFitOK() 
        << Form("  Lambda (Check): %4.2f +- %4.2f",GetLambdaCheck(),GetLambdaCheckErr())
        << endl;
  *fLog << all
        << " Flux available? :" << IsFluxInsidePlexiglassAvailable() 
        << Form("  Flux: %4.2f +- %4.2f",GetFluxInsidePlexiglass(),GetFluxInsidePlexiglassErr())
        << endl;
}
