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
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@phys.ethz.ch>
!
!   Copyright: CheObs Software Development, 2000-2013
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimCamera
//
//  This task initializes the analog channels with analog noise and simulated
//  the analog pulses from the photon signal.
//
//  Input Containers:
//   MPhotonEvent
//   MPhotonStatistics
//   MRawRunHeader
//
//  Output Containers:
//   MAnalogChannels
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimCamera.h"

#include <TF1.h>
#include <TRandom.h>            // Needed for TRandom

#include "MLog.h"
#include "MLogManip.h"

#include "MTruePhotonsPerPixelCont.h"

#include "MSpline3.h"
#include "MParSpline.h"

#include "MParList.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MAnalogSignal.h"
#include "MAnalogChannels.h"

#include "MParameters.h"

#include "MMcEvt.hxx"            // To be replaced by a CheObs class
#include "MRawRunHeader.h"

ClassImp(MSimCamera);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimCamera::MSimCamera(const char* name, const char *title)
    : fEvt(0), fStat(0), fRunHeader(0), fElectronicNoise(0), fGain(0),
      fCamera(0), fMcEvt(0),fCrosstalkCoeffParam(0), fSpline(0), fBaselineGain(kFALSE),
      fDefaultOffset(-1), fDefaultNoise(-1), fDefaultGain(-1), fACFudgeFactor(0),
      fACTimeConstant(0)

{
    fName  = name  ? name  : "MSimCamera";
    fTitle = title ? title : "Task to simulate the electronic noise and to convert photons into pulses";
}

// --------------------------------------------------------------------------
//
// Search for the necessayr parameter containers.
// Setup spline for pulse shape.
//
Int_t MSimCamera::PreProcess(MParList *pList)
{
    fMcEvt = (MMcEvt*)pList->FindCreateObj("MMcEvt");
    if (!fMcEvt)
        return kFALSE;

    fCamera = (MAnalogChannels*)pList->FindCreateObj("MAnalogChannels");
    if (!fCamera)
        return kFALSE;

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

    fRunHeader = (MRawRunHeader *)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }
    // -------------------------------------------------------------------
    // Dominik Neise and Sebastian Mueller on fix time offsets:
    // We obtain the fix temporal offsets for the FACT camera pixels out of 
    // a text file. The textfile must be mentioned in the ceres.rc file.
    // There are no default offsets on purporse. The filename must be specified
    // in ceres.rc and the file must be parsed without errors and it must 
    // provide exactly 1440 floating point numbers.   
    fFixTimeOffsetsBetweenPixelsInNs = 
    (MMatrix*)pList->FindObject("MFixTimeOffset");
    if (!fFixTimeOffsetsBetweenPixelsInNs)
    {
        // the key value pair providing the text file is not present in the
        // environment env.
        *fLog << err << "In Source: "<< __FILE__ <<" in line: "<< __LINE__;
        *fLog << " in function: "<< __func__ <<"\n";
        *fLog << "MFixTimeOffset not found... aborting." << endl;
        return kFALSE;

    }
    else if ( fFixTimeOffsetsBetweenPixelsInNs->fM.size() != 1440 )
    {
        // The number of time offsets must match the number of pixels in the
        // FACT camera.
        *fLog << err << "In Source: "<< __FILE__ <<" in line: "<< __LINE__;
        *fLog << " in function: "<< __func__ <<"\n";
        *fLog << "MFixTimeOffset has the wrong dimension! ";
        *fLog << "There should be "<< 1440 <<" time offsets ";
        *fLog << "(one for each pixel in FACT) but there are: ";
        *fLog << fFixTimeOffsetsBetweenPixelsInNs->fM.size() << "! ";
        *fLog << "... aborting." << endl;
        return kFALSE;
    }
    // Check all entries for inf and nan. Those are not accepted here.
    for( int row_index=0; row_index<fFixTimeOffsetsBetweenPixelsInNs->fM.size(); row_index++){
        std::vector<double> row = fFixTimeOffsetsBetweenPixelsInNs->fM.at(row_index);
        for( int col_index=0; col_index<row.size(); col_index++){
            double specific_delay = row.at(col_index);
            if( std::isnan(specific_delay) || std::isinf(specific_delay) ){
                *fLog << err << "In Source: "<< __FILE__ <<" in line: ";
                *fLog << __LINE__;
                *fLog << " in function: "<< __func__ <<"\n";
                *fLog << "There is a non normal specific_delay in the fix temporal ";
                *fLog << "pixel offsets. This is that at least one specific_delay is ";
                *fLog << "NaN or Inf. This here is >"<< specific_delay;
                *fLog << "<... aborting." << endl;               
                return kFALSE;
            }
        }

    }
    // -------------------------------------------------------------------
