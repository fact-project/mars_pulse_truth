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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */


/////////////////////////////////////////////////////////////////////////////
//
// MSignalPix
//
// Storage container for the signal in a pixel in number of photons.
//
// NOTE: This container is NOT ment for I/O. Write it to a file on your
//       own risk!
//
// fIsSaturated: boolean variable set to kTRUE whenever one or more of
//               the low gain FADC slices of the pixel is in saturation.
//
// Version 2:
// ----------
//  - added fIsSaturated
//
// Version 4:
// ----------
//  - added fIsHGSaturated
//
// Version 5:
// ----------
//  - added fIdxIsland
//
// Version 6:
// ----------
//  - put the '!' into the comment line for
//      Bool_t   fIsCore;        //! the pixel is a Core pixel -> kTRUE
//      Short_t  fRing;          //! NT: number of analyzed rings around the core pixels, fRing>0 means: used, fRing= 0 means: unused, fRing= -1 means: unmapped (no possible to use in the calculation of the image parameters)
//      Short_t  fIdxIsland;     //! the pixel is a Core pixel -> kTRUE
//      Bool_t   fIsHGSaturated; //! the pixel's high gain is saturated
//    This is a queick hack to gain storage space - the structure of
//    the container for calibrated data will change soon.
//
// Version 7:
// ----------
//  - removed '!' from fRing to allow the status 'Unmapped' to be stored
//    after calibration (bad pixel treatment). This increases the file
//    size of the calibrated data by roughly 0.5%
//
// Version 8:
// ----------
//  - added new time variable fTimeSlope describing the width of the rise time
//
////////////////////////////////////////////////////////////////////////////
#include "MSignalPix.h"

#include "MLog.h"

ClassImp(MSignalPix);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. The pixel is assumed as used and not a core pixel.
// NT 29/04/2003: A pixel is considered used when fRing > 0.
//
MSignalPix::MSignalPix(Float_t phot, Float_t errphot) :
    fIsCore(kFALSE), fRing(1), fIdxIsland(-1),
    fPhot(phot), fErrPhot(errphot), fArrivalTime(-1),
    fTimeSlope(-1)
{
    MMath::ReducePrecision(fPhot);
    MMath::ReducePrecision(fErrPhot);
} 

void MSignalPix::Clear(Option_t *o)
{
    fIsCore      = kFALSE;
    fRing        =  1;
    fIdxIsland   = -1;
    fPhot        =  0;
    fErrPhot     =  0;
    fArrivalTime = -1;
    fTimeSlope = -1;
}

// --------------------------------------------------------------------------
//
//  Print information to gLog.
//
void MSignalPix::Print(Option_t *) const
{ 
    gLog << GetDescriptor();// << " Pixel: "<< fPixId;
    switch (fRing)
    {
    case -1:
        gLog << "Unampped";
        break;
    case 0:
        gLog << " Unused ";
        break;
    default:
        gLog << "  Used  ";
        break;
    }
    gLog << (fIsCore?" Core ":"      ");
    gLog << "Nphot= " << fPhot << " Error(Nphot)=" << fErrPhot << endl;
}
