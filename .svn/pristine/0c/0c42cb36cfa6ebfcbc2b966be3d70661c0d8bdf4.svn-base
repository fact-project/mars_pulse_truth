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
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
// MExtractor
// ==========
//
//   Base class for the signal extractors, used the functions 
//   FindSignalHiGain() and FindSignalLoGain() to extract the signal and 
//   substract the pedestal value    
//
//   The following variables have to be set by the derived class and 
//   do not have defaults:
//   - fNumHiGainSamples
//   - fNumLoGainSamples
//   - fSqrtHiGainSamples
//   - fSqrtLoGainSamples
//
//   The signal extractor classes can be setup from an environmental
//   setup file. For more information see ReadEnv and MEvtLoop::ReadEnv
//
//
// IMPORTANT: For all classes you derive from MExtractor make sure that:
//    - Print() is correctly implemented
//    - Clone() works
//    - Class Version number != 0 and the I/O works PERFECTLY
//    - only data members which are necessary for the setup (not the ones
//      created in PreProcess and Process) are written
//    - the version number is maintained!
//    - If the flag fNoiseCalculation is set, the signal extractor should 
//      calculate the pure noise contriubtion from a fixed window in time.
//
// The following figure gives and example of possible inheritance trees. 
// An extractor class can inherit from each of the following base classes:
//    - MExtractor
//    - MExtractTime
//    - MExtractTimeAndCharge
//
//Begin_Html
/*
<img src="images/ExtractorClasses.gif">
*/
//End_Html
//
//
// Class Version 6:
// ----------------
//  + Float_t fResolutionPerPheHiGain; // Extractor-dependent charge resolution per phe for high-gain (see TDAS-0502).
//  + Float_t fResolutionPerPheLoGain; // Extractor-dependent charge resolution per phe for low-gain  (see TDAS-0502).
//
// Class Version 7:
// ----------------
//  - Byte_t fHiLoLast;                // Number of slices in fLoGainSamples counted for the High-Gain signal
//
// Class Version 8:
// ----------------
//  - Byte_t fSaturationLimit;
//  + Uint_t fSaturationLimit;
//
// Class Version 9:
// ----------------
//  - MPedestalCam fPedestals
//
//
// Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MPedestalCam
//
// Output Containers:
//   MExtractedSignalCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractor.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"
#include "MRawRunHeader.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"
//#include "MPedestalSubtractEvt.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MExtractor);

using namespace std;

const UInt_t  MExtractor::fgSaturationLimit = 245;
const char   *MExtractor::fgNameSignalCam   = "MExtractedSignalCam";
const Float_t MExtractor::fgOffsetLoGain    = 1.51;   // 5 ns

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Set: 
// - all pointers to NULL
// - all variables to 0
// - fSaturationLimit to fgSaturationLimit
// - fNameSignalCam to fgNameSignalCam
// - fNoiseCalculation to kFALSE 
//
MExtractor::MExtractor(const char *name, const char *title)
    : fResolutionPerPheHiGain(0), fResolutionPerPheLoGain(0),
    fSignals(NULL), fRawEvt(NULL), fRunHeader(NULL), fSignal(NULL),
    fLoGainLast(0), fNumHiGainSamples(0), fNumLoGainSamples(0)
{
    fName  = name  ? name  : "MExtractor";
    fTitle = title ? title : "Base class for signal extractors";

    SetNameSignalCam();
    SetOffsetLoGain();
    SetSaturationLimit();
    SetNoiseCalculation(kFALSE);
}

void MExtractor::SetRange(UShort_t hifirst, UShort_t hilast, Int_t lofirst, Byte_t lolast)
{
    fHiGainFirst = hifirst;
    fHiGainLast  = hilast;

    fLoGainFirst = lofirst;
    fLoGainLast  = lolast;
}

