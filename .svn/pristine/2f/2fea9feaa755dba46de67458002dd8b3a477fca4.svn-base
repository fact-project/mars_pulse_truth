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
!   Author(s): Josep Flix 04/2001 <mailto:jflix@ifae.es>
!   Author(s): Thomas Bretz 05/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Sebastian Commichau 12/2003 
!   Author(s): Javier Rico 01/2004 <mailto:jrico@ifae.es>
!   Author(s): Markus Gaug 01/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
//   MPedCalcPedRun
//
//  This task takes a pedestal run file and fills MPedestalCam during
//  the Process() with the pedestal and rms computed in an event basis.
//  In the PostProcess() MPedestalCam is finally filled with the pedestal
//  mean and rms computed in a run basis.
//  More than one run (file) can be merged
//
//  MPedCalcPedRun applies the following formula (1):
// 
//  Pedestal per slice = sum(x_i) / n / slices
//  PedRMS per slice   = Sqrt(  ( sum(x_i^2) - sum(x_i)^2/n ) / n-1 / slices )
// 
//  where x_i is the sum of "slices" FADC slices and sum means the sum over all
//  events. "n" is the number of events, "slices" is the number of summed FADC samples.
// 
//  Note that the slice-to-slice fluctuations are not Gaussian, but Poissonian, thus 
//  asymmetric and they are correlated.
// 
//  It is important to know that the Pedestal per slice and PedRMS per slice depend 
//  on the number of used FADC slices, as seen in the following plots:
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
//
// The plots show the inner and outer pixels, respectivly and have the following meaning:
// 
// 1) The calculated mean pedestal per slice (from MPedCalcPedRun)
// 2) The fitted mean pedestal per slice     (from MHPedestalCam)
// 3) The calculated pedestal RMS per slice  (from MPedCalcPedRun)
// 4) The fitted sigma of the pedestal distribution per slice
//                                           (from MHPedestalCam)
// 5) The relative difference between calculation and histogram fit
//    for the mean
// 6) The relative difference between calculation and histogram fit
//    for the sigma or RMS, respectively.
// 
// The calculated means do not change significantly except for the case of 2 slices, 
// however the RMS changes from 5.7 per slice in the case of 2 extracted slices 
// to 8.3 per slice in the case of 26 extracted slices. This change is very significant.
// 
// The plots have been produced on run 20123. You can reproduce them using
// the macro pedestalstudies.C
// 
//  Usage of this class: 
//  ====================
// 
//  Call: SetRange(higainfirst, higainlast, logainfirst, logainlast) 
//  to modify the ranges in which the window is allowed to move. 
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  0 
//   fHiGainLast  =  fgHiGainLast  =  29
//   fLoGainFirst =  fgLoGainFirst =  0 
//   fLoGainLast  =  fgLoGainLast  =  14
//
//  Call: SetWindowSize(windowhigain, windowlogain) 
//  to modify the sliding window widths. Windows have to be an even number. 
//  In case of odd numbers, the window will be modified.
//
//  Defaults are:
//
//   fHiGainWindowSize = fgHiGainWindowSize = 14
//   fLoGainWindowSize = fgLoGainWindowSize = 0
//
//
// ToDo:
//   - Take a setup file (ReadEnv-implementation) as input
//
//
//  Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MRawEvtHeader
//   MGeomCam
//
//  Output Containers:
//   MPedestalCam
//
//  See also: MPedestalCam, MPedestalPix, MHPedestalCam, MExtractor
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedCalcPedRun.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"

#include "MTriggerPattern.h"

ClassImp(MPedCalcPedRun);

using namespace std;

const UShort_t MPedCalcPedRun::fgExtractWinFirst       = 0;
const UShort_t MPedCalcPedRun::fgExtractWinSize        = 6;

const UInt_t   MPedCalcPedRun::gkFirstRunWithFinalBits = 45605;

// --------------------------------------------------------------------------
//
// Default constructor: 
//
// Calls: 
// - SetExtractWindow(fgExtractWinFirst, fgExtractWinSize)
//
MPedCalcPedRun::MPedCalcPedRun(const char *name, const char *title)
    : fIsFirstPedRun(kFALSE), fTrigPattern(NULL)
{
    fName  = name  ? name  : "MPedCalcPedRun";
    fTitle = title ? title : "Task to calculate pedestals from pedestal runs raw data";

    SetExtractWindow(fgExtractWinFirst, fgExtractWinSize);
}

// --------------------------------------------------------------------------
//
// Call MExtractPedestal::ResetArrays
//
void MPedCalcPedRun::Reset()
{
    MExtractPedestal::ResetArrays();
}

