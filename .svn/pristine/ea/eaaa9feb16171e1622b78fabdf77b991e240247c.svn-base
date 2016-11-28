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
!   Author(s): Thomas Bretz, 10/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MRawSocketRead
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
//   MRawCrateArray
//   MTime
//
//////////////////////////////////////////////////////////////////////////////
#include "MRawSocketRead.h"

#include <stdlib.h>  // atoi

#include <TArrayC.h> // TAraayC

#include "MReadSocket.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MTime.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateData.h"
#include "MRawCrateArray.h"

#include "MStatusDisplay.h"

ClassImp(MRawSocketRead);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file.
//
MRawSocketRead::MRawSocketRead(const char *name, const char *title)
    : fIn(NULL), fPort(-1)
{
    fName  = name  ? name  : "MRawSocketRead";
    fTitle = title ? title : "Task to read DAQ binary data from tcp/ip socket";

    fIn = new MReadSocket;
}

// --------------------------------------------------------------------------
//
// Destructor. Delete input stream.
//
MRawSocketRead::~MRawSocketRead()
{
    delete fIn;
}

// --------------------------------------------------------------------------
//
// Open the socket. This blocks until the connection has been established,
// an error occured opening the connection or the user requested to
// quit the application.
//
Bool_t MRawSocketRead::OpenSocket()
{
    if (fDisplay)
        fDisplay->SetStatusLine2(Form("Waiting for connection on port #%d...", fPort));

    //
    // Open socket connection
    //
    while (1)
    {
        //
        // If port could be opened eveything is ok
        //
        if (fIn->Open(fPort))
            return kTRUE;

        //
        // If a MStatusDisplay is attached the user might have
        // requested to quit the application
        //
        if (fDisplay)
            switch (fDisplay->CheckStatus())
            {
            case MStatusDisplay::kFileClose:
            case MStatusDisplay::kFileExit:
                *fLog << inf << "MRawSocketRead::PreProcess - MStatusDisplay triggered exit." << endl;
                return kFALSE;
            default:
                break;
            }

        //
        // If an error occured during opening the socket stop
        //
        if (fIn->fail())
            break;
    }

    *fLog << err << "ERROR - Cannot open port #" << fPort << endl;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// The PreProcess of this task checks for the following containers in the
// list:
//   MRawRunHeader <output>   if not found it is created
//   MRawEvtHeader <output>   if not found it is created
//   MRawEvtData <output>     if not found it is created
//   MRawCrateArray <output>  if not found it is created
//   MTime <output>           if not found it is created
//
// If all containers are found or created the run header is read from the
// binary file and printed.  If the Magic-Number (file identification)
// doesn't match we stop the eventloop.
//
// Now the EvtHeader and EvtData containers are initialized.
//
Int_t MRawSocketRead::PreProcess(MParList *pList)
{
    if (!OpenSocket())
        return kFALSE;

    if (!MRawRead::PreProcess(pList))
        return kFALSE;

    fParList = pList;
    fRunNumber = (UInt_t)-1;
    fEvtNumber = (UInt_t)-1;

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
Int_t MRawSocketRead::Process()
{
    //
    // Tag which could possibly be used for synchronizing the
    // data stream. At the moment we check only its correctness.
    // Synchronisation seems to work well - Why?
    //
    char dummy[4];
    fIn->read(dummy, 4);   // \nEVT

    if (!(dummy[0]=='\n' && dummy[1]=='E' && dummy[2]=='V' &&dummy[3]=='T'))
    {
        *fLog << warn << "EVT tag not found. Stream out of sync. Please try to restart..." << endl;
        // FIXME: Synchronization missing...
        return kFALSE;
    }

    //
    // No we get some size information (each 5 bytes ascii)
    //
    char sizecc[6] = {0,0,0,0,0,0}; // 5 bytes plus trailing 0-byte
    char sizerh[6] = {0,0,0,0,0,0}; // 5 bytes plus trailing 0-byte
    char sizeev[6] = {0,0,0,0,0,0}; // 5 bytes plus trailing 0-byte
    fIn->read(sizecc, 5); // Size CC info string
    fIn->read(sizerh, 5); // Size run header
    fIn->read(sizeev, 5); // Size Event (+ event header)

    //
    // Currently we skip the CC info string. We may decode this string
    // in the future to get additional information
    //
    TArrayC dummy2(atoi(sizecc));
    fIn->read(dummy2.GetArray(), dummy2.GetSize());

    //
    // Read RUN HEADER (see specification) from input stream
    //
    fLog->SetNullOutput();
    const Bool_t rc = fRawRunHeader->ReadEvt(*fIn);
    fLog->SetNullOutput(kFALSE);

    if (!rc)
    {
        *fLog << err << "Reading MRawRunHeader failed." << endl;
        return kFALSE;
    }

    if (fRunNumber!=fRawRunHeader->GetRunNumber())
    {
        fRawRunHeader->Print();

        MTaskList *tlist = (MTaskList*)fParList->FindObject("MTaskList");
        if (!tlist)
        {
            *fLog << err << dbginf << "ERROR - Task List not found in Parameter List." << endl;
            return kFALSE;
        }

        if (!tlist->ReInit())
            return kFALSE;

        fRunNumber = fRawRunHeader->GetRunNumber();
    }

    if (atoi(sizerh)==fRawRunHeader->GetNumTotalBytes())
    {
        *fLog << err << "Retrieved size of run header mismatch... stopped." << endl;
        return kFALSE;
    }

    if (atoi(sizeev)==0)
    {
        *fLog << dbg << "Event contains only run header information... skipped." << endl;
        return kCONTINUE;
    }

    *fRawEvtTime = fRawRunHeader->GetRunStart();

    //
    // Give the run header information to the 'sub-classes'
    // Run header must be valid!
    //
    fRawEvtHeader->InitRead(fRawRunHeader, fRawEvtTime);
    fRawEvtData1 ->InitRead(fRawRunHeader);
    fRawEvtData2 ->InitRead(fRawRunHeader);

    if (!ReadEvent(*fIn))
        return kFALSE;

    //
    // If no new event was recorded the DAQ resends an old event
    //
    if (fEvtNumber==fRawEvtHeader->GetDAQEvtNumber())
    {
        *fLog << dbg << "Event number #" << dec << fEvtNumber << " didn't change... skipped." << endl;
        return kCONTINUE;
    }

    fEvtNumber=fRawEvtHeader->GetDAQEvtNumber();

    return kTRUE;
}

Int_t MRawSocketRead::PostProcess()
{
    //
    // Close Socket connection
    //
    fIn->Close();

    return kTRUE;
}
