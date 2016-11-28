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
!   Author(s): Markus Gaug 01/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz 01/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
// 
//   MExtractPedestal
//
//  Pedestal Extractor base class
//
//  Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MRawEvtHeader
//   MGeomCam
//   MPedestalCam
//
//  Output Containers:
//   MPedestalCam
//
//  This class should be used for pedestal extractors with the following facilities:
//  a) Standardized calculation of AB-noise, mean pedestals and RMS
//  b) Standardized treatment of area- and sector averaged pedestals values
//  c) Possibility to use a signal extractor to be applied on the pedestals
//  d) Possibility to handle two MPedestalCams: one for the signal extractor and 
//     a second to be filled during the pedestal calculating process.  
//
//  ad a): Every calculated value is referred to one FADC slice (e.g. Mean pedestal per slice),
//         RMS per slice. 
//         MExtractPedestal applies the following formula (1):
// 
//         Pedestal per slice = sum(x_i) / n / slices
//         PedRMS per slice   = Sqrt(  ( sum(x_i^2) - sum(x_i)^2/n ) / n-1 / slices )
//         AB-Offset per slice = (sumAB0 - sumAB1) / n / slices 
//
//         where x_i is the sum of "slices" FADC slices and sum means the sum over all
//         events. "n" is the number of events, "slices" is the number of summed FADC samples.
// 
//         Note that the slice-to-slice fluctuations are not Gaussian, but Poissonian, thus 
//         asymmetric and they are correlated.
// 
//         It is important to know that the Pedestal per slice and PedRMS per slice depend 
//         on the number of used FADC slices, as seen in the following plots:
//
//Begin_Html
/*
<img src="images/PedestalStudyInner.gif">
*/
//End_Html
//
//Begin_Html
/*
<img src="images/PedestalStudyOuter.gif">
*/
//End_Html
//
//        The plots show the inner and outer pixels, respectivly and have the following meaning:
// 
//        1) The calculated mean pedestal per slice (from MPedCalcPedRun)
//        2) The fitted mean pedestal per slice     (from MHPedestalCam)
//        3) The calculated pedestal RMS per slice  (from MPedCalcPedRun)
//        4) The fitted sigma of the pedestal distribution per slice
//                                                  (from MHPedestalCam)
//        5) The relative difference between calculation and histogram fit
//           for the mean
//        6) The relative difference between calculation and histogram fit
//           for the sigma or RMS, respectively.
// 
//        The calculated means do not change significantly except for the case of 2 slices, 
//        however the RMS changes from 5.7 per slice in the case of 2 extracted slices 
//        to 8.3 per slice in the case of 26 extracted slices. This change is very significant.
// 
// ad b)  Every calculated value is referred to one FADC slice and one (averaged) pixel,
//        (e.g. Mean Pedestal per area index per slice per pixel, etc. )
//
//         MExtractPedestal applies the following formula (2):
// 
//         Averaged Pedestal per slice = sum(x_i) / n / slices / n_pix
//         PedRMS per slice   = Sqrt(  ( sum(x_i^2) - sum(x_i)^2/n ) / n-1 / slices / n_pix )
//         AB-Offset per slice = (sumAB0 - sumAB1) / n / slices / n_pix
//
//         where x_i is the sum of "slices" FADC slices and sum means the sum over all
//         events and all concerned pixels. 
//         "n" is the number of events, "slices" is the number of summed FADC samples and
//         "n_pix" is the number of pixels belonging to the specific area index or camera sector. 
// 
//         Calculating these averaged event-by-event values is very important to trace coherent
//         fluctuations. An example is given in the following plots:
//
//Begin_Html
/*
<img src="images/PedestalOscillations.gif">
*/
//End_Html
//
//        The plots show the extracted pedestals of the inner pixels (obtained
//        with MHPedestalCam), averaged on an event-by-event basis from 
//        run 13428 with switched off camera LV. 
//        The meaning of the four plots is:
// 
//        1) The distribution of the averaged pedestals
//        2) The averaged pedestals vs. time. 
//           One can see clearly the oscillation pattern
//        3) The fourier transform of the averaged pedestals vs. time. 
//           One can see clearly a peak at a certain frequency
//        4) The projection of the fourier components with the non-exponential
//           (and therefore significant) outlier.
//
//    ad c) Many signal extractors, especially those using a sliding window
//          have biases and their resolutions for zero-signals do not agree 
//          with the pedestal RMS. For the F-Factor method in the calibration
//          and the image cleaning, however, both have to be known and measured. 
//          
//          For this reason, a signal extractor can be handed over to the 
//          pedestal extractor and applied on the pedestal events with the 
//          function SetExtractor(). 
//          The results will get stored in an MPedestalCam. 
//        
//          Note that only extractors deriving from MExtractTimeAndCharge
//          can be used.
//
//   ad d)  The signal extractors themselves need a pedestal to be subtracted 
//          from the FADC slices. 
//          If the user wishes that the pededestals do not get overwritten by 
//          the results from the signal extractor, a different named MPedestalCam
//          can be created with the function: SetNamePedestalOut(). 
//
//  See also: MPedestalCam, MPedestalPix, MPedCalcPedRun, MPedCalcFromLoGain
//
/////////////////////////////////////////////////////////////////////////////
#include "MExtractPedestal.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"  
#include "MRawEvtHeader.h"  
#include "MRawEvtPixelIter.h"

