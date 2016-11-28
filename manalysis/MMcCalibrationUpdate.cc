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
!   Author(s): Abelardo Moralejo, 12/2003 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcCalibrationUpdate
//
//  This task looks for the information about FADC pedestals in
//  MMcFadcHeader and translates it to the pedestal mean and rms (in adc counts).
//  If not already existing in the parameter list, an MCalibrationCam object
//  is created, with the conversion factor between ADC counts and photons or 
//  photoelectrons (depending on fSignalType) is set to 1 to allow the analysis 
//  to proceed.
//
//  Then it creates and fills also the MPedPhotCam object containing the pedestal
//  mean and rms in units of photons or photoelectrons.
//
//  Input Containers:
//   MMcFadcHeader
//   MRawRunHeader
//  [MCalibrationChargeCam] (if it existed previously)
//  [MCalibrationQECam] (if it existed previously)
//
//  Output Containers:
//   MPedPhotCam
//  [MCalibrationChargeCam] (if it did not exist previously)
//  [MCalibrationQECam] (if it did not exist previously)
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcCalibrationUpdate.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MCalibrationChargePix.h"
#include "MCalibrationChargeCam.h"

#include "MCalibrationQEPix.h"
#include "MCalibrationQECam.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"
#include "MGeomCam.h"
#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MRawRunHeader.h"
#include "MMcRunHeader.hxx"
#include "MMcFadcHeader.hxx"
#include "MMcConfigRunHeader.h"
#include "MCalibrateData.h"

ClassImp(MMcCalibrationUpdate);

using namespace std;

MMcCalibrationUpdate::MMcCalibrationUpdate(const char *name, const char *title)
  :  fFillCalibrationCam(kTRUE), fOuterPixelsGainScaling(kTRUE), fAmplitude(-1.), 
     fAmplitudeOuter(-1.), fConversionHiLo(-1.), fUserLow2HiGainFactor(-1.), 
     fSignalType(MCalibrateData::kPhe)
{
    fName  = name  ? name  : "MMcCalibrationUpdate";
    fTitle = title ? title : "Write MC pedestals and conversion factors into MCalibration Container";
}

