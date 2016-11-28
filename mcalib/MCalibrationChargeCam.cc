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
!   Author(s): Markus Gaug   11/2003 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationChargeCam                                               
//                                                               
// Storage container for charge calibration results from the signal distribution 
// fits (see MHCalibrationChargeCam and MHCalibrationChargePix), the calculation 
// of reduced sigmas and number of photo-electrons (this class) and conversion 
// factors sum FADC slices to photo-electrons (see MCalibrationChargeCalc)
//
// Individual pixels have to be cast when retrieved e.g.:
// MCalibrationChargePix &avpix = (MCalibrationChargePix&)(*fChargeCam)[i]
// 
// Averaged values over one whole area index (e.g. inner or outer pixels for 
// the MAGIC camera), can be retrieved via: 
// MCalibrationChargePix &avpix = (MCalibrationChargePix&)fChargeCam->GetAverageArea(i)
//
// Averaged values over one whole camera sector can be retrieved via: 
// MCalibrationChargePix &avpix = (MCalibrationChargePix&)fChargeCam->GetAverageSector(i)
//
// Note the averageing has been done on an event-by-event basis. Resulting 
// Sigma's of the Gauss fit have been multiplied with the square root of the number 
// of involved pixels in order to make a direct comparison possible with the mean of 
// sigmas. 
//
// Final numbers of uncalibrated or unreliable pixels can be retrieved via the commands:
// GetNumUncalibrated(aidx) and GetNumUnreliable(aidx) where aidx is the area index (0 for 
// inner and 1 for outer pixels in the MAGIC camera).
//
// The following "calibration" constants are used for the calibration of each pixel 
// (see MCalibrate):
//
// - MCalibrationQEPix::GetMeanConvFADC2Phe(): The mean conversion factor from the 
//   summed FADC slices to the number of photo-electrons (in first order independent 
//   on colour and intensity)
// - MCalibrationQEPix::GetMeanFFactorFADC2Phot(): The mean F-Factor of the total  
//   readout chain dividing the signal to noise of the incoming number of photons 
//   (= sqrt(number photons)) by the signal to noise of the outgoing summed FADC slices 
//   signal (= MCalibrationPix::GetMean() / MCalibrationChargePix::GetRSigma() )
//
// The following calibration constants can be retrieved directly from this class:
//
// - GetConversionFactorFFactor    ( Int_t idx, Float_t &mean, Float_t &err, Float_t &sigma );
//
// where: 
// - idx is the pixel software ID
// - "mean" is the mean conversion constant, to be multiplied with the retrieved signal 
//   in order to get a calibrated number of PHOTONS. 
// - "err" is the pure statistical uncertainty about the MEAN
// - "sigma", if mulitplied with the square root of signal, gives the approximate sigma of the 
//   retrieved mean number of incident Cherenkov photons.
//
// Note, Conversion is ALWAYS (included the F-Factor method!) from summed FADC slices to PHOTONS.
//
// See also: MCalibrationChargePix, MCalibrationChargeCalc, MCalibrationQECam
//           MHCalibrationChargePix, MHCalibrationChargeCam              
//           MCalibrationBlindPix, MCalibrationChargePINDiode
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"

#include <TMath.h>

#include <TH1.h>
#include <TF1.h>
#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"

#include "MHCamera.h"

ClassImp(MCalibrationChargeCam);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Calls:
// - Clear()
//
MCalibrationChargeCam::MCalibrationChargeCam(const char *name, const char *title)
{
  fName  = name  ? name  : "MCalibrationChargeCam";
  fTitle = title ? title : "Storage container for the Calibration Information in the camera";
  
  Clear();
}