/*
    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }
 */
    fResidualTimeSpread = (MParameterD*)pList->FindObject("ResidualTimeSpread");
    if (!fResidualTimeSpread)
    {
        *fLog << err << "ResidualTimeSpread [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    // Get GapdTimeJitter from parameter list
    fGapdTimeJitter = (MParameterD*)pList->FindObject("GapdTimeJitter");
    if (!fGapdTimeJitter)
    {
        *fLog << err << "GapdTimeJitter [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    // Create it here to make sure that MGeomApply will set the correct size
    fElectronicNoise = (MPedestalCam*)pList->FindCreateObj("MPedestalCam", "ElectronicNoise");
    if (!fElectronicNoise)
        return kFALSE;

    fGain = (MPedestalCam*)pList->FindCreateObj("MPedestalCam", "Gain");
    if (!fGain)
        return kFALSE;

    fAccidentalPhotons = (MPedestalCam*)pList->FindObject("AccidentalPhotonRates","MPedestalCam");
    if(!fAccidentalPhotons)
    {
        *fLog << err << "AccidentalPhotonRates [MPedestalCam] not found... aborting." << endl;
        return kFALSE;
    }

    fCrosstalkCoeffParam = (MParameterD*)pList->FindCreateObj("MParameterD","CrosstalkCoeffParam");
    if (!fCrosstalkCoeffParam)
    {
        *fLog << err << "CrosstalkCoeffParam [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fTruePhotons = (MTruePhotonsPerPixelCont*)pList->FindCreateObj("MTruePhotonsPerPixelCont");
    if (!fTruePhotons)
    {
        *fLog << err << "MTruePhotonsPerPixelCont not found... aborting." << endl;
        return kFALSE;
    }

    MParSpline *pulse = (MParSpline*)pList->FindObject("PulseShape", "MParSpline");
    if (!pulse)
    {
        *fLog << err << "PulseShape [MParSpline] not found... aborting." << endl;
        return kFALSE;
    }

//    if (fRunHeader->GetFreqSampling()!=1000)
//    {
//        *fLog << err  << "ERROR - Sampling frequencies others than 1GHz are not yet supported." << endl;
//        *fLog << warn << "FIXME - SCALE MPulsShape WITH THE SAMPLING FREQUENCY." << endl;
//        return kFALSE;
//    }

    fSpline = pulse->GetSpline();
    if (!fSpline)
    {
        *fLog << err << "No spline initialized." << endl;
        return kFALSE;
    }

    // ---------------- Information output ----------------------

    if (fBaselineGain)
        *fLog << inf << "Gain is also applied to the electronic noise." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// FIXME: For now this is a workaround to set a baseline and the
// electronic (guassian noise)
//
Bool_t MSimCamera::ReInit(MParList *plist)
{
    for (int i=0; i<fElectronicNoise->GetSize(); i++)
    {
        MPedestalPix &ped = (*fElectronicNoise)[i];
        ped.SetPedestal(fDefaultOffset);
        if (fDefaultNoise>0)
            ped.SetPedestalRms(fDefaultNoise);

        ped.SetPedestalABoffset(0);
        ped.SetNumEvents(0);


        MPedestalPix &gain = (*fGain)[i];
        if (fDefaultGain>0)
            gain.SetPedestal(fDefaultGain);

        gain.SetPedestalRms(0);
        gain.SetPedestalABoffset(0);
        gain.SetNumEvents(0);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// fStat->GetMaxIndex must return the maximum index possible
// (equiv. number of pixels) not just the maximum index stored!
//
Int_t MSimCamera::Process()
{
    // Calculate start time, end time and corresponding number of samples
    const Double_t freq = fRunHeader->GetFreqSampling()/1000.;

    // FIXME: Should we use a higher sampling here?

    const Double_t start = fStat->GetTimeFirst()*freq;
    const Double_t end   = fStat->GetTimeLast() *freq;

    const UInt_t   nlen  = TMath::CeilNint(end-start);

    // Get number of pixels/channels
    const UInt_t npix = fStat->GetMaxIndex()+1;

    if (npix>(UInt_t)fElectronicNoise->GetSize())
    {
        *fLog << err << "ERROR - More indices (" << npix << ") ";
        *fLog << "assigned than existing in camera (";
        *fLog << fElectronicNoise->GetSize() << ")!" << endl;
        return kERROR;
    }

    const Double_t pl = fSpline->GetXmin()*freq;
    const Double_t pr = fSpline->GetXmax()*freq;

    // Init the arrays and set the range which will contain valid data
    fCamera->Init(npix, nlen);
    fCamera->SetValidRange(TMath::FloorNint(pr), TMath::CeilNint(nlen+pl));

    Double_t timeoffset[npix];


    // Add electronic noise to empty channels
    for (UInt_t i=0; i<npix; i++)
    {

        // Get the ResidualTimeSpread Parameter
        const Double_t residualTimeSpread = fResidualTimeSpread->GetVal();

        // Jens Buss on residual time spread:
        // randomly draw an additional time offset to be added to the arrivaltime 
        // from a gaussian normal distribution with a given standard deviation 
        timeoffset[i] = gRandom->Gaus(0.0, residualTimeSpread);
        const MPedestalPix &pix = (*fElectronicNoise)[i];

        const Double_t val = pix.GetPedestal();
        const Double_t rms = pix.GetPedestalRms();

        // FTemme: Implementation of AC-coupling:
        // to calculate the value of the accoupling per slice I use the
        // following equation:
        // accouplingPerSlice = accidentalPhotonRate * (1 + crossTalkProb)
        //       * areaOfOnePulse / samplingRate;
        // Therefore I need the following variables
        // Double_t accidentalPhotonRate; // [MHz]
        // Float_t crossTalkProb;         // [1]
        // Double_t areaOfOnePulse;       // [ADC-Counts * s]
        // Double_t samplingRate;         // [slices * MHz]

        // The accidental photon rate is stored in GHz, so we have to multiply
        // with 1E3 to get MHz:
        const MPedestalPix &accPhoPix = (*fAccidentalPhotons)[i];

        const Double_t accidentalPhotonRate = accPhoPix.GetPedestal() * 1e3; //[MHz]

        Double_t currentAccidentalPhotonRate = accidentalPhotonRate;
        if (fACTimeConstant!=0)
        {
            const Double_t accidentalPhotons      = fACTimeConstant * accidentalPhotonRate;
            const Double_t sigmaAccidentalPhotons = TMath::Sqrt(accidentalPhotons);

            const Double_t gaus = gRandom->Gaus(accidentalPhotons,sigmaAccidentalPhotons);

            currentAccidentalPhotonRate = gaus / fACTimeConstant;
        }

        // Get the CrosstalkCoefficient Parameter
        const Double_t crossTalkProb = fCrosstalkCoeffParam->GetVal();

        // To get the area of one Pulse, I only need to calculate the Integral
        // of the Pulse Shape, which is stored in fSpline. Because the spline is
        // normalized to a maximal amplitude of 1.0, I had to multiply it with
        // the Default gain [ADC-Counts * s]
        const Double_t areaOfOnePulse = fSpline->Integral() * fDefaultGain;

        // The sampling rate I get from the RunHeader:
        const Double_t samplingRate = fRunHeader->GetFreqSampling(); // [slices * MHz]

        const Double_t accouplingPerSlice = currentAccidentalPhotonRate
            * (1 + crossTalkProb + fACFudgeFactor)
            * areaOfOnePulse / samplingRate;

        // The accoupling is substracted from the timeline by decreasing the
        // mean of the gaussian noise which is added

        if (!fBaselineGain)
        {
            (*fCamera)[i].AddGaussianNoise(rms, val - accouplingPerSlice);
            continue;
        }
        // Sorry, the name "pedestal" is misleading here
        // FIXME: Simulate gain fluctuations
        const Double_t gain = (*fGain)[i].GetPedestal();

        // FIXME: We might add the base line here already.
        // FIXME: How stable is the offset?
        // FIXME: Should we write a container AppliedGain for MSImTrigger?

        (*fCamera)[i].AddGaussianNoise(rms*gain, (val - accouplingPerSlice)*gain);
    }

    // FIXME: Simulate correlations with neighboring pixels

    const Int_t num = fEvt->GetNumPhotons();

    // A random shift, uniformely distributed within one slice, to make sure that
    // the first photon is not always aligned identically with a sample edge.
    // FIXME: Make it switchable
    const Float_t rndm = gRandom->Uniform();

    // FIXME: Shell we add a random shift of [0,1] samples per channel?
    //        Or maybe per channel and run?

    Double_t tot = 0;

    for (int i=0 ; i<1440 ; i++)
    {
        (*fTruePhotons->cherenkov_photons_weight)[i] = 0;
        (*fTruePhotons->cherenkov_photons_number)[i] = 0;
        (*fTruePhotons->cherenkov_arrival_time_mean)[i] = 0;
        (*fTruePhotons->cherenkov_arrival_time_variance)[i] = 0;
        (*fTruePhotons->muon_cherenkov_photons_weight)[i] = 0;
        (*fTruePhotons->muon_cherenkov_photons_number)[i] = 0;
        (*fTruePhotons->cherenkov_arrival_time_min)[i] = 10000;
        (*fTruePhotons->cherenkov_arrival_time_max)[i] = 0;
        (*fTruePhotons->noise_photons_weight)[i] = 0;
    }

    //--------------------------------------------------------------------------

    // Get the ResidualTimeSpread Parameter
    const Double_t gapdTimeJitter = fGapdTimeJitter->GetVal();
    
    // Simulate pulses
    for (Int_t i=0; i<num; i++)
    {
        const MPhotonData &ph = (*fEvt)[i];

        const UInt_t   idx = ph.GetTag();
        Double_t t   = (ph.GetTime()-fStat->GetTimeFirst())*freq+rndm;// - fSpline->GetXmin();

        // Sebastian Mueller and Dominik Neise on fix time offsets:
        // We add a fix temporal offset to the relative arrival time of the 
        // individual pixel. The offsets are stored in the
        // fFixTimeOffsetsBetweenPixelsInNs -> fM matrix. We identify the first
        // column to hold the offsets in ns.
        t = t + freq*fFixTimeOffsetsBetweenPixelsInNs->fM[idx][0];

        // Jens Buss on residual time spread:
        // add random time offset to the arrivaltimes
        t = t + timeoffset[idx];

        // FIXME: Time jitter?
        // Jens Buss on GapdTimeJitter
        // add also a time offset to arrival times of single photons
        // TODO: change to ns, use: fRunHeader->GetFreqSampling()
        Double_t timeJitter = gRandom->Gaus(0.0, gapdTimeJitter);
        t = t + timeJitter;

        // FIXME: Add additional routing here?
        // FIMXE: How stable is the gain?

        if (ph.GetPrimary()!=MMcEvt::kNightSky && ph.GetPrimary()!=MMcEvt::kArtificial)
        {
            tot += ph.GetWeight();
            (*fTruePhotons->cherenkov_photons_weight)[idx] += ph.GetWeight();
            (*fTruePhotons->cherenkov_photons_number)[idx] += 1;

            (*fTruePhotons->cherenkov_arrival_time_mean)[idx] += t;
            (*fTruePhotons->cherenkov_arrival_time_variance)[idx] += t*t;

            if (ph.GetPrimary()==MMcEvt::kMUON)
            {
                (*fTruePhotons->muon_cherenkov_photons_weight)[idx] += ph.GetWeight();
                (*fTruePhotons->muon_cherenkov_photons_number)[idx] += 1;
            }

            // find min
            if (t < (*fTruePhotons->cherenkov_arrival_time_min)[idx] )
            {
                (*fTruePhotons->cherenkov_arrival_time_min)[idx] = t;
            }
            // find max
            if (t > (*fTruePhotons->cherenkov_arrival_time_max)[idx] )
            {
               (*fTruePhotons->cherenkov_arrival_time_max)[idx] = t;
            }
        }
        else
        {
            (*fTruePhotons->noise_photons_weight)[idx] += ph.GetWeight();
        }

        // Sorry, the name "pedestal" is misleading here
        // FIXME: Simulate gain fluctuations
        const Double_t gain = (*fGain)[idx].GetPedestal();

        // === FIXME === FIXME === FIXME === Frequency!!!!
        (*fCamera)[idx].AddPulse(*fSpline, t, ph.GetWeight()*gain);
    }

    for (unsigned int i=0 ; i < 1440 ; i++)
    {
        float number = (*fTruePhotons->cherenkov_photons_number)[i];
        (*fTruePhotons->cherenkov_arrival_time_mean)[i] /= number;
        float mean = (*fTruePhotons->cherenkov_arrival_time_mean)[i];
        float sum_tt = (*fTruePhotons->cherenkov_arrival_time_variance)[i];
        (*fTruePhotons->cherenkov_arrival_time_variance)[i] = (sum_tt / number - mean*mean) /(number - 1);
    }

    fMcEvt->SetPhotElfromShower(TMath::Nint(tot));

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// BaselineGain: Off
//
Int_t MSimCamera::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "BaselineGain", print))
    {
        rc = kTRUE;
        fBaselineGain = GetEnvValue(env, prefix, "BaselineGain", fBaselineGain);
    }

    if (IsEnvDefined(env, prefix, "DefaultOffset", print))
    {
        rc = kTRUE;
        fDefaultOffset = GetEnvValue(env, prefix, "DefaultOffset", fDefaultOffset);
    }
    if (IsEnvDefined(env, prefix, "DefaultNoise", print))
    {
        rc = kTRUE;
        fDefaultNoise = GetEnvValue(env, prefix, "DefaultNoise", fDefaultNoise);
    }
    if (IsEnvDefined(env, prefix, "DefaultGain", print))
    {
        rc = kTRUE;
        fDefaultGain = GetEnvValue(env, prefix, "DefaultGain", fDefaultGain);
    }
    if (IsEnvDefined(env, prefix, "ACFudgeFactor", print))
    {
        rc = kTRUE;
        fACFudgeFactor = GetEnvValue(env, prefix, "ACFudgeFactor", fACFudgeFactor);
    }
    if (IsEnvDefined(env, prefix, "ACTimeConstant", print))
    {
        rc = kTRUE;
        fACTimeConstant = GetEnvValue(env, prefix, "ACTimeConstant", fACTimeConstant);
    }

    return rc;
}
