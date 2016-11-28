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
!   Author(s): Thomas Bretz, 02/2004 <mailto:tbretz@astro.uni.wuerzburg.de>
!   Author(s): Stefan Ruegamer, 08/2005 <mailto:snruegam@astro.uni.wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MBadPixelsCalc
//
//
// The job of the task is to determin bad pixels event-wise. This must be
// redone for each event. This particular task is for what is explained
// below.
// New algorithms may enter new tasks.
//
//
// Check the pedestal RMS of every pixel with respect to the mean
// pedestal RMS of the camera.
//
// The pixels can be set as blind if the pedestalRMS is too big or 0.
//
// If you don't want to use this option set the PedestalLevel<=0;
//
//     MBadPixelsCalc calc;
//     calc.SetPedestalLevel(-1);
//
//
//  Input Containers:
//   [MPedPhotCam]
//   [MGeomCam]
//
//  Output Containers:
//   MBadPixels
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsCalc.h"

#include <TEnv.h>

#include "MArrayI.h"
#include "MArrayD.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MBadPixelsCalc);

using namespace std;

static const TString gsDefName  = "MBadPixelsCalc";
static const TString gsDefTitle = "Find hot spots (star, broken pixels, etc)";

// --------------------------------------------------------------------------
//
// Default constructor.
//
MBadPixelsCalc::MBadPixelsCalc(const char *name, const char *title)
    : fPedestalLevel(3), fPedestalLevelVarianceLo(5),
    fPedestalLevelVarianceHi(5), fNamePedPhotCam("MPedPhotCam"),
    fCheckInProcess(kTRUE), fCheckInPostProcess(kFALSE)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
