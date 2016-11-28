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
   !   Author(s): Abelardo Moralejo, 4/2004 <mailto:moralejo@pd.infn.it>
   !
   !   Copyright: MAGIC Software Development, 2000-2004
   !
   !
   \* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSignal3
//
//  Calculate the signal integrating fWindowSize time slices, the same for 
//  all pixels. The integrated slices change from event to event, since the
//  pulse positions in the FADC jump between events, but apparently in a 
//  "coherent" fashion. We first loop over all pixels and find the one 
//  which has the highest sum of fPeakSearchWindowSize (default: 4) consecutive 
//  non-saturated high gain slices. The position of the peak in this pixel 
//  determines the integration window position for all pixels of this event. 
//  For the moment we neglect time delays between pixels (which are in most 
//  cases small). The class is based on MExtractSignal2.
//
//////////////////////////////////////////////////////////////////////////////

#include "MExtractSignal3.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MExtractSignal3);

using namespace std;

const Byte_t MExtractSignal3::fgSaturationLimit = 254;
const Byte_t MExtractSignal3::fgFirst  =  0;
const Byte_t MExtractSignal3::fgLast   = 14;
const Byte_t MExtractSignal3::fgWindowSize = 6;
const Byte_t MExtractSignal3::fgPeakSearchWindowSize = 4;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MExtractSignal3::MExtractSignal3(const char *name, const char *title)
  : fNumHiGainSamples(15), fNumLoGainSamples(15), fSaturationLimit(fgSaturationLimit)
{

  fName  = name  ? name  : "MExtractSignal3";
  fTitle = title ? title : "Task to extract the signal from the FADC slices";

  AddToBranchList("MRawEvtData.*");

  SetWindows();
}

void MExtractSignal3::SetWindows(Byte_t windowh, Byte_t windowl, Byte_t peaksearchwindow)
{
  //
  // Set windows to even number of slices due to clock noise (odd/even slice effect).
  //
  fWindowSizeHiGain = windowh & ~1;
  fWindowSizeLoGain = windowl & ~1;
  fPeakSearchWindowSize = peaksearchwindow & ~1;


  if (fWindowSizeHiGain != windowh)
    *fLog << endl << warn << 
      "MExtractSignal3::SetWindows - Hi Gain window size has to be even, set to: " 
	  << int(fWindowSizeHiGain) << " samples " << endl;
    
  if (fWindowSizeLoGain != windowl)
    *fLog << endl << warn << 
      "MExtractSignal3::SetWindows - Lo Gain window size has to be even, set to: " 
	  << int(fWindowSizeLoGain) << " samples " << endl;

  if (fPeakSearchWindowSize != peaksearchwindow)
    *fLog << endl << warn << 
      "MExtractSignal3::SetWindows - Peak Search window size has to be even, set to: " 
	  << int(fPeakSearchWindowSize) << " samples " << endl;


  if (fWindowSizeHiGain<2) 
    {
      fWindowSizeHiGain = 2;
      *fLog << warn << "MExtractSignal3::SetWindows - Hi Gain window size set to two samples" << endl;
    }

  if (fWindowSizeLoGain<2) 
    {
      fWindowSizeLoGain = 2;
      *fLog << warn << "MExtractSignal3::SetWindows - Lo Gain window size set to two samples" << endl;
    }

  if (fPeakSearchWindowSize<2) 
    {
      fPeakSearchWindowSize = 2;
      *fLog << warn << "MExtractSignal3::SetWindows - Peak Search window size set to two samples" << endl;
    }


  if (fWindowSizeHiGain > fNumHiGainSamples)
    {
      fWindowSizeHiGain = fNumHiGainSamples & ~1;
      *fLog << warn << "MExtractSignal3::SetWindows - Hi Gain window size set to " 
            << int(fWindowSizeHiGain) << " samples " << endl;
    }
  
  if (fWindowSizeLoGain > fNumLoGainSamples)
    {
      fWindowSizeLoGain = fNumLoGainSamples & ~1;
      *fLog << warn << "MExtractSignal3::SetWindows - Lo Gain window size set to " 
            << int(fWindowSizeLoGain) << " samples " << endl;
    }

  fWindowSqrtHiGain = TMath::Sqrt((Float_t)fWindowSizeHiGain);
  fWindowSqrtLoGain = TMath::Sqrt((Float_t)fWindowSizeLoGain);

}


// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedSignalCam
//
Int_t MExtractSignal3::PreProcess(MParList *pList)
{
  fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
  if (!fRawEvt)
    {
      *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
      return kFALSE;
    }

  fSignals = (MExtractedSignalCam*)pList->FindCreateObj(AddSerialNumber("MExtractedSignalCam"));
  if (!fSignals)
    return kFALSE;

  //
  // FIXME? We should keep track in MExtractedSignalCam of the signal extraction method used.
  //
  fSignals->SetUsedFADCSlices(0, fNumHiGainSamples-1, (Float_t)fWindowSizeHiGain,
			      0, fNumLoGainSamples-1, (Float_t)fWindowSizeLoGain);

  fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
  if (!fPedestals)
    {
      *fLog << err << AddSerialNumber("MPedestalCam") << " not found... aborting" << endl;
      return kFALSE;
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// FindPeak
// Finds highest sum of "window" consecutive FADC slices in a pixel, and store
// in "startslice" the first slice of the group which yields the maximum sum.
// In "max" the value of the maximum sum is stored, in "sat" the number of 
// saturated slices.
//
void MExtractSignal3::FindPeak(Byte_t *ptr, Byte_t window, Byte_t &startslice, 
			       Int_t &max, Int_t &sat) const
{
  const Byte_t *end = ptr + fNumHiGainSamples;

  Int_t sum=0;

  //
  // Calculate the sum of the first "window" slices
  //
  sat = 0;
  Byte_t *p = ptr;

  while (p<ptr+window)
    {
      sum += *p;
      if (*p++ >= fSaturationLimit)
	sat++;
    }

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;

  //
  // Calculate the i-th sum of n as
  //    sum_i+1 = sum_i + slice[i+window] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  max=sum;
  for (p=ptr; p+window<end; p++)
    {
      sum += *(p+window) - *p;
      if (sum > max)
	{
	  max = sum;
	  startslice = p-ptr;
	}
    }

  return;
}

// --------------------------------------------------------------------------
//
// FindSignal
// Adds up "window" slices starting in slice to which "ptr" points. The result 
// is stored in "sum", and the number of saturated  slices in "sat".
//
void MExtractSignal3::FindSignal(Byte_t *ptr, Byte_t window, Int_t &sum, Int_t &sat) const
{
  //
  // Calculate the sum of the "window" slices starting in ptr
  //
  sum=0;
  sat = 0;
  Byte_t *p = ptr;

  while (p<ptr+window)
    {
      sum += *p;
      if (*p++ >= fSaturationLimit)
	sat++;
    }

  return;
}

// --------------------------------------------------------------------------
//
// Process
// First find the pixel with highest sum of fPeakSearchWindowSize slices (default:4)
// "startslice" will mark the slice at which the highest sum begins for that pixel.
// Then define the beginning of the integration window for ALL pixels as the slice
// before that: startslice-1 (this is somehow arbitrary), unless of course startslice=0,
// in which case we start at 0. We will also check that the integration window does not 
// go beyond the FADC limits.
//
Int_t MExtractSignal3::Process()
{
  MRawEvtPixelIter pixel(fRawEvt);

  Int_t sat;
  Int_t maxsumhi = 0;
  Byte_t startslice;
  Byte_t hiGainFirst = 0;
  Byte_t loGainFirst = 0;

  while (pixel.Next())
    {
      Int_t sumhi;
      sat = 0;

      FindPeak(pixel.GetHiGainSamples(), fPeakSearchWindowSize, startslice, sumhi, sat);
      if (sumhi > maxsumhi && sat == 0 )
	{
	  maxsumhi = sumhi;
	  if (startslice > 0)
	    hiGainFirst = startslice-1;
	  else
	    hiGainFirst = 0;
	}
    }


  loGainFirst = hiGainFirst;

  // Make sure we will not integrate beyond the hi gain limit:
  if (hiGainFirst+fWindowSizeHiGain > pixel.GetNumHiGainSamples())
    hiGainFirst = pixel.GetNumHiGainSamples()-fWindowSizeHiGain;

  // Make sure we will not integrate beyond the lo gain limit:
  if (loGainFirst+fWindowSizeLoGain > pixel.GetNumLoGainSamples())
    loGainFirst = pixel.GetNumLoGainSamples()-fWindowSizeLoGain;

  pixel.Reset();
  fSignals->Clear();

  sat = 0;
  while (pixel.Next())
    {
      //
      // Find signal in hi- and lo-gain
      //
      Int_t sumhi, sathi;

      FindSignal(pixel.GetHiGainSamples()+hiGainFirst, fWindowSizeHiGain, sumhi, sathi);

      Int_t sumlo=0;
      Int_t satlo=0;
      if (pixel.HasLoGain())
        {
	  FindSignal(pixel.GetLoGainSamples()+loGainFirst, fWindowSizeLoGain, sumlo, satlo);
	  if (satlo)
	    sat++;
        }

      //
      // Take corresponding pedestal
      //
      const Int_t pixid = pixel.GetPixelId();

      const MPedestalPix  &ped = (*fPedestals)[pixid];
      MExtractedSignalPix &pix = (*fSignals)[pixid];

      const Float_t pedes  = ped.GetPedestal();
      const Float_t pedrms = ped.GetPedestalRms();

      //
      // Set extracted signal with pedestal substracted
      //
      pix.SetExtractedSignal(sumhi - pedes*fWindowSizeHiGain, pedrms*fWindowSqrtHiGain,
			     sumlo - pedes*fWindowSizeLoGain, pedrms*fWindowSqrtLoGain);

      pix.SetGainSaturation(sathi, satlo);
    } /* while (pixel.Next()) */

  //
  // Print a warning if event has saturationg lo-gains
  //
  if (sat)
    *fLog << warn << "WARNING - Lo Gain saturated in " << sat << " pixels." << endl;

  fSignals->SetReadyToSave();

  return kTRUE;
}