//-----------------------------------------------------------------------
//
// - Set the variable fHiLoLast to 0 (will be initialized later in ReInit()
// - Get the pointers to: 
//     MRawEvtData
//     MRawRunHeader
//
Int_t MExtractor::PreProcessStd(MParList *pList)
{

    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }

    fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!fRunHeader)
    {
        *fLog << err << AddSerialNumber("MRawRunHeader") << " not found... aborting." << endl;
        return kFALSE;
    }

    fSignal = (MPedestalSubtractedEvt*)pList->FindObject(AddSerialNumber("MPedestalSubtractedEvt"));
    if (!fSignal)
    {
        *fLog << err << AddSerialNumber("MPedestalSubtractedEvt") << " not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MRawRunHeader
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedSignalCam
//
Int_t MExtractor::PreProcess(MParList *pList)
{
    fSignals = (MExtractedSignalCam*)pList->FindCreateObj("MExtractedSignalCam",AddSerialNumber(fNameSignalCam));
    if (!fSignals)
        return kFALSE;

    return PreProcessStd(pList);
}

// --------------------------------------------------------------------------
//
// The ReInit searches for:
// -  MRawRunHeader::GetNumSamplesHiGain()
// -  MRawRunHeader::GetNumSamplesLoGain()
//
// In case that the variable fLoGainLast is smaller than 
// the even part of the number of samples obtained from the run header, a
// warning is given an the range is set back accordingly. A call to:  
// - SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast-diff) 
// is performed in that case. The variable diff means here the difference 
// between the requested range (fLoGainLast) and the available one. Note that 
// the functions SetRange() are mostly overloaded and perform more checks, 
// modifying the ranges again, if necessary.
//
// In case that the variable fHiGainLast is smaller than the available range 
// obtained from the run header, a warning is given that a part of the low-gain 
// samples are used for the extraction of the high-gain signal. 
//
Bool_t MExtractor::ReInit(MParList *pList)
{
    const Int_t numl = fRunHeader->GetNumSamplesLoGain();
    const Int_t numh = fRunHeader->GetNumSamplesHiGain();
    const Int_t num  = numh+numl;

    if (fHiGainLast>=num)
    {
        *fLog << err << "MExtractor: ERROR - Last hi-gain slice " << (int)fHiGainLast << " must not exceed " << num-1 << endl;
        return kFALSE;
    }
    if (fLoGainLast>=num)
    {
        *fLog << err << "MExtractor: ERROR - Last lo-gain slice " << (int)fLoGainLast << " must not exceed " << num-1 << endl;
        return kFALSE;
    }

    if (numl==0)
    {
        *fLog << inf << "No lo-gains... resetting lo-gain range";
        fLoGainFirst=0;
        fLoGainLast =0;
        *fLog << "." << endl;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MExtractedSignalCam container.
//
Int_t MExtractor::Process()
{
    return kERROR;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MExtractor::StreamPrimitive(ostream &out) const
{
  out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
  out << "\"" << fName << "\", \"" << fTitle << "\");" << endl;
  
  if (fSaturationLimit!=fgSaturationLimit)
  {
      out << "   " << GetUniqueName() << ".SetSaturationLimit(";
      out << (int)fSaturationLimit << ");" << endl;
  }
  
  out << "   " << GetUniqueName() << ".SetRange(";
  out << (int)fHiGainFirst;
  out << ", " << (int)fHiGainLast;
  out << ", " << (int)fLoGainFirst;
  out << ", " << (int)fLoGainLast;
  out << ");" << endl;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MJPedestal.MExtractor.HiGainFirst: 5
//   MJPedestal.MExtractor.LoGainFirst: 5
//   MJPedestal.MExtractor.HiGainLast:  10
//   MJPedestal.MExtractor.LoGainLast:  10
//   MJPedestal.MExtractor.SaturationLimit: 88
//
Int_t MExtractor::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    UShort_t hf = fHiGainFirst;
    UShort_t hl = fHiGainLast;
    Int_t    lf = fLoGainFirst;
    Byte_t   ll = fLoGainLast;

    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "HiGainFirst", print))
    {
        hf = GetEnvValue(env, prefix, "HiGainFirst", hf);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "LoGainFirst", print))
    {
        lf = GetEnvValue(env, prefix, "LoGainFirst", lf);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "HiGainLast", print))
    {
        hl = GetEnvValue(env, prefix, "HiGainLast", hl);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "LoGainLast", print))
    {
        ll = GetEnvValue(env, prefix, "LoGainLast", ll);
        rc = kTRUE;
    }

    SetRange(hf, hl, lf, ll);

    if (IsEnvDefined(env, prefix, "OffsetLoGain", print))
    {
        SetOffsetLoGain(GetEnvValue(env, prefix, "OffsetLoGain", fOffsetLoGain));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "SaturationLimit", print))
    {
        SetSaturationLimit(GetEnvValue(env, prefix, "SaturationLimit", (Int_t)fSaturationLimit));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "NoiseCalculation", print))
    {
        SetNoiseCalculation(GetEnvValue(env, prefix, "NoiseCalculation", fNoiseCalculation));
        rc = kTRUE;
    }

    // Be carefull: Returning kERROR is not forseen in derived classes
    return rc;
}

void MExtractor::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ":" << endl;

    *fLog << " Hi Gain Range:      " << Form("%2d %2d", fHiGainFirst, fHiGainLast) << endl;
    *fLog << " Saturation Lim:     " << Form("%3d", fSaturationLimit) << endl;
    if (HasLoGain())
    {
        *fLog << " Lo Gain Range:      " << Form("%2d %2d", fLoGainFirst, fLoGainLast) << endl;
        *fLog << " Num Samples Hi/Lo:  " << Form("%2.1f %2.1f", fNumHiGainSamples, fNumLoGainSamples) << endl;
    }
}
