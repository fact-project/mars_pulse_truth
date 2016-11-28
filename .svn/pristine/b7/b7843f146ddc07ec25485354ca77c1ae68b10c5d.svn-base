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
!   Author(s): Thomas Bretz 10/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MRawRead
//
//  This tasks reads the raw binary file like specified in the TDAS???
//  and writes the data in the corresponding containers which are
//  either retrieved from the parameter list or created and added.
//
//  Input Containers:
//   -/-
//
//  Output Containers:                                                      
//   MRawRunHeader
//   MRawEvtHeader
//   MRawEvtData
//   MRawEvtData2 [MRawEvtData]
//   MRawCrateArray
//   MTime
//
//////////////////////////////////////////////////////////////////////////////
#include "MRawRead.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MParList.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateData.h"
#include "MRawCrateArray.h"

ClassImp(MRawRead);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file.
//
MRawRead::MRawRead(const char *name, const char *title)
    : fForceMode(kFALSE)
{
    fName  = name  ? name  : "MRawRead";
    fTitle = title ? title : "Bas class for reading DAQ files";
}

// --------------------------------------------------------------------------
//
// The PreProcess of this task checks for the following containers in the
// list:
//   MRawRunHeader <output>   if not found it is created
//   MRawEvtHeader <output>   if not found it is created
//   MRawEvtData <output>     if not found it is created
//   MRawCrateArray <output>  if not found it is created
//   MRawEvtTime <output>     if not found it is created (MTime)
//
// If all containers are found or created the run header is read from the
// binary file and printed.  If the Magic-Number (file identification)
// doesn't match we stop the eventloop.
//
// Now the EvtHeader and EvtData containers are initialized.
//
Int_t MRawRead::PreProcess(MParList *pList)
{
    if (!OpenStream())
        return kFALSE;

    //
    //  check if all necessary containers exist in the Parameter list.
    //  if not create one and add them to the list
    //
    fRawRunHeader = (MRawRunHeader*)pList->FindCreateObj("MRawRunHeader");
    if (!fRawRunHeader)
        return kFALSE;

    fRawEvtHeader = (MRawEvtHeader*)pList->FindCreateObj("MRawEvtHeader");
    if (!fRawEvtHeader)
        return kFALSE;

    fRawEvtData1 = (MRawEvtData*)pList->FindCreateObj("MRawEvtData");
    if (!fRawEvtData1)
        return kFALSE;

    fRawEvtData2 = (MRawEvtData*)pList->FindCreateObj("MRawEvtData", "MRawEvtData2");
    if (!fRawEvtData2)
        return kFALSE;

    fRawCrateArray = (MRawCrateArray*)pList->FindCreateObj("MRawCrateArray");
    if (!fRawCrateArray)
        return kFALSE;

    fRawEvtTime = (MTime*)pList->FindCreateObj("MTime");
    if (!fRawEvtTime)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This is a workaround for the oldest runs (file version<3)
// for which no time stamp was available.
// For this runs a fake time stamp is created
//
// Be carefull: This is NOT thread safe!
//
void MRawRead::CreateFakeTime() const
{
    static Double_t tm = 0; // Range of roughly 8min
    const UInt_t ct = (*fRawCrateArray)[0]->GetFADCClockTick();

    tm = ct<tm ? fmod(tm, (UInt_t)(-1))+(UInt_t)(-1)+ct : ct;

    const Double_t mhz = 9.375;                        // [1e6 ticks/s]
    const Double_t t   = tm/mhz;                       // [us]
    const UInt_t ns    = (UInt_t)fmod(t*1e3, 1e6);
    //const UShort_t ms  = (UShort_t)fmod(t/1e3, 1e3);
    const Byte_t s     = (Byte_t)fmod(t/1e6, 60);

    // Create an artificial time stamp!
    UInt_t m = (Byte_t)fmod(t/60e6, 60);
    //const Byte_t h     = (Byte_t)(t/3600e6);
    m += fRawRunHeader->GetRunNumber()*10;
    m %= 360; // 6h

    fRawEvtTime->UpdMagicTime(m/60, m%60, s, ns);
}

// --------------------------------------------------------------------------
//
// Read a single event from the stream
//
Bool_t MRawRead::ReadEvent(istream &fin)
{
    //
    //  Get file format version
    //
    const UShort_t ver = fRawRunHeader->GetFormatVersion();

    //
    // Read in the next EVENT HEADER (see specification),
    // if there is no next event anymore stop eventloop
    //
    const Int_t rc = fRawEvtHeader->ReadEvt(fin, ver, fRawRunHeader->GetHeaderSizeEvt());
    if (rc==kCONTINUE && fForceMode==kFALSE)
    {
        *fLog << err << "Problem found reading the event header... abort." << endl;
        return kFALSE;
    }
    if (rc==kFALSE)
        return kFALSE;

    //
    //  Get number of crates from the run header
    //
    const UShort_t nc = fRawRunHeader->GetNumCrates();

    //
    // Delete arrays which stores the pixel information (time slices)
    //
    fRawEvtData1->ResetPixels(fRawRunHeader->GetNumNormalPixels(),  fRawRunHeader->GetMaxPixId());
    fRawEvtData2->ResetPixels(fRawRunHeader->GetNumSpecialPixels(), fRawRunHeader->GetMinPixId());

    //
    // clear the TClonesArray which stores the Crate Information
    // and create a new array of the correct size
    //
    fRawCrateArray->SetSize(nc);

    //
    // Calculate the number of byte to be skipped in a file if a pixel is not connected
    //
    const UShort_t nskip = fRawRunHeader->GetNumSamples()*fRawRunHeader->GetNumBytesPerSample();

    //
    // read the CRATE DATA (see specification) from file
    //
    Int_t posinarray=0;
    for (int i=0; i<nc; i++)
    {
        if (!fRawCrateArray->GetEntry(i)->ReadEvt(fin, ver, fRawRunHeader->GetHeaderSizeCrate()))
            return kFALSE;

        //fRawEvtData1->ReadEvt(fin, posinarray++);

        const UShort_t npic = fRawRunHeader->GetNumPixInCrate();
        const Byte_t   ab   = fRawCrateArray->GetEntry(posinarray)->GetABFlags();
        for (int j=0; j<npic; j++)
        {
            // calc the spiral hardware pixel number
            const UShort_t ipos = posinarray*npic+j;

            // Get Hardware Id
            const Short_t hwid = fRawRunHeader->GetPixAssignment(ipos);

            // Check whether the pixel is connected or not
            if (hwid==0)
            {
                fin.seekg(nskip, ios::cur);
                continue;
            }

            // -1 converts the hardware pixel Id into the software pixel index
            if (hwid>0)
                fRawEvtData1->ReadPixel(fin,   hwid-1);
            else
                fRawEvtData2->ReadPixel(fin, -(hwid+1));

            if (ver>=7)
                continue;

            if (hwid>0)
                fRawEvtData1->SetABFlag(  hwid-1,  TESTBIT(ab, j));
            else
                fRawEvtData1->SetABFlag(-(hwid+1), TESTBIT(ab, j));
        }
        if (!fin)
            return kFALSE;

        posinarray++;
    }

    // This is a workaround for the oldest runs (version<3)
    // for which no time stamp was available.
    // For this runs a fake time stamp is created
    if (ver<3)
        CreateFakeTime();

    if (rc==kCONTINUE && fForceMode==kTRUE)
        return kCONTINUE;

    fRawEvtData1->SetReadyToSave();
    fRawEvtData2->SetReadyToSave();

    return kTRUE;
}

void MRawRead::SkipEvent(istream &fin)
{
    //
    //  Get file format version
    //
    const UShort_t ver = fRawRunHeader->GetFormatVersion();
    fRawEvtHeader->SkipEvt(fin, ver);

    const UShort_t nc = fRawRunHeader->GetNumCrates();
    for (int i=0; i<nc; i++)
    {
        fRawCrateArray->GetEntry(i)->SkipEvt(fin, ver);
        fRawEvtData1->SkipEvt(fin);
    }
}
