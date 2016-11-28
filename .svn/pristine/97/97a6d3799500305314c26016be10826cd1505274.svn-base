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
// MRawEvtHeader 
//    
// One Event is a sample of FADC measurements of different Pixels 
// (Photomultipliers) from the Camera of MAGIC. So all data (FADC) of the 
// interesting pixels are the modules of an event. To describe pixels the 
// class MRawPixel is used and the class MRawCrate to describe Crates.
// To define a single events some other data members are needed 
// (Time of the events, tirgger pattern of event..)
// 
// To describe one event on the level of FADC values the Class MRawEvtHeader is
// created. It has the following data members: 
//
// UInt_t    fDAQEvtNumber
// -----------------------
// This it the number of the Event in one 
// data run. The first event in this run get
// the number zero. The next one is one bigger.
//
// Assuming that one run takes 1 hour and a
// triggerrate of 1kHz the number must be able
// to reach 3.6e6 Events. To reach this number
// you need at least 22 bits. This is the reason
// why we use an integer (of root type UInt_t)
// with a range to 4.2e9. 
//
// MTime   fRawEvtTime
// -------------------
// Time of the event. 
// The start point of the time determination can be
// the millenium. From that start point the time is
// measured in 200ns-count. One day for example
// contains 432.e9 counts. An array of two unsigned Int is able to 
// contain 1.8e19 200ns-counts. This corresponds to 41.e6
// days. This should be more than the livetime of MAGIC.
// Private member of MTime.h
//
// UInt_t  fNumTrigLvl1
// --------------------
//
// Number of first level trigger
// This member counts the number of First Level Trigger
// between the last and this event. May be that due to 
// dead time of the DAQ this number is different from 1.
// If the DAQ is fast enough, this value should be 1. 
// This may be usefull in GammaRayBursts and if we 
// apply a data reduction in the DAQ-chain, which selects
// only good events. 
//
// UInt_t  fNumTrigLvl2
// --------------------
//
// Number of second level trigger
// This member counts the number of Second Level Trigger
// between the last and this event. 
//
// UInt_t  fTrigPattern[0]
// -----------------------
// Trigger Pattern used for this event
// Each event triggers for a particular configuration and each  
// configuration should have an ID (which is not fixed yet).
//
// UInt_t  fCalibPattern == fTrigPattern[1]  
// -----------------------
// Calibration Pattern used for this event
// Each (calibration) event uses a particular LEDs configuration and 
// particular strength and colour of the continunous light.
// Bits 1-16: Pulser slot pattern: 16 LEDs slots.
// Bits 17: CT1 Pulser
// Bits 21-24: Colour of Continous light source:
// Bits 25-32: Strength of continuous light source: 256 level
//
// UShort_t fAllLowGainOn
// ----------------------
// Type of Trigger. 
// This is a Byte (8 bit) to indicated if any of the pixels 
// have a non-negligible low gain (1) or not (0)
//
// Class Version 2:
// ---------------
//  - added fCalibPattern
//
/////////////////////////////////////////////////////////////////////////////
#include "MRawEvtHeader.h"

#include <fstream>

#include <TArrayC.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MArrayB.h"
#include "MRawRunHeader.h"

ClassImp(MRawEvtHeader);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Create the array to store the data.
//
MRawEvtHeader::MRawEvtHeader(const char *name, const char *title)
    : fTime(0), fNumTrigLvl1(0), fNumTrigLvl2(0), fNumLoGainOn(0), fPixLoGainOn(0)
{
    fName  = name  ? name  : "MRawEvtHeader";
    fTitle = title ? title : "Raw Event Header Information";

    //
    //   set all member to zero, init the pointer to ClonesArray,
    //
    fPixLoGainOn = new MArrayB;

    Clear();
}

// --------------------------------------------------------------------------
//
// Destructor. Deletes the array to store pixlogainon
//
MRawEvtHeader::~MRawEvtHeader()
{
    delete fPixLoGainOn;
}

// --------------------------------------------------------------------------
//
// you have to init the conatainer before you can read from
// a raw binary file
//
void MRawEvtHeader::InitRead(MRawRunHeader *rh, MTime *t)
{
    //
    // this is the number of entries in the array like specification
    //
    UInt_t fN = (rh->GetNumCrates() * rh->GetNumPixInCrate() + 7) / 8;

    //
    // initialize the array
    //
    fPixLoGainOn->Set(fN);

    //
    // this is the conatiner where we have to store the time of the event we
    // read from the input stream
    //
    fTime = t;
}

