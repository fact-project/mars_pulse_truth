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
// MCalibrationQECam                                               
//                                                               
// Storage container for the calibrated Quantum Efficiency of the whole camera.
//
// For a complete description of the quantum efficiency calibration process, 
// see MCalibrationQEPix.
//
// Individual pixels have to be cast when retrieved e.g.:
// MCalibrationQEPix &avpix = (MCalibrationQEPix&)(*fQECam)[i]
// 
// Averaged values over one whole area index (e.g. inner or outer pixels for 
// the MAGIC camera), can be retrieved via: 
// MCalibrationQEPix &avpix = (MCalibrationQEPix&)fQECam->GetAverageArea(i)
//
// Averaged values over one whole camera sector can be retrieved via: 
// MCalibrationQEPix &avpix = (MCalibrationQEPix&)fQECam->GetAverageSector(i)
//
// The following "calibration" constants are used for the calibration of each pixel:
//
// - MCalibrationQEPix::GetQECascadesBlindPixel(): The mean quantum efficiency folded 
//   into a cascades spectrum obtained with the Blind Pixel Method. 
// - MCalibrationQEPix::GetQECascadesFFactor(): The mean quantum efficiency folded 
//   into a cascades spectrum obtained with the F-Factor Method
// - MCalibrationQEPix::GetQECascadesPINDiode(): The mean quantum efficiency folded 
//   into a cascades spectrum obtained with the PIN Diode Method
// - MCalibrationQEPix::GetQECascadesCombined(): The mean quantum efficiency folded 
//   into a cascades spectrum obtained with the combination of the three methods
//
// The following "calibration" constants have been measured to obtain the above values:
//
// - MCalibrationQEPix::GetQEBlindPixel( MCalibrationCam::PulserColor_t color ): 
//   The mean quantum efficiency obtained with the calibration pulser color 
//   (e.g. kGREEN, kBLUE, kUV, kCT1) after the Blind Pixel Method
// - MCalibrationQEPix::GetQEFFactor( MCalibrationCam::PulserColor_t color ): 
//   The mean quantum efficiency obtained with the calibration pulser color
//   (e.g. kGREEN, kBLUE, kUV, kCT1) after the F-Factor Method
// - MCalibrationQEPix::GetQEPINDiode( MCalibrationCam::PulserColor_t color ): 
//   The mean quantum efficiency obtained with the calibration pulser color 
//   (e.g. kGREEN, kBLUE, kUV, kCT1) after the PIN Diode Method
// - MCalibrationQEPix::GetQECombined( MCalibrationCam::PulserColor_t color ): 
//   The mean quantum efficiency obtained with the calibration pulser color 
//   (e.g. kGREEN, kBLUE, kUV, kCT1) after the combination of the three methods
//
// See also: MCalibrationQEPix, MCalibrationChargeCam, MCalibrationChargeCalc
//           MCalibrationChargeBlindPix, MCalibrationChargePINDiode, MCalibrationChargePix
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"

#include <TOrdCollection.h>
#include <TGraphErrors.h>
#include <TH2D.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCalibrationQECam);

using namespace std;

const Float_t MCalibrationQECam::gkPlexiglassQE    = 0.92;
const Float_t MCalibrationQECam::gkPlexiglassQEErr = 0.01;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationQECam::MCalibrationQECam(const char *name, const char *title)
{
    fName  = name  ? name  : "MCalibrationQECam";
    fTitle = title ? title : "Storage container for the calibrated Quantum Efficiency of the camera";

    fFlags.Set(MCalibrationCam::gkNumPulserColors);

    Clear();
}

// -------------------------------------------------------------------
//
// Add MCalibrationQEPix's in the ranges from - to to fPixels
//
void MCalibrationQECam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationQEPix,i);
}

// -------------------------------------------------------------------
//
// Add MCalibrationQEPix's in the ranges from - to to fAverageAreas
//
void MCalibrationQECam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageAreas->AddAt(new MCalibrationQEPix,i);
}

// -------------------------------------------------------------------
//
// Add MCalibrationQEPix's in the ranges from - to to fAverageSectors
//
void MCalibrationQECam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageSectors->AddAt(new MCalibrationQEPix,i);
}

