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
// MCalibrationQEPix                                                    
//                                                                      
// Storage container of the calibrated Quantrum Efficiency of one pixel. 
// This container (like MCalibrationQECam) is designed to persist during 
// several eventloops over different calibration files, especially those 
// with different colour LEDs. This class contains all measured Quantum 
// Efficiencies with the calibration system for each individual pixel.
// 
// At the moment, this calibration works in the following steps:
//
// 1)  MHCalibrationChargeCam extracts mean and sigma (and its errors) of 
//     the summed FADC slices distribution and stores them in MCalibrationCam 
//     
// 2)  MHCalibrationChargeBlindPix extracts the mean of a Poisson fit to the 
//     single photo-electron spectrum and stores it in MCalibrationChargeBlindPix
//     
// 3)  MHCalibrationChargePINDiode extracts the mean of a charge distribution 
//     of the signals collected by the PIN Diode and stores it in 
//     MCalibrationChargePINDiode
//     
// 4)  MCalibrationChargeCalc calculates for every pixel the number of 
//     photo-electrons with the F-Factor method and stores them in MCalibrationChargePix
//
// 5)  MCalibrationChargeCalc calculates the (weighted) average number of photo-
//     electrons from the pixels with the area index 0 (Inner pixels for the MAGIC 
//     camera) and divides this number by gkDefaultQEGreen, gkDefaultQEBlue, 
//     gkDefaultQEUV or gkDefaultQECT1, depending on the used pulser LED colour, 
//     and further by MCalibrationQECam::gkPlexiglassQE. The obtained number is then 
//     divided further by MGeomCam::GetPixRatio(pixel idx) (1. for inner pixels) and 
//     gives the NUMBER OF PHOTONS incident on every pixel light guide OUTSIDE THE PLEXIGLASS 
//     of the camera, obtained with the F-Factor method. (In the case of the MAGIC camera, 
//     this number is thus BY CONSTRUCTION four times bigger for the outer pixels than for 
//     the inner ones.)
//
// 6)  MCalibrationChargeCalc calculates the mean photon flux per mm^2 in the camera 
//     from the MCalibrationChargeBlindPix and multiplies it with the light guides area 
//     of each pixel (MGeomPix::GetA()) and divides it by the quantum efficiency of the 
//     plexi-glass (MCalibrationQECam::gkPlexiglassQE). The obtained number gives the 
//     NUMBER OF PHOTONS incident on every pixel light guide OUTSIDE THE PLEXIGLASS of the camera, 
//     obtained with the Blind Pixel method. 
//
// 7)  MCalibrationChargeCalc calculates the mean photon flux per mm^2 in the camera 
//     from the MCalibrationChargePINDiode and multiplies it with the light guides area 
//     of each pixel (MGeomPix::GetA()).  The obtained number gives the NUMBER OF PHOTONS 
//     incident on every pixels light guid OUTSIDE THE PLEXIGLASS of the camera, 
//     obtained with the PIN Diode method. 
//
// 8)  Each of the three photons numbers is divided by the mean sum of FADC counts 
//     and defined as MEASURED QUANTUM EFFICIENCY AT A GIVEN COLOUR. They are stored 
//     in the variables SetQEBlindPixel(qe, color), SetQEFFactor(qe,color) and 
//     SetQEPINDiode(qe,color) 
//
// 9)  Errors are propagated and corresponding variances get stored in 
//     SetQEBlindPixelVar(var,color), SetQEFFactorVar(var,color) and 
//     SetQEPINDiodeVar(var,color).
//
// 10) After every eventloop, MCalibrationChargeCalc calls the functions UpdateBlindPixelMethod(),
//     UpdateFFactorMethod() and UpdatePINDiodeMethod() which calculate the ratio 
//     measured QE / gkDefaultQEGreen (or gkDefaultQEBlue or gkDefaultQEUV or gkDefaultQECT1) 
//     and calculates an weighted average of these quantum-efficiency normalizations obtained
//     by one of the three methods.
//
// 11) A call to GetQECascadesBlindPixel(), GetQECascadesFFactor() or
//     GetQECascadesPINDiode() returns then the normalization multiplied with an average QE
//     folded into a cascades spectrum. This number should be dependent on zenith angle, but 
//     this feature is not yet implemented, instead a fixed number gkDefaultAverageQE is used. 
// 
// The number gkDefaultAverageQE = 0.18 +- 0.02 can be obtained in the following way: 
//
// * Transmission probability Plexiglass: 0.92
//
// * Averaged QE coated PMTs: zenith     value
//                              0.       0.237
//                             20.       0.237
//                             40.       0.236
//                             60.       0.234
// (from D.Paneque et al., NIM A 504, 2003, 109-115, see following figure with the 
//  photon spectra at 2200 m altitude:)
//
//Begin_Html
/*
<img src="images/Photon_spectrum.png">
*/
//End_Html
// 
// * PMT photoelectron collection efficiency: 0.9
// (from D.Paneque, email 14.2.2004)
//
// * Light guides efficiency: 0.94
// (from D.Paneque, email 14.2.2004)
// 
// "Concerning the light guides effiency estimation... Daniel Ferenc 
//  is preparing some work (simulations) to estimate it. Yet so far, he has 
//  been busy with other stuff, and this work is still Unfinished.
//
//  The estimation I did comes from:
//  1) Reflectivity of light guide walls is 85 % (aluminum)
//  2) At ZERO degree light incidence, 37% of the light hits such walls 
//    (0.15X37%= 5.6% of light lost)
//  3) When increasing the light incidence angle, more and more light hits 
//     the walls.
//
//  However, the loses due to larger amount of photons hitting the walls is more 
//  or less counteracted by the fact that more and more photon trajectories cross 
//  the PMT photocathode twice, increasing the effective sensitivity of the PMT.
//
//Begin_Html
/*
<img src="images/Normalized_Cherenkov_phe_spectrums_20deg_60deg_coatedPMT.png">
*/
//End_Html
//
// The plot shows the normalized spectrum of photo-electrons preceding from 
// a typical spectrum of Cherenkov photons produced by an atmospheric shower. The 
// green line is for observation zenith angles of 20 deg. and the red line for 
// 60 deg. The overall effective QE drops from about 20.8 to about 19.8. 
//
// Jurgen Gebauer did some quick measurements about this issue. I attach a 
// plot. You can see that the angular dependence is (more or less) in agreement with a 
// CosTheta function (below 20-25 degrees), which is the variation of the entrance 
// window cross section. So, in first approximation, no losses when increasing light 
// incidence angle; and therefore, the factor 0.94.
//
//Begin_Html
/*
<img src="images/JuergensMeasurementWithCosThetaCurve.png">
*/
//End_Html
//
// The Quantum efficiencies for individual colours have been taken from: 
// D. Paneque et al., A Method to enhance the sensitivity of photomultipliers 
//                    of air Cherenkov Telescopes, NIM A 504, 2003, 109-115
// (see following figure)
//
//Begin_Html
/*
<img src="images/QE_Paneque.png">
*/
//End_Html
//
// The Transmission of the Plexiglass window has been provided by Eckart and is 
// displayed in the next plot. The above red curve has to be taken since it corresponds
// to the glass type set on the camera.
//
//Begin_Html
/*
<img src="images/Transmission_Plexiglass.jpg">
*/
//End_Html
//
// See also: MJCalibration, MCalibrationChargeCalc, 
//           MCalibrationChargeCam, MCalibrationChargePix, 
//           MHCalibrationChargeCam, MHCalibrationChargePix,
//           MHCalibrationChargePINDiode, MHCalibrationChargeBlindPix
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationQEPix.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCalibrationQEPix);

