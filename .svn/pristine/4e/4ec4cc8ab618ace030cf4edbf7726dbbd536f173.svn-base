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
!   Author(s): Wolfgang Wittek, 04/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 04/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFCT1SelStandard
//
//  This is a class to evaluate the Standard Cuts
//
//  WHAT ARE THE STANDARD CUTS?                                                                       //
//
//  to be called after the calculation of the image parameters
//               before the g/h separation
//
/////////////////////////////////////////////////////////////////////////////

#include "MFCT1SelStandard.h"

#include "MParList.h"

#include "MMcEvt.hxx"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MCerPhotEvt.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHillas.h"
#include "MHillasExt.h"
#include "MHillasSrc.h"
#include "MNewImagePar.h"

ClassImp(MFCT1SelStandard);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MFCT1SelStandard::MFCT1SelStandard(const char *hilsrcname,
                                   const char *name, const char *title)
    : fHilName("MHillas"), fHilSrcName(hilsrcname), fImgParName("MNewImagePar")
{
    fName  = name  ? name  : "MFCT1SelStandard";
    fTitle = title ? title : "Class to evaluate the Standard Cuts";

    // default values of cuts
    SetCuts(92, 4, 60, 0.4, 1.05, 0.0, 0.0);
}

// --------------------------------------------------------------------------
//
// Set the values for the cuts 
// 
//
void MFCT1SelStandard::SetCuts(Float_t usedpixelsmax, Float_t corepixelsmin,
                               Float_t sizemin, Float_t distmin, Float_t distmax,
                               Float_t lengthmin, Float_t widthmin)
{ 
    fUsedPixelsMax = usedpixelsmax;
    fCorePixelsMin = corepixelsmin;
    fSizeMin       = sizemin;
    fDistMin       = distmin;
    fDistMax       = distmax;
    fLengthMin     = lengthmin;
    fWidthMin      = widthmin;

    *fLog << inf << "MFCT1SelStandard cut values : fUsedPixelsMax, fCorePixelsMin = ";
    *fLog << fUsedPixelsMax << ",  " << fCorePixelsMin << endl;
    *fLog << inf << "     fSizeMin, fDistMin, fDistMax = " << fSizeMin ;
    *fLog << ",  " << fDistMin << ",  " << fDistMax << endl;
    *fLog << inf << "     fLengthMin, fWidthMin = " << fLengthMin ;
    *fLog << ",  " << fWidthMin << endl;
}

// --------------------------------------------------------------------------
//
// MISSING
//
Int_t MFCT1SelStandard::PreProcess(MParList *pList)
{
    fHil = (MHillas*)pList->FindObject(fHilName, "MHillas");
    if (!fHil)
    {
        *fLog << err << fHilName << " [MHillas] not found... aborting." << endl;
        return kFALSE;
    }

    fHilSrc = (MHillasSrc*)pList->FindObject(fHilSrcName, "MHillasSrc");
    if (!fHilSrc)
    {
        *fLog << err << fHilSrcName << " [MHillasSrc] not found... aborting." << endl;
        return kFALSE;
    }

    fNewImgPar = (MNewImagePar*)pList->FindObject(fImgParName, "MNewImagePar");
    if (!fNewImgPar)
    {
        *fLog << err << fImgParName << " [MNewImagePar] not found... aborting." << endl;
        return kFALSE;
    }

    MGeomCam *cam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam (Camera Geometry) not found... aborting." << endl;
        return kFALSE;
    }

    fMm2Deg = cam->GetConvMm2Deg();

    memset(fCut, 0, sizeof(fCut));

    return kTRUE;
}

Bool_t MFCT1SelStandard::Set(Int_t rc)
{
    fResult = kTRUE;
    fCut[rc]++;
    return kTRUE;
}
// --------------------------------------------------------------------------
//
// Evaluate standard cuts
// 
// if selections are fulfilled set fResult = kTRUE;
// 
//
Int_t MFCT1SelStandard::Process()
{
    const Double_t length     = fHil->GetLength() * fMm2Deg;
    const Double_t width      = fHil->GetWidth()  * fMm2Deg;
    const Double_t dist       = fHilSrc->GetDist()* fMm2Deg;
    //const Double_t delta      = fHil->GetDelta()  * kRad2Deg;
    const Double_t size       = fHil->GetSize();
    const Int_t numusedpixels = fNewImgPar->GetNumUsedPixels();
    const Int_t numcorepixels = fNewImgPar->GetNumCorePixels();

    fResult  = kFALSE;

    if (numusedpixels>=fUsedPixelsMax || numcorepixels<=fCorePixelsMin)
        return Set(1);

    if (size<=fSizeMin )
        return Set(2);

    if (dist<fDistMin || dist>fDistMax)
        return Set(3);

    if (length<=fLengthMin || width<=fWidthMin)
        return Set(4);

    fCut[0]++;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the Standard selections.
//
Int_t MFCT1SelStandard::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');
    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3);
    *fLog << (int)(fCut[1]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: Used pixels >= " << fUsedPixelsMax ;
    *fLog << " or Core pixels <= " << fCorePixelsMin << endl;

    *fLog << " " << setw(7) << fCut[2] << " (" << setw(3) ;
    *fLog << (int)(fCut[2]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: SIZE <= " << fSizeMin << endl;

    *fLog << " " << setw(7) << fCut[3] << " (" << setw(3) ;
    *fLog << (int)(fCut[3]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: DIST < " << fDistMin;
    *fLog << " or DIST > " << fDistMax << endl;

    *fLog << " " << setw(7) << fCut[4] << " (" << setw(3) ;
    *fLog << (int)(fCut[4]*100/GetNumExecutions()) ;
    *fLog << "%) Evts skipped due to: LENGTH <= " << fLengthMin;
    *fLog << " or WIDTH <= " << fWidthMin << endl;

    *fLog << " " << fCut[0] << " (" ;
    *fLog << (int)(fCut[0]*100/GetNumExecutions()) ;
    *fLog << "%) Evts survived Standard selections!" << endl;
    *fLog << endl;

    return kTRUE;
}
