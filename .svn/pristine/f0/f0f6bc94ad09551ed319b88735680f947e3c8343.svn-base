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
!   Author(s): Markus Gaug   07/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationBlindCam                                               
//                                                               
// Base class for Blind Pixels Calibration results. Derived classes intialize
// the actual values of the MCalibrationBlindPix's. 
//
// Contains TOrdCollections for the following objects:
// - fPixels:    Array of classes derived from MCalibrationBlindPix, one entry 
//               per blind pixel. 
//
// All TOrdCollections have to enlarged by the corresponding calls to (e.g. in MGeomApply): 
// - InitSize()
//
// See also: MCalibrationBlindCamOneOldStyle, MCalibrationBlindCamTwoNewStyle
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationBlindCam.h"
#include "MCalibrationBlindPix.h"

#include <TOrdCollection.h>

#include "MLogManip.h"

ClassImp(MCalibrationBlindCam);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationBlindCam::MCalibrationBlindCam(Int_t nblind,const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationBlindCam";
  fTitle = title ? title : "Calibration Information of blinded pixels in camera";

  InitSize(nblind);

}

void MCalibrationBlindCam::Add(const UInt_t a, const UInt_t b)
{

  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationBlindPix,i);

}

// --------------------------------------------------------------------------
//
// Return true, if any of the blind pixels have an available photon flux
//
Bool_t MCalibrationBlindCam::IsFluxInsidePlexiglassAvailable() const
{
  for (Int_t i=0; i<GetSize(); i++)
    {
      MCalibrationBlindPix &bp = (MCalibrationBlindPix&)(*this)[i];      
      if (bp.IsFluxInsidePlexiglassAvailable())
        return kTRUE;
    }
  
  return kFALSE;
}

// --------------------------------------------------------------------------
//
// Returns weighted average of the flux calculated by each blind pixel
//
Float_t MCalibrationBlindCam::GetFluxInsidePlexiglass() const
{

  Float_t flux    = 0.;
  Float_t fluxvar = 0.;  

  for (Int_t i=0; i<GetSize(); i++)
    {
      MCalibrationBlindPix &bp = (MCalibrationBlindPix&)(*this)[i];
      if (bp.IsFluxInsidePlexiglassAvailable())
        {
          const Float_t weight = 1./ 
                                bp.GetFluxInsidePlexiglassErr() 
                                / bp.GetFluxInsidePlexiglassErr();
          flux    += weight * bp.GetFluxInsidePlexiglass();
          fluxvar += weight;
        }
    }
  return fluxvar > 0.0001 ? flux / fluxvar : -1.;
}

// --------------------------------------------------------------------------
//
// Returns weighted variance of the flux calculated by each blind pixel
//
Float_t MCalibrationBlindCam::GetFluxInsidePlexiglassVar() const
{

  Float_t fluxvar = 0.;  

  for (Int_t i=0; i<GetSize(); i++)
    {
      MCalibrationBlindPix &bp = (MCalibrationBlindPix&)(*this)[i];
      if (bp.IsFluxInsidePlexiglassAvailable())
        {
          const Float_t weight = 1./ 
                                bp.GetFluxInsidePlexiglassErr() 
                                / bp.GetFluxInsidePlexiglassErr();
          fluxvar += weight;
        }
    }
  return fluxvar > 0.0001 ? 1. / fluxvar : -1.;
}

// --------------------------------------------------------------------------
//
// Returns weighted rel. variance of the flux calculated by each blind pixel
//
Float_t MCalibrationBlindCam::GetFluxInsidePlexiglassRelVar() const
{

  Float_t flux    = 0.;
  Float_t fluxvar = 0.;  

  for (Int_t i=0; i<GetSize(); i++)
    {
      MCalibrationBlindPix &bp = (MCalibrationBlindPix&)(*this)[i];
      if (bp.IsFluxInsidePlexiglassAvailable())
        {
          const Float_t weight = 1./ 
                                bp.GetFluxInsidePlexiglassErr() 
                                / bp.GetFluxInsidePlexiglassErr();
          flux    += weight * bp.GetFluxInsidePlexiglass();
          fluxvar += weight;
        }
    }

  return fluxvar > 0.0001 ? flux * fluxvar : -1.;
}


// --------------------------------------------------------------------------
//
// Set color to this class and to the MCalibrationBlindPix's
//
void  MCalibrationBlindCam::SetPulserColor ( const MCalibrationCam::PulserColor_t col )
{

  fPulserColor = col;
  fPixels->R__FOR_EACH(MCalibrationBlindPix, SetColor)(col);

}