// --------------------------------------------------------------------------
//
//   Implementation of the Clear function
//
//   Resets all members to zero, clear the list of Pixels
//
void MRawEvtHeader::Clear(Option_t *)
{
    fDAQEvtNumber   = 0;
    fNumTrigLvl1    = 0;
    fNumTrigLvl2    = 0;
    fTrigPattern[0] = 0;
    fTrigPattern[1] = 0;
    fTrigType       = 0;
    fNumLoGainOn    = 0;
}

// --------------------------------------------------------------------------
//
//  This member function prints all Data of one Event to *fLog.
//
void MRawEvtHeader::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << "DAQEvtNr: " << dec << fDAQEvtNumber << "  (";
    *fLog << "Trigger: ";
    *fLog << "NumLvl1=" << fNumTrigLvl1 << " ";
    *fLog << "NumLvl2=" << fNumTrigLvl2 << " ";
    *fLog << "Pattern=" << hex << setfill('0');
    *fLog << setw(8) << fTrigPattern[0];
    *fLog << setw(8) << fTrigPattern[1] << " " << dec;

    *fLog << "Type=";
    switch (fTrigType)
    {
    case 0:
        *fLog << "Trigger";
        break;
    case 1:
        *fLog << "Pedestal";
        break;
    case 2:
        *fLog << "Calibration";
        break;
    case 3:
        *fLog << "PinDiode";
        break;
    }
    *fLog << ")" << endl;
    *fLog << "Number of Lo Gains On: " << fNumLoGainOn << endl;

    TString str(o);
    str.ToLower();

    if (str.Contains("nogains"))
        return;

    for (unsigned int i=0; i<fPixLoGainOn->GetSize(); i++)
    {
        for (int j=0; j<8; j++)
        {
            const UInt_t on = (*fPixLoGainOn)[i]&(1<<j) ? 1 : 0;
            *fLog << on;
        }
    }
    if (fPixLoGainOn->GetSize())
        *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Return the size in bytes of the event header.
//
Int_t MRawEvtHeader::GetNumBytes() const
{
    return 36+fPixLoGainOn->GetSize();
}

// --------------------------------------------------------------------------
//
// Used to set the header information. This is for MC only. NEVER, NEVER
// use this somewhere else!
//
void MRawEvtHeader::FillHeader(UInt_t uiN, Float_t ulTP)
{
    fDAQEvtNumber = uiN;
    fTrigPattern[0] = (UInt_t)(ulTP/4294967296.0) ;
    fTrigPattern[1] = (UInt_t)(ulTP-fTrigPattern[0]*4294967296.0);
}

