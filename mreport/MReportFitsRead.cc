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
!   Author(s): Thomas Bretz, 12/2011 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2011
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportFitsRead
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportFitsRead.h"

#include <errno.h>
#include <fstream>

#include <TClass.h>
#include <TRegexp.h>
#include <THashTable.h>

#include "fits.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MReport.h"

ClassImp(MReportFitsRead);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file and creates a
// THashTable which allows faster access to the MReport* objects.
//
MReportFitsRead::MReportFitsRead(const char *fname, const char *name, const char *title)
    : fFileName(fname), fReport(NULL), fIn(NULL)
{
    fName  = name  ? name  : "MReportFitsRead";
    fTitle = title ? title : "Read task to read general report files";
}

// --------------------------------------------------------------------------
//
// Destructor. Delete input stream and hash table.
//
MReportFitsRead::~MReportFitsRead()
{
    delete fIn;
}

// --------------------------------------------------------------------------
//
// Call SetupReading for all MReportHelp objects scheduled.
// Try to open the file and check the file header.
//
Int_t MReportFitsRead::PreProcess(MParList *pList)
{
    // Add the MReport instances first to the paramter list
    // so that SetupReading can find them if needed
    //fList->R__FOR_EACH(MReportHelp, AddToList)(*pList);

    fReport = (MReport*)pList->FindCreateObj(fReportName);
    if (!fReport)
        return kFALSE;

    if (!fReport->InheritsFrom(MReport::Class()))
    {
        *fLog << err << fReportName << " does not derive from MReport." << endl;
        return kFALSE;
    }


    //
    // open the input stream
    // first of all check if opening the file in the constructor was
    // successfull
    //
    if (fIn)
        delete fIn;
    fIn = new fits(fFileName.Data());
    if (!(*fIn))
    {
        *fLog << err << "Cannot open file " << fFileName << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    if (fIn->GetStr("TELESCOP")!="FACT")
    {
        *fLog << err << "Not a valid FACT FITS file (key TELESCOP not 'FACT')." << endl;
        return kFALSE;
    }

    fMjdRef = fIn->HasKey("MJDREF") ? fIn->GetUInt("MJDREF") : 0;

    if (!fIn->HasKey("MJDREF"))
        *fLog << warn << "MJDREF header key missing... assuming MJDREF==0" << endl;

    return
        fIn->SetRefAddress("QoS",  fBufQos)  &&
        fIn->SetRefAddress("Time", fBufTime) &&
        fReport->SetupReading(*pList) &&
        fReport->SetupReadingFits(*fIn);
}

// --------------------------------------------------------------------------
//
// Read the file line by line as long as a matching MReport* class is found.
// In this case call its interpreter (Interprete()) and remove the identifier
// first (XYZ-REPORT)
//
Int_t MReportFitsRead::Process()
{
    if (!fIn->GetNextRow())
    {
        *fLog << inf << "End-of-file detected." << endl;
        return kFALSE;
    }

    if (fBufTime==0)
        return kCONTINUE;

    MTime &time = *fReport->GetTime();

    time.SetMjd(fBufTime+fMjdRef);
    time.SetReadyToSave();

    // return -1: This is the special case: out of time limit
    if (fStart && time<fStart)
        return kCONTINUE;
    if (fStop  && time>fStop)
        return kCONTINUE;

    const Int_t rc = fReport->InterpreteFits(*fIn);


    /*
    switch (rc)
    {
    case kTRUE:     fNumReports++; break;
    case kCONTINUE: fNumSkipped++; break;
    }*/

    switch (rc)
    {
    case kFALSE:
        *fLog << err << "ERROR - Interpreting '" << fReport->GetName() << "' failed (l." << fIn->GetRow() << ")... abort." << endl;
        break;
    case kCONTINUE:
        *fLog << warn << "WARNING - Interpreting '" << fReport->GetName() << "' failed (l." << fIn->GetRow() << ")... skipped." << endl;
        break;
    case -1: // This is the special case: out of time limit
        return kCONTINUE;
    }

    return rc;
}

// --------------------------------------------------------------------------
//
//  Close the file and print some execution statistics
//
Int_t MReportFitsRead::PostProcess()
{
    fIn->close();

    if (!GetNumExecutions())
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " statistics:" << endl;
    *fLog << dec << setfill(' ');
  /*
    *fLog << inf;
    *fLog << " " << setw(7) << rep->GetNumReports() << " (";
    *fLog << setw(3) << (int)(100.*rep->GetNumReports()/GetNumExecutions());
    *fLog << "%): " << rep->GetName() << endl;

    if (rep->GetNumSkipped()==0)
        continue;

    *fLog << warn;
    *fLog << " " << setw(7) << rep->GetNumSkipped() << " (";
    *fLog << setw(3) << (int)(100.*rep->GetNumSkipped()/GetNumExecutions());
    *fLog << "%): " << rep->GetName() << " skipped!" << endl;
    */
    return kTRUE;
}
