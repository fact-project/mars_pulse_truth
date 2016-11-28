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
// MCalibrationIntensityCam                                               
//                                                               
// Base class for intensity calibration results 
//
// Contains TOrdCollections for the following objects:
// - fCams:  Array of classes derived from MCalibrationCam, one entry 
//           per calibration camera result.
// - fHists: Array of classes derived from MHCalibrationPix, one entry
//           per calibration camera result and area index
//
// See also: MCalibrationIntensityChargeCam, 
//           MCalibrationCam, MCalibrationPix, MHCalibrationChargePix,
//           MHCalibrationChargeCam, MCalibrationChargeBlindPix,
//           MCalibrationChargePINDiode
//
// Inline Functions:
// -----------------
//
// GetSize(): Returns the current size of the TOrdCollection fCams
//  independently if the MCalibrationCam is filled with values or not.
//
// GetAverageAreas(): Returns the current size of the TOrdCollection
//  fAverageAreas of the current camera.
//
// GetAverageArea(UInt_t i): Get i-th High Gain pixel Area from the
//  current camera
//
// GetAverageArea(UInt_t i): Get i-th High Gain pixel Area from the
//  current camera
//
// GetAverageBadArea(UInt_t i): Get i-th High Gain pixel Area from the
//  current camera
//
// GetAverageBadArea(UInt_t i): Get i-th High Gain pixel Area from the
//  current camera
//
// GetAverageSectors(): Returns the current size of the TOrdCollection
//  fAverageSectors or the current camera
//
// GetAverageSector(UInt_t i): Get i-th High Gain Sector from the
//  current camera
//
// GetAverageSector(UInt_t i): Get i-th High Gain Sector from the current
//  camera
//
// GetAverageBadSector(UInt_t i): Get i-th High Gain Sector from the
//  current camera
//
// GetAverageBadSector(UInt_t i): Get i-th High Gain Sector from the
//  current camera
//
//
// ClassVersion 2:
//  + fHists
//
// ClassVersion 3:
//  - MArrayD fOffsets;           //! Arrays of Higain-vs-LoGain fit result Offsets
//  - MArrayD fSlopes;            //! Arrays of Higain-vs-LoGain fit result Slopes
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationIntensityCam.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MHCalibrationCam.h"

ClassImp(MCalibrationIntensityCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Set the following pointer to NULL:
// - fCams
// - fHists
//
MCalibrationIntensityCam::MCalibrationIntensityCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationIntensityCam";
  fTitle = title ? title : "Base container for the Intensity Calibration";

  fCams = new TOrdCollection;
  fCams->SetOwner();

  fHists = new TOrdCollection;
  fHists->SetOwner();
}


// --------------------------------------------------------------------------
//
// Deletes the histograms if they exist
//
MCalibrationIntensityCam::~MCalibrationIntensityCam()
{
    delete fCams;
    delete fHists;
}

// --------------------------------------------------------------------------
//
// Add a new MHCalibrationCam to fHists
//
void MCalibrationIntensityCam::AddHist( const MHCalibrationCam *cam)
{
    const Int_t size = fHists->GetSize();
    fHists->AddAt((TObject*)cam,size);

    if (size != GetSize()-1)
        *fLog << warn << "Histogram Cams and Calibration Cams size mismatch! " << endl;
}

// --------------------------------------------------------------------------
//
// Add a new MCalibrationCam to fCams, give it the name "name" and initialize
// it with geom.
//
void MCalibrationIntensityCam::AddToList( const char* name, const MGeomCam &geom) 
{
  InitSize(GetSize()+1);
  GetCam()->SetName(name);
  GetCam()->Init(geom);
}



// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MCalibrationIntensityCam::Copy(TObject& object) const
{
    MCalibrationIntensityCam &calib = (MCalibrationIntensityCam&)object;

    MParContainer::Copy(calib);

    const UInt_t n = GetSize();
    if (n != 0)
    {
        calib.InitSize(n);
        for (UInt_t i=0; i<n; i++)
        {
            GetCam(i)->Copy(*(calib.GetCam(i)));
            GetHist(i)->Copy(*(calib.GetHist(i)));
        }
    }
}

