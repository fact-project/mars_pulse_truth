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
!   Author(s): Christoph Kolodziejski, 12/2004  <mailto:>
!   Author(s): Thomas Bretz, 12/2004  <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamMagicXT
//
// This class stores the geometry information of the 'extended' Magic
// camera. The extended geometry has also small pixels in the outer region.
// To convert MCerPhotEvt from MGeomCamMagic to MGeomCamMagicXT use
// MGeomCamMagicExtend.
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamMagicXT.h"

ClassImp(MGeomCamMagicXT);

// --------------------------------------------------------------------------
//
//  Magic camera has 577 pixels. For geometry and Next Neighbor info see
//  CreateCam and CreateNN
//
MGeomCamMagicXT::MGeomCamMagicXT(const char *name)
    : MGeomCamDwarf(19, 30, 17, name)
{
}
