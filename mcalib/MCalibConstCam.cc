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
!   Author(s): Markus Gaug    01/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCalibConstCam
//
// Hold the temporary conversion factors for MCalibrateDatara
//
// Version 2:
// ---------
//   - fRunNumber
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibConstCam.h"
#include "MCalibConstPix.h"

#include <TClonesArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MCalibConstCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Creates a TClonesArray of MCalibConstPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel. Later, a call to MCalibConstCam::InitSize() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibConstPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel AREA. Later, a call to MCalibConstCam::InitAreas() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibConstPix containers, initialized to 1 entry, destinated
// to hold one container per camera SECTOR. Later, a call to MCalibConstCam::InitSectors() 
// has to be performed (in MGeomApply). 
//
MCalibConstCam::MCalibConstCam(const char *name, const char *title)
    : fBadPixels(0)
{
  fName  = name  ? name  : "MCalibConstCam";
  fTitle = title ? title : "Temporary Storage for Calibration Constants";
  
  fArray            = new TClonesArray("MCalibConstPix", 1);
  fAverageAreas     = new TClonesArray("MCalibConstPix", 1);
  fAverageSectors   = new TClonesArray("MCalibConstPix", 1);
}

// --------------------------------------------------------------------------
//
// Deletes the following TClonesArray's of MCalibConstPix containers:
// - fArray
// - fAverageAreas
// - fAverageSectors
//  
MCalibConstCam::~MCalibConstCam()
{
  delete fArray;
  delete fAverageAreas;
  delete fAverageSectors;
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MCalibConstCam::Copy(TObject &obj) const
{

  MParContainer::Copy(obj);

  MCalibConstCam &cam = (MCalibConstCam&)obj;
  
  Int_t n = GetSize();
  
  if (n==0)
    return;
  
  cam.InitSize(n);
  for (int i=0; i<n; i++)
    (*this)[i].Copy(cam[i]);

  n = GetNumAverageArea();
  cam.InitAverageAreas(n);
  for (int i=0; i<n; i++)
    GetAverageArea(i).Copy(cam.GetAverageArea(i));

  n = GetNumAverageSector();
  cam.InitAverageSectors(n);
  for (int i=0; i<n; i++)
    GetAverageSector(i).Copy(cam.GetAverageSector(i));
}


// -------------------------------------------------------------------
//
// Calls:
// - InitSize()
// - InitAverageAreas()
// - InitAverageSectors()
//
void MCalibConstCam::Init(const MGeomCam &geom)
{
  InitSize          (geom.GetNumPixels() );
  InitAverageAreas  (geom.GetNumAreas()  );
  InitAverageSectors(geom.GetNumSectors());
}

// --------------------------------------------------------------------------
//
// This function returns the current size of the TClonesArray 
// independently if the MCalibConstPix is filled with values or not.
//
// Get the size of the MCalibConstCam
//
Int_t MCalibConstCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TClonesArray fAverageAreas
// independently if the MCalibConstPix is filled with values or not.
//
const Int_t MCalibConstCam::GetNumAverageArea() const
{
  return fAverageAreas->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TClonesArray fAverageSectors
// independently if the MCalibConstPix is filled with values or not.
//
const Int_t MCalibConstCam::GetNumAverageSector() const
{
  return fAverageSectors->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
MCalibConstPix &MCalibConstCam::operator[](Int_t i)
{
    return *static_cast<MCalibConstPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
const MCalibConstPix &MCalibConstCam::operator[](Int_t i) const
{
    return *static_cast<MCalibConstPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
MCalibConstPix &MCalibConstCam::GetAverageArea(UInt_t i)
{
  return *static_cast<MCalibConstPix*>(fAverageAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
const MCalibConstPix &MCalibConstCam::GetAverageArea(UInt_t i) const 
{
  return *static_cast<MCalibConstPix*>(fAverageAreas->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
MCalibConstPix &MCalibConstCam::GetAverageSector(UInt_t i)
{
  return *static_cast<MCalibConstPix*>(fAverageSectors->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
const MCalibConstPix &MCalibConstCam::GetAverageSector(UInt_t i) const 
{
  return *static_cast<MCalibConstPix*>(fAverageSectors->UncheckedAt(i));
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
void MCalibConstCam::Clear(Option_t *o)
{
    { fArray->R__FOR_EACH(TObject, Clear)(); }
    { fAverageAreas->R__FOR_EACH(TObject, Clear)(); }
    { fAverageSectors->R__FOR_EACH(TObject, Clear)(); }
  
}

void MCalibConstCam::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":" << endl;
    int id = 0;

    TIter Next(fArray);
    MCalibConstPix *pix;
    while ((pix=(MCalibConstPix*)Next()))
    {
        *fLog << id 
	      << Form(": Conversion Factor: %4.3f  Global F-Factor: %4.3f",pix->GetCalibConst(),pix->GetCalibFFactor()) 
	      << endl;
        id++;
    }
}


Bool_t MCalibConstCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (GetSize() <= idx)
        return kFALSE;

    if (fBadPixels && (*fBadPixels)[idx].IsUnsuitable())
        return kFALSE;

    switch (type)
    {
    case 0:
      val = (*this)[idx].GetCalibConst();
      break;
    case 1:
      val = (*this)[idx].GetCalibFFactor();
      break;
    case 2:
      val = (*this)[idx].GetCalibConst()*cam.GetPixRatio(idx);
      break;
    default:
      return kFALSE;
    }

    return val>0.;
}

void MCalibConstCam::DrawPixelContent(Int_t idx) const
{
    *fLog << warn << "MCalibConstCam::DrawPixelContent - not available." << endl;
}
