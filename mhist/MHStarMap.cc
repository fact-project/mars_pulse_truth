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
!   Author(s): Wolfgang Wittek 02/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHStarMap
//
// This class contains a 2-dimensional histogram. It should show some
// kind of star map. The algorith which calculates the star map
// from the Hillas parameters (Fill) can be enhanced.
//
// For a given a shower, a series of points along its main axis are filled 
// into the 2-dim histogram (x, y) of the camera plane.
//
// Before filling a point (x, y) into the histogram it is
//        - shifted by (xSrc, ySrc)   (the expected source position)
//        - and rotated in order to compensate the rotation of the
//          sky image in the camera
//
// To be done:
//  - simplification of the algorithm, by doing all translation before
//    calculation of the line
//  - the center of rotation need not to be the camera center
//
/////////////////////////////////////////////////////////////////////////////
#include "MHStarMap.h"

#include <TH2.h>
#include <TStyle.h>   // gStyle
#include <TColor.h>   // SetRGB
#include <TCanvas.h>  // TCanvas
#include <TVector2.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MAstro.h"
#include "MGeomCam.h"
#include "MHillas.h"
#include "MBinning.h"
#include "MSrcPosCam.h"
#include "MObservatory.h"
#include "MPointingPos.h"

ClassImp(MHStarMap);

using namespace std;

// --------------------------------------------------------------------------
//
// Create the star map histogram
//
MHStarMap::MHStarMap(const char *name, const char *title)
    : fMm2Deg(-1), fUseMmScale(kTRUE)
{
    //
    //  default constructor
    //  creates an a list of histograms for all pixels and both gain channels
    //

    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHStarMap" ;
    fTitle = title ? title : "Container for a Star Map" ;

    //
    //   loop over all Pixels and create two histograms
    //   one for the Low and one for the High gain
    //   connect all the histogram with the container fHist
    //
    fStarMap = new TH2F("StarMap", " 2D Hillas Star Map ",
                        150, -300, 300,
                        150, -300, 300);

    fStarMap->SetDirectory(NULL);

    fStarMap->SetXTitle("x [mm]");
    fStarMap->SetYTitle("y [mm]");
    fStarMap->SetZTitle("Counts");
}

