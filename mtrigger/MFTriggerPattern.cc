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
!   Author(s): Thomas Bretz  12/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2008
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFTriggerPattern
//
//  A Filter for testing the trigger performance using Trigger Pattern.
//
// For files before file version 5 the trigger pattern is set to 00000000.
//
// To setup the filter you can use Deny, Allow and Require functions.
// Allow is just resetting a bit set by Deny (mostly used in combination
// with DenyAl in advance)
//
// For more details on the meaning of the bits set by these member
// functions see Eval()
//
// For more details on the meaning of the trigger pattern see:
//   MTriggerPattern
//
// Input Containers:
//   MTriggerPattern
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MFTriggerPattern.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTriggerPattern.h"

ClassImp(MFTriggerPattern);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. The default is to allow passing any trigger pattern.
//
MFTriggerPattern::MFTriggerPattern(const char *name, const char *title)
    : fPattern(0), fMaskRequiredPrescaled(0), fMaskRequiredUnprescaled(0),
      fMaskDeniedPrescaled(0), fMaskDeniedUnprescaled(0), fDefault(kTRUE)
{
    fName  = name  ? name  : "MFTriggerPattern";
    fTitle = title ? title : "Filter on Trigger Pattern";
}

// --------------------------------------------------------------------------
//
// Copy constructor
//
MFTriggerPattern::MFTriggerPattern(MFTriggerPattern &trigpatt)
: MFilter(trigpatt)
{
    fMaskRequiredPrescaled   = trigpatt.fMaskRequiredPrescaled;
    fMaskRequiredUnprescaled = trigpatt.fMaskRequiredUnprescaled;

    fMaskDeniedPrescaled     = trigpatt.fMaskDeniedPrescaled;
    fMaskDeniedUnprescaled   = trigpatt.fMaskDeniedUnprescaled;

    fDefault                 = trigpatt.fDefault;
}