// --------------------------------------------------------------------------
//
// Set fIsFirstPedRun=kTRUE
//
Int_t MPedCalcPedRun::PreProcess(MParList *pList)
{
    fIsFirstPedRun = kTRUE;
    fIsNotPedRun   = kFALSE;

    fTrigPattern = (MTriggerPattern*)pList->FindObject("MTriggerPattern");
    if (!fTrigPattern)
        *fLog << inf << "MTriggerPattern not found... Cannot use interlaced pedestal events." << endl;

    return MExtractPedestal::PreProcess(pList);
}

// --------------------------------------------------------------------------
//
// The run type is checked for "kRTPedestal"
// and the variable fIsNotPedRun is set in that case
//
Bool_t MPedCalcPedRun::ReInit(MParList *pList)
{
    if (!MExtractPedestal::ReInit(pList))
        return kFALSE;

    CheckExtractionWindow();

    //
    // If this is the first ped run, the next run (call to ReInit)
    // is not the first anymore
    //
    switch (fRunHeader->GetRunType())
    {
    case MRawRunHeader::kRTPedestal:
    case MRawRunHeader::kRTMonteCarlo:
        fIsFirstPedRun = kFALSE;
        fIsNotPedRun   = kFALSE;
        return kTRUE;

    case MRawRunHeader::kRTCalibration:
        {
            TString proj(fRunHeader->GetProjectName());
            proj.ToLower();

            // test if a continuous light run has been declared as calibration...
            if (proj.Contains("cl"))
            {
                fIsFirstPedRun = kFALSE;
                fIsNotPedRun   = kFALSE;
                return kTRUE;
            }
        }
    }

    fIsNotPedRun = kTRUE;

    //
    // If this is the first call to ReInit (before reading the first file)
    // nothing should be done. It occurs for the case that the first treated
    // file is not of pedestal type.
    //
    if (fIsFirstPedRun)
        return kTRUE;

    //
    // In the other case, produce the MPedestalCam to be use the subsequent
    // (non-pedestal) events
    //
    *fLog << inf << "Finalizing pedestal calculations..." << flush;

    if (!Finalize())
        return kFALSE;

    Reset();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return kTRUE (without doing anything) in case that the run type is not 
// equal to 1 (pedestal run)
//
// Fill the MPedestalCam container with the signal mean and rms for the event.
// Store the measured signal in arrays fSumx and fSumx2 so that we can 
// calculate the overall mean and rms in the PostProcess()
//
void MPedCalcPedRun::Calc()
{
    if (fIsNotPedRun && !IsPedBitSet())
        return;

    MRawEvtPixelIter pixel(fRawEvt);
    while (pixel.Next())
        CalcPixel(pixel, 0, fUseSpecialPixels);

    fCounter++;

    fPedestalsOut->SetReadyToSave();
}

// --------------------------------------------------------------------------
//
// Compute signal mean and rms in the whole run and store it in MPedestalCam
//
Int_t MPedCalcPedRun::Finalize()
{
    //
    // Necessary check for extraction of special pixels
    // together with data which does not yet have them
    //
    if (fSumx.GetSize()==0)
        return kTRUE;

    CalcPixResult();

    if (!fUseSpecialPixels)
    {
        CalcAreaResult();
        CalcSectorResult();
    }

    fPedestalsOut->SetNumSlices(fExtractWinSize);
    fPedestalsOut->SetReadyToSave();

    return kTRUE;
}

//-----------------------------------------------------------------------
//
// PostProcess MExtractPedestal and call Finalize
//
Int_t MPedCalcPedRun::PostProcess()
{
    if (!fRawEvt)
        return kTRUE;

    if (!Finalize())
        return kFALSE;

    return MExtractPedestal::PostProcess();
}

//-----------------------------------------------------------------------
// 
// Return if the pedestal bit was set from the calibration trigger box.
// The last but one bit is used for the "pedestal-bit".
//
// This bit is set since run gkFirstRunWithFinalBits
//
Bool_t MPedCalcPedRun::IsPedBitSet()
{     
    if (!fRunHeader->IsMonteCarloRun() && fRunHeader->GetTelescopeNumber()==1 && fRunHeader->GetRunNumber()<gkFirstRunWithFinalBits)
        return kFALSE;

    if (!fTrigPattern)
        return kFALSE;

    return (fTrigPattern->GetPrescaled() & MTriggerPattern::kPedestal) ? kTRUE : kFALSE;
}

//-----------------------------------------------------------------------
//
void MPedCalcPedRun::Print(Option_t *o) const
{
    MExtractPedestal::Print(o);

    *fLog << "First pedrun out of sequence: " << (fIsFirstPedRun?"yes":"no") << endl;
}
