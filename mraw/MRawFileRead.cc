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
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MRawFileRead
//
//  This tasks reads the raw binary file like specified in the TDAS???
//  and writes the data in the corresponding containers which are
//  either retrieved from the parameter list or created and added.
//
//  Use SetInterleave() if you don't want to read all events, eg
//    SetInterleave(5) reads only each 5th event.
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   MRawRunHeader, MRawEvtHeader, MRawEvtData, MRawCrateArray, MRawEvtTime
//
//////////////////////////////////////////////////////////////////////////////
#include "MRawFileRead.h"

#include <errno.h>

#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "izstream.h"
#include "MTime.h"
#include "MParList.h"
#include "MStatusDisplay.h"

#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateData.h"
#include "MRawCrateArray.h"

ClassImp(MRawFileRead);

using namespace std;

/*  ----------- please don't delete and don't care about (Thomas) ------------
#define kBUFSZ 64 //1024*1024*64
#include <iomanip.h>
class bifstream : public istream, public streambuf
{
private:
    char fBuffer[kBUFSZ]; //!
    FILE *fd;

    int sync()
    {
        memset(fBuffer, 0, kBUFSZ);
        return 0; 
    }
    int underflow()
    {
        int sz=fread(fBuffer, kBUFSZ, 1, fd);
        //int sz=fread(fBuffer, 1, kBUFSZ, fd);
        setg(fBuffer, fBuffer, fBuffer+kBUFSZ);

        return sz==1 ? *(unsigned char*)fBuffer : EOF;//EOF;
        //return sz==kBUFSZ ? *(unsigned char*)fBuffer : EOF;//EOF;
    }
public:
    bifstream(const char *name) : istream(this)
    {
        fd = fopen(name, "rb");
        setbuf(fBuffer, kBUFSZ);
    }
};
*/

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file.
//
MRawFileRead::MRawFileRead(const char *fname, const char *name, const char *title)
    : fFileNames(NULL), fNumFile(0), fNumTotalEvents(0), fIn(NULL), fParList(NULL), fInterleave(1), fForce(kFALSE), fIsMc(kFALSE)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Read task to read DAQ binary files";

    fFileNames = new TList;
    fFileNames->SetOwner();

    if (fname!=NULL)
        AddFile(fname);
}

// --------------------------------------------------------------------------
//
// Destructor. Delete input stream.
//
MRawFileRead::~MRawFileRead()
{
    delete fFileNames;
    if (fIn)
        delete fIn;
}

Byte_t MRawFileRead::IsFileValid(const char *name)
{
    izstream fin(name);
    if (!fin)
        return 0;

    Byte_t c[4];
    fin.read((char*)c, 4);
    if (!fin)
        return 0;

    if (c[0]!=0xc0)
        return 0;

    if (c[1]==0xc0)
        return 1;

    if (c[1]==0xc1)
        return 2;

    return 0;
}

// --------------------------------------------------------------------------
//
// Add a new file to a list of files to be processed, Returns the number
// of files added. (We can enhance this with a existance chack and
// wildcard support)
//
Int_t MRawFileRead::AddFile(const char *fname, Int_t entries)
{
    TNamed *name = new TNamed(fname, "");
    fFileNames->AddLast(name);
    return 1;

}

istream *MRawFileRead::OpenFile(const char *filename)
{
    return new izstream(filename);
}

