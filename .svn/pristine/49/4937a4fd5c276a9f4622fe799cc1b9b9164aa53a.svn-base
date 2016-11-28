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
!   Author(s): Thomas Bretz   12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Markus Gaug    02/2004 <mailto:markus@ifae.es>
!   Author(s): Florian Goebel 06/2004 <mailto:fgoebel@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MPedestalCam
//
// Hold the Pedestal information for all pixels in the camera
//
// Class Version 2:
// ----------------
//  + fNumSlices
//  - fTotalEntries
//
// Class Version 3:
// ----------------
//  + fNumEvents
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include <TMath.h>

#include <TArrayI.h>
#include <TArrayF.h>
#include <TArrayD.h>

#include <TClonesArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MPedestalCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Creates a TClonesArray of MPedestalPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel. Later, a call to MPedestalCam::InitSize() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MPedestalPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel AREA. Later, a call to MPedestalCam::InitAreas() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MPedestalPix containers, initialized to 1 entry, destinated
// to hold one container per camera SECTOR. Later, a call to MPedestalCam::InitSectors() 
// has to be performed (in MGeomApply). 
//
MPedestalCam::MPedestalCam(const char *name, const char *title) 
    : fNumSlices(0), fNumEvents(0)
{
  fName  = name  ? name  : "MPedestalCam";
  fTitle = title ? title : "Storage container for all Pedestal Information in the camera";

  fArray            = new TClonesArray("MPedestalPix", 1);
  fAverageAreas     = new TClonesArray("MPedestalPix", 1);
  fAverageSectors   = new TClonesArray("MPedestalPix", 1);
}