// --------------------------------------------------------------------------
//
// Make sure, that there is an MCalibrationCam Object in the Parameter List.
//
Int_t MMcCalibrationUpdate::PreProcess(MParList *pList)
{
    fCalCam = (MCalibrationChargeCam*) pList->FindObject(AddSerialNumber("MCalibrationChargeCam"));
    fQECam = (MCalibrationQECam*) pList->FindObject(AddSerialNumber("MCalibrationQECam"));

    if (!fCalCam || !fQECam)
    {
        fCalCam = (MCalibrationChargeCam*) pList->FindCreateObj(AddSerialNumber("MCalibrationChargeCam"));
        fQECam = (MCalibrationQECam*) pList->FindCreateObj(AddSerialNumber("MCalibrationQECam"));
        if (!fCalCam || !fQECam)
            return kFALSE;
    }
    else
    {
        fFillCalibrationCam = kFALSE;
        *fLog << inf << AddSerialNumber("MCalibrationChargeCam") << " and " <<
	  AddSerialNumber("MCalibrationQECam") << " already exist... " << endl;
    }

    fPedPhotCam = (MPedPhotCam*) pList->FindCreateObj(AddSerialNumber("MPedPhotCam"));
    if (!fPedPhotCam)
        return kFALSE;

    fSignalCam = (MExtractedSignalCam*) pList->FindObject(AddSerialNumber("MExtractedSignalCam"));
    if (!fSignalCam)
    {
        *fLog << err << AddSerialNumber("MExtractedSignalCam") << " not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check for the runtype.
// Search for MGeomCam and MMcFadcHeader.
// Fill the MCalibrationCam object.
//
Bool_t MMcCalibrationUpdate::ReInit(MParList *pList)
{
    const MRawRunHeader *run = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!run)
    {
        *fLog << warn << dbginf << "WARNING - cannot check file type, MRawRunHeader not found." << endl;
        return kTRUE;
    }

    //
    // If it is no MC file skip this function...
    //
    fGeom = 0;
    if (!run->IsMonteCarloRun())
    {
        *fLog << inf << "This is no MC file... skipping." << endl;
        return kTRUE;
    }

    const MMcRunHeader* mcrunh = (MMcRunHeader*) pList->FindObject(AddSerialNumber("MMcRunHeader"));
    if (!mcrunh)
    {
        *fLog << err << AddSerialNumber("MMcRunHeader") << " not found... aborting." << endl;
        return kFALSE;
    }

    if (mcrunh->IsCeres())
    {
        *fLog << inf;
        *fLog << "This is a ceres file... skipping." << endl;
        *fLog << "   MCalibrationChargeCam (calibration constants), MCalibrationQECam (avg quantum efficiency)" << endl;
        *fLog << "   and MPedPhotCam (randomly extracted pedestal) not updated." << endl;
        return kTRUE;
    }
	
    //
    // Now check the existence of all necessary containers.
    //
    fGeom = (MGeomCam*) pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fGeom)
    {
        *fLog << err << AddSerialNumber("MGeomCam") << " not found... aborting." << endl;
        return kFALSE;
    }

    fHeaderFadc = (MMcFadcHeader*)pList->FindObject(AddSerialNumber("MMcFadcHeader"));
    if (!fHeaderFadc)
    {
        *fLog << err << AddSerialNumber("MMcFadcHeader") << " not found... aborting." << endl;
        return kFALSE;
    }

    //
    // Initialize Fadc simulation parameters:
    //
    if (fAmplitude < 0)
    {
	fAmplitude = fHeaderFadc->GetAmplitud();
	if (mcrunh->GetCamVersion() > 60)
	  {
	    fAmplitudeOuter = fHeaderFadc->GetAmplitudOuter();

	    fHeaderLow2HiGainFactor = fHeaderFadc->GetLow2HighGain();

	    // The high to low gain ratio is stored in MMcFadcHeader.Low2HighGain. 
	    // However, this is just the ratio of total pulse integrals. Since the 
	    // shape of the low gain pulse is different from that of the high gain,
	    // the factor to be applied to signals extracted from low gain depends
	    // on the type of signal extractor (for instance if we extract the pulse 
	    // height, the factor is larger than Low2HighGain, because the low gain
	    // pulse shape is wider and hence lower than the high gain pulse. So the 
	    // user can set manually the value of the factor to be applied. If such 
	    // value has not been set by the user, then we takes as default Low2HighGain.

	    if (fUserLow2HiGainFactor < 0.)
	      fConversionHiLo = fHeaderLow2HiGainFactor;
	    else
	      fConversionHiLo = fUserLow2HiGainFactor;

	  }
	else // old MC files, camera  < v0.7
	  {
	    fAmplitudeOuter = fAmplitude;
	    fConversionHiLo = 10;  // dummy value
	  }

    }
    else   // Check that the following files have all the same FADC parameters as the first
    {
      if ( fabs(fHeaderFadc->GetAmplitud()-fAmplitude) > 1.e-6 )
	{
	  *fLog << err << "Parameters of MMcFadcHeader are not the same for all files... aborting." << endl;
	  return kFALSE;
	}

      if (mcrunh->GetCamVersion() > 60) // files from camera 0.7 or newer
	{
	  if( fabs(fHeaderFadc->GetAmplitudOuter()-fAmplitudeOuter) > 1.e-6  ||
	      fabs(fHeaderLow2HiGainFactor-fHeaderFadc->GetLow2HighGain()) > 1.e-6 )
	    {
	      *fLog << err << "Parameters of MMcFadcHeader are not the same for all files... aborting." << endl;
	      return kFALSE;
	    }
	}
    }

    //
    // If MCalibrationChargeCam and MCalibrationQECam already existed 
    // in the parameter list before MMcCalibrationUpdate::PreProcess was 
    // executed (from a previous calibration loop) we must not fill them, 
    // hence nothing else has to be done in ReInit:
    //
    if (!fFillCalibrationCam)
        return kTRUE;

    // Now check the light collection for inner and outer pixels to
    // calculate the ratio between the two. FIXME! Light collection
    // depends on the incidence angle of the light w.r.t. the camera
    // plane. For the moment we take the ratio for light impinging
    // perpendicular to the camera plane.
    //
    // As it happens with most containers, we look for AddSerialNumber("MMcConfigRunHeader")
    // because in the stereo option the camera simulation writes one such header 
    // per telescope.
    //
    MMcConfigRunHeader* mcconfig = (MMcConfigRunHeader*) pList->FindObject(AddSerialNumber("MMcConfigRunHeader"));
    if (!mcconfig)
    {
        *fLog << err << AddSerialNumber("MMcConfigRunHeader") << 
	  " not found... aborting." << endl;
        return kFALSE;
    }
    TArrayF innerlightcoll = mcconfig->GetLightCollectionFactor();
    TArrayF outerlightcoll = mcconfig->GetLightCollectionFactorOuter();

    // In principle outer pixels seem to have a different average light 
    // collection efficiency than outer ones. We set here the factor between
    // the two.

    fOuterPixelsLightCollection = outerlightcoll[90] / innerlightcoll[90]; 
    // (at angle = 90 deg)

    // Set now the default conversion from ADC counts to photoelectrons 
    // (in case no previous calibration existed in the parameter list).
    //
    // As default we want to have SIZE in ADC counts, or rather, in "inner pixel
    // equivalent ADC counts".
    //
    // To achieve this:
    // - In the case fSignalType==kPhot: we set the ADC to photoelectron conversion 
    // equal to the QE, which will later make the ADC to photon conversion factor 
    // (= ADC2PhotEl/QE) to be = 1, 
    //
    // - In the case fSignalType==kPhe: we set the ADC to photoelectron conversion 
    // equal to 1, since this will be applied directly to the signals...

    if (fSignalType == MCalibrateData::kPhot)
      fADC2PhElInner = MCalibrationQEPix::gkDefaultAverageQE;
    else
      fADC2PhElInner = 1.;

    //
    // Set the default ADC to "photoelectrons" conversion factor for outer 
    // pixels. One can choose not to apply the known (in MC) gain factor 
    // between inner and outer pixels, (in case fOuterPixelsGainScaling = kFALSE),
    // which may be useful for display purposes.
    // If on the contrary we apply the factor, we must take into account the 
    // different gains photoelectrons->ADC counts, given in MC by fAmplitude
    // and fAmplitudeOuter. This "default" calibration is such that a shower
    // completely contained in the inner part would have Size in ADC counts, 
    // whereas one partially in the outer part would have Size in "equivalent 
    // inner ADC counts" : the "same" shower (light density distribution) would
    // have the same Size no matter where in the camera it lies. For this we have 
    // also to set later (see below) the right QE for outer pixels, which may 
    // be different from that of inner pixels.
    //

    if (fOuterPixelsGainScaling)
      fADC2PhElOuter = fADC2PhElInner
	* (fAmplitude / fAmplitudeOuter);
    else
      fADC2PhElOuter = fADC2PhElInner;


    Int_t num = fCalCam->GetSize();

    fCalCam->SetFFactorMethodValid   ( kTRUE );
    fQECam->SetFFactorMethodValid    ( kTRUE );
    fQECam->SetBlindPixelMethodValid ( kTRUE );
    fQECam->SetCombinedMethodValid   ( kTRUE );
    fQECam->SetPINDiodeMethodValid   ( kTRUE );  
    
    for (Int_t i=0; i<num; i++)
    {
        MCalibrationChargePix &calpix = (MCalibrationChargePix&)(*fCalCam)[i];

	calpix.SetFFactorMethodValid();

	calpix.SetConversionHiLo(fConversionHiLo);
	calpix.SetConversionHiLoErr(0.);         // FIXME ?

	//
	// Write conversion factor ADC to photo-electrons (different for inner
	// and outer pixels).
	//
	Float_t adc2photel = (fGeom->GetPixRatio(i) < fGeom->GetPixRatio(0))?
	  fADC2PhElOuter : fADC2PhElInner;


	calpix.SetMeanConvFADC2Phe(adc2photel);
        calpix.SetMeanConvFADC2PheVar(0.);
        calpix.SetMeanFFactorFADC2Phot(0.); // Not used for now.

    }

    //
    // Now set the average QE for each type of pixels. Correct outer pixels
    // for different light collection efficiency.
    //
    num = fQECam->GetSize();
    for (Int_t i=0; i<num; i++)
    {
        MCalibrationQEPix &qepix = (MCalibrationQEPix&)(*fQECam)[i];

	Float_t avqe = MCalibrationQEPix::gkDefaultAverageQE;

	if (fOuterPixelsGainScaling)
	  if (fGeom->GetPixRatio(i) < fGeom->GetPixRatio(0))
	    avqe = MCalibrationQEPix::gkDefaultAverageQE*fOuterPixelsLightCollection;

	qepix.SetAvNormFFactor(1.);
	// This factor should convert the default average QE to average QE 
	// for a spectrum like that of Cherenkov light. See the documentation 
	// of MCalibrationQEPix. Here it is 1 because we calibrate using 
	// Cherenkov light.

	qepix.SetAverageQE(avqe);
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill the MCerPhotPed object
//
// This has to be done on an event by event basis because the (calibrated)
// pedestal fluctuations depend on whether, for each pixel, we are using
// the high gain or the low gain branch.
//
Int_t MMcCalibrationUpdate::Process()
{
    // This is the case it is no MC file...
    if (!fGeom)
        return kTRUE;

    const Int_t num = fCalCam->GetSize();

    for (Int_t i=0; i<num; i++)
    {
	MExtractedSignalPix &sigpix = (*fSignalCam)[i];

        const Bool_t uselo = sigpix.IsHiGainSaturated();

	//
	// ped mean and rms per pixel, in ADC counts, according to signal 
	// calculation (hi or low gain and number of integrated slices):
        //
        const Float_t pedestmean = uselo ?
	  fSignalCam->GetNumUsedLoGainFADCSlices()*fHeaderFadc->GetPedestal(i) : 
	  fSignalCam->GetNumUsedHiGainFADCSlices()*fHeaderFadc->GetPedestal(i);

	//
	// In some cases, depending on the camera simulation parameters, one can
	// have very little or no noise in the FADC. In the case the rms of  
	// pedestal is zero, the pixel will be cleaned out later in the image
	// cleaning. To avoid this problem,we set a default value of 0.01 ADC
	// counts for the RMS per slice:
	//
        const Double_t used = uselo ?
            fSignalCam->GetNumUsedLoGainFADCSlices() :
            fSignalCam->GetNumUsedHiGainFADCSlices();

        const Float_t rms0 = uselo ?
            fHeaderFadc->GetPedestalRmsLow(i) :
            fHeaderFadc->GetPedestalRmsHigh(i);

        const Float_t pedestrms = TMath::Sqrt(used) * (rms0>0 ? rms0 : 0.01);

	//
	// Write mean pedestal and pedestal rms per pixel
	// in number of photons:
	//
        const MCalibrationChargePix &calpix = (MCalibrationChargePix&)(*fCalCam)[i];
        const MCalibrationQEPix     &qepix  = (MCalibrationQEPix&)(*fQECam)[i];

        const Float_t conv = fSignalType == MCalibrateData::kPhot ?
            calpix.GetMeanConvFADC2Phe() / qepix.GetAverageQE() :
            calpix.GetMeanConvFADC2Phe();

        const Float_t hi2lo = uselo ? calpix.GetConversionHiLo() : 1;

        (*fPedPhotCam)[i].Set(conv*hi2lo*pedestmean, conv*hi2lo*pedestrms);

    }

    return kTRUE;
}

