/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: MFCT1SelBasic.cc,v 1.13 2006-10-17 14:07:17 tbretz Exp $
! --------------------------------------------------------------------------
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
!   Author(s): Wolfgang Wittek, 04/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 04/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFCT1SelBasic
//
//  This is a class to evaluate basic cuts
//
//  to be called after the calibration (when the number of photons is
//               available for all pixels)
//
//  The basic cuts are :
//
//      remove bad runs
//      thetamin < theta < thetamax
//      software trigger fullfilled (with minimum no.of photons = minphotons)
//
//
/////////////////////////////////////////////////////////////////////////////

#include "MFCT1SelBasic.h"

#include "MParList.h"

#include "MMcEvt.hxx"

#include "MCerPhotEvt.h"
#include "MRawRunHeader.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFCT1SelBasic);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MFCT1SelBasic::MFCT1SelBasic(const char *name, const char *title)
{
    fName  = name  ? name  : "MFCT1SelBasic";
    fTitle = title ? title : "Filter to evaluate basic cuts";

    // default values of cuts
    SetCuts(13.0, 0.0, 60.0);
}

// --------------------------------------------------------------------------
//
// Set the cut values
// 
//
void MFCT1SelBasic::SetCuts(Float_t minphotons, 
                            Float_t thetamin, Float_t thetamax)
{
    fMinPhotons = minphotons;
    fThetaMin   = thetamin;
    fThetaMax   = thetamax;

    *fLog << inf << "MFCT1SelBasic cut values : fMinPhotons, fThetaMin, fThetaMax = ";
    *fLog << fMinPhotons <<",  " << fThetaMin << ",  " << fThetaMax << endl;
}

// --------------------------------------------------------------------------
//
// Set the pointers
// 
//
Int_t MFCT1SelBasic::PreProcess(MParList *pList)
{
    fRawRun = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRawRun)
    {
        *fLog << dbginf << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << dbginf << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
    if (!fEvt)
    {
        *fLog << dbginf << "MCerPhotEvt not found... aborting." << endl;
        return kFALSE;
    }

    fCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << dbginf << "MGeomCam (Camera Geometry) missing in Parameter List... aborting." << endl;
        return kFALSE;
    }

    memset(fCut, 0, sizeof(fCut));

    return kTRUE;
}

Int_t MFCT1SelBasic::Set(Int_t rc)
{
    fCut[rc]++;
    fResult=kTRUE;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluate basic cuts
// 
//     bad events    : fResult = kTRUE;
//     good events   : fResult = kFALSE;
//
Int_t MFCT1SelBasic::Process()
{
    const Double_t theta = kRad2Deg*fMcEvt->GetTelescopeTheta();

    fResult  = kFALSE;

    // remove bad runs for MC gammas
    if (fMcEvt->GetEnergy() == 0.0  &&  fMcEvt->GetImpact() == 0.0)
    {
      if (fRawRun->GetRunNumber() == 601  ||
          fRawRun->GetRunNumber() == 613  ||
          fRawRun->GetRunNumber() == 614    )
	return Set(1);
    }

    if (theta<fThetaMin)
        return Set(2);

    if (theta>fThetaMax)
        return Set(3);

    if (!SwTrigger())
        return Set(4);

    fCut[0]++;

    return kTRUE;
}
// --------------------------------------------------------------------------
//
// Software trigger
// 
// require 2 neighboring pixels (which are not in the outermost ring), 
//                       each having at least 'fMinPhotons' photons
// 
// 
Bool_t MFCT1SelBasic::SwTrigger()
{
    const Int_t entries = fEvt->GetNumPixels();
 
    for (Int_t i=0; i<entries; i++)
    {
        const MCerPhotPix &pix = (*fEvt)[i];

        const Int_t id = pix.GetPixId();
        if (!pix.IsPixelUsed())
            continue;

        const Double_t photons = pix.GetNumPhotons();
        if (photons < fMinPhotons)
            continue;

        // this pixel is used and has the required no.of photons
        // check whether this is also true for a neigboring pixel

        const MGeomPix &gpix = (*fCam)[id];
        if ( gpix.IsInOutermostRing() )
            continue;

        const Int_t nneighbors = gpix.GetNumNeighbors();
        for (Int_t n=0; n<nneighbors; n++)
        {
            const Int_t id1 =  gpix.GetNeighbor(n);
            if ( !fEvt->IsPixelUsed(id1) )
                continue;

            const MGeomPix &gpix1 = (*fCam)[id1];
            if ( gpix1.IsInOutermostRing() )
                continue;

            const MCerPhotPix &pix1 = *fEvt->GetPixById(id1);

            const Double_t photons1 = pix1.GetNumPhotons();
            if (photons1 >= fMinPhotons)
                return kTRUE;
        }
    }
    return kFALSE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the Basic selections.
//
Int_t MFCT1SelBasic::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3) ;
    *fLog << (int)(fCut[1]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: bad run " << endl;

    *fLog << " " << setw(7) << fCut[2] << " (" << setw(3) ;
    *fLog << (int)(fCut[2]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: Zenith angle < " << fThetaMin << endl;

    *fLog << " " << setw(7) << fCut[3] << " (" << setw(3) ;
    *fLog << (int)(fCut[3]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: Zenith angle > " << fThetaMax << endl;

    *fLog << " " << setw(7) << fCut[4] << " (" << setw(3) ;
    *fLog << (int)(fCut[4]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: Software trigger not fullfilled" ;
    *fLog << " (with fMinPhotons = " << fMinPhotons << ")" << endl;

    *fLog << " " << fCut[0] << " (" << (int)(fCut[0]*100/GetNumExecutions()) ;
    *fLog << "%) Evts survived Basic selections!" << endl;
    *fLog << endl;

    return kTRUE;
}