// --------------------------------------------------------------------------
//
// Deletes the following TClonesArray's of MPedestalPix containers (if exist):
// - fArray
// - fAverageAreas
// - fAverageSectors
//  
MPedestalCam::~MPedestalCam()
{
  delete fArray;
  delete fAverageAreas;
  delete fAverageSectors;
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MPedestalCam::Copy(TObject &obj) const
{

  MParContainer::Copy(obj);

  MPedestalCam &cam = (MPedestalCam&)obj;

  Int_t n = GetSize();

  if (n==0)
    return;

  cam.InitSize(n);
  for (int i=0; i<n; i++)
    (*this)[i].Copy(cam[i]);

  cam.fNumEvents = fNumEvents;
  cam.fNumSlices = fNumSlices;

  n = GetNumAverageArea();
  cam.InitAverageAreas(n);
  for (int i=0; i<n; i++)
    GetAverageArea(i).Copy(cam.GetAverageArea(i));

  n = GetNumAverageSector();
  cam.InitAverageSectors(n);
  for (int i=0; i<n; i++)
    GetAverageSector(i).Copy(cam.GetAverageSector(i));
}

// --------------------------------------------------------------------------
//
// Set the size of the camera
//
void MPedestalCam::InitSize(const UInt_t i)
{
    fArray->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls TClonesArray::ExpandCreate() for:
// - fAverageAreas
//
void MPedestalCam::InitAverageAreas(const UInt_t i)
{
  fAverageAreas->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls TClonesArray::ExpandCreate() for:
// - fAverageSectors
//
void MPedestalCam::InitAverageSectors(const UInt_t i)
{
  fAverageSectors->ExpandCreate(i);
}

// -------------------------------------------------------------------
//
// Calls:
// - InitSize()
// - InitAverageAreas()
// - InitAverageSectors()
//
void MPedestalCam::Init(const MGeomCam &geom)
{
  InitSize          (geom.GetNumPixels() );
  InitAverageAreas  (geom.GetNumAreas()  );
  InitAverageSectors(geom.GetNumSectors());
}

// --------------------------------------------------------------------------
//
// This function returns the current size of the TClonesArray 
// independently if the MPedestalPix is filled with values or not.
//
// Get the size of the MPedestalCam
//
Int_t MPedestalCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TClonesArray fAverageAreas
// independently if the MPedestalPix is filled with values or not.
//
Int_t MPedestalCam::GetNumAverageArea() const
{
  return fAverageAreas->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TClonesArray fAverageSectors
// independently if the MPedestalPix is filled with values or not.
//
Int_t MPedestalCam::GetNumAverageSector() const
{
  return fAverageSectors->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
MPedestalPix &MPedestalCam::operator[](Int_t i)
{
    return *static_cast<MPedestalPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
const MPedestalPix &MPedestalCam::operator[](Int_t i) const
{
    return *static_cast<MPedestalPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
MPedestalPix &MPedestalCam::GetAverageArea(UInt_t i)
{
  return *static_cast<MPedestalPix*>(fAverageAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
const MPedestalPix &MPedestalCam::GetAverageArea(UInt_t i) const 
{
  return *static_cast<MPedestalPix*>(fAverageAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
MPedestalPix &MPedestalCam::GetAverageSector(UInt_t i)
{
  return *static_cast<MPedestalPix*>(fAverageSectors->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
const MPedestalPix &MPedestalCam::GetAverageSector(UInt_t i) const 
{
  return *static_cast<MPedestalPix*>(fAverageSectors->UncheckedAt(i));
}

// --------------------------------------
//
// Calls the ForEach macro for the TClonesArray fArray with the argument Clear()
// 
// Loops over the fAverageAreas, calling the function Clear() for 
// every entry in fAverageAreas
//
// Loops over the fAverageSectors, calling the function Clear() for 
// every entry in fAverageSectors
// 
void MPedestalCam::Clear(Option_t *o)
{
    { fArray->R__FOR_EACH(TObject, Clear)(); }
    { fAverageAreas->R__FOR_EACH(TObject, Clear)(); }
    { fAverageSectors->R__FOR_EACH(TObject, Clear)(); }

    fNumSlices = 0;
}

void MPedestalCam::PrintArr(const TCollection &list) const
{
    Int_t id = 0;

    TIter Next(&list);
    MPedestalPix *pix = 0;

    while ((pix=(MPedestalPix*)Next()))
        *fLog << Form("Nr.: %4i  Pedestal: %5.2f    RMS: %5.2f    ABOffset: %5.2f ",
                      id++, pix->GetPedestal(), pix->GetPedestalRms(), pix->GetPedestalABoffset()) << endl;
}

void MPedestalCam::Print(Option_t *option) const
{
    *fLog << all << GetDescriptor();
    *fLog << " - Events=" << fNumEvents << ", Slices=" << fNumSlices << ":" << endl;

    *fLog << endl;
    *fLog << "Event-by-event averaged areas:" << endl;
    PrintArr(*fAverageAreas);

    *fLog << endl;
    *fLog << "Event-by-event averaged sectors:" << endl;
    PrintArr(*fAverageSectors);

    if (TString(option).Contains("all", TString::kIgnoreCase))
    {
        *fLog << endl;
        *fLog << "Pixels:" << endl;
        PrintArr(*fArray);
    }
}

/*
Float_t MPedestalCam::GetPedestalMin(const MGeomCam *geom) const
{
    if (fArray->GetEntries() <= 0)
        return 50.;

    Float_t minval = (*this)[0].GetPedestalRms();

    for (Int_t i=1; i<fArray->GetEntries(); i++)
    {
        const MPedestalPix &pix = (*this)[i];

        Float_t testval = pix.GetPedestalRms();

        if (geom)
            testval *= geom->GetPixRatio(i);

        if (testval < minval)
            minval = testval;
    }
    return minval;
}

Float_t MPedestalCam::GetPedestalMax(const MGeomCam *geom) const
{
    if (fArray->GetEntries() <= 0)
        return 50.;

    Float_t maxval = (*this)[0].GetPedestalRms();

    for (Int_t i=1; i<fArray->GetEntries(); i++)
    {
        const MPedestalPix &pix = (*this)[i];

        Float_t testval = pix.GetPedestalRms();

        if (geom)
            testval *= geom->GetPixRatio(i);

        if (testval > maxval)
            maxval = testval;
    }
    return maxval;
}
*/

// --------------------------------------------------------------------------
//
// Calculates the average pedestal for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index) If the bad pixel
// container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged pedestal (default: -1.)
// arr[1]: Error (rms) of averaged pedestal (default: 0.)
//
TArrayF MPedestalCam::GetAveragedPedPerArea(const MGeomCam &geom, const UInt_t ai, MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean  = 0.;
  Double_t mean2 = 0.;
  Int_t    nr    = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;

      const MPedestalPix &pix = (*this)[i];
      
      mean  += pix.GetPedestal();
      mean2 += pix.GetPedestal()*pix.GetPedestal();
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1.;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average pedestal rms for pixel sizes. 
// The geometry container is used to get the necessary
// geometry information (area index) If the bad pixel
// container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged pedestal RMS (default: -1.)
// arr[1]: Error (rms) of averaged pedestal RMS (default: 0.)
//
TArrayF MPedestalCam::GetAveragedRmsPerArea(const MGeomCam &geom, const UInt_t ai, MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t rms  = 0.;
  Double_t rms2 = 0.;
  Int_t    nr   = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue; 
      
      const UInt_t aidx = geom[i].GetAidx();
      
      if (ai != aidx)
        continue;

      const MPedestalPix &pix = (*this)[i];
      
      rms  += pix.GetPedestalRms();
      rms2 += pix.GetPedestalRms()*pix.GetPedestalRms();
      nr   ++;
    }

  TArrayF arr(2);
  arr[0] = nr   ? rms/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((rms2 - rms*rms/nr)/(nr-1)) : 0;
  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average pedestal for camera sectors. 
// The geometry container is used to get the necessary
// geometry information (area index) If the bad pixel
// container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged pedestal (default: -1.)
// arr[1]: Error (rms) of averaged pedestal (default: 0.)
//
TArrayF MPedestalCam::GetAveragedPedPerSector(const MGeomCam &geom, const UInt_t sec, MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t mean = 0.;
  Double_t mean2 = 0.;
  Int_t    nr   = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue;
      
      const UInt_t sector = geom[i].GetSector();
      
      if (sec != sector)
        continue;

      const MPedestalPix &pix = (*this)[i];
      
      mean  += pix.GetPedestal();
      mean2 += pix.GetPedestal()*pix.GetPedestal();
      nr    ++;
      
    }

  TArrayF arr(2);
  arr[0] = nr   ? mean/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((mean2 - mean*mean/nr)/(nr-1)) : 0;
  return arr;
}

// --------------------------------------------------------------------------
//
// Calculates the average pedestal rms for camera sectors. 
// The geometry container is used to get the necessary
// geometry information (area index) If the bad pixel
// container is given all pixels which have the flag 'kUnsuitableRun' are ignored
// in the calculation of the size average.
//
// Returns a TArrayF of dimension 2: 
// arr[0]: averaged pedestal RMS (default: -1.)
// arr[1]: Error (rms) of averaged pedestal RMS (default: 0.)
//
TArrayF MPedestalCam::GetAveragedRmsPerSector(const MGeomCam &geom, const UInt_t sec, MBadPixelsCam *bad)
{

  const Int_t np = GetSize();

  Double_t rms  = 0.;
  Double_t rms2 = 0.;
  Int_t    nr   = 0;

  for (int i=0; i<np; i++)
    {
      if (bad && (*bad)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue;

      const UInt_t sector = geom[i].GetSector();

      if (sec != sector)
        continue;

      const MPedestalPix &pix = (*this)[i];

      rms  += pix.GetPedestalRms();
      rms2 += pix.GetPedestalRms()*pix.GetPedestalRms();
      nr   ++;

    }

  TArrayF arr(2);
  arr[0] = nr   ? rms/nr : -1;
  arr[1] = nr>1 ? TMath::Sqrt((rms2 - rms*rms/nr)/(nr-1)) : 0;
  return arr;
}


Bool_t MPedestalCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (GetSize() <= idx)
        return kFALSE;

    if (!(*this)[idx].IsValid())
        return kFALSE;

    const Float_t ped = (*this)[idx].GetPedestal();
    const Float_t rms = (*this)[idx].GetPedestalRms();
    const UInt_t  num = (*this)[idx].GetNumEvents()*fNumSlices;

    switch (type)
    {
    case 0:
        val = ped;
        break;
    case 1:
        val = fNumSlices>0 ? rms/TMath::Sqrt((Float_t)num)
            : (*this)[idx].GetPedestalError();
        break;
    case 2:
        val = rms;
        break;
    case 3:
        val = fNumSlices>0 ? rms/TMath::Sqrt((Float_t)num*2.)
            : (*this)[idx].GetPedestalRmsError();
        break;
    case 4:
        val = rms;
        break;
    default:
        return kFALSE;
    }
    return kTRUE;
}

void MPedestalCam::DrawPixelContent(Int_t idx) const
{
    *fLog << warn << "MPedestalCam::DrawPixelContent - not available." << endl;
}
