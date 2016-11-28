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
//  MDigitalSignal
//
// A digital signal with a start time and length.
//
//////////////////////////////////////////////////////////////////////////////
#include "MDigitalSignal.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MDigitalSignal);

using namespace std;

// ------------------------------------------------------------------------
//
// Initialize a new signal with a coincidence (and) of the two given
// signals.
//
MDigitalSignal::MDigitalSignal(const MDigitalSignal &ttl1, const MDigitalSignal &ttl2) : fIndex(-1)
{
    const Double_t new0 = TMath::Max(ttl1.fStart,              ttl2.fStart);
    const Double_t new1 = TMath::Min(ttl1.fStart+ttl1.fLength, ttl2.fStart+ttl2.fLength);

    fStart  = new0;
    fLength = new1-new0;
}

// ------------------------------------------------------------------------
//
// Compare the start time of two signal.
//
// Returns:
//    0) Both signals have identical start time
//    1) the argument starts earlier than this
//   -1) The argument starts later than this
//
Int_t MDigitalSignal::Compare(const TObject *obj) const
{
    const MDigitalSignal &ttl = *static_cast<const MDigitalSignal*>(obj);

    if (ttl.fStart>fStart)
        return -1;

    if (ttl.fStart<fStart)
        return 1;

    return 0;
}

// ------------------------------------------------------------------------
//
// Return whether two signals overlap.
//
Bool_t MDigitalSignal::Overlap(const TObject &obj) const
{
    const MDigitalSignal &ttl = static_cast<const MDigitalSignal&>(obj);

    return fStart<=ttl.fStart+ttl.fLength && ttl.fStart<=fStart+fLength;
}

// ------------------------------------------------------------------------
//
// Combine a new (overlapping) signal with this signal to one signal.
// If they don't overlap kFALSE is returned, kTRUE otherwise.
//
Bool_t MDigitalSignal::Combine(const TObject &obj)
{
    if (!Overlap(obj))
        return kFALSE;

    const MDigitalSignal &ttl = static_cast<const MDigitalSignal&>(obj);

    const Double_t new0 = TMath::Min(fStart,         ttl.fStart);
    const Double_t new1 = TMath::Max(fStart+fLength, ttl.fStart+ttl.fLength);

    fStart  = new0;
    fLength = new1-new0;

    return kTRUE;
}

/*
Bool_t Overlay(const TObject &obj)
{
    const TTL &ttl = static_cast<const TTL&>(obj);

    const Double_t new0 = TMath::Max(fStart,         ttl.fStart);
    const Double_t new1 = TMath::Min(fStart+fLength, ttl.fStart+ttl.fLength);

    fStart  = new0;
    fLength = new1-new0;

    return IsValid();
}*/


void MDigitalSignal::Print(Option_t *o) const
{
    gLog << all << Form("%.2f,%.2f ", fStart, fStart+fLength) << endl;
}
