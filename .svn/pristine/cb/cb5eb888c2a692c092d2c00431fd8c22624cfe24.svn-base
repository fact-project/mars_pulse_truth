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
!   Author(s): Wolfgang Wittek 03/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz            <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MNewImagePar
//
// Storage Container for new image parameters
//
//    Float_t fLeakage1;             // (photons in most outer ring of pixels) over fSize
//    Float_t fLeakage2;             // (photons in the 2 outer rings of pixels) over fSize
//
//    Float_t fConc;                 // [ratio] concentration ratio: sum of the two highest pixels / fSize
//    Float_t fConc1;                // [ratio] concentration ratio: sum of the highest pixel / fSize
//    Float_t fConcCOG;              // [ratio] concentration of the three pixels next to COG
//    Float_t fConcCore;             // [ratio] concentration of signals inside or touching the ellipse
//
//    Float_t fUsedArea;             // Area of pixels which survived the image cleaning
//    Float_t fCoreArea;             // Area of core pixels
//    Short_t fNumUsedPixels;        // Number of pixels which survived the image cleaning
//    Short_t fNumCorePixels;        // number of core pixels
//    Short_t fNumHGSaturatedPixels; // number of pixels with saturating hi-gains
//    Short_t fNumSaturatedPixels;   // number of pixels with saturating lo-gains
//
// Version 2:
// ----------
//  - added fNumSaturatedPixels
// 
// Version 3:
// ----------
//  - added fNumHGSaturatedPixels
//  - added fInnerLeakage1
//  - added fInnerLeakage2
//  - added fInnerSize
//  - added fUsedArea
//  - added fCoreArea
//
// Version 4:
// ----------
//  - moved cleaning/island independant parameters to MImagePar:
//    + removed fNumHGSaturatedPixels
//    + removed fNumSaturatedPixels
//
// Version 5:
// ----------
//  - added fConcCOG
//  - added fConcCore
//
// Version 6:
// ----------
//  - removed fInnerLeakage1
//  - removed fInnerLeakage2
//  - removed fInnerSize
//
/////////////////////////////////////////////////////////////////////////////
#include "MNewImagePar.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHillas.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

ClassImp(MNewImagePar);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MNewImagePar::MNewImagePar(const char *name, const char *title)
{
    fName  = name  ? name  : "MNewImagePar";
    fTitle = title ? title : "New image parameters";

    Reset();
}

// --------------------------------------------------------------------------
//
void MNewImagePar::Reset()
{
    fLeakage1 = -1;
    fLeakage2 = -1;

    fConc     = -1;
    fConc1    = -1;
    fConcCOG  = -1;
    fConcCore = -1;

    fNumUsedPixels = -1;
    fNumCorePixels = -1;

    fUsedArea = -1;
    fCoreArea = -1;
}

