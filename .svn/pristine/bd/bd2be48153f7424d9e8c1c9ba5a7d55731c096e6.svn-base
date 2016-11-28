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
!   Author(s): Wolfgang Wittek 06/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHillasSrc
//
// Storage Container for image parameters
//
//    source-dependent image parameters
//
//
// Version 1:
// ----------
//  fAlpha          angle between major axis and line source-to-center
//  fDist           distance from source to center of ellipse
//
//
// Version 2:
// ----------
//  fHeadTail       added
//
//
// Version 3:
// ----------
//  fCosDeltaAlpha  cosine of angle between d and a, where
//                   - d is the vector from the source position to the
//                     center of the ellipse
//                   - a is a vector along the main axis of the ellipse,
//                     defined with positive x-component
//
//
// Version 4:
// ----------
// fHeadTail        removed
//
//
// Version 5:
// ----------
//  - added Float_t fDCA;      // [mm]   Distance to closest approach 'DCA'
//  - added Float_t fDCADelta; // [deg]  Angle of the shower axis with respect
//                                       to the x-axis [0,2pi]
//
/////////////////////////////////////////////////////////////////////////////
#include "MHillasSrc.h"

#include <TArrayF.h>

#include <TLine.h>
#include <TMarker.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MHillas.h"
#include "MSrcPosCam.h"

ClassImp(MHillasSrc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MHillasSrc::MHillasSrc(const char *name, const char *title)
{
    fName  = name  ? name  : "MHillasSrc";
    fTitle = title ? title : "Parameters depending in source position";
}

void MHillasSrc::Reset()
{
    fDist          = -1;
    fAlpha         =  0;
    fCosDeltaAlpha =  0;

    fDCA           = -1;
    fDCADelta      =  0;
}

// --------------------------------------------------------------------------
//
//  Calculation of source-dependent parameters
//  In case you don't call Calc from within an eventloop make sure, that
//  you call the Reset member function before.
//
Int_t MHillasSrc::Calc(const MHillas &hillas)
{
     const Double_t mx = hillas.GetMeanX();     // [mm]
     const Double_t my = hillas.GetMeanY();     // [mm]

     const Double_t sx = mx - fSrcPos->GetX();   // [mm]
     const Double_t sy = my - fSrcPos->GetY();   // [mm]

     const Double_t sd = hillas.GetSinDelta();  // [1]
     const Double_t cd = hillas.GetCosDelta();  // [1]

     //
     // Distance from source position to center of ellipse.
     // If the distance is 0 distance, Alpha is not specified.
     // The calculation has failed and returnes kFALSE.
     //
     const Double_t dist = TMath::Sqrt(sx*sx + sy*sy);  // [mm]
     if (dist==0)
         return 1;

     //
     // Calculate Alpha and Cosda = cos(d,a)
     // The sign of Cosda will be used for quantities containing
     // a head-tail information
     //
     // *OLD* const Double_t arg = (sy-tand*sx) / (dist*sqrt(tand*tand+1));
     // *OLD* fAlpha = asin(arg)*kRad2Deg;
     //
     const Double_t arg2 = cd*sx + sd*sy;          // [mm]
     if (arg2==0)
         return 2;

     const Double_t arg1 = cd*sy - sd*sx;          // [mm]

     //
     // Due to numerical uncertanties in the calculation of the
     // square root (dist) and arg1 it can happen (in less than 1e-5 cases)
     // that the absolute value of arg exceeds 1. Because this uncertainty
     // results in an Delta Alpha which is still less than 1e-3 we don't care
     // about this uncertainty in general and simply set values which exceed
     // to 1 saving its sign.
     //
     const Double_t arg = arg1/dist;
     fAlpha = TMath::Abs(arg)>1 ? TMath::Sign(90., arg) : TMath::ASin(arg)*TMath::RadToDeg(); // [deg]

     fCosDeltaAlpha = arg2/dist;                 // [1]
     fDist          = dist;                      // [mm]

     // ----- Calculation of Distance to closest approach 'DCA' -----

     // Components of DCA vector
     const Double_t fpd1 = sx - arg2*cd;         // [mm]
     const Double_t fpd2 = sy - arg2*sd;         // [mm]

     // Determine the correct sign of the DCA (cross product of DCA vector and the
     // vector going from the intersection point of the DCA vector with the shower axis
     // to the COG)
     const Double_t sign = arg2*cd*fpd2 - arg2*sd*fpd1;
     fDCA  = TMath::Sign(TMath::Sqrt(fpd1*fpd1 + fpd2*fpd2), sign); // [mm]

     // Calculate angle of the shower axis with respect to the x-axis
     fDCADelta = TMath::ACos((sx-fpd1)/TMath::Abs(arg2))*TMath::RadToDeg(); // [deg]

     // Enlarge the interval of fDdca to [0, 2pi]
     if (sy < fpd2)
         fDCADelta = 360 - fDCADelta;

     SetReadyToSave();

     return 0;
}

void MHillasSrc::Paint(Option_t *opt)
{
    const Float_t x = fSrcPos ? fSrcPos->GetX() : 0;
    const Float_t y = fSrcPos ? fSrcPos->GetY() : 0;

    TMarker m;
    m.SetMarkerColor(kYellow);
    m.SetMarkerStyle(kStar);
    m.PaintMarker(x, y);
/*
    m.SetMarkerColor(kMagenta);
    m.PaintMarker(fDist*cos((fDCADelta-fAlpha)*TMath::DegToRad()),
                  fDist*sin((fDCADelta-fAlpha)*TMath::DegToRad()));

    TLine line;
    line.SetLineWidth(1);
    line.SetLineColor(108);

    line.PaintLine(-radius, y, radius, y);
    line.PaintLine(x, -radius, x, radius);
  
    // COG line
    TLine line(x, y, meanX, meanY);
    line.SetLineWidth(1);
    line.SetLineColor(2);
    line.Paint();*/
}

// --------------------------------------------------------------------------
//
// Print contents of MHillasSrc to *fLog
//
void MHillasSrc::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Dist           [mm]  = " << fDist << endl;
    *fLog << " - Alpha          [deg] = " << fAlpha << endl;
    *fLog << " - CosDeltaAlpha        = " << fCosDeltaAlpha << endl;
    *fLog << " - DCA            [mm]  = " << fDCA << endl;
    *fLog << " - DCA delta      [deg] = " << fDCADelta << endl;
}

// --------------------------------------------------------------------------
//
// Print contents of MHillasSrc to *fLog depending on the geometry in
// units of deg.
//
void MHillasSrc::Print(const MGeomCam &geom) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Dist           [deg] = " << fDist*geom.GetConvMm2Deg() << endl;
    *fLog << " - Alpha          [deg] = " << fAlpha << endl;
    *fLog << " - CosDeltaAlpha        = " << fCosDeltaAlpha << endl;
    *fLog << " - DCA            [deg] = " << fDCA*geom.GetConvMm2Deg() << endl;
    *fLog << " - DCA delta      [deg] = " << fDCADelta << endl;
}

// --------------------------------------------------------------------------
//
// This function is ment for special usage, please never try to set
// values via this function
//
void MHillasSrc::Set(const TArrayF &arr)
{
    if (arr.GetSize() != 5)
        return;

    fAlpha = arr.At(0);         // [deg]  angle of major axis with vector to src
    fDist  = arr.At(1);         // [mm]   distance between src and center of ellipse
    fCosDeltaAlpha = arr.At(2); // [1]    cosine of angle between d and a
    fDCA = arr.At(3);           // [mm]
    fDCADelta = arr.At(4);      // [mm]
}