// --------------------------------------------------------------------------
//
// Search for MTriggerPattern in the paremeter list.
//
Int_t MFTriggerPattern::PreProcess(MParList *pList)
{
    fPattern = (MTriggerPattern*)pList->FindObject("MTriggerPattern");
    if (!fPattern)
    {
	*fLog << err << "MTriggerPattern not found... abort." << endl;
	return kFALSE;
    }

    memset(fCounter, 0, sizeof(fCounter));

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
// If the prescaled and unprescaled trigger patters is equal 0 fDefault
// is returned.
//
// The returned value is false if any of the require mask bits doesn't
// match a corresponding bit in the trigger bits. If the trigger mask
// contains at least the same bits as in the require mask the returned
// value is true. (Note that this means if the require masks are 0
// true is returned)
//
// This return value can be overwritten by the deny mask. If any of the
// bits in the deny mask will match a trigger bit false will be returned.
//
Int_t MFTriggerPattern::Eval()
{
    const Byte_t p = fPattern->GetPrescaled();
    const Byte_t u = fPattern->GetUnprescaled();
    if (p==0 && u==0)
    {
        fCounter[2]++;
        return fDefault;
    }

    Bool_t rc = kFALSE;

    // Check whether all the bits required are ON
    if ( ((p & fMaskRequiredPrescaled)   == fMaskRequiredPrescaled) &&
         ((u & fMaskRequiredUnprescaled) == fMaskRequiredUnprescaled))
      rc = kTRUE;

    // Now overwrite the result if one of the bits is denied
    if ( (p & fMaskDeniedPrescaled) || (u & fMaskDeniedUnprescaled) )
    {
        fCounter[3]++;
        return kFALSE;
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// See Eval()
//
Int_t MFTriggerPattern::Process()
{
    fResult = Eval();
    fCounter[fResult ? 0 : 1]++;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MFTriggerPattern::PostProcess()
{
    const UInt_t n = GetNumExecutions();
    if (n==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    *fLog << "  " << setw(7) << fCounter[2] << " (" << setw(3);
    *fLog << (int)(fCounter[2]*100/n);
    *fLog << "%) Default (" << (fDefault?"true":"false") << ") returned." << endl;

    *fLog << "  " << setw(7) << fCounter[3] << " (" << setw(3);
    *fLog << (int)(fCounter[3]*100/n);
    *fLog << "%) Trigger denied." << endl;

    *fLog << " " << setw(7) << fCounter[0] << " (" << setw(3);
    *fLog << (int)(fCounter[0]*100/n);
    *fLog << "%) Accepted trigger pattern." << endl;

    *fLog << " " << setw(7) << fCounter[1] << " (" << setw(3);
    *fLog << (int)(fCounter[1]*100/n);
    *fLog << "%) Rejected trigger pattern!" << endl;
    *fLog << endl;

    return kTRUE;
}

// -------------------------------------------------------------------------
//
// Require that a prescaled or unprescaled bit in the trigger pattern is
// passed. The bit is defined by mask, the prescaling by prescaled. The
// default is unprescaled.
//
// Because it doesn't make sense to require a denied bit we reset
// the deny bit at the same time.
//
void MFTriggerPattern::Require(const Byte_t mask, Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskRequiredPrescaled |=  mask) : (fMaskRequiredUnprescaled |=  mask);
    prescaled==kPrescaled ? (fMaskDeniedPrescaled   &= ~mask) : (fMaskDeniedUnprescaled   &= ~mask);
}

// -------------------------------------------------------------------------
//
// Deny that a prescaled or unprescaled bit in the trigger pattern is
// passed. The bit is defined by mask, the prescaling by prescaled. The
// default is unprescaled.
//
// Because it doesn't make sense to deny a required bit we reset
// the require bit at the same time.
//
void MFTriggerPattern::Deny(const Byte_t mask, Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskDeniedPrescaled   |=  mask) : (fMaskDeniedUnprescaled   |=  mask);
    prescaled==kPrescaled ? (fMaskRequiredPrescaled &= ~mask) : (fMaskRequiredUnprescaled &= ~mask);
}

// -------------------------------------------------------------------------
//
// Remove the given bits from the deny-mask. Thus you can first deny
// all bits to pass and then define which bit you want to allow
// to pass. The bit is defined by mask, the prescaling by prescaled. The
// default is unprescaled.
//
void MFTriggerPattern::Allow(const Byte_t mask, Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskDeniedPrescaled &= ~mask) : (fMaskDeniedUnprescaled &= ~mask);
}


// -------------------------------------------------------------------------
//
// Deny all bits (i.e. also require non bits) for the given prescaling
// option. The prescaling is defined by prescaled. The default is
// unprescaled.
//
void MFTriggerPattern::DenyAll(Prescale_t prescaled)
{
    Deny(0xff, prescaled);
}

// -------------------------------------------------------------------------
//
// Allow all bits. resets the deny mask for the given prescaling option,
// but keeps the require mask unchanged. The prescaling is defined
// by prescaled. The default is unprescaled.
//
void MFTriggerPattern::AllowAll(Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskDeniedPrescaled = 0) : (fMaskDeniedUnprescaled = 0);
}

// -------------------------------------------------------------------------
//
// Low level settings. USE THESE ONLY IF YOU ARE AN EXPERT!
//
// You can concatenate bits either by using MTriggerPatter:
//   eg. MTriggerPattern::kTriggerLvl1 & MTiggerPattern::kTriggerLvl2
// of by hexadecimal values:
//   eg. 0xab
//
//  while 0xab can be decoded like:
//
//                                   8421 8421
//       0xa=10=8+2 0xb=11=8+2+1 --> 1010 1011
//
// or vice versa it is easy to get a hexadecimal number from a bit pattern,
//   eg.
//
//       8421 8421
//       0101 1101  -->  4+1=5=0x5 8+4+1=13=0xd --> 0x5d
//
void MFTriggerPattern::SetMaskRequired(const Byte_t mask, Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskRequiredPrescaled = mask) : (fMaskRequiredUnprescaled = mask);
}

