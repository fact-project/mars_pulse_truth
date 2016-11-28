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
!   Author(s): Thomas Bretz 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosCalc
//
// Calculate the current source position in the camera from the (possibly
// already corrected, by starguider) J2000 sky coordinates of the camera
// center (contained in MPointingPos), and the source J2000 sky
// coordinates contained in MSourcePos (of type MPointingPos as well). If
// no MSourcePos is found in the parameter list,  source position is
// assumed to be the same for all events, that specified in  MSrcPosCam
// (if it already existed in the parameter list), or (0,0), the center  of
// the camera, if no MSrcPosCam was present in the parameter list. In both
// cases, no calculation is necessary and then the PreProcess returns
// kSKIP so that the task is removed from the task list.
// 
// The conversion factor between the camera plain (mm) and the sky (deg)
// is taken from MGeomCam. The time is taken from MTime, and the
// coordinates of the observatory from MObservatory.
//
// If a reflector version >= 700 is detected the source position for
// Gammas (and _only_ gammas) is dynamically calculated from the
// telescope (e.g. MMcEvt::fTelescopePhi) and the shower (e.g. MMcEvt::fPhi)
// orientation. In off-mode it is fixed at the camera center.
//
// FIXME: Add here some information about the more-cycle calculation
//
// Input Container:
//   MPointingPos
//   MObservatory
//   MGeomCam
//   MTime
//   [MMcRunHeader]
//   [MMcCorsikaRunHeader]
//   [MSourcePos] (of type MPointingPos)
//
// Output Container:
//   MSrcPosCam
//
// To be done:
//   - a switch between using sky-coordinates and time or local-coordinates
//     from MPointingPos for determine the rotation angle. (Therefor a conversion
//     needs to be implemented inlcuding asrometric corrections)
//
//   - the center of rotation need not to be the camera center
/////  NOTE, A. Moralejo: I don't see the need for special code either.
//
//////////////////////////////////////////////////////////////////////////////
#include "MSrcPosCalc.h"

#include <TRandom.h>
#include <TVector2.h>

#include "MParList.h"
#include "MTaskList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MObservatory.h"
#include "MPointingPos.h"
#include "MPointingDev.h"
#include "MSrcPosCam.h"
#include "MRawRunHeader.h"

#include "MMcEvt.hxx"
#include "MMcRunHeader.hxx"
#include "MMcCorsikaRunHeader.h"

#include "MAstro.h"
#include "MVector3.h"
#include "MAstroSky2Local.h"