#include "MPedestalPix.h"
#include "MPedestalCam.h"

#include "MGeom.h"
#include "MGeomCam.h"

#include "MExtractTimeAndCharge.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MExtractPedestal);

using namespace std;

const TString  MExtractPedestal::fgNamePedestalCam = "MPedestalCam";
const TString  MExtractPedestal::fgNameRawEvtData  = "MRawEvtData";

const UShort_t MExtractPedestal::fgCheckWinFirst   =   0;
const UShort_t MExtractPedestal::fgCheckWinLast    =  29;
const UShort_t MExtractPedestal::fgMaxSignalVar    =  40;
const UShort_t MExtractPedestal::fgMaxSignalAbs    = 250;

// --------------------------------------------------------------------------
//
// Default constructor: 
//
// Sets:
// - all pointers to NULL
//
// Calls: 
// - Clear()
//
MExtractPedestal::MExtractPedestal(const char *name, const char *title)
    : fGeom(NULL), fPedestalsInter(NULL),
    fPedestalsOut(NULL), fExtractor(NULL), fSignal(0),
    fExtractWinFirst(0), fExtractWinSize(0), fUseSpecialPixels(kFALSE),
    fCounter(0)
{
    fName  = name  ? name  : "MExtractPedestal";
    fTitle = title ? title : "Base class to calculate pedestals";

    SetIntermediateStorage( kFALSE );
    SetRandomCalculation  ( kTRUE  );

    SetNamePedestalCamOut();
    SetNamePedestalCamInter();
    SetNameRawEvtData();

    SetCheckRange(fgCheckWinFirst, fgCheckWinLast);
    SetMaxSignalVar(fgMaxSignalVar);
    SetMaxSignalAbs(fgMaxSignalAbs);

    Clear();
}

// --------------------------------------------------------------------------
//
// Call reset() of all Arays
//
void MExtractPedestal::ResetArrays()
{
    // Reset contents of arrays.
    fSumx.Reset();
    fSumx2.Reset();
    fSumAB0.Reset();
    fSumAB1.Reset();
    fAreaSumx.Reset();
    fAreaSumx2.Reset();
    fAreaSumAB0.Reset();
    fAreaSumAB1.Reset();
    fAreaFilled.Reset();
    fAreaValid.Reset();
    fSectorSumx.Reset();
    fSectorSumx2.Reset();
    fSectorSumAB0.Reset();
    fSectorSumAB1.Reset();
    fSectorFilled.Reset();
    fSectorValid.Reset();
    fNumEventsUsed.Reset();
}

// --------------------------------------------------------------------------
//
// Resets Arrays:
//
// Sets:
// - fRawEvt to NULL
// - fRunHeader to NULL
//
void MExtractPedestal::Clear(const Option_t *o)
{
 
  fRawEvt       = NULL;
  fRunHeader    = NULL;

  // If the size is yet set, set the size
  if (fSumx.GetSize()>0)
    ResetArrays();

}

// --------------------------------------------------------------------------
//
// Checks:
// - if a window is odd
// 
Bool_t MExtractPedestal::SetExtractWindow(UShort_t windowf, UShort_t windows)
{
    Bool_t rc = kTRUE;

    if (windows==0)
    {
        *fLog << warn << GetDescriptor();
        *fLog << " - WARNING: Window size in SetExtractWindow has to be > 0... adjusting to 2!" << endl;
        windows = 2;
        rc = kFALSE;
    }

    fExtractWinSize  = windows;
    fExtractWinFirst = windowf;
    fExtractWinLast  = fExtractWinFirst+fExtractWinSize-1;

    return rc;
}

