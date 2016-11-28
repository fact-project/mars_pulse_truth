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
!   Author(s): Thomas Bretz    12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Rudolf Bock     10/2001 <mailto:Rudolf.Bock@cern.ch>
!   Author(s): Wolfgang Wittek 06/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHillasExt
//
// Storage Container for extended image parameters
//
//    extended image parameters
//
//
// Version 1:
// ----------
// fConc     ratio of sum of two highest pixels over fSize
// fConc1    ratio of highest pixel over fSize
// fAsym     distance from highest pixel to center, projected onto major axis
// fM3Long   third moment along major axis
// fM3Trans  third moment along minor axis
//
// Version 2:
// ----------
// fConc     removed
// fConc1    removed
//
// Version 3:
// ----------
// fMaxDist  added. Distance between center and most distant used pixel
//
// Version 4:
// ----------
// fMaxDist      removed.
// fSlopeLong    added
// fSlopeTrans   added
//
// Version 5:
// ----------
// fTimeSpread           added
// fTimeSpreadWeighted   added
//
// fSlopeSpread          added
// fSlopeSpreadWeighted  added
//
//
// WARNING: Before you can use fAsym, fSlope* and fM3* you must
//          multiply by the sign of MHillasSrc::fCosDeltaAlpha
//
////////////////////////////////////////////////////////////////////////////
/*
 // fAsymna   d/(d na) of ( sum(x*q^na)/sum(q^na), sum(y*q^na)/sum(q^na) )
 //           projected onto the major axis
 // fAsym0    (F-B)/(F+B) along the major axis
 */
#include "MHillasExt.h"

#include <TArrayF.h>
#include <TMarker.h>
#include <TVector2.h>
#include <TMinuit.h>
#include <TVirtualPad.h>

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHillas.h"

ClassImp(MHillasExt);

using namespace std;

// -------------------------------------------------------------------------
//
// Default constructor.
//
MHillasExt::MHillasExt(const char *name, const char *title)
{
    fName  = name  ? name  : "MHillasExt";
    fTitle = title ? title : "Storage container for extended parameter set of one event";

    Reset();
}

// -------------------------------------------------------------------------
//
// Reset all values to 0
//
void MHillasExt::Reset()
{
    fAsym       =  0;
    fM3Long     =  0;
    fM3Trans    =  0;
    fSlopeLong  =  0;
    fSlopeTrans =  0;

    fTimeSpread          = -1;
    fTimeSpreadWeighted  = -1;
    fSlopeSpread         = -1;
    fSlopeSpreadWeighted = -1;
}

