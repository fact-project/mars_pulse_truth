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
// MCalibrationPedCam                                               
//                                                               
// Hold the pedestal Calibration results obtained from MHPedestalCam of the camera:
//                                                               
// The calculated values (types of GetPixelContent) are:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Pedestal
// 1: Error of fitted Pedestal
// 2: Sigma of fitted Pedestal
// 3: Error of Sigma of fitted Pedestal
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to Pedestal distribution
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationPedCam.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MCalibrationPix.h"

ClassImp(MCalibrationPedCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Creates a TClonesArray of MCalibrationPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel. Later, a call to MCalibrationRelTimeCam::InitSize() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibrationPix containers, initialized to 1 entry, destinated 
// to hold one container per pixel AREA. Later, a call to MCalibrationRelTimeCam::InitAreas() 
// has to be performed (in MGeomApply). 
//
// Creates a TClonesArray of MCalibrationPix containers, initialized to 1 entry, destinated
// to hold one container per camera SECTOR. Later, a call to MCalibrationRelTimeCam::InitSectors() 
// has to be performed (in MGeomApply). 
//
MCalibrationPedCam::MCalibrationPedCam(const char *name, const char *title)
{
    fName  = name  ? name  : "MCalibrationPedCam";
    fTitle = title ? title : "Storage container for the Pedestal Calibration Results in the camera";

}

// --------------------------------------------------------------------------
//
// The calculated values (types of GetPixelContent) are:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Pedestal
// 1: Error of fitted Pedestal
// 2: Sigma of fitted Pedestal
// 3: Error of Sigma of fitted Pedestal
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to Pedestal distribution
//
Bool_t MCalibrationPedCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx > GetSize())
    return kFALSE;

  Float_t area = cam[idx].GetA();

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
