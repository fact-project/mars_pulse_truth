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
#include "MPointingPosCalcWA.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MVector3.h"

#include "MPointingPos.h"
#include "MAstro.h"
#include "MObservatory.h"
#include "MAstroSky2Local.h"
#include "MRawRunHeader.h"
#include "MReportDrive.h"
#include "MMcEvt.hxx"

ClassImp(MPointingPosCalcWA);

using namespace std;

// --------------------------------------------------------------------------
//
// Search for MRawRunHeader. Get the run type from there. Depending on
// the run type search either for MMcEvt or MReportDrive.
//
Bool_t MPointingPosCalcWA::ReInit(MParList *plist)
{
    fObs = (MObservatory*)plist->FindCreateObj("MObservatory");
    if (!fObs)
    {
       *fLog << err << "MObservatory not found... aborting." << endl;
        return kFALSE; 
    }

    fRunHeader = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRunType = fRunHeader->GetRunType();

    switch (fRunType)
    {
    case MRawRunHeader::kRTData:
        fReport = (MReportDrive*)plist->FindObject("MReportDrive");
        if (!fReport)
        {
            *fLog << err << "MReportDrive not found... will set Az and Zd to a fixed value." << endl;
            return kTRUE;
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
Int_t MPointingPosCalcWA::PreProcess(MParList *plist)
{
    fPosition = (MPointingPos*)plist->FindCreateObj("MPointingPos");
    return fPosition ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
//  See class description.
//
Int_t MPointingPosCalcWA::Process()
{

    const char plus = '+';
    Char_t Rsgn;
    UShort_t Rhour, Rmin, Rsec;
    Double_t ra;

    switch (fRunType)
    {
    case MRawRunHeader::kRTData:
     if(!fReport)
     {
        MTime start = fRunHeader->GetRunStart();

        MVector3 v;
        v.SetRaDec(fRa,fDec);
        v *= MAstroSky2Local(start,*fObs);
        Double_t ZA = v.Theta()*180./acos(-1.);
        Double_t Az = v.Phi()*180./acos(-1.);
//        cout << " Za = " << ZA << " v.Theta() " << v.Theta() << " Az " << Az << " v.Phi " << v.Phi() << endl;
        fPosition->SetLocalPosition(ZA, Az);
        MAstro::Rad2Hms(fRa, Rsgn, Rhour, Rmin, Rsec);
        if (Rsgn==plus) ra = Rhour + Rmin/60. + Rsec/3600.;
        else ra = - (Rhour + Rmin/60. + Rsec/3600.);
        fPosition->SetSkyPosition(ra, TMath::RadToDeg()*fDec);
        return kTRUE;
     }
     else 
     {
        fPosition->SetLocalPosition(fReport->GetNominalZd(), fReport->GetNominalAz());
        fPosition->SetSkyPosition(fReport->GetRa(), fReport->GetDec());
        return kTRUE;
     }

    case MRawRunHeader::kRTMonteCarlo:
        fPosition->SetLocalPosition(fMcEvt->GetTelescopeTheta()*TMath::RadToDeg(), fMcEvt->GetTelescopePhi()*TMath::RadToDeg());
        return kTRUE;
    }
    return kTRUE;
}
