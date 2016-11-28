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

/////////////////////////////////////////////////////////////////////////////
//
// MPointingPosCalc
//
// Currently:
//
//  * MC files:  Copy the simulated telescope position (Telescope Theta,
//               Telescope Phi in MMcEvt) to MPointingPosition
//
//  * Real Data: Copy the nominal poiting position (Nominal Zd, Nominal Az
//               in MReportDrive) to MPointingPosition
//
// We do a trick: Because it is not guranteed, that MReadReports calls
//  ReInit before the first event from the drive-tree is read (because
//  the first data event may have a time-stamp later than the first
//  drive event) we always assume that the current run is a data run.
//  If the assumption is true fReport should be initialized correctly.
//  For MC runs this should never happen, Because they are read though
//  MReadMarsFile which gurantees, that ReInit is always called before
//  Process. If we encounter such a case we stop execution.
//
// Future: Interpolate the pointing position for each event between two
//         consecutive drive reports.
//
// Input Container:
//   MRawRunHeader
//   [MMcEvt, MReportDrive]
//
// Output Container:
//   MPointingPosition
//
/////////////////////////////////////////////////////////////////////////////
#include "MPointingPosCalc.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MPointingPos.h"
#include "MRawRunHeader.h"
#include "MReportDrive.h"
#include "MMcEvt.hxx"

ClassImp(MPointingPosCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Search for MRawRunHeader. Get the run type from there. Depending on
// the run type search either for MMcEvt or MReportDrive.
//
Bool_t MPointingPosCalc::ReInit(MParList *plist)
{
    MRawRunHeader *run = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!run)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRunType = run->GetRunType();

    switch (fRunType)
    {
    case MRawRunHeader::kRTData:
        if (!fReport)
        {
            *fLog << err << "MReportDrive not found... aborting." << endl;
            return kFALSE;
        }
        return kTRUE;

    case MRawRunHeader::kRTMonteCarlo:
        fMcEvt = (MMcEvt*)plist->FindObject("MMcEvt");
        if (!fMcEvt)
        {
            *fLog << err << "MMcEvt not found... aborting." << endl;
            return kFALSE;
        }
        return kTRUE;

    case MRawRunHeader::kRTPedestal:
        *fLog << err << "Cannot work in a pedestal Run!... aborting." << endl;
        return kFALSE;

    case MRawRunHeader::kRTCalibration:
        *fLog << err << "Cannot work in a calibration Run!... aborting." << endl;
        return kFALSE;

    default:
        *fLog << err << "Run Type " << fRunType << " unknown!... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Search for 'MPointingPos'. Create if not found.
//
Int_t MPointingPosCalc::PreProcess(MParList *plist)
{
    fPosition = (MPointingPos*)plist->FindCreateObj("MPointingPos");
    fReport   = (MReportDrive*)plist->FindObject("MReportDrive");

    // We use kRTNone here as a placeholder for data runs.
    fRunType  = MRawRunHeader::kRTNone;

    return fPosition ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
//  See class description.
//
Int_t MPointingPosCalc::Process()
{
    switch (fRunType)
    {
    case MRawRunHeader::kRTNone:
    case MRawRunHeader::kRTData:
        if (!fReport)
        {
            *fLog << warn;
            *fLog << "MPointingPosCalc::Process: fReport==NULL && fRunType!=kRTMonteCarlo... abort!" << endl;
            return kERROR;
        }
        fPosition->SetLocalPosition(fReport->GetNominalZd(), fReport->GetNominalAz());
        fPosition->SetSkyPosition(fReport->GetRa(), fReport->GetDec());
        return kTRUE;

    case MRawRunHeader::kRTMonteCarlo:
        if (!fMcEvt)
        {
            *fLog << warn;
            *fLog << "MPointingPosCalc::Process: fMcEvt==NULL && fRunType==kRTMonteCarlo... abort!" << endl;
            return kERROR;
        }
        fPosition->SetLocalPosition(fMcEvt->GetTelescopeTheta()*TMath::RadToDeg(), fMcEvt->GetTelescopePhi()*TMath::RadToDeg());
        return kTRUE;
    }
    return kTRUE;
}
