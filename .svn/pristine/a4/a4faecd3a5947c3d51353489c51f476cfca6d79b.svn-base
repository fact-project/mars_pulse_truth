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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2002-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MEventRateCalc
//
//  This task calculates the event rates from the event times and numbers.
//
//  The algorithm is explained in Process()
//
//
//    rate = Number of events / time in which the events were recorded
//       r = N / t
//       s = t / N  = 1 / r    mean time interval between consecutive events
//
//    for an exponential distribution of the time differences d between
//    consecutive events:
//
//       s = <d>
//       sigma(d)     = <d> = s
//       delta(s)     = sigma(d) /sqrt(N) = s / sqrt(N)
//       delta(s) / s = 1 / sqrt(N)
//
//       delta(r) / r = delta(s) / s = 1 / sqrt(N)
//
//
//  In addition the difference between the event time of the current event
//  and the last event is written into a MParameterD calles MTimeDiff.
//
//
//  Input Containers:
//    MTime
//
//  Output Containers:
//    MEventRate
//    MTimeDiff [MParameterD]
//    MTimeRate [MTime] (missing)
//
//  FIXME: For convinience we could implement a mode which always takes
//         n events to calculate the event rate and sets the corresponding
//         time. This mode could be used to UPADTE files with the event
//         rate.
//
//////////////////////////////////////////////////////////////////////////////
#include "MEventRateCalc.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MTime.h"
#include "MEventRate.h"
#include "MParameters.h"

ClassImp(MEventRateCalc);

using namespace std;

const TString MEventRateCalc::gsDefName  = "MEventRateCalc";
const TString MEventRateCalc::gsDefTitle = "Calculate event rate";

const TString MEventRateCalc::gsNameTime      = "MTime";
const TString MEventRateCalc::gsNameEventRate = "MEventRate";
const TString MEventRateCalc::gsNameTimeDiff  = "MTimeDiff";
const TString MEventRateCalc::gsNameTimeRate  = "MTimeRate";

const Int_t MEventRateCalc::gsNumEvents = 1000;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MEventRateCalc::MEventRateCalc(const char *name, const char *title)
    : fNameEventRate(gsNameEventRate), fNameTime(gsNameTime),
    fNameTimeRate(gsNameTimeRate), fNameTimeDiff(gsNameTimeDiff),
    fTimes(gsNumEvents)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//   MTime
//
// The PreProcess searches for the following input containers:
//   MEventRate
//
// Reset all times in the buffer
//
Int_t MEventRateCalc::PreProcess(MParList *pList)
{
    fTime = (MTime*)pList->FindObject(AddSerialNumber(fNameTime), "MTime");
    if (!fTime)
    {
        *fLog << err << AddSerialNumber(fNameTime) << " [MTime] not found... aborting." << endl;
        return kFALSE;
    }

    fRate = (MEventRate*)pList->FindCreateObj("MEventRate", AddSerialNumber(fNameEventRate));
    if (!fRate)
        return kFALSE;

    fTimeRate = (MTime*)pList->FindCreateObj("MTime", AddSerialNumber(fNameTimeRate));
    if (!fTimeRate)
        return kFALSE;

    fTimeDiff = (MParameterD*)pList->FindCreateObj("MParameterD", AddSerialNumber(fNameTimeDiff));
    if (!fTimeDiff)
        return kFALSE;

    fTimes.Reset();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This resets the calculation whenever a new file is opened.
//
Bool_t MEventRateCalc::ReInit(MParList *pList)
{
    fNumFirstEvent = GetNumExecutions();
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the events rate as (t1-t0)/n while t1 is the n-th event after
// t0. If there are not yet enough events in the buffer n is the number
// of available events. Otherwise the number setup in SetNumEvents.
//
Int_t MEventRateCalc::Process()
{
    const ULong_t exec = GetNumExecutions()-fNumFirstEvent-1;

    //*fLog << all << fNumFirstEvent << " " << exec << endl;

    // Calculate the rate
    const UInt_t n = fTimes.GetSize();

    // Get the positon in the ring-buffer
    const UInt_t n1 = exec;
    const UInt_t n2 = exec>=n ? exec+1 : 0;

    // Store the current event time
    fTimes[n1%n] = *fTime;

    // Get the number of events
    const UInt_t cnt = n1<n2 ? n : n1-n2;

    // Store the time difference between two consecutive events
    fTimeDiff->SetVal(exec==0 ? -1 : fTimes[n1%n] - fTimes[(n1+n-1)%n]);
    fTimeDiff->SetReadyToSave();

    // Make sure, that the beginning of data-takeing (open
    // a new file) doesn't effect the rate too much
    if (cnt<n/10)
        return kTRUE;

    // Calculate the rate
    const Double_t rate = (Double_t)cnt/(fTimes[n1%n]-fTimes[n2%n]);

    // Store the rate
    fRate->SetRate(exec>1?rate:0, cnt);
    fRate->SetReadyToSave();

    // Calculate and store the corresponding time
    const Double_t diff = fTimes[n1%n] - fTimes[n2%n];
    const Double_t time = fTimes[n2%n] + (cnt-n/10.)/(n-n/10.)*diff/2;

    fTimeRate->SetMean(time, time);
    fTimeRate->SetReadyToSave();

   /*
    // Store the corresponding time
    if (exec==fNumFirstEvent+n)
        fTimeRate->SetMean(fTimes[n2%n], fTimes[n2%n]);
    else
        fTimeRate->SetMean(fTimes[n1%n], fTimes[n2%n]);
        */

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MEventRateCalc::StreamPrimitive(ostream &out) const
{
    out << "   MEventRateCalc " << GetUniqueName() << "(";
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "\"" <<fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl;

    if (fTimes.GetSize()!=gsNumEvents)
        out << "   " << GetUniqueName() << ".SetNumEvents(" << fTimes.GetSize() << ");" << endl;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MEventRateCalc.NumEvents: 1000
//
Int_t MEventRateCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumEvents", print))
    {
        rc = kTRUE;
        SetNumEvents(GetEnvValue(env, prefix, "NumEvents", fTimes.GetSize()));
    }

    return rc;
}
