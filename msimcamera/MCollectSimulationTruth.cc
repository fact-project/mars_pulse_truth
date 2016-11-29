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
//  MCollectSimulationTruth
//
// This task takes the pure analog channels and simulates a trigger
// electronics.
//
// In a first step several channels can be summed together by a look-up table
// fRouteAC.
//
// In a second step from these analog channels the output of a discriminator
// is calculated using a threshold and optional a fixed digital signal length.
//
// The signal length of the digital signal emitted by the discriminator
// can either be bound to the time the signal is above the threshold
// defined by fDiscriminatorThreshold if fDigitalSignalLength<0 or set to a
// fixed length (fDigitalSignalLength>0).
//
// With a second look-up table fCoincidenceMap the analog channels are
// checked for coincidences. The coincidence must at least be of the length
// defined by fCoincidenceTime. The earliest coincide is then stored as
// trigger position.
//
// If a minimum multiplicity m is given, m signals above threshold
// in the coincidence patterns are enough to emit a trigger signal.
//
//
// For MAGIC1:
//  - fDigitalSignalLength between 6ns and 12ns
//  - fCoincidenceTime between 0.25ns to 1ns
//
//
//  Input Containers:
//   IntendedPulsePos [MParameterD]
//   MAnalogChannels
//   MRawRunHeader
//
//  Output Containers:
//   TriggerPos [MParameterD]
//   MRawEvtHeader
//
//////////////////////////////////////////////////////////////////////////////
#include "MCollectSimulationTruth.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MParameters.h"

#include "MLut.h"
#include "MArrayI.h"

#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"
#include "MRawEvtData.h"

#include "MAnalogSignal.h"
#include "MAnalogChannels.h"
#include "MDigitalSignal.h"

#include "MTriggerPattern.h"

#include "MPhotonData.h" 
#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MCollectSimulationTruth);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MCollectSimulationTruth::MCollectSimulationTruth(const char *name, const char *title)
    : fCamera(0),
      fPulsePos(0),
      fTrigger(0),
      fEvt(0),
      fStat(0),
      fData(0),
      fRunHeader(0)
{
    fName  = name  ? name  : "MCollectSimulationTruth";
    fTitle = title ? title : "Task to collect some simulation truth";
}

// --------------------------------------------------------------------------
//
// Check for the necessary parameter containers. Read the luts.
//
Int_t MCollectSimulationTruth::PreProcess(MParList *pList)
{
    fCamera = (MAnalogChannels*)pList->FindObject("MAnalogChannels");
    if (!fCamera)
    {
    	*fLog << err << "MAnalogChannels not found... aborting." << endl;
	return kFALSE;
    }

    fTrigger = (MParameterD*)pList->FindCreateObj("MParameterD", "TriggerPos");
    if (!fTrigger)
        return kFALSE;

    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
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

    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
	*fLog << err << "MRawRunHeader not found... aborting." << endl;
	return kFALSE;
    }

    fData = (MRawEvtData*)pList->FindCreateObj("MRawEvtData");
    if (!fData)
        return kFALSE;


    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MCollectSimulationTruth::Process()
{
    const Double_t freq    = fRunHeader->GetFreqSampling()/1000.;

    const Float_t  pulspos = fPulsePos->GetVal()/freq;

    // Get number of pixels/channels
    const UInt_t numberOfPixels = fCamera->GetNumChannels();
//    std::vector<std::vector<MPulse>> eventTruth(numberOfPixels);

    // Get trigger position and correct for intended pulse position
    const Int_t start_slice = TMath::CeilNint(fTrigger->GetVal()-pulpos);

    // Get number of samples to be digitized
    const Int_t length_of_roi = fData->GetNumSamples();

    const Int_t end_slice = start_slice + length_of_roi

    const Int_t numberOfPulses = fEvt->GetNumPhotons();

      // loop over all pulses
    for (Int_t i=0; i<numberOfPulses; i++)
    {
      const MPhotonData &pulse = (*fEvt)[i];

      MPulse true_pulse;
      true_pulse.amplitude = pulse.GetWeight();
      true_pulse.time      = pulse.GetTime();
      true_pulse.origin    = pulse.GetPrimary();


    }

    return kTRUE;
}