// -------------------------------------------------------------------------
//
// Low level settings. USE THESE ONLY IF YOU ARE AN EXPERT!
//
// You can concatenate bits either by using MTriggerPatter:
//   eg. MTriggerPattern::kTriggerLvl1 & MTiggerPattern::kTriggerLvl2
// of by hexadecimal values:
//   eg. 0xab
//
//  while 0xab can be decoded like:
//
//                                   8421 8421
//       0xa=10=8+2 0xb=11=8+2+1 --> 1010 1011
//
// or vice versa it is easy to get a hexadecimal number from a bit pattern,
//   eg.
//
//       8421 8421
//       0101 1101  -->  4+1=5=0x5 8+4+1=13=0xd --> 0x5d
//
void MFTriggerPattern::SetMaskDenied(const Byte_t mask, Prescale_t prescaled)
{
    prescaled==kPrescaled ? (fMaskDeniedPrescaled  = mask) : (fMaskDeniedUnprescaled  = mask);
}

// -------------------------------------------------------------------------
//
// Create the mask to allow a particular (un)prescaled trigger pattern.
//
// Possible arguments are (upper/lower case is ignored):
//
//           "LT1"  : Trigger Level 1 flag
//           "CAL"  : Calibration flag
//           "LT2"  : Trigger Level 2 flag
//           "PED"  : Pedestal flag
//           "PIND" : Pin Diode flag
//           "SUMT" : Sum Trigger flag
// 
// concatenations of these strings are allowed and considered as 
// a logic "and", while trigger pattern flags not considered are
// anyway allowed. To deny a particular trigger pattern use
// the method Deny
// Example: patt = "lt1 lt2" allows events with trigger pattern flags
// {LT1,CAL,LT2} but not events with flags {LT1,CAL}.
//
void MFTriggerPattern::Require(TString patt, Prescale_t prescaled)
{
    if (patt.Contains("LT1", TString::kIgnoreCase))
        RequireTriggerLvl1(prescaled);

    if (patt.Contains("LT2", TString::kIgnoreCase))
        RequireTriggerLvl2(prescaled);

    if (patt.Contains("CAL", TString::kIgnoreCase))
        RequireCalibration(prescaled);

    if (patt.Contains("PED", TString::kIgnoreCase))
        RequirePedestal(prescaled);

    if (patt.Contains("PIND", TString::kIgnoreCase))
        RequirePinDiode(prescaled);

    if (patt.Contains("SUMT", TString::kIgnoreCase))
        RequireSumTrigger(prescaled);
}

// -------------------------------------------------------------------------
//
// Create the mask to deny a particular (un)prescaled trigger pattern.
//
// This method is there because is not possible to deny trigger patterns
// using only the Require pattern. Possible arguments are (upper/lower
// case is ignored) the flags for:
//
//           "LT1"  : Trigger Level 1
//           "CAL"  : Calibration
//           "LT2"  : Trigger Level 2
//           "PED"  : Pedestal
//           "PIND" : Pin Diode
//           "SUMT" : Sum Trigger
// 
// concatenations of these strings are allowed and considered as 
// a logic "and", while trigger pattern flags not considered are
// anyway allowed.
//
// Example: patt = "lt1 lt2" deny events with trigger pattern flags
// {LT1,CAL,LT2} but not events with flags {LT1,CAL}.
//
void MFTriggerPattern::Deny(TString patt, Prescale_t prescaled)
{
    if (patt.Contains("LT1", TString::kIgnoreCase))
        DenyTriggerLvl1(prescaled);

    if (patt.Contains("LT2", TString::kIgnoreCase))
        DenyTriggerLvl2(prescaled);

    if (patt.Contains("CAL", TString::kIgnoreCase))
        DenyCalibration(prescaled);

    if (patt.Contains("PED", TString::kIgnoreCase))
        DenyPedestal(prescaled);

    if (patt.Contains("PIND", TString::kIgnoreCase))
        DenyPinDiode(prescaled);

    if (patt.Contains("SUMT", TString::kIgnoreCase))
        DenySumTrigger(prescaled);
}