// ------------------------------------------------------------------------
//
// Sets all bits to kFALSE
// 
// Calls:
// - MCalibrationCam::Clear()
//
void MCalibrationQECam::Clear(Option_t *o)
{

  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kGREEN);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kGREEN);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kGREEN);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kGREEN);
  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kBLUE);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kBLUE);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kBLUE);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kBLUE);
  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kUV);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kUV);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kUV);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kUV);
  SetBlindPixelMethodValid ( kFALSE, MCalibrationCam::kCT1);
  SetFFactorMethodValid    ( kFALSE, MCalibrationCam::kCT1);
  SetCombinedMethodValid   ( kFALSE, MCalibrationCam::kCT1);
  SetPINDiodeMethodValid   ( kFALSE, MCalibrationCam::kCT1);

  MCalibrationCam::Clear();
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MCalibrationQECam::Copy(TObject& object) const
{
  
  MCalibrationQECam &qecam = (MCalibrationQECam&)object;
  
  MCalibrationCam::Copy(qecam);
  
  qecam.fFlags        = fFlags;
  qecam.fCorningBlues = fCorningBlues;
  qecam.fCorningReds  = fCorningReds;
  
}

// --------------------------------------------------------------------
//
// Types used by MCalibrate and MCalibrateData: 
// ============================================
//
// 0: Mean Quantum Efficiency for cascades, obtained with the F-Factor method
// 1: Error of the Mean QE for cascades, obtained with the F-Factor method
// 2: Mean Quantum Efficiency for cascades, obtained with the Blind Pixel method
// 3: Error of the Mean QE for cascades, obtained with the Blind Pixel method
// 4: Mean Quantum Efficiency for cascades, obtained with the PIN Diode method
// 5: Error of the Mean QE for cascades, obtained with the PIN Diode method
// 6: Mean Quantum Efficiency for cascades, obtained with combination of the 3 methods
// 7: Error of the Mean QE for cascades, obtained with combination of the 3 methods
// 8: Availabiliy of Quantum Efficiency for cascades, F-Factor method
// 9: Availabiliy of Quantum Efficiency for cascades, F-Factor method
// 10: Availabiliy of Quantum Efficiency for cascades, F-Factor method
// 11: Availabiliy of Quantum Efficiency for cascades, F-Factor method
//
// Types filled by MCalibrationChargeCalc in combination of MCalibrationChargePix:
// ===============================================================================
//
// 12: Mean Quantum Efficiency obtained with F-Factor Method ( color: kCT1)
// 13: Error of the Mean QE obtained with F-Factor Method ( color: kCT1)
// 14: Mean Quantum Efficiency obtained with F-Factor Method ( color: kGREEN)
// 15: Error of the Mean QE obtained with F-Factor Method ( color: kGREEN)
// 16: Mean Quantum Efficiency obtained with F-Factor Method ( color: kBLUE)
// 17: Error of the Mean QE obtained with F-Factor Method ( color: kBLUE)
// 18: Mean Quantum Efficiency obtained with F-Factor Method ( color: kUV)
// 19: Error of the Mean QE obtained with F-Factor Method ( color: kUV)
//
// Types filled by MCalibrationChargeCalc in combination of MCalibrationChargeBlindPix:
// ====================================================================================
//
// 20: Mean Quantum Efficiency obtained with Blind Pixel Method ( color: kCT1)
// 21: Error of the Mean QE obtained with Blind Pixel Method ( color: kCT1)
// 22: Mean Quantum Efficiency obtained with Blind Pixel Method ( color: kGREEN)
// 23: Error of the Mean QE obtained with Blind Pixel Method ( color: kGREEN)
// 24: Mean Quantum Efficiency obtained with Blind Pixel Method ( color: kBLUE)
// 25: Error of the Mean QE obtained with Blind Pixel Method ( color: kBLUE)
// 26: Mean Quantum Efficiency obtained with Blind Pixel Method ( color: kUV)
// 27: Error of the Mean QE obtained with Blind Pixel Method ( color: kUV)
//
// Types filled by MCalibrationChargeCalc in combination of MCalibrationChargePINDiode:
// ====================================================================================
//
// 28: Mean Quantum Efficiency obtained with PIN Diode Method ( color: kCT1)
// 29: Error of the Mean QE obtained with PIN Diode Method ( color: kCT1)
// 30: Mean Quantum Efficiency obtained with PIN Diode Method ( color: kGREEN)
// 31: Error of the Mean QE obtained with PIN Diode Method ( color: kGREEN)
// 32: Mean Quantum Efficiency obtained with PIN Diode Method ( color: kBLUE)
// 33: Error of the Mean QE obtained with PIN Diode Method ( color: kBLUE)
// 34: Mean Quantum Efficiency obtained with PIN Diode Method ( color: kUV)
// 35: Error of the Mean QE obtained with PIN Diode Method ( color: kUV)
//
// Types filled by MCalibrationChargeCalc in combination of MCalibrationQEPix:
// ===========================================================================
//
// 36: Mean Quantum Efficiency obtained with combination of 3 methods ( color: kCT1)
// 37: Error of the Mean QE obtained with combination of 3 methods ( color: kCT1)
// 38: Mean Quantum Efficiency obtained with combination of 3 methods ( color: kGREEN)
// 39: Error of the Mean QE obtained with combination of 3 methods ( color: kGREEN)
// 40: Mean Quantum Efficiency obtained with combination of 3 methods ( color: kBLUE)
// 41: Error of the Mean QE obtained with combination of 3 methods ( color: kBLUE)
// 42: Mean Quantum Efficiency obtained with combination of 3 methods ( color: kUV)
// 43: Error of the Mean QE obtained with combination of 3 methods ( color: kUV)
//
Bool_t MCalibrationQECam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx > GetSize())
    return kFALSE;

  MCalibrationQEPix &pix = (MCalibrationQEPix&)(*this)[idx];

  if (pix.IsExcluded())
    return kFALSE;

  switch (type)
    {
    case 0:
      if (!pix.IsAverageQEFFactorAvailable())
        return kFALSE;
      val = pix.GetQECascadesFFactor();
      break;
    case 1:
      if (!pix.IsAverageQEFFactorAvailable())
        return kFALSE;
      val = pix.GetQECascadesFFactorErr();
      break;
    case 2:
      if (!pix.IsAverageQEBlindPixelAvailable())
        return kFALSE;
      val = pix.GetQECascadesBlindPixel();
      break;
    case 3:
      if (!pix.IsAverageQEBlindPixelAvailable())
        return kFALSE;
      val = pix.GetQECascadesBlindPixelErr();
      break;
    case 4:
      if (!pix.IsAverageQEPINDiodeAvailable())
        return kFALSE;
      val = pix.GetQECascadesPINDiode();
      break;
    case 5:
      if (!pix.IsAverageQEPINDiodeAvailable())
        return kFALSE;
      val = pix.GetQECascadesPINDiodeErr();
      break;
    case 6:
      if (!pix.IsAverageQECombinedAvailable())
        return kFALSE;
      val = pix.GetQECascadesCombined();
      break;
    case 7:
      if (!pix.IsAverageQECombinedAvailable())
        return kFALSE;
      val = pix.GetQECascadesCombinedErr();
      break;
    case 8:
      if (pix.IsAverageQEFFactorAvailable())
        val = 1;
      else
        return kFALSE;
      break;
    case 9:
      if (pix.IsAverageQEBlindPixelAvailable())
        val = 1;
      else
        return kFALSE;
      break;
    case 10:
      if (pix.IsAverageQEPINDiodeAvailable())
        val = 1;
      else
        return kFALSE;
      break;
    case 11:
      if (pix.IsAverageQECombinedAvailable())
        val = 1;
      else
        return kFALSE;
      break;
    case 12:
      val = pix.GetQEFFactor(kCT1);
      break;
    case 13:
      val = pix.GetQEFFactorErr(kCT1);
      break;
    case 14:
      val = pix.GetQEFFactor(kGREEN);
      break;
    case 15:
      val = pix.GetQEFFactorErr(kGREEN);
      break;
    case 16:
      val = pix.GetQEFFactor(kBLUE);
      break;
    case 17:
      val = pix.GetQEFFactorErr(kBLUE);
      break;
    case 18:
      val = pix.GetQEFFactor(kUV);
      break;
    case 19:
      val = pix.GetQEFFactorErr(kUV);
      break;
    case 20:
      val = pix.GetQEBlindPixel(kCT1);
      break;
    case 21:
      val = pix.GetQEBlindPixelErr(kCT1);
      break;
    case 22:
      val = pix.GetQEBlindPixel(kGREEN);
      break;
    case 23:
      val = pix.GetQEBlindPixelErr(kGREEN);
      break;
    case 24:
      val = pix.GetQEBlindPixel(kBLUE);
      break;
    case 25:
      val = pix.GetQEBlindPixelErr(kBLUE);
      break;
    case 26:
      val = pix.GetQEBlindPixel(kUV);
      break;
    case 27:
      val = pix.GetQEBlindPixelErr(kUV);
      break;
    case 28:
      val = pix.GetQEPINDiode(kCT1);
      break;
    case 29:
      val = pix.GetQEPINDiodeErr(kCT1);
      break;
    case 30:
      val = pix.GetQEPINDiode(kGREEN);
      break;
    case 31:
      val = pix.GetQEPINDiodeErr(kGREEN);
      break;
    case 32:
      val = pix.GetQEPINDiode(kBLUE);
      break;
    case 33:
      val = pix.GetQEPINDiodeErr(kBLUE);
      break;
    case 34:
      val = pix.GetQEPINDiode(kUV);
      break;
    case 35:
      val = pix.GetQEPINDiodeErr(kUV);
      break;
    case 36:
      val = pix.GetQECombined(kCT1);
      break;
    case 37:
      val = pix.GetQECombinedErr(kCT1);
      break;
    case 38:
      val = pix.GetQECombined(kGREEN);
      break;
    case 39:
      val = pix.GetQECombinedErr(kGREEN);
      break;
    case 40:
      val = pix.GetQECombined(kBLUE);
      break;
    case 41:
      val = pix.GetQECombinedErr(kBLUE);
      break;
    case 42:
      val = pix.GetQECombined(kUV);
      break;
    case 43:
      val = pix.GetQECombinedErr(kUV);
      break;
    default:
      return kFALSE;
    }
  return val!=-1.;
}