// --------------------------------------------------------------------------
//
// SetCheckRange: 
// 
// Exits, if the first argument is smaller than 0
// Exits, if the the last argument is smaller than the first
//
Bool_t MExtractPedestal::SetCheckRange(UShort_t chfirst, UShort_t chlast)
{

  Bool_t rc = kTRUE;
  
  if (chlast<=chfirst)
    {
      *fLog << warn << GetDescriptor();
      *fLog << " - WARNING: Last slice in SetCheckRange smaller than first slice... set to first+2" << endl;
      chlast = chfirst+1;
      rc = kFALSE;
    }

  fCheckWinFirst = chfirst;
  fCheckWinLast  = chlast;

  return rc;
}

Bool_t MExtractPedestal::SetRangeFromExtractor(const MExtractor &ext, Bool_t logain)
{
    const Bool_t haslogains = ext.GetLoGainFirst()!=0 && ext.GetLoGainLast()!=0;

    Bool_t rc1 = kTRUE;
    if (!haslogains)
    {
        // We assume that in case without lo-gains we
        // deal with pedestal events only
        rc1 = SetCheckRange(ext.GetHiGainFirst(), ext.GetHiGainLast());
    }

    const Int_t f = logain && haslogains ? ext.GetLoGainFirst() : ext.GetHiGainFirst();
    const Int_t l = logain && haslogains ? ext.GetLoGainLast()  : ext.GetHiGainLast();

    const Int_t w = (l-f+1);

    // Setup to use the hi-gain extraction window in the lo-gain
    // range (the start of the lo-gain range is added automatically
    // by MPedCalcFromLoGain)
    const Bool_t rc2 = SetExtractWindow(f, w);

    return rc1 && rc2;
}

// --------------------------------------------------------------------------
//
// Check (and if neccesary: correct) the extraction and check ranges.
//
void MExtractPedestal::CheckExtractionWindow(UInt_t offset)
{
    *fLog << inf;
    *fLog << "Requested CheckWindow is [" << fCheckWinFirst << "," << fCheckWinLast << "]." <<endl;
    *fLog << "Requested ExtractWindow is [" << fExtractWinFirst+offset << "," << fExtractWinLast+offset << "]." <<endl;

    // fSignal->GetNumSamples() not yet initialized!!!
    const UInt_t num = fRunHeader->GetNumSamplesHiGain()+fRunHeader->GetNumSamplesLoGain();

    // Check upper bound for check window
    if (fCheckWinLast >= num)
    {
        *fLog << inf << "CheckWindow [" << fCheckWinFirst << "," << fCheckWinLast;
        *fLog << "] out of range [0," << num-1 << "]... ";
        *fLog << "reset upper edge to " << num-1 << "." << endl;

        fCheckWinLast = num-1;
    }

    // Now check lower bound for check window
    if (fCheckWinFirst>fCheckWinLast)
    {
        *fLog << err << "CheckWindow first slice " << fCheckWinFirst;
        *fLog << " greater than last slice " << fCheckWinLast;
        *fLog << "... reset to 0." << endl;

        fCheckWinFirst = 0;
    }

    // check upper bound for extaction window
    if (fExtractWinLast+offset >= num)
    {
        *fLog << inf << "ExtractWindow [" << fExtractWinFirst+offset << "," << fExtractWinLast+offset;
        *fLog << "] out of range [0," << num-1 << "]... ";
        *fLog << "reset upper edge to " << num-1 << "." << endl;

        fExtractWinLast = num-offset-1;
    }

    // Now check lower bound for check window
    if (fExtractWinFirst>fExtractWinLast)
    {
        *fLog << err << "ExtractWindow first slice " << fExtractWinFirst+offset;
        *fLog << " greater than last slice " << fExtractWinLast+offset;
        *fLog << "... reset to 0." << endl;

        fExtractWinFirst = 0;
    }

    // Calculate window size for extraction window
    fExtractWinSize = fExtractWinLast-fExtractWinFirst+1;

    // Check if use tries to do a fundamental pedestal extraction
    // with an odd number of slices
    if (fExtractor || TMath::Even(fExtractWinSize))
        return;

    // Make sure the number of extracted slices is even
    fExtractWinLast += offset+fExtractWinLast==num-1 ? -1 : +1;

    *fLog << inf << "ExtractionWindow odd... set to [";
    *fLog << fExtractWinFirst+offset << "," << fExtractWinLast+offset << "]" << endl;

    fExtractWinSize = fExtractWinLast-fExtractWinFirst+1;
}