// --------------------------------------------------------------------------
//
// Decode the binary Time Stamp. For more detailed information see the
// source code.
//
Bool_t MRawEvtHeader::DecodeTime(const UInt_t tm[2], UShort_t ver) const
{
    //
    // SuperSecond (20 bits giving hh:mm:ss)
    // ------------
    // 
    // Reading the hours:
    // Swap bits: 23->16, 22->17, 21->16, 20->19
    //
    UInt_t abstime[2] = { tm[0], tm[1] };

    abstime[0] =
        (abstime[0]>>7 & 0x00010000) |
        (abstime[0]>>5 & 0x00020000) |
        (abstime[0]>>3 & 0x00040000) |
        (abstime[0]>>1 & 0x00080000) |
        (abstime[0]    & 0xff00ffff);

    // 
    // SubSecond (24 bits giving number of clock ticks of a 5Mhz signal since 
    // the beginning of last second, i.e., number of ns with a precision of to
    // 200 ns) 
    // ----------
    //
    // The last 8 bits must be flipped.
    //
    abstime[1] ^= 0x000000ff;

    //
    // Due to a problem with one Digital Module, three of the less significant
    // eight bits of the subsecond are corrupted. So, until new DM's arrive to
    // La Palma, we won't use the eight first bits of the subsecond. 
    // This reduces the precision from 200 ns to of 51.2 us. (ver<5)
    //
    if (ver<5)
        abstime[1] &= 0xffffff00;

    //
    // Decode Time Stamp
    //
    const Byte_t h  = (abstime[0]>>18 & 0x3)*10 + (abstime[0]>>14 & 0xf);
    const Byte_t m  = (abstime[0]>>11 & 0x7)*10 + (abstime[0]>> 7 & 0xf);
    const Byte_t s  = (abstime[0]>> 4 & 0x7)*10 + (abstime[0]>> 0 & 0xf);
    const UInt_t ns =  abstime[1]*200;

    //
    // Update the time stamp with the current event time.
    // Make sure, that the time stamp was initialized correctly
    // with the start-date/time of the run (after reading the run header)
    //
    if (fTime->UpdMagicTime(h, m, s, ns))
        return kTRUE;

    *fLog << warn << "WARNING - Time (" << Form("%2d:%02d:%02d,%09d", h, m, s, ns);
    *fLog << ") in header of event #" << dec << fDAQEvtNumber << " invalid..." << endl;
 
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// read the EVENT HEADER information from the input stream
// return FALSE if there is now header anymore, else TRUE
//
// For version>2 we expect to have a valid time-stamp in the files.
//
// Updates the time stamp with the current event time.
// Make sure, that the time stamp was initialized correctly
// with the start-date/time of the run (after reading the run header)
//
// Remark: This 'feature' disallows single runs of more than 11h!
//
Int_t MRawEvtHeader::ReadEvtOld(istream &fin, UShort_t ver)
{
    Int_t rc = kTRUE;

    fin.read((char*)&fDAQEvtNumber, 4);  // Total=4
    if (!fin)
        return kFALSE;

    UInt_t abstime[2];
    fin.read((char*)abstime,        8);  // Total=12

    if (ver>2)
        if (!DecodeTime(abstime, ver))
            rc = kCONTINUE;

    Byte_t dummy[4];
    fin.read((char*)&fNumTrigLvl1,  4);  // Total=16
    fin.read((char*)&fNumTrigLvl2,  4);  // Total=20
    fin.read((char*)fTrigPattern,   8);  // Total=28
    fin.read((char*)&fTrigType,     2);  // Total=30
    if (ver<=6)
    {
        fin.read((char*)dummy,          2);  // Total=32, was fAllLoGainOn
        fin.read((char*)fPixLoGainOn->GetArray(), fPixLoGainOn->GetSize());

        fNumLoGainOn = 0;
        for (unsigned int i=0; i<fPixLoGainOn->GetSize(); i++)
            for (int j=0; j<8; j++)
                if ((*fPixLoGainOn)[i] & (1<<j))
                    fNumLoGainOn++;

        fin.read((char*)&dummy, 4);
    }
    else
    {
        // No LoGains for version 7 data
        fPixLoGainOn->Reset();
        fNumLoGainOn = 0;
    }

    return fin.eof() ? kFALSE : rc;
}

// --------------------------------------------------------------------------
//
Int_t MRawEvtHeader::ReadEvt(istream &fin, UShort_t ver, UInt_t size)
{
    if (ver<11)
        return ReadEvtOld(fin, ver);

    if (size==0)
    {
        *fLog << err << "ERROR - Event header size unknown." << endl;
        return kFALSE;
    }

    if (size<32)
    {
        *fLog << err << "ERROR - Event header too small (<32b)." << endl;
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

    fDAQEvtNumber   = Int[0];
    // Decode Time
    fNumTrigLvl1    = Int[3];
    fNumTrigLvl2    = Int[4];
    fTrigPattern[0] = Int[5];
    fTrigPattern[1] = Int[6];
    fTrigType       = Int[7];

    // No LoGains for version 7 data
    fPixLoGainOn->Reset();
    fNumLoGainOn = 0;

    return DecodeTime(Int+1, ver) ? kTRUE : kCONTINUE;
}

void MRawEvtHeader::SkipEvt(istream &fin, UShort_t ver)
{
    fin.seekg(GetNumBytes(), ios::cur);
}

// --------------------------------------------------------------------------
//
//   Low level decoding of the trigger pattern.
//   The trigger pattern consists of 16 bits (8+8 bits) generated by the 
//   trigger system.
//   The first 8 bits correspond to the trigger configuration before the
//   prescaling, the others after prescaling.
//   The meaning of the configuration depends on the chosen trigger table
//   (that is how the trigger has been programmed) and must be interpreted 
//    at higher level by the analysis. 
//   Bit structure:
//          not prscd | prscaled
//           xxxx xxxx xxxx xxxx    <-- pattern (x=0,1)
//     bit   7654 3210 7654 3210
//          H                   L
//
//    e.g.   1000 0000 1000 1000 (hex: 8080) is the pattern when no
//         L2 trigger selection and no prescaling is applied. 
//       
//    Up to now only fTrigPattern[0] is used.
//
UInt_t MRawEvtHeader::GetTriggerID() const
{
    return fTrigPattern[0];
}

UInt_t MRawEvtHeader::GetCalibrationPattern() const
{
    return fTrigPattern[1];
}

UInt_t MRawEvtHeader::GetPulserSlotPattern() const
{
    return (fTrigPattern[1] >> 16) & 0x1ffff;
}

