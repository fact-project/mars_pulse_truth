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
//  MAnalogChannels
//
// A collection of MAnalogSignals with a simplified access interface.
//
//////////////////////////////////////////////////////////////////////////////
#include "MAnalogChannels.h"

#include <TObjArray.h>

#include "MAnalogSignal.h"

ClassImp(MAnalogChannels);

using namespace std;

// ------------------------------------------------------------------------
//
// Default constructor
//
MAnalogChannels::MAnalogChannels(const char *name, const char *title)
: fArray(0), fValidRangeMin(0), fValidRangeMax(0)
{
    fName  = name  ? name  : "MAnalogChannels";
    fTitle = title ? title : "Parameter container for a collection of analog signals";
}

// ------------------------------------------------------------------------
//
// Constructor. Calls Init(n, len)
//
MAnalogChannels::MAnalogChannels(Int_t n, Int_t len, const char *name, const char *title)
: fArray(0), fValidRangeMin(0), fValidRangeMax(0)
{
    fName  = name  ? name  : "MAnalogChannels";
    fTitle = title ? title : "Parameter container for a collection of analog signals";

    Init(n, len);
}

// ------------------------------------------------------------------------
//
// delete fArray
//
void MAnalogChannels::Clear(Option_t *o)
{
    if (fArray)
    {
        delete fArray;
        fArray = 0;
    }
}

// ------------------------------------------------------------------------
//
// Initializes n channels with a length of n sampling points.
//
// FIXME: We could improve speed by only increasing the arrays?
//
void MAnalogChannels::Init(UInt_t n, UInt_t len)
{
    Clear();

    fArray = new TObjArray(n);
    fArray->SetOwner();

    for (UInt_t i=0; i<n; i++)
        fArray->AddAt(new MAnalogSignal(len), i);
}

// ------------------------------------------------------------------------
//
// return an unchecked reference to the i-th channel.
//
MAnalogSignal &MAnalogChannels::operator[](UInt_t i)
{
    return *static_cast<MAnalogSignal*>(fArray->UncheckedAt(i));
}

// ------------------------------------------------------------------------
//
// return an unchecked pointer to the i-th channel.
//
MAnalogSignal *MAnalogChannels::operator()(UInt_t i)
{
    return  static_cast<MAnalogSignal*>(fArray->UncheckedAt(i));
}

// ------------------------------------------------------------------------
//
// Return an unchecked const reference to the i-th channel.
//
const MAnalogSignal &MAnalogChannels::operator[](UInt_t i) const
{
    return *static_cast<MAnalogSignal*>(fArray->UncheckedAt(i));
}

// ------------------------------------------------------------------------
//
// Return an unchecked const pointer to the i-th channel.
//
const MAnalogSignal *MAnalogChannels::operator()(UInt_t i) const
{
    return  static_cast<MAnalogSignal*>(fArray->UncheckedAt(i));
}

// ------------------------------------------------------------------------
//
// Return the number of channels.
//
UInt_t MAnalogChannels::GetNumChannels() const
{
    return fArray ? fArray->GetEntriesFast() : 0;
}

// ------------------------------------------------------------------------
//
// Return the number of samples per channel
//
UInt_t MAnalogChannels::GetNumSamples() const
{
    return operator()(0) ? operator()(0)->GetSize() : 0;
}
