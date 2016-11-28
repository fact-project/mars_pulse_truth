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
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRflSinglePhoton
//
// Single Photon of a event from the reflector program
//
/////////////////////////////////////////////////////////////////////////////
#include "MRflSinglePhoton.h"

#include <TMath.h>

#include <TH2.h>
#include <TH3.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MRflSinglePhoton);

using namespace std;

// --------------------------------------------------------------------------
//
// Fill radial photon distance scaled by scale into 1D histogram.
//
void MRflSinglePhoton::FillRad(TH1 &hist, Float_t scale) const
{
    hist.Fill(TMath::Hypot(fX, fY)*scale);
}

// --------------------------------------------------------------------------
//
// Fill radial photon distance scaled by scale versus x into 2D histogram.
//
void MRflSinglePhoton::FillRad(TH2 &hist, Double_t x, Float_t scale) const
{
    hist.Fill(x, TMath::Hypot(fX, fY)*scale);
}

// --------------------------------------------------------------------------
//
// Fill photon position (x,y) scaled by scale into 2D histogram.
//
void MRflSinglePhoton::Fill(TH2 &hist, Float_t scale) const
{
    hist.Fill(fX*scale, fY*scale);
}

// --------------------------------------------------------------------------
//
// Fill photon position (x,y) scaled by scale versus z into 3D histogram.
//
void MRflSinglePhoton::Fill(TH3 &hist, Double_t z, Float_t scale) const
{
    hist.Fill(fX*scale, fY*scale);
}

// --------------------------------------------------------------------------
//
// Dump all photon information
//
void MRflSinglePhoton::Print(Option_t *o) const
{
    *fLog << all << "x=" << fX << " y=" << fY << endl;
}
