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
! * in supporting documentation. It is provided "as is" without expressed
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Robert Wagner, 7/2004 <mailto:rwagner@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MAstroCamera
// ============
//
// A tools displaying stars from a catalog in the camera display.
// PRELIMINARY!!
//
//
// Usage:
// ------
// For a usage example see macros/starfield.C
//
// To be able to reflect the star-light you need the geometry of the
// mirror and the distance of the plain screen.
//
// You can get the mirror geometry from a MC file and the distance of
// the screen from MGeomCam.
//
//
// Algorithm:
// ----------
// The calculation of the position of the reflection in the camera is
// done by:
//   - Rotation of the star-field such that the camera is looking into
//     the pointing direction
//   - Calculation of the reflected star-light vector by calling
//     MGeomMirror::GetReflection (which returns the point at which
//     the vector hits the camera plain)
//   - Depending on the draw-option you get each reflected point, the
//     reflection on a virtual ideal mirror or the reflection on each
//     individual mirror
//
//
// GUI:
// ----
//  * You can use the the cursor keys to change the pointing position
//    and plus/minus to change the time by a quarter of an hour.
//
// ToDo:
// -----
//  * possibility to overwrite distance from mirror to screen
//  * read the mirror geometry directly from the MC input file
//
/////////////////////////////////////////////////////////////////////////////
#include "MAstroCamera.h"

#include <errno.h>        // strerror
#include <fstream>        // ifstream

#include <KeySymbols.h>   // kKey_*

#include <TH2.h>          // TH2D
#include <TMarker.h>      // TMarker
#include <TVirtualPad.h>  // gPad

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomMirror.h"

#include "MString.h"

#include "MTime.h"
#include "MAstroSky2Local.h"
#include "MObservatory.h"

#include "MHCamera.h" // FIXME: This dependancy is very bad!
                      // HOW TO GET RID OF IT? Move MHCamera to mgeom?

//#include "MStarPos.h"

ClassImp(MAstroCamera);

using namespace std;

