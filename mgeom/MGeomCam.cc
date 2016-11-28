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
!   Author(s): Thomas Bretz     12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer 01/2001
!   Author(s): Markus Gaug      03/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCam
//
// This is the base class of different camera geometries. It creates
// a pixel object for a given number of pixels and defines the
// interface of how to acccess the geometry information.
//
// We use a TObjArray for possible future usage (it is much more flexible
// than a TClonesArray so that it can store more types of pixels (eg
// fake pixels which are not really existing)
//
// Version 1:
// ----------
//  - first implementation
//
// Version 2:
// ----------
//  - added fPixRatio
//  - added fPixRatioSqrt
//
// Version 3:
// ----------
//  - added fNumAreas
//  - added fNumSectors
//
// Version 4:
// ----------
//  - added fMaxRadius
//  - added fMinRadius
//
// Version 5:
// ----------
//  - added fNumPixInSector
//  - added fNumPixWithAidx
//  - removed fNumSectors
//  - removed fNumAreas
//
/////////////////////////////////////////////////////////////////////////////
#include "MGeomCam.h"

#include <TMath.h>      // TMath
#include <TClass.h>     // IsA()->New()
#include <TArrayI.h>    // TArrayI
#include <TVector2.h>   // TVector2

#include "MLog.h"
#include "MLogManip.h"

#include "MGeom.h"

