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
!   Author(s): Thomas Bretz 6/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosCorrect
//
// Performs a misfocussing correction for source and anti-source position
// in the camera, i.e. it is assumed that the telscope is pointing well
// (all starguider and other corrections are already applied to the
// source positon) but the mirror is not focussing to the center of
// the camera.
//
// Due to missfocussing a shift of
//    dx=0.048deg and dy=0.034deg
//  or
//    dx=14.24mm and dy=9.495mm
// is added between run 53832 (excl) and 56161 (excl)
//
//
// See also: Runbook
//
//    2005-05-23 03:33:51:
//    We start again to make tests on AMC with Roque lamp and PinDiode Tests.
//    At park position, we do a Laser initial isation and a Laser adjustment.
//    We discovered that the procedure we used yester day for Roque Lamp
//    light source was producing a very non-uniform light
//    We did some studies to produce an uniformly illuminated light from
//    the Roque Lamp Then we moved the telescope to the Roque Lamp position
//    and did a Laser adjust for the Roque position. We put the telescope to
//    the same shaftencoder values as were used in August to adjust the
//    mirrors
//    ( 29691 for SE-Az and SE-Zd1 1301 and SE-Zd2 9912 ( sometimes
//    oscillating to 9913 ) ) When we looked at the image of the spot on the
//    camer a, we saw a clear offset from the centre. Then we calculated this
//    offset and put the correct numbers in the AMC code. Then we redid the
//    laser adjustment and found the spot to be nicely centered.  Our
//    calculations showed that the offset was 0.048 deg in X direction and
//    0.032 deg in Y direction.
//    Good night
//
// Believing the Database the following data is affected:
//   1ES1426+428           Off1ES1426-1
//   1ES1959+650           Off1ES1959-1
//   2E-1415+2557          Off2E1415-1
//   Arp-220               OffArp-220-1
//   GC-W1                 OffHB1553-1
//   HB89-1553+11          OffM87-1
//   M87                   OffW-Comae-1
//   W-Comae
//
//
// For more details see Process()
//
// Input:
//   MSrcPosCam
//   MSrcPosAnti [MSrcPosCam]
//
// Output:
//   OpticalAxis [MSrcPosCam]
//   MSrcPosCam
//   MSrcPosAnti [MSrcPosCam]
//
//////////////////////////////////////////////////////////////////////////////
#include "MSrcPosCorrect.h"

#include <TVector2.h>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MRawRunHeader.h"
#include "MReportStarguider.h"

ClassImp(MSrcPosCorrect);

using namespace std;

// --------------------------------------------------------------------------
//
MSrcPosCorrect::MSrcPosCorrect(const char *name, const char *title)
    : fSrcPosCam(NULL), fSrcPosAnti(NULL), fAxis(NULL), fGeom(NULL)
     , fDx(-14.24) , fDy(-9.495)

{
    fName  = name  ? name  : "MSrcPosCorrect";
    fTitle = title ? title : "Calculates the source position in the camera";
}

// --------------------------------------------------------------------------
//
// Search and if necessary create MSrcPosCam in the parameter list. Search
// MSourcePos. If not found, do nothing else, and skip the task. If MSrcPosCam
// did not exist before and has been created here, it will contain as source
// position the camera center (0,0).
// In the case that MSourcePos is found, go ahead in searching the rest of
// necessary containers. The source position will be calculated for each
// event in Process.
//
Int_t MSrcPosCorrect::PreProcess(MParList *pList)
{
    fSrcPosCam = (MSrcPosCam*)pList->FindObject("MSrcPosCam");
    if (!fSrcPosCam)
    {
        *fLog << err << "MSrcPosCam not found... aborting." << endl;
        return kFALSE;
    }

    fSrcPosAnti = (MSrcPosCam*)pList->FindObject("MSrcPosAnti", "MSrcPosCam");

    fAxis = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam", "OpticalAxis");
    if (!fAxis)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Checking for file type. If the file type is Monte Carlo the
// source position is arbitrarily determined from the MC headers.
//
Bool_t MSrcPosCorrect::ReInit(MParList *plist)
{
    MRawRunHeader *run = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!run)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRunType   = run->GetRunType();
    fRunNumber = run->GetRunNumber();

    if (fRunNumber<56161 && fRunNumber>53832)
    {
        *fLog << inf << "Run Number " << fRunNumber << " between 53832 and 56161." << endl;
        *fLog << "A misfocussing correction (" << fDx << "mm/" << fDy << "mm) will be applied." << endl;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The offset fDx/fDy is subtracted from the source position. The anti-
// source position is shifted away from the camera-center and rotated
// around the camera center according to this correction, because it
// should always be symmetric w.r.t. the camera-center, but it is not
// necessary on the other side of the camera (e.g. three off-regions).
//
Int_t MSrcPosCorrect::Process()
{
    if (fRunType==MRawRunHeader::kRTMonteCarlo)
        return kTRUE;

    if (fRunNumber<=53832 || fRunNumber>=56161)
        return kTRUE;

    // dx=-0.048deg, dy=0.034deg, d=0.059deg
    const TVector2 dxy(-fDx, -fDy);

    const TVector2 s1(fSrcPosCam->GetXY());
    const TVector2 a1(fSrcPosAnti->GetXY());
    const TVector2 s2 = s1 + dxy;


    // Anti-Source position should always be symetric w.r.t. camera center
    TVector2 a2;
    a2.SetMagPhi(a1.Mod()+s2.Mod()-s1.Mod(), a1.Phi()+s2.DeltaPhi(s1));

    fAxis->SetXY(dxy);
    fSrcPosCam->SetXY(s2);
    fSrcPosAnti->SetXY(s2);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// MSrcPosCorrect.Dx: -14.24
// MSrcPosCorrect.Dy: -9.495
//
// For a detailed description see the class reference.
//
Int_t MSrcPosCorrect::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Dx", print))
    {
        fDx = GetEnvValue(env, prefix, "Dx", fDx);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Dy", print))
    {
        fDy = GetEnvValue(env, prefix, "Dy", fDy);
        rc = kTRUE;
    }

    return rc;
}