using namespace std;

const Float_t MCalibrationQEPix::gkDefaultQEGreen      = 0.192;
const Float_t MCalibrationQEPix::gkDefaultQEBlue       = 0.27;
const Float_t MCalibrationQEPix::gkDefaultQEUV         = 0.285;
const Float_t MCalibrationQEPix::gkDefaultQECT1        = 0.285;
const Float_t MCalibrationQEPix::gkDefaultQEGreenErr   = 0.007;
const Float_t MCalibrationQEPix::gkDefaultQEBlueErr    = 0.01 ;
const Float_t MCalibrationQEPix::gkDefaultQEUVErr      = 0.012;
const Float_t MCalibrationQEPix::gkDefaultQECT1Err     = 0.012;
const Float_t MCalibrationQEPix::gkDefaultAverageQE    = 0.184;     
const Float_t MCalibrationQEPix::gkDefaultAverageQEErr = 0.02 ;  
const Float_t MCalibrationQEPix::gkPMTCollectionEff    = 0.90 ;
const Float_t MCalibrationQEPix::gkPMTCollectionEffErr = 0.05 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffGreen    = 0.94 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffGreenErr = 0.03 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffBlue     = 0.94 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffBlueErr  = 0.03 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffUV       = 0.94 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffUVErr    = 0.03 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffCT1      = 0.94 ;
const Float_t MCalibrationQEPix::gkLightGuidesEffCT1Err   = 0.03 ;

// --------------------------------------------------------------------------
//
// Default Constructor: 
//
// Initializes all TArrays to MCalibrationCam::gkNumPulserColors
//
// Calls:
// - Clear()
//
MCalibrationQEPix::MCalibrationQEPix(const char *name, const char *title)
    :  fAverageQE ( gkDefaultAverageQE )
{

  fName  = name  ? name  : "MCalibrationQEPix";
  fTitle = title ? title : "Container of the calibrated quantum efficiency ";

  fQEBlindPixel    .Set( MCalibrationCam::gkNumPulserColors ); 
  fQEBlindPixelVar .Set( MCalibrationCam::gkNumPulserColors );
  fQECombined      .Set( MCalibrationCam::gkNumPulserColors );    
  fQECombinedVar   .Set( MCalibrationCam::gkNumPulserColors ); 
  fQEFFactor       .Set( MCalibrationCam::gkNumPulserColors );     
  fQEFFactorVar    .Set( MCalibrationCam::gkNumPulserColors );  
  fQEPINDiode      .Set( MCalibrationCam::gkNumPulserColors );    
  fQEPINDiodeVar   .Set( MCalibrationCam::gkNumPulserColors );
  fValidFlags      .Set( MCalibrationCam::gkNumPulserColors );

  Clear();

}

// -----------------------------------------------------
//
// copy 'constructor'
//
void MCalibrationQEPix::Copy(TObject& object) const
{

  MCalibrationQEPix &pix = (MCalibrationQEPix&)object;

  MCalibrationPix::Copy(pix);
  //
  // Copy the rest of the data members
  //
  pix.fQEBlindPixel      = fQEBlindPixel;
  pix.fQEBlindPixelVar   = fQEBlindPixelVar;
  pix.fQECombined        = fQECombined;
  pix.fQECombinedVar     = fQECombinedVar;
  pix.fQEFFactor         = fQEFFactor;
  pix.fQEFFactorVar      = fQEFFactorVar;
  pix.fQEPINDiode        = fQEPINDiode;
  pix.fQEPINDiodeVar     = fQEPINDiodeVar;
                                     
  pix.fAvNormBlindPixel    = fAvNormBlindPixel;
  pix.fAvNormBlindPixelVar = fAvNormBlindPixelVar;
  pix.fAvNormCombined      = fAvNormCombined;
  pix.fAvNormCombinedVar   = fAvNormCombinedVar;
  pix.fAvNormFFactor       = fAvNormFFactor;
  pix.fAvNormFFactorVar    = fAvNormFFactorVar;
  pix.fAvNormPINDiode      = fAvNormPINDiode;
  pix.fAvNormPINDiodeVar   = fAvNormPINDiodeVar;
  pix.fAverageQE           = fAverageQE;

  pix.fValidFlags          = fValidFlags;
  pix.fAvailableFlags      = fAvailableFlags;
  
}