ClassImp(MSrcPosCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize fY and fY with 0
//
MSrcPosCalc::MSrcPosCalc(const char *name, const char *title)
    : fObservatory(NULL), fPointPos(NULL), fDeviation(NULL), fMcEvt(NULL),
    fMcHeader(NULL), fMcCorsika(NULL), fGeom(NULL), fTime(NULL), fCallback(NULL),
    fSourcePos(NULL), fSrcPosCam(NULL), fSrcPosAnti(NULL), fMode(kDefault),
    fNumRandomOffPositions(0)
{
    fName  = name  ? name  : "MSrcPosCalc";
    fTitle = title ? title : "Calculates the source position in the camera";

    AddToBranchList("MTime.*");
    AddToBranchList("MMcEvt.*");
    AddToBranchList("MPointingPos.*");
}

// --------------------------------------------------------------------------
//
//  delete fSourcePos if kIsOwner
//  set fSourcePos to NULL
//
void MSrcPosCalc::FreeSourcePos()
{
    if (fSourcePos && TestBit(kIsOwner))
        delete fSourcePos;

    fSourcePos = NULL;
}

// --------------------------------------------------------------------------
//
//  ra  [h]
//  dec [deg]
//
void MSrcPosCalc::SetSourcePos(Double_t ra, Double_t dec)
{
    FreeSourcePos();

    fSourcePos = new MPointingPos("MSourcePos");
    fSourcePos->SetSkyPosition(ra, dec);

    SetOwner();
}

// --------------------------------------------------------------------------
//
// Return ra/dec as string
//
TString MSrcPosCalc::GetRaDec(const MPointingPos &pos) const
{
    const TString rstr = MAstro::Angle2Coordinate(pos.GetRa());
    const TString dstr = MAstro::Angle2Coordinate(pos.GetDec());

    return Form("Ra=%sh Dec=%sdeg", rstr.Data(), dstr.Data());
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
Int_t MSrcPosCalc::PreProcess(MParList *pList)
{
    // Reset fixed position
    fFixedPos = TVector2();

    fSrcPosCam = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam");
    if (!fSrcPosCam)
        return kFALSE;

    fSrcPosAnti = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam", "MSrcPosAnti");
    if (!fSrcPosAnti)
        return kFALSE;

    fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fPointPos = (MPointingPos*)pList->FindObject("MPointingPos");
    if (!fPointPos)
    {
        *fLog << err << "MPointingPos not found... aborting." << endl;
        return kFALSE;
    }

    if (!fSourcePos)
    {
        fSourcePos = (MPointingPos*)pList->FindObject("MSourcePos", "MPointingPos");
        if (!fSourcePos)
        {
            *fLog << inf;
            *fLog << "MSourcePos [MPointPos] not found... The source position" << endl;
            *fLog << "set in MSrcPosCam will be set to  (0/0)  or in the case" << endl;
            *fLog << "of  Monte Carlo set to the appropriate wobble position." << endl;
            return kTRUE;
        }
    }
    // FIXME: Maybe we have to call FreeSourcePos in PostProcess()?

    fDeviation = (MPointingDev*)pList->FindObject("MPointingDev");

    *fLog << inf;
    //*fLog << "Pointing Position: " << GetRaDec(*fPointPos)  << endl;
    *fLog << "Source Position: " << GetRaDec(*fSourcePos) << endl;
    if (fCallback)
        *fLog << "Calculating " << fCallback->GetNumPasses() << " off-regions." << endl;
    if (fNumRandomOffPositions)
        *fLog << "Calculating " << fNumRandomOffPositions << " random off-regions." << endl;

    // For the case ReInit is never called we try:
    fObservatory = (MObservatory*)pList->FindObject("MObservatory");
    fTime        = (MTime*)       pList->FindObject("MTime");
    fRunType     = MRawRunHeader::kRTNone;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Divide the 360deg into num+1 positions. Rotate the given vector
// by pass+1 steps and return the result.
//
TVector2 MSrcPosCalc::Rotate(TVector2 v, Int_t pass, Int_t num) const
{
    const Double_t step = TMath::TwoPi()/(num+1);
    return v.Rotate(step*(pass+1));
}

// --------------------------------------------------------------------------
//
// If fIsWobbleMode==kFALSE set source position to v and anto-source
// position to -v, if fIsWobbleMode==kTRUE vice versa.
//
void MSrcPosCalc::SetSrcPos(TVector2 v) const
{
    if (fMode==kWobble)
    {
        // The trick here is that the anti-source position in case
        // of the off-source regions is always the on-source positon
        // thus a proper anti-source cut is possible.
        fSrcPosAnti->SetXY(v);
        if (fCallback)
            v = Rotate(v, fCallback->GetNumPass(), fCallback->GetNumPasses());
        else
            v *= -1;
        fSrcPosCam->SetXY(v);
    }
    else
    {
        // Because we don't process this three times like in the
        // wobble case we have to find another way to determine which
        // off-source region is the right anti-source position
        // We do it randomly.
        fSrcPosCam->SetXY(v);
        if (fNumRandomOffPositions>1)
            v = Rotate(v, gRandom->Integer(fNumRandomOffPositions), fNumRandomOffPositions);
        else
            v *= -1;
        fSrcPosAnti->SetXY(v);
    }
}

void MSrcPosCalc::InitFixedPos() const
{
    // It is set here for the cases in which Process is not called at all
    fSrcPosCam->SetXY(fFixedPos/fGeom->GetConvMm2Deg());

    // Informal message output
    *fLog << inf;
    *fLog << "Source position set to x=" << fFixedPos.X() << "deg ";
    *fLog << "y=" << fFixedPos.Y() << "deg" << endl;
}

// --------------------------------------------------------------------------
//
// Checking for file type. If the file type is Monte Carlo the
// source position is arbitrarily determined from the MC headers.
//
Bool_t MSrcPosCalc::ReInit(MParList *plist)
{
    // In OffMode set fixed position to camera center
    if (fMode==kOffData)
        fFixedPos = TVector2();

    if (fMode==kOffData || fMode==kFixed)
    {
        // It is set here for the cases in which Process is not called at all
        InitFixedPos();
        return kTRUE;
    }

    MRawRunHeader *run = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!run)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRunType = run->GetRunType();

    if (fRunType!=MRawRunHeader::kRTMonteCarlo)
    {
        fObservatory = (MObservatory*)plist->FindObject("MObservatory");
        if (!fObservatory)
        {
            *fLog << err << "MObservatory not found... aborting." << endl;
            return kFALSE;
        }
        fTime = (MTime*)plist->FindObject("MTime");
        if (!fTime)
        {
            *fLog << err << "MTime not found... aborting." << endl;
            return kFALSE;
        }

        return kTRUE;
    }

    fMcEvt = (MMcEvt*)plist->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    fMcHeader = (const MMcRunHeader*)plist->FindObject("MMcRunHeader");
    /*
    if (!fMcHeader)
    {
        *fLog << err << "MMcRunHeader not found... aborting." << endl;
        return kFALSE;
    }*/

    fMcCorsika = 0;
    if (!fMcHeader || fMcHeader->IsCeres())
        return kTRUE;

    fMcCorsika = (MMcCorsikaRunHeader*)plist->FindObject("MMcCorsikaRunHeader");
    if (!fMcCorsika)
    {
        *fLog << err << "MMcCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    // We don't know here if these are gammas
    //if (fMcHeader->GetReflVersion()>600)
    //    *fLog << inf << "Source position will be calculated from shower and telescope orientation." << endl;

    // Determine Monte Carlo position from Monte Carlo header
    TVector2 v(0, 0);
    if (fMcCorsika->GetWobbleMode()>0.5)
        v.Set(120.*fGeom->GetConvMm2Deg(), 0.);
    if (fMcCorsika->GetWobbleMode()<-0.5)
        v.Set(-120.*fGeom->GetConvMm2Deg(), 0.);

    // Set fixed position
    fFixedPos = v;

    InitFixedPos();

    return kTRUE;
}

void MSrcPosCalc::CalcResult(const MVector3 &pos0, const MVector3 &pos)
{
    // Calculate source position in camera, and convert to mm:
    TVector2 v = MAstro::GetDistOnPlain(pos0, pos, -fGeom->GetCameraDist()*1000);

    if (fDeviation)
        v -= fDeviation->GetDevXY()/fGeom->GetConvMm2Deg();

    SetSrcPos(v);
}

// --------------------------------------------------------------------------
//
// Derotate fX/fY by the current rotation angle, set MSrcPosCam
//
Int_t MSrcPosCalc::Process()
{
    // In off-data mode the position is set to a fixed value
    // independent of the run type. In fixed-data mode the position
    // is set to the predefined position converted to mm.
    if (fMode==kOffData || fMode==kFixed)
    {
        SetSrcPos(fFixedPos/fGeom->GetConvMm2Deg());
        return kTRUE;
    }

    // If it is a monte carlo run calculate source position from
    // Monte Carlo headers
    if (fRunType==MRawRunHeader::kRTMonteCarlo)
    {
        // If the reflector version is too old take source position
        // from the WobbleMode in the header
        if (fMcHeader && fMcHeader->GetReflVersion()<=600)
        {
            SetSrcPos(fFixedPos/fGeom->GetConvMm2Deg());
            return kTRUE;
        }

        // If this is ceres data and we have a view cone keep
        // the source position fixed to the center of the camera
        if (fMcCorsika && fMcCorsika->HasViewCone())
        {
            SetSrcPos();
            return kTRUE;
        }

        // If the reflector version was new enough calculate the
        // source position from shower and telescope orientation
        // FIXME: Copy fMcEvt to fSourcePos!
        MVector3 pos0, pos;
        pos0.SetZdAz(fPointPos->GetZdRad(), fPointPos->GetAzRad());
        pos.SetZdAz(fMcEvt->GetParticleTheta(), fMcEvt->GetParticlePhi());

        CalcResult(pos0, pos);

        // If it was not ceres due to a bug in the MAGIC MCs
        // we have to fix the sign of X
        if (fMcHeader && !fMcHeader->IsCeres())
            SetSrcPos(TVector2(-fSrcPosCam->GetX(), fSrcPosCam->GetY()));

        return kTRUE;
    }

    // If the user requested a fixed source position use this position
    if (!fSourcePos || !fTime || !fObservatory)
    {
        SetSrcPos(fFixedPos/fGeom->GetConvMm2Deg());
        return kTRUE;
    }

    // Set Sky coordinates of source, taken from container "MSourcePos"
    // of type MPointingPos. The sky coordinates must be J2000, as the
    // sky coordinates of the camera center that we get from the container
    // "MPointingPos" filled by the Drive.
    MVector3 pos;  // pos: source position
    pos.SetRaDec(fSourcePos->GetRaRad(), fSourcePos->GetDecRad());

    // Set sky coordinates of camera center in pos0 (for details see below)
    MVector3 pos0;  // pos0: camera center
    pos0.SetRaDec(fPointPos->GetRaRad(), fPointPos->GetDecRad());

    // Here we check that the source position which was given
    // by the drive system is not off by more than 1deg from the
    // source position given by the user. (Checking every individual
    // event is not the fastest, but the safest)
    if (pos.Angle(pos0)*TMath::RadToDeg()>1)
    {
        *fLog << err << "ERROR - Defined source position deviates from pointing-ra/dec by more than 1deg (";
        *fLog << pos.Angle(pos0) << ")!" << endl;
        *fLog << "User defined source pos:  ";
        fSourcePos->Print();
        *fLog << "Pointing position of tel: ";
        fPointPos->Print();

        return kERROR;
    }

    // Convert sky coordinates of source to local coordinates. Warning! These are not the "true" local
    // coordinates, since this transformation ignores precession and nutation effects.
    const MAstroSky2Local conv(*fTime, *fObservatory);
    pos *= conv;

    // Convert sky coordinates of camera center convert to local.
    // Same comment as above. These coordinates differ from the true
    // local coordinates of the camera center that one could get from
    // "MPointingPos", calculated by the Drive: the reason is that the Drive
    // takes into account precession and nutation corrections, while
    // MAstroSky2Local (as of Jan 27 2005 at least) does not. Since we just
    // want to get the source position on the camera from the local
    // coordinates of the center and the source, it does not matter that
    // the coordinates contained in pos and pos0 ignore precession and
    // nutation... since the shift would be the same in both cases. What
    // would be wrong is to set in pos0 directly the local coordinates
    // found in MPointingPos!
    pos0 *= conv;

    //TVector2 vx;
    if (fDeviation)
    {
        // Position at which the starguider camera is pointing in real:
        //       pointing position = nominal position - dev
        //
        //vx = CalcXYinCamera(pos0, pos)*fGeom->GetCameraDist()*1000;
        pos0.SetZdAz(pos0.Theta()-fDeviation->GetDevZdRad(),
                     pos0.Phi()  -fDeviation->GetDevAzRad());
    }

    CalcResult(pos0, pos);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Convert a coordinate stored in str into a double, stored in ret.
// Returns kTRUE on success, otherwise kFALSE
//
// Allowed formats are:
//     12.5
//    -12.5
//    +12.5
//  -12:30:00.0
//   12:30:00.0
//  +12:30:00.0
//
Bool_t MSrcPosCalc::GetCoordinate(TString str, Double_t &ret) const
{
    str = str.Strip(TString::kBoth);

    if (str.First(':')<0)
    {
        ret = str.Atof();
        return kTRUE;
    }

    if (MAstro::Coordinate2Angle(str, ret))
        return kTRUE;

    *fLog << err << GetDescriptor() << endl;
    *fLog << "Interpretation of Coordinate '" << str << "' not successfull... abort." << endl;
    *fLog << "Corrdinate must have the format: '-12:30:00.0', '12:30:00.0' or '+12:30:00.0'" << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MSrcPosCalc.SourceRa:  ra
//   MSrcPosCalc.SourceDec: dec
//   MSrcPosCalc.SourcePos: ra dec
//
// For format of 'ra' and 'dec' see GetCoordinate()
//
// Coordinates are J2000.0
//
Int_t MSrcPosCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "FixedPos", print))
    {
        TString str = GetEnvValue(env, prefix, "FixedPos", "");
        str = str.Strip(TString::kBoth);

        Double_t x, y;
        const Int_t n=sscanf(str.Data(), "%lf %lf", &x, &y);
        if (n!=2)
        {
            *fLog << warn << "WARNING - FixedPos requires two numbers." << endl;
            return kERROR;
        }

        fFixedPos = TVector2(x, y);
        fMode = kFixed;

        rc = kTRUE;
    }

    Double_t ra=0;
    Double_t dec=0;

    if (IsEnvDefined(env, prefix, "SourceRaDec", print))
    {
        TString str = GetEnvValue(env, prefix, "SourceRaDec", "");
        str = str.Strip(TString::kBoth);

        const Ssiz_t pos = str.First(' ');
        if (pos<0)
        {
            *fLog << err << GetDescriptor() << "SourceRaDec empty... abort." << endl;
            return kERROR;
        }

        if (!GetCoordinate(str(0, pos), ra))
            return kERROR;
        if (!GetCoordinate(str(pos+1, str.Length()), dec))
            return kERROR;

        SetSourcePos(ra, dec);
        return rc;
    }

    Bool_t rcsrc = kFALSE;
    if (IsEnvDefined(env, prefix, "SourceRa", print))
    {
        TString str = GetEnvValue(env, prefix, "SourceRa", "");

        if (!GetCoordinate(str, ra))
            return kERROR;

        rcsrc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "SourceDec", print))
    {
        TString str = GetEnvValue(env, prefix, "SourceDec", "");

        if (!GetCoordinate(str, dec))
            return kERROR;

        rcsrc = kTRUE;
    }

    if (rcsrc)
        SetSourcePos(ra, dec);

    return rcsrc || rc;
}
