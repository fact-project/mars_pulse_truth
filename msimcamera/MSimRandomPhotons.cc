/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimRandomPhotons
//
//  Simulate poissonian photons. Since the distribution of the arrival time
// differences of these photons is an exonential we can simulate them
// using exponentially distributed time differences between two consecutive
// photons.
//
// FIXME: We should add the wavelength distribution.
//
// The artificial night sky background rate is calculated as follows:
//
//  * The photon detection efficiency vs. wavelength of the detector is obtained
//    from "PhotonDetectionEfficiency" of type "MParSpline"
//
//  * The angular acceptance of the light collectors is obtained
//    from "ConesAngularAcceptance" of type "MParSpline"
//
//  * The spectral acceptance of the light collectors is obtained
//    from "ConesTransmission" of type "MParSpline"
//
//  * The reflectivity of the mirrors vs wavelength is obtained
//    from "MirrorReflectivity" of type "MParSpline"
//
// The rate is then calculated as
//
//   R = R0 * Ai * f
//
// R0 is the night sky background rate as given in Eckart's paper (divided
// by the wavelength window). Ai the area of the cones acceptance window,
// f is given as:
//
//   f = nm * Min(Ar, sr*d^2)
//
// with
//
//   nm being the integral of the product of the mirror reflectivity, the cone
//   transmission and the photon detection efficiency.
//
//   d the distance of the focal plane to the mirror
//
//   Ar is the total reflective area of the reflector
//
//   sr is the effective solid angle corresponding to the integral of the
//   cones angular acceptance
//
// Alternatively, the night-sky background rate can be calculated from
// a spectrum as given in Fig. 1 (but versus Nanometers) in
//
//   Chris R. Benn & Sara L. Ellison La Palma Night-Sky Brightness
//
// After proper conversion of the units, the rate of the pixel 0
// is then calculated by
//
//     rate = f * nsb
//
// With nsb
//
//   nsb = Integral(nsb spectrum * combines efficiencies)
//
// and f can be either
//
//   Eff. angular acceptance Cones (e.g. 20deg) * Cone-Area (mm^2)
//   f = sr * A0
//
// or
//
//   Mirror-Area * Field of view of cones (deg^2)
//   f = Ar * A0;
//
//
//  Input Containers:
//   fNameGeomCam [MGeomCam]
//   MPhotonEvent
//   MPhotonStatistics
//   MCorsikaEvtHeader
//   [MCorsikaRunHeader]
//
//  Output Containers:
//   MPhotonEvent
//   AccidentalPhotonRate [MPedestalCam]
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimRandomPhotons.h"

#include <TRandom.h>

#include "MMath.h"        // RndmExp

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MCorsikaRunHeader.h"

#include "MSpline3.h"
#include "MParSpline.h"
#include "MReflector.h"

ClassImp(MSimRandomPhotons);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimRandomPhotons::MSimRandomPhotons(const char* name, const char *title)
    : fGeom(0), fEvt(0), fStat(0), /*fEvtHeader(0),*/ fRunHeader(0),
    fRates(0), fSimulateWavelength(kFALSE), fNameGeomCam("MGeomCam"),
    fFileNameNSB("resmc/night-sky-la-palma.txt")
{
    fName  = name  ? name  : "MSimRandomPhotons";
    fTitle = title ? title : "Simulate possonian photons (like NSB or dark current)";
}

