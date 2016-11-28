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
!   Author(s): Thomas Bretz, 5/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRflEvtData
//
// All Photons of a event from the reflector program
//
// Should be filled like this:
//   MRflEvtData evt;
//   evt.Reset();
//   for (int i=0; i<10; i++)
//      MRflSinglePhoton &ph = evt.GetNewPhoton();
//   evt.FixSize();
//
/////////////////////////////////////////////////////////////////////////////
#include "MRflEvtData.h"

#include <TMarker.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MRflSinglePhoton.h"

ClassImp(MRflEvtData);

using namespace std;

// --------------------------------------------------------------------------
//
// Creates a MCerPhotPix object for each pixel in the event
//
MRflEvtData::MRflEvtData(const char *name, const char *title)
   : fList("MRflSinglePhoton", 0), fPos(0)
{
    fName  = name  ? name  : "MRflEvtData";
    fTitle = title ? title : "All Photons from a reflector event";
}

// --------------------------------------------------------------------------
//
// Copy constructor which copies the contents of dat into the new object
//
MRflEvtData::MRflEvtData(const MRflEvtData &dat)
   : fList("MRflSinglePhoton", 0), fPos(0)
{
    MRflSinglePhoton *ph=NULL;

    TIter Next(&dat.fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        new (fList[fPos++]) MRflSinglePhoton(*ph);
}

// --------------------------------------------------------------------------
//
// Clone function producing a clone of this. A pointer to the clone is
// returned.
//
TObject *MRflEvtData::Clone(Option_t *) const
{
    return new MRflEvtData(*this);
}

// --------------------------------------------------------------------------
//
// Return the i-th MRflSinglePhoton. Be carefull the result is not
// range checked!
//
const MRflSinglePhoton &MRflEvtData::GetPhoton(Int_t i) const
{
    return *static_cast<MRflSinglePhoton*>(fList.UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Dump informations off all photons
//
void MRflEvtData::Print(Option_t *o) const
{
    *fLog << all << underline << GetDescriptor() << ":" << endl;
    fList.Print();
}

// --------------------------------------------------------------------------
//
// Add a new photon to the list
//
MRflSinglePhoton &MRflEvtData::GetNewPhoton()
{
    // If necessary the []-operator creates a new element
    // Warning: The virtual table may not be set correctly,
    //          this is why you have to call the new-operator.
    return *new (fList[fPos++]) MRflSinglePhoton;
}

// --------------------------------------------------------------------------
//
// If you have added all photon fix the size of the container.
//
void MRflEvtData::FixSize()
{
    if (fList.GetEntriesFast() == fPos)
        return;

    fList.ExpandCreate(fPos);
}

// --------------------------------------------------------------------------
//
// Fills all photon distances scaled with scale (default=1) into a TH1
//
void MRflEvtData::FillRad(TH1 &hist, Float_t scale) const
{
    MRflSinglePhoton *ph=NULL;

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        ph->FillRad(hist, scale);
}

// --------------------------------------------------------------------------
//
// Fills all photon distances scaled with scale (default=1) versus x
// into a TH2
//
void MRflEvtData::FillRad(TH2 &hist, Double_t x, Float_t scale) const
{
    MRflSinglePhoton *ph=NULL;

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        ph->FillRad(hist, x, scale);
}

// --------------------------------------------------------------------------
//
// Fills all photons (x,y) scaled with scale (default=1) into a TH2.
//
void MRflEvtData::Fill(TH2 &hist, Float_t scale) const
{
    MRflSinglePhoton *ph=NULL;

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        ph->Fill(hist, scale);
}

// --------------------------------------------------------------------------
//
// Fills all photons (x,y) scaled with scale (default=1) versus z into a TH3
//
void MRflEvtData::Fill(TH3 &hist, Double_t z, Float_t scale) const
{
    MRflSinglePhoton *ph=NULL;

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        ph->Fill(hist, z, scale);
}

void MRflEvtData::DrawPixelContent(Int_t num) const
{
}

// ------------------------------------------------------------------------
//
// Fill a reflector event. Sums all pixels in each pixel as the
// pixel contents.
//
// WARNING: Due to the estimation in DistanceToPrimitive and the
//          calculation in pixels instead of x, y this is only a
//          rough estimate.
//
Bool_t MRflEvtData::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    //
    // sum the photons content in each pixel
    //
    val = 0;

    MRflSinglePhoton *ph=NULL;

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
        if (cam[idx].IsInside(ph->GetX(), ph->GetY()))
            val += cam.GetPixRatio(idx);

    return val>0;
}

// ------------------------------------------------------------------------
//
// You can call Draw() to add the photons to the current pad.
// The photons are painted each tim ethe pad is updated.
// Make sure that you use the right (world) coordinate system,
// like created, eg. by the MHCamera histogram.
//
void MRflEvtData::Paint(Option_t *)
{
    MRflSinglePhoton *ph=NULL;

    TMarker m;
    m.SetMarkerStyle(kFullDotMedium); // Gtypes.h

    TIter Next(&fList);
    while ((ph=(MRflSinglePhoton*)Next()))
    {
        m.SetX(ph->GetX());
        m.SetY(ph->GetY());
        m.Paint();
    }
}

