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
!   Author(s): Nicola Galante  12/2004 <mailto:nicola.galante@pi.infn.it>
!   Author(s): Thomas Bretz 12/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MTriggerPatternDecode
//
//  Decodes the trigger pattern from MRawEvtData into MTriggerPattern.
//
// For files before file version 5 the trigger pattern is set according
// to the file type
//
// For more details see: MTriggerPattern
//
// Input:
//   MRawEvtData
//
// Output:
//   MTriggerPattern
//
/////////////////////////////////////////////////////////////////////////////
#include "MTriggerPatternDecode.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"
#include "MTriggerPattern.h"

ClassImp(MTriggerPatternDecode);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MTriggerPatternDecode::MTriggerPatternDecode(const char *name, const char *title)
    : fRunHeader(0), fEvtHeader(0), fPattern(0)
{
    fName  = name  ? name  : "MTriggerPatternDecode";
    fTitle = title ? title : "Task to decode Trigger Pattern";
}

// --------------------------------------------------------------------------
//
Int_t MTriggerPatternDecode::PreProcess(MParList *pList)
{
    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
	*fLog << err << "MRawRunHeader not found... abort." << endl;
	return kFALSE;
    }

    fEvtHeader = (MRawEvtHeader*)pList->FindObject("MRawEvtHeader");
    if (!fEvtHeader)
    {
	*fLog << err << "MRawEvtHeader not found... abort." << endl;
	return kFALSE;
    }

    fPattern = (MTriggerPattern*)pList->FindCreateObj("MTriggerPattern");
    if (!fPattern)
	return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MTriggerPatternDecode::Process()
{
    if (fRunHeader->GetCameraVersion()==0xfac7)
    {
        UInt_t pat = 0;

        if (fEvtHeader->GetTriggerID()&0x4)
            pat |= MTriggerPattern::kTriggerLvl1;

        if (fEvtHeader->GetTriggerID()&0x400)
            pat |= MTriggerPattern::kPedestal;

        if (fEvtHeader->GetTriggerID()&0x100)
            pat |= MTriggerPattern::kCalibration;

        if (fEvtHeader->GetTriggerID()&0x8000)
            pat |= MTriggerPattern::kSumTrigger;

        fPattern->fPrescaled   = pat;
        fPattern->fUnprescaled = pat;

        return kTRUE;
    }

    if (fRunHeader->GetFormatVersion()<5)
    {
        switch (fRunHeader->GetRunType()&0xff)
        {
        case MRawRunHeader::kRTData:
            fPattern->fPrescaled   = MTriggerPattern::kTriggerLvl1;
            fPattern->fUnprescaled = MTriggerPattern::kTriggerLvl1;
            return kTRUE;

        case MRawRunHeader::kRTPedestal:
            fPattern->fPrescaled   = MTriggerPattern::kPedestal;
            fPattern->fUnprescaled = MTriggerPattern::kPedestal;
            return kTRUE;

        case MRawRunHeader::kRTCalibration:
            fPattern->fPrescaled   = MTriggerPattern::kCalibration;
            fPattern->fUnprescaled = MTriggerPattern::kCalibration;
            return kTRUE;
        }
        return kTRUE;
    }

    const UInt_t pattern = ~fEvtHeader->GetTriggerID();

    // The the trigger pattern is currently written with inverted bits,
    // but in the future this could be changed. In this case the file version
    // number of the raw-data format must be checked. It tells you the
    // file format (meaning of the bits)
    // If for some reason the file-format has not been correctly changed
    // use the run-number to decide whether the bits must be inverted or not.

    // Trigger Pattern is a number of 16 bits, but if the machine
    // is a 64 bits then the bit inversion the first 16 bits are set to 1,
    // possibly giving problems with the filtering with the masks.
    // Because UInt_t by definition is a 32 bit number on any
    // machine (see root manual) no check is needed.
    // The simplest workaround is:
    //   pattern &= 0xffffffff;

    // For the moment the meaning of the bits in
    // MRawEvtHeader::GetTriggerID and in MTriggerPattern::fTriggerPattern
    // are identical - this may change in the future! In this case
    // the decoding HERE must be changed - NOT MTriggerPattern.
    // If an enhancement of the information stored in MTriggerPattern
    // is necessary ADD new variables! Don't change the meaning of the old
    // ones!
    fPattern->fPrescaled   =  pattern     & 0xff;
    fPattern->fUnprescaled = (pattern>>8) & 0xff;

    // This is a workaround for the new scheme in which L1TPU (the signal
    // comming directly from the L1 is connected, but the L1 (routed
    // over L2 is disconnected)
    if (!fRunHeader->IsMonteCarloRun() && fRunHeader->GetTelescopeNumber()==1 &&
        fRunHeader->GetRunNumber()>1006246)
    {
        fPattern->fPrescaled   |= (pattern>> 6)&1;
        fPattern->fUnprescaled |= (pattern>>14)&1;

        fPattern->fPrescaled   &= 0xbf;
        fPattern->fUnprescaled &= 0xbf;
    }

    return kTRUE;
}