// --------------------------------------------------------------------------
//
//  Check for the necessary containers
//
Int_t MSimRandomPhotons::PreProcess(MParList *pList)
{
    fGeom = (MGeomCam*)pList->FindObject(fNameGeomCam, "MGeomCam");
    if (!fGeom)
    {
        *fLog << inf << fNameGeomCam << " [MGeomCam] not found..." << endl;

        fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
        if (!fGeom)
        {
            *fLog << err << "MGeomCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fStat = (MPhotonStatistics*)pList->FindObject("MPhotonStatistics");
    if (!fStat)
    {
        *fLog << err << "MPhotonStatistics not found... aborting." << endl;
        return kFALSE;
    }

    fRates = (MPedestalCam*)pList->FindCreateObj("MPedestalCam", "AccidentalPhotonRates");
    if (!fRates)
        return kFALSE;

    /*
    fEvtHeader = (MCorsikaEvtHeader*)pList->FindObject("MCorsikaEvtHeader");
    if (!fEvtHeader)
    {
        *fLog << err << "MCorsikaEvtHeader not found... aborting." << endl;
        return kFALSE;
    }*/

    fRunHeader = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (fSimulateWavelength && !fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    MReflector *r = (MReflector*)pList->FindObject("Reflector", "MReflector");
    if (!r)
    {
        *fLog << err << "Reflector [MReflector] not found... aborting." << endl;
        return kFALSE;
    }

    const MParSpline *s1 = (MParSpline*)pList->FindObject("PhotonDetectionEfficiency", "MParSpline");
    const MParSpline *s2 = (MParSpline*)pList->FindObject("ConesTransmission",         "MParSpline");
    const MParSpline *s3 = (MParSpline*)pList->FindObject("MirrorReflectivity",        "MParSpline");
    const MParSpline *s4 = (MParSpline*)pList->FindObject("ConesAngularAcceptance",    "MParSpline");

    // Ensure that all efficiencies are at least defined in the raneg of the
    // photon detection efficiency. We assume that this is the limiting factor
    // and has to be zero at both ends.
    if (s2->GetXmin()>s1->GetXmin())
    {
        *fLog << err << "ERROR - ConeTransmission range must be defined down to " << s1->GetXmin() << "nm (PhotonDetectionEffciency)." << endl;
        return kFALSE;
    }
    if (s2->GetXmax()<s1->GetXmax())
    {
        *fLog << err << "ERROR - ConeTransmission range must be defined up to " << s1->GetXmax() << "nm (PhotonDetectionEffciency)." << endl;
        return kFALSE;
    }
    if (s3->GetXmin()>s1->GetXmin())
    {
        *fLog << err << "ERROR - MirrorReflectivity range must be defined down to " << s1->GetXmin() << "nm (PhotonDetectionEffciency)." << endl;
        return kFALSE;
    }
    if (s3->GetXmax()<s1->GetXmax())
    {
        *fLog << err << "ERROR - MirrorReflectivity range must be defined up to " << s1->GetXmax() << "nm (PhotonDetectionEffciency)." << endl;
        return kFALSE;
    }

    // If the simulated wavelength range exists and is smaller, reduce the
    // range to it. Later it is checked that at both edges the transmission
    // is 0. This must be true in both cases: The simulated wavelength range
    // exceed the PDE or the PDE range exceeds the simulated waveband.
    const Float_t wmin = fRunHeader && fRunHeader->GetWavelengthMin()>s1->GetXmin() ? fRunHeader->GetWavelengthMin() : s1->GetXmin();
    const Float_t wmax = fRunHeader && fRunHeader->GetWavelengthMax()<s1->GetXmax() ? fRunHeader->GetWavelengthMax() : s1->GetXmax();

    const Int_t min = TMath::FloorNint(wmin);
    const Int_t max = TMath::CeilNint(wmax);

    // Multiply all relevant efficiencies to get the total transmission
    MParSpline eff;
    eff.SetFunction("1", max-min, min, max);

    eff.Multiply(*s1->GetSpline());
    eff.Multiply(*s2->GetSpline());
    eff.Multiply(*s3->GetSpline());

    // Effectively transmitted wavelength band in the simulated range
    const Double_t nm = eff.GetSpline()->Integral();

    // Angular acceptance of the cones
    const Double_t sr = s4 && s4->GetSpline() ? s4->GetSpline()->IntegralSolidAngle() : 1;

    {
        const Double_t d2   = fGeom->GetCameraDist()*fGeom->GetCameraDist();
        const Double_t conv = fGeom->GetConvMm2Deg()*TMath::DegToRad();
        const Double_t f1   = TMath::Min(r->GetA()/1e4, sr*d2) * conv*conv;

        // Rate in GHz / mm^2
        fScale = fFreqNSB * nm * f1; // [GHz/mm^2] efficiency * m^2 *rad^2 *mm^2

        const Double_t freq0 = fScale*(*fGeom)[0].GetA()*1000;

        *fLog << inf << "Resulting Freq. in " << fNameGeomCam << "[0]: " << Form("%.2f", freq0) << "MHz" << endl;

        // FIXME: Scale with the number of pixels
        if (freq0>1000)
        {
            *fLog << err << "ERROR - Frequency exceeds 1GHz, this might leed to too much memory consumption." << endl;
            return kFALSE;
        }
    }

    if (fFileNameNSB.IsNull())
        return kTRUE;

    // const MMcRunHeader *mcrunheader = (MMcRunHeader*)pList->FindObject("MMcRunHeader");
    // Set NumPheFromDNSB

    // # Number of photons from the diffuse NSB (nphe / ns 0.1*0.1 deg^2 239 m^2) and
    // nsb_mean 0.20
    // Magic pixel: 0.00885361 deg
    // dnsbpix = 0.2*50/15
    // ampl = MMcFadcHeader->GetAmplitud()
    // sqrt(pedrms*pedrms + dnsbpix*ampl*ampl/ratio)

    // Conversion of the y-axis
    // ------------------------
    // Double_t ff = 1;                               // myJy / arcsec^2 per nm
    // ff *= 1e-6;                                    // Jy   / arcsec^2 per nm
    // ff *= 3600*3600;                               // Jy   / deg^2
    // ff *= 1./TMath::DegToRad()/TMath::DegToRad();  // Jy/sr = 1e-26J/s/m^2/Hz/sr
    // ff *= 1e-26;                                   // J/s/m^2/Hz/sr   per nm

    const Double_t arcsec2rad = TMath::DegToRad()/3600.;
    const Double_t f = 1e-32 / (arcsec2rad*arcsec2rad);

    // Read night sky background flux from file
    MParSpline flux;
    if (!flux.ReadFile(fFileNameNSB))
        return kFALSE;

    if (flux.GetXmin()>wmin)
    {
        *fLog << err << "ERROR - NSB flux from " << fFileNameNSB << " must be defined down to " << wmin << "nm." << endl;
        return kFALSE;
    }
    if (flux.GetXmax()<wmax)
    {
        *fLog << err << "ERROR - NSB flux from " << fFileNameNSB << " must be defined up to " << wmax << "nm." << endl;
        return kFALSE;
    }

    MParSpline nsb;

    // Normalization to our units,
    // conversion from energy flux to photon flux
    nsb.SetFunction(Form("%.12e/(x*TMath::H())", f), max-min, min, max);

    // multiply night sky background flux with normalization
    nsb.Multiply(*flux.GetSpline());

    // Multiply with the total transmission
    nsb.Multiply(*eff.GetSpline());

    // Check if the photon flux is zero at both ends of the NSB
    if (eff.GetSpline()->Eval(min)>1e-5)
    {
        *fLog << warn << "WARNING - Total transmission efficiency at ";
        *fLog << min << "nm is not zero, but " << eff.GetSpline()->Eval(min) << "... abort." << endl;
    }
    if (eff.GetSpline()->Eval(max)>1e-5)
    {
        *fLog << warn << "WARNING - Total transmission efficiency at ";
        *fLog << max << "nm is not zero, but " << eff.GetSpline()->Eval(max) << "... abort." << endl;
    }

    // Check if the photon flux is zero at both ends of the simulated region
    if (eff.GetSpline()->Eval(wmin)>1e-5)
    {
        *fLog << err << "ERROR - Total transmission efficiency at ";
        *fLog << wmin << "nm is not zero... abort." << endl;
        *fLog << "        PhotonDetectionEfficency: " << s1->GetSpline()->Eval(wmin) << endl;
        *fLog << "        ConeTransmission:         " << s2->GetSpline()->Eval(wmin) << endl;
        *fLog << "        MirrorReflectivity:       " << s3->GetSpline()->Eval(wmin) << endl;
        *fLog << "        TotalEfficiency:          " << eff.GetSpline()->Eval(wmin) << endl;
        return kFALSE;
    }
    if (eff.GetSpline()->Eval(wmax)>1e-5)
    {
        *fLog << err << "ERROR - Total transmission efficiency at ";
        *fLog << wmax << "nm is not zero... abort." << endl;
        *fLog << "        PhotonDetectionEfficency: " << s1->GetSpline()->Eval(wmax) << endl;
        *fLog << "        ConeTransmission:         " << s2->GetSpline()->Eval(wmax) << endl;
        *fLog << "        MirrorReflectivity:       " << s3->GetSpline()->Eval(wmax) << endl;
        *fLog << "        TotalEfficiency:          " << eff.GetSpline()->Eval(wmax) << endl;
        return kFALSE;
    }

    // Conversion from m to radians
    const Double_t conv = fGeom->GetConvMm2Deg()*TMath::DegToRad()*1e3;

    // Angular acceptance of the cones
    //const Double_t sr = s5.GetSpline()->IntegralSolidAngle(); // sr
    // Absolute reflector area
    const Double_t Ar = r->GetA()/1e4;                          // m^2
    // Size of the cone's entrance window
    const Double_t A0 = (*fGeom)[0].GetA()*1e-6;                // m^2

    // Rate * m^2 * Solid Angle
    // -------------------------

    // Angular acceptance Cones (e.g. 20deg) * Cone-Area
    const Double_t f1 = A0 * sr;                // m^2 sr

    // Mirror-Area * Field of view of cones (e.g. 0.1deg)
    const Double_t f2 = Ar * A0*conv*conv;      // m^2 sr

    // FIXME: Calculate the reflectivity of the bottom by replacing
    // MirrorReflectivity by bottom reflectivity and reflect
    // and use it to reflect the difference between f1 and f2
    // if any.

    // Total NSB rate in MHz per m^2 and sr
    const Double_t rate = nsb.GetSpline()->Integral() * 1e-6;

    *fLog << inf;

    // Resulting rates as if Razmick's constant had been used
    // *fLog << 1.75e6/(600-300) * f1 * eff.GetSpline()->Integral() << " MHz" << endl;
    // *fLog << 1.75e6/(600-300) * f2 * eff.GetSpline()->Integral() << " MHz" << endl;

    *fLog << "Conversion factor Fnu:         " << f  << endl;
    *fLog << "Total reflective area:         " << Form("%.2f", Ar) << " m" << UTF8::kSquare << endl;
    *fLog << "Acceptance area of cone 0:     " << Form("%.2f", A0*1e6) << " mm" << UTF8::kSquare << " = ";
    *fLog << A0*conv*conv << " sr" << endl;
    *fLog << "Cones angular acceptance:      " << sr << " sr" << endl;
    *fLog << "ConeArea*ConeSolidAngle (f1):  " << f1 << " m^2 sr" << endl;
    *fLog << "MirrorArea*ConeSkyAngle (f2):  " << f2 << " m^2 sr" << endl;
    *fLog << "Effective. transmission:       " << Form("%.1f", nm) << " nm" << endl;
    *fLog << "NSB freq. in " << fNameGeomCam << "[0] (f1): " << Form("%.2f", rate * f1) << " MHz" << endl;
    *fLog << "NSB freq. in " << fNameGeomCam << "[0] (f2): " << Form("%.2f", rate * f2) << " MHz" << endl;
    *fLog << "Using f2." << endl;

    // Scale the rate per mm^2 and to GHz
    fScale = rate * f2 / (*fGeom)[0].GetA() / 1000;

    // FIXME: Scale with the number of pixels
    if (rate*f2>1000)
    {
        *fLog << err << "ERROR - Frequency exceeds 1GHz, this might leed to too much memory consumption." << endl;
        return kFALSE;
    }

    return kTRUE;
}

Bool_t MSimRandomPhotons::ReInit(MParList *pList)
{
    // Overwrite the default set by MGeomApply
    fRates->Init(*fGeom);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Check for the necessary containers
//
Int_t MSimRandomPhotons::Process()
{
    // Get array from event container
    // const Int_t num = fEvt->GetNumPhotons();
    //
    // Do not produce pure pedestal events!
    // if (num==0)
    //    return kTRUE;

    // Get array from event container
    // FIXME: Use statistics container instead
    const UInt_t npix = fGeom->GetNumPixels();

    // This is the possible window in which the triggered digitization
    // may take place.
    const Double_t start = fStat->GetTimeFirst();
    const Double_t end   = fStat->GetTimeLast();

    // Loop over all pixels
    for (UInt_t idx=0; idx<npix; idx++)
    {
        // Scale the rate with the pixel size.
        const Double_t rate = fFreqFixed + fScale*(*fGeom)[idx].GetA();

        (*fRates)[idx].SetPedestal(rate);

        // Calculate the average distance between two consequtive photons
        const Double_t avglen = 1./rate;

        // Start producing photons at time "start"
        Double_t t = start;
        while (1)
        {
            // Get a random time for the photon.
            // The differences are exponentially distributed.
            t += MMath::RndmExp(avglen);

            // Check if we reached the end of the useful time window
            if (t>end)
                break;

            // Add a new photon
            // FIXME: SLOW!
            MPhotonData &ph = fEvt->Add();

            // Set source to NightSky, time to t and tag to pixel index
            ph.SetPrimary(MMcEvtBasic::kNightSky);
            ph.SetWeight();
            ph.SetTime(t);
            ph.SetTag(idx);

            // fProductionHeight, fPosX, fPosY, fCosU, fCosV (irrelevant)  FIXME: Reset?

            if (fSimulateWavelength)
            {
                const Float_t wmin = fRunHeader->GetWavelengthMin();
                const Float_t wmax = fRunHeader->GetWavelengthMax();

                ph.SetWavelength(TMath::Nint(gRandom->Uniform(wmin, wmax)));
            }
        }
    }

    // Re-sort the photons by time!
    fEvt->Sort(kTRUE);

    // Update maximum index
    fStat->SetMaxIndex(npix-1);

    // Shrink
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the parameters from the resource file.
//
//    FrequencyFixed: 0.040
//    FrequencyNSB:   5.8
//
// The fixed frequency is given in units fitting the units of the time.
// Usually the time is given in nanoseconds thus, e.g., 0.040 means 40MHz.
//
// The FrequencyNSB is scaled by the area of the pixel in cm^2. Therefore
// 0.040 would mean 40MHz/cm^2
//
Int_t MSimRandomPhotons::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FrequencyFixed", print))
    {
        rc = kTRUE;
        fFreqFixed = GetEnvValue(env, prefix, "FrequencyFixed", fFreqFixed);
    }

    if (IsEnvDefined(env, prefix, "FrequencyNSB", print))
    {
        rc = kTRUE;
        fFreqNSB = GetEnvValue(env, prefix, "FrequencyNSB", fFreqNSB);
    }

    if (IsEnvDefined(env, prefix, "FileNameNSB", print))
    {
        rc = kTRUE;
        fFileNameNSB = GetEnvValue(env, prefix, "FileNameNSB", fFileNameNSB);
    }

    if (IsEnvDefined(env, prefix, "SimulateCherenkovSpectrum", print))
    {
        rc = kTRUE;
        fSimulateWavelength = GetEnvValue(env, prefix, "SimulateCherenkovSpectrum", fSimulateWavelength);
    }

    return rc;
}
