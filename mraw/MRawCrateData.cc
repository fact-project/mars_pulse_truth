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
!   Author(s): Thomas Bretz 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRawCrateData
//
//  This container stores the information about one crate. A list of this
//  informations can be find at MRawCrateArray
//
/////////////////////////////////////////////////////////////////////////////
#include "MRawCrateData.h"

#include <fstream>

#include <TArrayC.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MRawCrateData);

using namespace std;

MRawCrateData::MRawCrateData() : fDAQCrateNumber(0), fFADCEvtNumber(0), fFADCClockTick(0), fABFlags(0)
{
}

// --------------------------------------------------------------------------
//
//  read the information from a binary input stream about the CRATE DATA,
//  like specified in a TDAS note
//
Bool_t MRawCrateData::ReadEvtOld(istream& fin, UShort_t ver)
{
    if (ver<7)
    {
        fin.read((char*)&fDAQCrateNumber, 2);
        fin.read((char*)&fFADCEvtNumber,  4);
        fin.read((char*)&fFADCClockTick,  4);
        if (ver>1)
            fin.read((char*)&fABFlags, 1);
    }
    else
    {
        //  U8  CrateNumber;    // 0-4
        //  U8  BoardNumber;    // 0-1
        //  U8  ChannelNumber;  // 0-3
        fDAQCrateNumber = 0;
        fin.read((char*)&fDAQCrateNumber, 1); // U8: CrateNumber 0-4

        Byte_t dummyb;
        fin.read((char*)&dummyb, 1); // U8 Board   number 0-1
        fin.read((char*)&dummyb, 1); // U8 Channel number 0-3

        fin.read((char*)&fFADCEvtNumber, 4); // U32 CrateEvtNumber

        // Clock count. The Clock is synchronized with the 10 MHz external clock,
        //  which is feed to for all FADC channels.
        // The units are [psec], which is obviously much smaller than the real accuracy.
        // The ClockTick should be identical for all channels of the same board.
        //  Still keep it for debugging purposes.
        // Like in the 300MHz system this number is extremely useful to check the
        //  integrity of the data.
        UInt_t dummyi;
        fin.read((char*)&dummyi, 4); //  U32 FadcClockTickHi;  // high significant bits
        fin.read((char*)&dummyi, 4); //  U32 FadcClockTickLo;  // low significant bits

        // Trigger Time Interpolation in [psec] (originally it is a double
        // in Acqiris software). Again this number should be identical for
        // all channels in the same board. It is not clear at the moment
        // if this number will be useful in the end, but I propose to keep
        // it. The data volume is increase by <0.1%
        UInt_t dummys;
        fin.read((char*)&dummys, 2); //   U16 TrigTimeInterpol;
    }

    return fin.eof() ? kFALSE : kTRUE;
}

Bool_t MRawCrateData::ReadEvt(istream& fin, UShort_t ver, UInt_t size)
{
    if (ver<11)
        return ReadEvtOld(fin, ver);

    if (size==0)
    {
        *fLog << err << "ERROR - Event header size unknown." << endl;
        return kFALSE;
    }

    if (size<28)
    {
        *fLog << err << "ERROR - Event header too small (<28b)." << endl;
        return kFALSE;
    }

    TArrayC h(size);
    fin.read(h.GetArray(), h.GetSize());
    if (!fin)
        return kFALSE;

    // ----- convert -----
    //const Byte_t  *Char  = reinterpret_cast<Byte_t* >(h.GetArray());
    const UInt_t  *Int   = reinterpret_cast<UInt_t* >(h.GetArray());
    //const Float_t *Float = reinterpret_cast<Float_t*>(h.GetArray());

    fDAQCrateNumber = Int[0];
    fFADCEvtNumber  = Int[3];

    return kTRUE;
}

void MRawCrateData::SkipEvt(istream &fin, UShort_t ver)
{
    fin.seekg(ver>1?11:10);
}

// --------------------------------------------------------------------------
//
//  print all stored information to gLog
//
void MRawCrateData::Print(Option_t *t) const
{
    *fLog << all;
    *fLog << "Crate #" << dec << fDAQCrateNumber << ":  ";
    *fLog << "FADCEventNr = " << fFADCEvtNumber << "  ";
    *fLog << "FADCClockTick = " << fFADCClockTick << " (20MHz)  ";
    *fLog << "ABFlags = 0x" << Form("%02x", fABFlags) << endl;
}
