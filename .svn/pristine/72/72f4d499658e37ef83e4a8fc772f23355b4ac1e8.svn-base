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
!   Author(s): Thomas Bretz    12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Harald Kornmayer 1/2001
!   Author(s): Rudolf Bock     10/2001 <mailto:Rudolf.Bock@cern.ch>
!   Author(s): Wolfgang Wittek  6/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHillas
//
// Storage Container for image parameters
//
//    basic image parameters
//
// Version 1:
// ----------
// fLength   [mm]       major axis of ellipse
// fWidth    [mm]       minor axis
// fDelta    [rad]      angle of major axis with x-axis
//                      by definition the major axis is pointing into
//                      the hemisphere x>0, thus -pi/2 < delta < pi/2
// fSize     [#CerPhot] total sum of pixels
// fMeanX    [mm]       x of center of ellipse
// fMeanY    [mm]       y of center of ellipse
//
// Version 2:
// ----------
// fNumCorePixels  number of pixels called core
// fNumUsedPixels  number of pixels which survived the cleaning
//
// Version 3:
// ----------
// fNumCorePixels  moved to MNewImagePar
// fNumUsedPixels  moved to MNewImagePar
// fCosDelta       added
// fSinDelte       added
//
/////////////////////////////////////////////////////////////////////////////
#include "MHillas.h"

#include <TArrayF.h>

#include <TLine.h>
#include <TEllipse.h>
#include <TVector2.h>

#include "MGeom.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHillas);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MHillas::MHillas(const char *name, const char *title)
{
    fName  = name  ? name  : "MHillas";
    fTitle = title ? title : "Storage container for image parameters of one event";

    Reset();
}

// --------------------------------------------------------------------------
//
// Initializes the values with defaults. For the default values see the
// source code.
//
void MHillas::Reset()
{
    fLength = -1;
    fWidth  = -1;
    fDelta  =  0;

    fSize   = -1;
    fMeanX  =  0;
    fMeanY  =  0;
}

// --------------------------------------------------------------------------
//
//  return distance to center (coordinate origin)
//
Double_t MHillas::GetDist0() const
{
    return TMath::Hypot(fMeanX, fMeanY);
} 

// --------------------------------------------------------------------------
//
//  return the mean position as TVector2(meanx, meany)
//
TVector2 MHillas::GetMean() const
{
    return TVector2(fMeanX, fMeanY);
}

// --------------------------------------------------------------------------
//
//  return the direction of the main axis (normalized). The main axis
//  always has the direction  -->  x>0
//
TVector2 MHillas::GetNormAxis() const
{
    return TVector2(fCosDelta, fSinDelta);
}

// --------------------------------------------------------------------------
//
//  Analytical estimation of length of border line:
//    U = pi*(a+b - sqrt(a*b))
//
//  GetBorderLine() [mm]
//
Double_t MHillas::GetBorderLine() const
{
    const Double_t a = fWidth+fLength;
    const Double_t s = fWidth*fLength;

    return TMath::Pi()*(1.5*a - TMath::Sqrt(s));
}

// --------------------------------------------------------------------------
//
//  Analytical estimation of length of border line:
//    A = pi*a*b
//
//  GetArea() [mm^2]
//
Double_t MHillas::GetArea() const
{
    return TMath::Pi()*fWidth*fLength;
}

// --------------------------------------------------------------------------
//
// Print the hillas Parameters to *fLog
//
void MHillas::Print(Option_t *) const
{
    const Double_t atg = atan2(fMeanX, fMeanY)*kRad2Deg;

    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Length         [mm]  = " << fLength << endl;
    *fLog << " - Width          [mm]  = " << fWidth  << endl;
    *fLog << " - Delta          [deg] = " << fDelta*kRad2Deg << endl;
    *fLog << " - Size           [phe] = " << fSize   << endl;
    *fLog << " - Meanx          [mm]  = " << fMeanX  << endl;
    *fLog << " - Meany          [mm]  = " << fMeanY  << endl;
    *fLog << " - atg(y/x)       [deg] = " << atg     << endl;
}