// --------------------------------------------------------------------------
//
//  Calculation of new image parameters
//
void MNewImagePar::Calc(const MGeomCam &geom, const MSignalCam &evt,
                        const MHillas &hillas, Int_t island)
{
    fNumUsedPixels = 0;
    fNumCorePixels = 0;

    fUsedArea = 0;
    fCoreArea = 0;

    fConcCore  = 0;

    Double_t edgepix1 = 0;
    Double_t edgepix2 = 0;

    Float_t maxpix1 = 0;                                 // [#phot]
    Float_t maxpix2 = 0;                                 // [#phot]

    const Double_t d = FLT_MAX; //geom.GetMaxRadius()*geom.GetMaxRadius();
    Double_t dist[3] = { d, d, d };
    Int_t    idx[3]  = { -1, -1, -1};

    const Double_t rl = 1./(hillas.GetLength()*hillas.GetLength());
    const Double_t rw = 1./(hillas.GetWidth() *hillas.GetWidth());

    UInt_t npix = evt.GetNumPixels();
    for (UInt_t i=0; i<npix; i++)
    {
        const MSignalPix &pix = evt[i];
        if (pix.IsPixelUnmapped())
            continue;

        // Get geometry of pixel
        const MGeom &gpix = geom[i];

        // Find the three pixels which are next to the COG
        const Double_t dx = gpix.GetX() - hillas.GetMeanX();      // [mm]
        const Double_t dy = gpix.GetY() - hillas.GetMeanY();      // [mm]

        const Double_t dist0 = dx*dx+dy*dy;

        if (dist0<dist[0])
        {
            dist[2] = dist[1];
            dist[1] = dist[0];
            dist[0] = dist0;

            idx[2]  = idx[1];
            idx[1]  = idx[0];
            idx[0]  = i;
        }
        else
            if (dist0<dist[1])
            {
                dist[2] = dist[1];
                dist[1] = dist0;

                idx[2]  = idx[1];
                idx[1]  = i;
            }
            else
                if (dist0<dist[2])
                {
                    dist[2] = dist0;
                    idx[2]  = i;
                }

        if (!pix.IsPixelUsed())
            continue;

        // Check for requested islands
        if (island>=0 && pix.GetIdxIsland()!=island)
            continue;

        // count used and core pixels
        if (pix.IsPixelCore())
        {
            fNumCorePixels++;
            fCoreArea += gpix.GetA();
        }

        // count used pixels
        fNumUsedPixels++;
        fUsedArea += gpix.GetA();

        // signal in pixel
        Double_t nphot = pix.GetNumPhotons();

        //
        // Calculate signal contained inside ellipse
        //
        const Double_t dzx   =  hillas.GetCosDelta()*dx + hillas.GetSinDelta()*dy; // [mm]
        const Double_t dzy   = -hillas.GetSinDelta()*dx + hillas.GetCosDelta()*dy; // [mm]
        const Double_t dz    =  gpix.GetT()/2;
        const Double_t distr =  (dzy*dzy+dzx*dzx)/(dzx*dzx*rl + dzy*dzy*rw);
        if ((dzx==0 && dzy==0) || sqrt(distr)>sqrt(dist0)-dz)
            fConcCore += nphot;

        //
        // count photons in outer rings of camera
        //
        if (gpix.IsInOutermostRing())
           edgepix1 += nphot;
        if (gpix.IsInOuterRing())
           edgepix2 += nphot;

        //
        // Now convert nphot from absolute number of photons or phe to signal
        // density (divide by pixel area), to find the pixel with highest signal
        // density:
        //
        nphot *= geom.GetPixRatio(i);

 	// Look for signal density in two highest pixels:
        if (nphot>maxpix1)
        {
            maxpix2  = maxpix1;
            maxpix1  = nphot;                            // [1]
        }
        else
            if (nphot>maxpix2)
                maxpix2 = nphot;                         // [1]
    }

    fLeakage1 = edgepix1 / hillas.GetSize();
    fLeakage2 = edgepix2 / hillas.GetSize();

    // FIXME?: in case the pixel with highest signal density is an outer
    // pixel, the value of fConc (ratio of signal in two highest pixels
    // to SIZE) should rather be 2*fConc1, under the simplest assumption
    // that the light density inside the outer (large) pixel is uniform.
    fConc  = (maxpix1+maxpix2)/hillas.GetSize();         // [ratio]
    fConc1 = maxpix1/hillas.GetSize();                   // [ratio]

    //
    // Concentration around COG (it is calculated here, because the
    // distance of the pixel to COG is calculated anyhow)
    //
    fConcCOG = 0;
    for (UInt_t i=0; i<TMath::Min(3U, npix); i++)
        fConcCOG += idx[i]<0 ? 0 : evt[idx[i]].GetNumPhotons()*geom.GetPixRatio(idx[i]);
    fConcCOG /= hillas.GetSize();                        // [ratio]

    // This can for example happen in case of Muon Rings
    if (fConcCOG<0)
        fConcCOG=0;

    // Concentration of signal contained in ellipse
    fConcCore /= hillas.GetSize();

    SetReadyToSave();
}

// --------------------------------------------------------------------------
//
void MNewImagePar::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Leakage1         [1] = " << fLeakage1      << endl;
    *fLog << " - Leakage2         [1] = " << fLeakage2      << endl;
    *fLog << " - Conc             [1] = " << fConc          << endl;
    *fLog << " - Conc1            [1] = " << fConc1         << endl;
    *fLog << " - ConcCOG          [1] = " << fConcCOG       << endl;
    *fLog << " - ConcCore         [1] = " << fConcCore      << endl;
    *fLog << " - Num Used Pixels  [#] = " << fNumUsedPixels << endl;
    *fLog << " - Num Core Pixels  [#] = " << fNumCorePixels << endl;
    *fLog << " - Used Area      [mm" << UTF8::kSquare << "] = " << fUsedArea      << endl;
    *fLog << " - Core Area      [mm" << UTF8::kSquare << "] = " << fCoreArea      << endl;
}

// -------------------------------------------------------------------------
//
// Print contents of MNewImagePar to *fLog, depending on the geometry in
// units of deg.
//
void MNewImagePar::Print(const MGeomCam &geom) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Leakage1         [1] = " << fLeakage1      << endl;
    *fLog << " - Leakage2         [1] = " << fLeakage2      << endl;
    *fLog << " - Conc             [1] = " << fConc          << endl;
    *fLog << " - Conc1            [1] = " << fConc1         << endl;
    *fLog << " - ConcCOG          [1] = " << fConcCOG       << endl;
    *fLog << " - ConcCore         [1] = " << fConcCore      << endl;
    *fLog << " - Num Used Pixels  [#] = " << fNumUsedPixels << endl;
    *fLog << " - Num Core Pixels  [#] = " << fNumCorePixels << endl;
    *fLog << " - Used Area     [deg" << UTF8::kSquare << "] = " << fUsedArea*geom.GetConvMm2Deg()*geom.GetConvMm2Deg() << endl;
    *fLog << " - Core Area     [deg" << UTF8::kSquare << "] = " << fCoreArea*geom.GetConvMm2Deg()*geom.GetConvMm2Deg() << endl;
}