// --------------------------------------------------------------------------
//
// delete the histogram instance
//
MHStarMap::~MHStarMap()
{
    delete fStarMap;
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning (with the names 'BinningCamera')
// are found in the parameter list
// Use this function if you want to set the conversion factor which
// is used to convert the mm-scale in the camera plain into the deg-scale
// used for histogram presentations. The conversion factor is part of
// the camera geometry. Please create a corresponding MGeomCam container.
//
Bool_t MHStarMap::SetupFill(const MParList *plist)
{
    const MGeomCam *geom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (geom)
    {
        fMm2Deg = geom->GetConvMm2Deg();
        fUseMmScale = kFALSE;

        fStarMap->SetXTitle("x [\\circ]");
        fStarMap->SetYTitle("y [\\circ]");
    }

    fPointPos = (MPointingPos*)plist->FindObject("MPointingPos");
    if (!fPointPos)
        *fLog << warn << "MPointingPos not found... no derotation." << endl;

    fSrcPos = (MSrcPosCam*)plist->FindObject(AddSerialNumber("MSrcPosCam"));
    if (!fSrcPos)
        *fLog << warn << "MSrcPosCam not found...  no source translation." << endl;


    const MBinning *bins = (MBinning*)plist->FindObject("BinningStarMap");
    if (!bins)
    {
        float r = geom ? geom->GetMaxRadius() : 600;
        r *= 5./6;
        if (!fUseMmScale)
            r *= fMm2Deg;

        const MBinning b(100, -r, r);
        SetBinning(*fStarMap, b, b);
    }
    else
        SetBinning(*fStarMap, *bins, *bins);

    if (!geom)
    {
        *fLog << warn << "No Camera Geometry available. Using mm-scale for histograms." << endl;
        return kTRUE;
    }

    *fLog << warn << "WARNING - Using MHStarMap doesn't take care of the Source Position!" << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Get the observatory location "MObservatory" from parameter list
// 
Bool_t MHStarMap::ReInit(MParList *pList)
{
    fObservatory = (MObservatory*)pList->FindObject(AddSerialNumber("MObservatory"));
    if (!fObservatory)
        *fLog << err << "MObservatory not found... no derotation." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the four histograms with data from a MHillas-Container.
// Be careful: Only call this with an object of type MHillas
//
Int_t MHStarMap::Fill(const MParContainer *par, const Stat_t w)
{
    const MHillas &h = *(MHillas*)par;

    const Float_t delta = h.GetDelta();

    const Float_t m    = TMath::Tan(delta);

    const Float_t cosd = 1.0/TMath::Sqrt(m*m+1);
    const Float_t sind = TMath::Sqrt(1-cosd*cosd);

    Float_t t = h.GetMeanY() - m*h.GetMeanX();

    TVector2 src;
    if (fSrcPos)
        src = fSrcPos->GetXY();

    if (!fUseMmScale)
    {
        t   *= fMm2Deg;
        src *= fMm2Deg;
    }

    // get step size ds along the main axis of the ellipse
    const TAxis &axex = *fStarMap->GetXaxis();
    const Float_t xmin = axex.GetXmin();
    const Float_t xmax = axex.GetXmax();

    // FIXME: Fixed number?
    const Float_t ds = (xmax-xmin) / 200.0;

    // Fill points along the main axis of the shower into the histogram;
    // before filling 
    //   - perform a translation by (xSource, ySource)
    //   - and perform a rotation to compensate the rotation of the
    //     sky image in the camera
    const Double_t rho = fPointPos && fObservatory ?
        fPointPos->RotationAngle(*fObservatory) : 0;

    if (m>-1 && m<1)
    {
        const Float_t dx = ds * cosd;

        for (float x=xmin+dx/2; x<(xmax-xmin)+dx; x+=dx)
        {
            TVector2 v(x, m*x+t);
            v -= src;
            v=v.Rotate(-rho);

            fStarMap->Fill(v.X(), v.Y(), w);
        }
    }
    else
    {
        const TAxis &axey = *fStarMap->GetYaxis();
        const Float_t ymin = axey.GetXmin();
        const Float_t ymax = axey.GetXmax();

        const float dy = ds * sind;

        for (float y=ymin+dy/2; y<(ymax-ymin)+dy; y+=dy)
        {
            TVector2 v((y-t)/m, y);
            v -= src;
            v=v.Rotate(-rho);

            fStarMap->Fill(v.X(), v.Y(), w);
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Use this function to setup your own conversion factor between degrees
// and millimeters. The conversion factor should be the one calculated in
// MGeomCam. Use this function with Caution: You could create wrong values
// by setting up your own scale factor.
//
void MHStarMap::SetMm2Deg(Float_t mmdeg)
{
    if (mmdeg<0)
    {
        *fLog << warn << dbginf << "Warning - Conversion factor < 0 - nonsense. Ignored." << endl;
        return;
    }

    if (fMm2Deg>=0)
        *fLog << warn << dbginf << "Warning - Conversion factor already set. Overwriting" << endl;

    fMm2Deg = mmdeg;
}

// --------------------------------------------------------------------------
//
// With this function you can convert the histogram ('on the fly') between
// degrees and millimeters.
//
void MHStarMap::SetMmScale(Bool_t mmscale)
{
    if (fUseMmScale == mmscale)
        return;

    if (fMm2Deg<0)
    {
        *fLog << warn << dbginf << "Warning - Sorry, no conversion factor for conversion available." << endl;
        return;
    }

    if (fUseMmScale)
    {
        fStarMap->SetXTitle("x [mm]");
        fStarMap->SetYTitle("y [mm]");

        fStarMap->Scale(1./fMm2Deg);
    }
    else
    {
        fStarMap->SetXTitle("x [\\circ]");
        fStarMap->SetYTitle("y [\\circ]");

        fStarMap->Scale(1./fMm2Deg);
    }

    fUseMmScale = mmscale;
}

// --------------------------------------------------------------------------
//
// Set the palette you wanna use:
//  - you could set the root "Pretty Palette Violet->Red" by
//    gStyle->SetPalette(1, 0), but in some cases this may look
//    confusing
//  - The maximum colors root allowes us to set by ourself
//    is 50 (idx: 51-100). This colors are set to a grayscaled
//    palette
//  - the number of contours must be two less than the number
//    of palette entries
//
void MHStarMap::PrepareDrawing() const
{
    const Int_t numg = 32; // number of gray scaled colors
    const Int_t numw = 32; // number of white

    Int_t palette[numg+numw];

    //
    // The first half of the colors are white.
    // This is some kind of optical background supression
    //
    gROOT->GetColor(51)->SetRGB(1, 1, 1);

    Int_t i;
    for (i=0; i<numw; i++)
        palette[i] = 51;

    //
    // now the (gray) scaled part is coming
    //
    for (;i<numw+numg; i++)
    {
        const Float_t gray = 1.0-(float)(i-numw)/(numg-1.0);

        gROOT->GetColor(52+i)->SetRGB(gray, gray, gray);
        palette[i] = 52+i;
    }

    //
    // Set the palette and the number of contour levels
    //
    gStyle->SetPalette(numg+numw, palette);
    fStarMap->SetContour(numg+numw-2);
}

// --------------------------------------------------------------------------
//
// Draw clones of the  histograms, so that the object can be deleted
// and the histogram is still visible in the canvas.
// The cloned object is deleted together with the canvas if the canvas is
// destroyed. If you want to handle destroying the canvas you can get a
// pointer to it from this function
//
TObject *MHStarMap::DrawClone(Option_t *opt) const
{
    return MH::DrawClone(opt, 500, 500);
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draw the histogram into it.
// Be careful: The histogram belongs to this object and won't get deleted
// together with the canvas.
//
void MHStarMap::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this, 500, 500);
    pad->SetBorderMode(0);

    pad->Divide(1,1);

    pad->cd(1);
    gPad->SetBorderMode(0);

    AppendPad("");
}

void MHStarMap::Paint(Option_t *opt)
{
    //
    // Maintain aspect ratio
    //
    const float w = gPad->GetWw();
    const float h = gPad->GetWh();

    if (h<w)
        gPad->SetPad((1.-h/w)/2, 0, (h/w+1)/2, 1);
    else
        gPad->SetPad(0, (1.-w/h)/2, 1, (w/h+1)/2);

    //
    // Maintain colors
    //
    PrepareDrawing();

    //
    // Paint Histogram
    //
    fStarMap->Paint("colz");
}