// --------------------------------------------------------------------------
//
// Print the hillas Parameters to *fLog, depending on the geometry in
// units of deg
//
void MHillas::Print(const MGeomCam &geom) const
{
    const Double_t atg = atan2(fMeanX, fMeanY)*kRad2Deg;

    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Length         [deg] = " << fLength*geom.GetConvMm2Deg() << endl;
    *fLog << " - Width          [deg] = " << fWidth *geom.GetConvMm2Deg() << endl;
    *fLog << " - Delta          [deg] = " << fDelta*kRad2Deg << endl;
    *fLog << " - Size           [phe] = " << fSize << endl;
    *fLog << " - Meanx          [deg] = " << fMeanX *geom.GetConvMm2Deg() << endl;
    *fLog << " - Meany          [deg] = " << fMeanY *geom.GetConvMm2Deg() << endl;
    *fLog << " - atg(y/x)       [deg] = " << atg     << endl;
}

// --------------------------------------------------------------------------
//
// Paint the ellipse corresponding to the parameters
//
void MHillas::Paint(Option_t *opt)
{
    static const Float_t sOffsetW=40.0;
    static const Float_t sOffsetL=266.0;

    if (fLength<0 || fWidth<0)
        return;

    const Float_t l = fLength+sOffsetL;
    const Float_t w = fWidth +sOffsetW;

    TLine line;
    line.SetLineWidth(1);
    line.SetLineColor(kRed);

    // Length line
    line.PaintLine(-l*fCosDelta+fMeanX, -l*fSinDelta+fMeanY,
                   +l*fCosDelta+fMeanX, +l*fSinDelta+fMeanY);


    // Width line
    line.PaintLine(+w*fSinDelta+fMeanX, -w*fCosDelta+fMeanY,
                   -w*fSinDelta+fMeanX, +w*fCosDelta+fMeanY);

    // Rough estimate for disp
    const Double_t p = 362*(1-fWidth/fLength);

    // Disp line
    line.PaintLine( p*fCosDelta+20*fSinDelta+fMeanX,  p*fSinDelta-20*fCosDelta+fMeanY,
                    p*fCosDelta-20*fSinDelta+fMeanX,  p*fSinDelta+20*fCosDelta+fMeanY);
    line.PaintLine(-p*fCosDelta+20*fSinDelta+fMeanX, -p*fSinDelta-20*fCosDelta+fMeanY,
                   -p*fCosDelta-20*fSinDelta+fMeanX, -p*fSinDelta+20*fCosDelta+fMeanY);

    TEllipse e(fMeanX, fMeanY, fLength, fWidth, 0, 360, fDelta*kRad2Deg+180);
    e.SetLineWidth(2);
    e.SetLineColor(kGreen);
    e.Paint();
}

