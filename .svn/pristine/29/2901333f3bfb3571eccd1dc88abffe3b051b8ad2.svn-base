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
!   Author(s): Thomas Bretz  12/2004 <mailto:nicola.galante@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2004-2007
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MTriggerPattern
//
//  A container to store the decoded trigger pattern.
//
// The idea is, that this container will never change the meaning of its
// variables, while the trigger pattern itself could.
//
// If new 'features' are necessary the decoding (MTriggerPatternDecode)
// must be changed to correctly decode the pattern into the existing
// MTriggerPattern. If new information is decoded you may have to
// add new variables to this container. Don't forget to increase the
// class version number (ClassDef) and document your change HERE.
//
// For files before file version 5 the trigger pattern is set to 00000000.
//
// --------------------------------------------------------------------------
//
// Here an explanation about the meaning of the Trigger Pattern.
// The trigger pattern is a 16-BIT number where are stored informations
// about which thriggers have been shot on each event. Every bit
// correspond to a prticular kind of trigger (Pedestal, Calibration,
// LT1, LT2, PIN Diode...) but the whole trigger pattern number
// is divided into two parts (from left to right):
// 
// 1) The first concerns unprescaled triggers.
// 2) The second concerns prescaled triggers.
// 
// The prescaler is a devicee installed AFTER the LT2. It collects
// all kind of triggers and can prescale each trigger by a different
// prescaling factor. This means that we can set the prescaler to
// accept every LT2 trigger but only 1% of calibration triggers.
// Therefore LT2 prescaling factor will be set to 1, while CAL prescaling
// factor will be set to 100. If after the prescaler at least one trigger
// survives, then the event is considered "TRIGGERED" and aquired by the DAQ.
// 
// The current BIT meaning is:
// 
// BIT(0):  prescaled LT1
// BIT(1):  prescaled Calibration Trigger
// BIT(2):  prescaled LT2
// BIT(3):  prescaled Pedestal Trigger
// BIT(4):  prescaled Pin Diode
// BIT(5):  prescaled Sum Trigger
// BIT(6):  unused
// BIT(7):  unused
// BIT(8):  unprescaled LT1
// BIT(9):  unprescaled Calibration Trigger
// BIT(10): unprescaled LT2
// BIT(11): unprescaled Pedestal Trigger
// BIT(12): unprescaled Pin Diode
// BIT(13): unprescaled Sum Trigger
// BIT(14): unused
// BIT(15): unused
// 
// Why are we saving both prescaled and unprescaled triggers?
// Which should I look at? Let's give an example:
// 
//    BIT #      15-14-13-12-11-10- 9- 8- 7- 6- 5- 4- 3- 2- 1- 0
// 
//    event #1:   0  0  0  0  0  0  1  1  0  0  0  0  0  0  0  1
//    event #2:   0  0  0  0  0  0  1  1  0  0  0  0  0  0  1  1
// 
// In both cases you have both CAL and LT1 trigger, but in first
// event calibration trigger is prescaled and in second event no
// trigger is prescaled. Imagine you are looking for calibration events. 
// If you look at the prescale bits you are sure that events with CAL
// flag are calibration events (event #2) but you can miss other
// real calibration events (event #1). If you are lucky that
// the related prescaling factor is 1 you won't have this problem,
// otherway you will have it. 
// 
// To select events by the trigger pattern you should use MFTriggerPattern
// filter. This filter uses Require- and Deny- methods to select your
// trigger pattern. Require- methods requires that your trigger bit is ON,
// otherway the event is kicked out. Deny- methods requires that your
// trigger bit is OFF, otherway your event is kicked out. Other bits not
// selected by your Require- or Deny- call are ignored. Let's give an
// example. You want to select all events that have both LT1 and LT2
// trigger but which are not calibration neither Pin Diode events. You
// should look at unprescaled bits to be sure about which were the initial
// triggers. Then you can implement in your macro something like:
// 
//      MFTriggerPattern ftrigpatt;
//      ftrigpatt.RequireTriggerLvl1(MFTriggerPattern::kUnPrescaled);
//      ftrigpatt.RequireTriggerLvl2(MFTriggerPattern::kUnPrescaled);
//      ftrigpatt.DenyCalibration(MFTriggerPattern::kUnPrescaled);
//      ftrigpatt.DenyPinDiode(MFTriggerPattern::kUnPrescaled);
// 
// Then you use in your tasklist as a usual MF filter. In this
// example Pedestal trigger flag is ignored. Consider that by default
// MFTriggerPattern::kUnPrescaled is set for Require- and Deny- methods.
// 
// WARNING: please use MTriggerPatternDecode task to read the trigger pattern
// of the event and to fill MTriggerPattern container. If you use your
// private stuff to read MRawEvtHeader.fTriggerPattern[0] (data member
// where the trigger pattern is stored) you must invert all the bits of
// your number. Current hardware, infact, writes the trigger pattern bit-inverted.
// 
// 
// For further informations contact:
// 
// Nicola Galante        nicola.galante@pi.infn.it
// Riccardo Paoletti     riccardo.paoletti@pi.infn.it
// Antonio Stamerra      antonio.stamerra@pi.infn.it
//
/////////////////////////////////////////////////////////////////////////////
#include "MTriggerPattern.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MTriggerPattern);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MTriggerPattern::MTriggerPattern(const char *name, const char *title)
    : fPrescaled(0), fUnprescaled(0)
{
    fName  = name  ? name  : "MTriggerPattern";
    fTitle = title ? title : "Container for decoded trigger pattern";
}

void MTriggerPattern::Copy(TObject &obj) const
{
    static_cast<MTriggerPattern&>(obj).fPrescaled=fPrescaled;
    static_cast<MTriggerPattern&>(obj).fUnprescaled=fUnprescaled;
}

void MTriggerPattern::Print(Option_t *) const
{
    *fLog << all;
    *fLog << "Trigger Pattern (un/prescaled): ";

    if (fUnprescaled&kPedestal)
        *fLog << "P";
    if (fUnprescaled&kCalibration)
        *fLog << "C";
    if (fUnprescaled&kTriggerLvl1)
        *fLog << "1";
    if (fUnprescaled&kTriggerLvl2)
        *fLog << "2";
    if (fUnprescaled&kSumTrigger)
        *fLog << "S";
    if (fUnprescaled&kPinDiode)
        *fLog << "-";
    *fLog << "/";
    if (fPrescaled&kPedestal)
        *fLog << "P";
    if (fPrescaled&kCalibration)
        *fLog << "C";
    if (fPrescaled&kTriggerLvl1)
        *fLog << "1";
    if (fPrescaled&kTriggerLvl2)
        *fLog << "2";
    if (fPrescaled&kSumTrigger)
        *fLog << "S";
    if (fPrescaled&kPinDiode)
        *fLog << "-";
    *fLog << endl;
}
