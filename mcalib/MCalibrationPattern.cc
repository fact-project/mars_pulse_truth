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
!   Author(s): Nicola Galante  12/2004 <mailto:nicola.galante@pi.infn.it>
!   Author(s): Thomas Bretz  12/2004 <mailto:nicola.galante@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2004
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MCalibrationPattern
//
//  A container to store the decoded calibration pattern.
//
// The idea is, that this container will never change the meaning of its
// variables, while the calibration pattern itself could.
//
// If new 'features' are necessary the decoding (MCalibrationPatternDecode)
// must be changed to correctly decode the pattern into the existing
// MCalibrationPattern. If new information is decoded you may have to
// add new variables to this container. Don't forget to increase the
// class version number (ClassDef) and document your change HERE.
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationPattern.h"

ClassImp(MCalibrationPattern);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MCalibrationPattern::MCalibrationPattern(const char *name, const char *title)
{
    fName  = name  ? name  : "MCalibrationPattern";
    fTitle = title ? title : "Container for decoded calibration pattern";

    Reset();
}