// --------------------------------------------------------------------------
//
// Calculate the image parameters from a Cherenkov photon event
// assuming Cher.photons/pixel and pixel coordinates are given
// In case you don't call Calc from within an eventloop make sure, that
// you call the Reset member function before.
// Returns:
//   0  no error
//   1  number of pixels in event == 0 (special MC events)
//   2  size==0
//   3  number of used pixel < 3
//   4  CorrXY == 0
//
Int_t MHillas::Calc(const MGeomCam &geom, const MSignalCam &evt, Int_t island)
{
    const UInt_t numpix  = evt.GetNumPixels();

    //
    // sanity check 1 (special MC events)
    //
    if (numpix==0)
        return 1;

    //
    // calculate mean value of pixel coordinates and fSize
    // -----------------------------------------------------
    //
    // Because this are only simple sums of roughly 600 values
    // with an accuracy less than three digits there should not
    // be any need to use double precision (Double_t) for the
    // calculation of fMeanX, fMeanY and fSize.
    //
    fMeanX = 0;
    fMeanY = 0;
    fSize  = 0;

    UInt_t numused = 0;

    for (UInt_t i=0; i<numpix; i++)
    {
        MSignalPix &pix = evt[i];
        if (!pix.IsPixelUsed())
            continue;

        if (island>=0 && pix.GetIdxIsland()!=island)
            continue;

        const MGeom &gpix = geom[i];

        const Float_t nphot = pix.GetNumPhotons();

        fSize  += nphot;		             // [counter]
        fMeanX += nphot * gpix.GetX();               // [mm]
        fMeanY += nphot * gpix.GetY();               // [mm]

        numused++;
    }

    //
    // sanity check 2
    //
    if (fSize==0)
        return 2;

    fMeanX /= fSize;                                 // [mm]
    fMeanY /= fSize;                                 // [mm]

    //
    // sanity check 3
    //
    if (numused<3)
        return 3;

    //
    // calculate 2nd moments
    // ---------------------
    //
    // To make sure that the more complicated sum is evaluated as
    // accurate as possible (because it is needed for more
    // complicated calculations (see below) we calculate it using
    // double prcision.
    //
    Double_t corrxx=0;                               // [m^2]
    Double_t corrxy=0;                               // [m^2]
    Double_t corryy=0;                               // [m^2]

    for (UInt_t i=0; i<numpix; i++)
    {
        const MSignalPix &pix = evt[i];
        if (!pix.IsPixelUsed())
            continue;

        if (island>=0 && pix.GetIdxIsland()!=island)
            continue;

        const MGeom &gpix = geom[i];

        const Float_t dx = gpix.GetX() - fMeanX;     // [mm]
        const Float_t dy = gpix.GetY() - fMeanY;     // [mm]

        const Float_t nphot = pix.GetNumPhotons();   // [#phot]

        corrxx += nphot * dx*dx;                     // [mm^2]
        corrxy += nphot * dx*dy;                     // [mm^2]
        corryy += nphot * dy*dy;                     // [mm^2]
    }

    //
    // calculate the basic Hillas parameters: orientation and size of axes
    // -------------------------------------------------------------------
    //
    // fDelta is the angle between the major axis of the ellipse and
    //  the x axis. It is independent of the position of the ellipse
    //  in the camera it has values between -pi/2 and pi/2 degrees
    //
    // Rem: I tested replacing sqrt() by hypot() but they exactly
    //      consume the same amount of time
    //
    const Double_t d0    = corryy - corrxx;
    const Double_t d1    = corrxy*2;
    const Double_t d2    = TMath::Sqrt(d0*d0 + d1*d1) + d0; // [0

    const Double_t tand  = d2==0 ? 0 : d2 / d1; // Force 0 also if d1==0
    const Double_t tand2 = tand*tand;

    const Double_t s2    = tand2+1;
    const Double_t s     = TMath::Sqrt(s2);

    // Set default for the case in which the image is symmetric on the y-axis
    fDelta    = TMath::Pi()/2;
    fCosDelta = 0;
    fSinDelta = 1;

    Double_t axis1 = corryy;
    Double_t axis2 = corrxx;

    // This are all cases in which the image is not symmetric on the y-axis
    if (d1!=0 || d2==0)
    {
        fDelta    = TMath::ATan(tand);

        fCosDelta = 1.0 /s;   // need these in derived classes
        fSinDelta = tand/s;   // like MHillasExt

        axis1 = (tand2*corryy + d2 + corrxx)/s2;
        axis2 = (tand2*corrxx - d2 + corryy)/s2;
    }

    //
    // fLength^2 is the second moment along the major axis of the distribution
    // fWidth^2  is the second moment along the minor axis of the distribution
    //
    // From the algorithm we get: fWidth <= fLength is always true
    //
    // very small numbers can get negative by rounding
    //
    fLength = axis1<0 ? 0 : TMath::Sqrt(axis1/fSize);  // [mm]
    fWidth  = axis2<0 ? 0 : TMath::Sqrt(axis2/fSize);  // [mm]

    SetReadyToSave();

    return 0;
}

// --------------------------------------------------------------------------
//
// This function is ment for special usage, please never try to set
// values via this function
//
void MHillas::Set(const TArrayF &arr)
{
    if (arr.GetSize() != 6)
        return;

    fLength = arr.At(0);  // [mm]        major axis of ellipse
    fWidth  = arr.At(1);  // [mm]        minor axis of ellipse
    fDelta  = arr.At(2);  // [rad]       angle of major axis with x-axis
    fSize   = arr.At(3);  // [#CerPhot]  sum of content of all pixels (number of Cherenkov photons)
    fMeanX  = arr.At(4);  // [mm]        x-coordinate of center of ellipse
    fMeanY  = arr.At(5);  // [mm]        y-coordinate of center of ellipse
}
