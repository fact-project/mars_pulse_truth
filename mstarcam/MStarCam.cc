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
! * in supporting documentation. It is provided "as is" without expressed
! * or implied warranty.
! *
!
!
!   Author(s): Javier Lopez 04/2004 <mailto:jlopez@ifae.es>
!   Author(s): Jordi Albert 04/2004 <mailto:albert@astro.uni-wuerzburg.de>
!   Author(s): Robert Wagner 08/2004 <mailto:rwagner@mppmu.mpg.de>
!                 
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MStarCam                                                                //
//                                                                         //
// A Container to hold star positions in the camera                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "MStarCam.h"

#include <TList.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MStarPos.h"

using namespace std;

// --------------------------------------------------------------------------
// Default constructor. 
//
//
MStarCam::MStarCam(const char *name, const char *title) 
{
  fName  = name  ? name  : "MStarCam";
  fTitle = title ? title : "";
  
  fStars = new TList;

  fInnerPedestalDC = 0.;
  fOuterPedestalDC = 0.;
  fInnerPedestalRMSDC = 0.;
  fOuterPedestalRMSDC = 0.;
  
}

MStarCam::~MStarCam()
{
    fStars->SetOwner();
    fStars->Delete();
}

// --------------------------------------------------------------------------
//
// Get i-th
//
MStarPos &MStarCam::operator[] (Int_t i)
{
  MStarPos& star = *static_cast<MStarPos*>(fStars->At(i));
  return star;
}

// --------------------------------------------------------------------------
//
// Get i-th
//
const MStarPos &MStarCam::operator[] (Int_t i) const
{
    return *static_cast<MStarPos*>(fStars->At(i));
}

void MStarCam::Paint(Option_t *o)
{
	TIter Next(fStars);
	MStarPos* star;
	while ((star=(MStarPos*)Next())) 
	    star->Paint(o);
}

void MStarCam::Print(Option_t *o) const
{
      *fLog << inf << "DCs baseline:" << endl;
      *fLog << inf << " Inner Pixels Mean pedestal \t" << setw(4) << fInnerPedestalDC << " uA  and RMS " << setw(4) << fInnerPedestalRMSDC << " uA for DCs" << endl;
      *fLog << inf << " Outer Pixels Mean pedestal \t" << setw(4) << fOuterPedestalDC << " uA  and RMS " << setw(4) << fOuterPedestalRMSDC << " uA for DCs" << endl;
      
      TIter Next(fStars);
      MStarPos* star;
      UInt_t starnum = 0;
      while ((star=(MStarPos*)Next())) 
        {
          *fLog << inf << "Star[" << starnum << "] info:" << endl;
          star->Print(o);
          starnum++;
        }
}