// --------------------------------------------------------------------------
//
// Create a virtual MGeomMirror which is in the center of the coordinate
// system and has a normal vector in z-direction.
//
MAstroCamera::MAstroCamera() : fGeom(0), fMirrors(0)
{
    fMirror0 = new MGeomMirror;
    fMirror0->SetMirrorContent(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
}

// --------------------------------------------------------------------------
//
// Delete fGeom, fMirrors and the virtual 0-Mirror fMirror0
//
MAstroCamera::~MAstroCamera()
{
    if (fGeom)
        delete fGeom;
    if (fMirrors)
        delete fMirrors;

    delete fMirror0;

}

// --------------------------------------------------------------------------
//
// Set a list of mirrors. The Mirrors must be of type MGeomMirror and
// stored in a TClonesArray
//
void MAstroCamera::SetMirrors(TClonesArray &arr)
{
    if (arr.GetClass()!=MGeomMirror::Class())
    {
      cout << "ERROR - TClonesArray doesn't contain objects of type MGeomMirror... ignored." << endl;
      return;
    }

    const Int_t n = arr.GetSize();

    if (!fMirrors)
        fMirrors = new TClonesArray(MGeomMirror::Class(), n);

    fMirrors->ExpandCreate(n);

    for (int i=0; i<n; i++)
        memcpy((*fMirrors)[i], arr[i], sizeof(MGeomMirror));

}


// --------------------------------------------------------------------------
//
// Read the mirror geometry from a MC .def file. The following
// structure is expected:
//
// #*  TYPE=1  (MAGIC)
// #*      i  f   sx   sy   x   y   z   thetan  phin 
// #* 
// #*       i : number of the mirror
// #*       f : focal distance of that mirror
// #*      sx : curvilinear coordinate of mirror's center in X[cm]
// #*      sy : curvilinear coordinate of mirror's center in X[cm]
// #*       x : x coordinate of the center of the mirror [cm]
// #*       y : y coordinate of the center of the mirror [cm]
// #*       z : z coordinate of the center of the mirror [cm]
// #*  thetan : polar theta angle of the direction where the mirror points to
// #*    phin : polar phi angle of the direction where the mirror points to
// #*      xn : xn coordinate of the normal vector in the center (normalized)
// #*      yn : yn coordinate of the normal vector in the center (normalized)
// #*      zn : zn coordinate of the normal vector in the center (normalized)
// #
// define_mirrors
//   1 1700.9200   25.0002   75.0061   25.0000   75.0000    0.9207 0.02328894 1.24904577 -0.00736394 -0.02209183 0.99972882
//   2 ...
//
void MAstroCamera::SetMirrors(const char *fname)
{
    ifstream fin(fname);
    if (!fin)
    {
        gLog << err << "Cannot open file " << fname << ": ";
        gLog << strerror(errno) << endl;
        return;
    }

    TString line;
    while (1)
    {
        line.ReadLine(fin);
        if (!fin)
            return;

        line = line.Strip(TString::kBoth);

        if (line.BeginsWith("n_mirrors"))
        {
            Int_t n;
            sscanf(line.Data(), "%*s %d", &n);

            if (!fMirrors)
                fMirrors = new TClonesArray(MGeomMirror::Class(), n);

            fMirrors->ExpandCreate(n);
            continue;
        }


        Int_t id;
        Float_t f, sx, sy, x, y, z, thetan, phin, xn, yn, zn;

        const Int_t n = sscanf(line.Data(), "%d %f %f %f %f %f %f %f %f %f %f %f",
                               &id, &f, &sx, &sy, &x, &y, &z, &thetan,
                               &phin, &xn, &yn, &zn);
        if (n!=12)
            continue;

        new ((*fMirrors)[id-1]) MGeomMirror;
        ((MGeomMirror*)(*fMirrors)[id-1])->SetMirrorContent(id, f, sx, sy, x, y, z, thetan, phin, xn, yn, zn);
    }
}

// --------------------------------------------------------------------------
//
// Set the camera geometry. The MGeomCam object is cloned.
//
void MAstroCamera::SetGeom(const MGeomCam &cam)
{
    if (fGeom)
        delete fGeom;

    fGeom = (MGeomCam*)cam.Clone();
}

// --------------------------------------------------------------------------
//
// Convert To Pad coordinates (see MAstroCatalog)
//
Int_t MAstroCamera::ConvertToPad(const TVector3 &w, TVector2 &v) const
{
    /*
     --- Use this to plot the 'mean grid' instead of the grid of a
         theoretical central mirror ---

        TVector3 spot;
        const Int_t num = fConfig->GetNumMirror();
        for (int i=0; i<num; i++)
        spot += fConfig->GetMirror(i).GetReflection(w, fGeom->GetCameraDist())*1000;
        spot *= 1./num;
        */

    const TVector3 spot = fMirror0->GetReflection(w, fGeom->GetCameraDist())*1000;
    v.Set(spot(0), spot(1));

    const Float_t max = fGeom->GetMaxRadius()*0.70;
    return v.X()>-max && v.Y()>-max && v.X()<max && v.Y()<max;
}

// --------------------------------------------------------------------------
//
// Find an object with a given name in the list of primitives of this pad.
//
TObject *FindObjectInPad(const char *name, TVirtualPad *pad)
{
    if (!pad)
        pad = gPad;

    if (!pad)
        return NULL;

    TObject *o;

    TIter Next(pad->GetListOfPrimitives());
    while ((o=Next()))
    {
        if (o->InheritsFrom(gROOT->GetClass(name)))
            return o;

        if (o->InheritsFrom("TPad"))
            if ((o = FindObjectInPad(name, (TVirtualPad*)o)))
                return o;
    }
    return NULL;
}

// --------------------------------------------------------------------------
//
// Options:
//
//  '*' Draw the mean of the reflections on all mirrors
//  '.' Draw a dot for the reflection on each individual mirror
//  'h' To create a TH2D of the star-light which is displayed
//  'c' Use the underlaying MHCamera as histogram
//  '0' Draw the reflection on a virtual perfect mirror
//  '=' Draw '0' or '*' propotional to the star magnitude
//
// If the Pad contains an object MHCamera of type MHCamera it is used.
// Otherwise a new object is created.
//
// FIXME: Add Project functionand change treating MHCamera and
//        TH2D accordingly
//
void MAstroCamera::AddPrimitives(TString o)
{
    if (!fTime || !fObservatory || !fMirrors)
    {
      cout << "Missing data..." << endl;
        return;
    }

    if (o.IsNull())
        o = "*.";

    const Bool_t hasnull = o.Contains("0", TString::kIgnoreCase);
    const Bool_t hashist = o.Contains("h", TString::kIgnoreCase);
    const Bool_t hasmean = o.Contains("*", TString::kIgnoreCase);
    const Bool_t hasdot  = o.Contains(".", TString::kIgnoreCase);
    const Bool_t usecam  = o.Contains("c", TString::kIgnoreCase);
    const Bool_t resize  = o.Contains("=", TString::kIgnoreCase);

    // Get camera
    MHCamera *camera=(MHCamera*)FindObjectInPad("MHCamera", gPad);
    if (camera)
    {
        if (!camera->GetGeometry() || camera->GetGeometry()->IsA()!=fGeom->IsA())
            camera->SetGeometry(*fGeom);
    }
    else
    {
        camera = new MHCamera(*fGeom);
        camera->SetName("MHCamera");
        camera->SetStats(0);
        camera->SetInvDeepBlueSeaPalette();
        camera->SetBit(kCanDelete);
        camera->Draw();
    }

    camera->SetTitle(GetPadTitle());

    gPad->cd(1);

    if (!usecam)
    {
        if (camera->GetEntries()==0)
            camera->SetBit(MHCamera::kNoLegend);
    }
    else
    {
        camera->Reset();
        camera->SetYTitle("arb.cur");
    }

    TH2 *h=0;
    if (hashist)
    {
        TH2F hist("","", 90, -650, 650, 90, -650, 650);
        hist.SetMinimum(0);
        h = (TH2*)hist.DrawCopy("samecont1");
    }

    const TRotation rot(GetGrid(kTRUE));

    MVector3 *radec;
    TIter Next(&fList);

    while ((radec=(MVector3*)Next()))
    {
        const Double_t mag = radec->Magnitude();
        if (mag>GetLimMag())
            continue;

        TVector3 star(*radec);

        // Rotate Star into telescope system
        star *= rot;

        TVector3 mean;

        Int_t num = 0;

        MGeomMirror *mirror = 0;
        TIter NextM(fMirrors);
        while ((mirror=(MGeomMirror*)NextM()))
        {
  	    const TVector3 spot = mirror->GetReflection(star, fGeom->GetCameraDist())*1000;
            
            // calculate mean of all 'stars' hitting the camera plane
            // by taking the sum of the intersection points between
            // the light vector and the camera plane
            mean += spot;

            if (hasdot)
            {
                TMarker *m=new TMarker(spot(0), spot(1), 1);
                m->SetMarkerColor((Int_t)(6*mirror->GetMirrorCenter().Mag())+51);
                m->SetMarkerStyle(kDot);
                fMapG.Add(m);
            }
            if (h)
                h->Fill(spot(0), spot(1), pow(10, -mag/2.5));

            if (usecam)
                camera->Fill(spot(0), spot(1), pow(10, -mag/2.5));

            num++;
        }

        // transform meters into millimeters (camera display works with mm)
        mean *= 1./num;
        DrawStar(mean(0), mean(1), *radec, hasmean?kBlack:-1, MString::Format("x=%.1fmm y=%.1fmm", mean(0), mean(1)), resize);
        if (hasnull)
        {
            TVector3 vstar(*radec);
            vstar *= rot;
            const TVector3 spot = fMirror0->GetReflection(vstar, fGeom->GetCameraDist())*1000;
            DrawStar(spot(0), spot(1), *radec, hasmean?kBlack:-1, MString::Format("x=%.1fmm y=%.1fmm", mean(0), mean(1)), resize);
            // This can be used to get the abberation...
            //cout << TMath::Hypot(spot(0), spot(1)) << " " << TMath::Hypot(mean(0)-spot(0), mean(1)-spot(1)) << endl;
        }
    }
}


// --------------------------------------------------------------------------
//
// Fills a TList with all stars found under current presets
// (Coordinates, Time, FOV). The list is emptied before the filling is done.
// Currently, the mean spot when averaging over all mirrors is used.
//
/*
void MAstroCamera::FillStarList(TList* list)
{
    if (!fTime || !fObservatory || !fMirrors) {
      cout << "Missing data..." << endl;
      return;
    }

    if (!list) {
      cout << "Missing storage list for stars found..." << endl;
      return;
    }

    list->Delete(); // dump old list... (otherwise the same stars would pile up)
   
    const TRotation rot(GetGrid(kTRUE));
    MVector3 *radec;
    TIter Next(&fList);

    while ((radec=(MVector3*)Next())) {
      const Double_t mag = radec->Magnitude();
      if (mag>GetLimMag())
	continue;
      TVector3 star(*radec);
      // Rotate Star into telescope system
      star *= rot;
      TVector3 mean;
      Int_t num = 0;
      MGeomMirror *mirror = 0;
      TIter NextM(fMirrors);
      while ((mirror=(MGeomMirror*)NextM())) {
	const TVector3 spot = mirror->GetReflection(star, fGeom->GetCameraDist())*1000;
	mean += spot;
	num++;
      }
      mean *= 1./num;

      MStarPos *starpos = new MStarPos;
      starpos->SetExpValues(mag,mean(0),mean(1));

      // Do _not_ remove this, name needs to be 
      // transferred to the starpos.
      const TString name = radec->GetName();
      starpos->SetName(name.IsNull() ? "n/a" : name.Data());

      const TVector3 spot = fMirror0->GetReflection(star, fGeom->GetCameraDist())*1000;
      starpos->SetIdealValues(mag,spot(0),spot(1));
      list->Add(starpos);
    }
}
*/

// ------------------------------------------------------------------------
//
// Uses fRaDec as a reference point.
//
// Return dZd and dAz corresponding to the distance from x,y to fRaDec
//
// Before calling this function you should correct for abberation. In
// case of MAGIC you can do this by:
//    x /= 1.0713;
//    y /= 1.0713;
//
// x [mm]: x coordinate in the camera plane (assuming a perfect mirror)
// y [mm]: y coordinate in the camera plane (assuming a perfect mirror)
//
// We assume (0, 0) to be the center of the FOV
//
// dzd [deg]: Delta Zd
// daz [deg]: Delta Az
//
void MAstroCamera::GetDiffZdAz(Double_t x, Double_t y, Double_t &dzd, Double_t &daz)
{
    // Reflect the corrected pixel on a perfect mirror
    TVector3 v(x, y, fGeom->GetCameraDist()*1000);
    TVector3 spot = fMirror0->GetReflection(v);
    
    // Derotate into the local coordinate system
    const MAstroSky2Local rot(*fTime, *fObservatory);
    const TRotation align(AlignCoordinates(rot*fRaDec).Inverse());
    spot *= align;

    cout << "Zd="<<spot.Theta()*TMath::RadToDeg() << " ";
    cout << "Az="<<spot.Phi()  *TMath::RadToDeg()+360 << endl;

    // Derotatet the center of the camera
    TVector3 c(0, 0, 1);
    c *= align;

    dzd = (spot.Theta()-c.Theta())*TMath::RadToDeg();
    daz = (spot.Phi()  -c.Phi())  *TMath::RadToDeg();

    if (daz> 180) daz -= 360;
    if (daz<-180) daz += 360;
}

// ------------------------------------------------------------------------
//
// Execute a gui event on the camera
//
void MAstroCamera::ExecuteEvent(Int_t event, Int_t mp1, Int_t mp2)
{
    // if (mp1>0 && mp2>0)
    // {
    //     // Calculate World coordinates from pixel
    //     Double_t x = gPad->AbsPixeltoX(mp1);
    //     Double_t y = gPad->AbsPixeltoY(mp2);
    //
    //     // Correct for abberation
    //     x /= 1.0713;
    //     y /= 1.0713;
    //
    //     Double_t dzd, daz;
    //     GetDiffZdAz(x, y, dzd, daz);
    //
    //     cout << "dZd="<< dzd << " " << "dAz="<< daz << endl;
    // }
    //
    // For MAGIC the distance of the mean of the light distribution
    // to the Mirror0 reflection of the star (Abberation) can be
    // expressed as:  dr = 0.0713*r = r/14.03
    //                   +-0.0002

    if (event==kKeyPress && fTime)
        switch (mp2)
        {
        case kKey_Plus:
            fTime->SetMjd(fTime->GetMjd()+0.25/24);
            Update(kTRUE);
            return;

        case kKey_Minus:
            fTime->SetMjd(fTime->GetMjd()-0.25/24);
            Update(kTRUE);
            return;
        }

    MAstroCatalog::ExecuteEvent(event, mp1, mp2);
}
