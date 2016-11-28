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
!   Author(s): Ester Aliu ,  9/2004 <mailto:aliu@ifae.es>
!             
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
#include "MImgIsland.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MImgIsland);

using namespace std;

MImgIsland::MImgIsland(const char *name, const char *title)
{

    fName  = name  ? name  : "MImgIsland";
    fTitle = title ? title : "Storage container for one island information";
    //    cout << "Creo 1 isla" << endl;      
    Reset();
}
MImgIsland::~MImgIsland()
{
  //  cout << "Destruyo 1 isla" << endl;
}

void MImgIsland::Reset()
{
  fPixNum = 0;
  fSigToNoise = -1;
  fTimeSpread = -1;
  fMeanX = -1000;
  fMeanY = -1000;
  fDist = -1;
  fDistW = -1;
  fDistL = -1;
  fDistS = -1;
 
  fWidth = -1;
  fLength = -1;
  fSizeIsl = -1;
  fAlpha = -1000;
  
  fPixList.Reset(-1);
  fPeakPulse.Reset(-1);
   
}

// -------------------------------------------------------------------------
// 
// Initialize
// 
void MImgIsland::InitSize(Int_t i)
{
  fPixList.Set(i);
  fPeakPulse.Set(i);
}


// -------------------------------------------------------------------------
// 
// Set the pixels id in the island
//
void MImgIsland::SetPixList(const Int_t i, const Int_t idx)
{
    fPixList[i] = idx;   
}




// -------------------------------------------------------------------------
// 
// Set the arrival time in one pixel of the island
//
void MImgIsland::SetPeakPulse(const Int_t i, const Float_t t)
{
    fPeakPulse[i] = t;
}


// -------------------------------------------------------------------------
// 
// Print the island information
//
void MImgIsland::Print(Option_t *opt) const
{

  // TString o = opt;

  *fLog << inf << "  Number of pixels = " << fPixNum << endl;
  *fLog << inf << "  Signal-To-Noise ="  << fSigToNoise << endl;
  *fLog << inf << "  Time Spread (Core pixels) = " << fTimeSpread << endl;
  *fLog << inf << "  DistL = " << fDistL << endl;
  *fLog << inf << "  DistW = " << fDistW << endl;
  *fLog << inf << "  DistS = " << fDistS << endl;
  *fLog << inf << "  Alpha = " << fAlpha << endl;

}
      





