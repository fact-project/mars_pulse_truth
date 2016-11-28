/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analyzing Data of imaging Cerenkov telescopes.
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
!   Author(s): Sebastian Raducci 12/2003 <mailto:raducci@fisica.uniud.it>
!              Markus Gaug       04/2004 <mailto:markus@ifae.es> 
!
!   Copyright: MAGIC Software Development, 2002-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeSpline
//
//   This is a task that calculates the arrival times of photons. 
//   It returns the absolute maximum of the spline that interpolates
//   the FADC slices 
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeSpline.h"
#include "MCubicSpline.h"

#include "MGeomCam.h"
#include "MPedestalPix.h"

#include "MLog.h"
#include "MLogManip.h"


ClassImp(MExtractTimeSpline);

using namespace std;

const Byte_t MExtractTimeSpline::fgHiGainFirst  = 0;
const Byte_t MExtractTimeSpline::fgHiGainLast   = 14;
const Byte_t MExtractTimeSpline::fgLoGainFirst  = 3;
const Byte_t MExtractTimeSpline::fgLoGainLast   = 14;
// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
MExtractTimeSpline::MExtractTimeSpline(const char *name, const char *title) 
    : fXHiGain(NULL), fXLoGain(NULL)
{

  fName  = name  ? name  : "MExtractTimeSpline";
  fTitle = title ? title : "Calculate photons arrival time using a spline";
    
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);

}

MExtractTimeSpline::~MExtractTimeSpline()
{
  
  if (fXHiGain)
    delete fXHiGain;
  if (fXLoGain)
    delete fXLoGain;
  
}


// --------------------------------------------------------------------------
//
// SetRange: 
//
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// - Deletes x, if not NULL
// - Creates x according to the range
//
void MExtractTimeSpline::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  if (fXHiGain)
    delete fXHiGain;

  if (fXLoGain)
    delete fXLoGain;
  
  Int_t range = fHiGainLast - fHiGainFirst + 1;

  if (range < 2)
    {
      *fLog << warn << GetDescriptor()
            << Form("%s%2i%s%2i%s",": Hi-Gain Extraction range [",(int)fHiGainFirst,","
                    ,fHiGainLast,"] too small, ") << endl;
      *fLog << warn << GetDescriptor()
            << " will move higher limit to obtain 4 slices " << endl;
      SetRange(fHiGainFirst, fHiGainLast+4-range,fLoGainFirst,fLoGainLast);
      range = fHiGainLast - fHiGainFirst + 1;
    }
  
  fXHiGain = new Byte_t[range+1];
  for (Int_t i=0; i<range+1; i++)
    fXHiGain[i] = i;
  
  range = fLoGainLast - fLoGainFirst + 1;

  if (range >= 2)
    {
      
      fXLoGain = new Byte_t[range+1];
      for (Int_t i=0; i<range+1; i++)
        fXLoGain[i] = i;
    }
}


// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeSpline::FindTimeHiGain(Byte_t *first, Float_t &time, Float_t &dtime, 
                                        Byte_t &sat, const MPedestalPix &ped) const
{
  
  const Int_t range = fHiGainLast - fHiGainFirst + 1;
  const Byte_t *end = first + range;
  Byte_t *p = first;

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;

  //
  // Initialize the spline
  //
  MCubicSpline spline(first,fXHiGain,kTRUE,range,0.0,0.0);  

  //
  // Now find the maximum  
  //
  Double_t abMaximum = spline.EvalAbMax();
  Double_t maximum = spline.EvalMax();
  const Double_t pedestal = ped.GetPedestal();
  const Double_t halfMax = (maximum + pedestal)/2.;
  time = (halfMax > pedestal) ? (Float_t ) spline.FindVal(halfMax,abMaximum,'l') + (Float_t)fHiGainFirst : 0.0;
}

// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeSpline::FindTimeLoGain(Byte_t *first, Float_t &time, Float_t &dtime, 
                                        Byte_t &sat, const MPedestalPix &ped) const
{
  
  const Int_t range = fLoGainLast - fLoGainFirst + 1;

  if (range < 2)
    return;

  const Byte_t *end = first + range;
  Byte_t *p = first;

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;

  //
  // Initialize the spline
  //
  MCubicSpline spline(first,fXLoGain,kTRUE,range,0.0,0.0);  

  //
  // Now find the maximum  
  //
  Double_t abMaximum = spline.EvalAbMax();
  Double_t maximum = spline.EvalMax();
  const Double_t pedestal = ped.GetPedestal();
  const Double_t halfMax = (maximum + pedestal)/2.;

  time = (halfMax > pedestal) ? (Float_t )spline.FindVal(halfMax,abMaximum,'l') + (Float_t)fLoGainFirst : 0.0;
}