// --------------------------------------------------------------------------
//
// Return -1 if gkPlexiglassQEErr is smaller than 0.
// Return -1 if gkPlexiglassQE    is 0.
// Return gkPlexiglassQEErr^2 / gkPlexiglassQE^2
// 
Float_t MCalibrationQECam::GetPlexiglassQERelVar() const
{
  if (gkPlexiglassQEErr < 0.)
    return -1.;
  
  if (gkPlexiglassQE  == 0.)
    return -1.;
  
  return gkPlexiglassQEErr * gkPlexiglassQEErr / gkPlexiglassQE / gkPlexiglassQE ;
}


// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the four colours have the bit kBlindPixelMethodValid set,
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsBlindPixelMethodValid () const  
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

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the four colours have the bit kCombinedMethodValid set,
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsCombinedMethodValid () const
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

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the four colours have the bit kFFactorMethodValid set,
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsFFactorMethodValid () const
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


// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the four colours have the bit kPINDiodeMethodValid set,
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsPINDiodeMethodValid () const
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

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the bit kBlindPixelMethodValid is set for colour "col"
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsBlindPixelMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fFlags[ MCalibrationCam::kGREEN ],kBlindPixelMethodValid);
}

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the bit kCombinedMethodValid is set for colour "col"
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsCombinedMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fFlags[ MCalibrationCam::kGREEN ],kCombinedMethodValid);
}

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the bit kFFactorMethodValid is set for colour "col"
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsFFactorMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fFlags[ MCalibrationCam::kGREEN ],kFFactorMethodValid);
}

