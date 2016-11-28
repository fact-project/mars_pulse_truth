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
//  MSimAPD
//
// This tasks simulates the individual APDs. Before starting the APD is
// initialized randomly according to the photon rate hitting the APD. Such
// it is assumed that the initial condition of the APD is similar to the real
// one. In this context it is assumed that the events are independent, so
// that the APD is always in the same condition.
//
// For every photon and event the behaviour of the APD is simulated. The
// output is set as weight to the MPhotonData containers.
//
// Remark:
//   - The photon rate used to initialize the APD must match the one used
//     to "fill" the random photons. (FIXME: This should be stored somewhere)
//
//  Input Containers:
//   fNameGeomCam [MGeomCam]
//   MPhotonEvent
//   MPhotonStatistics
//
//  Output Containers:
//   MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimAPD.h"

#include <TH2.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"
#include "MParList.h"

#include "MGeomCam.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MAvalanchePhotoDiode.h"

#include "MParameters.h"

ClassImp(MSimAPD);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimAPD::MSimAPD(const char* name, const char *title)
    : fGeom(0), fEvt(0), fStat(0), fType(1),
    fNumCells(60), fCrosstalkCoeff(0), fDeadTime(3),
    fRecoveryTime(8.75), fAfterpulseProb1(0.11), fAfterpulseProb2(0.14)

{
    fName  = name  ? name  : "MSimAPD";
    fTitle = title ? title : " Task to simulate the detection behaviour of APDs";
}

