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
!   Author(s): Thomas Bretz 03/2005, <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MNewImagePar2
//
// Storage Container for new image parameters
//
/////////////////////////////////////////////////////////////////////////////
#include "MNewImagePar2.h"

#include <TArrayI.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

ClassImp(MNewImagePar2);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MNewImagePar2::MNewImagePar2(const char *name, const char *title)
{
    fName  = name  ? name  : "MNewImagePar2";
    fTitle = title ? title : "New image parameters 2";

    Reset();
}

// --------------------------------------------------------------------------
//
void MNewImagePar2::Reset()
{
    fBorderLinePixel  = 0;
    fBorderLineCenter = 0;
}

// --------------------------------------------------------------------------
//
//  Calculation of new image parameters
//
void MNewImagePar2::Calc(const MGeomCam &geom, const MSignalCam &evt, Int_t island)
{
    TArrayI idx(evt.GetNumPixels());
    idx.Reset(-1);
    Int_t cnt=0;

    Int_t n = evt.GetNumPixels();
    for (int i=0; i<n; i++)
    {
        const MSignalPix &pix = evt[i];
        if (!pix.IsPixelUsed())
            continue;

        if (island>=0 && pix.GetIdxIsland()!=island)
            continue;

        Int_t used=0;

        const MGeom &gpix = geom[i];
        const Int_t nn = gpix.GetNumNeighbors();
        for (int j=0; j<nn; j++)
        {
            const Int_t k = gpix.GetNeighbor(j);
            if (evt[k].IsPixelUsed())
                used++;
        }

        if (used<nn)
        {
            idx[cnt++] = i;
            evt[i].SetBit(BIT(14));
        }
        else
            evt[i].ResetBit(BIT(14));

        // FIXME: GetT is not the correct value
        fBorderLinePixel += (nn-used)*gpix.GetL();
    }

    for (Int_t m=0; idx[m]>=0; m++)
    {
        const Int_t l = idx[m];

        const MGeom &gpix = geom[l];

        const Int_t nn = gpix.GetNumNeighbors();
        for (int j=0; j<nn; j++)
        {
            const Int_t k = gpix.GetNeighbor(j);
            if (k<l)
                continue;

            if (!evt[k].IsPixelUsed())
                continue;

            if (!evt[k].TestBit(BIT(14)))
                continue;

            fBorderLineCenter += TMath::Hypot(gpix.GetX()-geom[k].GetX(),
                                              gpix.GetY()-geom[k].GetY());
        }
    }
}

// --------------------------------------------------------------------------
//
void MNewImagePar2::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Border L.Pixel  [mm] = " << fBorderLinePixel  << endl;
    *fLog << " - Border L.Center [mm] = " << fBorderLineCenter << endl;
}

// -------------------------------------------------------------------------
//
// Print contents of MNewImagePar to *fLog, depending on the geometry in
// units of deg.
//
void MNewImagePar2::Print(const MGeomCam &geom) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - BorderL.Pixel  [deg] = " << fBorderLinePixel*geom.GetConvMm2Deg()  << endl;
    *fLog << " - BorderL.Center [deg] = " << fBorderLineCenter*geom.GetConvMm2Deg() << endl;
}
