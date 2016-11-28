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
// MRawRunHeader
//
// Root storage container for the RUN HEADER information
//
//
//  RAW DATA FORMAT VERSION
//  =======================
//
//  Format Version 12:
//  -----------------
//   * fIsSigned
//
//  Format Version 11:
//  -----------------
//   * all variables got four bytes
//   * header sizes allow to make the format backward compatible
//   + fHeaderSizeRun
//   + fHeaderSizeEvt
//   + fHeaderSizeCrate
//   + fFileNumber
//   + fNumSamplesRemovedHead
//   + fNumSamplesRemovedTail
//
//  Format Version 10:
//  -----------------
//    ?
//
//  Format Version 9:
//  -----------------
//   + fNumEventsRead;
//   + fSamplingFrequency
//   - fFreqSampling
//   + fFadcResolution;
//   - fNumSignificantBits
//
//  Format Version 8:
//  -----------------
//   + fNumBytesPerSample;
//   + fFreqSampling;
//   + fNumSignificantBits
//   * changes in MRawCrateHeader
//
//  Format Version 7:
//  -----------------
//   - unused
//
//  Format Version 6:
//  -----------------
//   + added CameraVersion
//   + added TelescopeNumber
//   + added ObservationMode
//   + added dummies for TelescopeRa/Dec
//
//  Format Version 5:
//  -----------------
//   - now the sub millisecond information of the time is valid and decoded
//     which enhances the precision from 51.2us to 200ns
//
//  Format Version 4:
//  -----------------
//   - added support for pixels with negative IDs
//
//  Format Version 3:
//  -----------------
//   - ???
//
//  Format Version 2:
//  -----------------
//   - removed mjd from data
//   - added start time
//   - added stop  time
//
//
//  MRawRunHeader CLASS VERSION
//  ===========================
//
//  Format Version 10:
//  -----------------
//   - added fHeaderSizeRun
//   - added fHeaderSizeEvt
//   - added fHeaderSizeCrate
//   - added fFileNumber
//   - increased fSourceEpochChar
//
//  Format Version 7:
//  -----------------
//   - added fNumEventsRead;
//   * renamed fFreqSampling to fSamplingFrequency
//   * renamed fNumSignificantBits to fFadcResolution
//
//  Format Version 6:
//  -----------------
//   - added fNumBytesPerSample;
//   - added fSamplingFrequency;
//   - added fFadcResolution;
//
//  Class Version 5:
//  -----------------
//   - for compatibility with newer camera versions
//
//  Class Version 4:
//  -----------------
//   - added fCameraVersion
//   - added fTelescopeNumber
//   - changed length of fProjectName to 101
//   - changed length of fSourceName  to 81
//
//  Class Version 3:
//  ----------------
//   - enhanced SourceName and ProjectName by one character, because
//     without telling us the guranteed trailing \0-character has
//     skipped
//
//  Class Version 2:
//  ----------------
//   - removed fMJD, fYear, fMonth, fDay
//   - added fRunStart
//   - added fRunStop
// 
//  Class Version 1:
//  ----------------
//   - first implementation
//
////////////////////////////////////////////////////////////////////////////
#include "MRawRunHeader.h"

#include <fstream>
#include <iomanip>

#include <TArrayC.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayS.h"
#include "MString.h"

ClassImp(MRawRunHeader);

using namespace std;

const UShort_t MRawRunHeader::kMagicNumber      = 0xc0c0;
const Byte_t   MRawRunHeader::kMaxFormatVersion =     11;

// --------------------------------------------------------------------------
//
// Default constructor. Creates array which stores the pixel assignment.
//
//
MRawRunHeader::MRawRunHeader(const char *name, const char *title) : fPixAssignment(NULL)
{
    fName  = name  ? name  : "MRawRunHeader";
    fTitle = title ? title : "Raw Run Header Information";

    fPixAssignment = new MArrayS(0);

    // Remark: If we read old MC data from a root file which do not
    // yet contain one of these variable, the value given here is
    // the default. Do not mix files with and without a value if the
    // files with the value do not match the default!
    fFormatVersion=11;
    fSoftVersion=0;
    fTelescopeNumber=1;
    fCameraVersion=1;
    fFadcType=1;
    fRunType=kRTNone;  // use 0xffff for invalidation, 0 means: Data run
    fRunNumber=0;
    fFileNumber=0;
    memset(fProjectName,     0, 101);
    memset(fSourceName,      0,  81);
    memset(fObservationMode, 0,  61);
    fSourceEpochChar[0]=0;
    fSourceEpochChar[1]=0;
    fSourceEpochDate=0;
    fNumCrates=0;
    fNumPixInCrate=0;
    fNumSamplesLoGain=0;
    fNumSamplesHiGain=0;
    fNumEvents=0;
    fNumEventsRead=0;
    fNumBytesPerSample=1;
    fIsSigned = kFALSE;
    fSamplingFrequency=300;
    fFadcResolution=8;

    fHeaderSizeRun=0;
    fHeaderSizeEvt=0;
    fHeaderSizeCrate=0;
}