// --------------------------------------------------------------------------
//
//  Get the necessary parameter containers
//
Int_t MSimAPD::PreProcess(MParList *pList)
{
    if (fNameGeomCam.IsNull())
    {
        *fLog << inf << "No geometry container... skipping." << endl;
        return kSKIP;
    }

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

    fStat = (MPhotonStatistics*)pList->FindObject("MPhotonStatistics");
    if (!fStat)
    {
        *fLog << err << "MPhotonStatistics not found... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fCrosstalkCoeffParam = (MParameterD*)pList->FindCreateObj("MParameterD","CrosstalkCoeffParam");
    if (!fCrosstalkCoeffParam)
    {
        *fLog << err << "CrosstalkCoeffParam [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }
    fCrosstalkCoeffParam->SetVal(fCrosstalkCoeff);

    fRates = (MPedestalCam*)pList->FindObject("AccidentalPhotonRates", "MPedestalCam");
    if (!fRates)
    {
        *fLog << inf;
        *fLog << "AccidentalPhotonRates [MPedestalCam] not found..." << endl;
        *fLog << " using " << fFreq << " as default for all G-APDs." << endl;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Initialize as many APDs as we have pixels in the fGeomCam
//
Bool_t MSimAPD::ReInit(MParList *plist)
{
    if (UInt_t(fAPDs.GetEntriesFast())==fGeom->GetNumPixels())
        return kTRUE;

    fAPDs.Delete();

    // FIXME:
    //   * initialize an empty APD and read the APD setup from a file to
    //     allow different APDs.
    //   * Make the arguments a data member of MSimAPD

    Int_t   ncells     = 0;
    Float_t crosstalk  = 0;
    Float_t deadtime   = 0;
    Float_t recovery   = 0;
    Float_t afterprob1 = 0;
    Float_t afterprob2 = 0;

    switch (fType)
    {
    case 0:
        ncells     = fNumCells;
        crosstalk  = fCrosstalkCoeff;
        deadtime   = fDeadTime;
        recovery   = fRecoveryTime;
        afterprob1 = fAfterpulseProb1;
        afterprob2 = fAfterpulseProb2;

        gLog << inf << "Using custom G-APD parameters:" << endl;
        gLog << " - Num cells:          " << fNumCells << endl;
        gLog << " - Crosstalk coeff.:   " << fCrosstalkCoeff << endl;
        gLog << " - Dead time:          " << fDeadTime << " ns" << endl;
        gLog << " - Recovery time:      " << fRecoveryTime << " ns" << endl;
        gLog << " - Afterpulse 1 prob.: " << fAfterpulseProb1 << endl;
        gLog << " - Afterpulse 2 prob.: " << fAfterpulseProb2 << endl;
        break;

    case 1:
        ncells     = 30;
        crosstalk  = 0.2;
        deadtime   = 3;
        recovery   = 8.75*4;
        afterprob1 = 0;
        afterprob2 = 0;
        break;

    case 2:
        ncells     = 60;
        crosstalk  = 0.2;
        deadtime   = 3;
        recovery   = 8.75;
        afterprob1 = 0;
        afterprob2 = 0;
        break;

    case 3:
        ncells     = 60;
        crosstalk  = 0.15;
        deadtime   = 3;
        recovery   = 8.75;
        afterprob1 = 0;
        afterprob2 = 0;
        break;

    case 4:
        ncells     = 60;
        crosstalk  = 0.15;
        deadtime   = 3;
        recovery   = 8.75;
        afterprob1 = 0.14;
        afterprob2 = 0.11;
        break;

    default:
        *fLog << err << "ERROR - APD type " << fType << " undefined." << endl;
        return kFALSE;
    }

    for (UInt_t i=0; i<fGeom->GetNumPixels(); i++)
    {
        APD *apd = new APD(ncells, crosstalk, deadtime, recovery);
        apd->SetAfterpulseProb(afterprob1, afterprob2);

        fAPDs.Add(apd);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process all photons through the corresponding APD and set the output
// (weight) accordingly.
//
Int_t MSimAPD::Process()
{
    // Make all APDs look neutral for the first hit by a photon according to the
    // average hit rate
    const UInt_t npix = fAPDs.GetEntriesFast();

    // Check if we can safely proceed (this can fail if we either haven't been
    // ReInit'ed or the max index in MPhotonStatistics is wrong)
    if ((Int_t)npix<fStat->GetMaxIndex())
    {
        *fLog << err << "ERROR - MSimAPD::Process: Only " << npix << " APDs initialized. At least " << fStat->GetMaxIndex() << " needed... abort." << endl;
        return kERROR;
    }

    // To ensure that the photons are always sorted when calling
    // HitRandomCellRelative the array is sorted here. If it is sorted
    // already nothing will be done since the status is stored.
    // FIXME: Check that this is true and check that it is really necessary
    fEvt->Sort();

    // This tries to initialize dead and relaxing cells properly. If
    // the APD has not been initialized before the chip is randomsly
    // filled, otherwise a time window of the default relaxing time
    // is simulated, so that the previous influence is less than a permille.
    for (UInt_t idx=0; idx<npix; idx++)
    {
        const Double_t freq = fRates ? (*fRates)[idx].GetPedestal() : fFreq;

        // Init creates an empty G-APD, i.e. without external pulses
        // but the correct history for afterpulses and relaxation.
        // If it was already initialized once it evolves the G-APD
        // for a time until the effect of relaxation and afterpulses
        // is below 0.1%. The also creates the possible afterpulses
        // of the future and deletes later afterpulses from the list.
        // After the the time stamp fTime is set to 0.
        static_cast<APD*>(fAPDs.UncheckedAt(idx))->Init(freq);
    }

    // Get number of photons
    const Int_t num = fEvt->GetNumPhotons();

    // Loop over all photons
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon
        MPhotonData &ph = (*fEvt)[i];

        // Get arrival time of photon wrt to left edge of window and its index
        const Double_t t = ph.GetTime()-fStat->GetTimeFirst();
        const Int_t  idx = ph.GetTag();
        if (idx<0)
        {
            *fLog << err << "ERROR - MSimAPD: Invalid index -1." << endl;
            return kERROR;
        }

        if (ph.GetWeight()!=1)
        {
            *fLog << err << "ERROR - MSimAPD: Weight of " << i << "-th photon not 1, but " << ph.GetWeight() << endl;
            ph.Print();
            return kERROR;
        }

        // Simulate hitting the APD at a time t after T0 (APD::fTime).
        // Crosstalk is taken into account and the resulting signal height
        // in effective "number of photons" is returned. Afterpulses until
        // this time "hit" the G-APD and newly created afterpulses
        // are stored in the list of afterpulses
        const Double_t hits = static_cast<APD*>(fAPDs.UncheckedAt(idx))->HitRandomCellRelative(t);

        // Set the weight to the input
        ph.SetWeight(hits);
    }

    // Now we have to shift the evolved time of all APDs to the end of our
    // simulated time.
    for (UInt_t idx=0; idx<npix; idx++)
    {
        APD *a = static_cast<APD*>(fAPDs.UncheckedAt(idx));

        const Double_t end = fStat->GetTimeLast()-fStat->GetTimeFirst();

        // This moves T0 (APD::fTime) at the right edge of our time-
        // window. For the next event this means that afterpulses of past
        // noise and afterpulses will be available already.
        // FIXME: Note, that this might mean that a cosmics-pulse
        //        might increase the noise above the normal level.
        a->IncreaseTime(end);

        // Get the afterpulses and add them to the signal
        TIter Next(&a->GetListOfAfterpulses());
        Afterpulse *ap = 0;
        while ((ap=static_cast<Afterpulse*>(Next())))
        {
            // Skip afterpulses later than that which have been
            // already produced
            if (ap->GetTime()>=end)
                continue;

            // Add a new photon
            // FIXME: SLOW!
            MPhotonData &ph = fEvt->Add();

            // Set source to Artificial (noise), the amplitude produced by the
            // afterpulse (includes crosstalk), its arrival time
            // and its amplitude, as well as the index of the channel it
            // corresponds to.
            ph.SetPrimary(MMcEvtBasic::kArtificial);
            ph.SetWeight(ap->GetAmplitude());
            ph.SetTime(ap->GetTime()+fStat->GetTimeFirst());
            ph.SetTag(idx);
        }

        // It seems to make sense to delete the previous afterpulses now
        // but this is not necessary. We have to loop over them in any
        // case. So we omit the additional loop for deletion but instead
        // do the deletion in the next loop at the end of Init()
        // If needed this could be used to keep the total memory
        // consumption slighly lower.
    }

    // Now the newly added afterpulses have to be sorted into the array correctly
    fEvt->Sort();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// NameGeomCam
// Type: 1
//
// Example for a custom made G-APD:
//
//    Type: 0
//    NumCells: 60
//    CrosstalkCoefficient: 0.1   // [prob]
//    DeadTime: 3                 // [ns]
//    RecoveryTime: 8.75          // [ns]
//    AfterpulseProb1: 0.14       // [prob]
//    AfterpulseProb2: 0.11       // [prob]
//
Int_t MSimAPD::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NameGeomCam", print))
    {
        rc = kTRUE;
        fNameGeomCam = GetEnvValue(env, prefix, "NameGeomCam", fNameGeomCam);
    }

    if (IsEnvDefined(env, prefix, "Type", print))
    {
        rc = kTRUE;
        fType = GetEnvValue(env, prefix, "Type", fType);
    }

    if (fType!=0)
        return rc;

    if (IsEnvDefined(env, prefix, "NumCells", print))
    {
        rc = kTRUE;
        fNumCells = GetEnvValue(env, prefix, "NumCells", fNumCells);
    }
    if (IsEnvDefined(env, prefix, "CrosstalkCoefficient", print))
    {
        rc = kTRUE;
        fCrosstalkCoeff = GetEnvValue(env, prefix, "CrosstalkCoefficient", fCrosstalkCoeff);
    }
    if (IsEnvDefined(env, prefix, "DeadTime", print))
    {
        rc = kTRUE;
        fDeadTime = GetEnvValue(env, prefix, "DeadTime", fDeadTime);
    }
    if (IsEnvDefined(env, prefix, "RecoveryTime", print))
    {
        rc = kTRUE;
        fRecoveryTime = GetEnvValue(env, prefix, "RecoveryTime", fRecoveryTime);
    }
    if (IsEnvDefined(env, prefix, "AfterpulseProb1", print))
    {
        rc = kTRUE;
        fAfterpulseProb1 = GetEnvValue(env, prefix, "AfterpulseProb1", fAfterpulseProb1);
    }
    if (IsEnvDefined(env, prefix, "AfterpulseProb2", print))
    {
        rc = kTRUE;
        fAfterpulseProb2 = GetEnvValue(env, prefix, "AfterpulseProb2", fAfterpulseProb2);
    }

    return rc;
}
