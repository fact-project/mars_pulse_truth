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
!   Author(s): Javier Lopez    12/2003 <mailto:jlopez@ifae.es>
!   Author(s): Javier Rico     01/2004 <mailto:jrico@ifae.es>
!   Author(s): Wolfgang Wittek 02/2004 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Markus Gaug     04/2004 <mailto:markus@ifae.es>
!   Author(s): Hendrik Bartko  08/2004 <mailto:hbartko@mppmu.mpg.de>
!   Author(s): Thomas Bretz    08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////////////
//
//   MCalibrateData
//
//   This task takes the integrated charge from MExtractedSignal and applies
//   the calibration constants from MCalibrationCam to convert the summed FADC 
//   slices into photons. The number of photons obtained is stored in MSignalCam. 
//   Optionally, the calibration of pedestals from an MPedestalCam container into 
//   an MPedPhotCam container can be chosen with the member functions 
//   SetPedestalType(). Default is 'kRun', i.e. calibration of pedestals from a 
//   dedicated pedestal run.
//   In case, the chosen pedestal type is kRun or kEvent, in ReInit() the MPedPhotCam 
//   container is filled using the information from MPedestalCam, MExtractedSignalCam, 
//   MCalibrationChargeCam and MCalibrationQECam
//
//   Selection of different calibration methods is allowed through the 
//   SetCalibrationMode() member function (default: kFfactor)
//
//   The calibration modes which exclude non-valid pixels are the following: 
//
//     kFfactor:    calibrates using the F-Factor method
//     kBlindpixel: calibrates using the BlindPixel method
//     kBlindpixel: calibrates using the BlindPixel method
//     kFlatCharge: perform a charge flat-flatfielding. Outer pixels are area-corrected.
//     kDummy:      calibrates with fixed conversion factors of 1 and errors of 0.
//
//   The calibration modes which include all pixels regardless of their validity is:
//
//     kNone:       calibrates with fixed conversion factors of 1 and errors of 0.
//
//   Use the kDummy and kNone methods ONLY FOR DEBUGGING!
//
//
//   This class can calibrate data and/or pedestals. To switch off calibration of data
//   set the Calibration Mode to kSkip. To switch on pedestal calibration call either
//     SetPedestalFlag(MCalibrateData::kRun)    (calibration is done once in ReInit)
//     SetPedestalFlag(MCalibrateData::kEvent)  (calibration is done for each event)
//
//   By calling AddPedestal() you can control the name of the
//   MPedestalCam and/or MPedPhotCam container which is used.
//
//   Assume you want to calibrate "MPedestalCam" once and "MPedestalFromLoGain" [MPedestalCam]
//   event-by-event, so:
//     MCalibrateData cal1;
//     cal1.SetCalibrationMode(MCalibrateData::kSkip);
//     cal1.SetPedestalFlag(MCalibrateData::kRun);
//     MCalibrateData cal2;
//     cal2.SetCalibrationMode(MCalibrateData::kSkip);
//     cal2.AddPedestal("FromLoGain");
//     cal2.SetPedestalFlag(MCalibrateData::kEvent);
//
//
//   Input Containers:
//    [MPedestalCam]
//    [MExtractedSignalCam]
//    [MCalibrationChargeCam]
//    [MCalibrationQECam]
//    MBadPixelsCam
//
//   Output Containers:
//    [MPedPhotCam]
//    [MSignalCam]
//
// See also: MJCalibration, MJPedestal, MJExtractSignal, MJExtractCalibTest
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrateData.h"

#include <fstream>

#include <TEnv.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MH.h"

#include "MGeomCam.h"
#include "MRawRunHeader.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"
 
#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"
 
#include "MCalibConstCam.h"
#include "MCalibConstPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MSignalCam.h"

ClassImp(MCalibrateData);

using namespace std;

const Float_t MCalibrateData::gkCalibConvMinLimit = 0.01;
const Float_t MCalibrateData::gkCalibConvMaxLimit = 5.;