Bool_t MRawFileRead::ReadRunHeader(istream &fin)
{
    //
    // Read RUN HEADER (see specification) from input stream
    //
    if (!fRawRunHeader->ReadEvt(fin))
        if (!fForce)
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Int_t MRawFileRead::OpenNextFile(Bool_t print)
{
    //
    // open the input stream and check if it is really open (file exists?)
    //
    if (fIn)
        delete fIn;
    fIn = NULL;

    //
    // Check for the existance of a next file to read
    //
    TObject *file = fFileNames->At(fNumFile);
    if (!file)
        return kFALSE;

    //
    // open the file which is the first one in the chain
    //
    const char *name = file->GetName();

    const char *expname = gSystem->ExpandPathName(name);
    fIn = OpenFile(expname);

    const Bool_t noexist = !(*fIn);
    if (noexist)
    {
        *fLog << err << "Cannot open file " << expname << ": ";
        *fLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
    }
    else
    {
        *fLog << inf << "Open file: '" << name << "'" << endl;

        if (fDisplay)
        {
            // Show the new file name and the event number after which
            // the new file has been opened
            TString txt = GetFileName();
            txt += " @ ";
            txt += GetNumExecutions()-1;
            fDisplay->SetStatusLine2(txt);
        }
    }

    delete [] expname;

    if (noexist)
        return kERROR;

    fNumFile++;

    MRawRunHeader h(*fRawRunHeader);

    if (!ReadRunHeader(*fIn))
        return kERROR;

    if (!(*fIn))
    {
        *fLog << err << "Error: Accessing file '" << name << "'" << endl;
        return kFALSE;
    }

    if (h.IsValidRun() && !fRawRunHeader->IsConsistent(h))
    {
        *fLog << err << "Error: Inconsistency between previous header and '" << name << "' found." << endl;
        fRawRunHeader->Print();
        return kERROR;
    }

    if (!print)
        return kTRUE;

    //
    // Print Run Header
    //
    fRawRunHeader->Print("header");
    *fRawEvtTime = fRawRunHeader->GetRunStart();

    fNumEvents += fRawRunHeader->GetNumEvents();

    fRawRunHeader->SetReadyToSave();

    //
    // Give the run header information to the 'sub-classes'
    // Run header must be valid!
    //
    fRawEvtHeader->InitRead(fRawRunHeader, fRawEvtTime);
    fRawEvtData1 ->InitRead(fRawRunHeader);
    fRawEvtData2 ->InitRead(fRawRunHeader);

    if (!InitReadData(*fIn))
        return kERROR;

    //
    // Search for MTaskList
    //
    MTask *tlist = (MTask*)fParList->FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << err << dbginf << "MTaskList not found... abort." << endl;
        return kERROR;
    }

    //
    // A new file has been opened and new headers have been read.
    //  --> ReInit tasklist
    //
    return tlist->ReInit(fParList) ? kTRUE : kERROR;
}

// --------------------------------------------------------------------------
//
// Return file name of current file.
//
TString MRawFileRead::GetFullFileName() const
{
    const TObject *file = fFileNames->At(fNumFile-1);
    return file ? file->GetName() : "";
}

// --------------------------------------------------------------------------
//
// Restart with the first file
//
Bool_t MRawFileRead::Rewind()
{
    fNumFile=0;
    fNumEvents=0;
    return OpenNextFile()==kTRUE;
}

Bool_t MRawFileRead::CalcNumTotalEvents()
{
    fNumTotalEvents = 0;

    Bool_t rc = kTRUE;

    while (1)
    {
        switch (OpenNextFile(kFALSE))
        {
        case kFALSE:
            break;
        case kERROR:
            rc = kFALSE;
            break;
        case kTRUE:
            fNumTotalEvents += fRawRunHeader->GetNumEvents();
            continue;
        }
        break;
    }

    if (fIn)
        delete fIn;
    fIn = NULL;

    return rc;
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
Int_t MRawFileRead::PreProcess(MParList *pList)
{
    fParList = pList;

    //
    // open the input stream
    // first of all check if opening the file in the constructor was
    // successfull
    //
    if (!MRawRead::PreProcess(pList))
        return kFALSE;

    *fLog << inf << "Calculating number of total events..." << flush;
    if (!CalcNumTotalEvents())
        return kFALSE;
    *fLog << inf << " " << fNumTotalEvents << " found." << endl;

    fNumFile=0;
    fNumEvents=0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The Process reads one event from the binary file:
//  - The event header is read
//  - the run header is read
//  - all crate information is read
//  - the raw data information of one event is read
//
Int_t MRawFileRead::Process()
{
    while (1)
    {
        if (fIn)
        {
            //
            // skip events if requested
            //
            if (fInterleave>1 && GetNumExecutions()%fInterleave>0 && fIn->peek()!=EOF)
            {
                SkipEvent(*fIn);
                return kCONTINUE;
            }

            //
            // Read a single event from file
            //
            const Bool_t rc = ReadEvent(*fIn);
            if (rc!=kFALSE)
                return rc;
        }

        //
        // If an event could not be read from file try to open new file
        //
        const Int_t rc = OpenNextFile();
        if (rc!=kTRUE)
            return rc;
    }
    return kTRUE; 
}

// --------------------------------------------------------------------------
//
//  Close the file. Check whether the number of read events differs from
//  the number the file should containe (MRawRunHeader). Prints a warning
//  if it doesn't match.
//
Int_t MRawFileRead::PostProcess()
{
    //
    // Sanity check for the number of events
    //
    if (fNumEvents==GetNumExecutions()-1 || GetNumExecutions()==0)
        return kTRUE;

    *fLog << warn << dec;
    *fLog << "Warning - number of read events (" << GetNumExecutions()-1;
    *fLog << ") doesn't match number in run header(s) (";
    *fLog << fNumEvents << ")." << endl;

    return kTRUE;
}
