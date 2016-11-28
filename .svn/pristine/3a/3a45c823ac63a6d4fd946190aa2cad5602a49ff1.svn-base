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
!   Author(s): Markus Gaug   08/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                     
// MCalibrationTestPix                                     
//                                                     
// Base Storage container for the test result of a calibration pixel. 
// Holds calibrated number of photons, number of photons per area (mm^2) and 
// their errors. Not interpolateable pixels are declared excluded.
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationTestPix.h"

ClassImp(MCalibrationTestPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor: 
//
// Sets:
// - fPixId to -1
//
// Calls:
// - Clear()
//
MCalibrationTestPix::MCalibrationTestPix(const char *name, const char *title)
    : fPixId(-1)
{

  fName  = name  ? name  : "MCalibrationTestPix";
  fTitle = title ? title : "Container of the fit results of MHCalibrationTestPix ";

  Clear();

}

// ------------------------------------------------------------------------
//
// Sets:
// - all data members to -1
// - all excluded to kFALSE
//
void MCalibrationTestPix::Clear(Option_t *o)
{

  fNumPhotons           =  -1.;
  fNumPhotonsErr        =  -1.;
  fNumPhotonsPerArea    =  -1.;
  fNumPhotonsPerAreaErr =  -1.;

  SetExcluded          ( kFALSE );
}

