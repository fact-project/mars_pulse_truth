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
!   Author(s): Markus Gaug   11/2003 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationCam                                               
//                                                               
// Base class for Camera Calibration results. 
//
// Contains TOrdCollections for the following objects:
// - fPixels:         Array of classes derived from MCalibrationPix, one entry 
//                    per pixel. Has to be created
// - fAverageAreas:   Array of classes derived from MCalibrationPix, one entry
//                    per pixel AREA. Has to be created
// - fAverageSectors: Array of classes derived from MCalibrationPix, one entry
//                    per camera SECTOR. Has to be created
//
// - fAverageBadAreas: Array of classes derived from MBadPixelsPix, one entry
//                     per pixel AREA. Is created automatically. 
// - fAverageBadSectors: Array of classes derived from MBadPixelsPix, one entry
//                    per camera SECTOR. Is created automatically.
//
// Previous Class Versions haven't been commented by the author!
//
// Class Version 6:
// ----------------
//  + added fRunNumber
//
// All TOrdCollections have to enlarged by the corresponding calls to (e.g. in MGeomApply): 
// - InitSize()
// - InitAverageAreas()
// - InitAverageSectors() 
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationCam.h"

#include <TOrdCollection.h>

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MCalibrationPix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCalibrationCam);

using namespace std;

const Int_t MCalibrationCam::gkNumPulserColors = 4;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Set the following pointer to NULL:
// - fPixels
// - fAverageAreas
// - fAverageSectors
//
// Initializes:
// - fPulserColor to kNONE 
// - fNumHiGainFADCSlices to 0.
// - fNumLoGainFADCSlices to 0.
//
// Creates a TOrdCollection of MBadPixelsPix containers for the TOrdCollection's: 
// - fAverageBadAreas
// - fAverageBadSectors
// all initialized to 1 entry
//
// Later, a call to InitAverageAreas() and InitAverageSectors() (or Init()) 
// has to be performed in order to get the dimension correctly.
//
MCalibrationCam::MCalibrationCam(const char *name, const char *title)
    : fRunNumber(-1), fPulserColor(kNONE)
{

  fPixels = new TOrdCollection;
  fPixels->SetOwner();

  fAverageAreas = new TOrdCollection;
  fAverageAreas->SetOwner();
  
  fAverageSectors = new TOrdCollection;
  fAverageSectors->SetOwner();

  fAverageBadAreas = new TOrdCollection;
  fAverageBadAreas->SetOwner();
  
  fAverageBadSectors = new TOrdCollection;
  fAverageBadSectors->SetOwner();

  fNumHiGainFADCSlices.Set(1);
  fNumLoGainFADCSlices.Set(1);
}

// --------------------------------------------------------------------------
//
// Deletes the following TOrdCollection's of MCalibrationPix containers (if exist):
// - fPixels
// - fAverageAreas
// - fAverageSectors
//  
// Deletes the following TOrdCollection's of MBadPixelsPix containers (if exist):
// - fAverageBadAreas
// - fAverageBadSectors
//
MCalibrationCam::~MCalibrationCam()
{

  //
  // delete fPixels should delete all Objects stored inside
  //
  if (fPixels)
    delete fPixels;

  if (fAverageAreas)
    delete fAverageAreas;

  if (fAverageSectors)
    delete fAverageSectors;

  if (fAverageBadAreas)
    delete fAverageBadAreas;

  if (fAverageBadSectors)
    delete fAverageBadSectors;
  
}

// --------------------------------------
//
// Calls the ForEach macro for the TOrdCollection fPixels with the argument Clear()
// 
// Loops over the fAverageAreas, calling the function Clear() for 
// every entry in:
// - fAverageAreas
// - fAverageBadAreas
// 
// Loops over the fAverageSectors, calling the function Clear() for 
// every entry in:
// - fAverageSectors
// - fAverageBadSectors
// 
void MCalibrationCam::Clear(Option_t *o)
{
    { fPixels        ->R__FOR_EACH(TObject, Clear)();  }
    { fAverageAreas  ->R__FOR_EACH(TObject, Clear)();  }
    { fAverageSectors->R__FOR_EACH(TObject, Clear)();  }
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MCalibrationCam::Copy(TObject& object) const
{
  
  MCalibrationCam &calib = (MCalibrationCam&)object;
  
  MParContainer::Copy(calib);
  
  calib.fPulserColor = fPulserColor;
  
  const Int_t n = GetSize();
  if (n != 0)
    {
      calib.InitSize(n);
      for (int i=0; i<n; i++)
        (*this)[i].Copy(calib[i]);
    }
  
  const Int_t n2 = GetAverageAreas();
  if (n2 != 0)
    {
      calib.InitAverageAreas(n2);
      for (int i=0; i<n2; i++)
        {
          GetAverageArea   (i).Copy(calib.GetAverageArea(i));
          GetAverageBadArea(i).Copy(calib.GetAverageBadArea(i));
          calib.fNumUnsuitable      [i] = fNumUnsuitable[i];
          calib.fNumUnreliable      [i] = fNumUnreliable[i];
          calib.fNumHiGainFADCSlices[i] = fNumHiGainFADCSlices[i];
          calib.fNumLoGainFADCSlices[i] = fNumLoGainFADCSlices[i];
        }
    }
  
  const Int_t n3 = GetAverageSectors();
  if (n3 != 0)
    {
      calib.InitAverageSectors(n3);
      for (int i=0; i<n3; i++)
        {
          GetAverageSector   (i).Copy(calib.GetAverageSector(i));
          GetAverageBadSector(i).Copy(calib.GetAverageBadSector(i));
        }
    }
}

// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollection using the 
// virtual function Add().
//
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MCalibrationCam::InitSize(const UInt_t i)
{

  const UInt_t save = GetSize();

  if (i==save)
    return;
  
  if (i>save)
    Add(save,i);
}

void MCalibrationCam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationPix,i);
}