// ----------------------------------------------------------------------------------------------
// 
// Search all available QE's of a certain colour after the blind pixel method, 
// compare them to the default QE of that colour and 
// add up a weighted average (wav) and a sum of weights (sumw)
//
// FIXME: This has to be replaced by a decent fit the QE-spectrum!
//
void MCalibrationQEPix::AddAverageBlindPixelQEs(const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw )
{

  if (IsBlindPixelMethodValid (col))
  {
    const Float_t newavqe    =   GetQEBlindPixel      (col) / GetDefaultQE      (col) 
                               / GetLightGuidesEff    (col) / GetPMTCollectionEff(); 
    const Float_t newavqevar = ( GetQEBlindPixelRelVar(col) + GetDefaultQERelVar(col) 
                               + GetLightGuidesEffRelVar(col) + GetPMTCollectionEffRelVar()  ) 
                               * newavqe * newavqe;
    const Float_t weight     = 1./newavqevar;

    wav  += newavqe * weight;
    sumw += weight;
  }
}


// ----------------------------------------------------------------------------------------------
// 
// Search all available QE's of a certain colour after the F-Factor method, 
// compare them to the default QE of that colour and 
// add up a weighted average (wav) and a sum of weights (sumw)
//
// FIXME: This has to be replaced by a decent fit the QE-spectrum!
//
void MCalibrationQEPix::AddAverageFFactorQEs(const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw )
{

  if (IsFFactorMethodValid (col))
  {
    const Float_t newavqe    =   GetQEFFactor(col)       / GetDefaultQE (col) 
                               / GetLightGuidesEff    (col) / GetPMTCollectionEff(); 
    const Float_t newavqevar = ( GetQEFFactorRelVar(col) + GetDefaultQERelVar(col) 
                               + GetLightGuidesEffRelVar(col) + GetPMTCollectionEffRelVar()  ) 
                               * newavqe * newavqe;
    const Float_t weight     = 1./newavqevar;

    wav  += newavqe *weight;
    sumw += weight;

  }


}

// ----------------------------------------------------------------------------------------------
// 
// Search all available QE's of a certain colour after the PIN Diode method, 
// compare them to the default QE of that colour and 
// add up a weighted average (wav) and a sum of weights (sumw)
//
// FIXME: This has to be replaced by a decent fit the QE-spectrum!
//
void MCalibrationQEPix::AddAveragePINDiodeQEs(const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw )
{

  if (IsPINDiodeMethodValid (col))
  {
    const Float_t newavqe    =   GetQEPINDiode(col)       / GetDefaultQE (col) 
                               / GetLightGuidesEff    (col) / GetPMTCollectionEff(); 
    const Float_t newavqevar = ( GetQEPINDiodeRelVar(col) + GetDefaultQERelVar(col) 
                               + GetLightGuidesEffRelVar(col) + GetPMTCollectionEffRelVar()  ) 
                               * newavqe * newavqe;
    const Float_t weight     = 1./newavqevar;
    wav  += newavqe *weight;
    sumw += weight;
  }
}



// ------------------------------------------------------------------------
//
// Sets all quantum efficiencies to the gkDefaultQE*
// Sets all Variances to the square root of gkDefaultQE*Err
// Sets all flags to kFALSE
// Sets all fAvNorm-Variables to 1.;
// Sets all fAvNorm-Variances to 0.;
// 
// Calls:
// - MCalibrationPix::Clear()
//
void MCalibrationQEPix::Clear(Option_t *o)
{

  SetAverageQEBlindPixelAvailable ( kFALSE );
  SetAverageQEFFactorAvailable    ( kFALSE );
  SetAverageQECombinedAvailable   ( kFALSE );
  SetAverageQEPINDiodeAvailable   ( kFALSE );

  fQEBlindPixel    [ MCalibrationCam::kGREEN ] = gkDefaultQEGreen;  
  fQEBlindPixelVar [ MCalibrationCam::kGREEN ] = gkDefaultQEGreenErr*gkDefaultQEGreenErr;
  fQEFFactor       [ MCalibrationCam::kGREEN ] = gkDefaultQEGreen;     
  fQEFFactorVar    [ MCalibrationCam::kGREEN ] = gkDefaultQEGreenErr*gkDefaultQEGreenErr;
  fQECombined      [ MCalibrationCam::kGREEN ] = gkDefaultQEGreen;    
  fQECombinedVar   [ MCalibrationCam::kGREEN ] = gkDefaultQEGreenErr*gkDefaultQEGreenErr; 
  fQEPINDiode      [ MCalibrationCam::kGREEN ] = gkDefaultQEGreen;    
  fQEPINDiodeVar   [ MCalibrationCam::kGREEN ] = gkDefaultQEGreenErr*gkDefaultQEGreenErr; 

  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kGREEN);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kGREEN);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kGREEN);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kGREEN);

  fQEBlindPixel    [ MCalibrationCam::kBLUE ] = gkDefaultQEBlue;  
  fQEBlindPixelVar [ MCalibrationCam::kBLUE ] = gkDefaultQEBlueErr*gkDefaultQEBlueErr;
  fQEFFactor       [ MCalibrationCam::kBLUE ] = gkDefaultQEBlue;     
  fQEFFactorVar    [ MCalibrationCam::kBLUE ] = gkDefaultQEBlueErr*gkDefaultQEBlueErr;
  fQECombined      [ MCalibrationCam::kBLUE ] = gkDefaultQEBlue;    
  fQECombinedVar   [ MCalibrationCam::kBLUE ] = gkDefaultQEBlueErr*gkDefaultQEBlueErr; 
  fQEPINDiode      [ MCalibrationCam::kBLUE ] = gkDefaultQEBlue;    
  fQEPINDiodeVar   [ MCalibrationCam::kBLUE ] = gkDefaultQEBlueErr*gkDefaultQEBlueErr; 

  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kBLUE);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kBLUE);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kBLUE);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kBLUE);

  fQEBlindPixel    [ MCalibrationCam::kUV ] = gkDefaultQEUV;  
  fQEBlindPixelVar [ MCalibrationCam::kUV ] = gkDefaultQEUVErr*gkDefaultQEUVErr;
  fQEFFactor       [ MCalibrationCam::kUV ] = gkDefaultQEUV;     
  fQEFFactorVar    [ MCalibrationCam::kUV ] = gkDefaultQEUVErr*gkDefaultQEUVErr;
  fQECombined      [ MCalibrationCam::kUV ] = gkDefaultQEUV;    
  fQECombinedVar   [ MCalibrationCam::kUV ] = gkDefaultQEUVErr*gkDefaultQEUVErr; 
  fQEPINDiode      [ MCalibrationCam::kUV ] = gkDefaultQEUV;    
  fQEPINDiodeVar   [ MCalibrationCam::kUV ] = gkDefaultQEUVErr*gkDefaultQEUVErr; 

  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kUV);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kUV);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kUV);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kUV);

  fQEBlindPixel    [ MCalibrationCam::kCT1 ] = gkDefaultQECT1;  
  fQEBlindPixelVar [ MCalibrationCam::kCT1 ] = gkDefaultQECT1Err*gkDefaultQECT1Err;
  fQEFFactor       [ MCalibrationCam::kCT1 ] = gkDefaultQECT1;     
  fQEFFactorVar    [ MCalibrationCam::kCT1 ] = gkDefaultQECT1Err*gkDefaultQECT1Err;
  fQECombined      [ MCalibrationCam::kCT1 ] = gkDefaultQECT1;    
  fQECombinedVar   [ MCalibrationCam::kCT1 ] = gkDefaultQECT1Err*gkDefaultQECT1Err; 
  fQEPINDiode      [ MCalibrationCam::kCT1 ] = gkDefaultQECT1;    
  fQEPINDiodeVar   [ MCalibrationCam::kCT1 ] = gkDefaultQECT1Err*gkDefaultQECT1Err; 

  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kCT1);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kCT1);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kCT1);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kCT1);

  fAvNormBlindPixel     = -1.;     
  fAvNormBlindPixelVar  = 0.;  
  fAvNormCombined       = -1.;       
  fAvNormCombinedVar    = 0.;    
  fAvNormFFactor        = -1.;        
  fAvNormFFactorVar     = 0.;     
  fAvNormPINDiode       = -1.;       
  fAvNormPINDiodeVar    = 0.;    

  MCalibrationPix::Clear();
}


