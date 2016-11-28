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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Markus Gaus 10/2002 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////////
//
//  MRawEvtPixelIter
//
//  class to iterate over all pixels of one event.
//  The calling is similar to a root iterator:
//  
//  MRawEvtData *evtdata;            // must be filled with data from somewhere
//  MRawEvtPixelIter pixel(evtdata); // evtdata: ptr to event you want to iterate
//
//  while (pixel.Next())
//  {
//     // here you can access the actual time slices by using
//     //   pixel.GetPixelId();
//     //   pixel.GetHiGainFadcSamples()[i]; // i is the number of the slice
//     //   pixel.HasLoGain();               // check if pixel has
//     //   pixel.GetLoGainFadcSamples()[i]; // i is the number of the slice
//
//     // WARNING: Don't acces more time slices than available.
//     //   Get the numbers by calling: evtdata->GetNum[Lo,Hi]GainSamples()
//     //   This number is constant for one event
//  }
//
///////////////////////////////////////////////////////////////////////////////
#include "MRawEvtPixelIter.h"

#include <TArrayC.h>

#include "MRawEvtData.h"

#include "MArrayS.h"
#include "MArrayB.h"

ClassImp(MRawEvtPixelIter);

using namespace std;

MRawEvtPixelIter::MRawEvtPixelIter(MRawEvtData *dat) :
    fABFlags(0), fStartCell(0), fData(dat)
{
    fNumBytesHiGain  = dat->GetNumHiGainSamples()*dat->GetNumBytesPerSample();
    fNumBytesLoGain  = dat->GetNumLoGainSamples()*dat->GetNumBytesPerSample();

    Reset();
}

// --------------------------------------------------------------------------
//
// It steps to the next pixel. If there is no next pixel NULL is returned.
// If a next pixel where found, a pointer to the primary given (constructor)
// data structur is returned.
//
MRawEvtData *MRawEvtPixelIter::Next()
{
    //
    // if we are already at the last entry there is no 'next' entry anymore
    //
    if (fNumHiGainEntry==fData->fHiGainPixId->GetSize())
        return NULL;

    //
    // For old MC data which stores hi- and lo-gain in two arrays
    // we have to use the old algorithm
    //
    if (fData->fLoGainPixId->GetSize())
    {
        fNumHiGainEntry++;
        fHiGainId++;
        fHiGainPos += fNumBytesHiGain;

        fNumLoGainEntry++;
        fLoGainId++;
        fLoGainPos += fNumBytesLoGain;
    }
    else
    {
        fNumLoGainEntry = ++fNumHiGainEntry;
        fLoGainId       = ++fHiGainId;

        fHiGainPos     += fNumBytesHiGain+fNumBytesLoGain;
        fLoGainPos      = fHiGainPos + fNumBytesHiGain;

        fStartCell++;
    }


    //
    // return a pointer to the 'source' class if we succeed
    //
    return fData;
}

// --------------------------------------------------------------------------
//
// Reset the iteration. Jump to the first pixel.
//
void MRawEvtPixelIter::Reset()
{
    //
    // set counter to zero
    //
    fNumLoGainEntry = 0;
    fNumHiGainEntry = 0;

    //
    // set pointer to first entry of arrays
    //
    fHiGainId = fData->fHiGainPixId->GetArray()-1;
    fABFlags  = fData->fABFlags->GetSize()==0 ? 0 : fData->fABFlags->GetArray();

    //
    // For old MC data which stores hi- and lo-gain in two arrays
    // we have to use the old algorithm
    //
    if (fData->fLoGainPixId->GetSize())
    {
        fLoGainId  = fData->fLoGainPixId->GetArray();
        fHiGainPos = fData->fHiGainFadcSamples->GetArray()-fNumBytesHiGain;
        fLoGainPos = fData->fLoGainFadcSamples->GetArray()-fNumBytesLoGain;
    }
    else
    {
        fLoGainId  = fHiGainId;
        fLoGainPos = fHiGainPos+fNumBytesHiGain;
        fHiGainPos = fData->fHiGainFadcSamples->GetArray()-(fNumBytesHiGain+fNumBytesLoGain);
    }

    fStartCell = fData->fStartCells->GetSize() ? fData->GetStartCells()-1 : 0;

    //
    // In case fLoGainPixId.GetSize()=0 some root versions seems to
    // initialize the array with NULL. This makes both cases work.
    //
    if (fLoGainId)
        fLoGainId -= 1;
}

// --------------------------------------------------------------------------
//
// Calls the draw-function of the actual pixel (see MRawEvtData::Draw)
//
void MRawEvtPixelIter::Draw(Option_t *t)
{
    fData->Draw(Form("%s%d", t, *fHiGainId));
}
