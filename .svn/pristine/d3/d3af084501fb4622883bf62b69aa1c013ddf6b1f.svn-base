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
!   Copyright: MAGIC Software Development, 2000-2005
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
// MCalibrationHiLoPix
//
// Storage container for high-gain vs. low-gain charge calibration results
// of one Pixel (PMT).
// The following "calibration" constants can be retrieved:
// - GetHiLoRatio(): The mean conversion High-gain vs. Low-gain
//   with which the low-gain result has to be multiplied
// - GetHiLoSigma(): The Gauss sigma of histogrammed High-gain vs.
//   Low-gain ratios
//
// See also: MHCalibrationHiLoPix, MHCalibrationHiLoCam
//
// Class Version 2:
//  + Float_t fOffsetPerSlice;  // Offset from fit (per FADC slice)
//  + Float_t fGainRatio;       // Ratio of gains from fit
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationHiLoPix.h"

ClassImp(MCalibrationHiLoPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MCalibrationHiLoPix::MCalibrationHiLoPix(const char *name, const char *title)
  : fOffsetPerSlice(-9999.), fGainRatio(-1.)
{

  fName  = name  ? name  : "MCalibrationHiLoPix";
  fTitle = title ? title : "Results of MHCalibrationHiLoPix ";

}