// -----------------------------------------------------------------
//
// Return the average Default QE
//
const Float_t MCalibrationQEPix::GetAverageQE() const
{
  return fAverageQE; 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the average Default QE
//
const Float_t MCalibrationQEPix::GetAverageQERelVar() const
{
  return gkDefaultAverageQEErr * gkDefaultAverageQEErr / (gkDefaultAverageQE * gkDefaultAverageQE ); 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the average normalization (Blind Pixel Method)
//
const Float_t MCalibrationQEPix::GetAvNormBlindPixelRelVar( ) const 
{
  return fAvNormBlindPixelVar / (fAvNormBlindPixel * fAvNormBlindPixel ); 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the average normalization (Combined Method)
//
const Float_t MCalibrationQEPix::GetAvNormCombinedRelVar( ) const 
{
  return fAvNormCombinedVar / (fAvNormCombined * fAvNormCombined ); 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the average normalization (F-Factor Method)
//
const Float_t MCalibrationQEPix::GetAvNormFFactorRelVar( ) const 
{
  return fAvNormFFactorVar / (fAvNormFFactor * fAvNormFFactor ); 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the average normalization (PIN Diode Method)
//
const Float_t MCalibrationQEPix::GetAvNormPINDiodeRelVar( ) const 
{
  return fAvNormPINDiodeVar / (fAvNormPINDiode * fAvNormPINDiode ); 
}

// ------------------------------------------------------------------------------
//
// Get the default Quantum efficiency for pulser colour "col"
//
Float_t MCalibrationQEPix::GetDefaultQE( const MCalibrationCam::PulserColor_t col )  const
{
  switch (col)
    {
    case MCalibrationCam::kGREEN:
      return gkDefaultQEGreen;
      break;
    case MCalibrationCam::kBLUE:
      return gkDefaultQEBlue;
      break;
    case MCalibrationCam::kUV:
      return gkDefaultQEUV;
      break;
    case MCalibrationCam::kCT1:
      return gkDefaultQECT1;
      break;
    default:
      return gkDefaultQECT1;
      break;
    }
  return -1.;
}

// ------------------------------------------------------------------------------
//
// Get the relative variance of the default Quantum efficiency for pulser colour "col"
//
Float_t MCalibrationQEPix::GetDefaultQERelVar( const MCalibrationCam::PulserColor_t col )  const
{

  switch (col)
    {
    case MCalibrationCam::kGREEN:
      return gkDefaultQEGreenErr * gkDefaultQEGreenErr / (gkDefaultQEGreen * gkDefaultQEGreen );
      break;
    case MCalibrationCam::kBLUE:
      return gkDefaultQEBlueErr  * gkDefaultQEBlueErr  / (gkDefaultQEBlue  * gkDefaultQEBlue  );
      break;
    case MCalibrationCam::kUV:
      return gkDefaultQEUVErr    * gkDefaultQEUVErr    / (gkDefaultQEUV    * gkDefaultQEUV    );
      break;
    case MCalibrationCam::kCT1:
      return gkDefaultQECT1Err   * gkDefaultQECT1Err   / (gkDefaultQECT1   * gkDefaultQECT1   );
      break;
    default: 
      return gkDefaultQECT1Err   * gkDefaultQECT1Err   / (gkDefaultQECT1   * gkDefaultQECT1   );
      break;
    }
  return -1.;
}

// ------------------------------------------------------------------------------
//
// Get the light guides efficiency depending on the pulser colour "col"
// FIXME: Lacking detailed measurement, these number are not yet available 
//        for the individual colours and therefore, only one same number is 
//        returned, namely gkLightGuidesEff
//
Float_t MCalibrationQEPix::GetLightGuidesEff( const MCalibrationCam::PulserColor_t col )  const
{
  switch (col)
    {
    case MCalibrationCam::kGREEN:
      return gkLightGuidesEffGreen;
      break;
    case MCalibrationCam::kBLUE:
      return gkLightGuidesEffBlue;
      break;
    case MCalibrationCam::kUV:
      return gkLightGuidesEffUV;
      break;
    case MCalibrationCam::kCT1:
      return gkLightGuidesEffCT1;
      break;
    default:
      return gkLightGuidesEffCT1;
      break;
    }
  return -1.;
}

// ------------------------------------------------------------------------------
//
// Get the relative variance of the light guides efficiency depending on the 
// pulser colour "col"
// FIXME: Lacking detailed measurement, these number are not yet available 
//        for the individual colours and therefore, only one same number is 
//        returned, namely gkLightGuidesEffErr^2 / gkLightGuidesEff^2
//
Float_t MCalibrationQEPix::GetLightGuidesEffRelVar( const MCalibrationCam::PulserColor_t col )  const
{

  switch (col)
    {
    case MCalibrationCam::kGREEN:
      return gkLightGuidesEffGreenErr * gkLightGuidesEffGreenErr / gkLightGuidesEffGreen / gkLightGuidesEffGreen;
      break;
    case MCalibrationCam::kBLUE:
      return gkLightGuidesEffBlueErr  * gkLightGuidesEffBlueErr / gkLightGuidesEffBlue / gkLightGuidesEffBlue;
      break;
    case MCalibrationCam::kUV:
      return gkLightGuidesEffUVErr  * gkLightGuidesEffUVErr / gkLightGuidesEffUV / gkLightGuidesEffUV;
      break;
    case MCalibrationCam::kCT1:
      return gkLightGuidesEffCT1Err * gkLightGuidesEffCT1Err / gkLightGuidesEffCT1 / gkLightGuidesEffCT1;
      break;
    default: 
      return gkLightGuidesEffCT1Err * gkLightGuidesEffCT1Err / gkLightGuidesEffCT1 / gkLightGuidesEffCT1;
      break;
    }
  return -1.;
}

// ------------------------------------------------------------------------------
//
// Get the light guides efficiency for Cherenkov spectra.
// FIXME: Lacking detailed measurement, these number are not yet available 
//        for the individual colours and therefore, only one same number is 
//        returned, namely gkLightGuidesEffBlue
//
Float_t MCalibrationQEPix::GetLightGuidesEff()  const
{
  return gkLightGuidesEffBlue;
}


// ------------------------------------------------------------------------------
//
// Get the relative variance of the light guides efficiency for Cherenkov spectra.
// FIXME: Lacking detailed measurement, these number are not yet available 
//        for the individual colours and therefore, only one same number is 
//        returned, namely gkLightGuidesEffBlueErr^2 / gkLightGuidesBlueEff^2
//
Float_t MCalibrationQEPix::GetLightGuidesEffRelVar()  const
{
  return gkLightGuidesEffBlueErr  * gkLightGuidesEffBlueErr / gkLightGuidesEffBlue / gkLightGuidesEffBlue;
}



// ------------------------------------------------------------------------------
//
// Get the calculated Quantum efficiency with the blind pixel method, 
// obtained with pulser colour "col"
//
Float_t MCalibrationQEPix::GetQEBlindPixel( const MCalibrationCam::PulserColor_t col )  const
{
  return fQEBlindPixel[col];
}

// ------------------------------------------------------------------------------
//
// Get the error on the calculated Quantum efficiency with the blind pixel method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEBlindPixelErr( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEBlindPixelVar[col] < 0.)
    return -1.;

  return TMath::Sqrt(fQEBlindPixelVar[col]);

}

// ------------------------------------------------------------------------------
//
// Get the relative variance of the calculated Quantum efficiency with the blind pixel method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
// Tests for quantum efficiency equal to  0. and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEBlindPixelRelVar( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEBlindPixelVar[col] < 0.)
    return -1.;
  if (fQEBlindPixel[col] < 0.)
      return -1.;
  return fQEBlindPixelVar[col] / ( fQEBlindPixel[col] * fQEBlindPixel[col] );

}

// ------------------------------------------------------------------------------
//
// Get the calculated Quantum efficiency with the combination of the three methods
// obtained with pulser colour "col"
//
Float_t MCalibrationQEPix::GetQECombined( const MCalibrationCam::PulserColor_t col )  const
{
  return fQECombined[col];
}


// ------------------------------------------------------------------------------
//
// Get the error on the calculated Quantum efficiency with the combination of the three methods
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQECombinedErr( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQECombinedVar[col] < 0.)
    return -1.;

  return TMath::Sqrt(fQECombinedVar[col]);

}


// ----------------------------------------------------------------------------------------
//
// Get the relative variance of the calculated Quantum efficiency with the combination of 
// the three methods, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
// Tests for quantum efficiency equal to  0. and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQECombinedRelVar( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQECombinedVar[col] < 0.)
    return -1.;
  if (fQECombined[col] < 0.)
      return -1.;
  return fQECombinedVar[col] / ( fQECombined[col] * fQECombined[col] );

}

// ------------------------------------------------------------------------------
//
// Get the calculated Quantum efficiency with the F-Factor method
// obtained with pulser colour "col"
//
Float_t MCalibrationQEPix::GetQEFFactor( const MCalibrationCam::PulserColor_t col )  const
{
  return fQEFFactor[col];
}


// ------------------------------------------------------------------------------
//
// Get the error on the calculated Quantum efficiency with the F-Factor method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEFFactorErr( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEFFactorVar[col] < 0.)
    return -1.;

  return TMath::Sqrt(fQEFFactorVar[col]);

}


