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
!   Author(s): Markus Gaug   02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MCalibrationRelTimePix                                                  //
//                                                                         //
// Storage container for relative arrival time calibration results         // 
// of one Pixel (PMT).                                                     //
// The following "calibration" constants can be retrieved:                 //
// - GetTimeOffset(): The mean offset in relative times, 
//   has to be added to any calculated relative time in the camera. 
// - GetTimePrecision(): The Gauss sigma of histogrammed relative arrival 
//   times for the calibration run. Gives an estimate about the timing 
//   resolution.
//
// ALL RELATIVE TIMES HAVE TO BE CALCULATED W.R.T. PIXEL IDX 1 
// (HARDWARE NUMBER: 2) !!
//
// See also: MHCalibrationRelTimePix, MHCalibrationRelTimeCam              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationRelTimePix.h"

ClassImp(MCalibrationRelTimePix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MCalibrationRelTimePix::MCalibrationRelTimePix(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationRelTimePix";
  fTitle = title ? title : "Results of MHCalibrationRelTimePix ";

}