// -------------------------------------------------------------------------
//
// calculation of additional parameters based on the camera geometry
// and the cerenkov photon event
//
Int_t MHillasExt::Calc(const MGeomCam &geom, const MSignalCam &evt, const MHillas &hil, Int_t island)
{
    //
    //   calculate the additional image parameters
    // --------------------------------------------
    //
    //  loop to get third moments along ellipse axes and two max pixels
    //
    //  For the moments double precision is used to make sure, that
    //  the complex matrix multiplication and sum is evaluated correctly.
    //
    Int_t maxpixid  = 0;
    Float_t maxpix  = 0;

    // Variables to caluclate time slope
    // Formula: http://mo.mathematik.uni-stuttgart.de/inhalt/erlaeuterung/erlaeuterung300/
    UInt_t cnt = 0;

    Double_t sumx      = 0;
    Double_t sumy      = 0;
    Double_t sumt      = 0;
    Double_t sumw      = 0;

    Double_t sumxt     = 0;
    Double_t sumyt     = 0;

    Double_t sumx2     = 0;
    Double_t sumy2     = 0;
    Double_t sumxy     = 0;
    Double_t sumt2     = 0;

    Double_t sumxw     = 0;
    Double_t sumyw     = 0;
    Double_t sumtw     = 0;

    Double_t sumx2w    = 0;
    Double_t sumy2w    = 0;
    Double_t sumt2w    = 0;
    Double_t sumxyw    = 0;
    Double_t sumxtw    = 0;
    Double_t sumytw    = 0;

    Double_t sumdx3w   = 0;
    Double_t sumdy3w   = 0;

    Double_t sumdx2dyw = 0;
    Double_t sumdxdy2w = 0;

    const UInt_t npix = evt.GetNumPixels();
    for (UInt_t i=0; i<npix; i++)
    {
        const MSignalPix &pix = evt[i];
        if (!pix.IsPixelUsed())
            continue;

        if (island>=0 && pix.GetIdxIsland()!=island)
            continue;

        const MGeom &gpix = geom[i];

        const Double_t x = gpix.GetX();
        const Double_t y = gpix.GetY();
        const Double_t t = pix.GetArrivalTime();

        Double_t nphot = pix.GetNumPhotons();        // [1]

        // --- time slope ----
        sumx    += x;
        sumy    += y;
        sumt    += t;
        sumw    += nphot;

        sumxt   += x*t;
        sumyt   += y*t;

        sumx2   += x*x;
        sumy2   += y*y;
        sumxy   += x*y;
        sumt2   += t*t;

        sumxw   += x*nphot;
        sumyw   += y*nphot;
        sumtw   += t*nphot;

        sumx2w  += x*x*nphot;
        sumy2w  += y*y*nphot;
        sumt2w  += t*t*nphot;

        sumxyw  += x*y*nphot;

        sumxtw  += x*t*nphot;
        sumytw  += y*t*nphot;

        // --- 3rd moment ---
        const Double_t dx = x - hil.GetMeanX();      // [mm]
        const Double_t dy = y - hil.GetMeanY();      // [mm]

        sumdx3w   += dx*dx*dx*nphot;
        sumdy3w   += dy*dy*dy*nphot;
        sumdx2dyw += dx*dx*dy*nphot;
        sumdxdy2w += dx*dy*dy*nphot;

        cnt++;

        //
        // Now we are working on absolute values of nphot, which
        // must take pixel size into account
        //
        nphot *= geom.GetPixRatio(i);

        // --- max pixel ---
        if (nphot>maxpix)
        {
            maxpix   = nphot;                        // [1]
            maxpixid = i;
        }
    }

    const Double_t c = hil.GetCosDelta();
    const Double_t s = hil.GetSinDelta();

    //
    // Time slope
    //
    const Double_t dxt  =  c*sumxt + s*sumyt;
    const Double_t dyt  = -s*sumxt + c*sumyt;

    const Double_t dx   =  c*sumx  + s*sumy;
    const Double_t dy   = -s*sumx  + c*sumy;

    const Double_t dx2  =  c*c*sumx2 + 2*c*s*sumxy + s*s*sumy2;
    const Double_t dy2  =  s*s*sumx2 - 2*c*s*sumxy + c*c*sumy2;

    const Double_t detx =  cnt*dx2 - dx*dx;
    const Double_t dety =  cnt*dy2 - dy*dy;

    fSlopeLong  = detx==0 ? 0 : (cnt*dxt - sumt*dx)/detx;
    fSlopeTrans = dety==0 ? 0 : (cnt*dyt - sumt*dy)/dety;

    //
    // Time spread
    //
    const Double_t m = fSlopeLong;

    const Double_t z        = dx; //c*sumx   + s*sumy;
    const Double_t zw       =       c*sumxw  + s*sumyw;

    const Double_t zt       = dxt; //c*sumxt  + s*sumyt;
    const Double_t ztw      =        c*sumxtw + s*sumytw;

    const Double_t x2y2     = dx2; //c*c*sumx2  + s*s*sumy2  + 2*c*s*sumxy;
    const Double_t x2y2w    =        c*c*sumx2w + s*s*sumy2w + 2*c*s*sumxyw;

    const Double_t sumdt    = sumt  - m*z;
    const Double_t sumdtw   = sumtw - m*zw;

    const Double_t sumdt2   = sumt2  + m*m*x2y2  - 2*m*zt;
    const Double_t sumdt2w  = sumt2w + m*m*x2y2w - 2*m*ztw;

    const Double_t meant    = sumt  /cnt;
    const Double_t meandt   = sumdt /cnt;
    const Double_t meantw   = sumtw /sumw;
    const Double_t meandtw  = sumdtw/sumw;

    fTimeSpread          = TMath::Sqrt(sumt2 /cnt  - meant *meant);
    fTimeSpreadWeighted  = TMath::Sqrt(sumt2w/sumw - meantw*meantw);

    fSlopeSpread         = TMath::Sqrt(sumdt2 /cnt  - meandt *meandt);
    fSlopeSpreadWeighted = TMath::Sqrt(sumdt2w/sumw - meandtw*meandtw);

    //
    // Third moments along axes get normalized
    //
    const Double_t m3l = c*c*c*sumdx3w + s*s*s*sumdy3w + 3*(s*c*c*sumdx2dyw + c*s*s*sumdxdy2w);
    const Double_t m3t = c*c*c*sumdy3w - s*s*s*sumdx3w + 3*(s*s*c*sumdx2dyw - s*c*c*sumdxdy2w);

    fM3Long  = MMath::Sqrt3(m3l/sumw);      // [mm]
    fM3Trans = MMath::Sqrt3(m3t/sumw);      // [mm]

    //
    // Asymmetry
    //
    const MGeom &maxp = geom[maxpixid];
    fAsym = (hil.GetMeanX()-maxp.GetX())*c + (hil.GetMeanY()-maxp.GetY())*s;            // [mm]

    SetReadyToSave();

    return 0;
}