// ----------------------------------------------------------------------------------------
//
// Get the relative variance of the calculated Quantum efficiency with the F-Factor method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
// Tests for quantum efficiency equal to  0. and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEFFactorRelVar( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEFFactorVar[col] < 0.)
    return -1.;
  if (fQEFFactor[col] < 0.)
      return -1.;
  return fQEFFactorVar[col] / ( fQEFFactor[col] * fQEFFactor[col] );

}

// ------------------------------------------------------------------------------
//
// Get the calculated Quantum efficiency with the PIN-Diode method
// obtained with pulser colour "col"
//
Float_t MCalibrationQEPix::GetQEPINDiode( const MCalibrationCam::PulserColor_t col )  const
{
  return fQEPINDiode[col];
}


// ------------------------------------------------------------------------------
//
// Get the error on the calculated Quantum efficiency with the PIN Diode method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEPINDiodeErr( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEPINDiodeVar[col] < 0.)
    return -1.;

  return TMath::Sqrt(fQEPINDiodeVar[col]);

}

// ----------------------------------------------------------------------------------------
//
// Get the relative variance of the calculated Quantum efficiency with the PIN Diode method, 
// obtained with pulser colour "col"
// Tests for variances smaller than 0. (e.g. if it has not yet been set) 
// and returns -1. in that case
// Tests for quantum efficiency equal to  0. and returns -1. in that case
//
Float_t MCalibrationQEPix::GetQEPINDiodeRelVar( const MCalibrationCam::PulserColor_t col )  const
{

  if (fQEPINDiodeVar[col] < 0.)
    return -1.;
  if (fQEPINDiode[col] < 0.)
      return -1.;
  return fQEPINDiodeVar[col] / ( fQEPINDiode[col] * fQEPINDiode[col] );

}