void MCalibrationCam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageAreas->AddAt(new MCalibrationPix,i);
}

void MCalibrationCam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageSectors->AddAt(new MCalibrationPix,i);
}


// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollections
// - fAverageAreas
// - fAverageBadAreas
// using the virtual function Add().
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MCalibrationCam::InitAverageAreas(const UInt_t i)
{

  const UInt_t save = GetAverageAreas();

  if (i==save)
    return;
  
  fNumUnsuitable.Set(i);
  fNumUnreliable.Set(i);
  fNumHiGainFADCSlices.Set(i);
  fNumLoGainFADCSlices.Set(i);

  if (i < save)
    return;

  for (UInt_t j=save; j<i; j++)
    fAverageBadAreas->AddAt(new MBadPixelsPix,j);
  
  AddArea(save,i);

  for (UInt_t j=save; j<i; j++)
    GetAverageArea(j).SetPixId(j);

}

// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollections
// - fAverageSectors
// - fAverageBadSectors
// using the virtual function Add().
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MCalibrationCam::InitAverageSectors(const UInt_t i)
{

  const UInt_t save = GetAverageSectors();

  if (i==save)
    return;
  
  if (i < save)
    return;

  for (UInt_t j=save; j<i; j++)
    fAverageBadSectors->AddAt(new MBadPixelsPix,j);
  
  AddSector(save,i);

  for (UInt_t j=save; j<i; j++)
    GetAverageSector(j).SetPixId(j);
}

// -------------------------------------------------------------------
//
// Calls:
// - InitSize()
// - InitAverageAreas()
// - InitAverageSectors()
//
void MCalibrationCam::Init(const MGeomCam &geom)
{
  InitSize          (geom.GetNumPixels() );
  InitAverageAreas  (geom.GetNumAreas()  );
  InitAverageSectors(geom.GetNumSectors());
}

// --------------------------------------------------------------------------
//
// Returns the number of un-suitable pixels per area index and -1 if 
// the area index exceeds the initialized array.
//
Int_t MCalibrationCam::GetNumUnsuitable( const Int_t  aidx ) const
{
  if (aidx < 0)
    {
      Int_t num = 0;
      for (Int_t i=0;i<fNumUnsuitable.GetSize();i++)
        num += fNumUnsuitable[i];
      return num;
    }
  
  return aidx > fNumUnsuitable.GetSize() ? -1 : fNumUnsuitable[aidx]; 
}

// --------------------------------------------------------------------------
//
// Returns the number of un-reliable pixels per area index and -1 if 
// the area index exceeds the initialized array.
//
Int_t MCalibrationCam::GetNumUnreliable( const Int_t  aidx ) const
{
  if (aidx < 0)
    {
      Int_t num = 0;
      for (Int_t i=0;i<fNumUnreliable.GetSize();i++)
        num += fNumUnreliable[i];
      return num;
    }

  return aidx > fNumUnreliable.GetSize() ? -1 : fNumUnreliable[aidx]; 
}

// --------------------------------------------------------------------------
//
// Returns the mean number of High-Gain FADC slices per area index and -1 if 
// the area index exceeds the initialized array.
//
Float_t MCalibrationCam::GetNumHiGainFADCSlices( const Int_t  aidx ) const
{
  if (aidx < 0)
    return -1;

  return aidx > fNumHiGainFADCSlices.GetSize() ? -1 : fNumHiGainFADCSlices[aidx]; 
}

// --------------------------------------------------------------------------
//
// Returns the mean number of Low-Gain FADC slices per area index and -1 if 
// the area index exceeds the initialized array.
//
Float_t MCalibrationCam::GetNumLoGainFADCSlices( const Int_t  aidx ) const
{
  if (aidx < 0)
    return -1;

  return aidx > fNumLoGainFADCSlices.GetSize() ? -1 : fNumLoGainFADCSlices[aidx]; 
}



// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fAverageAreas
// independently if the MCalibrationPix is filled with values or not.
//
Int_t MCalibrationCam::GetAverageAreas() const
{
  return fAverageAreas->GetSize();
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fAverageSectors
// independently if the MCalibrationPix is filled with values or not.
//
Int_t MCalibrationCam::GetAverageSectors() const
{
  return fAverageSectors->GetSize();
}


// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
MCalibrationPix &MCalibrationCam::operator[](UInt_t i)
{
  return *static_cast<MCalibrationPix*>(fPixels->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
const MCalibrationPix &MCalibrationCam::operator[](UInt_t i) const
{
  return *static_cast<MCalibrationPix*>(fPixels->At(i));
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fPixels 
// independently if the MCalibrationPix is filled with values or not.
//
Int_t MCalibrationCam::GetSize() const
{
  return fPixels->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
MCalibrationPix &MCalibrationCam::GetAverageArea(const UInt_t i)
{
  return *static_cast<MCalibrationPix*>(fAverageAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
const MCalibrationPix &MCalibrationCam::GetAverageArea(const UInt_t i) const 
{
  return *static_cast<MCalibrationPix*>(fAverageAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
MCalibrationPix &MCalibrationCam::GetAverageSector(const UInt_t i)
{
  return *static_cast<MCalibrationPix*>(fAverageSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
const MCalibrationPix &MCalibrationCam::GetAverageSector(const UInt_t i) const 
{
  return *static_cast<MCalibrationPix*>(fAverageSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
MBadPixelsPix &MCalibrationCam::GetAverageBadArea(const UInt_t i)
{
  return *static_cast<MBadPixelsPix*>(fAverageBadAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (area number)
//
const MBadPixelsPix &MCalibrationCam::GetAverageBadArea(const UInt_t i) const 
{
  return *static_cast<MBadPixelsPix*>(fAverageBadAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
MBadPixelsPix &MCalibrationCam::GetAverageBadSector(const UInt_t i)
{
  return *static_cast<MBadPixelsPix*>(fAverageBadSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th average pixel (sector number)
//
const MBadPixelsPix &MCalibrationCam::GetAverageBadSector(const UInt_t i) const 
{
  return *static_cast<MBadPixelsPix*>(fAverageBadSectors->At(i));
}



// --------------------------------------------------------------------------
//
Bool_t MCalibrationCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx > GetSize())
        return kFALSE;

    const Float_t area = cam[idx].GetA();

    if (area == 0)
        return kFALSE;

    if ((*this)[idx].IsExcluded())
        return kFALSE;

    switch (type)
    {
    case 0:
        val = (*this)[idx].GetHiGainMean();
        break;
    case 1:
        val = (*this)[idx].GetHiGainMeanErr();
        break;
    case 2:
        val = (*this)[idx].GetHiGainSigma();
        break;
    case 3:
        val = (*this)[idx].GetHiGainSigmaErr();
        break;
    case 4:
        val = (*this)[idx].GetHiGainProb();
        break;
    case 5:
        val = (*this)[idx].GetLoGainMean();
        break;
    case 6:
        val = (*this)[idx].GetLoGainMeanErr();
        break;
    case 7:
        val = (*this)[idx].GetLoGainSigma();
        break;
    case 8:
        val = (*this)[idx].GetLoGainSigmaErr();
        break;
    default:
        return kFALSE;
    }

    return val!=-1.;
}

// --------------------------------------------------------------------------
//
// Calls MCalibrationPix::DrawClone()
//
void MCalibrationCam::DrawPixelContent(Int_t idx) const
{
  (*this)[idx].DrawClone();
}

void MCalibrationCam::SetNumHiGainFADCSlices( const Float_t i, const Int_t aidx)
{
  if (aidx < 0)
    return;

  if (aidx < fNumHiGainFADCSlices.GetSize())
    fNumHiGainFADCSlices[aidx] = i;  
}

void MCalibrationCam::SetNumLoGainFADCSlices( const Float_t i, const Int_t aidx)
{
  if (aidx < 0)
    return;
  if (aidx < fNumLoGainFADCSlices.GetSize())
    fNumLoGainFADCSlices[aidx] = i; 
}

void MCalibrationCam::SetNumUnsuitable( const UInt_t i, const Int_t aidx)
{
  if (aidx < 0)
    return;

  if (aidx < fNumUnsuitable.GetSize())
    fNumUnsuitable[aidx] = i;  
}

void MCalibrationCam::SetNumUnreliable( const UInt_t i, const Int_t aidx)
{
  if (aidx < 0)
    return;
  if (aidx < fNumUnreliable.GetSize())
    fNumUnreliable[aidx] = i; 
}

TString MCalibrationCam::GetPulserColorStr(PulserColor_t col)
{
    TString str;
    switch (col)
    {
    case kCT1:   str += "CT1";      break;
    case kGREEN: str += "Green";    break;
    case kBLUE:  str += "Blue";     break;
    case kUV:    str += "UV";       break;
    case kNONE:  str += "None";     break;
    default:     str += "Unknown";  break;
    }
    str += " (";
    str += (int)col;
    str += ")";
    return str;
}
