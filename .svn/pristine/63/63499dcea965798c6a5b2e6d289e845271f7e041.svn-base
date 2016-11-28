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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcCalibrationCalc
//
//  Input Containers:
//   MMcConfigRunHeader
//   MRawRunHeader
//   MMcFadcHeader
//   MHillas
//   MNewImagePar
//   MMcEvt
//
//  Output Containers:
//  (containers mus exist already, they are filled with new values).
//   MCalibrationChargeCam
//   MCalibrationQECam  
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcCalibrationCalc.h"

#include <TH1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCalibrationChargePix.h"
#include "MCalibrationChargeCam.h"

#include "MCalibrationQEPix.h"
#include "MCalibrationQECam.h"

#include "MGeomCam.h"
#include "MRawRunHeader.h"
#include "MMcConfigRunHeader.h"

#include "MHillas.h"
#include "MImagePar.h"
#include "MNewImagePar.h"

#include "MMcEvt.hxx"
#include "MMcFadcHeader.hxx"

ClassImp(MMcCalibrationCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Constructor. Default value for fMinSize is 1000 ADC counts. This must be
// set in general by the user (SetMinSize), since it depends among other things
// on the signal extractor used.
//
MMcCalibrationCalc::MMcCalibrationCalc(const char *name, const char *title): fMinSize(1000)
{
    fName  = name  ? name  : "MMcCalibrationCalc";
    fTitle = title ? title : "Calculate and write conversion factors into MCalibrationChargeCam and MCalibrationQECam containers";
}

// --------------------------------------------------------------------------
//
// Check for the run type. Return kTRUE if it is a MC run or if there
// is no MC run header (old camera files) kFALSE in case of a different
// run type
//
Bool_t MMcCalibrationCalc::CheckRunType(MParList *pList) const
{
    const MRawRunHeader *run = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!run)
    {
      *fLog << warn << "Warning - cannot check file type, " << AddSerialNumber("MRawRunHeader") 
	    << " not found." << endl;
      return kTRUE;
    }

    return  run->IsMonteCarloRun();
}

