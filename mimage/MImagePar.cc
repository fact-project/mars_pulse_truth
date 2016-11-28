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
!   Author(s): Thomas Bretz, 8/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MImagePar
//
// Storage Container for new image parameters
//
//  Class Version 2:
//  ----------------
//    - added Short_t fNumSinglePixels;
//    - added Float_t fSizeSinglePixels;
//    - added Float_t fSizeSubIslands;
//
//  Class Version 1:
//  ----------------
//    Short_t fNumIslands;           // number of islands found
//
//    Short_t fNumHGSaturatedPixels; // number of pixels with saturating hi-gains
//    Short_t fNumSaturatedPixels;   // number of pixels with saturating lo-gains
//
/////////////////////////////////////////////////////////////////////////////
#include "MImagePar.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MSignalCam.h"

ClassImp(MImagePar);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MImagePar::MImagePar(const char *name, const char *title)
{
    fName  = name  ? name  : "MImagePar";
    fTitle = title ? title : "New image parameters";

    Reset();
}

// --------------------------------------------------------------------------
//
void MImagePar::Reset()
{
    fNumIslands       = -1;
    fNumSinglePixels  = -1;

    fNumSatPixelsHG   = -1;
    fNumSatPixelsLG   = -1;

    fSizeSinglePixels = -1;
    fSizeSubIslands   = -1;
    fSizeMainIsland   = -1;
}

// --------------------------------------------------------------------------
//
//  Calculation of new image parameters
//
void MImagePar::Calc(const MSignalCam &evt)
{
    // Get number of saturating pixels
    fNumSatPixelsHG   = evt.GetNumPixelsSaturatedHiGain();
    fNumSatPixelsLG   = evt.GetNumPixelsSaturatedLoGain();

    // Get number of islands
    fNumIslands       = evt.GetNumIslands();
    fNumSinglePixels  = evt.GetNumSinglePixels();
    fSizeSinglePixels = evt.GetSizeSinglePixels();
    fSizeSubIslands   = evt.GetSizeSubIslands();
    fSizeMainIsland   = evt.GetSizeMainIsland();

    SetReadyToSave();
}

// --------------------------------------------------------------------------
//
void MImagePar::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Num Islands      [#] = " << fNumIslands       << " Islands" << endl;
    *fLog << " - Sat.Pixels (HG)  [#] = " << fNumSatPixelsHG   << " Pixels" << endl;
    *fLog << " - Sat.Pixels (LG)  [#] = " << fNumSatPixelsLG   << " Pixels" << endl;
    *fLog << " - No rmvd CorePx   [#] = " << fNumSinglePixels  << " Pixels" << endl;
    *fLog << " - Sz rmvd CorePx [phe] = " << fSizeSinglePixels << endl;
    *fLog << " - Sz Sub Islands [phe] = " << fSizeSubIslands   << endl;
    *fLog << " - Sz Main Island [phe] = " << fSizeMainIsland   << endl;
}