// ------------------------------------------------------------------------------
//
// Get the averaged Quantum efficiency folded over the cascade spectrum, obtained 
// with the blind pixel method and averaged over the results from the different colours.
//
Float_t MCalibrationQEPix::GetQECascadesBlindPixel()  const
{
  return fAvNormBlindPixel * GetAverageQE();
}

// ------------------------------------------------------------------------------
//
// Get the variance of the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the blind pixel method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesBlindPixelVar()  const
{
  return ( GetAvNormBlindPixelRelVar()  + GetAverageQERelVar())
    * GetQECascadesBlindPixel() * GetQECascadesBlindPixel();
}

// ------------------------------------------------------------------------------
//
// Get the error on the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the blind pixel method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesBlindPixelErr()  const
{
  const Float_t var = GetQECascadesBlindPixelVar();
  
  if (var < 0.)
    return -1.;

  return TMath::Sqrt(var);
}

// ------------------------------------------------------------------------------
//
// Get the averaged Quantum efficiency folded over the cascade spectrum, obtained 
// with the combination of the three methods and averaged over the results 
// from the different colours.
//
Float_t MCalibrationQEPix::GetQECascadesCombined() const
{
  return fAvNormCombined * GetAverageQE();
}

// ------------------------------------------------------------------------------
//
// Get the error on the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the combined method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesCombinedErr() const
{
  const Float_t var = GetQECascadesCombinedVar();
  
  if (var < 0.)
    return -1.;

  return TMath::Sqrt(var);
}

// ------------------------------------------------------------------------------
//
// Get the variance of the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the combination of the three methods and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesCombinedVar() const
{
  return ( GetAvNormCombinedRelVar()  + GetAverageQERelVar())
    * GetQECascadesCombined() * GetQECascadesCombined();
}

// ------------------------------------------------------------------------------
//
// Get the averaged Quantum efficiency folded over the cascade spectrum, obtained 
// with the F-Factor method and averaged over the results from the different colours.
//
Float_t MCalibrationQEPix::GetQECascadesFFactor()  const
{
  return fAvNormFFactor * GetAverageQE();
}

// ------------------------------------------------------------------------------
//
// Get the error on the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the F-Factor method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesFFactorErr()  const
{
  const Float_t var = GetQECascadesFFactorVar();
  
  if (var < 0.)
    return -1.;

  return TMath::Sqrt(var);
}

// ------------------------------------------------------------------------------
//
// Get the variance of the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the F-Factor method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesFFactorVar()  const
{
  return ( GetAvNormFFactorRelVar()  + GetAverageQERelVar())
    * GetQECascadesFFactor() * GetQECascadesFFactor();
}

// ------------------------------------------------------------------------------
//
// Get the averaged Quantum efficiency folded over the cascade spectrum, obtained 
// with the PIN Diode method and averaged over the results from the different colours.
//
Float_t MCalibrationQEPix::GetQECascadesPINDiode()  const
{
  return fAvNormPINDiode * GetAverageQE();
}

// ------------------------------------------------------------------------------
//
// Get the error on the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the PIN Diode method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesPINDiodeErr() const
{
  const Float_t var = GetQECascadesPINDiodeVar();
  
  if (var < 0.)
    return -1.;

  return TMath::Sqrt(var);
}

// ------------------------------------------------------------------------------
//
// Get the variance of the averaged Quantum efficiency folded over the cascade spectrum, 
// obtained with the PIN Diode method and averaged over the results from the 
// different colours.
//
Float_t MCalibrationQEPix::GetQECascadesPINDiodeVar() const
{
  return ( GetAvNormPINDiodeRelVar()  + GetAverageQERelVar())
    * GetQECascadesPINDiode() * GetQECascadesPINDiode();
}

// -----------------------------------------------------------------
//
// Return the overall collection efficiency of the PMT
//
Float_t MCalibrationQEPix::GetPMTCollectionEff() const 
{
  return gkPMTCollectionEff; 
}

// -----------------------------------------------------------------
//
// Return the relative variance of the collection efficiency of the PMT
//
Float_t MCalibrationQEPix::GetPMTCollectionEffRelVar() const 
{
  return gkPMTCollectionEffErr * gkPMTCollectionEffErr / gkPMTCollectionEff / gkPMTCollectionEff; 
}

