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
!   Author(s): Thomas Bretz, 5/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRflEvtHeader
//
/////////////////////////////////////////////////////////////////////////////
#include "MRflEvtHeader.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MRflEvtHeader);

using namespace std;

// --------------------------------------------------------------------------
//
MRflEvtHeader::MRflEvtHeader(const char *name, const char *title)
{
    fName  = name  ? name  : "MRflEvtHeader";
    fTitle = title ? title : "Event header information from a reflector event";
}

void MRflEvtHeader::Print(Option_t *) const
{
    *fLog << all;
    *fLog << "Event Number:                " << fEvtNumber << endl;
    *fLog << "Energy:                      " << fEnergy << "GeV" << endl;
    *fLog << "Height of first intercation: " << fHeightFirstInt << "cm" << endl;
    *fLog << "Momentum X/Y/Z:              " << fMomentum.X() << "/" << fMomentum.Y() << "/" << fMomentum.Z() << endl;
    *fLog << "Zenith/Azimuth angle:        " << fTheta*TMath::RadToDeg() << "/" << fPhi*TMath::RadToDeg() << endl;
    *fLog << endl;
}
