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
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                     
// MCalibrationPix                                     
//                                                     
// Base Storage container for a calibration pixel. Holds mean and sigmas,  
// their errors, the fit probability and the number of pickup events for 
// the high-gain and the low-gain derived values.    
//                                                                         
// Errors are stored internally as variances, but are returned and filled 
// as square root of the variances. 
//
// Calls to GetMean(), GetMeanErr(), GetSigma(), GetSigmaErr(), GetProb() or 
// GetNumPickup() and GetNumBlackout() test first the bit kHiGainSaturation 
// before returning the high-gain or low-gain value, analogue for the 
// corr. Setters.
//
// The three flags: kValid, kExcluded and kHiGainSaturation may be set.
// The colors: kGREEN, kBLUE, kUV and kCT1 may be set. 
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationPix.h"

#include <TMath.h>

ClassImp(MCalibrationPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor: 
//
// Sets:
// - fPixId to -1
// - fFlags to 0
//
// Calls:
// - Clear()
//
MCalibrationPix::MCalibrationPix(const char *name, const char *title)
    : fPixId(-1),
      fFlags(0)
{

  fName  = name  ? name  : "MCalibrationPix";
  fTitle = title ? title : "Container of the fit results of MHCalibrationPixs ";

  Clear();

}

// ------------------------------------------------------------------------
//
// Sets:
// - all variables to -1
// - all flags to kFALSE
//
void MCalibrationPix::Clear(Option_t *o)
{

  fHiGainNumBlackout   =  -1 ;
  fHiGainNumPickup     =  -1 ;
  fHiGainMean          =  -1.;
  fHiGainMeanVar       =  -1.;
  fHiGainProb          =  -1.;
  fHiGainRms           =  -1.;
  fHiGainSigma         =  -1.;
  fHiGainSigmaVar      =  -1.;

  fLoGainNumBlackout   =  -1 ;
  fLoGainNumPickup     =  -1 ;
  fLoGainMean          =  -1.;
  fLoGainMeanVar       =  -1.;
  fLoGainProb          =  -1.;
  fLoGainRms           =  -1.;
  fLoGainSigma         =  -1.;
  fLoGainSigmaVar      =  -1.;

  SetHiGainSaturation  ( kFALSE );
  SetExcluded          ( kFALSE );
  SetValid             ( kFALSE );
  SetDebug             ( kFALSE );

}


// -----------------------------------------------------
//
// copy 'constructor'
//
void MCalibrationPix::Copy(TObject& object) const
{

  MCalibrationPix &pix = (MCalibrationPix&)object;

  //
  // Copy the data members
  //
  pix.fPixId             = fPixId;
  pix.fFlags             = fFlags;
  pix.fHiGainMean        = fHiGainMean;
  pix.fHiGainMeanVar     = fHiGainMeanVar;
  pix.fHiGainNumBlackout = fHiGainNumBlackout;
  pix.fHiGainNumPickup   = fHiGainNumPickup;
  pix.fHiGainSigma       = fHiGainSigma;
  pix.fHiGainSigmaVar    = fHiGainSigmaVar;
  pix.fHiGainProb        = fHiGainProb;
  pix.fLoGainMean        = fLoGainMean;
  pix.fLoGainMeanVar     = fLoGainMeanVar;
  pix.fLoGainNumBlackout = fLoGainNumBlackout;
  pix.fLoGainNumPickup   = fLoGainNumPickup;
  pix.fLoGainSigma       = fLoGainSigma;
  pix.fLoGainSigmaVar    = fLoGainSigmaVar;
  pix.fLoGainProb        = fLoGainProb;
  
}


// --------------------------------------------------------------------------
//
// Set the Hi Gain Saturation Bit from outside
//
void MCalibrationPix::SetHiGainSaturation(Bool_t b)
{
    b ?  SETBIT(fFlags, kHiGainSaturation) : CLRBIT(fFlags, kHiGainSaturation); 
}

// --------------------------------------------------------------------------
//
// Set the Valid Bit from outside 
//
void MCalibrationPix::SetDebug(Bool_t b )
{ 
    b ?  SETBIT(fFlags, kDebug) : CLRBIT(fFlags, kDebug); 
}

// --------------------------------------------------------------------------
//
// Set the Excluded Bit from outside 
//
void MCalibrationPix::SetExcluded(Bool_t b )
{ 
    b ?  SETBIT(fFlags, kExcluded) : CLRBIT(fFlags, kExcluded); 
}

// --------------------------------------------------------------------------
//
// Set the Valid Bit from outside 
//
void MCalibrationPix::SetValid(Bool_t b )
{ 
    b ?  SETBIT(fFlags, kValid) : CLRBIT(fFlags, kValid); 
}


// --------------------------------------------------------------------------
//
// Return -1, if IsHiGainSaturation()
// Return -1, if the LoGain Mean is smaller than 0.5
// Return -1, if the HiGain Mean is -1.         (has not yet been set) 
// Return fHiGainMean / fLoGainMean
//
Float_t MCalibrationPix::GetHiLoMeansDivided() const
{
  
  if (IsHiGainSaturation())
    return -1.;

  if (fLoGainMean <= 0.5)
    return -1.;

  if (fHiGainMean == -1.)
    return -1.;
  
  return fHiGainMean / fLoGainMean;
  
}

// ----------------------------------------------------------------------------------
//
// Return -1, if IsHiGainSaturation()
// Return -1, if the LoGain Mean or its variance is smaller than 0.5 (has not yet been set)
// Return -1, if the HiGain Mean or its variance is -1.         (has not yet been set) 
// Return propagated error of GetHiLoMeansDivided()
//
Float_t MCalibrationPix::GetHiLoMeansDividedErr() const
{
  
  if (IsHiGainSaturation())
    return -1.;

  if (fLoGainMean <= 0.5)
    return -1.;

  if (fHiGainMean == -1.)
    return -1.;
  
  if (fLoGainMeanVar <= 0.)
    return -1.;

  if (fHiGainMeanVar <= 0.)
    return -1.;
  
  const Float_t lomeansquare = fLoGainMean * fLoGainMean;
  const Float_t deltaHi = fHiGainMeanVar / lomeansquare;
  const Float_t deltaLo = fLoGainMeanVar / (lomeansquare * lomeansquare) * fHiGainMean * fHiGainMean;

  return TMath::Sqrt(deltaHi + deltaLo);
  
}

// --------------------------------------------------------------------------
//
// Return -1, if IsHiGainSaturation()
// Return -1, if the LoGain Sigma is smaller than 0.01 
// Return -1, if the HiGain Sigma is -1.         (has not yet been set) 
// Return fHiGainSigma / fLoGainSigma
//
Float_t MCalibrationPix::GetHiLoSigmasDivided() const
{
  
  if (IsHiGainSaturation())
    return -1.;

  if (fLoGainSigma <= 0.01)
    return -1.;

  if (fHiGainSigma == -1.)
    return -1.;
  
  return fHiGainSigma / fLoGainSigma;
  
}

// ----------------------------------------------------------------------------------
//
// Return -1, if IsHiGainSaturation()
// Return -1, if the LoGain Sigma variance is smaller than 0.
// Return -1, if the LoGain Sigma is smaller than 0.01
// Return -1, if the HiGain Sigma or its variance is -1.         (has not yet been set) 
// Return propagated error of GetHiLoSigmasDivided()
//
Float_t MCalibrationPix::GetHiLoSigmasDividedErr() const
{
  
  if (IsHiGainSaturation())
    return -1.;

  if (fLoGainSigma <= 0.01) 
    return -1.;

  if (fHiGainSigma == -1.)
    return -1.;
  
  if (fLoGainSigmaVar <= 0.)
    return -1.;

  if (fHiGainSigmaVar <= 0.)
    return -1.;
  
  const Float_t losigmasquare = fLoGainSigma * fLoGainSigma;
  const Float_t deltaHi = fHiGainSigmaVar / losigmasquare;
  const Float_t deltaLo = fLoGainSigmaVar / (losigmasquare * losigmasquare) * fHiGainSigma * fHiGainSigma;

  return TMath::Sqrt(deltaHi + deltaLo);
  
}


// --------------------------------------------------------------------------
//
// Get the Relative Variance of either High Gain or Low Gain Mean 
// depending on IsHighGainSaturation()
//
// If variance is smaller than 0. return -1.
//
Float_t MCalibrationPix::GetMeanRelVar()  const
{

  if (IsHiGainSaturation())
    if (fLoGainMeanVar < 0. || fLoGainMean < 0.)
      return -1.;
    else
      return fLoGainMeanVar / (fLoGainMean * fLoGainMean);
  else
    if (fHiGainMeanVar < 0. || fHiGainMean < 0.)
      return -1.;
    else
      return fHiGainMeanVar / (fHiGainMean * fHiGainMean);
}

// --------------------------------------------------------------------------
//
// Get the Square of either High Gain or Low Gain Mean 
// depending on IsHighGainSaturation()
//
Float_t MCalibrationPix::GetMeanSquare()  const
{

  if (IsHiGainSaturation())
    return fLoGainMean == -1. ? -1. : fLoGainMean * fLoGainMean;
  else
    return fHiGainMean == -1. ? -1. : fHiGainMean * fHiGainMean;
}


// --------------------------------------------------------------------------
//
// Get the Relative Variance of either High Gain or Low Gain Sigma 
// depending on IsHighGainSaturation()
//
// If variance is smaller than 0. return -1.
//
Float_t MCalibrationPix::GetSigmaRelVar()  const
{

  if (IsHiGainSaturation())
    if (fLoGainSigmaVar < 0.)
      return -1.;
    else
      return fLoGainSigmaVar / (fLoGainSigma * fLoGainSigma);
  else
    if (fHiGainSigmaVar < 0.)
      return -1.;
    else
      return fHiGainSigmaVar / (fHiGainSigma * fHiGainSigma);
}

// --------------------------------------------------------------------------
//
// Get the High Gain Mean Error: Takes square root of fHiGainMeanVar
//
Float_t MCalibrationPix::GetHiGainMeanErr()  const
{
  if (fHiGainMeanVar < 0.)
    return -1.;

  return TMath::Sqrt(fHiGainMeanVar);
}


// --------------------------------------------------------------------------
//
// Get the High Gain Sigma Error: Takes square root of fHiGainSigmaVar
//
Float_t MCalibrationPix::GetHiGainSigmaErr()  const
{
  if (fHiGainSigmaVar < 0.)
    return -1.;

  return TMath::Sqrt(fHiGainSigmaVar);
}

// --------------------------------------------------------------------------
//
// Get the Low Gain Mean Error: Takes square root of fLoGainMeanVar
//
Float_t MCalibrationPix::GetLoGainMeanErr()  const
{
  if (fLoGainMeanVar < 0.)
    return -1.;

  return TMath::Sqrt(fLoGainMeanVar);
}

// --------------------------------------------------------------------------
//
// Get the Low Gain Mean Rel Variance
//
Float_t MCalibrationPix::GetLoGainMeanRelVar()  const
{
  if (fLoGainMeanVar < 0.)
    return -1.;
  if (fLoGainMean   == 0.)
    return -1.;

  return fLoGainMeanVar / ( fLoGainMean * fLoGainMean);
}

// --------------------------------------------------------------------------
//
// Get the High Gain Mean Rel Variance
//
Float_t MCalibrationPix::GetHiGainMeanRelVar()  const
{
  if (fHiGainMeanVar < 0.)
    return -1.;
  if (fHiGainMean   == 0.)
    return -1.;

  return fHiGainMeanVar / ( fHiGainMean * fHiGainMean);
}

// --------------------------------------------------------------------------
//
// Get the Low Gain Sigma Error: Takes square root of fHiGainSigmaVar
//
Float_t MCalibrationPix::GetLoGainSigmaErr()  const
{
  if (fLoGainSigmaVar < 0.)
    return -1.;

  return TMath::Sqrt(fLoGainSigmaVar);
}

// --------------------------------------------------------------------------
//
// Test bit kHiGainSaturation
//
Bool_t MCalibrationPix::IsHiGainSaturation()    const
{ 
  return TESTBIT(fFlags,kHiGainSaturation);  
}

// --------------------------------------------------------------------------
//
// Test bit kDebug
//
Bool_t MCalibrationPix::IsDebug()     const
{ 
   return TESTBIT(fFlags,kDebug);  
}

// --------------------------------------------------------------------------
//
// Test bit kExcluded
//
Bool_t MCalibrationPix::IsExcluded()     const
{ 
   return TESTBIT(fFlags,kExcluded);  
}

// --------------------------------------------------------------------------
//
// Test bit kValid
//
Bool_t MCalibrationPix::IsValid()     const
{ 
   return TESTBIT(fFlags,kValid);  
}