/*
// --------------------------------------------------------------------------
//
// Creates new MCalibrationCam only for the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//
TObject *MCalibrationChargeCam::Clone(const char *) const
{

  //
  // FIXME, this might be done faster and more elegant, by direct copy.
  //
  MCalibrationChargeCam *cam = new MCalibrationChargeCam(fName,fTitle);

  cam->fNumUnsuitable        = fNumUnsuitable;
  cam->fNumUnreliable        = fNumUnreliable;
  cam->fNumHiGainFADCSlices  = fNumHiGainFADCSlices;
  cam->fNumLoGainFADCSlices  = fNumLoGainFADCSlices;
  cam->fPulserColor          = fPulserColor;

  cam->fFlags                = fFlags;

  cam->fNumPhotonsBlindPixelMethod      = fNumPhotonsBlindPixelMethod;
  cam->fNumPhotonsFFactorMethod         = fNumPhotonsFFactorMethod;
  cam->fNumPhotonsPINDiodeMethod        = fNumPhotonsPINDiodeMethod;
  cam->fNumPhotonsBlindPixelMethodErr   = fNumPhotonsBlindPixelMethodErr;
  cam->fNumPhotonsFFactorMethodErr      = fNumPhotonsFFactorMethodErr;
  cam->fNumPhotonsPINDiodeMethodErr     = fNumPhotonsPINDiodeMethodErr;

  for (Int_t i=0; i<GetSize(); i++)
    cam->fPixels->AddAt((*this)[i].Clone(),i);
  
  for (Int_t i=0; i<GetAverageAreas(); i++)
    {
      cam->fAverageAreas->AddAt(GetAverageArea(i).Clone(),i);
      cam->fAverageBadAreas->AddAt(GetAverageBadArea(i).Clone(),i);
    }
  for (Int_t i=0; i<GetAverageSectors(); i++)
    {
      cam->fAverageSectors->AddAt(GetAverageSector(i).Clone(),i);
      cam->fAverageBadSectors->AddAt(GetAverageBadSector(i).Clone(),i);
    }

  return cam;
}
*/

// -------------------------------------------------------------------
//
// Add MCalibrationChargePix's in the ranges from - to to fPixels
//
void MCalibrationChargeCam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    {
      fPixels->AddAt(new MCalibrationChargePix,i);
      (*this)[i].SetPixId(i);
    }
}

// -------------------------------------------------------------------
//
// Add MCalibrationChargePix's in the ranges from - to to fAverageAreas
//
void MCalibrationChargeCam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    {
      fAverageAreas->AddAt(new MCalibrationChargePix,i);
      GetAverageArea(i).SetPixId(i);
    }
}

// -------------------------------------------------------------------
//
// Add MCalibrationChargePix's in the ranges from - to to fAverageSectors
//
void MCalibrationChargeCam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    {
      fAverageSectors->AddAt(new MCalibrationChargePix,i);
      GetAverageSector(i).SetPixId(i);
    }
}


// --------------------------------------
//
// Sets all variable to 0.
// Sets all flags to kFALSE
// Calls MCalibrationCam::Clear()
//
void MCalibrationChargeCam::Clear(Option_t *o)
{
  // DO NOT RESET THE HI-/LO-GAIN CONSTANTS
  SetFFactorMethodValid    ( kFALSE );

  fNumPhotonsBlindPixelMethod    = 0.;
  fNumPhotonsFFactorMethod       = 0.;
  fNumPhotonsPINDiodeMethod      = 0.;
  fNumPhotonsBlindPixelMethodErr = 0.;
  fNumPhotonsFFactorMethodErr    = 0.;
  fNumPhotonsPINDiodeMethodErr   = 0.;

  MCalibrationCam::Clear();
}

// -----------------------------------------------
//
// Sets the kFFactorMethodValid bit from outside
//
void MCalibrationChargeCam::SetFFactorMethodValid(const Bool_t b)
{
    b ? SETBIT(fFlags, kFFactorMethodValid) : CLRBIT(fFlags, kFFactorMethodValid); 
}


// --------------------------------------------------------------------------
//
// Test bit kFFactorMethodValid
//
Bool_t  MCalibrationChargeCam::IsFFactorMethodValid()   const
{
  return TESTBIT(fFlags,kFFactorMethodValid);
}