MRawRunHeader::MRawRunHeader(const MRawRunHeader &h)
{
    fMagicNumber=h.fMagicNumber;                       // File type identifier

    fHeaderSizeRun=h.fHeaderSizeRun;                   // Size of run header
    fHeaderSizeEvt=h.fHeaderSizeEvt;                   // Size of evt header
    fHeaderSizeCrate=h.fHeaderSizeCrate;               // Size of crate header

    fFormatVersion=h.fFormatVersion;                   // File format version
    fSoftVersion=h.fSoftVersion;                       // DAQ software version
    fFadcType=h.fFadcType;                             // FADC type (1=Siegen, 2=MUX)
    fCameraVersion=h.fCameraVersion;                   // Camera Version (1=MAGIC I)
    fTelescopeNumber=h.fTelescopeNumber;               // Telescope number (1=Magic I)
    fRunType=h.fRunType;                               // Run Type
    fRunNumber=h.fRunNumber;                           // Run number
    fFileNumber=h.fFileNumber;                         // File number
    memcpy(fProjectName, h.fProjectName, 101);         // Project name
    memcpy(fSourceName, h.fSourceName, 81);            // Source name
    memcpy(fObservationMode, h.fObservationMode, 61);  // observation mode
    memcpy(fSourceEpochChar, h.fSourceEpochChar, 4);   // epoch char of the source
    fSourceEpochDate=h.fSourceEpochDate;               // epoch date of the source
    fNumCrates=h.fNumCrates;                           // number of electronic boards
    fNumPixInCrate=h.fNumPixInCrate;                   // number of pixels in crate
    fNumSamplesLoGain=h.fNumSamplesLoGain;             // number of logain samples stored
    fNumSamplesHiGain=h.fNumSamplesHiGain;             // number of higain samples stored
    fNumBytesPerSample=h.fNumBytesPerSample;           // number of bytes per sample
    fNumEvents=h.fNumEvents;                           // number of events stored
    fNumEventsRead=h.fNumEventsRead;                   // number of events read by the electronics
    fSamplingFrequency=h.fSamplingFrequency;           // Sampling Frequency [MHz]
    fFadcResolution=h.fFadcResolution;                 // number of significant bits
    fRunStart=h.fRunStart;                             // time of run start
    fRunStop=h.fRunStop;                               // time of run stop
    fPixAssignment = new MArrayS(*h.fPixAssignment);   //-> pixel assignment table
}

// --------------------------------------------------------------------------
//
// Destructor. Deletes the 'pixel-assignment-array'
//
MRawRunHeader::~MRawRunHeader()
{
    delete fPixAssignment;
}

// --------------------------------------------------------------------------
//
// Checks for consistency between two run headers. Checks:
//    fNumCrates
//    fNumPixInCrate
//    fNumSamplesLoGain
//    fNumSamplesHiGain
//    fNumBytesPerSample
//
Bool_t MRawRunHeader::IsConsistent(const MRawRunHeader &h) const
{
    return fNumCrates==h.fNumCrates &&
        fNumPixInCrate==h.fNumPixInCrate &&
        fNumSamplesLoGain==h.fNumSamplesLoGain &&
        fNumSamplesHiGain==h.fNumSamplesHiGain &&
        fNumBytesPerSample==h.fNumBytesPerSample;
}

