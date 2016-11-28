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
!   Author(s): Oscar Blanch, 11/2001 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcPedestalNSBAdd
//  -----------------
//
//  This Task adds the contribution of the diffuse NSB to the FADC
//  pedestals. We assume that NSB introduces larger fluctuation but does
//  not change the mean value.
//  To be precise we add quadratically to the rms, which is already in
//  MPedestalCam, the NSB contribution.
//  The number of photons from the diffuse NSB follows a poisson
//  distribution with expected mean value X, then its standard deviation
//  is sqrt(X).
//  X is the number of phe per ns so we have to convert in FADC counts per
//  slice:
//
//  Y = sqrt(X * FADC_time / Number_of_slices * pixel_size)
//      * Amp_single_phe_response
//
//  Input Containers:
//   MMcFadcHeader
//   MRawRunHeader
//   [MMcRunHeader]
//
//  Output Containers:
//   MPedestalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcPedestalNSBAdd.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MPedestalPix.h"
#include "MPedestalCam.h"

#include "MRawRunHeader.h"
#include "MMcRunHeader.hxx"
#include "MMcFadcHeader.hxx"

ClassImp(MMcPedestalNSBAdd);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMcPedestalNSBAdd::MMcPedestalNSBAdd(const Float_t difnsb,
                                     const char *name, const char *title)
    : fDnsbPixel(difnsb)
{
    fName  = name  ? name  : "MMcPedestalNSBAdd";
    fTitle = title ? title : "Add diffuse NSB to the pedestal signal";

    //
    // This is not needed here using MReadMarsFile because for the
    // RunHeader tree the auto scheme is disabled by default
    //
    AddToBranchList("MMcFadcHeader.fPedesMean");
    AddToBranchList("MMcFadcHeader.fElecNoise");
}

// --------------------------------------------------------------------------
//
// Check for the run type. Return kTRUE if it is a MC run or if there
// is no MC run header (old camera files) kFALSE in case of a different
// run type
//
Bool_t MMcPedestalNSBAdd::CheckRunType(MParList *pList) const
{
    const MRawRunHeader *runheader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!runheader)
    {
        *fLog << warn << dbginf << "Warning - cannot check file type, MRawRunHeader not found." << endl;
        return kTRUE;
    }

    return runheader->IsMonteCarloRun();
}

// --------------------------------------------------------------------------
//
// Check for the camera version. This class should not be used with
// camera files >= 0.7
//
Bool_t MMcPedestalNSBAdd::CheckCamVersion(MParList *pList) const
{
    const MMcRunHeader *run = (MMcRunHeader*)pList->FindObject("MMcRunHeader");
    if (!run)
        *fLog << warn << dbginf << "MMcRunHeader not found... assuming camera<0.7" << endl;

    return !run || run->GetCamVersion()<70;
}

// --------------------------------------------------------------------------
//
// If a MMcRunHeader is available the DNSB MMcRunHeader::GetNumPheFromDNSB
// is returned. Otherwise the user given number is used.
//
Float_t MMcPedestalNSBAdd::GetDnsb(MParList *pList) const
{
    const MMcRunHeader *mcrunheader = (MMcRunHeader*)pList->FindObject("MMcRunHeader");
    if (!mcrunheader && fDnsbPixel<0)
    {
        *fLog << err << dbginf << "Using the default argument only ";
        *fLog << "allowed if MMcRunHeader is available... aborting." << endl;
        return -1;
    }

    if (!mcrunheader)
        return fDnsbPixel;

    if (fDnsbPixel >= 0 && fDnsbPixel != mcrunheader->GetNumPheFromDNSB())
    {
        *fLog << warn << dbginf << "The MC file has been generated with diffuse nsb " << mcrunheader->GetNumPheFromDNSB();
        *fLog <<" but you set up the diffuse NSB to " << fDnsbPixel << endl;

        return fDnsbPixel;
    }

    return mcrunheader->GetNumPheFromDNSB();
}

// --------------------------------------------------------------------------
//
//  This function is called each time MReadTree::Notify is called, which 
//  happens when it  changes the file to read from.
//  Here we add the contribution from NSB to the pedestals.
//  The ReInit searches for the following input containers:
//   - MRawRunHeader
//   - MMcRunHeader
//   - MMcFacdHeader
//   - MGeomCam
//
//   The following output containers are also searched and created if
//   they were not found:
//   - MPedestalCam
//
Bool_t MMcPedestalNSBAdd::ReInit(MParList *pList)
{
    //
    // If it is no MC file skip this function...
    //
    if (!CheckRunType(pList))
        return kTRUE;

    //
    // If it is the wrong camera version this algorithm is not needed...
    //
    if (!CheckCamVersion(pList))
        return kTRUE;

    //
    // Now check the existance of all necessary containers. This has
    // to be done only if this is a MC file and the camera version
    // is correct.
    //
    MPedestalCam *pedcam = (MPedestalCam*)pList->FindCreateObj(AddSerialNumber("MPedestalCam"));
    if (!pedcam)
	return kFALSE;

    MMcFadcHeader *fadc = (MMcFadcHeader*)pList->FindObject(AddSerialNumber("MMcFadcHeader"));
    if (!fadc)
    {
        *fLog << err << dbginf << "MMcFadcHeader not found... aborting." << endl;
	return kFALSE;
    }

    MGeomCam *geom = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!geom)
    {
        *fLog << err << dbginf << "MGeomCam not found... aborting." << endl;
	return kFALSE;
    }

    const Float_t dnsbpix = GetDnsb(pList) * 50.0/15.0;

    if (dnsbpix < 0)
        return kFALSE;

    const int num = pedcam->GetSize();

    for (int i=0; i<num; i++)
    {
        MPedestalPix &pix    = (*pedcam)[i];

        const Float_t pedrms = pix.GetPedestalRms();
        const Float_t ratio  = geom->GetPixRatio(i);
        const Float_t ampl   = fadc->GetAmplitud();

	pix.SetPedestalRms(sqrt(pedrms*pedrms + dnsbpix*ampl*ampl/ratio));
    }

    pedcam->SetReadyToSave();

    return kTRUE;
}
