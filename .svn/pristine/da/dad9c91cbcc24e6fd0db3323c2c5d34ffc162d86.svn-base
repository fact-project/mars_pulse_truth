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
!   Author(s): Florian Goebel 06/2004 <mailto:fgoebel@mppmu.mpg.de>
!   Author(s): Nepomuk Otte 10/2004 <mailto:otte@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
// 
//   MPedCalcLoGain
//
//  This task derives from MExtractPedestal. 
//  It calculates the pedestals using the low gain slices, whenever the difference 
//  between the highest and the lowest slice in the high gain
//  slices is below a given threshold (SetMaxHiGainVar). In this case the receiver
//  boards do not switch to lo gain and the so called lo gain slices are actually
//  high gain slices. 
//
//  MPedCalcLoGain also fills the ABoffset in MPedestalPix which allows to correct 
//  the 150 MHz clock noise.
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
//End_Html
//
// The plots show the inner and outer pixels, respectivly and have the following meaning:
// 
// 1) The calculated mean pedestal per slice (from MPedCalcFromLoGain)
// 2) The fitted mean pedestal per slice     (from MHPedestalCam)
// 3) The calculated pedestal RMS per slice  (from MPedCalcFromLoGain)
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
// 
//   fCheckWinFirst   =  fgCheckWinFirst   =  0 
//   fCheckWinLast    =  fgCheckWinLast    =  29
//   fExtractWinFirst =  fgExtractWinFirst =  17
//   fExtractWinSize  =  fgExtractWinSize  =  6
//   fMaxSignalVar    =  fgMaxSignalVar    = 40;
//
//  Call: 
//  SetCheckRange(fCheckWinFirst,fCheckWinLast); 
//  to set the Window in which a signal is searched
//
//  SetExtractWindow(fExtractWinFirst,fExtractWinSize);
//  to set the Window from which a signal is extracted
//
//  SetMaxSignalVar(fMaxSignalVar);
//  set the maximum allowed difference between maximum and minimal signal in CheckWindow  
//
//   Variables:
//   fgCheckWinFirst;      First FADC slice to check for signal (currently set to: 0)
//   fgCheckWinLast:       Last FADC slice to check for signal (currently set to: 29)
//   fgExtractWinFirst:    First FADC slice to be used for pedestal extraction (currently set to: 15)
//   fgExtractWinSize:     Window size in slices used for pedestal extraction (currently set to: 6)
//   fgMaxSignalVar:       The maximum difference between the highest and lowest slice
//                         in the check window allowed in order to use event
//
//  Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MGeomCam
//
//  Output Containers:
//   MPedestalCam
//
//  See also: MPedestalCam, MPedestalPix, MHPedestalCam, MExtractor
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedCalcFromLoGain.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"  
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"

ClassImp(MPedCalcFromLoGain);

using namespace std;

const UShort_t MPedCalcFromLoGain::fgExtractWinFirst =  17;
const UShort_t MPedCalcFromLoGain::fgExtractWinSize  =   6;
const UInt_t   MPedCalcFromLoGain::fgNumDump         = 500;

// --------------------------------------------------------------------------
//
// Default constructor: 
//
// Calls: 
// - SetExtractWindow(fgExtractWinFirst, fgExtractWinSize)
//
MPedCalcFromLoGain::MPedCalcFromLoGain(const char *name, const char *title)
{
    fName  = name  ? name  : "MPedCalcFromLoGain";
    fTitle = title ? title : "Task to calculate pedestals from lo-gains";

    SetExtractWindow(fgExtractWinFirst, fgExtractWinSize);
    SetPedestalUpdate(kTRUE);
    SetNumDump();
}

