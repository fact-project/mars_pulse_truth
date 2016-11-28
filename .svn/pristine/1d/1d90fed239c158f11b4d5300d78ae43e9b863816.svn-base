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
!   Author(s): Thomas Bretz, 10/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Hendrik Bartko, 08/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHexagonFreqSpace
//
// This is a generalized class storing camera data. It stores real and
// imaginary part of the triangular frequency space of the MHexagonalFT.
//
/////////////////////////////////////////////////////////////////////////////
#include "MHexagonFreqSpace.h"

#include "MMath.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"


ClassImp(MHexagonFreqSpace);

using namespace std;

// --------------------------------------------------------------------------
//
// Creates a MCerPhotPix object for each pixel in the event
//
MHexagonFreqSpace::MHexagonFreqSpace(const char *name, const char *title)
{
    fName  = name  ? name  : "MHexagonFreqSpace";
    fTitle = title ? title : "Storage container for fourier space camera";
}

Double_t MHexagonFreqSpace::GetAbs(UInt_t idx) const
{
    return TMath::Hypot(fDataRe[idx], fDataIm[idx]);
}

/*
// --------------------------------------------------------------------------
//
// This is not yet implemented like it should.
//

void MHexagonFreqSpace::Draw(Option_t* option) 
{
    //
    //   FIXME!!! Here the Draw function of the CamDisplay
    //   should be called to add the CamDisplay to the Pad.
    //   The drawing should be done in MCamDisplay::Paint
    //

    //    MGeomCam *geom = fType ? new MGeomCamMagic : new MGeomCamCT1;
    //    MCamDisplay *disp = new MCamDisplay(geom);
    //    delete geom;
    //    disp->DrawPhotNum(this);
}
*/

// --------------------------------------------------------------------------
//
// Returns the contents of the pixel.
//  type 0: sqrt(re^2+im^2)
//  type 1: re
//  type 2: im
//
Bool_t MHexagonFreqSpace::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx<0 || (UInt_t)idx>=fDataRe.GetSize())
        return kFALSE;

    switch (type)
    {
    case 0:
        val = TMath::Hypot(fDataRe[idx], fDataIm[idx]);
        break;
    case 1:
        val = fDataRe[idx];
        break;
    case 2:
        val = fDataIm[idx];
        break;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set real and imaginary part from MArrayD re and im. The size of both
// arrays must be identical.
//
void MHexagonFreqSpace::Set(const MArrayD &re, const MArrayD &im)
{
    if (re.GetSize()!=im.GetSize())
        return;
/*
    fDataRe.Set(re.GetSize());
    fDataIm.Set(re.GetSize());
    for(unsigned int j=0; j<re.GetSize(); j++)
    {
        fDataRe[j]=re[j];
        fDataIm[j]=im[j];
    }
  */
    fDataRe = re;
    fDataIm = im;
}

void MHexagonFreqSpace::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MHexagonFreqSpace::DrawPixelContent - not available." << endl;
}

// --------------------------------------------------------------------------
//
// Create a new geometry based on the size num. which is the number
// of pixels in the triangle.
//
// It is YOUR responsibility to make sure that the allocated MGeomCam is
// deleted elsewhere!
//
// WARNING:
// ========
// * Currently there are some basic problems with MGeomCam -- so please don't
// wonder if you encounter strange behaviour, especially using this
// kind of MGeomCam as a data member in MHCamera which is itself a data
// member of some other class.
// This maybe fixed soon by intoducing a MGeomCamTriangle.
// * The created geometry does not contain ANY valid neighbor information yet.
//
MGeomCam *MHexagonFreqSpace::NewGeomCam(UShort_t num)
{
    static const Double_t fgTan30 = TMath::Tan(30*TMath::DegToRad())*3;

    MGeomCam *cam = new MGeomCam(num, 1);

    num = (int)((1.+sqrt(1.+8.*num))/2.)-1;

    for(int j=0; j<num; j++)
    {
        for(int n=0; n<num-j; n++)
        {
            int idx1 = (j+n)*(j+n+1)/2 + j;

            cam->SetAt(idx1, MGeomPix(n-j, (n+j-num/2)*fgTan30, 2));
            (*cam)[idx1].SetNeighbors();
        }
    }
    cam->InitGeometry();
    return cam;
}