ClassImp(MGeomCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. Initializes a Camera Geometry with npix pixels. All
// pixels are deleted when the corresponding array is deleted.
//
MGeomCam::MGeomCam(UInt_t npix, Float_t dist, const char *name, const char *title)
    : fNumPixels(npix), fCamDist(dist), fConvMm2Deg(kRad2Deg/(dist*1000)), fPixels(npix),
    fMaxRadius(1), fMinRadius(1), fPixRatio(npix), fPixRatioSqrt(npix)
{
    fName  = name  ? name  : "MGeomCam";
    fTitle = title ? title : "Storage container for a camera geometry";

    //
    // make sure that the destructor delete all contained objects
    //
    fPixels.SetOwner();
}


// --------------------------------------------------------------------------
//
// To make sure that everything is correctly initialized when
// cloned we call InitGeometry after cloning. This repairs
// a few I/O problems mainly with files written with older
// MARS versions (see also StreamerWorkaround)
//
TObject *MGeomCam::Clone(const char *newname) const
{
    MGeomCam *clone = (MGeomCam*)MParContainer::Clone(newname);
    clone->InitGeometry();
    return clone;
}

// --------------------------------------------------------------------------
//
// Copy function. It does intentionally (see workaround in Streamer)
// not copy name and title.
//
void MGeomCam::Copy(TObject &o) const
{
    MGeomCam &c = (MGeomCam&)o;

    c.fNumPixels      = fNumPixels;
    c.fCamDist        = fCamDist;
    c.fConvMm2Deg     = fConvMm2Deg;

    c.fMaxRadius      = fMaxRadius;
    c.fMinRadius      = fMinRadius;
    c.fPixRatio       = fPixRatio;
    c.fPixRatioSqrt   = fPixRatioSqrt;

    c.fNumPixInSector = fNumPixInSector;
    c.fNumPixWithAidx = fNumPixWithAidx;

    Int_t n = fPixels.GetEntriesFast();
    Int_t m = c.fPixels.GetEntriesFast();

    c.fPixels.Delete();
    c.fPixels.Expand(n);

    for (int i=m; i<n; i++)
        c.fPixels.AddAt(fPixels[i]->Clone(), i);
}

// --------------------------------------------------------------------------
//
// Returns a reference of the i-th entry (the pixel with the software idx i)
// The access is unchecked (for speed reasons!) accesing non existing
// entries may crash the program!
//
MGeom &MGeomCam::operator[](Int_t i)
{
    return *static_cast<MGeom*>(fPixels.UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Returns a reference of the i-th entry (the pixel with the software idx i)
// The access is unchecked (for speed reasons!) accesing non existing
// entries may crash the program!
//
MGeom &MGeomCam::operator[](Int_t i) const
{
    return *static_cast<MGeom*>(fPixels.UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Calculate and fill the arrays storing the ratio of the area of a pixel
// i to the pixel 0 and its square root.
// The precalculation is done for speed reasons. Having an event the
// ratio would be calculated at least once for each pixel which is
// an enormous amount of numerical calculations, which are time
// consuming and which can be avoided doing the precalculation.
//
void MGeomCam::CalcPixRatio()
{
    const Double_t a0 = (*this)[0].GetA();

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        fPixRatio[i] = a0/(*this)[i].GetA();
        fPixRatioSqrt[i] = TMath::Sqrt(fPixRatio[i]);
    }
}

// --------------------------------------------------------------------------
//
//  Set the kIsOuterRing flag for all pixels which have a outermost pixel
//  as Next Neighbor and don't have the kIsOutermostRing flag itself.
//
void MGeomCam::InitOuterRing()
{
    fPixels.R__FOR_EACH(MGeom, CheckOuterRing)(*this);
}

// --------------------------------------------------------------------------
//
// Calculate the highest sector index+1 of all pixels, please make sure
// the the sector numbers are continous.
//
void MGeomCam::CalcNumSectors()
{
    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const Int_t s = (*this)[i].GetSector();

        if (s>=fNumPixInSector.GetSize())
            fNumPixInSector.Set(s+1);

        fNumPixInSector[s]++;
    }
}

// --------------------------------------------------------------------------
//
// Calculate the highest area index+1 of all pixels, please make sure
// the the area indices are continous.
//
void MGeomCam::CalcNumAreas()
{
    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const Int_t s = (*this)[i].GetAidx();

        if (s>=fNumPixWithAidx.GetSize())
            fNumPixWithAidx.Set(s+1);

        fNumPixWithAidx[s]++;
    }
}

// --------------------------------------------------------------------------
//
// Calculate the maximum radius of the camera. This is ment for GUI layout.
//
void MGeomCam::CalcMaxRadius()
{
    fMaxRadius.Set(GetNumAreas()+1);
    fMinRadius.Set(GetNumAreas()+1);

    for (UInt_t i=0; i<GetNumAreas()+1; i++)
    {
        fMaxRadius[i] = 0.;
        fMinRadius[i] = FLT_MAX;
    }

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGeom &pix = (*this)[i];

        const UInt_t  s = pix.GetAidx();

        const Float_t d = pix.GetT();
        const Float_t r = pix.GetDist();

        const Float_t maxr = r + d;
        const Float_t minr = r>d ? r-d : 0;

        if (maxr>fMaxRadius[s+1])
            fMaxRadius[s+1] = maxr;

        if (minr<fMinRadius[s+1])
            fMinRadius[s+1] = minr;

        if (minr<fMinRadius[0])
            fMinRadius[0] = minr;

        if (maxr>fMaxRadius[0])
            fMaxRadius[0] = maxr;
    }
}

// --------------------------------------------------------------------------
//
// sort neighbours from angle of -180 degree to -180 degree
//
// where right side of main pixel contains negative degrees
// and left side positive degrees
//
// angle measured from top (0 degree) to bottom (180 degree)
//             ^  -     |     +         //
//             |      _ | _             //
//             |     / \|/ \            //
//  30 degree -+-   |   |   |           //
//             |   / \ / \ / \          //
//  90 degree -+- |   | X |   |         //
//             |   \ / \ / \ /          //
// 150 degree -+-   |   |   |           //
//             |     \_/|\_/            //
//             |        |               //
//             |  -     |     +         //
//             ------------------>      //
//                                      //
void MGeomCam::SortNeighbors()
{
    for (unsigned int i=0; i<fNumPixels; i++)
    {
        MGeom &gpix = (*this)[i];

        Double_t phi[6];
        Int_t    idx[7] = { 0, 0, 0, 0, 0, 0, -1 };

        const Int_t n2 = gpix.GetNumNeighbors();
        for (int j=0; j<n2; j++)
        {
            idx[j] = gpix.GetNeighbor(j);
            phi[j] = (*this)[idx[j]].GetAngle(gpix);
        }

        Int_t sort[6] = { 6, 6, 6, 6, 6, 6 };

        TMath::Sort(n2, phi, sort, kFALSE);

        gpix.SetNeighbors(idx[sort[0]], idx[sort[1]], idx[sort[2]],
                          idx[sort[3]], idx[sort[4]], idx[sort[5]]);
    }
}

// --------------------------------------------------------------------------
//
// Return the total area of the camera
//
Float_t MGeomCam::GetA() const
{
    Double_t A = 0;
    for (unsigned int i=0; i<fNumPixels; i++)
        A += (*this)[i].GetA();

    return A;
}

// --------------------------------------------------------------------------
//
// Returns the distance between the pixels i and j. -1 if an index
// doesn't exist. The default for j is 0. Assuming that 0 is the index
// for a central pixel you can get the distance to the camera center.
//
Float_t MGeomCam::GetDist(UShort_t i, UShort_t j) const
{
    if (i>=fNumPixels || j>=fNumPixels)
        return -1;

    return (*this)[i].GetDist((*this)[j]);
}

// --------------------------------------------------------------------------
//
// Returns the angle between of pixels i wrt pixel j (default=0). The angle
// is returned in the range between -pi and pi (atan2) and 2*pi if i or j
// is out of range.
//
Float_t MGeomCam::GetAngle(UShort_t i, UShort_t j) const
{
    if (i>=fNumPixels || j>=fNumPixels)
        return TMath::TwoPi();

    return (*this)[i].GetAngle((*this)[j]);
}

// --------------------------------------------------------------------------
//
// The maximum possible distance from the origin.
//
Float_t MGeomCam::GetMaxRadius() const
{
    return fMaxRadius[0];
}

// --------------------------------------------------------------------------
//
// The minimum possible distance from the origin.
//
Float_t MGeomCam::GetMinRadius() const
{
    return fMinRadius[0];
}

// --------------------------------------------------------------------------
//
// Have to call the radii of the subcameras starting to count from 1
//
Float_t MGeomCam::GetMaxRadius(const Int_t i) const
{
    return i<0 || i>=(Int_t)GetNumAreas() ? -1 : fMaxRadius[i+1];
}

// --------------------------------------------------------------------------
//
// Have to call the radii of the subcameras starting to count from 1
//
Float_t MGeomCam::GetMinRadius(const Int_t i) const
{
    return i<0 || i>=(Int_t)GetNumAreas() ? -1 : fMinRadius[i+1];
}

// --------------------------------------------------------------------------
//
//  returns the ratio of the area of the pixel with index 0 to the pixel
//  with the specified index i. 0 Is returned if the index argument is
//  out of range.
//
Float_t MGeomCam::GetPixRatio(UInt_t i) const
{
    // Former: (*this)[0].GetA()/(*this)[i].GetA();
    // The const_cast is necessary to support older root version
    return i<fNumPixels ? const_cast<TArrayF&>(fPixRatio)[i] : 0;
}

// --------------------------------------------------------------------------
//
//  returns the square root of the ratio of the area of the pixel with
//  index 0 to the pixel with the specified index i. 0 Is returned if
//  the index argument is out of range.
//
Float_t MGeomCam::GetPixRatioSqrt(UInt_t i) const
{
    // The const_cast is necessary to support older root version
    return i<fNumPixels ? const_cast<TArrayF&>(fPixRatioSqrt)[i] : 0;
}

// --------------------------------------------------------------------------
//
// Check if the photon which is flying along the trajectory u has passed
// (or will pass) the frame of the camera (and consequently get
// absorbed). The position p and direction u must be in the
// telescope coordinate frame, which is z parallel to the focal plane,
// x to the right and y upwards, looking from the mirror towards the camera.
//
// The units are cm.
//
Bool_t MGeomCam::HitFrame(MQuaternion p, const MQuaternion &u, Double_t margin) const
{
    if (margin<0)
        return HitFrame(p, u);

    // z is defined from the mirror (0) to the camera (z>0).
    // Thus we just propagate to the focal plane (z=fDist)
    p.PropagateZ(u, GetCameraDist()*100); // m->cm
    return p.R()<margin;
}

// --------------------------------------------------------------------------
//
// Check if the position given in the focal plane (so z can be ignored)
// is a position which might hit the detector. It is meant to be a rough
// and fast estimate not a precise calculation. All positions dicarded
// must not hit the detector. All positions accepted might still miss
// the detector.
//
Bool_t MGeomCam::HitDetector(const MQuaternion &v, Double_t offset) const
{
    const Double_t max = fMaxRadius[0]/10+offset; // cm --> mm
    return v.R2()<max*max;
}

// --------------------------------------------------------------------------
//
//  Prints the Geometry information of all pixels in the camera.
//  With the option "simple" you can suppress the output of the contents
//  of the individual pixels.
//
void MGeomCam::Print(Option_t *o) const
{
    //
    //   Print Information about the Geometry of the camera
    //
    *fLog << all << " Number of Pixels (" << GetTitle() << "): " << fNumPixels << endl;
    *fLog << " Number of Sectors: " << GetNumSectors() << "  Area-Indices: " << GetNumAreas() << endl;
    *fLog << " Min.Radius: " << GetMinRadius() << "  Max.Radius: " << GetMaxRadius() << endl;

    if (!TString(o).Contains("simple", TString::kIgnoreCase))
        fPixels.Print();
} 

// --------------------------------------------------------------------------
//
//  Return the pixel index corresponding to the coordinates given in x, y.
//  The coordinates are given in pixel units (millimeters)
//  If no pixel exists return -1;
//
Int_t MGeomCam::GetPixelIdxXY(Float_t x, Float_t y) const
{
    for (unsigned int i=0; i<fNumPixels; i++)
        if ((*this)[i].IsInside(x, y))
            return i;

    return -1;
}

Int_t MGeomCam::GetPixelIdx(const TVector2 &v) const
{
    return GetPixelIdxXY(v.X(), v.Y());
}

Int_t MGeomCam::GetPixelIdxDeg(const TVector2 &v) const
{
    return GetPixelIdxXYdeg(v.X(), v.Y());
}

// --------------------------------------------------------------------------
//
// Add all indices to arr of all neighbors around pix in a radius r.
// The center pixel is also returned.
//
void MGeomCam::GetNeighbors(TArrayI &arr, const MGeom &pix, Float_t r) const
{
    arr.Set(GetNumPixels());

    Int_t n = 0;

    for (unsigned int i=0; i<GetNumPixels(); i++)
    {
        if (r>TMath::Hypot(pix.GetX()-(*this)[i].GetX(), pix.GetY()-(*this)[i].GetY()))
            arr[n++] = i;
    }

    arr.Set(n);
}

// --------------------------------------------------------------------------
//
// Add all indices to arr of all neighbors around idx in a radius r.
// The center pixel is also returned.
//
void MGeomCam::GetNeighbors(TArrayI &arr, UInt_t idx, Float_t r) const
{
    if (idx>=GetNumPixels())
    {
        arr.Set(0);
        return;
    }

    const MGeom &pix = (*this)[idx];
    GetNeighbors(arr, pix, r);
}

// --------------------------------------------------------------------------
//
// Add all pixels to list of all neighbors around pix in a radius r.
// The center pixel is also returned.
//
void MGeomCam::GetNeighbors(TList &arr, const MGeom &pix, Float_t r) const
{
    for (unsigned int i=0; i<GetNumPixels(); i++)
    {
        if (r>TMath::Hypot(pix.GetX()-(*this)[i].GetX(), pix.GetY()-(*this)[i].GetY()))
            arr.Add(fPixels.UncheckedAt(i));
    }
}

// --------------------------------------------------------------------------
//
// Add all pixels to list of all neighbors around idx in a radius r.
// The center pixel is also returned.
//
void MGeomCam::GetNeighbors(TList &arr, UInt_t idx, Float_t r) const
{
    if (idx>=GetNumPixels())
        return;

    const MGeom &pix = (*this)[idx];
    GetNeighbors(arr, pix, r);
}

// --------------------------------------------------------------------------
//
// Return direction of p2 w.r.t. p1. For more details
// see MGeom::GetDirection
//
Int_t MGeomCam::GetDirection(UInt_t p1, UInt_t p2) const
{
    if (p1>fNumPixels || p2>fNumPixels)
        return -1;

    return operator[](p1).GetDirection(operator[](p2));
}

// --------------------------------------------------------------------------
//
// Get index of neighbor of pixel idx in direction dir, if existing.
//
Int_t MGeomCam::GetNeighbor(UInt_t idx, Int_t dir) const
{
    if (idx>fNumPixels)
        return -1;

    const MGeom &pix=operator[](idx);

    //
    // search for the neighbor in the given direction
    //
    for (int i=0; i<pix.GetNumNeighbors(); i++)
        if (GetDirection(idx, pix.GetNeighbor(i))==dir)
            return pix.GetNeighbor(i);

    return -1;
}

// --------------------------------------------------------------------------
//
//  This fills the next neighbor information from a table into the pixel
//  objects. An overloaded function must call SortNeighbors()
//  at the end.
//
// All pixels with a center withing 1.75*GetT() are considered neighbors,
// Only six neighbors are allowed.
//
void MGeomCam::CreateNN()
{
    TArrayI nn(6);

    for (UInt_t i=0; i<GetNumPixels(); i++)
    {
        MGeom &pix = (*this)[i];

        Int_t k = 0;
        nn.Reset(-1);

        for (UInt_t j=0; j<GetNumPixels(); j++)
        {
            if (i==j)
                continue;

            if (pix.GetDist((*this)[j])>pix.GetT()*1.75)
                continue;

            if (k==6)
            {
                *fLog << err << "ERROR - MGeomCam::CreateNN: Pixel " << j << " has too many neighbors." << endl;
                break;
            }

            nn[k++] = j;
        }

        pix.SetNeighbors(nn[0], nn[1], nn[2], nn[3], nn[4], nn[5]);
    }

    SortNeighbors();
}

// --------------------------------------------------------------------------
//
// This workaround reproduces (as much as possible) the contents
// of fPixels which got lost writing MGeomCam in which the
// fPixels were filles with the []-operator instead of AddAt
// and a root version previous to 5.18.
// You try to read broken contents from file if fNumPixels is empty
// but fNumPixels>0.
// If you ever read broken contents from a split branch you
// MUST call this function after reading.
//
// Furthermore since we moved all contents of MGeomPix to its
// new base class the contents of fNeighbors is not read anymore
// although all other contents is read correctly. Therefore we have
// top recreate the neighbor table.
//
void MGeomCam::StreamerWorkaround()
{
    if (fNumPixels==0)
        return;


    if (!fPixels.IsEmpty())
    {
        CreateNN();
        return;
    }

    const TObject *cam = (TObject*)IsA()->New();
    cam->Copy(*this);
    delete cam;
}

// --------------------------------------------------------------------------
//
// This is a custom made streamer. Due to a bug in TObjArray::operator[]
// old root-versions didn't correctly store the contents of the TObjArray.
// If such a file is detected (TObjArray empty) a new MGeomCam is created
// with IsA()->New() and its contents is copied to this. Unfortunately
// this won't work for all MGeomCam derivatives which need arguments
// in the constructor and MGeomCam itself (no derivative). Fortunately
// in prodoction we have never stored anything else than MGeomCamMagic yet.
// The bug in root can be worked around using AddAt instead of operator[].
//
void MGeomCam::Streamer(TBuffer &b)
{
    if (b.IsReading())
    {
        MGeomCam::Class()->ReadBuffer(b, this);
        StreamerWorkaround();
    }
    else
        MGeomCam::Class()->WriteBuffer(b, this);
}

// --------------------------------------------------------------------------
//
// This deletes the pixel i and sets it to a clone of pix.
// If i>=fNumPixels it is just ignored.
//
void MGeomCam::SetAt(UInt_t i, const MGeom &pix)
{
    if (i>=fNumPixels)
        return;

    if (fPixels[i])
        delete fPixels.RemoveAt(i);

    // For root versions <5.18 AddAt is mandatory, for newer
    // root-version the []-operator can be used safely
    fPixels.AddAt(pix.Clone(), i);
}