// --------------------------------------------------------------------------
//
// Look for the following input containers:
//
//  - MRawEvtData
//  - MRawRunHeader
//  - MRawEvtHeader
//  - MGeomCam
// 
// The following output containers are also searched and created if
// they were not found:
//
//  - MPedestalCam with the name fPedContainerName
//
Int_t MExtractPedestal::PreProcess(MParList *pList)
{

  Clear();

  fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber(fNameRawEvtData));
  if (!fRawEvt)
  {
      *fLog << err << AddSerialNumber(fNameRawEvtData) << " not found... aborting." << endl;
      return kFALSE;
  }

  fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
  if (!fRunHeader)
  {
      *fLog << err << AddSerialNumber("MRawRunHeader") << " not found... aborting." << endl;
      return kFALSE;
  }
  
  fGeom = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
  if (!fGeom)
  {
      *fLog << err << AddSerialNumber("MGeomCam") << " not found... aborting." << endl;
      return kFALSE;
  }

  fSignal = (MPedestalSubtractedEvt*)pList->FindObject(AddSerialNumber("MPedestalSubtractedEvt"));
  if (!fSignal)
  {
      *fLog << err << AddSerialNumber("MPedestalSubtractedEvt") << " not found... aborting." << endl;
      return kFALSE;
  }

  if (!fPedestalsInter && fIntermediateStorage)
  {
      fPedestalsInter = (MPedestalCam*)pList->FindCreateObj("MPedestalCam", AddSerialNumber(fNamePedestalCamInter));
      if (!fPedestalsInter)
          return kFALSE;
  }

  if (!fPedestalsOut)
  {
      fPedestalsOut = (MPedestalCam*)pList->FindCreateObj("MPedestalCam", AddSerialNumber(fNamePedestalCamOut));
      if (!fPedestalsOut)
          return kFALSE;
  }

  fCounter = 0;

  return fExtractor ? fExtractor->CallPreProcess(pList) : kTRUE;
}

//-----------------------------------------------------------------------
//
// Call Calc(). If fExtractor!=NULL enclose the call in setting the
// NoiseCalculation to fRandomCalculation
//
Int_t MExtractPedestal::Process()
{
    //
    // Necessary check for extraction of special pixels
    // together with data which does not yet have them
    //
    if (fSumx.GetSize()==0)
        return kTRUE;

    if (fExtractor)
        fExtractor->SetNoiseCalculation(fRandomCalculation);

    Calc();

    if (fExtractor)
        fExtractor->SetNoiseCalculation(kFALSE);

    fPedestalsOut->SetNumEvents(fCounter);

    return kTRUE;
}

// ---------------------------------------------------------------------------------
//
// Sets the size (from MPedestalCam::GetSize() ) and resets the following arrays:
//  - fSumx
//  - fSumx2
//  - fSumAB0
//  - fSumAB1
//  - fAreaSumx
//  - fAreaSumx2
//  - fAreaSumAB0
//  - fAreaSumAB1
//  - fAreaFilled
//  - fAreaValid 
//  - fSectorSumx 
//  - fSectorSumx2
//  - fSectorSumAB0
//  - fSectorSumAB1
//  - fSectorFilled
//  - fSectorValid
//
Bool_t MExtractPedestal::ReInit(MParList *pList)
{
    // Necessary check for special pixels which might not yet have existed
    if (!fRawEvt)
    {
        if (fRunHeader->GetFormatVersion() > 3)
            return kTRUE;

        *fLog << err << "ERROR - " << fNameRawEvtData << " [MRawEvtData] has not ";
        *fLog << "been found and format version > 3... abort." << endl;
        return kFALSE;
    }

    // If the size is not yet set, set the size
    if (fSumx.GetSize()==0)
    {
        // Initialize the normal pixels (size of MPedestalCam already set by MGeomApply)
        const Int_t npixels  = fPedestalsOut->GetSize();

        fSumx.  Set(npixels);
        fSumx2. Set(npixels);
        fSumAB0.Set(npixels);
        fSumAB1.Set(npixels);

        fNumEventsUsed.Set(npixels);

        if (fUseSpecialPixels)
        {
            // Initialize size of MPedestalCam in case of special pixels (not done by MGeomApply)
            const UShort_t nspecial = fRunHeader->GetNumSpecialPixels();
            if (nspecial == 0)
            {
                *fLog << warn << "WARNING - Number of special pixels is 0." << endl;
                return kTRUE;
            }

            fPedestalsOut->InitSize((UInt_t)nspecial);
        }
        else
        {
            // Initialize the averaged areas and sectors (do not exist for special pixels)
            const Int_t areas    = fPedestalsOut->GetNumAverageArea();
            const Int_t sectors  = fPedestalsOut->GetNumAverageSector();

            fAreaSumx.  Set(areas);
            fAreaSumx2. Set(areas);
            fAreaSumAB0.Set(areas);
            fAreaSumAB1.Set(areas);
            fAreaFilled.Set(areas);
            fAreaValid .Set(areas);

            fSectorSumx.  Set(sectors);
            fSectorSumx2. Set(sectors);
            fSectorSumAB0.Set(sectors);
            fSectorSumAB1.Set(sectors);
            fSectorFilled.Set(sectors);
            fSectorValid .Set(sectors);

            for (Int_t i=0; i<npixels; i++)
            {
                const UInt_t aidx   = (*fGeom)[i].GetAidx();
                const UInt_t sector = (*fGeom)[i].GetSector();

                fAreaValid  [aidx]  ++;
                fSectorValid[sector]++;
            }
        }
    }

    if (fExtractor)
    {
        *fLog << all << fExtractor->ClassName() << "... " << flush;
        if (!fExtractor->ReInit(pList))
            return kFALSE;

        SetRangeFromExtractor(*fExtractor);

        // fSignal->GetNumSamples() not yet initialized!!!
        const UInt_t num = fRunHeader->GetNumSamples();
        if (fExtractWinLast >= num)
        {
            *fLog << err;
            *fLog << "ERROR - Selected fExtractWinLast " << fExtractWinLast;
            *fLog << " out of range (>=" << num<< ")." << endl;
            return kFALSE;
        }
    }
    else
        if (fRunHeader->GetNumSamplesLoGain()==0 && (fCheckWinFirst!=0 || fCheckWinLast!=0))
        {
            *fLog << inf << "Data has no lo-gains... resetting check window to extraction window." << endl;
            SetCheckRange(fExtractWinFirst, fExtractWinLast);
        }

    //CheckExtractionWindow();

    return kTRUE;
}