//
Int_t MBadPixelsCalc::PreProcess (MParList *pList)
{
    fBadPixels = (MBadPixelsCam*)pList->FindCreateObj(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
        return kFALSE;

    if (fPedestalLevel>0)
    {
        fPedPhotCam = (MPedPhotCam*)pList->FindObject(AddSerialNumber(fNamePedPhotCam), "MPedPhotCam");
        if (!fPedPhotCam)
        {
	  *fLog << err << fNamePedPhotCam << "[MPedPhotCam] not found... aborting." << endl;
            return kFALSE;
        }

        fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
        if (!fGeomCam)
        {
            *fLog << err << "MGeomCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    *fLog << inf << "Name of MPedPhotCam to get pedestal rms from: " << fNamePedPhotCam << endl;
    if (fPedestalLevel>0)
        *fLog << "Checking mean 'pedestal rms' against absolute value with level " << fPedestalLevel << endl;
    if (fPedestalLevelVarianceLo>0)
        *fLog << "Checking mean 'pedestal rms' against its lower variance with level " << fPedestalLevelVarianceLo << endl;
    if (fPedestalLevelVarianceHi>0)
        *fLog << "Checking mean 'pedestal rms' against its upper variance with level " << fPedestalLevelVarianceHi << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check the pedestal Rms of the pixels: compute with 2 iterations the mean 
// for inner and outer pixels. Set as blind the pixels with too small or 
// too high pedestal Rms with respect to the mean.
// 
Bool_t MBadPixelsCalc::CheckPedestalRms(MBadPixelsPix::UnsuitableType_t type) const
{
    const Bool_t checklo = fPedestalLevelVarianceLo>0;
    const Bool_t checkhi = fPedestalLevelVarianceHi>0;

    if (fPedestalLevel<=0 && !checklo && !checkhi)
        return kTRUE;

    if (!fGeomCam || !fPedPhotCam || !fBadPixels)
    {
        *fLog << err << "MBadPixelsCalc::CheckPedestalRms: ERROR - One of the necessary container are not initialized..." << endl;
        return kFALSE;
    }

    const Int_t entries = fPedPhotCam->GetSize();

    const Int_t na = fGeomCam->GetNumAreas();

    MArrayD meanrms(na);
    MArrayI npix(na);

    for (Int_t i=0; i<entries; i++)
    {
        if ((*fBadPixels)[i].IsUnsuitable())
            continue;

        const Double_t rms = (*fPedPhotCam)[i].GetRms();

        if (rms<=0 || rms>=200*fGeomCam->GetPixRatioSqrt(i))
            continue;

        const Byte_t aidx = (*fGeomCam)[i].GetAidx();

        meanrms[aidx] += rms;
        npix[aidx]++;
    }

    //if no pixel has a minimum signal, return
    Int_t counter=0;
    for (int i=0; i<na; i++)
    {
        if (npix[i]==0 || meanrms[i]==0) //no valid Pedestals Rms
        {
            counter++;
            continue;
        }

        meanrms[i] /= npix[i];
        npix[i]=0;
    }

    if (counter==na)
    {
        *fLog << err << "MBadPixelsCalc::CheckPedestalRms: ERROR - No pixel seems to contain a valid pedestal RMS..." << endl;
        return kFALSE;
    }

    MArrayD meanrms2(na);
    MArrayD varrms2(na);
    for (Int_t i=0; i<entries; i++)
    {
        if ((*fBadPixels)[i].IsUnsuitable())
            continue;

        const Double_t rms = (*fPedPhotCam)[i].GetRms();
        const Byte_t  aidx = (*fGeomCam)[i].GetAidx();

        //Calculate the corrected means:
        if (rms<=meanrms[aidx]/1.5 || rms>=meanrms[aidx]*1.5)
            continue;

        meanrms2[aidx] += rms;
        varrms2 [aidx] += rms*rms;
        npix[aidx]++;
    }

    //if no pixel has a minimum signal, return
    MArrayD lolim1(na), lolim2(na); // Precalcualtion of limits
    MArrayD uplim1(na), uplim2(na); // for speeed reasons
    counter = 0;
    for (int i=0; i<na; i++)
    {
        if (npix[i]==0 || meanrms2[i]==0)
        {
            counter++;
            continue;
        }

        meanrms2[i] /= npix[i];

        if (fPedestalLevel>0)
        {
            lolim1[i]  = meanrms2[i]/fPedestalLevel;
            uplim1[i]  = meanrms2[i]*fPedestalLevel;
        }

        if (checklo || checkhi)
        {
            varrms2[i] /= npix[i];
            varrms2[i]  = TMath::Sqrt(varrms2[i]-meanrms2[i]*meanrms2[i]);

            lolim2[i]   = meanrms2[i]-fPedestalLevelVarianceLo*varrms2[i];
            uplim2[i]   = meanrms2[i]+fPedestalLevelVarianceHi*varrms2[i];
        }
    }

    if (counter==na)
    {
        *fLog << err << "MBadPixelsCalc::CheckPedestalRms: ERROR - No pixel seems to contain a valid pedestal RMS anymore..." << endl;
        return kFALSE;
    }

    Int_t bads = 0;

    //Blind the Bad Pixels
    for (Int_t i=0; i<entries; i++)
    {
        if ((*fBadPixels)[i].IsUnsuitable())
            continue;

        const Double_t rms = (*fPedPhotCam)[i].GetRms();
        const Byte_t  aidx = (*fGeomCam)[i].GetAidx();

        if ((fPedestalLevel<=0 || (rms> lolim1[aidx] && rms<=uplim1[aidx])) &&
            (!checklo          ||  rms> lolim2[aidx])                       &&
            (!checkhi          ||  rms<=uplim2[aidx])
           )
            continue;

        (*fBadPixels)[i].SetUnsuitable(type);
        if (type==MBadPixelsPix::kUnsuitableRun)
            (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeadPedestalRms);

        bads++;
    }

    // Check if the number of pixels to blind is > 60% of total number of pixels
    //
    if (bads>0.5*entries)
    {
        *fLog << err << "ERROR - More than 50% unsuitable pixels (" << bads << "/" << entries << ")... something must be wrong!" << endl;
        return kFALSE;
    }

    return kTRUE;
}

Bool_t MBadPixelsCalc::CheckPedestalRms(MBadPixelsCam &cam, const MPedPhotCam &ped, MBadPixelsPix::UnsuitableType_t t)
{
    MBadPixelsCam *store1 = fBadPixels;
    const MPedPhotCam *store2 = fPedPhotCam;

    fBadPixels  = &cam;
    fPedPhotCam = &ped;

    const Bool_t rc = CheckPedestalRms(t);

    fBadPixels  = store1;
    fPedPhotCam = store2;

    return rc;
}

Bool_t MBadPixelsCalc::CheckPedestalRms(MBadPixelsCam &cam, const MPedestalCam &ped, MBadPixelsPix::UnsuitableType_t t)
{
    return CheckPedestalRms(cam, MPedPhotCam(ped), t);
}

// --------------------------------------------------------------------------
//
//
Int_t MBadPixelsCalc::Process()
{
    if (!fCheckInProcess)
        return kTRUE;

    return CheckPedestalRms(MBadPixelsPix::kUnsuitableEvt) ? kTRUE : kERROR;
}

// --------------------------------------------------------------------------
//
//
Int_t MBadPixelsCalc::PostProcess()
{
    return fCheckInPostProcess ? CheckPedestalRms(MBadPixelsPix::kUnsuitableRun) : kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MBadPixelsCalc.PedestalLevel:         3.0
//
//   MBadPixelsCalc.PedestalLevelVariance:   5.0
//     overwrites
//   MBadPixelsCalc.PedestalLevelVarianceLo: 5.0
//     and
//   MBadPixelsCalc.PedestalLevelVarianceHi: 5.0
//
Int_t MBadPixelsCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "PedestalLevel", print))
    {
        rc = kTRUE;
        SetPedestalLevel(GetEnvValue(env, prefix, "PedestalLevel", fPedestalLevel));
    }
    if (IsEnvDefined(env, prefix, "PedestalLevelVarianceLo", print))
    {
        rc = kTRUE;
        SetPedestalLevelVarianceLo(GetEnvValue(env, prefix, "PedestalLevelVarianceLo", fPedestalLevelVarianceLo));
    }
    if (IsEnvDefined(env, prefix, "PedestalLevelVarianceHi", print))
    {
        rc = kTRUE;
        SetPedestalLevelVarianceHi(GetEnvValue(env, prefix, "PedestalLevelVarianceHi", fPedestalLevelVarianceHi));
    }

    if (IsEnvDefined(env, prefix, "PedestalLevelVariance", print))
    {
        rc = kTRUE;
        SetPedestalLevelVariance(GetEnvValue(env, prefix, "PedestalLevelVariance", -1));
    }
    return rc;
}