// --------------------------------------------------------------------------
//
// This function is ment for special usage, please never try to set
// values via this function
//
void MHillasExt::Set(const TArrayF &arr)
{
    if (arr.GetSize() != 9)
        return;

    fAsym       = arr.At(0);
    fM3Long     = arr.At(1);
    fM3Trans    = arr.At(2);
    fSlopeLong  = arr.At(3);
    fSlopeTrans = arr.At(4);

    fTimeSpread          = arr.At(5);
    fTimeSpreadWeighted  = arr.At(6);
    fSlopeSpread         = arr.At(7);
    fSlopeSpreadWeighted = arr.At(8);
}

// -------------------------------------------------------------------------
//
// Print contents of MHillasExt to *fLog.
//
void MHillasExt::Print(Option_t *o) const
{
    const Bool_t showtrans = TString(o).Contains("trans");

    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Asymmetry      [mm]  = " << fAsym    << endl;
    *fLog << " - 3.Moment Long  [mm]  = " << fM3Long  << endl;
    if (showtrans)
        *fLog << " - 3.Moment Trans [mm]  = " << fM3Trans << endl;
    *fLog << " - Slope Long     [mm]  = " << fSlopeLong  << endl;
    if (showtrans)
        *fLog << " - Slope Trans    [mm]  = " << fSlopeTrans << endl;
    *fLog << " - Time Spread    [ns]  = " << fTimeSpread << endl;
    *fLog << " - Slope Spread   [ns]  = " << fSlopeSpread << endl;
    *fLog << " - Time Spread W  [ns]  = " << fTimeSpreadWeighted << endl;
    *fLog << " - Slope Spread W [ns]  = " << fSlopeSpreadWeighted << endl;
}

// -------------------------------------------------------------------------
//
// Print contents of MHillasExt to *fLog, depending on the geometry in
// units of deg.
//
void MHillasExt::Print(const MGeomCam &geom) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Asymmetry      [deg] = " << fAsym   *geom.GetConvMm2Deg() << endl;
    *fLog << " - 3.Moment Long  [deg] = " << fM3Long *geom.GetConvMm2Deg() << endl;
    *fLog << " - 3.Moment Trans [mm]  = " << fM3Trans*geom.GetConvMm2Deg() << endl;
    *fLog << " - Slope Long     [mm]  = " << fSlopeLong*geom.GetConvMm2Deg()  << endl;
    *fLog << " - Slope Trans    [mm]  = " << fSlopeTrans*geom.GetConvMm2Deg() << endl;
    *fLog << " - Slope Spread   [ns]  = " << fSlopeSpread << endl;
    *fLog << " - Time Spread W  [ns]  = " << fTimeSpreadWeighted << endl;
    *fLog << " - Slope Spread W [ns]  = " << fSlopeSpreadWeighted << endl;
}

// -------------------------------------------------------------------------
//
// Paint the 3rd moment on top of the shower. Therefor "MHillas" is needed.
// it is searched via gPad->FindObject. If opt is not IsNull an object with
// the name opt is searched.
//
void MHillasExt::Paint(Option_t *opt)
{
    const TString name(opt);

    const MHillas *hil = dynamic_cast<const MHillas*>(gPad->FindObject(name.IsNull() ? "MHillas" : name.Data()));
    if (!hil)
        return;

    TVector2 v(fM3Long, fM3Trans);
    v  = v.Rotate(hil->GetDelta()+TMath::Pi());
    v += hil->GetMean();

    TMarker m;
    m.SetMarkerColor(kRed);
    m.SetMarkerStyle(kFullDotLarge);
    m.PaintMarker(v.X(), v.Y());
}