// ------------------------------------------------------------------------------
//
// Test if the average QE can be obtained from the blind pixel method
//
Bool_t MCalibrationQEPix::IsAverageQEBlindPixelAvailable() const
{
  return TESTBIT(fAvailableFlags,kAverageQEBlindPixelAvailable);
}

// ------------------------------------------------------------------------------
//
// Test if the average QE can be obtained from the combination of the three methods
//
Bool_t MCalibrationQEPix::IsAverageQECombinedAvailable() const
{
  return TESTBIT(fAvailableFlags,kAverageQECombinedAvailable);
}

// ------------------------------------------------------------------------------
//
// Test if the average QE can be obtained from the F-Factor method
//
Bool_t MCalibrationQEPix::IsAverageQEFFactorAvailable() const
{
  return TESTBIT(fAvailableFlags,kAverageQEFFactorAvailable);
}

// ------------------------------------------------------------------------------
//
// Test if the average QE can be obtained from the PIN Diode method
//
Bool_t MCalibrationQEPix::IsAverageQEPINDiodeAvailable() const
{
  return TESTBIT(fAvailableFlags,kAverageQEPINDiodeAvailable);
}

// ------------------------------------------------------------------------------
//
// Test if any of the three colours has already been calibrated with the blind pixel method
//
Bool_t MCalibrationQEPix::IsBlindPixelMethodValid () const
{

  if (IsBlindPixelMethodValid (MCalibrationCam::kGREEN))
    return kTRUE;
  if (IsBlindPixelMethodValid (MCalibrationCam::kBLUE ))
    return kTRUE;
  if (IsBlindPixelMethodValid (MCalibrationCam::kUV   ))
    return kTRUE;
  if (IsBlindPixelMethodValid (MCalibrationCam::kCT1  ))
    return kTRUE;

  return kFALSE;
}

// ------------------------------------------------------------------------------
//
// Test if any of the three colours has already been calibrated with the combination 
// of the three methods
//
Bool_t MCalibrationQEPix::IsCombinedMethodValid () const
{
  if (IsCombinedMethodValid (MCalibrationCam::kGREEN))
    return kTRUE;
  if (IsCombinedMethodValid (MCalibrationCam::kBLUE ))
    return kTRUE;
  if (IsCombinedMethodValid (MCalibrationCam::kUV   ))
    return kTRUE;
  if (IsCombinedMethodValid (MCalibrationCam::kCT1  ))
    return kTRUE;

  return kFALSE;
}

// ------------------------------------------------------------------------------
//
// Test if any of the three colours has already been calibrated with the F-Factor method
//
Bool_t MCalibrationQEPix::IsFFactorMethodValid () const
{
  if (IsFFactorMethodValid (MCalibrationCam::kGREEN))
    return kTRUE;
  if (IsFFactorMethodValid (MCalibrationCam::kBLUE ))
    return kTRUE;
  if (IsFFactorMethodValid (MCalibrationCam::kUV   ))
    return kTRUE;
  if (IsFFactorMethodValid (MCalibrationCam::kCT1  ))
    return kTRUE;

  return kFALSE;
}


// ------------------------------------------------------------------------------
//
// Test if any of the three colours has already been calibrated with the PIN Diode method
//
Bool_t MCalibrationQEPix::IsPINDiodeMethodValid () const
{
  if (IsPINDiodeMethodValid (MCalibrationCam::kGREEN))
    return kTRUE;
  if (IsPINDiodeMethodValid (MCalibrationCam::kBLUE ))
    return kTRUE;
  if (IsPINDiodeMethodValid (MCalibrationCam::kUV   ))
    return kTRUE;
  if (IsPINDiodeMethodValid (MCalibrationCam::kCT1  ))
    return kTRUE;

  return kFALSE;
}

// ------------------------------------------------------------------------------
//
// Test if the colour "col" has already been calibrated with the Blind Pixel method
//
Bool_t MCalibrationQEPix::IsBlindPixelMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fValidFlags[ col ],kBlindPixelMethodValid);
}

// ------------------------------------------------------------------------------
//
// Test if the colour "col" has already been calibrated with the combination of 
// the three methods
//
Bool_t MCalibrationQEPix::IsCombinedMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fValidFlags[ col ],kCombinedMethodValid);
}

// ------------------------------------------------------------------------------
//
// Test if the colour "col" has already been calibrated with the F-Factor method
//
Bool_t MCalibrationQEPix::IsFFactorMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fValidFlags[ col ],kFFactorMethodValid);
}

// ------------------------------------------------------------------------------
//
// Test if the colour "col" has already been calibrated with the PIN Diode method
//
Bool_t MCalibrationQEPix::IsPINDiodeMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fValidFlags[ col ],kPINDiodeMethodValid);
}

// ------------------------------------------------------------------------------
//
// Set the bit Average QE Blind Pixel method available from outside (only for MC!)
//
void MCalibrationQEPix::SetAverageQEBlindPixelAvailable ( Bool_t b ) 
{
  if (b) 
    SETBIT(fAvailableFlags,kAverageQEBlindPixelAvailable);
  else
    CLRBIT(fAvailableFlags,kAverageQEBlindPixelAvailable);    
}

// ------------------------------------------------------------------------------
//
// Set the bit Average QE combination of three methods available from outside (only for MC!)
//
void MCalibrationQEPix::SetAverageQECombinedAvailable ( Bool_t b ) 
{
  if (b) 
    SETBIT(fAvailableFlags,kAverageQECombinedAvailable);
  else
    CLRBIT(fAvailableFlags,kAverageQECombinedAvailable);    
}

// ------------------------------------------------------------------------------
//
// Set the bit Average QE F-Factor method available from outside (only for MC!)
//
void MCalibrationQEPix::SetAverageQEFFactorAvailable ( Bool_t b ) 
{
  if (b) 
    SETBIT(fAvailableFlags,kAverageQEFFactorAvailable);
  else
    CLRBIT(fAvailableFlags,kAverageQEFFactorAvailable);    
}

