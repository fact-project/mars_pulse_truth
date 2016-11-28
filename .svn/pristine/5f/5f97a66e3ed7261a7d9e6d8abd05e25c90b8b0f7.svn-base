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
// MCalibrationTestCam                                               
//                                                               
// Storage container for calibrated photons, with calibration applied on the 
// same calibration run (see MHCalibrationTestCam and MHCalibrationTestPix).
//
// See also: MCalibrationTestPix, MCalibrationTestCalc, MCalibrationQECam
//           MHCalibrationTestPix, MHCalibrationTestCam              
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationTestCam.h"
#include "MCalibrationTestPix.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

ClassImp(MCalibrationTestCam);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets all pointers to 0
// 
// Creates a TClonesArray of MCalibrationTestPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel. Later, a call to MCalibrationTestCam::InitSize() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibrationTestPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel AREA. Later, a call to MCalibrationTestCam::InitAreas() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibrationTestPix containers, initialized to 1 entry, destinated
// to hold one container per camera SECTOR. Later, a call to MCalibrationTestCam::InitSectors() 
// has to be performed (in MGeomApply). 
//
// Calls:
// - Clear()
//
MCalibrationTestCam::MCalibrationTestCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationTestCam";
  fTitle = title ? title : "Storage container for the Calibration Test Information in the camera";
  
  Clear();
}

// --------------------------------------
//
// Sets all variable to 0.
// Sets all flags to kFALSE
// Calls MCalibrationCam::Clear()
//
void MCalibrationTestCam::Clear(Option_t *o)
{

  fNumUninterpolatedInMaxCluster = 0; 

  return;
}

// -------------------------------------------------------------------
//
// Add MCalibrationTestPix's in the ranges from - to to fPixels
//
void MCalibrationTestCam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationTestPix,i);
}

// -------------------------------------------------------------------
//
// Add MCalibrationTestPix's in the ranges from - to to fAverageAreas
//
void MCalibrationTestCam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageAreas->AddAt(new MCalibrationTestPix,i);
}

// -------------------------------------------------------------------
//
// Add MCalibrationTestPix's in the ranges from - to to fAverageSectors
//
void MCalibrationTestCam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageSectors->AddAt(new MCalibrationTestPix,i);
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
void MCalibrationTestCam::InitAverageAreas(const UInt_t i)
{

  const UInt_t save = GetAverageAreas();

  if (i==save)
    return;
  
  fNumUninterpolated.Set(i);
  
  MCalibrationCam::InitAverageAreas(i);
  
  return;
  
}



// --------------------------------------------------------------------------
//
// Print first the well fitted pixels 
// and then the ones which are not FitValid
//
void MCalibrationTestCam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;
  
  *fLog << all << "Calibrated (or interpolated) pixels:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationTestPix *pix;
  while ((pix=(MCalibrationTestPix*)Next()))
    {
      
      if (!pix->IsExcluded()) 
	{                            

          *fLog << all 
                << Form("%s%3i","Pixel: ",pix->GetPixId())
		<< Form("%s%4.2f%s%4.2f","  Num.Photons: ",pix->GetNumPhotons(),"+-",pix->GetNumPhotonsErr()) 
		<< Form("%s%4.2f%s%4.2f","  Num.Photons/mm^2: ",pix->GetNumPhotonsPerArea()
                        ,"+-",pix->GetNumPhotonsPerAreaErr()) 
		<< endl;
          id++;
	}
    }
  
  *fLog << all << id << " pixels" << endl;
  id = 0;
  
   
  *fLog << all << endl;
  *fLog << all << "Not interpolated pixels:" << endl;
  *fLog << all << endl;
  
  id = 0;

  TIter Next4(fPixels);
  while ((pix=(MCalibrationTestPix*)Next4()))
  {
      if (pix->IsExcluded())
      {
        *fLog << all << pix->GetPixId() << " ";
        id++;

        if (!(id % 25))
          *fLog << endl;
      }
  }
  
  *fLog << endl;
  *fLog << all << id << " Excluded pixels " << endl;
  *fLog << endl;

  *fLog << all << endl;
  *fLog << all << "Averaged Areas:" << endl;
  *fLog << all << endl;

  TIter Next5(fAverageAreas);
  while ((pix=(MCalibrationTestPix*)Next5()))
  {
    *fLog << all << Form("%s%3i","Area Idx: ",pix->GetPixId())
          << Form("%s%4.2f%s%4.2f","  Num.Photons: ",pix->GetNumPhotons(),"+-",pix->GetNumPhotonsErr()) 
          << Form("%s%4.2f%s%4.2f","  Num.Photons/mm^2: ",pix->GetNumPhotonsPerArea()
                  ,"+-",pix->GetNumPhotonsPerAreaErr()) 
          << endl;
  }

  *fLog << all << endl;
  *fLog << all << "Averaged Sectors:" << endl;
  *fLog << all << endl;

  TIter Next6(fAverageSectors);
  while ((pix=(MCalibrationTestPix*)Next6()))
  {
    *fLog << all << Form("%s%3i","Sector: ",pix->GetPixId())
          << Form("%s%4.2f%s%4.2f","  Num.Photons: ",pix->GetNumPhotons(),"+-",pix->GetNumPhotonsErr()) 
          << Form("%s%4.2f%s%4.2f","  Num.Photons/mm^2: ",pix->GetNumPhotonsPerArea()
                  ,"+-",pix->GetNumPhotonsPerAreaErr()) 
          << endl;
  }
  *fLog << all << endl;
}


// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// 0: Number Photons
// 1: Error Number Photons
// 2: Number photons per area
// 3: Error Number Photons per area
// 4: Pixels which are not interpolateable
//
Bool_t MCalibrationTestCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx > GetSize())
    return kFALSE;

  Float_t area = cam[idx].GetA();

 if (area == 0)
    return kFALSE;

 MCalibrationTestPix &pix = (MCalibrationTestPix&)(*this)[idx];

  switch (type)
    {
    case 0:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetNumPhotons();
      break;
    case 1:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetNumPhotonsErr();
      break;
    case 2:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetNumPhotonsPerArea();
      break;
    case 3:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetNumPhotonsPerAreaErr();
      break;
    case 4:
      if (!pix.IsExcluded())
        return kFALSE;
      val = 1;
      break;
    default:
      return kFALSE;
    }

  return val!=-1.;
}

void  MCalibrationTestCam::SetNumUninterpolated( const UInt_t i, const Int_t aidx ) 
{

  if (aidx < 0)
    return;

  if (aidx < fNumUninterpolated.GetSize())
    fNumUninterpolated[aidx] = i;  
}