// --------------------------------------------------------------------------------
//
// Returns kTRUE if ANY of the bit kPINDiodeMethodValid is set for colour "col"
// otherwise kFALSE
// 
Bool_t MCalibrationQECam::IsPINDiodeMethodValid (MCalibrationCam::PulserColor_t col)  const
{
  return TESTBIT(fFlags[ MCalibrationCam::kGREEN ],kPINDiodeMethodValid);
}

// --------------------------------------------------------------------------
//
// Print the 
// - MCalibrationQEPix::GetQECascadesFFactor() 
// - MCalibrationQEPix::GetQECascadesBlindPixel() 
// - MCalibrationQEPix::GetQECascadesPINDiode() 
// - MCalibrationQEPix::GetQECascadesCombined()
// for all pixels 
//
void MCalibrationQECam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;
  
  *fLog << all << endl;
  *fLog << all << "Quantum Efficiencies averaged over cascades spectra, measured with F-Factor method:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationQEPix *pix;
  while ((pix=(MCalibrationQEPix*)Next()))
    {
      
      if (!pix->IsExcluded() && pix->IsAverageQEFFactorAvailable()) 
	{
          *fLog << all 
                << Form("%s%4i%s%4.2f%s%4.2f","Pix  ",pix->GetPixId(),
                        ":  QE: ",pix->GetQECascadesFFactor()," +- ",pix->GetQECascadesFFactorErr())
		<< endl;
          id++;
	}
    }
  
  *fLog << all << id << " succesful pixels :-))" << endl;
  id = 0;
  
  *fLog << all << endl;
  *fLog << all << "Quantum Efficiencies averaged over cascades spectra, " 
        << "measured with Blind Pixel method:" << endl;
  *fLog << all << endl;
  
  TIter Next2(fPixels);
    while ((pix=(MCalibrationQEPix*)Next2()))
      {
        
      if (!pix->IsExcluded() && pix->IsAverageQEBlindPixelAvailable()) 
	{
          *fLog << all 
                << Form("%s%4i%s%4.2f%s%4.2f","Pix  ",pix->GetPixId(),
                        ":  QE: ",pix->GetQECascadesBlindPixel()," +- ",pix->GetQECascadesBlindPixelErr())
		<< endl;
          id++;
	}
      }

  *fLog << all << id << " succesful pixels :-))" << endl;
  id = 0;
  
  *fLog << all << endl;
  *fLog << all << "Quantum Efficiencies averaged over cascades spectra, " 
        << "measured with PIN Diode method:" << endl;
  *fLog << all << endl;
  
  TIter Next3(fPixels);
    while ((pix=(MCalibrationQEPix*)Next3()))
      {
        
      if (!pix->IsExcluded() && pix->IsAverageQEPINDiodeAvailable()) 
	{
          *fLog << all 
                << Form("%s%4i%s%4.2f%s%4.2f","Pix  ",pix->GetPixId(),
                        ":  QE: ",pix->GetQECascadesPINDiode()," +- ",pix->GetQECascadesPINDiodeErr())
		<< endl;
          id++;
	}
      }

  *fLog << all << id << " succesful pixels :-))" << endl;
  id = 0;

    
  *fLog << all << endl;
  *fLog << all << "Quantum Efficiencies averaged over cascades spectra, " 
        << "measured with combination of the 3 methods:" << endl;
  *fLog << all << endl;
  
  TIter Next4(fPixels);
    while ((pix=(MCalibrationQEPix*)Next4()))
      {
        
      if (!pix->IsExcluded() && pix->IsAverageQECombinedAvailable()) 
	{
          *fLog << all 
                << Form("%s%4i%s%4.2f%s%4.2f","Pix  ",pix->GetPixId(),
                        ":  QE: ",pix->GetQECascadesCombined()," +- ",pix->GetQECascadesCombinedErr())
		<< endl;
          id++;
	}
      }

  *fLog << all << id << " succesful pixels :-))" << endl;
  id = 0;

  *fLog << all << endl;
  *fLog << all << "Excluded pixels:" << endl;
  *fLog << all << endl;
  
  TIter Next5(fPixels);
  while ((pix=(MCalibrationQEPix*)Next5()))
  {
      if (pix->IsExcluded())
      {
	  *fLog << all << pix->GetPixId() << endl;
	  id++;
      }
  }
  *fLog << all << id << " Excluded pixels " << endl;
}


// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the Blind Pixel Method, 
// for all colours (kGREEN, kBLUE, kUV, kCT1)
// 
void MCalibrationQECam::SetBlindPixelMethodValid ( const Bool_t b ) 
{
  SetBlindPixelMethodValid ( b, MCalibrationCam::kGREEN);
  SetBlindPixelMethodValid ( b, MCalibrationCam::kBLUE );
  SetBlindPixelMethodValid ( b, MCalibrationCam::kUV   );
  SetBlindPixelMethodValid ( b, MCalibrationCam::kCT1  );
}

// ----------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the combination of the 3 methods
// for all colours (kGREEN, kBLUE, kUV, kCT1)
// 
void MCalibrationQECam::SetCombinedMethodValid ( const Bool_t b ) 
{
  SetCombinedMethodValid ( b, MCalibrationCam::kGREEN);
  SetCombinedMethodValid ( b, MCalibrationCam::kBLUE );
  SetCombinedMethodValid ( b, MCalibrationCam::kUV   );
  SetCombinedMethodValid ( b, MCalibrationCam::kCT1  );
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the F-Factor Method
// for all colours (kGREEN, kBLUE, kUV, kCT1)
// 
void MCalibrationQECam::SetFFactorMethodValid ( const Bool_t b ) 
{
  SetFFactorMethodValid ( b, MCalibrationCam::kGREEN);
  SetFFactorMethodValid ( b, MCalibrationCam::kBLUE );
  SetFFactorMethodValid ( b, MCalibrationCam::kUV   );
  SetFFactorMethodValid ( b, MCalibrationCam::kCT1  );
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the PIN Diode Method, 
// for all colours (kGREEN, kBLUE, kUV, kCT1)
// 
void MCalibrationQECam::SetPINDiodeMethodValid ( const Bool_t b ) 
{
  SetPINDiodeMethodValid ( b, MCalibrationCam::kGREEN);
  SetPINDiodeMethodValid ( b, MCalibrationCam::kBLUE );
  SetPINDiodeMethodValid ( b, MCalibrationCam::kUV   );
  SetPINDiodeMethodValid ( b, MCalibrationCam::kCT1  );
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the Blind Pixel Method, 
// for colour "col"
// 
void MCalibrationQECam::SetBlindPixelMethodValid ( const Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fFlags[ MCalibrationCam::kGREEN ],kBlindPixelMethodValid);
  else
    CLRBIT(fFlags[ MCalibrationCam::kGREEN ],kBlindPixelMethodValid);    
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the combination of 3 methods
// for colour "col"
// 
void MCalibrationQECam::SetCombinedMethodValid ( const Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fFlags[ MCalibrationCam::kGREEN ],kCombinedMethodValid);
  else
    CLRBIT(fFlags[ MCalibrationCam::kGREEN ],kCombinedMethodValid);    
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the F-Factor Method, 
// for colour "col"
// 
void MCalibrationQECam::SetFFactorMethodValid ( const Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fFlags[ MCalibrationCam::kGREEN ],kFFactorMethodValid);
  else
    CLRBIT(fFlags[ MCalibrationCam::kGREEN ],kFFactorMethodValid);    
}

// --------------------------------------------------------------------------
//
// Sets the validity flag (according to b) for the PIN Diode Method, 
// for colour "col"
// 
void MCalibrationQECam::SetPINDiodeMethodValid ( const Bool_t b,  MCalibrationCam::PulserColor_t col ) 
{
  if (b) 
    SETBIT(fFlags[ MCalibrationCam::kGREEN ],kPINDiodeMethodValid);
  else
    CLRBIT(fFlags[ MCalibrationCam::kGREEN ],kPINDiodeMethodValid);    
}

// --------------------------------------------------------------------------
//
// Returns a TGraphErrors correlating the corning blues with the 
// calcualted quantum efficiency of each pixel, obtained with the F-Factor 
// method.
// 
TGraphErrors *MCalibrationQECam::GetGraphQEvsCorningBlues() const
{

  const UInt_t size = GetSize();

  if (fCorningBlues.GetSize() == 0)
    {
      *fLog << warn << "Size of intialized Cornings Blue is zero, please use MCalibrationQECamMagic" << endl;
      return NULL;
    }
  
  if (fCorningBlues.GetSize() != size)
    *fLog << warn << "Sizes mismatch, cannot create Graph!! " << endl;

  TArrayD qes(size);
  TArrayD qeerrs(size);
  TArrayD corns(size);
  TArrayD cornerrs(size);
  
  Int_t cnt = 0;
  
  for (UInt_t i=0; i<size; i++)
    {
      MCalibrationQEPix &pix = (MCalibrationQEPix&)(*this)[i];
      if (pix.IsFFactorMethodValid() && fCorningBlues[i] > 0. && pix.GetQECascadesFFactorErr() > 0.)
        {
          qes   [i] = pix.GetQECascadesFFactor();
          qeerrs[i] = pix.GetQECascadesFFactorErr();
          corns [i] = fCorningBlues[i];
          cornerrs[i] = 0.05;
          cnt++;
        }
    }
  
  TGraphErrors *gr = new TGraphErrors(cnt,
                                     corns.GetArray(),qes.GetArray(),
                                      cornerrs.GetArray(),qeerrs.GetArray());
  return gr;
}

// --------------------------------------------------------------------------
//
// Returns a TGraphErrors correlating the corning reds with the 
// calcualted quantum efficiency of each pixel, obtained with the F-Factor 
// method.
// 
TGraphErrors *MCalibrationQECam::GetGraphQEvsCorningReds() const
{

  const UInt_t size = GetSize();

  if (fCorningReds.GetSize() == 0)
    {
      *fLog << warn << "Size of intialized Cornings Red is zero, please use MCalibrationQECamMagic" << endl;
      return NULL;
    }
  
  if (fCorningReds.GetSize() != size)
    *fLog << warn << "Sizes mismatch, cannot create Graph!! " << endl;

  TArrayD qes(size);
  TArrayD qeerrs(size);
  TArrayD corns(size);
  TArrayD cornerrs(size);

  Int_t cnt = 0;

  for (UInt_t i=0; i<size; i++)
    {
      MCalibrationQEPix &pix = (MCalibrationQEPix&)(*this)[i];
      if (pix.IsFFactorMethodValid() && fCorningReds[i] > 0. && pix.GetQECascadesFFactorErr() > 0.)
        {
          qes     [i] = pix.GetQECascadesFFactor();
          qeerrs  [i] = pix.GetQECascadesFFactorErr();
          corns   [i] = fCorningReds[i];
          cornerrs[i] = 0.05;
          cnt++;
        }
      
    }
  
  TGraphErrors *gr = new TGraphErrors(cnt,
                                      corns.GetArray(),qes.GetArray(),
                                      cornerrs.GetArray(),qeerrs.GetArray());
  
  return gr;
}

TH2D *MCalibrationQECam::GetHistQEvsCorningBlues( const Int_t nbins, const Axis_t first, const Axis_t last ) const
{

  const UInt_t size = GetSize();

  if (fCorningBlues.GetSize() == 0)
    return NULL;
  
  if (fCorningBlues.GetSize() != size)
    *fLog << warn << "Sizes mismatch, cannot create Graph!! " << endl;

  TH2D *h = new TH2D("hist","QE vs. Corning Blue",nbins,first,last,nbins,0.,0.35);
  
  for (UInt_t i=0; i<size; i++)
    {
      MCalibrationQEPix &pix = (MCalibrationQEPix&)(*this)[i];
      if (pix.IsFFactorMethodValid() && fCorningBlues[i] > 0.)
        h->Fill(fCorningBlues[i],pix.GetQECascadesFFactor());
    }
  
  return h;
}

TH2D *MCalibrationQECam::GetHistQEvsCorningReds( const Int_t nbins, const Axis_t first, const Axis_t last ) const
{

  const UInt_t size = GetSize();

  if (fCorningReds.GetSize() == 0)
    return NULL;
  
  if (fCorningReds.GetSize() != size)
    *fLog << warn << "Sizes mismatch, cannot create Graph!! " << endl;

  TH2D *h = new TH2D("hist","QE vs. Corning Red",nbins,first,last,nbins,0.,0.35);

  for (UInt_t i=0; i<size; i++)
    {
      MCalibrationQEPix &pix = (MCalibrationQEPix&)(*this)[i];
      if (pix.IsFFactorMethodValid() && fCorningReds[i] > 0.)
        h->Fill(fCorningReds[i],pix.GetQECascadesFFactor());
    }
  
  return h;
}