// -----------------------------------------------------
//
// Calls Clear() for all entries fCams
//
void MCalibrationIntensityCam::Clear(Option_t *o)
{
    fCams->R__FOR_EACH(MCalibrationCam, Clear)();
    fHists->R__FOR_EACH(MHCalibrationCam, Clear)();
}

// -----------------------------------------------------
//
// Calls Print(o) for all entries fCams
//
void MCalibrationIntensityCam::Print(Option_t *o) const
{
    fCams->R__FOR_EACH(MCalibrationCam, Print)(o);
    fHists->R__FOR_EACH(MHCalibrationCam, Print)(o);
}

// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollection using the 
// virtual function Add().
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MCalibrationIntensityCam::InitSize(const UInt_t i)
{

  const UInt_t save = GetSize();

  if (i==save)
    return;
  
  if (i>save)
    Add(save,i);
}

// -------------------------------------------------------------------
//
// Add MCalibrationCams in the ranges from - to. In order to initialize
// from MCalibrationCam derived containers, overwrite this function
//
void MCalibrationIntensityCam::Add(const UInt_t from, const UInt_t to)
{
  for (UInt_t i=from; i<to; i++)
    fCams->AddAt(new MCalibrationCam,i);
}

// -------------------------------------------------------------------
//
// If size is still 0, Intialize a first Cam.
// Calls Init(geom) for all fCams
//
void MCalibrationIntensityCam::Init(const MGeomCam &geom)
{
  if (GetSize() == 0)
    InitSize(1);

  fCams->R__FOR_EACH(MCalibrationCam,Init)(geom);
}

// --------------------------------------------------------------------------
//
Int_t MCalibrationIntensityCam::CountNumEntries(const MCalibrationCam::PulserColor_t col) const
{
  
  Int_t size = 0;

  if (col == MCalibrationCam::kNONE)
    return GetSize();
  else
    for (Int_t i=0;i<GetSize();i++)
      {
        const MCalibrationCam *cam = GetCam(i);
        if (cam->GetPulserColor() == col)
          size++;
      }

  return size;
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
MCalibrationPix &MCalibrationIntensityCam::operator[](UInt_t i)
{
  return (*GetCam())[i];
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
const MCalibrationPix &MCalibrationIntensityCam::operator[](UInt_t i) const 
{
  return (*GetCam())[i];
}

// --------------------------------------------------------------------------
//
// Get camera with name 'name' 
//
/*
MCalibrationCam *MCalibrationIntensityCam::GetCam(const char *name)
{
  return static_cast<MCalibrationCam*>(fCams->FindObject(name));
}

// --------------------------------------------------------------------------
//
// Get camera with name 'name' 
//
const MCalibrationCam *MCalibrationIntensityCam::GetCam(const char *name) const
{
  return static_cast<MCalibrationCam*>(fCams->FindObject(name));
}
*/
// --------------------------------------------------------------------------
//
// Get i-th histogram class 
//
MHCalibrationCam *MCalibrationIntensityCam::GetHist(Int_t i)
{
  return static_cast<MHCalibrationCam*>(i==-1 ? fHists->Last() : fHists->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th histogram class 
//
const MHCalibrationCam *MCalibrationIntensityCam::GetHist(Int_t i) const 
{
  return static_cast<MHCalibrationCam*>(i==-1 ? fHists->Last() : fHists->At(i));
}

// --------------------------------------------------------------------------
//
// Get histogram class with name 'name' 
//
MHCalibrationCam *MCalibrationIntensityCam::GetHist(const char *name )
{
    return static_cast<MHCalibrationCam*>(fHists->FindObject(name));
}

// --------------------------------------------------------------------------
//
// Get histogram class with name 'name' 
//
const MHCalibrationCam *MCalibrationIntensityCam::GetHist(const char *name ) const 
{
  return static_cast<MHCalibrationCam*>(fHists->FindObject(name));
}

// --------------------------------------------------------------------------
//
// Calls DrawPixelContent for the current entry in fCams
//
void MCalibrationIntensityCam::DrawPixelContent( Int_t num ) const
{
  return GetCam()->DrawPixelContent(num);
}