// ------------------------------------------------------------------------------
//
// Set the bit Average QE PIN Diode method available from outside (only for MC!)
//
void MCalibrationQEPix::SetAverageQEPINDiodeAvailable ( Bool_t b ) 
{
  if (b) 
    SETBIT(fAvailableFlags,kAverageQEPINDiodeAvailable);
  else
    CLRBIT(fAvailableFlags,kAverageQEPINDiodeAvailable);    
}

// ------------------------------------------------------------------------------
//
// Set the bit QE Blind Pixel method available from colour "col"
//
void MCalibrationQEPix::SetBlindPixelMethodValid ( Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fValidFlags[ col ],kBlindPixelMethodValid);
  else
    CLRBIT(fValidFlags[ col ],kBlindPixelMethodValid);    
}

// ------------------------------------------------------------------------------
//
// Set the bit QE Combination of three methods available from colour "col"
//
void MCalibrationQEPix::SetCombinedMethodValid ( Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fValidFlags[ col ],kCombinedMethodValid);
  else
    CLRBIT(fValidFlags[ col ],kCombinedMethodValid);    
}

// ------------------------------------------------------------------------------
//
// Set the bit QE F-Factor method available from colour "col"
//
void MCalibrationQEPix::SetFFactorMethodValid ( Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fValidFlags[ col ],kFFactorMethodValid);
  else
    CLRBIT(fValidFlags[ col ],kFFactorMethodValid);    
}

// ------------------------------------------------------------------------------
//
// Set the bit QE PIN Diode method available from colour "col"
//
void MCalibrationQEPix::SetPINDiodeMethodValid ( Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fValidFlags[ col ],kPINDiodeMethodValid);
  else
    CLRBIT(fValidFlags[ col ],kPINDiodeMethodValid);    
}

// ------------------------------------------------------------------------------
//
// Update the Blind Pixel Method: Calculate new average QE's
//
Bool_t  MCalibrationQEPix::UpdateBlindPixelMethod( const Float_t plex )
{

  Float_t weightedav = 0.;
  Float_t sumweights = 0.;

  AddAverageBlindPixelQEs(MCalibrationCam::kGREEN, weightedav, sumweights);
  AddAverageBlindPixelQEs(MCalibrationCam::kBLUE , weightedav, sumweights);
  AddAverageBlindPixelQEs(MCalibrationCam::kUV   , weightedav, sumweights);
  AddAverageBlindPixelQEs(MCalibrationCam::kCT1  , weightedav, sumweights);

  if (weightedav == 0. || sumweights == 0.)
    return kFALSE;

  fAvNormBlindPixel     = weightedav / plex / sumweights;
  fAvNormBlindPixelVar  = 1./ sumweights;

  SetAverageQEBlindPixelAvailable();

  return kTRUE;
}

// ------------------------------------------------------------------------------
//
// Update the Combination of the three Methods: Calculate new average QE's
//
Bool_t  MCalibrationQEPix::UpdateCombinedMethod()
{
  
  fAvNormCombinedVar = 0.;
  fAvNormCombined    = 0.;
  
  if (fAvNormBlindPixel > 0. && fAvNormBlindPixelVar > 0.)
    {
      const Float_t weight = 1./fAvNormBlindPixelVar;
      fAvNormCombinedVar  += weight;
      fAvNormCombined     += fAvNormBlindPixel*weight;
    }
  
  if (fAvNormFFactor    > 0. && fAvNormFFactorVar > 0. )
    {
      const Float_t weight = 1./fAvNormFFactorVar;
      fAvNormCombinedVar  += weight;
      fAvNormCombined     += fAvNormFFactor*weight;
    }
  
  if (fAvNormPINDiode > 0. && fAvNormPINDiodeVar > 0. )
    {
      const Float_t weight = 1./fAvNormPINDiodeVar;
      fAvNormCombinedVar  += weight;
      fAvNormCombined     += fAvNormPINDiode*weight;
    }
  
  fAvNormCombined = ( fAvNormCombinedVar > 0.) ? -1. : fAvNormCombined/fAvNormCombinedVar ;
  
  if (fAvNormCombined > 0.)
    SetAverageQECombinedAvailable();

  return kTRUE;
  
}

// ------------------------------------------------------------------------------
//
// Update the F-Factor Method: Calculate new average QE's
//
Bool_t  MCalibrationQEPix::UpdateFFactorMethod( const Float_t plex )
{

  Float_t weightedav = 0.;
  Float_t sumweights = 0.;

  AddAverageFFactorQEs(MCalibrationCam::kGREEN, weightedav, sumweights);
  AddAverageFFactorQEs(MCalibrationCam::kBLUE , weightedav, sumweights);
  AddAverageFFactorQEs(MCalibrationCam::kUV   , weightedav, sumweights);
  AddAverageFFactorQEs(MCalibrationCam::kCT1  , weightedav, sumweights);
 
  if (weightedav == 0. || sumweights == 0.)
    return kFALSE;

  fAvNormFFactor     = weightedav / plex / sumweights;
  fAvNormFFactorVar  = 1./ sumweights;
  
  SetAverageQEFFactorAvailable();

  return kTRUE;
  
  
}

// ------------------------------------------------------------------------------
//
// Update the PIN Diode Method: Calculate new average QE's
//
Bool_t  MCalibrationQEPix::UpdatePINDiodeMethod()
{
  
  Float_t weightedav = 0.;
  Float_t sumweights = 0.;

  AddAveragePINDiodeQEs(MCalibrationCam::kGREEN, weightedav, sumweights);
  AddAveragePINDiodeQEs(MCalibrationCam::kBLUE , weightedav, sumweights);
  AddAveragePINDiodeQEs(MCalibrationCam::kUV   , weightedav, sumweights);
  AddAveragePINDiodeQEs(MCalibrationCam::kCT1  , weightedav, sumweights);

  if (weightedav == 0. || sumweights == 0.)
    return kFALSE;

  fAvNormPINDiode     = weightedav / sumweights;
  fAvNormPINDiodeVar  = 1./ sumweights ;

  SetAverageQEPINDiodeAvailable();

  return kTRUE;

  
}

