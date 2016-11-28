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
!   Author(s): Thomas Bretz, 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Hendrik Bartko, 07/2003 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MPedPhotCam
//
// Hold the Pedestal information for all pixels in the camera (in usints
// of photons)
//
// Version 2:
// ----------
//   - added fAreas
//   - added fSectors
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedPhotCam.h"

#include <TClonesArray.h>

#include "MArrayI.h"
#include "MArrayD.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MPedPhotPix.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MPedPhotCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Contains the default constructor. Creates a MPedPhotPix object
// for each pixel
//
void MPedPhotCam::InitArrays(const char *name, const char *title)
{
    fName  = name  ? name  : "MPedPhotCam";
    fTitle = title ? title : "Storage container for all Pedestal Information in the camera (in units of photons)";

    fArray   = new TClonesArray("MPedPhotPix", 1);
    fAreas   = new TClonesArray("MPedPhotPix", 1);
    fSectors = new TClonesArray("MPedPhotPix", 1);
}

// --------------------------------------------------------------------------
//
// Default constructor. Creates a MPedPhotPix object for each pixel
//
MPedPhotCam::MPedPhotCam(const char *name, const char *title)
{
    InitArrays(name, title);
}

// --------------------------------------------------------------------------
//
// Initialize the mean, rms and number of events with the values from
// the MPedestalCam converted 1:1
//
MPedPhotCam::MPedPhotCam(const MPedestalCam &p)
{
    const Int_t n = p.GetSize();

    InitArrays();
    InitSize(n);

    for (int i=0; i<n; i++)
        (*this)[i].Set(p[i].GetPedestal(), p[i].GetPedestalRms(), p[i].GetNumEvents());
}

// --------------------------------------------------------------------------
//
// Delete the array conatining the pixel pedest information
//
MPedPhotCam::~MPedPhotCam()
{
    delete fArray;
    delete fAreas;
    delete fSectors;
}