const MCalibrateData::CalibrationMode_t MCalibrateData::gkDefault = kFfactor;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets all pointers to NULL
// 
// Initializes:
// - fCalibrationMode to kDefault
// - fPedestalFlag to kNo
//
MCalibrateData::MCalibrateData(CalibrationMode_t calmode,const char *name, const char *title) 
    : fGeomCam(NULL),   fBadPixels(NULL), fCalibrations(NULL),
      fQEs(NULL), fSignals(NULL), fCerPhotEvt(NULL), fCalibConstCam(NULL),
    fPedestalFlag(kNo), fSignalType(kPhot), fRenormFactor(1.), fScaleFactor(1)
{

  fName  = name  ? name  : "MCalibrateData";
  fTitle = title ? title : "Task to calculate the number of photons in one event";
  
  SetCalibrationMode(calmode);
  
  SetCalibConvMinLimit();
  SetCalibConvMaxLimit();

  fNamesPedestal.SetOwner();
}

void MCalibrateData::AddPedestal(const char *name)
{
    TString ped(name);
    TString pho(name);
    ped.Prepend("MPedestal");
    pho.Prepend("MPedPhot");

    fNamesPedestal.Add(new TNamed(ped, pho));
}

void MCalibrateData::AddPedestal(const char *pedestal, const char *pedphot)
{
    fNamesPedestal.Add(new TNamed(pedestal, pedphot));
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//
//  - MGeomCam
//  - MPedestalCam
//  - MCalibrationChargeCam
//  - MCalibrationQECam
//  - MExtractedSignalCam
//  - MBadPixelsCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MPedPhotCam
//  - MSignalCam
//
Int_t MCalibrateData::PreProcess(MParList *pList)
{
    // input containers

    fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
    {
        *fLog << err << AddSerialNumber("MBadPixelsCam") << " not found ... aborting" << endl;
        return kFALSE;
    }

    fSignals    = 0;
    fCerPhotEvt = 0;
    if (fCalibrationMode>kSkip)
    {
        fSignals = (MExtractedSignalCam*)pList->FindObject(AddSerialNumber("MExtractedSignalCam"));
        if (!fSignals)
        {
            *fLog << err << AddSerialNumber("MExtractedSignalCam") << " not found ... aborting" << endl;
            return kFALSE;
        }

        fCerPhotEvt = (MSignalCam*)pList->FindCreateObj(AddSerialNumber("MSignalCam"));
        if (!fCerPhotEvt)
            return kFALSE;

        fCalibConstCam = (MCalibConstCam*)pList->FindCreateObj(AddSerialNumber("MCalibConstCam"));
        if (!fCalibConstCam)
            return kFALSE;
    }

    fCalibrations = 0;
    fQEs          = 0;
    if (fCalibrationMode>kNone)
    {
        fCalibrations = (MCalibrationChargeCam*)pList->FindObject(AddSerialNumber("MCalibrationChargeCam"));
        if (!fCalibrations)
        {
            *fLog << err << AddSerialNumber("MCalibrationChargeCam") << " not found ... aborting." << endl;
            return kFALSE;
        }

        fQEs = (MCalibrationQECam*)pList->FindObject(AddSerialNumber("MCalibrationQECam"));
        if (!fQEs)
        {
            *fLog << err << AddSerialNumber("MCalibrationQECam") << " not found ... aborting." << endl;
            return kFALSE;
        }

    }

    if (fNamesPedestal.GetSize()>0 && fPedestalFlag==kNo)
    {
        *fLog << warn << "Pedestal list contains entries, but mode is set to kNo... setting to kEvent." << endl;
        fPedestalFlag = kEvent;
    }

    if (fPedestalFlag!=kNo)
    {
        if (fNamesPedestal.GetSize()==0)
        {
            *fLog << inf << "No container names specified... using default: MPedestalCam and MPedPhotCam." << endl;
            AddPedestal();
        }

        fPedestalCams.Clear();
        fPedPhotCams.Clear();

        TIter Next(&fNamesPedestal);
        TObject *o=0;
        while ((o=Next()))
        {
            TObject *pedcam = pList->FindObject(AddSerialNumber(o->GetName()), "MPedestalCam");
            if (!pedcam)
            {
                *fLog << err << AddSerialNumber(o->GetName()) << " [MPedestalCam] not found ... aborting" << endl;
                return kFALSE;
            }
            TObject *pedphot = pList->FindCreateObj("MPedPhotCam", AddSerialNumber(o->GetTitle()));
            if (!pedphot)
                return kFALSE;

            fPedestalCams.Add(pedcam);
            fPedPhotCams.Add(pedphot);
        }
    }

    switch (fSignalType)
      {
      case kPhe:
          //
          // Average QE for C-photons, for pixels in the inner pixel region ("area 0"),
          // used later to convert from C-photons into "equivalent phes":
          //

          // Average areas not yet initialized? use default value
          fRenormFactor = MCalibrationQEPix::gkDefaultAverageQE;
          if (fQEs->GetAverageAreas() > 0)
          {
              MCalibrationQEPix& avqepix = (MCalibrationQEPix&)(fQEs->GetAverageArea(0));
              fRenormFactor = avqepix.GetAverageQE();
          }
          break;

      case kPhot:
          fRenormFactor = 1.;
          break;
      }

    fCalibConsts.Reset();
    fCalibFFactors.Reset();
    fHiLoConv.Reset();
    fHiLoConvErr.Reset();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The ReInit searches for the following input containers:
//
//  - MGeomCam
//
// Check for validity of the selected calibration method, switch to a 
// different one in case of need
//
// Fill the MPedPhotCam container using the information from MPedestalCam,
// MExtractedSignalCam and MCalibrationCam
//
Bool_t MCalibrateData::ReInit(MParList *pList)
{
    MRawRunHeader *header = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!header)
    {
        *fLog << err << "MRawRunHeader not found... abort." << endl;
        return kFALSE;
    }

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fGeomCam)
    {
        *fLog << err << "No MGeomCam found... aborting." << endl;
        return kFALSE;
    }

    // Sizes might have changed
    if (fPedestalFlag!=kNo)
    {
        TIter Next(&fPedestalCams);
        MPedestalCam *cam=0;
        while ((cam=(MPedestalCam*)Next()))
        if ((Int_t)cam->GetSize() != fSignals->GetSize())
        {
            *fLog << err << "Size mismatch of " << cam->GetDescriptor() << " and MCalibrationCam... abort." << endl;
            return kFALSE;
        }
    }

    if(fCalibrationMode == kBlindPixel && !fQEs->IsBlindPixelMethodValid())
    {
        *fLog << warn << "Blind pixel calibration method not valid, switching to F-factor method" << endl;
        fCalibrationMode = kFfactor;
    }

    if(fCalibrationMode == kPinDiode && !fQEs->IsPINDiodeMethodValid())
    {
        *fLog << warn << "PIN diode calibration method not valid, switching to F-factor method" << endl;
        fCalibrationMode = kFfactor;
    }

    if(fCalibrationMode == kCombined && !fQEs->IsCombinedMethodValid())
    {
        *fLog << warn << "Combined calibration method not valid, switching to F-factor method" << endl;
        fCalibrationMode = kFfactor;
    }

    //
    // output information or warnings:
    //
    switch(fCalibrationMode)
    {
    case kBlindPixel:
        break;
    case kFfactor:
        break;
    case kPinDiode:
        *fLog << err << "PIN Diode Calibration mode not yet available!" << endl;
        return kFALSE;
        break;
    case kCombined:
        *fLog << err << "Combined Calibration mode not yet available!" << endl;
        return kFALSE;
        break;
    case kFlatCharge:
        *fLog << warn << "WARNING - Flat-fielding charges - only for muon calibration!" << endl;
        break;
    case kDummy:
        *fLog << warn << "WARNING - Dummy calibration, no calibration applied!" << endl;
        break;
    case kNone:
        *fLog << warn << "WARNING - No calibration applied!" << endl;
        break;
    default:
        *fLog << warn << "WARNING - Calibration mode value (" << fCalibrationMode << ") not known" << endl;
        return kFALSE;
    }

    //
    // output information or warnings:
    //
    switch (fSignalType)
    {
    case kPhe:
        *fLog << inf << "Calibrating in units of equivalent (outer/inner=4) photo-electrons." << endl;
        break;
    case kPhot:
        *fLog << inf << "Calibrating in units of photons." << endl;
        break;
    }

    if (header->IsMonteCarloRun())
    {
        if (fScaleFactor!=1)
            *fLog << warn << "WARNING - Additional scale factor for MonteCarlo run not unity but " << fScaleFactor << endl;
    }
    else
    {
        if (!fFileNameScale.IsNull())
        {
            if (gSystem->AccessPathName(fFileNameScale, kFileExists))
            {
                *fLog << err << "ERROR - Configuration file '" << fFileNameScale << "' doesn't exist... abort." << endl;
                return kFALSE;
            }

            const Int_t   p   = header->GetRunStart().GetMagicPeriod();
            const TString per = Form("%2d", p);

            TEnv rc(fFileNameScale);
            const Double_t scale = rc.GetValue(per, -1.);

            if (scale<=0)
            {
                *fLog << err << "ERROR - No valid entry for scale factor found for period " << p << " in " << fFileNameScale << "... abort." << endl;
                return kFALSE;
            }

            *fLog << inf << "New additional scale factor for period " << p << ": " << scale << endl;
            fScaleFactor = scale;
        }
        else
            *fLog << inf << "Additional scale factor set to: " << fScaleFactor << endl;
    }

    const Int_t npixels = fGeomCam->GetNumPixels();

    if (fCalibrationMode > kNone)
    {
        if (fCalibrations->GetSize() != npixels)
        {
            *fLog << "Size mismatch between MGeomCam and MCalibrationChargeCam... abort!" << endl;
            return kFALSE;
        }

        if (fBadPixels->GetSize() != npixels)
        {
            *fLog << "Size mismatch between MGeomCam and MBadPixelsCam... abort!" << endl;
            return kFALSE;
        }
    }

    fCalibConsts  .Set(npixels);
    fCalibFFactors.Set(npixels);
    fHiLoConv     .Set(npixels);
    fHiLoConvErr  .Set(npixels);
    
    if (!UpdateConversionFactors())
      return kFALSE;

    if (TestPedestalFlag(kRun))
        Calibrate(kFALSE, kTRUE);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Update the conversion factors and F-Factors from MCalibrationCams into 
// the arrays. Later, the here pre-calcualted conversion factors get simply 
// copied from memory. 
//
// This function can be called from outside in case that the MCalibrationCams 
// have been updated...
//
Bool_t MCalibrateData::UpdateConversionFactors( const MCalibrationChargeCam *updatecam)
{

    *fLog << inf << GetDescriptor() << ": Updating Conversion Factors... " << endl;
    
    fCalibConsts.Reset();
    fCalibFFactors.Reset();
    fHiLoConv.Reset();
    fHiLoConvErr.Reset();

    //
    // For the moment, we use only a dummy zenith for the calibration:
    //
    UInt_t skip = 0;

    for (UInt_t pixidx=0; pixidx<fGeomCam->GetNumPixels(); pixidx++)
      {

        Float_t hiloconv     = 1.;
        Float_t hiloconverr  = 0.;
        Float_t calibConv    = 1.;
        Float_t calibConvVar = 0.;
        Float_t calibFFactor = 0.;

        Float_t calibQE       = 1.;
        Float_t calibQEVar    = 0.;

        Float_t calibUpdate   = 1.;

        if(fCalibrationMode!=kNone)
          {
            if ((*fBadPixels)[pixidx].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
              {
                skip++;
                continue; // calibConv will remain 0
             }
            
            const MCalibrationChargePix &pix   = (MCalibrationChargePix&)(*fCalibrations)[pixidx];
            const MCalibrationChargePix &avpix = (MCalibrationChargePix&)fCalibrations->GetAverageArea(0);

            hiloconv   = pix.GetConversionHiLo     ();
            hiloconverr= pix.GetConversionHiLoSigma();

            calibConv    = pix.GetMeanConvFADC2Phe();
            calibConvVar = pix.GetMeanConvFADC2PheVar();
            calibFFactor = pix.GetMeanFFactorFADC2Phot();

            const MCalibrationQEPix &qe = (MCalibrationQEPix&)(*fQEs)[pixidx];

            if (updatecam)
              {
                const MCalibrationChargePix &upix = (MCalibrationChargePix&)(*updatecam)[pixidx];

                //
                // Correct for the possible change in amplification of the individual pixels chain
                // 
                const Float_t pixmean = upix.GetConvertedMean();
                calibUpdate = pixmean==0 ? 1 : pix.GetConvertedMean()/pixmean;

                //
                // Correct for global shifts in light emission
                // 
                const MCalibrationChargePix &ugpix = static_cast<const MCalibrationChargePix&>(updatecam->GetAverageArea(0));

                const Float_t globmean = avpix.GetConvertedMean();
                calibUpdate = globmean==0 ? 1 : ugpix.GetConvertedMean()/globmean;

                const MBadPixelsPix &ubad  = static_cast<const MBadPixelsPix&>(updatecam->GetAverageBadArea(0));
                if (ubad.IsUncalibrated(MBadPixelsPix::kChargeIsPedestal))
                  {
                    *fLog << warn << GetDescriptor() << ": Mean charge in inner pixels is smaller than 3 ped. RMS." << endl;
                    *fLog << "Maybe calibration pulses have been lost!" << endl;
                    calibUpdate = 1.;

                  }
              }
            
            switch(fCalibrationMode)
              {
              case kFlatCharge:
                {
                  calibConv    = avpix.GetConvertedMean() 
                              / (pix.GetConvertedMean() * fGeomCam->GetPixRatio(pixidx));
                  calibConvVar = (avpix.GetMeanRelVar() + pix.GetMeanRelVar()) * calibConv * calibConv;
                  if (pix.IsFFactorMethodValid())
                    {
                      const Float_t convmin1 = qe.GetQECascadesFFactor()/pix.GetMeanConvFADC2Phe();
                      if (convmin1 > 0)
                        calibFFactor *= TMath::Sqrt(convmin1);
                      else
                        calibFFactor = -1.;
                    }
                  break;
                }
              case kBlindPixel:
                if (!qe.IsBlindPixelMethodValid())
                  {
                    skip++;
                    continue;
                  }
                calibQE     = qe.GetQECascadesBlindPixel();
                calibQEVar  = qe.GetQECascadesBlindPixelVar();
                break;
                
              case kPinDiode:
                if (!qe.IsPINDiodeMethodValid())
                  {
                    skip++;
                    continue;
                  }
                calibQE     = qe.GetQECascadesPINDiode();
                calibQEVar  = qe.GetQECascadesPINDiodeVar();
                break;
                
              case kFfactor:
                if (!pix.IsFFactorMethodValid())
                  {
                    skip++;
                    continue;
                  }
                calibQE     = qe.GetQECascadesFFactor();
                calibQEVar  = qe.GetQECascadesFFactorVar();
                break;
                
              case kCombined:
                if (!qe.IsCombinedMethodValid())
                  {
                    skip++;
                    continue;
                  }
                calibQE     = qe.GetQECascadesCombined();
                calibQEVar  = qe.GetQECascadesCombinedVar();
                break;
                
              case kDummy:
                hiloconv    = 1.;
                hiloconverr = 0.;
                calibUpdate = 1.;
                break;

              default:
                  break;
              } /* switch calibration mode */
          } /* if(fCalibrationMode!=kNone) */
        else
          {
            calibConv  = 1./fGeomCam->GetPixRatio(pixidx);
          }
        
        calibConv /= calibQE;

        if (calibConv > 0.00001 && calibQE > 0.00001)
          {
            calibConvVar  = calibConvVar/(calibConv*calibConv) + calibQEVar/(calibQE*calibQE);
            calibConvVar *= (calibConv*calibConv);
          }

	calibConv    *= fRenormFactor*fScaleFactor * calibUpdate;
	calibFFactor *= TMath::Sqrt(fRenormFactor*fScaleFactor);

        fHiLoConv     [pixidx] = hiloconv;
        fHiLoConvErr  [pixidx] = hiloconverr;
        fCalibConsts  [pixidx] = calibConv;
        fCalibFFactors[pixidx] = calibFFactor;

	if (calibConv < fCalibConvMinLimit || calibConv > fCalibConvMaxLimit)
	  {
	    (*fBadPixels)[pixidx].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);  
	    calibConv    = -1.;
	    calibFFactor = -1.;
            *fLog << warn << GetDescriptor() << ": ";
            *fLog << "Conv.factor " << calibConv << " of Pixel " << pixidx << " out of range ]";
            *fLog << fCalibConvMinLimit << "," << fCalibConvMaxLimit << "[... set unsuitable. " << endl;
	  }

        MCalibConstPix &cpix  = (*fCalibConstCam)[pixidx];

        cpix.SetCalibConst(calibConv);
	cpix.SetCalibFFactor(calibFFactor);

      } /*     for (Int_t pixidx=0; pixidx<fGeomCam->GetNumPixels(); pixidx++) */

    fCalibConstCam->SetReadyToSave();

    if (skip>fGeomCam->GetNumPixels()*0.9)
    {
        *fLog << err << GetDescriptor() << ": ERROR - ";
        *fLog << "GetConversionFactor has skipped more than 90% of the " << fGeomCam->GetNumPixels() <<  " pixels... abort." << endl;
        return kFALSE;
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Apply the conversion factors and F-Factors from the arrays to the data. 
//
// The flags 'data' and 'pedestal' decide whether the signal and/or the pedetals
// shall be calibrated, respectively. 
//
Int_t MCalibrateData::Calibrate(Bool_t data, Bool_t doped) const
{
    if (!data && !doped)
        return kTRUE;

    const UInt_t  npix       = fSignals->GetSize();

    // The number of used slices are just a mean value
    // the real number might change from event to event.
    // (up to 50%!)
    const Float_t slices     = fSignals->GetNumUsedHiGainFADCSlices();
    const Float_t sqrtslices = TMath::Sqrt(slices);

    Int_t numsatlo=0;
    Int_t numsathi=0;

    for (UInt_t pixidx=0; pixidx<npix; pixidx++)
    {
        MBadPixelsPix &bpix = (*fBadPixels)[pixidx];
      
        if (data)
        {
            const MExtractedSignalPix &sig = (*fSignals)[pixidx];

            Float_t signal    = 0.;
//            Float_t signalErr = 0.;
            Bool_t  ok        = kFALSE;

            // If hi-gain is not saturated and has successfully been
            // extracted use the hi-gain arrival time
            if (!sig.IsHiGainSaturated() && sig.IsHiGainValid())
            {
                signal = sig.GetExtractedSignalHiGain();
                ok = kTRUE;
            }
            else
            {
                // Use lo-gain if it can be used
                if (sig.IsLoGainValid() && fHiLoConv[pixidx]>0.5)
                {
                    signal    = sig.GetExtractedSignalLoGain()*fHiLoConv[pixidx];
//                    signalErr = sig.GetExtractedSignalLoGain()*fHiLoConvErr[pixidx];
                    ok = kTRUE;
                }
            }

            // In this cases the signal extraction has failed.
            if (!ok)
                bpix.SetUnsuitable(MBadPixelsPix::kUnsuitableEvt);

            const Float_t nphot    = signal                         * fCalibConsts  [pixidx];
            const Float_t nphotErr = TMath::Sqrt(TMath::Abs(nphot)) * fCalibFFactors[pixidx];

            fCerPhotEvt->AddPixel(pixidx, nphot, nphotErr);

            if (!bpix.IsUnsuitable())
            {
                if (sig.IsHiGainSaturated())
                    numsathi++;

                if (sig.IsLoGainSaturated())
                    numsatlo++;
            }
        } /* if (data) */
      
        if (doped)
        {
            TIter NextPed(&fPedestalCams);
            TIter NextPhot(&fPedPhotCams);

            MPedestalCam *pedestal = 0;
            MPedPhotCam  *pedphot  = 0;
            
            const Float_t pedmeancalib = slices    *fCalibConsts[pixidx];
            const Float_t pedrmscalib  = sqrtslices*fCalibConsts[pixidx];

            while ((pedestal=(MPedestalCam*)NextPed()) &&
                   (pedphot =(MPedPhotCam*)NextPhot()))
            {
              // pedestals/(used FADC slices)   in [number of photons]
              const Float_t mean = (*pedestal)[pixidx].GetPedestal()   *pedmeancalib;
              const Float_t rms  = (*pedestal)[pixidx].GetPedestalRms()*pedrmscalib;

              (*pedphot)[pixidx].Set(mean, rms);
              pedphot->SetReadyToSave();
              //break;
            }
        } /* if (pedestal) */
    }

    // Now we should take the bias (MPedPhotExtractor/Mean) and
    // pedestal rms (MPedPhotExtractorRndm/Rms) and store it
    // into MSignalPix

    if (data)
    {
        fCerPhotEvt->SetNumPixelsSaturated(numsathi, numsatlo);
        fCerPhotEvt->SetReadyToSave();
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Apply the calibration factors to the extracted signal according to the 
// selected calibration method
//
Int_t MCalibrateData::Process()
{
    return Calibrate(fCalibrationMode!=kSkip, TestPedestalFlag(kEvent));
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MCalibrateData::StreamPrimitive(ostream &out) const
{
    out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
    out << "\"" << fName << "\", \"" << fTitle << "\");" << endl;

    if (TestPedestalFlag(kEvent))
        out << "   " << GetUniqueName() << ".EnablePedestalType(MCalibrateData::kEvent)" << endl;
    if (TestPedestalFlag(kRun))
        out << "   " << GetUniqueName() << ".EnablePedestalType(MCalibrateData::kRun)" << endl;

    if (fCalibrationMode != gkDefault)
    {
        out << "   " << GetUniqueName() << ".SetCalibrationMode(MCalibrateData::";
        switch (fCalibrationMode)
        {
        case kSkip:       out << "kSkip";               break;
        case kNone:       out << "kNone";               break;
        case kFlatCharge: out << "kFlatCharge";         break;
        case kBlindPixel: out << "kBlindPixel";         break;
        case kFfactor:    out << "kFfactor";            break;
        case kPinDiode:   out << "kPinDiode";           break;
        case kCombined:   out << "kCombined";           break;
        case kDummy:      out << "kDummy";              break;
        default:          out << (int)fCalibrationMode; break;
        }
        out << ")" << endl;
    }

    TIter Next(&fNamesPedestal);
    TObject *o=0;
    while ((o=Next()))
    {
        out << "   " << GetUniqueName() << ".AddPedestal(\"";
        out << o->GetName() << "\", \"" << o->GetTitle() << "\");" << endl;
    }
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MJPedestal.MCalibrateDate.PedestalFlag: no,run,event
//   MJPedestal.MCalibrateDate.CalibrationMode: skip,none,flatcharge,blindpixel,ffactor,pindiode,combined,dummy,default
//
Int_t MCalibrateData::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "PedestalFlag", print))
    {
        rc = kTRUE;
        TString s = GetEnvValue(env, prefix, "PedestalFlag", "");
        s.ToLower();
        if (s.BeginsWith("no"))
            SetPedestalFlag(kNo);
        if (s.BeginsWith("run"))
            SetPedestalFlag(kRun);
        if (s.BeginsWith("event"))
            SetPedestalFlag(kEvent);
    }

    if (IsEnvDefined(env, prefix, "CalibrationMode", print))
    {
        rc = kTRUE;
        TString s = GetEnvValue(env, prefix, "CalibrationMode", "");
        s.ToLower();
        if (s.BeginsWith("skip"))
            SetCalibrationMode(kSkip);
        if (s.BeginsWith("none"))
            SetCalibrationMode(kNone);
        if (s.BeginsWith("flatcharge"))
            SetCalibrationMode(kFlatCharge);
        if (s.BeginsWith("blindpixel"))
            SetCalibrationMode(kBlindPixel);
        if (s.BeginsWith("ffactor"))
            SetCalibrationMode(kFfactor);
        if (s.BeginsWith("pindiode"))
            SetCalibrationMode(kPinDiode);
        if (s.BeginsWith("combined"))
            SetCalibrationMode(kCombined);
        if (s.BeginsWith("dummy"))
            SetCalibrationMode(kDummy);
        if (s.BeginsWith("default"))
            SetCalibrationMode();
    }

    if (IsEnvDefined(env, prefix, "SignalType", print))
    {
        rc = kTRUE;
        TString s = GetEnvValue(env, prefix, "SignalType", "");
        s.ToLower();
        if (s.BeginsWith("phot"))
            SetSignalType(kPhot);
        if (s.BeginsWith("phe"))
            SetSignalType(kPhe);
        if (s.BeginsWith("default"))
            SetSignalType();
    }

    if (IsEnvDefined(env, prefix, "CalibConvMinLimit", print))
    {
        fCalibConvMinLimit = GetEnvValue(env, prefix, "CalibConvMinLimit", fCalibConvMinLimit);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "CalibConvMaxLimit", print))
    {
        fCalibConvMaxLimit = GetEnvValue(env, prefix, "CalibConvMaxLimit", fCalibConvMaxLimit);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "ScaleFactor", print))
    {
        fScaleFactor = GetEnvValue(env, prefix, "ScaleFactor", fScaleFactor);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "FileNameScale", print))
    {
        fFileNameScale = GetEnvValue(env, prefix, "FileNameScale", fFileNameScale);
        rc = kTRUE;
    }

    return rc;
}

void MCalibrateData::Print(Option_t *o) const 
{
  
  *fLog << all << GetDescriptor() << ":" << endl;
  
  for (UInt_t pixidx=0; pixidx<fGeomCam->GetNumPixels(); pixidx++)
    {
      *fLog << all
            << "Pixel: " << Form("%3i",pixidx)
            << "  CalibConst: " << Form("%4.2f",fCalibConsts[pixidx])
            << "  F-Factor: "   << Form("%4.2f",fCalibFFactors[pixidx])
            << "  HiLoConv: "   << Form("%4.2f",fHiLoConv[pixidx])
            << endl;
    }
}