// --------------------------------------------------------------------------
//
// Copy High-gain vs. low-gain conversion factors from cam to this.
//
Bool_t MCalibrationChargeCam::MergeHiLoConversionFactors(const MCalibrationChargeCam &cam) const
{

  if (GetSize() != cam.GetSize())
    {
      *fLog << warn << "Sizes mismatch! Cannot merge high-gain vs. low-gain convertion factors" << endl;
      return kFALSE;
    }

  for (Int_t i=0; i<GetSize(); i++)
    {
      ((MCalibrationChargePix&)(*this)[i]).SetConversionHiLo   (((MCalibrationChargePix&)cam[i]).GetConversionHiLo());
      ((MCalibrationChargePix&)(*this)[i]).SetConversionHiLoErr(((MCalibrationChargePix&)cam[i]).GetConversionHiLoErr());
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print first the well fitted pixels 
// and then the ones which are not FitValid
//
void MCalibrationChargeCam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;
  
  *fLog << all << "Calibrated pixels:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationChargePix *pix;
  while ((pix=(MCalibrationChargePix*)Next()))
    {
      
      if (pix->IsFFactorMethodValid()) 
	{                            

          *fLog << all 
                << Form("%s%3i","Pixel: ",pix->GetPixId())
                << Form("%s%4.2f%s%4.2f","  Ped.Rms: ",pix->GetPedRms(),"+-",pix->GetPedRmsErr())
		<< Form("%s%4.2f%s%4.2f","  Charge: " ,pix->GetConvertedMean(),"+-",pix->GetConvertedSigma())
		<< Form("%s%4.2f%s%4.2f","  Red.Sigma: ",pix->GetConvertedRSigma(),"+-",pix->GetConvertedRSigmaErr())
		<< Form("%s%4.2f%s%4.2f","  Num.Phes: ",pix->GetPheFFactorMethod(),"+-",pix->GetPheFFactorMethodErr()) 
		<< Form("%s%4.2f%s%4.2f","  Conv.FADC2Phe: ",pix->GetMeanConvFADC2Phe(),"+-",pix->GetMeanConvFADC2PheErr())
                << " Saturated? :" << pix->IsHiGainSaturation() 
		<< Form("%s%4.2f%s%4.2f","  Conv.HiLo: ",pix->GetConversionHiLo(),"+-",pix->GetConversionHiLoErr())
		<< endl;
          id++;
	}
    }
  
  *fLog << all << id << " pixels" << endl;
  id = 0;
  
   
  *fLog << all << endl;
  *fLog << all << "Previously Excluded pixels:" << endl;
  *fLog << all << endl;
  
  id = 0;

  TIter Next4(fPixels);
  while ((pix=(MCalibrationChargePix*)Next4()))
  {
      if (pix->IsExcluded())
      {
        *fLog << all << pix->GetPixId() << " ";
        id++;

        if (!(id % 25))
          *fLog << endl;
      }
  }
  
  *fLog << all << endl;
  *fLog << all << "New Excluded pixels:" << endl;
  *fLog << all << endl;
  
  TIter Next5(fPixels);
  while ((pix=(MCalibrationChargePix*)Next5()))
  {
      if (!pix->IsFFactorMethodValid() && !pix->IsExcluded())
      {
        *fLog << all << pix->GetPixId() << " ";
        id++;

        if (!(id % 25))
          *fLog << endl;
      }
  }
  
  *fLog << endl;
  *fLog << all << id << " Excluded pixels " << endl;
  *fLog << endl;

  *fLog << all << endl;
  *fLog << all << "Averaged Areas:" << endl;
  *fLog << all << endl;

  TIter Next6(fAverageAreas);
  while ((pix=(MCalibrationChargePix*)Next6()))
  {
    *fLog << all << Form("%s%3i","Area Idx: ",pix->GetPixId())
          << "   Ped.  Rms: "            << pix->GetPedRms()        << " +- " << pix->GetPedRmsErr() 
          << "   Mean signal: "          << pix->GetMean()    << " +- " << pix->GetMeanErr() 
          << "   Sigma signal: "         << pix->GetSigma()    << " +- "<< pix->GetSigmaErr() 
          << "   Reduced Sigma: "        << pix->GetRSigma() 
          << "   Nr Phe's: "             << pix->GetPheFFactorMethod() 
          << endl;
  }

  *fLog << all << endl;
  *fLog << all << "Averaged Sectors:" << endl;
  *fLog << all << endl;

  TIter Next7(fAverageSectors);
  while ((pix=(MCalibrationChargePix*)Next7()))
  {
    *fLog << all << Form("%s%3i","Sector: ",pix->GetPixId())
          << "   Ped.  Rms: "            << pix->GetPedRms()        << " +- " << pix->GetPedRmsErr() 
          << "   Mean signal: "          << pix->GetMean()    << " +- " << pix->GetMeanErr() 
          << "   Sigma signal: "         << pix->GetSigma()    << " +- "<< pix->GetSigmaErr() 
          << "   Reduced Sigma: "        << pix->GetRSigma() 
          << "   Nr Phe's: "             << pix->GetPheFFactorMethod() 
          << endl;
  }
  *fLog << all << endl;
}


// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Charge                              (see MCalibrationPix::GetMean())
// 1: Error of fitted Charge                     (see MCalibrationPix::GetMeanErr())
// 2: Sigma of fitted Charge                     (see MCalibrationPix::GetSigma())
// 3: Error of Sigma of fitted Charge            (see MCalibrationPix::GetSigmaErr())
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Probability Gauss fit Charge distribution  (see MCalibrationPix::GetProb())
// 5: Reduced Sigma of fitted Charge             (see MCalibrationChargePix::GetRSigma())
// 6: Error Reduced Sigma of fitted Charge       (see MCalibrationChargePix::GetRSigmaErr())
// 7: Reduced Sigma per Charge                   (see MCalibrationChargePix::GetRSigmaPerCharge())
// 8: Error of Reduced Sigma per Charge          (see MCalibrationChargePix::GetRSigmaPerChargeErr())
//
// Results of the F-Factor calibration Method:
// ===========================================
//
//  9: Nr. Photo-electrons from F-Factor Method  (see MCalibrationChargePix::GetPheFFactorMethod())
// 10: Error Nr. Photo-el. from F-Factor Method  (see MCalibrationChargePix::GetPheFFactorMethodErr())
// 11: Conversion factor   from F-Factor Method  (see MCalibrationChargePix::GetMeanConvFADC2Phe()
// 12: Error conv. factor  from F-Factor Method  (see MCalibrationChargePix::GetMeanConvFADC2PheErr()
// 13: Overall F-Factor    from F-Factor Method  (see MCalibrationChargePix::GetMeanFFactorFADC2Phot()
// 14: Error F-Factor      from F-Factor Method  (see MCalibrationChargePix::GetMeanFFactorFADC2PhotErr()
// 15: Pixels valid calibration F-Factor-Method  (see MCalibrationChargePix::IsFFactorMethodValid())           
//
// Results of the Low-Gain vs. High-Gain Conversion:
// =================================================
//
// 16: Mean Signal Hi-Gain / Mean Signal Lo-Gain (see MCalibrationPix::GetHiLoMeansDivided())
// 17: Error Signal High-Gain / Signal Low Gain  (see MCalibrationPix::GetHiLoMeansDividedErr())
// 18: Sigma High-Gain / Sigma Low Gain          (see MCalibrationPix::GetHiLoSigmasDivided())
// 19: Error Sigma High-Gain / Sigma Low Gain    (see MCalibrationPix::GetHiLoSigmasDividedErr())
//                                                
// Localized defects:                             
// ==================
//
// 20: Excluded Pixels
// 21: Number of pickup   events in the Hi Gain  (see MCalibrationPix::GetHiGainNumPickup())
// 22: Number of pickup   events in the Lo Gain  (see MCalibrationPix::GetLoGainNumPickup())
// 23: Number of blackout events in the Hi Gain  (see MCalibrationPix::GetHiGainNumBlackout())
// 24: Number of blackout events in the Lo Gain  (see MCalibrationPix::GetLoGainNumBlackout())
//
// Other classifications of pixels:
// ================================
//
// 25: Pixels with saturated High-Gain           (see MCalibrationPix::IsHiGainSaturation())
//
// Calculated absolute arrival times (very low precision!):
// ========================================================
//
// 26: Absolute Arrival time of the signal       (see MCalibrationChargePix::GetAbsTimeMean())
// 27: RMS Ab.  Arrival time of the signal       (see MCalibrationChargePix::GetAbsTimeRms())
//
// Used Pedestals:
// ===============
//
// 28: Pedestal for entire signal extr. range    (see MCalibrationChargePix::Ped())
// 29: Error Pedestal entire signal extr. range  (see MCalibrationChargePix::PedErr())
// 30: Ped. RMS entire signal extraction range   (see MCalibrationChargePix::PedRms())
// 31: Error Ped. RMS entire signal extr. range  (see MCalibrationChargePix::PedRmsErr())
//
// Special variables (for data check):
// ====================================
//
// 32: HiGain RMS divided by Mean for every pixel (with inclusion of the excluded pixels)
//
Bool_t MCalibrationChargeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx > GetSize())
        return kFALSE;

    const Float_t area = cam[idx].GetA();
    if (area == 0)
        return kFALSE;

    const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[idx];
    switch (type)
    {
    case 0:
        if (pix.IsExcluded() || pix.GetConvertedMeanErr()<0)
            return kFALSE;
        val = pix.GetConvertedMean();
        return kTRUE;
    case 1:
        if (pix.IsExcluded() || pix.GetConvertedMeanErr()<0)
            return kFALSE;
        val = pix.GetConvertedMeanErr();
        return kTRUE;
    case 2:
        if (pix.IsExcluded() || pix.GetConvertedSigmaErr()<0)
            return kFALSE;
        val = pix.GetConvertedSigma();
        return kTRUE;
    case 3:
        if (pix.IsExcluded() || pix.GetConvertedSigmaErr()<0)
            return kFALSE;
        val = pix.GetConvertedSigmaErr();
        return kTRUE;
    case 4:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetProb();
        return val>=0;
    case 5:
        if (!pix.IsFFactorMethodValid())
            return kFALSE;
        if (pix.GetRSigma() == -1.)
            return kFALSE;
        val = pix.GetConvertedRSigma();
        break;
    case 6:
        if (!pix.IsFFactorMethodValid())
            return kFALSE;
        if (pix.GetRSigma() == -1.)
            return kFALSE;
        val = pix.GetConvertedRSigmaErr();
        break;
    case 7:
        if (!pix.IsFFactorMethodValid())
            return kFALSE;
        val = pix.GetRSigmaPerCharge();
        break;
    case 8:
        if (!pix.IsFFactorMethodValid())
            return kFALSE;
        val =  pix.GetRSigmaPerChargeErr();
        break;
    case 9:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded() || pix.GetPheFFactorMethodErr()<0)
            return kFALSE;
        val = pix.GetPheFFactorMethod();
        return kTRUE;
    case 10:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded() || pix.GetPheFFactorMethodErr()<=0)
            return kFALSE;
        val = pix.GetPheFFactorMethodErr();
        return kTRUE;
    case 11:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded() || pix.GetMeanConvFADC2PheErr()<0)
            return kFALSE;
        val = pix.GetMeanConvFADC2Phe();
        return kTRUE;
    case 12:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded() || pix.GetMeanConvFADC2PheErr()<=0)
            return kFALSE;
        val = pix.GetMeanConvFADC2PheErr();
        return kTRUE;
    case 13:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetMeanFFactorFADC2Phot();
        break;
    case 14:
        //      if (!pix.IsFFactorMethodValid())
        //        return kFALSE;
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetMeanFFactorFADC2PhotErr();
        if (val <= 0.)
            val = 0.00001;
        break;
    case 15:
        if (pix.IsExcluded())
            return kFALSE;
        if (!pix.IsFFactorMethodValid())
            return kFALSE;
        val = 1;
        return kTRUE;
    case 16:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiLoMeansDivided();
        break;
    case 17:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiLoMeansDividedErr();
        break;
    case 18:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiLoSigmasDivided();
        break;
    case 19:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiLoSigmasDividedErr();
        break;
    case 20:
        if (!pix.IsExcluded())
            return kFALSE;
        val = 1.;
        return kTRUE;
    case 21:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiGainNumPickup();
        break;
    case 22:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetLoGainNumPickup();
        break;
    case 23:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetHiGainNumBlackout();
        break;
    case 24:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetLoGainNumBlackout();
        break;
    case 25:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.IsHiGainSaturation();
        break;
    case 26:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetAbsTimeMean();
        break;
    case 27:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetAbsTimeRms();
        break;
    case 28:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetPed();
        break;
    case 29:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetPedErr();
        break;
    case 30:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetPedRms();
        break;
    case 31:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetPedErr()/2.;
        break;
    case 32:
        if (pix.IsExcluded() || pix.GetRms()<0)
            return kFALSE;
        val = pix.GetMean() == 0. ? 0. : pix.GetRms()/pix.GetMean();
        return kTRUE;
    case 33:
        if (pix.IsExcluded() || pix.GetRms()<0)
            return kFALSE;
        if (pix.GetMean() == 0.)
            val = 0.;
        else
            val = pix.GetSigmaErr()/pix.GetMean() + pix.GetRms()*pix.GetMeanErr()/pix.GetMean()/pix.GetMean();
        return kTRUE;
    default:
        return kFALSE;
    }

    return val!=-1.;
}