// ---------------------------------------------------------------------------------
//
// PostProcess the extractor if available
//
Int_t MExtractPedestal::PostProcess()
{
    return fExtractor ? fExtractor->CallPostProcess() : kTRUE;
}

// ---------------------------------------------------------------------------------
//
// Check whether the signal variation between fCheckWinFirst and fCheckWinLast
// exceeds fMaxSignalVar or the signal is greater than fMaxSignalAbs
//
Bool_t MExtractPedestal::CheckVariation(UInt_t idx) const
{
    // This is the fast workaround to put hi- and lo-gains together
    USample_t *slices = fSignal->GetSamplesRaw(idx);

    // Start 'real' work
    USample_t max = 0;
    USample_t min = (USample_t)-1;

    // Find the maximum and minimum signal per slice in the high gain window
    for (USample_t *slice=slices+fCheckWinFirst; slice<=slices+fCheckWinLast; slice++)
    {
        if (*slice > max)
            max = *slice;
        if (*slice < min)
            min = *slice;
    }

    max /= fRunHeader->GetScale();
    min /= fRunHeader->GetScale();

    // If the maximum in the high gain window is smaller than
    return max-min<fMaxSignalVar && max<fMaxSignalAbs;
}

// ---------------------------------------------------------------------------------
//
// Invoke the hi-gain extraction starting at fExtractWinFirst+offset
// for fExtractWinLast-fExtractWinFirst+1 slices. If Noise calculation
// is set it is up to the signal extractor to do the right thing.
//
// Returns the extracted signal.
//
Float_t MExtractPedestal::CalcExtractor(const MRawEvtPixelIter &pixel, Int_t offset) const
{
    // Use the same extraction window as for signal extraction
    const Int_t first = fExtractWinFirst;
    const Int_t last  = fExtractWinLast;

    const Int_t start = first+offset;

    const Int_t range = last-first+1;

    // This check is already done in CheckExtractionWindow
    //    if (range>pixel.GetNumSamples()-start)
    //        range = pixel.GetNumSamples()-start;

    const Int_t idx = pixel.GetPixelId();

    // Do some handling if maxpos is last slice?
    const Int_t maxposhi = fRandomCalculation ? 0 : fSignal->GetMaxPos(idx, start, start+range-1);

    const Float_t *sig = fSignal->GetSamples(idx);

    // The pedestal is extracted with the hi-gain extractor (eg. digital
    // filter weights) but from the lo-gains
    Float_t dummy[3];
    Float_t sum = 0;
    fExtractor->FindTimeAndChargeHiGain2(sig+start, range, sum,
                                         dummy[0], dummy[1], dummy[2],
                                         0, maxposhi);

    return sum;
}

