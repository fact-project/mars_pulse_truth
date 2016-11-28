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
!   Author(s): Thomas Bretz, 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!              Markus Gaug,  3/2004 <mailto:markus@ifae.es>
! 
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
// MBadPixelsPix
//
// The bits of an integer array fInfo are used to declare and inform about 
// possible defects in a pixel. Default and absence of defects create an array
// of zeros. 
//
// The first index (fInfo[0]) holds general information which is coded as follows:
// * BIT(1): Unsuitable Run: The pixel is not suited for analysis for the entire run
// * BIT(2): Unsuitable Evt: The pixel is not suited for analysis for the current event
// * BIT(3): Unreliable Run: The pixel can in principle be used for analysis, although 
//                           previous analysis steps have yielded certain inconsistencies
//
// These bits can be called with the enum MBadPixelsPix::UnsuitableTupe_t in combination 
// with the function IsUnsuitable(MBadPixelsPix::UnsuitableTupe_t), e.g. 
// MBadPixelsPix::IsUnsuitalbe(MBadPixelsPix::kUnsuitableRun) asks if the first bit is set. 
//
// The second index (fInfo[1]) hold information acquired during the calibration. The bits 
// are coded in the following form:
//
//
// * Set bits leading to an unsuitable flag:
//
// BIT(7 ): kLoGainSaturation     :  The Low Gain signals were saturated during calibration
// BIT(8 ): kChargeIsPedestal     :  The calibration signal contained only pedestals - presumably dead pixel
// BIT(12): kMeanTimeInFirstBin   :  The signal has its mean maximum in the first used FADC slice - signal extractor bad
// BIT(13): kMeanTimeInLast2Bins  :  The signal has its mean maximum in the last two used FADC slice - signal extractor bad
// BIT(14): kDeviatingNumPhes     :  The calculated number of phes deviates by more than +6-5.5 sigma of the phes mean of the same area idx.
// BIT(19): kHiGainOverFlow       :  The Hi-Gain calibration histogram showed overflow in more than 0.5% of the events
// BIT(20): kLoGainOverFlow       :  The Lo-Gain calibration histogram showed overflow in more than 0.5% of the events
// BIT(23): kDeadPedestalRms      :  The pedestal RMS was 4.5 sigma below or 25 sigma above the average per area idx.
// BIT(24): kFluctuatingArivalTimes: The RMS of the position of the pulse maximum is larger than 3.5 FADC slices.
// BIT(24): kLoGainBlackout       :  A high gain saturated pixel had too many blackout events in the low gain
//
//
// * Set bits leading to an unreliable flag:
// 
// BIT(1 ): kHiGainNotFitted     :  Any High Gain signal is calibrated without a Gauss Fit to the signal distribution
// BIT(2 ): kLoGainNotFitted     :  Any Low  Gain signal is calibrated without a Gauss Fit to the signal distribution
// BIT(3 ): kRelTimeNotFitted    :  Any High Gain signal's arrival times are calibrated without a Gauss Fit
// BIT(4 ): kHiGainOscillating   :  The High Gain signals fourier transform showed abnormal behavior  
// BIT(5 ): kLoGainOscillating   :  The Low  Gain signals fourier transform showed abnormal behavior  
// BIT(6 ): kRelTimeOscillating  :  The High Gain arrival times fourier transform showed abnormal behavior  
// BIT(11): kChargeSigmaNotValid :  The sigma of the signal distribution is smaller than the pedestal RMS - presumably a pixel with a star in its FOV only during the pedestal taking
// BIT(16): kDeviatingFFactor    :  The calculated overall F-Factor deviates too much from the mean - inconsistency
// BIT(15): kDeviatingNumPhots   :  The calculated number of calibrated photons deviates too much from the mean - inconsistency
//
//
// * Set bits leading to not useable low-gain signal:
//
// BIT(17): kConversionHiLoNotValid: The inter-calibration constant between Hi-Gain and Low Gain does not exist.
//
// These bits can be called with the enum MBadPixelsPix::UncalibratedType_t in combination 
// with the function IsUncalibrated(MBadPixelsPix::UncalibratedTupe_t), e.g. 
// MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kHiGainNotCalibrated) asks if the Hi Gain signal
// could be calibrated.
//
// The third index (fInfo[2]) holds information about possible hardware mulfunctionning. The bits 
// are coded in the following form:
// BIT(1 ): kHVNotNominal :  The HV deviates more than 3% from the nominal value.
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsPix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MBadPixelsPix);

using namespace std;

const Int_t MBadPixelsPix::fgRunMask =
    MBadPixelsPix::kUnsuitableRun |
    MBadPixelsPix::kUnreliableRun;

// ------------------------------------------------------------------------
//
// Initialize Pixel to be Ok.
//
MBadPixelsPix::MBadPixelsPix(const char* name, const char* title)
{
    fName  = name  ? name  : "MBadPixelsPix";
    fTitle = title ? title : "Container storing bad pixel information for a single pixel";

    fInfo.Set(3);

}

// ------------------------------------------------------------------------
//
// Invalidate all bits which are not run-wise. This will be called for
// all entries in the parameter list, just before each time the task-list
// is executed.
//
void MBadPixelsPix::Reset()
{
    fInfo[0] &= fgRunMask;
}

// ------------------------------------------------------------------------
//
// Invalidate values (set=0 which mean Pixel OK)
//
void MBadPixelsPix::Clear(Option_t *o)
{
    if (TString(o).IsNull())
    {
        fInfo[1] &= BIT(0);
        fInfo[0]  = fInfo[1] ? kUnsuitableRun : 0;
        fInfo[2]  = 0;
    }
    else
        fInfo.Reset(0);
}

// ------------------------------------------------------------------------
//
// Merge (bitwise or) the information in pix into this pixel.
//
void MBadPixelsPix::Merge(const MBadPixelsPix &pix)
{
  const Int_t n = pix.fInfo.GetSize();
  if (n>fInfo.GetSize())
    fInfo.Set(n);
  
  for (int i=0; i<n; i++)
    fInfo[i] |= pix.fInfo[i];
}


/****************************************************************************
           This is a collection of possible defects for later use
 ****************************************************************************/

/*
 1  PMT defective.
 2  Preamplifier defective.
 3  Optical link defective.
 4  HV cannot be set.
 7  HV unstable.
 5  HV readout defective.
 8  DC unstable.
 6  DC readout defective.
 9  Discriminator threshold cannot be set.
 10  Trigger delay cannot be set.
 11  Discriminator gives no output.
 <-? 12  Pixel out of L1T.
 13  FADC defective.
 14  FADC board digital information defective.
 */

/*
 1  Pixel shows no signal
 */

/*

Hardware defects which cannot be detected automatically by software. This might be stored at least in the data-base. I think we should wait until we implement these things...
Preamplifier defective.
Optical link defective.
HV cannot be set.
HV readout defective.
DC readout defective.
Discriminator threshold cannot be set.
Trigger delay cannot be set.
Discriminator gives no output.
FADC defective.
FADC board digital information defective.
Pixel out of L1T. (this is an important information, but not necessarily a defect, is it?)

*/
