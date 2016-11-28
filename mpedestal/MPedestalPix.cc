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
!   Author(s): Thomas Bretz   12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Markus Gaug    04/2004 <mailto:markus@ifae.es>
!   Author(s): Florian Goebel 06/2004 <mailto:fgoebel@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MPedestalPix
//
// This is the storage container to hold informations about the pedestal
// (offset) value of one Pixel (PMT).
//
//  Float_t fPedestal:
//   - mean value of pedestal (PMT offset)
//  Float_t fPedestalRms:
//   - root mean square / sigma  / standard deviation of pedestal
//  Float_t fPedestalABoffset:
//   - the difference between odd slice pedestal mean and the
//     total pedestal mean (fPedestal). For even slices pedestal
//     use -fPedestalABoffset.
//  UInt_t  fNumEvents:
//   - number of times, the Process was executed (to estimate the error
//     of pedestal)
//
// version 2:
// ----------
// added:
//  fPedestalABoffset   difference between pedestal mean of odd slices and
//                      the total pedestal mean (fPedestal)
//  fNumEvents          number of times, the Process was executed
//                      (to estimate the error of pedestal)
//
// version 3:
// ----------
// - fValid removed
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedestalPix.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MPedestalPix);

using namespace std;

// ------------------------------------------------------------------------
//
// Default constructor. Calls Clear()
//
MPedestalPix::MPedestalPix()
{
    Clear();
}

// ------------------------------------------------------------------------
//
//  return fNumEvents>0 ? fPedestalRms/TMath::Sqrt((Float_t)fNumEvents)   : 0;
//
Float_t MPedestalPix::GetPedestalError() const
{
    return fNumEvents>0 ? fPedestalRms/TMath::Sqrt((Float_t)fNumEvents)   : 0;
}

// ------------------------------------------------------------------------
//
// return fNumEvents>0 ? fPedestalRms/TMath::Sqrt((Float_t)fNumEvents*2) : 0;
//
Float_t MPedestalPix::GetPedestalRmsError() const
{
    return fNumEvents>0 ? fPedestalRms/TMath::Sqrt((Float_t)fNumEvents*2) : 0;
}

void MPedestalPix::Print(Option_t *) const
{
    *fLog << all << "P=" << fPedestal;
    if (fPedestalRms>=0)
        *fLog << " RMS=" << fPedestalRms;
    if (fPedestalABoffset>=0)
        *fLog << " AB=" << fPedestalABoffset;
    if (fNumEvents>0)
        *fLog << " (N=" << fNumEvents << ")";
    *fLog << endl;
}

// ------------------------------------------------------------------------
//
// Invalidate values
//
void MPedestalPix::Clear(Option_t *o)
{
    fPedestal         = -1;
    fPedestalRms      = -1;
    fPedestalABoffset = -1;
    fNumEvents        =  0;
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MPedestalPix::Copy(TObject &object) const
{

  MPedestalPix &pix =  (MPedestalPix&)object;

  pix.fPedestal         = fPedestal         ;
  pix.fPedestalRms      = fPedestalRms      ;
  pix.fPedestalABoffset = fPedestalABoffset ;
  pix.fNumEvents        = fNumEvents;
}

// ------------------------------------------------------------------------
//
// Set all values to 0
//
void MPedestalPix::InitUseHists()
{
    fPedestal         = 0;
    fPedestalRms      = 0;
    fPedestalABoffset = 0;
    fNumEvents        = 0;
}

// ------------------------------------------------------------------------
//
// Set fPedestal=m, fPedestalRms=r, fPedestalABoffset=offs, fNumEvents=n
//
void MPedestalPix::Set(Float_t m, Float_t r, Float_t offs, UInt_t n)
{
    fPedestal         = m;
    fPedestalRms      = r;
    fPedestalABoffset = offs;
    fNumEvents        = n;
}

// ------------------------------------------------------------------------
//
// Return kTRUE if pedestal rms is valid (>=0)
//
Bool_t MPedestalPix::IsValid() const
{
    return fPedestalRms>=0;
}
