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
!   Author(s): Oscar Blanch 1/2005 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */


/////////////////////////////////////////////////////////////////////////////
//
// MGainFluctuationPix
//
// Storage container for the gain fluctuation in a pixel in relative value.
//
////////////////////////////////////////////////////////////////////////////
#include "MGainFluctuationPix.h"

#include "MLog.h"

ClassImp(MGainFluctuationPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MGainFluctuationPix::MGainFluctuationPix(Int_t pix, Float_t gain):
  fPixId(pix), fGain(gain)
{
} 


// --------------------------------------------------------------------------
//
//  Print information to gLog.
//
void MGainFluctuationPix::Print(Option_t *) const
{ 
    gLog << GetDescriptor() <<" Pixel: "<< fPixId;
    gLog << "Relative gain " << fGain << endl;
}