// --------------------------------------------------------------------------
//
// Swap the assignment of the pixels with hardware id id0 and id1
//
Bool_t MRawRunHeader::SwapAssignment(Short_t id0, Short_t id1)
{
    const Int_t n = fPixAssignment->GetSize();

    // Look-up-table
    UShort_t *lut = fPixAssignment->GetArray();

    // Search for one of the hardware indices to get exchanged
    int i;
    for (i=0; i<n; i++)
        if (lut[i]==id0 || lut[i]==id1)
            break;

    // Check if one of the two pixels were found
    if (i==n)
    {
        *fLog << warn << "WARNING - Assignment of pixels with hardware ID " << id0 << " and " << id1;
        *fLog << " should be exchanged, but none were found." << endl;
        return kTRUE;
    }

    // Store first index
    const Int_t idx0 = i;

    // Search for the other hardware indices to get exchanged
    for (i++; i<n; i++)
        if (lut[i]==id0 || lut[i]==id1)
            break;

    // Check if the second pixel was found
    if (i==n)
    {
        *fLog << err << "ERROR - Assignment of pixels with hardware ID " << id0 << " and " << id1;
        *fLog << " should be exchanged, but only one was found." << endl;
        return kFALSE;
    }

    const Int_t idx1 = i;

    const Short_t p0 = lut[idx0];
    const Short_t p1 = lut[idx1];

    lut[idx0] = p1;
    lut[idx1] = p0;

    *fLog << inf << "Assignment of pixels with hardware ID " << id0 << " and " << id1 << " exchanged." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return Telescope number, runnumber and filenumber on the form:
//      run
// as string for runnumber<1000000 and
//      telescope:run/file
// otherwise.
//
TString  MRawRunHeader::GetStringID() const
{
    return fRunNumber<1000000?MString::Format("%d", fRunNumber):MString::Format("%d:%d/%d", fTelescopeNumber, fRunNumber, fFileNumber);
}

// --------------------------------------------------------------------------
//
// Consistency checks. See code for detils.
//
Bool_t MRawRunHeader::IsConsistent() const
{
    // FIXME: Match first digits of run-number with telescope number

    if (fFormatVersion>10)
    {
        if (GetTypeID()!=fTelescopeNumber     &&
            GetTypeID()!=fTelescopeNumber*10U &&
            GetTypeID()!=5U)
        {
            *fLog << err << "ERROR - Telescope number " << fTelescopeNumber << " doesn't match the first two digits of the run number " << fRunNumber << " and run number doesn't start with 5." << endl;
            return kFALSE;
        }

        // Old formats can not contain a run number larger 999999
        if (fRunNumber<1000000)
        {
            *fLog << err << "ERROR - Run number " << fRunNumber << " smaller than 1000000." << endl;
            return kFALSE;
        }
    }

    // Check for correct number of bytes in data stream
    if (fFormatVersion>7 && fNumBytesPerSample!=2)
    {
        *fLog << err << "ERROR - " << fNumBytesPerSample << " bytes per sample are not supported!" << endl;
        return kFALSE;
    }

    // If we have a vlid stop time check its consistency with the start time
    if (fRunStop!=MTime() && fRunStop<fRunStart)
    {
        *fLog << err << "ERROR - Stop time smaller than start time." << endl;
        return kFALSE;
    }

    // No file numbers larger than 999 allowed in general
    if (fFileNumber>999)
    {
        *fLog << err << "ERROR - File number " << fFileNumber << " larger than 999." << endl;
        return kFALSE;
    }

    // Old formats can not contain a run number larger 0
    if (fFormatVersion<11 && fFileNumber>0)
    {
        *fLog << err << "ERROR - File number " << fFileNumber << " larger than 0." << endl;
        return kFALSE;
    }

    if (fFormatVersion>1)
    {
        // For most of the formats the start time must be valid
        if (fRunStart==MTime())
        {
            *fLog << err << "ERROR - Start time invalid." << endl;
            return kFALSE;
        }

        // For most of the formats an invalid stop time cannot happen if file closed
        if (fMagicNumber==kMagicNumber && fRunStop==MTime())
        {
            *fLog << err << "ERROR - File closed but stop time invalid." << endl;
            return kFALSE;
        }
    }
    return kTRUE;
}

void MRawRunHeader::FixRunNumbers()
{
    if (fFormatVersion<11 || fTelescopeNumber!=1)
        return;

    // Map 1:1001349 to 47:1001395
    if (fRunNumber<48 &&
        fRunStart.GetMjd()>54674.5 && fRunStart.GetMjd()<56476.5)
    {
        fRunNumber += 1001348;
        *fLog << warn << "Format >V10: Wrong run number increased by 1001348 to " << fRunNumber << "." << endl;
    }

    // Map 1001916:1001922 to 1002349:1002355
    if (fRunNumber>1001915 && fRunNumber<1001923 &&
        fRunStart.GetMjd()>54710.5 && fRunStart.GetMjd()<54711.5)
    {
        fRunNumber += 433;
        *fLog << warn << "Format >V10: Wrong run number increased by 434 to " << fRunNumber << "." << endl;
    }

    // Map 10000342:1000343 to 10000351:1000351
    if (fRunNumber>10000342 && fRunNumber<10000352 &&
        fRunStart.GetMjd()>54254.5 && fRunStart.GetMjd()<54255.5)
    {
        fRunNumber -= 9000000;
        *fLog << warn << "Format >V10: Wrong run number decreased by 9000000 to " << fRunNumber << "." << endl;
    }

    if (fRunNumber==1000382 &&
        fRunStart.GetMjd()>54256.5 && fRunStart.GetMjd()<54257.5 &&
        !strcmp(fSourceName, "GRB080605-2347"))
    {
        fFileNumber += 99;
        *fLog << warn << "Format >V10: Ambiguous file number increased by 99 to " << fFileNumber << "." << endl;
    }
}

// --------------------------------------------------------------------------
//
// This implements a fix of the pixel assignment before 25.9.2005
//
// From magic_online (the pixel numbers are hardware indices):
// --------------------------------------------------------------------------
//   From: Florian Goebel <fgoebel@mppmu.mpg.de>
//   Date: Sun Sep 25 2005 - 05:13:19 CEST
//   
//   [...]
//   - problem 2: pixels were swaped
//   - cause: opical fibers were wrongly connected to receiver board
//      554 <-> 559
//      555 <-> 558
//      556 <-> 557
//   - action: reconnect correctly
//   - result: ok now
//   - comment: I don't know when this error was introduced, so backward
//      correction of the data is difficult.
//      Fortunately the effect is not too large since the affected 6 pixels are
//      on the outermost edge of the camera
//      Since this board has special pixels the optical fibers have been
//      unplugged several times.
//   !!!!! Whenever you unplug and reconnect optical fibers make really !!!!!
//   !!!!! sure you connect them back correctly. !!!!!
//   !!!!! Always contact me before you do so and if you have any doubts !!!!!
//   !!!!! how to reconnect the fibers ask me. !!!!!
//      These swapped pixels have only been found by chance when doing the
//      flatfielding.
//   [...]
//
// --------------------------------------------------------------------------
//
// MAGIC runbook CC_2006_04_22_22_28_52.rbk
//
// [2006-04-22 23:14:13]
//
// [...]
// Found 2 pairs of swapped pixels.
// We corrected swapped pixels 54<->55 in the receiver boards. We probably
// swapped today in the camera.
// We did not correct 92<-<93 which are likely swapped. Will check and correct
// tomorrow.
//
// ---
//
// comments:
// - 54<->55 were corrected but have not been swapped, hence they are swapped
//   since then (run 88560 ok, run 88669 swapped; between them mostly dummy and
//   test runs)
// - 92<->93 are never swapped, always ok.
//
// --------------------------------------------------------------------------
//
// MAGIC runbook CC_2006_08_28_19_40_18.rbk
//
// [2006-08-28 23:09:07]
// While doing a flatfielding we have found out that the signals for pixels
// 119 and 120 were swapped. We have fixed it by exchanging the corresponding
// fibers at the input of the receivers (not before the splitters!).
//
// ---
//
// MAGIC runbook CC_2006_08_29_15_19_14.rbk
//
// [2006-08-29 16:43:09]
// In the last hours we have found out and fixed a good number of pixels which
// were swapped: 119-120, 160-161-162 and 210-263. According to Florian,
// 160-161-162 and 210-263 were swapped since November 2005.
//
// ---
//
// mail Florian Goebel (08/30/2006 03:13 PM):
//
// As far as I can tell pixels 161 and 162 as well as 263 and 210 were
// swapped in the trigger. This leads to some inefficiency of the trigger.
// However, they were not swapped in the readout. So, you don't have to
// correct anything in the data for these pixels.
//
// ---
//
// comments:
// - 119-120 swapped between run 93251 (not swapped) and 93283 (swapped)
//   (only testruns between these runs)
//   corrected since run 99354 (== runbook [2006-08-28 23:09:07])
// - 160 never swapped
// - 161-162 were only swapped in the trigger, but nevertheless were
//   "corrected" also in the signal. Hence signal swapped since 99354
//
Bool_t MRawRunHeader::FixAssignment()
{
    if (fTelescopeNumber!=1)
        return kTRUE;

    if (fRunNumber>=53300 && fRunNumber<=68754)
    {
        if (!SwapAssignment(554, 559))
            return kFALSE;
        if (!SwapAssignment(555, 558))
            return kFALSE;
        if (!SwapAssignment(556, 557))
            return kFALSE;
    }

    if (fRunNumber>=93283 && fRunNumber<99354)
    {
        if (!SwapAssignment(119, 120))
            return kFALSE;
    }

    if (fRunNumber>=99354 && fRunNumber<=101789)
    {
        if (!SwapAssignment(161, 162))
            return kFALSE;
        if (!SwapAssignment(210, 263))
            return kFALSE;
    }

    if (fRunNumber>=88669)
    {
        if (!SwapAssignment(54, 55))
            return kFALSE;
    }

    if (fRunNumber>=200000)
    {
        if (!SwapAssignment(428, 429))
            return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fixes to fix bugs in the run header
//
Bool_t MRawRunHeader::Fixes()
{
    FixRunNumbers();

    if (fFormatVersion>8 && fRunNumber>326152 && fTelescopeNumber==1)
    {
        fNumEvents--;
        fNumEventsRead--;
        *fLog << inf << "Format >V8 and Run>M1:326152: Stored number of events decreased by 1." << endl;
    }

    return FixAssignment();
}

// --------------------------------------------------------------------------
//
//  Reading function to read/interpret the file formats 1-10
//
Bool_t MRawRunHeader::ReadEvtOld(istream& fin)
{
    if (fFormatVersion==7)
    {
        *fLog << err << "ERROR - File format V7 was for testing only and is not correctly implemented!" << endl;
        return kFALSE;
    }

    // ----- DAQ software format version -----
    fin.read((char*)&fSoftVersion, 2);     // Total=6


    fFadcType = 1;
    if (fFormatVersion>7)
        fin.read((char*)&fFadcType, 2);

    // ----- Camera geometry and telescope number -----
    fCameraVersion   = 1;
    fTelescopeNumber = 1;
    if (fFormatVersion>5)
    {
        fin.read((char*)&fCameraVersion,   2); // (+2)
        fin.read((char*)&fTelescopeNumber, 2); // (+2)
    }

    // ----- Run information -----
    fin.read((char*)&fRunType,     2); // Total=8

    fin.read((char*)&fRunNumber,   4); // Total=12
    fin.read((char*)&fProjectName, fFormatVersion>5?100:22); // Total=34  (+78)
    fin.read((char*)&fSourceName,  fFormatVersion>5? 80:12); // Total=46  (+58)

    if (fFormatVersion>5)
        fin.read((char*)fObservationMode, 60); // (+60)
    // Maybe we should set fObservationMode to something
    // in case of fFormatVersion<6

    // ----- Source position -----
    fin.seekg(fFormatVersion>5 ? 16 : 8, ios::cur);
    /*
    if (fFormatVersion>5)
    {
        fin.read((char*)&fSourceRa,  4);  // F32 SourceRA;  Total=48
        fin.read((char*)&fSourceDec, 4);  // F32 SourceDEC; Total=52
    }
    fin.read((char*)&fTelescopeRa,  4);   // F32 TelescopeRA;  (+4)
    fin.read((char*)&fTelescopeDec, 4);   // F32 TelescopeDEC; (+4)
    */

    // Maybe we should set these to something
    // in case of fFormatVersion<6
    fin.read((char*)&fSourceEpochChar,  2);     // Total=56
    fin.read((char*)&fSourceEpochDate,  2);     // Total=58

    // ----- Old Start time -----
    if (fFormatVersion<2)                       // Total += 10
    {
        UShort_t y, m, d;
        fin.seekg(4, ios::cur);    // Former fMJD[4],
        fin.read((char*)&y,    2); // Former fDateYear[2]
        fin.read((char*)&m,    2); // Former fDateMonth[2]
        fin.read((char*)&d,    2); // Former fDateDay[2]
        fRunStart.Set(y, m, d, 0, 0, 0, 0);
    }

    // ----- Data Geometry -----

    fin.read((char*)&fNumCrates,        2); // MUX: number of channels
    fin.read((char*)&fNumPixInCrate,    2); // MUX: number of pix in channel
    fin.read((char*)&fNumSamplesLoGain, 2); // MUX: dummy (must be 0 for MUX data)
    fin.read((char*)&fNumSamplesHiGain, 2); // MUX: Number of samples per pixel

    char dummy[16];
    if (fFormatVersion>8)
        fin.read(dummy, 4); // 2xU16 (NumSamplesRemovedHead and NumSamplesRemovedTail)

    // ----- Number of events -----
    fin.read((char*)&fNumEvents, 4);     // Total=70

    if (fFormatVersion>8)
        fin.read((char*)&fNumEventsRead, 4);

    // New in general features: (should they be included in new MAGIC1 formats, too?)
    fNumBytesPerSample  = 1;      // 2 for MUX DATA
    fSamplingFrequency  = 300;
    fFadcResolution     = 8;

    if (fFormatVersion>7)
    {
        fin.read((char*)&fNumBytesPerSample,  2);
        fin.read((char*)&fSamplingFrequency,  2); // [MHz], 2000 for MuxFadc
        fin.read((char*)&fFadcResolution,     1); // nominal resolution [# Bits], 10 for MuxFadc
    }

    // ----- Start/Stop time -----
    if (fFormatVersion>1)
    {
        fRunStart.ReadBinary(fin);              // Total += 7
        fRunStop.ReadBinary(fin);               // Total += 7
    }

    //
    // calculate size of array, create it and fill it
    //
    const Int_t nPixel = fNumCrates*fNumPixInCrate;
    fPixAssignment->Set(nPixel);

    // ----- Pixel Assignement -----
    fin.read((char*)fPixAssignment->GetArray(), nPixel*2);

    if (fFormatVersion<7)
        fin.read(dummy, 16);

    // ----- Fixes for broken files or contents -----
    if (!Fixes())
        return kFALSE;

    // ----- Consistency checks -----
    return IsConsistent();
}

// --------------------------------------------------------------------------
//
// Read in one run header from the binary file
//
Bool_t MRawRunHeader::ReadEvt(istream& fin)
{
    //
    // read one RUN HEADER from the input stream
    //
    fMagicNumber = 0;

    fin.read((char*)&fMagicNumber, 2);          // Total=2

    //
    // check whether the the file has the right file type or not
    //
    if (fMagicNumber != kMagicNumber && fMagicNumber != kMagicNumber+1)
    {
        *fLog << err << "ERROR - Wrong Magic Number (0x" << hex << fMagicNumber << "): Not a Magic File!" << endl;
        return kFALSE;
    }

    if (fMagicNumber == kMagicNumber+1)
        *fLog << warn << "WARNING - This file maybe broken (0xc0c1) - DAQ didn't close it correctly!" << endl;

    // ----- File format version -----
    fin.read((char*)&fFormatVersion, 2);     // Total=4
    if (fFormatVersion==10 || fFormatVersion>kMaxFormatVersion)
    {
        *fLog << err << "ERROR - File format V" << fFormatVersion << " not implemented!" << endl;
        return kFALSE;
    }

    // ----- Process old file formats -----
    if (fFormatVersion<10)
        return ReadEvtOld(fin);

    // ----- Overwrite format version for format 11 -----
    fin.read((char*)&fFormatVersion, 4);
    if (fFormatVersion<11)
    {
        *fLog << err << "ERROR - Format Version <11." << endl;
        return kFALSE;
    }

    // ----- Read Header by size as written in the header -----
    fin.read((char*)&fHeaderSizeRun, 4);
    if (fHeaderSizeRun<346)
    {
        *fLog << err << "ERROR - Event header too small (<388b)." << endl;
        return kFALSE;
    }

    TArrayC h(fHeaderSizeRun-12);
    fin.read(h.GetArray(), h.GetSize());
    if (!fin)
        return kFALSE;

    // ----- convert -----
    const Byte_t  *Char  = reinterpret_cast<Byte_t* >(h.GetArray());
    const UInt_t  *Int   = reinterpret_cast<UInt_t* >(h.GetArray());
    //const Float_t *Float = reinterpret_cast<Float_t*>(h.GetArray());

    // ----- Start interpretation -----

    fHeaderSizeEvt   = Int[0];
    fHeaderSizeCrate = Int[1];
    fSoftVersion     = Int[2];
    fFadcType        = Int[3];
    fCameraVersion   = Int[4];
    fTelescopeNumber = Int[5];
    fRunType         = Int[6];
    fRunNumber       = Int[7];
    fFileNumber      = Int[8];

    memcpy(fProjectName,     Char+ 36, 100);  // 25
    memcpy(fSourceName,      Char+136,  80);  // 20
    memcpy(fObservationMode, Char+216,  60);  // 15

    //F32       fSourceRA     = Float[69];
    //F32       fSourceDEC    = Float[70];
    //F32       fTelescopeRA  = Float[71];
    //F32       fTelescopeDEC = Float[72];

    memcpy(fSourceEpochChar, Char+232, 4);

    fSourceEpochDate    = Int[74];
    fNumCrates          = Int[75];
    fNumPixInCrate      = Int[76];
    fNumSamplesHiGain   = Int[77];
    fNumSamplesLoGain   = 0;

    //fNumSamplesRemovedHead = Int[78];
    //fNumSamplesRemovedTail = Int[79];

    fNumEvents          = Int[80];
    fNumEventsRead      = Int[81];
    fNumBytesPerSample  = Int[82];
    fSamplingFrequency  = Int[83];
    fFadcResolution     = Int[84];

    fRunStart.SetBinary(Int+85);
    fRunStop.SetBinary(Int+91);

    // ----- 388 bytes so far -----

    const UInt_t n = fNumCrates*fNumPixInCrate;
    if (fHeaderSizeRun<388+n*4)
    {
        *fLog << err << "ERROR - Event header too small to contain pix assignment." << endl;
        return kFALSE;
    }

    // ----- Pixel Assignment -----
    fPixAssignment->Set(n);

    for (UInt_t i=0; i<n; i++)
        (*fPixAssignment)[i] = Int[97+i];

    // ----- Fixes for broken files or contents -----
    if (!Fixes())
        return kFALSE;

    // ----- Consistency checks -----
    return IsConsistent();
}
/*
Bool_t MRawRunHeader::WriteEvt(ostream& out) const
{
    //
    // write one RUN HEADER from the input stream
    //
    const UInt_t n = fNumCrates*fNumPixInCrate;

    const UShort_t magicnumber     = kMagicNumber;
    const UInt_t   formatversion   = 11;
    const UInt_t   headersizerun   = (97+n)*4; //???

    // FIXME: Write fixed number (c0c0)
    out.write((char*)&magicnumber,   2);     // Total=2
    out.write((char*)&formatversion, 2);     // Total=4
    out.write((char*)&formatversion, 4);
    out.write((char*)&headersizerun, 4);

    TArrayC h(headersizerun-12);

    // ----- convert -----
    Byte_t  *Char  = reinterpret_cast<Byte_t* >(h.GetArray());
    UInt_t  *Int   = reinterpret_cast<UInt_t* >(h.GetArray());
    //const Float_t *Float = reinterpret_cast<Float_t*>(h.GetArray());

    // ----- Start interpretation -----

    Int[0] = 0; // fHeaderSizeEvt;
    Int[1] = 0; // fHeaderSizeCrate;
    Int[2] = 0; // fSoftVersion;
    Int[3] = fFadcType;
    Int[4] = fCameraVersion;
    Int[5] = fTelescopeNumber;
    Int[5] = fRunType;
    Int[6] = fRunNumber;
    Int[7] = fFileNumber;

    memcpy(Char+ 36, fProjectName,    100);  // 25
    memcpy(Char+136, fSourceName,      80);  // 20
    memcpy(Char+216, fObservationMode, 60);  // 15

    //F32       fSourceRA     = Float[69];
    //F32       fSourceDEC    = Float[70];
    //F32       fTelescopeRA  = Float[71];
    //F32       fTelescopeDEC = Float[72];

    memcpy(Char+232, fSourceEpochChar, 4);

    Int[74] = fSourceEpochDate;
    Int[75] = fNumCrates;
    Int[76] = fNumPixInCrate;
    Int[77] = fNumSamplesHiGain;

    //fNumSamplesRemovedHead = Int[78];
    //fNumSamplesRemovedTail = Int[79];

    Int[80] = fNumEvents;
    Int[81] = fNumEvents; //fNumEventsRead;
    Int[82] = fNumBytesPerSample;
    Int[83] = fSamplingFrequency;
    Int[84] = fFadcResolution;

    fRunStart.GetBinary(Int+85);
    fRunStop.GetBinary(Int+91);

    // ----- 388 bytes so far -----

    //const UInt_t n = fNumCrates*fNumPixInCrate;
    //if (fHeaderSizeRun<(97+n)*4)
    //{
    //    *fLog << err << "ERROR - Event header too small to contain pix assignment." << endl;
    //    return kFALSE;
    //}

    // ----- Pixel Assignment -----
    for (UInt_t i=0; i<n; i++)
        Int[97+i] = (*fPixAssignment)[i];

    out.write(h.GetArray(), h.GetSize());

    return out;
}
*/
// --------------------------------------------------------------------------
//
// Return the run type as string ("Data", "Pedestal", ...), for example
// to print it as readable text.
//
const Char_t *MRawRunHeader::GetRunTypeStr() const
{
    switch (fRunType)
    {
    case kRTData:
        return "Data";
    case kRTPedestal:
        return "Pedestal";
    case kRTCalibration:
        return "Calibration";
    case kRTDominoCal:
        return "DominoCal";
    case kRTLinearity:
        return "Linearity";
    case kRTPointRun:
        return "Point-Run";
    case kRTMonteCarlo:
        return "Monte Carlo";
    case kRTNone:
        return "<none>";
    default:
        return "<unknown>";
    }
}

Char_t MRawRunHeader::GetRunTypeChar() const
{
    switch (fRunType&0xff)
    {
    case kRTData:
    case kRTPointRun:
        return 'D';
    case kRTPedestal:
        return 'P';
    case kRTCalibration:
        return 'C';
    case kRTDominoCal:
        return 'L';
    case kRTLinearity:
        return 'N';
    default:
        return ' ';
    }
}

// --------------------------------------------------------------------------
//
// print run header information on *fLog. The option 'header' supresses
// the pixel index translation table.
//
void MRawRunHeader::Print(Option_t *t) const
{
    *fLog << all << endl;
    *fLog << "MagicNumber:  0x" << hex << fMagicNumber << " - ";
    switch (fMagicNumber)
    {
    case kMagicNumber:   *fLog << "OK";               break;
    case kMagicNumber+1: *fLog << "File not closed!"; break;
    default:             *fLog << "Wrong!";           break;
    }
    *fLog << endl;
    *fLog << "Versions:     " << dec << "Format=";
    if (fFormatVersion==0xf172)
        *fLog << "FITS[f172]";
    else
        *fLog << fFormatVersion;
    *fLog << "  Software=" << fSoftVersion << "  ";
    if (fFormatVersion>5)
    {
        *fLog << "Camera=";
        if (fCameraVersion==0xfac7)
            *fLog << "FACT[fac7]";
        else
            *fLog << fCameraVersion;
    }
    *fLog << endl;
    if (fFormatVersion>10 && fHeaderSizeRun>0 && fHeaderSizeEvt>0 && fHeaderSizeCrate>0)
        *fLog << "Header sizes: " << fHeaderSizeRun << "b (run), " << fHeaderSizeEvt << "b (evt), " << fHeaderSizeCrate << "b (crate)" << endl;
    if (fRunNumber>0)
    {
        if (fFormatVersion>5 && fTelescopeNumber>0)
            *fLog << "Telescope:    " << fTelescopeNumber << endl;
        *fLog << "RunNumber:    " << fRunNumber;
        if (fFormatVersion>10)
            *fLog << "/" << fFileNumber << " (id=" << GetFileID() << ")";
        *fLog << " (Type=" << GetRunTypeStr() << ")" << endl;
    }
    if (fFormatVersion>7)
    {
        *fLog << "FadcType:     " << fFadcType << " (";
        switch (fFadcType)
        {
        case 1:      *fLog << "Siegen"; break;
        case 2:      *fLog << "MUX"; break;
        case 0xfac7: *fLog << "FACT DRS4"; break;
        case 0xffff: *fLog << "artificial"; break;
        default: *fLog << "unknown";
        }
        *fLog << ")" << endl;
    }
    *fLog << "ProjectName: '" << fProjectName << "'" << endl;
    if (fFormatVersion>5)
        *fLog << "Observation: '" << fObservationMode << "'" << endl;
    if (fSourceName[0]!=0 || fSourceEpochChar[0]!=0 || fSourceEpochDate!=0)
    {
        *fLog << "Source:      '" << fSourceName << "' " << "  ";
        *fLog << fSourceEpochChar << dec << fSourceEpochDate << endl;
    }
    if (fRunStart)
        *fLog << "Run Start:    " << fRunStart << endl;
    if (fRunStop)
        *fLog << "Run Stop:     " << fRunStop << endl;
    if (fNumCrates>0 || fNumPixInCrate>0)
        *fLog << "Crates:       " << fNumCrates << " x " << fNumPixInCrate << " Pixel/Crate = " << fNumCrates*fNumPixInCrate << " Pixel/Evt" << endl;
    if (GetNumConnectedPixels()>0)
        *fLog << "Num Pixels:   " << GetNumNormalPixels() << " (normal) + " << GetNumSpecialPixels() << " (special) = " << GetNumConnectedPixels() << " (total)" << endl;
    if (fFormatVersion>6)
        *fLog << "Sampling:     " << fSamplingFrequency << "MHz with " << (int)fFadcResolution << " significant bits" << endl;
    *fLog << "Samples:      " << fNumSamplesHiGain << "/" << fNumSamplesLoGain << " (hi/lo) * " << fNumBytesPerSample << "B/sample = ";
    if (fNumCrates>0)
        *fLog << (fNumSamplesLoGain+fNumSamplesHiGain) * fNumCrates * fNumPixInCrate * fNumBytesPerSample/1000 << "kB/Evt" << endl;
    else
        *fLog << (fNumSamplesLoGain+fNumSamplesHiGain) * fNumBytesPerSample << "B/pix" << endl;
    if (fNumEvents>0 || fNumEventsRead>0)
    {
        *fLog << "Evt Counter:  " << fNumEvents;
        if (fFormatVersion>8)
            *fLog << " (read=" << fNumEventsRead << ")";
        *fLog << endl;
    }

    if (TString(t).Contains("header", TString::kIgnoreCase))
        return;

    *fLog << inf3 << "Assignment:" << hex << endl;
    for (int i=0; i<GetNumPixel(); i++)
        *fLog << setfill('0') << setw(3) << (*fPixAssignment)[i] << " ";

    *fLog << dec << setfill(' ') << endl;
}

// --------------------------------------------------------------------------
//
// Return the assigned pixel number for the given FADC channel
//
Short_t MRawRunHeader::GetPixAssignment(UShort_t i) const
{
    // FIXME: Do we need a range check here?
    return (Short_t)(*fPixAssignment)[i];
}

// --------------------------------------------------------------------------
//
// Return the number of pixel which are markes as connected in the
// pix assignment (!=0)
//
UShort_t MRawRunHeader::GetNumConnectedPixels() const
{
    const Int_t num = fPixAssignment->GetSize();

    UShort_t rc = 0;
    for (int i=0; i<num; i++)
    {
        if (GetPixAssignment(i)!=0)
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Return the number of pixel which are markes as connected and so-called
// 'normal' pixels in the pix assignment (>0)
//
UShort_t MRawRunHeader::GetNumNormalPixels() const
{
    const Int_t num = fPixAssignment->GetSize();

    UShort_t rc = 0;
    for (int i=0; i<num; i++)
    {
        if (GetPixAssignment(i)>0)
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Return the number of pixel which are markes as connected and so-called
// 'special' pixels in the pix assignment (<0)
//
UShort_t MRawRunHeader::GetNumSpecialPixels() const
{
    const Int_t num = fPixAssignment->GetSize();

    UShort_t rc = 0;
    for (int i=0; i<num; i++)
    {
        if (GetPixAssignment(i)<0)
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Return the maximum id which exists in the pix assignment
//
UShort_t MRawRunHeader::GetMaxPixId() const
{
    const Int_t num = fPixAssignment->GetSize();

    Short_t rc = 0;
    for (int i=0; i<num; i++)
        rc = TMath::Max(GetPixAssignment(i), rc);

    return rc;
}

// --------------------------------------------------------------------------
//
// Return minus th minimum id which exists in the pix assignment
//
UShort_t MRawRunHeader::GetMinPixId() const
{
    const Int_t num = fPixAssignment->GetSize();

    Short_t rc = 0;
    for (int i=0; i<num; i++)
        rc = TMath::Min(GetPixAssignment(i), rc);

    return (UShort_t)-rc;
}

// --------------------------------------------------------------------------
//
// Return the number of pixel in this event.
//
// WARNING: This is the number of pixels stored in this file which is
//          a multiple of the number of pixels per crate and in general
//          a number which is larger than the camera size!
//
//          To know the range of the pixel indices please use the geometry
//          container!
//
UShort_t MRawRunHeader::GetNumPixel() const
{
    return fPixAssignment->GetSize();
}

// --------------------------------------------------------------------------
//
// Returns absolute size in bytes of the run header as read from a raw file.
// This must be done _after_ the header is read, because the header doesn't
// have a fixed size (used in MRawSocketRead)
//
Int_t MRawRunHeader::GetNumTotalBytes() const
{
    switch (fFormatVersion)
    {
    case 1:
        return 80+fNumCrates*fNumPixInCrate*2+16;
    case 2:
    case 3:
    case 4:
    case 5:
        return 84+fNumCrates*fNumPixInCrate*2+16;
    case 6:
        return 84+fNumCrates*fNumPixInCrate*2+16 +4+78+58+60+8;
    case 7:
        return 84+fNumCrates*fNumPixInCrate*2+16 +4+78+58+60+8 +3-16;
    }
    return 0;
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
// This is a (prelimiary) way to setup an existing FADC system.
//
//  1: Siegen FADCs
//  2: MUX FADCs
//
void MRawRunHeader::InitFadcType(UShort_t type)
{
    switch (type)
    {
    case 1:
        fNumSamplesHiGain  =   15;
        fNumSamplesLoGain  =   15;
        fNumBytesPerSample =    1;    // number of bytes per sample
        fSamplingFrequency =  300;    // Sampling Frequency [MHz]
        fFadcResolution    =    8;    // number of significant bits
        break;
    case 2:
        fNumSamplesHiGain  =   50;
        fNumSamplesLoGain  =    0;
        fNumBytesPerSample =    2;    // number of bytes per sample
        fSamplingFrequency = 2000;    // Sampling Frequency [MHz]
        fFadcResolution    =   12;    // number of significant bits
        break;
    case 3:
        fNumSamplesHiGain  =  150;
        fNumSamplesLoGain  =    0;
        fNumBytesPerSample =    2;    // number of bytes per sample
        fSamplingFrequency = 1000;    // Sampling Frequency [MHz]
        fFadcResolution    =   12;    // number of significant bits
        break;
    }

    fFadcType = type;
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Init a camera
//
void MRawRunHeader::InitCamera(UShort_t type, UShort_t pix)
{
    switch (type)
    {
    case 1:
        fNumCrates     =   1;
        fNumPixInCrate = 577;
        break;
    case 2:
        fNumCrates     =   1;
        fNumPixInCrate = 703;
        break;
    case (UShort_t)-1:
        fNumCrates     =   1;
        fNumPixInCrate = pix;
        break;
    }

    fCameraVersion = type;

    const Int_t n = fNumCrates*fNumPixInCrate;

    fPixAssignment->Set(n);

    for (int i=0; i<n; i++)
        (*fPixAssignment)[i] = i+1;
}

void MRawRunHeader::InitFact(UShort_t num, UShort_t pix, UShort_t samples, UShort_t *map)
{
    fNumCrates     = num;
    fNumPixInCrate = pix;
    fCameraVersion = 0xfac7;
    fFadcType      = 0xfac7;

    fPixAssignment->Set(num*pix);

    if (map)
        for (int i=0; i<num*pix; i++)
            (*fPixAssignment)[i] = map[i]+1;
    else
    {
        for (int i=0; i<num*pix; i++)
            (*fPixAssignment)[i] = i+1;
    }

    fNumSamplesHiGain  = samples;
    fNumSamplesLoGain  = 0;
    fNumBytesPerSample = 2;      // number of bytes per sample
    fIsSigned          = kTRUE;
    fSamplingFrequency = 2000;   // Sampling Frequency [MHz]
    fFadcResolution    = 12;     // number of significant bits
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Set telescope number, run-number and file-number
//
void MRawRunHeader::SetRunInfo(UShort_t tel, UInt_t run, UInt_t file)
{
    fTelescopeNumber = tel;
    fRunNumber       = run;
    fFileNumber      = file;
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Set source-name, epoch (default J) and date (default 2000)
//
void MRawRunHeader::SetSourceInfo(const TString src, char epoch, UShort_t date)
{
    strncpy(fSourceName, src.Data(), 80);

    fSourceEpochChar[0] = epoch;     // epoch char of the source
    fSourceEpochDate    = date;      // epoch date of the source
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Set run-start and -stop time
//
void MRawRunHeader::SetRunTime(const MTime &start, const MTime &end)
{
    fRunStart = start;
    fRunStop  = end;
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Set project name and observation mode
//
void MRawRunHeader::SetObservation(const TString mode, const TString proj)
{
    strncpy(fProjectName,     proj.Data(), 100);
    strncpy(fObservationMode, mode.Data(),  60);
}

// --------------------------------------------------------------------------
//
// Monte Carlo Interface
//
//  Set number of events in file.
//
void MRawRunHeader::SetNumEvents(UInt_t num)
{
    fNumEvents     = num;
    fNumEventsRead = num;
}

// --------------------------------------------------------------------------
//
//  NumSamples: 50
//  NumBytePerSample: 2
//  SamplingFrequency: 2000
//  FadcResolution: 12
//  FadcType: XXXX
//
Int_t MRawRunHeader::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "NumSamples", print))
    {
        rc = kTRUE;
        fNumSamplesHiGain = GetEnvValue(env, prefix, "NumSamples", fNumSamplesHiGain);
        fNumSamplesLoGain = 0;
    }

    if (IsEnvDefined(env, prefix, "NumBytesPerSample", print))
    {
        rc = kTRUE;
        fNumBytesPerSample = GetEnvValue(env, prefix, "NumBytesPerSample", fNumBytesPerSample);
    }

    if (IsEnvDefined(env, prefix, "SamplingFrequency", print))
    {
        rc = kTRUE;
        fSamplingFrequency = GetEnvValue(env, prefix, "SamplingFrequency", fSamplingFrequency);
    }
    
    if (IsEnvDefined(env, prefix, "IsSigned", print))
    {
        rc = kTRUE;
        fIsSigned = GetEnvValue(env, prefix, "IsSigned", fIsSigned);
    }

    if (IsEnvDefined(env, prefix, "FadcResolution", print))
    {
        rc = kTRUE;
        fFadcResolution = GetEnvValue(env, prefix, "FadcResolution", fFadcResolution);
    }
    // Saturation behaviour etc.
    if (IsEnvDefined(env, prefix, "FadcType", print))
    {
        //rc = kTRUE;
        //TString type = GetEnvValue(env, prefix, "FadcType", "");
        // Eval "Siegen", "MUX", Dwarf"
    }
    else
        if (rc)
            fFadcType = 0xffff; // "Artificial"

    return rc;
}

