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
!   Author(s): Sebastian Raducci 12/2003 <mailto:raducci@fisica.uniud.it>
!
!   Copyright: MAGIC Software Development, 2002-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MArrivalTimeCalc
//
//   This is a task that calculates the arrival times of photons. 
//   It returns the absolute maximum of the spline that interpolates
//   the FADC slices 
//
// Input Containers:
//   MRawEvtData
//
// Output Containers:
//   MArrivalTime
//   MRawEvtData
//
//////////////////////////////////////////////////////////////////////////////
#include "MArrivalTimeCalc.h"

#include "MCubicSpline.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MRawEvtData.h"      
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MArrivalTimeCalc);

using namespace std;

const Byte_t MArrivalTimeCalc::fgSaturationLimit = 254;
// --------------------------------------------------------------------------
//
// Default constructor.
//
// 
MArrivalTimeCalc::MArrivalTimeCalc(const char *name, const char *title) 
{

    fName  = name  ? name  : "MArrivalTimeCalc";
    fTitle = title ? title : "Calculate photons arrival time";
    
    SetSaturationLimit();
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//  - MArrivalTimeCam
//

Int_t MArrivalTimeCalc::PreProcess(MParList *pList)
{

    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }
    
    fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
    if (!fPedestals)
    {
        *fLog << err << AddSerialNumber("MPedestalCam") << " not found... aborting" << endl;
        return kFALSE;
    }


    fArrTime = (MArrivalTimeCam*)pList->FindCreateObj(AddSerialNumber("MArrivalTimeCam"));
    if (!fArrTime)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluation of the mean arrival times (spline interpolation)
// per pixel and store them in the MArrivalTime container.
//
Int_t MArrivalTimeCalc::Process()
{

  MRawEvtPixelIter pixel(fRawEvt);
  
  while (pixel.Next())
    {
      
      const UInt_t idx = pixel.GetPixelId();
      Float_t time = 0.;


      //
      // If pixel is saturated we use LoGains
      //
      if ((pixel.GetMaxHiGainSample() >= fSaturationLimit) && pixel.HasLoGain())
        {
          
          const Short_t nslices = fRawEvt->GetNumLoGainSamples();
          time = Calc(pixel.GetLoGainSamples(),nslices,idx);
        }
      

      //
      // Use HiGains
      //
      else if (pixel.HasLoGain())
        {
          
          const Short_t nslices = fRawEvt->GetNumHiGainSamples();
          time = Calc(pixel.GetHiGainSamples(),nslices,idx);
        }
      
      //
      // If pixel is saturated and hasn't lo gains we do nothing, it's value remains -1
      //
      MArrivalTimePix &pix = (*fArrTime)[idx];
      pix.SetArrivalTime(time,0.);
    }
    
  fArrTime->SetReadyToSave();
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
// Possible Methods 
// Case 1: MCubicSpline (3rd order spline)
//
Float_t MArrivalTimeCalc::Calc(const Byte_t *fadcSamples, const Short_t nslices, const UInt_t idx)
{

  //
  // Initialize the spline
  //
  MCubicSpline *spline = new MCubicSpline(fadcSamples);  

  //
  // Now find the maximum  
  //
  Double_t abMaximum = spline->EvalAbMax();
  Double_t maximum = spline->EvalMax();
  const MPedestalPix &ped = (*fPedestals)[idx];
  const Double_t pedestal = ped.GetPedestal();
  const Double_t halfMax = (maximum + pedestal)/2.;
  Float_t time = (halfMax > pedestal) ? (Float_t ) spline->FindVal(halfMax,abMaximum,'l'): 0.0;
  delete spline;
  return time;
    
}