// ---------------------------------------------------------------------------------
//
// Sum slices from fExtractWinFirst to fExtractWinLast. The total sum is
// returned. ab0 and ab1 will contain the total sum splitted by the
// AB-flag. If the AB-flag is invalid ab0=ab1=0 is returned.
//
UInt_t MExtractPedestal::CalcSums(const MRawEvtPixelIter &pixel, Int_t offset, UInt_t &ab0, UInt_t &ab1) const
{
    const Int_t first = fExtractWinFirst+offset;

    USample_t *ptr = fSignal->GetSamplesRaw(pixel.GetPixelId())+first;
    USample_t *end = ptr + fExtractWinSize;

    Int_t abflag = pixel.HasABFlag() + first;

    UInt_t ab[2] = { 0, 0 };
    while (ptr<end)
        ab[abflag++ & 0x1] += *ptr++;

    // This check if for old data without AB-Flag in the data
    const Bool_t valid = pixel.IsABFlagValid();

    ab0 = valid ? ab[0] : 0;
    ab1 = valid ? ab[1] : 0;

    return ab[0]+ab[1];
}

// ---------------------------------------------------------------------------------
//
// Check for the variation of the pixel. Return kFALSE if this pixel
// should not be used.
// Calculate the pedestal either with the extractor or by summing slices.
// And update all arrays.
//
Bool_t MExtractPedestal::CalcPixel(const MRawEvtPixelIter &pixel, Int_t offset, UInt_t usespecialpixels)
{
    const UInt_t idx = pixel.GetPixelId();
    if (!CheckVariation(idx))
        return kFALSE;

    //extract pedestal
    UInt_t ab[2];
    const Float_t sum = fExtractor ?
        CalcExtractor(pixel, offset) :
        CalcSums(pixel, offset, ab[0], ab[1]);

    if (fIntermediateStorage)
        (*fPedestalsInter)[idx].Set(sum, 0, 0, fNumEventsUsed[idx]);

    const Double_t sqrsum = sum*sum;

    fSumx[idx]  += sum;
    fSumx2[idx] += sqrsum;

    fNumEventsUsed[idx]++;

    if (!fExtractor && pixel.IsABFlagValid())
    {
        fSumAB0[idx] += ab[0];
        fSumAB1[idx] += ab[1];
    }

    if (usespecialpixels)
        return kTRUE;

    const UInt_t aidx   = (*fGeom)[idx].GetAidx();
    const UInt_t sector = (*fGeom)[idx].GetSector();

    fAreaFilled[aidx]++;
    fSectorFilled[sector]++;

    fAreaSumx[aidx]      += sum;
    fAreaSumx2[aidx]     += sqrsum;
    fSectorSumx[sector]  += sum;
    fSectorSumx2[sector] += sqrsum;

    if (!fExtractor && pixel.IsABFlagValid())
    {
        fAreaSumAB0[aidx]   += ab[0];
        fAreaSumAB1[aidx]   += ab[1];
        fSectorSumAB0[aidx] += ab[0];
        fSectorSumAB1[aidx] += ab[1];
    }

    return kTRUE;
}

// ---------------------------------------------------------------------------------
//
// Calculates for pixel "idx":
//
// Ped per slice      = sum / n / fExtractWinSize;
// RMS per slice      = sqrt { (sum2 -  sum*sum/n) / (n-1) / fExtractWinSize }
// ABOffset per slice = (fSumAB0[idx] - fSumAB1[idx]) / n / fExtractWinSize;
//
// Stores the results in MPedestalCam[pixid]
//
void MExtractPedestal::CalcPixResults(const UInt_t pixid)
{
    const UInt_t  nevts = fNumEventsUsed[pixid];
    if (nevts<2)
        return;

    const Double_t sum  = fSumx[pixid];
    const Double_t sum2 = fSumx2[pixid];

    // 1. Calculate the mean of the sums:
    Double_t ped = sum/nevts;

    // 2. Calculate the Variance of the sums:
    Double_t var = (sum2-sum*sum/nevts)/(nevts-1.);

    // 3. Calculate the amplitude of the 150MHz "AB" noise
    Double_t abOffs = (fSumAB0[pixid] - fSumAB1[pixid]) / nevts;

    // 4. Scale the mean, variance and AB-noise to the number of slices:
    ped    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    var    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    abOffs /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    // The pedestal extracted with the extractor is divided by
    // the number of hi-gain samples because the calibration
    // multiplies by this number

    // scale to 256
    const UInt_t scale = fExtractor ? 1 : fRunHeader->GetScale();

    ped    /= scale;
    abOffs /= scale;

    // 5. Calculate the RMS from the Variance:
    const Double_t rms = var<0 ? 0 : TMath::Sqrt(var)/scale;

    // abOffs contains only half of the signal as ped.
    // Therefor abOffs is not the full, but the half amplitude
    (*fPedestalsOut)[pixid].Set(ped, rms, abOffs, nevts);
}

