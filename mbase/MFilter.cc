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
!   Author(s): Thomas Bretz, 07/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFilter
//
//   This is a base class which defines an interface to create your own
//   filters. To do it derive a class from MFilter.
//
//   You can invert the meaning of a filter (logical NOT '!') by calling
//   SetInverted().
//
//   You can create two types of Filters:
//    - static Filters and
//    - dynamic Filters
//
//   Static Filters:
//    A static filter is a filter which value doesn't change dynamically,
//    which mean only once while running through your tasklist. To create
//    a static filter override the Process-function of MFilter (this is
//    the function in which the filer-value should be updated). If
//    necessary you can also overwrite Pre-/PostProcess. The process
//    function should calculate the return value of IsExpressionTrue.
//    IsExpressionTrue should simply return this value. This kind of
//    filter must be added to the tasklist at a point which is forseen to
//    update the value of the filter (eg. after the reading task).
//
//   Dynamic Filters:
//    A dynamic filter is a filter which returns a value which must be
//    calculated at the time the filter is called. To create such a
//    filter you have to overwrite IsExpressionTrue only. If there is
//    no need for a 'one-point' update this filter must not be added to
//    the tasklist.
//
//   Usage:
//    A Filter is connected to a task by calling MTask::SetFilter. The
//    task is now executed when IsExpressionTrue returns a value
//    (different from kFALSE) only.
//
//   Remarks:
//    - Make sure, that all tasks which depends on this filter are either
//      collected in a MTaskList-object or are conected to the same
//      filter.
//    - If you want to use different filters (combined logically) for one
//      task please look for the MFilterList class.
//    - Be careful, the return value of IsExpressionTrue is NOT a real
//      boolean. You can return other values, too.
//
/////////////////////////////////////////////////////////////////////////////
#include "MFilter.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

ClassImp(MFilter);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor for a filter. Initializes fInverted with kFALSE
//
MFilter::MFilter(const char *name, const char *title) : fInverted(kFALSE)
{
    fName  = name  ? name  : "MFilter";
    fTitle = title ? title : "Base Class for a filter";
}

// --------------------------------------------------------------------------
//
//  return the Rule corresponding to this filter (see MF and MDataChain)
//
TString MFilter::GetRule() const
{
    return MString::Format("(%s)", ClassName()); //"<GetRule n/a for " + fName + ">";
}

// --------------------------------------------------------------------------
//
//  This is used to print the output in the PostProcess/Finalize.
//  Or everywhere else in a nice fashioned and unified way.
//
void MFilter::PrintSkipped(UInt_t n, const char *str)
{
    *fLog << " " << setw(7) << n << " (";
    *fLog << setw(3) << TMath::Nint(100.*n/GetNumExecutions());
    *fLog << "%) Evts fullfilled: " << str << endl;
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup filters.
//
// Assuming your filter is called (Set/GetName): MyFilter
//
// First you can setup whether the filter is inverted or not:
//     MyFilter.Inverted: yes, no  <default=no>
//
Int_t MFilter::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (IsEnvDefined(env, prefix, "Inverted", print))
    {
        SetInverted(GetEnvValue(env, prefix, "Inverted", IsInverted()));
        return kTRUE;
    }
    return kFALSE;
}
