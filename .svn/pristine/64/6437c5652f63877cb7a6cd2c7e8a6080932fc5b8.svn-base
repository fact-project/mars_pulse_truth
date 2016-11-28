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
!   Author(s): Oscar Blanch 11/2002 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCorsikaCT
//
// This is the base class of the PMT characteristics. 
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCorsikaCT.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MGeomCorsikaCT);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes a Telescope geometry.
//
MGeomCorsikaCT::MGeomCorsikaCT(const char *name, const char *title)
{
    fName  = name  ? name  : "MGeomCorsikaCT";
    fTitle = title ? title : "Storage container for CT Telescope characteristics";
}

// --------------------------------------------------------------------------
//
// It fills the member variable of this class
//
void MGeomCorsikaCT::Fill(Float_t ctx, Float_t cty, Float_t ctz,
			  Float_t cttheta, Float_t ctphi,
			  Float_t ctdiam, Float_t ctfocal){
  
  fCTx=ctx;
  fCTy=cty;
  fCTz=ctz;
  fCTtheta=cttheta;
  fCTphi=ctphi;
  fCTdiam=ctdiam;
  fCTfocal=ctfocal;
}
// --------------------------------------------------------------------------
//
// Print the geometry information of one pixel.
//
void MGeomCorsikaCT::Print(Option_t *opt) const
{ 
    //   information about a telescope
  *fLog << all << "x = " << fCTx << ", y = " << fCTy << ", z = " 
	<< fCTz << " cm " << endl;
}