// ---------------------------------------------------------------------------------
//
// Calculates for area idx "aidx" with "napix" valid pixels:
//
// Ped per slice      = sum / nevts / fExtractWinSize / napix;
// RMS per slice      = sqrt { (sum2 -  sum*sum/nevts) / (nevts-1) / fExtractWinSize / napix }
// ABOffset per slice = (fSumAB0[idx] - fSumAB1[idx]) / nevts / fExtractWinSize / napix;
//
// Stores the results in MPedestalCam::GetAverageArea(aidx)
//
void MExtractPedestal::CalcAreaResults(const UInt_t aidx)
{
    const UInt_t nevts = fAreaFilled[aidx];
    if (nevts<2)
        return;

    const UInt_t napix = fAreaValid[aidx];
    if (napix<1)
        return;

    const Double_t sum  = fAreaSumx[aidx];
    const Double_t sum2 = fAreaSumx2[aidx];

    // 1. Calculate the mean of the sums:
    Double_t ped = sum/nevts;

    // 2. Calculate the Variance of the sums:
    Double_t var = (sum2/napix-sum*sum/nevts)/(nevts-1.);

    // 3. Calculate the amplitude of the 150MHz "AB" noise
    Double_t abOffs = (fAreaSumAB0[aidx] - fAreaSumAB1[aidx]) / nevts;

    // 4. Scale the mean, variance and AB-noise to the number of slices:
    ped    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    var    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    abOffs /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    // The pedestal extracted with the extractor is divided by
    // the number of hi-gain samples because the calibration
    // multiplies by this number

    // scale to 256
    const UInt_t scale = fExtractor ? 1 : fRunHeader->GetScale();

    // 5. Scale the mean, variance and AB-noise to the number of pixels:
    ped    /= napix*scale;
    abOffs /= napix*scale;

    // 6. Calculate the RMS from the Variance:
    const Double_t rms = var<0 ? 0 : TMath::Sqrt(var)/scale;

    // abOffs contains only half of the signal as ped.
    // Therefor abOffs is not the full, but the half amplitude
    fPedestalsOut->GetAverageArea(aidx).Set(ped, rms, abOffs, nevts);
}

// ---------------------------------------------------------------------------------
//
// Calculates for sector idx "sector" with "nspix" valid pixels:
//
// Ped per slice      = sum / nevts / fExtractWinSize / nspix;
// RMS per slice      = sqrt { (sum2 -  sum*sum/nevts) / (nevts-1) / fExtractWinSize / nspix }
// ABOffset per slice = (fSumAB0[idx] - fSumAB1[idx]) / nevts / fExtractWinSize / nspix;
//
// Stores the results in MPedestalCam::GetAverageSector(sector)
//
void MExtractPedestal::CalcSectorResults(const UInt_t sector)
{
    const UInt_t nevts = fSectorFilled[sector];
    if (nevts<2)
        return;

    const UInt_t nspix = fSectorValid[sector];
    if (nspix<1)
        return;

    const Double_t sum  = fSectorSumx[sector];
    const Double_t sum2 = fSectorSumx2[sector];

    // 1. Calculate the mean of the sums:
    Double_t ped        = sum/nevts;

    // 2. Calculate the Variance of the sums:
    Double_t var = (sum2/nspix-sum*sum/nevts)/(nevts-1.);

    // 3. Calculate the amplitude of the 150MHz "AB" noise
    Double_t abOffs = (fSectorSumAB0[sector] - fSectorSumAB1[sector]) / nevts;

    // 4. Scale the mean, variance and AB-noise to the number of slices:
    ped    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    var    /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    abOffs /= fExtractor ? fExtractor->GetNumHiGainSamples() : fExtractWinSize;
    // The pedestal extracted with the extractor is divided by
    // the number of hi-gain samples because the calibration
    // multiplies by this number

    // scale to 256
    const UInt_t scale = fExtractor ? 1 : fRunHeader->GetScale();

    // 5. Scale the mean, variance and AB-noise to the number of pixels:
    ped    /= nspix*scale;
    abOffs /= nspix*scale;

    // 6. Calculate the RMS from the Variance:
    const Double_t rms = var<0 ? 0 : TMath::Sqrt(var)/scale;

    // abOffs contains only half of the signal as ped.
    // Therefor abOffs is not the full, but the half amplitude
    fPedestalsOut->GetAverageSector(sector).Set(ped, rms, abOffs, nevts);
}

// --------------------------------------------------------------------------
//
// Loop over the pixels to get the averaged pedestal
//
void MExtractPedestal::CalcPixResult()
{
    for (UInt_t idx=0; idx<fNumEventsUsed.GetSize(); idx++)
        CalcPixResults(idx);
}

// --------------------------------------------------------------------------
//
// Loop over the sector indices to get the averaged pedestal per sector
//
void MExtractPedestal::CalcSectorResult()
{
    for (UInt_t sector=0; sector<fSectorFilled.GetSize(); sector++)
        CalcSectorResults(sector);
}