// --------------------------------------------------------------------------
//
// Look for all necessary containers and create histograms
//
Int_t MMcCalibrationCalc::PreProcess(MParList *pList)
{
    fADC2PhotEl = 0;
    fPhot2PhotEl = 0;

    fCalCam = (MCalibrationChargeCam*) pList->FindObject(AddSerialNumber("MCalibrationChargeCam"));
    if (!fCalCam)
    {
        *fLog << err << AddSerialNumber("MCalibrationChargeCam") << "not found... aborting." << endl;
        return kFALSE;
    }

    fQECam = (MCalibrationQECam*) pList->FindObject(AddSerialNumber("MCalibrationQECam"));
    if (!fQECam)
    {
        *fLog << err << AddSerialNumber("MCalibrationQECam") << "not found... aborting." << endl;
        return kFALSE;
    }

    fHillas = (MHillas*) pList->FindObject(AddSerialNumber("MHillas"));
    if ( !fHillas)
    {
        *fLog << err << AddSerialNumber("MHillas") << "not found... aborting." << endl;
        return kFALSE;
    }

    fNew = (MNewImagePar*)pList->FindObject(AddSerialNumber("MNewImagePar"));
    if (!fNew)
    {
        *fLog << err << AddSerialNumber("MNewImagePar") << "not found... aborting." << endl;
        return kFALSE;
    }

    fPar = (MImagePar*)pList->FindObject(AddSerialNumber("MImagePar"));
    if (!fPar)
    {
        *fLog << err << AddSerialNumber("MImagePar") << "not found... aborting." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*) pList->FindObject(AddSerialNumber("MMcEvt"));
    if (!fMcEvt)
    {
        *fLog << err << AddSerialNumber("MMcEvt") << "not found... aborting." << endl;
        return kFALSE;
    }

    //
    // Create histograms:
    //

    fHistADC2PhotEl = new TH1F(AddSerialNumber("ADC2PhotEl"), "log10(fPhotElfromShower/fSize)", 1500, -3., 3.);
    fHistADC2PhotEl->SetXTitle("log_{10}(fPhotElfromShower / fSize) [photel/ADC count]");


    fHistPhot2PhotEl = new TH1F(AddSerialNumber("Phot2PhotEl"), "Photon conversion efficiency", 1000, 0., 1.);
    fHistPhot2PhotEl->SetXTitle("Overall photon conversion efficiency [photoelectron/photon]");


    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check for the runtype.
// Search for MGeomCam and MMcFadcHeader.
//
Bool_t MMcCalibrationCalc::ReInit(MParList *pList)
{
  //
  // If it is no MC file display error and exit
  //
  if (!CheckRunType(pList))
  {
      *fLog << err << "MMcCalibrationCalc can only be used with MC files... aborting." << endl;
      return kFALSE;
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

  for (UInt_t ipix = 0; ipix < fGeom->GetNumPixels(); ipix++)
  {
      if (fHeaderFadc->GetPedestalRmsHigh(ipix) > 0 ||
	  fHeaderFadc->GetPedestalRmsLow(ipix)  > 0 )
      {
          *fLog << err << "Trying to calibrate the data using a Camera file produced with added noise." << endl;
          *fLog << "Please use a noiseless file for calibration... aborting." << endl << endl;
	  return kFALSE;
      }
  }

  // Now check the light collection for inner and outer pixels to
  // calculate the ratio between the two. FIXME! Light collection
  // depends on the incidence angle of the light w.r.t. the camera
  // plane. For the moment we take the ratio for light impinging
  // perpendicular to the camera plane.
  //
  MMcConfigRunHeader* mcconfig = (MMcConfigRunHeader*) pList->FindObject(AddSerialNumber("MMcConfigRunHeader"));
  if (!mcconfig)
    {
      *fLog << err << AddSerialNumber("MMcConfigRunHeader") << " not found... aborting." << endl;
      return kFALSE;
    }
  const TArrayF &innerlightcoll = mcconfig->GetLightCollectionFactor();
  const TArrayF &outerlightcoll = mcconfig->GetLightCollectionFactorOuter();

  // In principle outer pixels seem to have a different average light 
  // collection efficiency than outer ones. We set here the factor between
  // the two.

  fOuterPixelsLightCollection = outerlightcoll[90] / innerlightcoll[90]; 
  // (at angle = 90 deg)


  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Obtain average ratio of photons in camera to image Size.
//
Int_t MMcCalibrationCalc::Process()
{
    //
    // Exclude events with some high-gain saturated pixel
    //
    if (fPar->GetNumSatPixelsHG()>0)
        return kTRUE;

    const Float_t size = fHillas->GetSize(); 
    const Float_t innersize = fNew->GetInnerSize();

    // Size will at this point be in ADC counts (still uncalibrated)
    //
    // Exclude events with low Size (larger fluctuations)
    //   

    if (size < fMinSize)
        return kTRUE;

    //
    // PATCH: Convert number of photoelectrons in camera to the approximate number 
    // of photoelectrons that would have been registered if all pixels had the same
    // light collection efficiency as inner ones (called here "corrected_photel").
    //
 
    const Float_t corrected_photel = (Float_t) fMcEvt->GetPhotElfromShower() /
      (fOuterPixelsLightCollection + innersize / size * (1. - fOuterPixelsLightCollection));

    fHistADC2PhotEl->Fill(TMath::Log10(corrected_photel/size));
    fHistPhot2PhotEl->Fill( corrected_photel / (Float_t) fMcEvt->GetPassPhotCone() );

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the MCalibrationCam object
//
Int_t MMcCalibrationCalc::PostProcess()
{
    const Stat_t n = fHistADC2PhotEl->GetEntries();
    if (n<1)
    {
        *fLog << err << "No events read... aborting." << endl;
        return kFALSE;
    }

    fPhot2PhotEl = fHistPhot2PhotEl->GetMean();   
    // Average quantum efficiency. For now we will set this value for all pixels, although
    // even in MC there may be differences from pixel to pixel, if the .dat file containing 
    // QE vs lambda, input of the camera simulation, has different QE curves for different
    // pixels. FIXME?

    MCalibrationQEPix &avqepix = (MCalibrationQEPix&)(fQECam->GetAverageArea(0));
    avqepix.SetAverageQE(fPhot2PhotEl); // Needed by MCalibrateData!

    //
    // Find peak of log10(photel/Size) histogram:
    //
    const Int_t reach = 2;
    Stat_t summax = 0;
    Int_t  mode   = 0;
    for (Int_t ibin = 1+reach; ibin <= fHistADC2PhotEl->GetNbinsX()-reach; ibin++)
    {
        const Stat_t sum = fHistADC2PhotEl->Integral(ibin-reach, ibin+reach);

        if (sum <= summax)
            continue;

        summax = sum;
        mode = ibin;
    }

    fADC2PhotEl = TMath::Power(10, fHistADC2PhotEl->GetBinCenter(mode));

    const Int_t num = fCalCam->GetSize();

    for (int i=0; i<num; i++)
    {
        MCalibrationChargePix &calpix = (MCalibrationChargePix&)(*fCalCam)[i];
	MCalibrationQEPix     &qepix  = (MCalibrationQEPix&)    (*fQECam) [i];

	Float_t qe = fPhot2PhotEl;
	if (fGeom->GetPixRatio(i) < 1.)
	  qe *= fOuterPixelsLightCollection;
	qepix.SetAverageQE(qe);

	qepix.SetAvNormFFactor(1.);
	// This factor should convert the default average QE for different colors to 
        // average QE for a spectrum like that of Cherenkov light (see the documentration 
	// of MCalibrationQEPix). 
	// Here we obtain average QE using already a Cherenkov spectrum so AvNormFFactor
	// must be 1.

	
	Float_t factor = fADC2PhotEl;

	//
	// We take into account the (possibly) different gain of outer pixels:
	// FIXME: we are now assuming that all inner pixels have the same gain, and all 
	// outer pixels have the same gain (different from inner ones though). This can 
        // only be like this in camera 0.7, but might change in future versions of camera.
	//

	if (fGeom->GetPixRatio(i) < 1.)
	  factor *= fHeaderFadc->GetAmplitud()/fHeaderFadc->GetAmplitudOuter();

        calpix.SetMeanConvFADC2Phe(factor);
        calpix.SetMeanConvFADC2PheVar(0.);

        calpix.SetMeanFFactorFADC2Phot(0.);
    }


    return kTRUE;
}