// ---------------------------------------------------------------------------------
//
// Checks:
// - if the number of available slices 
//   (fRunHeader->GetNumSamplesHiGain()+(Int_t)fRunHeader->GetNumSamplesLoGain()-1)
//   is smaller than the number of used slices
//   (fExtractWinSize+ fExtractWinFirst-1) or 
//    fCheckWinLast
//
Bool_t MPedCalcFromLoGain::ReInit(MParList *pList)
{
    if (!MExtractPedestal::ReInit(pList))
        return kFALSE;

    const Int_t nhi = fRunHeader->GetNumSamplesHiGain();
    const Int_t nlo = fRunHeader->GetNumSamplesLoGain();
    CheckExtractionWindow(nlo>0?nhi:0);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the MPedestalCam container with the signal mean and rms for the event.
// Store the measured signal in arrays fSumx and fSumx2 so that we can 
// calculate the overall mean and rms in the PostProcess()
//
void MPedCalcFromLoGain::Calc()
{
    const Int_t nhi = fRunHeader->GetNumSamplesHiGain();
    const Int_t nlo = fRunHeader->GetNumSamplesLoGain();

    const Int_t offset = nlo>0?nhi:0;

    // Real Process
    MRawEvtPixelIter pixel(fRawEvt);
    while (pixel.Next())
    {
        if (!CalcPixel(pixel, offset))
            continue;

        const UInt_t idx = pixel.GetPixelId();
        if (!fPedestalUpdate || (UInt_t)fNumEventsUsed[idx]<fNumEventsDump)
            continue;

        CalcPixResults(idx);

        fNumEventsUsed[idx]=0;
        fSumx[idx]=0;
        fSumx2[idx]=0;
        fSumAB0[idx]=0;
        fSumAB1[idx]=0;
    }

    if (fNumAreasDump>0 && !(GetNumExecutions() % fNumAreasDump))
    {
        CalcAreaResult();
        fAreaFilled.Reset();
    }

    if (fNumSectorsDump>0 && !(GetNumExecutions() % fNumSectorsDump))
    {
        CalcSectorResult();
        fSectorFilled.Reset();
    }

    fCounter++;

    if (fPedestalUpdate)
        fPedestalsOut->SetReadyToSave();
}

// --------------------------------------------------------------------------
//
// Compute signal mean and rms in the whole run and store it in MPedestalCam
//
Int_t MPedCalcFromLoGain::PostProcess()
{
    // Compute pedestals and rms from the whole run
    if (fPedestalUpdate)
        return kTRUE;

    *fLog << flush << inf << "Calculating Pedestals..." << flush;

    CalcPixResult();
    CalcAreaResult();
    CalcSectorResult();

    fPedestalsOut->SetReadyToSave();

    return MExtractPedestal::PostProcess();
}

// --------------------------------------------------------------------------
//
//  The following resources are available:
//
Int_t MPedCalcFromLoGain::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Int_t rc=MExtractPedestal::ReadEnv(env, prefix, print);
    if (rc==kERROR)
        return kERROR;

    // find resource for pedestal update
    if (IsEnvDefined(env, prefix, "PedestalUpdate", print))
    {
        SetPedestalUpdate(GetEnvValue(env, prefix, "PedestalUpdate", fPedestalUpdate));
        rc = kTRUE;
    }

    // find resource for numdump
    if (IsEnvDefined(env, prefix, "NumDump", print))
    {
        const Int_t num = GetEnvValue(env, prefix, "NumDump", -1);
        if (num<=0)
        {
            *fLog << err << GetDescriptor() << ": ERROR - NumDump invalid!" << endl;
            return kERROR;
        }

        SetNumDump(num);
        rc = kTRUE;
    }

    // find resource for numeventsdump
    if (IsEnvDefined(env, prefix, "NumEventsDump", print))
    {
        SetNumEventsDump(GetEnvValue(env, prefix, "NumEventsDump", (Int_t)fNumEventsDump));
        rc = kTRUE;
    }

    // find resource for numeventsdump
    if (IsEnvDefined(env, prefix, "NumAreasDump", print))
    {
        SetNumAreasDump(GetEnvValue(env, prefix, "NumAreasDump", (Int_t)fNumAreasDump));
        rc = kTRUE;
    }

    // find resource for numeventsdump
    if (IsEnvDefined(env, prefix, "NumSectorsDump", print))
    {
        SetNumSectorsDump(GetEnvValue(env, prefix, "NumSectorsDump", (Int_t)fNumSectorsDump));
        rc = kTRUE;
    }

    return rc;
}

void MPedCalcFromLoGain::Print(Option_t *o) const
{
    MExtractPedestal::Print(o);

    *fLog << "Pedestal Update is            " << (fPedestalUpdate?"on":"off") << endl;
    if (fPedestalUpdate)
    {
        *fLog << "Num evts for pedestal   calc: " << fNumEventsDump << endl;
        *fLog << "Num evts for avg.areas  calc: " << fNumAreasDump << endl;
        *fLog << "Num evts for avg.sector calc: " << fNumSectorsDump << endl;
    }
}