// --------------------------------------------------------------------------
//
// Set the size of the camera
//
void MPedPhotCam::InitSize(const UInt_t i)
{
    fArray->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls TClonesArray::ExpandCreate() for:
// - fAverageAreas
//
void MPedPhotCam::InitAreas(const UInt_t i)
{
    fAreas->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls TClonesArray::ExpandCreate() for:
// - fAverageSectors
//
void MPedPhotCam::InitSectors(const UInt_t i)
{
    fSectors->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls:
// - InitSize()
// - InitAverageAreas()
// - InitAverageSectors()
//
void MPedPhotCam::Init(const MGeomCam &geom)
{
    InitSize(geom.GetNumPixels());
    InitAreas(geom.GetNumAreas());
    InitSectors(geom.GetNumSectors());
}


// --------------------------------------------------------------------------
//
// Get the size of the MPedPhotCam
//
Int_t MPedPhotCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get the size of the MPedPhotCam
//
Int_t MPedPhotCam::GetNumSectors() const
{
    return fSectors->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get the size of the MPedPhotCam
//
Int_t MPedPhotCam::GetNumAreas() const
{
    return fAreas->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
MPedPhotPix &MPedPhotCam::operator[](Int_t i)
{
    return *static_cast<MPedPhotPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
const MPedPhotPix &MPedPhotCam::operator[](Int_t i) const
{
    return *static_cast<MPedPhotPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
MPedPhotPix &MPedPhotCam::GetArea(UInt_t i)
{
    return *static_cast<MPedPhotPix*>(fAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
MPedPhotPix &MPedPhotCam::GetSector(UInt_t i)
{
    return *static_cast<MPedPhotPix*>(fSectors->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
const MPedPhotPix &MPedPhotCam::GetArea(UInt_t i)const
{
    return *static_cast<MPedPhotPix*>(fAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
const MPedPhotPix &MPedPhotCam::GetSector(UInt_t i) const
{
    return *static_cast<MPedPhotPix*>(fSectors->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Call clear of all three TClonesArray
//
void MPedPhotCam::Clear(Option_t *o)
{
    { fArray->R__FOR_EACH(TObject, Clear)(); }
    { fAreas->R__FOR_EACH(TObject, Clear)(); }
    { fSectors->R__FOR_EACH(TObject, Clear)(); }
}

// --------------------------------------------------------------------------
//
// Calculates the avarage pedestal and pedestal rms for all sectors
// and pixel sizes. The geometry container is used to get the necessary
// geometry information (sector number, size index) If the bad pixel
// container is given all pixels which have the flag 'bad' are ignored
// in the calculation of the sector and size average.
//
void MPedPhotCam::ReCalc(const MGeomCam &geom, MBadPixelsCam *bad)
{
    const Int_t np = GetSize();
    const Int_t ns = GetNumSectors();
    const Int_t na = GetNumAreas();

    // Using MArray instead of TArray because they don't do range checks
    MArrayI acnt(na);
    MArrayI scnt(ns);
    MArrayD asumx(na);
    MArrayD ssumx(ns);
    MArrayD asumr(na);
    MArrayD ssumr(ns);

    for (int i=0; i<np; i++)
    {
        if (bad && (*bad)[i].IsUnsuitable())
            continue;

        // Create sums for areas and sectors
        const MPedPhotPix &pix = (*this)[i];

        const UInt_t  ne   = pix.GetNumEvents();
        const Float_t mean = ne*pix.GetMean();
	const Float_t rms  = ne*pix.GetRms();

        const UInt_t aidx = geom[i].GetAidx();
        asumx[aidx] += mean;
	asumr[aidx] += rms;
        acnt[aidx]  += ne;

        const UInt_t sect = geom[i].GetSector();
        ssumx[sect] += mean;
	ssumr[sect] += rms;
        scnt[sect]  += ne;
    }

    for (int i=0; i<ns; i++)
        if (scnt[i]>0)
            GetSector(i).Set(ssumx[i]/scnt[i], ssumr[i]/scnt[i], scnt[i]);
        else
            GetSector(i).Clear();

    for (int i=0; i<na; i++)
        if (acnt[i]>0)
            GetArea(i).Set(asumx[i]/acnt[i], asumr[i]/acnt[i], acnt[i]);
        else
            GetArea(i).Clear();
}

// --------------------------------------------------------------------------
//
// print contents
//
void MPedPhotCam::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":" << endl;
    int id = 0;

    TIter Next(fArray);
    MPedPhotPix *pix;
    while ((pix=(MPedPhotPix*)Next()))
    {
        id++;

        if (!pix->IsValid())
            continue;

        *fLog << id-1 << ": ";
        *fLog << pix->GetMean() << " " << pix->GetRms() << endl;
    }
}

// --------------------------------------------------------------------------
//
// See MCamEvent
//
Bool_t MPedPhotCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx>=GetSize())
        return kFALSE;

    switch (type)
    {
    case 0:
        val = (*this)[idx].GetMean();
        break;
    case 1:
        val = (*this)[idx].GetRms();
        break;
    case 2:
        val = (*this)[idx].GetNumEvents()>0 ? (*this)[idx].GetRms()/TMath::Sqrt((Float_t)(*this)[idx].GetNumEvents()) : -1;
        break;
    case 3:
        val = (*this)[idx].GetNumEvents()>0 ? (*this)[idx].GetRms()/TMath::Sqrt((Float_t)(*this)[idx].GetNumEvents())/2. : -1;
        break;
    case 4:
        val = (*this)[idx].GetMean()*cam.GetPixRatio(idx);
        break;
    case 5:
        val = (*this)[idx].GetRms()*cam.GetPixRatioSqrt(idx);
        break;
    default:
	return kFALSE;
    }
    return val>=0;
}

void MPedPhotCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MPedPhotCam::DrawPixelContent - not available." << endl;
}
