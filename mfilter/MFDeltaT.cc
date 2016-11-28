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
!   Author(s): Thomas Bretz  09/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFDeltaT
//
//  for more details see Construtor and Process()
//
/////////////////////////////////////////////////////////////////////////////
#include "MFDeltaT.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcEvt.hxx"

ClassImp(MFDeltaT);

using namespace std;

// --------------------------------------------------------------------------
//
// Constructor. First argument is the upper limit of the filter (default=0.5).
// Second argument is the name of the parameter container storing the time.
// (default="MTime")
//
MFDeltaT::MFDeltaT(Float_t max, const char *time,
                   const char *name, const char *title)
{
    Init(name, title, max, time);
}

// --------------------------------------------------------------------------
//
// Default Constructor. Initializes upper limit of the filter (default=0.5)
// and name of the parameter container storing the time (default="MTime")
//
MFDeltaT::MFDeltaT(const char *name, const char *title)
{
    Init(name, title);
}

// --------------------------------------------------------------------------
//
void MFDeltaT::Init(const char *name, const char *title,
                    Float_t max, const char *time)
{
    fName  = name  ? name  : "MFDeltaT";
    fTitle = title ? title : "Filter for time differences of consecutive events";

    fUpperLimit = max;
    fNameTime   = time;

    fErrors.Set(6);

    AddToBranchList(Form("%s.*", (const char*)fNameTime));
}

// --------------------------------------------------------------------------
//
// Return result of conditional
//
Bool_t MFDeltaT::IsExpressionTrue() const
{
    return fResult;
}

// --------------------------------------------------------------------------
//
// Search for fNameTime [MTime]
//
Int_t MFDeltaT::PreProcess(MParList *pList)
{
    fErrors.Reset();

    fTime = (MTime*)pList->FindObject(fNameTime, "MTime");
    if (fTime)
        return kTRUE;

    *fLog << err << fNameTime << " [MTime] not found... aborting." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Calculate the condition. The condition is true if the time difference
// between two consecutive events are >0 or <= upper limit.
//
Int_t MFDeltaT::Process()
{
    fResult = kFALSE;

    Int_t i=0;

    if (GetNumExecutions()>1)
    {
        if (*fTime-fLastTime>fUpperLimit)
            i=1;
        if (*fTime<fLastTime)
            i=2;
        if (*fTime==fLastTime)
            i=3;
        if (fTime->IsMidnight())
            i=4;
        if (*fTime==MTime())
            i=5;
    }

    fLastTime = *fTime;

    fResult = i==0;
    fErrors[i]++;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print some statistics.
//
Int_t MFDeltaT::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " filter statistics:" << endl;
    *fLog << dec << setfill(' ');
    PrintSkipped(fErrors[2], "Delta-T  < 0");
    PrintSkipped(fErrors[3], "Delta-T == 0");
    PrintSkipped(fErrors[1], Form("Delta-T  > %.2fs", fUpperLimit));
    PrintSkipped(fErrors[4], "MTime is midnight");
    PrintSkipped(fErrors[5], "MTime not initialized");
    *fLog << " " << (int)fErrors[0] << " (";
    *fLog << Form("%5.1f", 100.*fErrors[0]/GetNumExecutions());
    *fLog << "%) Evts fullfilled filter condition!" << endl;
    *fLog << endl;

    return kTRUE;
}
