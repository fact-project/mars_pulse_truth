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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHFadcCam
//
// This class contains a list of MHFadcPix.
//
///////////////////////////////////////////////////////////////////////

#include "MHFadcCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MGeomCam.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

ClassImp(MHFadcCam);

using namespace std;

// --------------------------------------------------------------------------
//
//  default constructor
//  creates an a list of histograms for all pixels and both gain channels
//
MHFadcCam::MHFadcCam(/*const Int_t n,*/ MHFadcPix::Type_t t, const char *name, const char *title)
    : fNumHiGains(-1), fNumLoGains(-1), fType(t)
{
    //
    // set the name and title of this object
    //
    fName  = name  ? name  : "MHFadcCam";
    fTitle = title ? title : "Container for ADC spectra histograms";

    //
    // loop over all Pixels and create two histograms
    // one for the Low and one for the High gain
    // connect all the histogram with the container fHist
    //
    fArray = new TObjArray;
    fArray->SetOwner();

    //    for (Int_t i=0; i<n; i++)
    //        (*fArray)[i] = new MHFadcPix(i, fType);
}

// --------------------------------------------------------------------------
MHFadcCam::~MHFadcCam()
{
    delete fArray;
}

// --------------------------------------------------------------------------
//
// Our own clone function is necessary since root 3.01/06 or Mars 0.4
// I don't know the reason
//
TObject *MHFadcCam::Clone(const char *) const
{
    const Int_t n = fArray->GetSize();

    //
    // FIXME, this might be done faster and more elegant, by direct copy.
    //
    MHFadcCam *cam = new MHFadcCam(fType);

    cam->fArray->Expand(n);

    for (int i=0; i<n; i++)
    {
        delete (*cam->fArray)[i];
        (*cam->fArray)[i] = (MHFadcPix*)(*fArray)[i]->Clone();
    }
    return cam;
}

// --------------------------------------------------------------------------
//
// To setup the object we get the number of pixels from a MGeomCam object
// in the Parameter list.
//
Bool_t MHFadcCam::SetupFill(const MParList *pList)
{
    MGeomCam *cam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam (Camera Geometry) missing in Parameter List... aborting." << endl;
        return kFALSE;
    }

    const Int_t n = cam->GetNumPixels();

    fArray->Delete();
    fArray->Expand(n);

    for (Int_t i=0; i<n; i++)
        (*fArray)[i] = new MHFadcPix(i, fType);

    return kTRUE;
}

// --------------------------------------------------------------------------
Bool_t MHFadcCam::Fill(const MParContainer *par, const Stat_t w)
{
    return Fill((MRawEvtData*)par);
}

Bool_t MHFadcCam::Fill(const MRawEvtData *par)
{
    const Int_t n = fArray->GetSize();

    if (fType==MHFadcPix::kSlices)
    {
        const Int_t nhi = par->GetNumHiGainSamples();

        //
        // skip MC events without FADC information stored
        //
        if (nhi==0)
            return kTRUE;

        const Int_t nlo = par->GetNumLoGainSamples();

        //
        // First call with nhi!=0
        //
        if (fNumHiGains<0)
            for (int i=0; i<n; i++)
                (*this)[i].Init(nhi, nlo);
        else
        {
            if (fNumLoGains!=nlo)
            {
                *fLog << err << dbginf << "ERROR - Number of lo-gain samples changed from " << fNumLoGains << " to " << nlo << endl;
                return kFALSE;
            }
            if (fNumHiGains!=nhi)
            {
                *fLog << err << dbginf << "ERROR - Number of hi-gain samples changed from " << fNumHiGains << " to " << nhi << endl;
                return kFALSE;
            }
        }

        fNumHiGains = nhi;
        fNumLoGains = nlo;
    }

    for (int i=0; i<n; i++)
        if (!(*this)[i].Fill(*par))
            return kFALSE;

    return kTRUE;
}

void MHFadcCam::ResetHistograms()
{
    const Int_t n = fArray->GetSize();
    for (Int_t i=0; i<n; i++)
        ResetEntry(i);
}

void MHFadcCam::ResetEntry(Int_t i)
{
    GetHistHi(i)->Reset();
    GetHistLo(i)->Reset();
}
