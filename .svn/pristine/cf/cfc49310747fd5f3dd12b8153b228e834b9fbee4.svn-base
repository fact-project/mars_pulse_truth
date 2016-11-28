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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReport
//
//  This is a base class for all reports comming from subsystems stored in
//  a report file.
//
//
// Due to wrong assignment by arehucas the veriosn number for some
// cases is replaced.
//
//   Old Version | MjdMin  | MjdMax  | New Version
//  -------------+---------+---------+-------------
//    see MReport::Interprete()
//
//  Be carefull: The class name of all classes derived from this class
//               should start with 'MReport', see SetupReading
//
//////////////////////////////////////////////////////////////////////////////
#include "MReport.h"

#include "MLogManip.h"

#include "MTime.h"
#include "MParList.h"

ClassImp(MReport);

using namespace std;

// --------------------------------------------------------------------------
//
// Copy fState and fTime
//
void MReport::Copy(TObject &obj) const
{
    MReport &rep = static_cast<MReport&>(obj);

    rep.fState = fState;

    if (rep.fTime && fTime)
        *rep.fTime = *fTime;
}

// --------------------------------------------------------------------------
//
// Check whether the given TString begins with the given tag. Remove
// the tag from the string.
//
Bool_t MReport::CheckTag(TString &str, const char *tag) const
{
    if (!str.BeginsWith(tag))
    {
        *fLog << warn << "WARNING - '" << tag << "' tag not found." << endl;
        return kFALSE;
    }
    str.Remove(0, strlen(tag)); // Remove Tag
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interpretes the beginning of a line which starts like:
//   status hour minute second millisec skip skip skip skip skip
// The identifier is assumed to be removed.
//
// While skip are numbers which won't enter the analysis
//
// SetupReading must be called successfully before.
//
Bool_t MReport::InterpreteHeader(TString &str, Int_t ver)
{
    int len, state;
    int yea, mon, day, hor, min, sec, ms;

    TString fmt(fHasReportTime ?
                " %d %d %d %d %d %d %d %d %*d %*d %*d %*d %*d %*d %*d %*d %n" :
                " %d %d %d %d %d %d %d %d %n");

    // M1/M2 telescope number (FIXME: Readout, check?)
    if (ver>=200805190)
        fmt.Prepend(" %*c%*d");

    int n = sscanf(str.Data(), fmt.Data(),
                   &state, &yea, &mon, &day, &hor, &min, &sec, &ms, &len);
    if (n!=8)
    {
        *fLog << err << "ERROR - Cannot interprete header of " << fIdentifier << " (n=" << n << ")" << endl;
        return kFALSE;
    }

    if (ms==1000)
    {
        *fLog << warn << "WARNING - Milliseconds in timestamp of " << fIdentifier;
        *fLog << Form(" %d.%d.%d %02d:%02d:%02d.%03d", day, mon, yea, hor, min, sec, ms);
        *fLog << " reset to 999." << endl;
        ms = 999;
    }

    fState=state;
    if (!fTime->Set(yea, mon, day, hor, min, sec, ms))
    {
        *fLog << err << "ERROR - Event " << fIdentifier << " has invalid time ";
        *fLog << Form("%d.%d.%d %02d:%02d:%02d.%03d", day, mon, yea, hor, min, sec, ms);
        *fLog << "... abort." << endl;
        return kFALSE;
    }

    str.Remove(0, len);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Report Body must be overwritten. It will get the line idetified to belong
// to fIdentifier without the leading status and time infomration as an
// argument.
//
Int_t MReport::InterpreteBody(TString &str, Int_t ver)
{
    *fLog << warn << "No interpreter existing for: " << fIdentifier << endl;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprets Header and Body of a report file line. Calls SetReadyToSave()
// in case the interpretation was successfull. And fTime->SetReadyToSave()
// when a corresponding time container exists.
//
// SetupReading must be called successfully before.
//
// Due to wrong assignment by arehucas the veriosn number for some
// cases is replaced.
//
//   Old Version | MjdMin  | MjdMax  | New Version
//  -------------+---------+---------+-------------
//    200504130  | 53548.0 | 53567.0 |  200506300
//    200503170  | 53446.5 | 53447.5 |  200502240
//    200508290  | 53643.5 |         |  200509300
//    200510250  | 53801.5 | 53813.5 |  200603080
//    200510250  | 53813.5 |         |  200603190
//    200604010  | 53863.5 |         |  200605080
//    200805190  | 54711.5 |         |  200809030
//    200812040  | 54814.5 |         |  200812140
//    200902210  | 54968.5 |         |  200905170
//
Int_t MReport::Interprete(TString &str, const MTime &start, const MTime &stop, Int_t ver)
{
    // Interprete header (time, status, etc) of report
    if (!InterpreteHeader(str, ver))
        return kFALSE;

    // return -1: This is the special case: out of time limit
    if (start && *fTime<start)
        return -1;
    if (stop  && *fTime>stop)
        return -1;

    // Due to wrong assignment by arehucas the veriosn number for some
    // cases is replaced.
    if (ver==200504130 && GetMjd()>53548 && GetMjd()<53567)
        ver=200506300;

    if (ver==200503170 && GetMjd()>53446.5 && GetMjd()<53447.5)
        ver=200502240;

    if (ver==200508290 && GetMjd()>53643.5)
        ver=200509300;

    if (ver==200510250 && GetMjd()>53801.5 && GetMjd()<53813.5)
        ver=200603080;

    if (ver==200510250 && GetMjd()>53813.5)
        ver=200603190;

    if (ver==200604010 && GetMjd()>53864.5)
        ver=200605080;

    if (ver==200805190 && GetMjd()>54711.5)
        ver=200809030;

    if (ver==200812040 && GetMjd()>54814.5)
        ver=200812140;

    if (ver==200902210 && GetMjd()>54968.5)
        ver=200905170;

    // Interprete body (contents) of report
    const Int_t rc = InterpreteBody(str, ver);
    if (rc != kTRUE)
        return rc;

    SetReadyToSave();
    fTime->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check for the existance of a corresponding MTime in the given parameter
// list. If it is not found a new one will be created. The name of the
// MTime object is created by taking the ClassName() of the derived MReport
// class and stripping the leading MReport
//
Bool_t MReport::SetupReading(MParList &plist)
{
    fTime = NULL;

    TString id(ClassName());
    if (!id.BeginsWith("MReport"))
    {
        *fLog << warn << " WARNING - Class name '" << id << "' ";
        *fLog << " doesn't begin with 'MReport'... no MTime assigned." << endl;
        return kFALSE;
    }

    id.Remove(0, 7);
    if (id.IsNull())
    {
        *fLog << warn << " WARNING - No postfix existing... no MTime assigned." << endl;
        return kFALSE;
    }

    id.Prepend("MTime");

    fTime = (MTime*)plist.FindCreateObj("MTime", id);
    if (!fTime)
        return kFALSE;

    return kTRUE;
}