Bool_t MCalibrationChargeCam::GetConversionFactorFFactor(Int_t ipx, Float_t &mean, Float_t &ferr, Float_t &ffactor)
{

  MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[ipx];

  Float_t conv = pix.GetMeanConvFADC2Phe();

  if (conv < 0.)
    return kFALSE;

  mean    = conv;
  ferr    = pix.GetMeanConvFADC2PheErr();
  ffactor = pix.GetMeanFFactorFADC2Phot();

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Calculates the average conversion factor FADC counts to photons for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index). The MCalibrationQECam container is necessary for 
// the quantum efficiency information. 
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged conversion factors (default: -1.)
// arr[1]: Error (rms) of averaged conversion factors (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedConvFADC2PhotPerArea  ( const MGeomCam &geom, const MCalibrationQECam &qecam,
                                           const UInt_t ai,  MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  MHCamera convcam(geom,"ConvFactors","Conversion Factors;Conv Factor [phot/FADC cnts];channels");

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 

      if (bad && (*bad)[i].IsUncalibrated(MBadPixelsPix::kDeviatingNumPhes))
        continue; 

      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;
      
      const MCalibrationQEPix &qepix = (MCalibrationQEPix&)qecam[i];
      if (!qepix.IsAverageQEFFactorAvailable())
        continue;
      
      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t conv = pix.GetMeanConvFADC2Phe()/qepix.GetQECascadesFFactor();

      mean  += conv;
      mean2 += conv*conv;
      nr    ++;

      convcam.Fill(i,conv);
      convcam.SetUsed(i);
    }

  Float_t mn  = nr   ? mean/nr : -1.;
  Float_t sg  = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0.;

  const Int_t aidx = (Int_t)ai;

  TH1D *h = convcam.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",750);
  h->SetDirectory(NULL);

  TF1 *fit = NULL;

  if (geom.InheritsFrom("MGeomCamMagic"))
    {

      fit = new TF1("fit","gaus",0.4,5.);
      
      // Fix the ranges, as found by Nadia
      if(aidx == 0)
        h->Fit(fit, "REQ0", "",0.4,1.5);
      else
        h->Fit(fit ,"REQ0", "",1.,5.);
    }
  else
    {
      h->Fit("gaus","Q0");
      fit = h->GetFunction("gaus");
    }

  Float_t ci2   = fit->GetChisquare();
  Float_t sigma = fit->GetParameter(2);
  
  if (ci2 > 500. || sigma > sg)
    {
      if (geom.InheritsFrom("MGeomCamMagic"))
        {
          // Fix the ranges, as found by Nadia
          if(aidx == 0)
            h->Fit(fit, "REQ0", "",0.4,1.5);
          else
            h->Fit(fit, "REQ0", "",1.,5.);
        }
      else
        {
          h->Fit("gaus","MREQ0");
          fit = h->GetFunction("gaus");
        }
      
      ci2   = fit->GetChisquare();
      sigma = fit->GetParameter(2);
    }
  
  const Int_t ndf = fit->GetNDF();

  if (ci2 < 500. && sigma < sg && ndf > 2)
    {
      mn  = fit->GetParameter(1);
      sg  = sigma;
    }
  
  *fLog << inf << "Conversion Factors to photons area idx: " << aidx << ":" << endl;
  *fLog << inf << "Mean: " << Form("%4.3f",mn) 
        << "+-" << Form("%4.3f",fit->GetParError(1))
        << "  Sigma: " << Form("%4.3f",sg) << "+-" << Form("%4.3f",fit->GetParError(2)) 
        << "  Chisquare: " << Form("%4.3f",fit->GetChisquare()) << "  NDF  : " << ndf << endl;          
  
  delete fit;
  delete h;
  gROOT->GetListOfFunctions()->Remove(fit);

  TArrayF arr(2);
  arr[0] = mn;
  arr[1] = sg;

  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average conversion factor FADC counts to equiv. photo-electrons for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index). The MCalibrationQECam container is necessary for 
