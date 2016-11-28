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

#include "MAnalogSignal.h"
#include "MAnalogChannels.h"
#include "MDigitalSignal.h"

#include "MTriggerPattern.h"

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
      fRunHeader(0),
      fEvtHeader(0),
      fElectronicNoise(0),
      fGain(0),
      fDiscriminatorThreshold(-1),
      fDigitalSignalLength(8),
      fCoincidenceTime(0.5),
      fShiftBaseline(kTRUE),
      fUngainSignal(kTRUE),
      fSimulateElectronics(kTRUE),
      fMinMultiplicity(-1),
      fCableDelay(21),
      fCableDamping(0.)     // default Damping Set to zero, so users, who do not set
                            // the CableDamoing in the ceres.rc do not see a difference.

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
    fTrigger = (MParameterD*)pList->FindCreateObj("MParameterD", "TriggerPos");
    if (!fTrigger)
        return kFALSE;

    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MCollectSimulationTruth::Process()
{
    const Float_t  pulspos = fPulsePos->GetVal()/freq;

    // Valid range in units of bins
    const Float_t min = fCamera->GetValidRangeMin()+pulspos;
    const Float_t max = fCamera->GetValidRangeMax()-(nsamp-pulspos);

    return kTRUE;
}