// --------------------------------------------------------------------------
//
// Loop over the (two) area indices to get the averaged pedestal per aidx
//
void MExtractPedestal::CalcAreaResult()
{
    for (UInt_t aidx=0; aidx<fAreaFilled.GetSize(); aidx++)
        CalcAreaResults(aidx);
}

//-----------------------------------------------------------------------
//
void MExtractPedestal::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << "Name of interm. MPedestalCam: " << (fPedestalsInter?fPedestalsInter->GetName():fNamePedestalCamInter.Data()) << " (" << fPedestalsInter << ")" << endl;
    *fLog << "Name of output MPedestalCam:  " << (fPedestalsOut?fPedestalsOut->GetName():fNamePedestalCamOut.Data()) << " (" << fPedestalsOut << ")" << endl;
    *fLog << "Intermediate Storage is       " << (fIntermediateStorage?"on":"off") << endl;
    *fLog << "Special pixel mode            " << (fUseSpecialPixels?"on":"off") << endl;
    if (fExtractor)
    {
        *fLog << "Extractor used:               " << fExtractor->ClassName() << " (";
        *fLog << (fRandomCalculation?"":"non-") << "random)" << endl;
    }
    *fLog << "ExtractWindow from slice " << fExtractWinFirst << " to " << fExtractWinLast << " incl." << endl;
    *fLog << "CheckWindow from slice " << fCheckWinFirst   << " to " << fCheckWinLast << " incl." << endl;
    *fLog << "Max.allowed signal variation: " << fMaxSignalVar << endl;
    *fLog << "Max.allowed signal absolute:  " << fMaxSignalAbs << endl;
}

// --------------------------------------------------------------------------
//
//  The following resources are available:
//    ExtractWindowFirst:    15
//    ExtractWindowSize:      6
//    PedestalUpdate:       yes
//    RandomCalculation:    yes
//
Int_t MExtractPedestal::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc=kFALSE;

    // find resource for fUseSpecialPixels
    if (IsEnvDefined(env, prefix, "UseSpecialPixels", print))
    {
        SetUseSpecialPixels(GetEnvValue(env, prefix, "UseSpecialPixels", fUseSpecialPixels));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "IntermediateStorage", print))
    {
        SetIntermediateStorage(GetEnvValue(env, prefix, "IntermediateStorage", fIntermediateStorage));
        rc = kTRUE;
    }

    // find resource for random calculation
    if (IsEnvDefined(env, prefix, "RandomCalculation", print))
    {
        SetRandomCalculation(GetEnvValue(env, prefix, "RandomCalculation", fRandomCalculation));
        rc = kTRUE;
    }

    // Find resources for ExtractWindow
    Int_t ef = fExtractWinFirst;
    Int_t es = fExtractWinSize;
    if (IsEnvDefined(env, prefix, "ExtractWinFirst", print))
    {
        ef = GetEnvValue(env, prefix, "ExtractWinFirst", ef);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "ExtractWinSize", print))
    {
        es = GetEnvValue(env, prefix, "ExtractWinSize", es);
        rc = kTRUE;
    }

    SetExtractWindow(ef,es);

    // Find resources for CheckWindow
    Int_t cfs = fCheckWinFirst;
    Int_t cls = fCheckWinLast;
    if (IsEnvDefined(env, prefix, "CheckWinFirst", print))
    {
        cfs = GetEnvValue(env, prefix, "CheckWinFirst", cfs);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "CheckWinLast", print))
    {
        cls = GetEnvValue(env, prefix, "CheckWinLast", cls);
        rc = kTRUE;
    }

    SetCheckRange(cfs,cls);

    // find resource for maximum signal variation
    if (IsEnvDefined(env, prefix, "MaxSignalVar", print))
    {
        SetMaxSignalVar(GetEnvValue(env, prefix, "MaxSignalVar", fMaxSignalVar));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "MaxSignalAbs", print))
    {
        SetMaxSignalAbs(GetEnvValue(env, prefix, "MaxSignalAbs", fMaxSignalAbs));
        rc = kTRUE;
    }

    // find resource for MPedestalCam
    if (IsEnvDefined(env, prefix, "NamePedestalCamInter", print))
    {
        SetNamePedestalCamInter(GetEnvValue(env, prefix, "NamePedestalCamInter", fNamePedestalCamInter));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "NamePedestalCamOut", print))
    {
        SetNamePedestalCamOut(GetEnvValue(env, prefix, "NamePedestalCamOut", fNamePedestalCamOut));
        rc = kTRUE;
    }

    return rc;
}