// the quantum efficiency information. 
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged conversion factors (default: -1.)
// arr[1]: Error (rms) of averaged conversion factors (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedConvFADC2PhePerArea  ( const MGeomCam &geom, const MCalibrationQECam &qecam,
                                           const UInt_t ai,  MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  MHCamera convcam(geom,"ConvFactors","Conversion Factors;Conv Factor [phe/FADC cnts];channels");

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 

      if (bad && (*bad)[i].IsUncalibrated(MBadPixelsPix::kDeviatingNumPhes))
        continue; 

      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;
      
      const MCalibrationQEPix &qepix = (MCalibrationQEPix&)qecam[i];
      if (!qepix.IsAverageQEFFactorAvailable())
        continue;
      
      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t conv = pix.GetMeanConvFADC2Phe()/qepix.GetQECascadesFFactor()*MCalibrationQEPix::gkDefaultAverageQE;

      mean  += conv;
      mean2 += conv*conv;
      nr    ++;

      convcam.Fill(i,conv);
      convcam.SetUsed(i);
    }

  Float_t mn  = nr   ? mean/nr : -1.;
  Float_t sg  = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0.;

  const Int_t aidx = (Int_t)ai;

  TH1D *h = convcam.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",750);
  h->SetDirectory(NULL);

  TF1 *fit = NULL;

  if (geom.InheritsFrom("MGeomCamMagic"))
    {

      fit = new TF1("fit","gaus",0.01,1.);
      
      // Fix the ranges, as found by Nadia
      if(aidx == 0)
        {h->Fit("fit","REQ0", "",0.07,0.3);}
      else
        {h->Fit("fit","REQ0", "",0.15,1.0);}
    }
  else
    {
      h->Fit("gaus","Q0");
      fit = h->GetFunction("gaus");
    }

  Float_t ci2   = fit->GetChisquare();
  Float_t sigma = fit->GetParameter(2);
  
  if (ci2 > 500. || sigma > sg)
    {
      if (geom.InheritsFrom("MGeomCamMagic"))
        {
          // Fix the ranges, as found by Nadia
          if(aidx == 0)
            {h->Fit("fit","REQ0", "",0.07,0.3);}
          else
            {h->Fit("fit","REQ0", "",0.15,1.0);}
        }
      else
        {
          h->Fit("gaus","MREQ0");
          fit = h->GetFunction("gaus");
        }
      
      ci2   = fit->GetChisquare();
      sigma = fit->GetParameter(2);
    }
  
  const Int_t ndf = fit->GetNDF();

  if (ci2 < 500. && sigma < sg && ndf > 2)
    {
      mn  = fit->GetParameter(1);
      sg  = sigma;
    }
  
  *fLog << inf << "Conversion Factors to equiv. photo-electrons area idx: " << aidx << ":" << endl;
  *fLog << inf << "Mean: " << Form("%4.3f",mn) 
        << "+-" << Form("%4.3f",fit->GetParError(1))
        << "  Sigma: " << Form("%4.3f",sg) << "+-" << Form("%4.3f",fit->GetParError(2)) 
        << "  Chisquare: " << Form("%4.3f",fit->GetChisquare()) << "  NDF  : " << ndf << endl;          
  
  delete fit;
  delete h;
  gROOT->GetListOfFunctions()->Remove(fit);

  TArrayF arr(2);
  arr[0] = mn;
  arr[1] = sg;

  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average conversion factor FADC counts to photons for camera sectors. 
