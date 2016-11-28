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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportFileRead
//
// Task to read the central control report file. For more information see
// the base class of all reports MReport.
//
// To add a report which should be read use AddToList.
//
// eg. AddToList("Drive") will assume the existance of a class called
//     MReportDrive. It will create such an object automatically. It will
//     send all lines starting with 'MReportDrive::fIndetifier-REPORT'
//     to this class.
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportFileRead.h"

#include <errno.h>
#include <fstream>

#include <TRegexp.h>
#include <THashTable.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MReportHelp.h"

ClassImp(MReportFileRead);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file and creates a
// THashTable which allows faster access to the MReport* objects.
//
MReportFileRead::MReportFileRead(const char *fname, const char *name, const char *title)
    : fFileName(fname), fVersion(-1), fIn(NULL)
{
    fName  = name  ? name  : "MReportFileRead";
    fTitle = title ? title : "Read task to read general report files";

    fIn = new ifstream;

    fList = new THashTable(1,1);
    fList->SetOwner();
}

// --------------------------------------------------------------------------
//
// Destructor. Delete input stream and hash table.
//
MReportFileRead::~MReportFileRead()
{
    delete fIn;
    delete fList;
}

// --------------------------------------------------------------------------
//
// Wrapper. Returns the MReportHelp with the given identifier from the
// hash table.
//
MReportHelp *MReportFileRead::GetReportHelp(const TString &str) const
{
    return static_cast<MReportHelp*>(fList->FindObject(str));
}

// --------------------------------------------------------------------------
//
// Wrapper. Returns the MReport stored in the given MReportHelp
//
MReport *MReportFileRead::GetReport(MReportHelp *help) const
{
    return help ? help->GetReport() : 0;
}

// --------------------------------------------------------------------------
//
// Wrapper. Returns the MReport stored in the MReportHelp given by its
// identifier.
//
MReport *MReportFileRead::GetReport(const TString &str) const
{
    return GetReport(GetReportHelp(str));
}

// --------------------------------------------------------------------------
//
// Add a new MReport* to the list. The name is the class name (eg MReportDrive)
// For convinience the object is created as a MReportHelp object.
//
Bool_t MReportFileRead::AddToList(const char *name) const
{
    MReportHelp *help = new MReportHelp(name, fLog);

    if (!help->GetReport())
    {
        delete help;
        return kFALSE;
    }

    if (GetReport(help->GetName()))
    {
        *fLog << warn << "WARNING - Report with Identifier '";
        *fLog << help->GetName() << "' already added to the list... ";
        *fLog << "ignored." << endl;
        delete help;
        return kFALSE;
    }

    fList->Add(help);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Call SetupReading for all MReportHelp objects scheduled.
// Try to open the file and check the file header.
//
Int_t MReportFileRead::PreProcess(MParList *pList)
{
    fNumLine = 0;

    // Add the MReport instances first to the paramter list
    // so that SetupReading can find them if needed
    fList->R__FOR_EACH(MReportHelp, AddToList)(*pList);

    // Setup reading
    TIter Next(fList);
    MReportHelp *help=0;
    while ((help=(MReportHelp*)Next()))
        if (!help->SetupReading(*pList))
            return kFALSE;

    //
    // open the input stream
    // first of all check if opening the file in the constructor was
    // successfull
    //
    fIn->open(fFileName);
    if (!(*fIn))
    {
        *fLog << err << "Cannot open file " << fFileName << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    if (TestBit(kHasNoHeader))
        return kTRUE;

    fNumLine = CheckFileHeader();
    return fNumLine>0;
}

// --------------------------------------------------------------------------
//
// Read the file line by line as long as a matching MReport* class is found.
// In this case call its interpreter (Interprete()) and remove the identifier
// first (XYZ-REPORT)
//
Int_t MReportFileRead::Process()
{
    TString str;

    MReportHelp *rep=NULL;
    while (!GetReport(rep))
    {
        str.ReadLine(*fIn);
        if (!*fIn)
        {
            *fLog << dbg << "EOF detected." << endl;
            return kFALSE;
        }

        fNumLine++;

        const Int_t pos = str.First(' ');
        if (pos<=0)
            continue;

        // Check for MReport{str(0,pos)}
        rep = GetReportHelp(str(0,pos));

        // Remove this part from the string
        if (GetReport(rep))
            str.Remove(0, pos);
    }

    const Int_t rc = rep->Interprete(str, fStart, fStop, fVersion);

    switch (rc)
    {
    case kFALSE:
        *fLog << err << "ERROR - Interpreting '" << rep->GetName() << "' failed (l." << fNumLine << ", V" << fVersion << ")... abort." << endl;
        break;
    case kCONTINUE:
        *fLog << warn << "WARNING - Interpreting '" << rep->GetName() << "' failed (l." << fNumLine << ", V" << fVersion << ")... skipped." << endl;
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
Int_t MReportFileRead::PostProcess()
{
    fIn->close();

    if (!GetNumExecutions())
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " statistics:" << endl;
    *fLog << dec << setfill(' ');

    TIter Next(fList);
    MReportHelp *rep=0;

    while ((rep=(MReportHelp*)Next()))
    {
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
    }

    return kTRUE;
}
