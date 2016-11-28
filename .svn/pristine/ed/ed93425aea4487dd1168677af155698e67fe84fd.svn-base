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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportHelp
//
// This is a wrapper class for MReport derived files. It provides root-like
// access to the identifier of the report, such that this identifiers can
// be used in hast tables to be able to speed up access to the class
// corresponding to a report identifier. It also provides access to the
// class by the name of the identifier.
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportHelp.h"

#include <TROOT.h>  // gROOT->GeClass
#include <TClass.h> // TClass

#include "MLog.h"
#include "MLogManip.h"

#include "MReport.h"
#include "MParList.h"

ClassImp(MReportHelp);

using namespace std;

// --------------------------------------------------------------------------
//
// Construtor. Takes the name of the MReport-class (eg MReportDrive) and
// a log-stream as an argument. The log-stream is used for output in the
// constructor  An instance of the MReport-class is created using its
// default constructor.
//
MReportHelp::MReportHelp(const char *name, MLog *fLog) : fReport(NULL), fNumReports(0), fNumSkipped(0)
{
    //
    // create the parameter container of the the given class type
    //
    *fLog << err;
    TClass *cls = MParList::GetClass(name, fLog);

    if (cls && cls->InheritsFrom(MReport::Class()))
        fReport = static_cast<MReport*>(cls->New());
}

// --------------------------------------------------------------------------
//
// The instance of the MReport-class is deleted
//
MReportHelp::~MReportHelp()
{
    if (fReport)
        delete fReport;
}

// --------------------------------------------------------------------------
//
// Return the Identifier ("DRIVE-REPORT") as name. This allows
// calling FindObject("[identifier]") in lists.
//
const char *MReportHelp::GetName() const
{
    return fReport->GetIdentifier();
}

// --------------------------------------------------------------------------
//
// Return the Identifier ("DRIVE-REPORT") hash value as hash value. This
// allows faster access b usage of a THashTable
//
ULong_t MReportHelp::Hash() const
{
    return fReport->GetIdentifier().Hash();
}

// --------------------------------------------------------------------------
//
// Calls the Interprete function of the report and counts the number of
// successfull interpretations.
//
Int_t MReportHelp::Interprete(TString &str, const MTime &start, const MTime &stop, Int_t ver)
{
    const Int_t rc = fReport->Interprete(str, start, stop, ver);

    switch (rc)
    {
    case kTRUE:     fNumReports++; break;
    case kCONTINUE: fNumSkipped++; break;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Calls the Setip function for reading of the report
//
Bool_t MReportHelp::SetupReading(MParList &plist)
{
    return fReport->SetupReading(plist);
}

// --------------------------------------------------------------------------
//
// Add the report to the given parameter list.
//
void MReportHelp::AddToList(MParList &plist)
{
    plist.AddToList(fReport);
}