// The geometry container is used to get the necessary
// geometry information (area index). The MCalibrationQECam container is necessary for 
// the quantum efficiency information. 
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged conversion factors (default: -1.)
// arr[1]: Error (rms) of averaged conversion factors (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedConvFADC2PhotPerSector( const MGeomCam &geom, const MCalibrationQECam &qecam,
                                             const UInt_t sec, MBadPixelsCam *bad)
{
  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t sector = geom[i].GetSector();
      
      if (sector != sec)
        continue;

      const MCalibrationQEPix &qepix = (MCalibrationQEPix&)qecam[i];
      if (!qepix.IsAverageQEFFactorAvailable())
        continue;
      
      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t conv = pix.GetMeanConvFADC2Phe();
      const Float_t qe = qepix.GetQECascadesFFactor();

      mean  += conv/qe;
      mean2 += conv*conv/qe/qe;
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}  

// --------------------------------------------------------------------------
//
// Calculates the average mean arrival times for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index). 
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged mean arrival times (default: -1.)
// arr[1]: Error (rms) of averaged mean arrival times (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedArrivalTimeMeanPerArea(const MGeomCam &geom,
                                                                 const UInt_t ai,  MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;

      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t time = pix.GetAbsTimeMean();

      mean  += time   ;
      mean2 += time*time;
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average mean arrival times for camera sectors. 
// The geometry container is used to get the necessary
// geometry information (area index).
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged mean arrival times (default: -1.)
// arr[1]: Error (rms) of averaged mean arrival times (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedArrivalTimeMeanPerSector(const MGeomCam &geom,
                                                                   const UInt_t sec, MBadPixelsCam *bad)
{
  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t sector = geom[i].GetSector();
      
      if (sector != sec)
        continue;

      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t time = pix.GetAbsTimeMean();

      mean  += time;
      mean2 += time*time;
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}  

// --------------------------------------------------------------------------
//
// Calculates the average arrival time RMSs for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index). 
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged arrival time RMSs (default: -1.)
// arr[1]: Error (rms) of averaged arrival time RMSs (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedArrivalTimeRmsPerArea  ( const MGeomCam &geom,
                                                                   const UInt_t ai,  MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;

      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t rms = pix.GetAbsTimeRms();

      mean  += rms;
      mean2 += rms*rms;
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average arrival time RMSs for camera sectors. 
// The geometry container is used to get the necessary
// geometry information (area index).
// If the bad pixel container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged arrival time RMSs (default: -1.)
// arr[1]: Error (rms) of averaged arrival time RMSs (default: 0.)
//
TArrayF MCalibrationChargeCam::GetAveragedArrivalTimeRmsPerSector( const MGeomCam &geom, const UInt_t sec, MBadPixelsCam *bad)
{
  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t sector = geom[i].GetSector();
      
      if (sector != sec)
        continue;

      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*this)[i];
      const Float_t rms = pix.GetAbsTimeRms();

      mean  += rms;
      mean2 += rms*rms;
      nr    ++;
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;

  return arr;
}  
