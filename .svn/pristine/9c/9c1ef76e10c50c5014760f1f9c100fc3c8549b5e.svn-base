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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Antonio Stamerra, 05/2004 <mailto:antonio.stamerra@pi.infn.it>
! 
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportTrigger
//
// This is the class interpreting and storing the TRIGGER-REPORT information.
//  Updated to add IPR; data format follows TDAS 00-07 ver.6.3 jul-04
//  http://hegra1.mppmu.mpg.de/MAGIC/private/software/doc/control/tdas0007_v6.3.ps.gz
//  
//  *Input:
//
//  The report is divided into 9 sections: 
//  - the name of the Table                 (1 field)
//  - the cell rates                       (32 fields)
//  - L1 and L2 table name                 ( 2 fields)
//  - prescaling factors                  (2x8 fields)
//  - livetime and deadtime               (5x4 fields)
//  - L2 output bit rates                (20 integers)
//  - global rates (before/after presc.)    (2 floats)
//  - 18 dummy fields                      (18 fields)
//  - IPR                    (325 hexs + 397 integers) 
//
//  *Output:
//
//  The values read from the report string are used to fill the following 
//  containers:
//  - MTriggerIPR        (Individual Pixel Rates)
//  - MTriggerCell       (Rate of trigger cells)
//  - MTriggerBit        (Output Bits from prescaler (before and after presc.)
//  - MTriggerPrescFact  (Prescaling factors for each bit)
//  - MTriggerLiveTime   (Values of counters for dead/livetime)
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportTrigger.h"

#include "MParList.h"

#include "MLogManip.h"

#include "MTriggerIPR.h"
#include "MTriggerCell.h"
#include "MTriggerBit.h"
#include "MTriggerPrescFact.h"
#include "MTriggerLiveTime.h"

#include "MReportFileRead.h"

ClassImp(MReportTrigger);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "TRIGGER-REPORT"
//
MReportTrigger::MReportTrigger() : MReport("TRIGGER-REPORT")
{
    fName  = "MReportTrigger";
    fTitle = "Class for TRIGGER-REPORT information";
}

// --------------------------------------------------------------------------
//
// FindCreate the following objects:
//  - MTriggerIPR
//  - MTriggerCell
//  - MTriggerBit
//  - MTriggerPrescFact
//  - MTriggerLiveTime
//
Bool_t MReportTrigger::SetupReading(MParList &plist)
{
  fIPR = (MTriggerIPR*)plist.FindCreateObj("MTriggerIPR");
  if (!fIPR)
    return kFALSE;

  fCell = (MTriggerCell*)plist.FindCreateObj("MTriggerCell");
  if (!fCell)
    return kFALSE;

  fBit = (MTriggerBit*)plist.FindCreateObj("MTriggerBit");
  if (!fBit)
    return kFALSE;

  fPrescFactor = (MTriggerPrescFact*)plist.FindCreateObj("MTriggerPrescFact");
  if (!fPrescFactor)
    return kFALSE;

  fLiveTime = (MTriggerLiveTime*)plist.FindCreateObj("MTriggerLiveTime");
  if (!fLiveTime)
    return kFALSE;
  
  return MReport::SetupReading(plist);
}


// --------------------------------------------------------------------------
//
// Interprete the Cell rates section of the report
//  Read 32 floats separated with a blank
//
Bool_t MReportTrigger::InterpreteCell(TString &str)
{
  Int_t len=0, n, i=0;
  Int_t gsNCells=32;

  for (i=0;i<gsNCells;i++)
    {
      n = sscanf(str.Data(), " %f %n", &fCell->fCellRate[i], &len);
      if (n!=1)
	{
	  *fLog << warn << "WARNING - Cell Scaler Value #" << i << " missing." << endl;
	  return kCONTINUE;
	}
      str.Remove(0, len); // Remove cell rates from report string
    }

  str=str.Strip(TString::kLeading);  

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the Prescaling factors section of the report
//
Bool_t MReportTrigger::InterpretePrescFact(TString &str)
{
  Int_t len=0, n, i=0;
  Int_t gsNPrescFacts=8;
  
  str.Remove(0, 1);

  for (i=0;i<gsNPrescFacts;i++)
    {
    const Int_t ws = str.First(' ');
    if (ws<=0)
       {
	 *fLog << warn << "WARNING - Cannot determine Prescaling factor #" << i << " descriptor" << endl;
        return kCONTINUE;
       }
    TString descriptor = str(0, ws);
    str.Remove(0, ws);
      
      n = sscanf(str.Data(), " %li %n", &fPrescFactor->fPrescFact[i], &len);
      if (n!=1)
	{
	  *fLog << warn << "WARNING - prescaler factor " << i << " missing." << endl;
	  return kCONTINUE;
	}
      str.Remove(0, len); // Remove Prescal. factors from report string
    }
  str=str.Strip(TString::kLeading);  
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the Scalers with Live-Deadtime section of the report
//  Read 4x5 integers separated with a blank
//  There are 5 scalers,each one with the live and deadtime.
//  Live and deadtimes have two fields, with the most significant
//  and less significant bits.
//
Bool_t MReportTrigger::InterpreteLiveTime(TString &str)
{
  Int_t len, n, i=0;
  Int_t gsNScalers=5;
  Int_t  dLSB, dMSB,lLSB, lMSB;

  for (i=0;i<gsNScalers;i++)
    {
      n = sscanf(str.Data(), " %d %d %d %d %n", &lLSB, &lMSB,&dLSB, &dMSB, &len);
      if (n!=4)
	{
	  *fLog << warn << "WARNING - Live-Deadtime Scaler Value #" << i << " missing." << endl;
	  return kCONTINUE;
	}

      str.Remove(0, len); // Remove Live-Deadtimes from string

      //convert to seconds and fill container
      // (FIXME! only the MSB (seconds is now considered)
      (fLiveTime->fLiveTime)[i] = lMSB;
      (fLiveTime->fDeadTime)[i] = dMSB;      
    }

  str=str.Strip(TString::kLeading);  

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the Bit rates section of the report
// 20 integers. First and last two are not used
//
Bool_t MReportTrigger::InterpreteBit(TString &str)
{
  Int_t len, n, i=0;
  Int_t gsNBits=20;
  
  for (i=0;i<gsNBits;i++)
    {
      n = sscanf(str.Data(), " %f %n", &fBit->fBit[i], &len);
      if (n!=1)
	{
	  *fLog << warn << "WARNING - Bit rate #" << i << " missing." << endl;
	  return kCONTINUE;
	}
      str.Remove(0, len); // Remove output bit rates from string
    }

  str=str.Strip(TString::kLeading);  

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the L1 and L2 table names
// 1String + 1Int +1 String
//
Bool_t MReportTrigger::InterpreteL1L2Table(TString &str)
{
    const Int_t wsL1 = str.First(' ');
 
    if (wsL1<=0)
       {
        *fLog << warn << "WARNING - Cannot determine name of L1 trigger table." << endl;
        return kCONTINUE;
       }
    
    fL1Tablename = str(0, wsL1);
    str.Remove(0, wsL1);

    // remove an integer between names
    Int_t len;
    Int_t mi;
    Int_t n=sscanf(str.Data(), "%d %n", &mi, &len);
    if (n!=1)
      {
        *fLog << warn << "WARNING - Not enough arguments." << endl;
        return kCONTINUE;
    }
    str.Remove(0, len);

    // L2 tablename
    const Int_t wsL2 = str.First(' ');
 
    if (wsL2<=0)
      {
        *fLog << warn << "WARNING - Cannot determine name of L2 trigger table." << endl;
        return kCONTINUE;
      }    
    fL2Tablename = str(0, wsL2);
    str.Remove(0,wsL2);
    str.Strip(TString::kBoth);
    
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete an unused section of the report 
// 18 integers
//
Bool_t MReportTrigger::InterpreteDummy(TString &str)
{
  Int_t len, n, i=0;
  Int_t gsNDummies=18;
  Int_t dummy;  

  for (i=0;i<gsNDummies;i++)
    {
      n = sscanf(str.Data(), " %d %n", &dummy, &len);
      if (n!=1)
	{
	  *fLog << warn << "WARNING - Dummy #" << i << " missing." << endl;
	  return kCONTINUE;
	}
      str.Remove(0, len); // Remove dummies from report string

    }

  str=str.Strip(TString::kLeading);  


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the IPR section of the report
//   sep-04
//   The IPR are saved as 340 (=number of pixs in trigger area) hex numbers 
//   and as 397 (= #pixs in inner region) dec numbers.
//   Only the dec numbers are now saved in the MTriggerIPR container.
//  

Bool_t MReportTrigger::InterpreteIPR(TString &str)
{

  Int_t gsNhexIPR=340; //number of IPR saved in hex format
  Int_t gsNdecIPR=397; //number of IPR saved in dec format
  
  // Read Individual pixel rates in hex format
  const char *pos = str.Data();
  const char *end = str.Data() + gsNhexIPR*8;
  
  Int_t i=0,n,len;
  short dummy;
  while (pos < end)
    {
      const Char_t hex[9] = { pos[0], pos[1], pos[2], pos[3],pos[4],pos[5],pos[6],pos[7],0 };
      n = sscanf(hex, "%hx", &dummy);
      pos+=8;
      if (n!=1)
        {
	  *fLog << warn << "WARNING - Rate #" << i << " missing." << endl;
	  return kFALSE;
        }
    }
    
  str.Remove(0, end-str.Data()); // Remove IPR hex  from report string
  str.Strip(TString::kBoth);
  
  // ------  
  // Read Individual pixel rates in dec format 
  // and save them in the MTriggerIPR container
  
  for (i=0;i<gsNdecIPR;i++)
    {
      n = sscanf(str.Data(), " %ld %n", &fIPR->fIPR[i], &len);
      if (n!=1)
	{
	  *fLog << warn << "WARNING - IPR dec #" << i << " missing." << endl;
	  return kCONTINUE;
	}
      str.Remove(0, len); // Remove IPR dec from report string      
    }
  
  str=str.Strip(TString::kLeading);  

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Interprete the body of the TRIGGER-REPORT string
//  Read comments for details
//
Int_t MReportTrigger::InterpreteBody(TString &str, Int_t ver )
{

    str = str.Strip(TString::kLeading);
 
    // Extract trigger table name
    const Int_t ws = str.First(' ');
 
   if (ws<=0)
    {
        *fLog << warn << "WARNING - Cannot determine name of trigger table." << endl;
        return kCONTINUE;
    }

    fTablename = str(0, ws);
    str.Remove(0, ws);

    // Check the Version of CC file, and takes care of the differences
    // introduced in the format of the report (on May 2004).

    if (ver < 200405050)	
      {
	*fLog << warn << " WARNING - This is  an old TRIGGER-REPORT without IPRs" <<endl;
        return InterpreteOldBody(str);
      }

    // Read the cell rates (32 fields)
    if (!InterpreteCell(str))
      return kCONTINUE;

    // Read L1 and L2 table name (2 fields)
    if (!InterpreteL1L2Table(str))
      return kCONTINUE;    

    // Read prescaling factors  (2x8 fields)
    if (!InterpretePrescFact(str))
      return kCONTINUE;
    
    // Read livetime and deadtime (5x4 fields)
    if (!InterpreteLiveTime(str))
      return kCONTINUE;

    // Read L2 outout bit rates
    if (!InterpreteBit(str))
      return kCONTINUE;

    // Read global rates (before and after prescaling)
    Int_t len, n;
    n = sscanf(str.Data(), " %f %f %n", &fL2BeforePrescaler, &fL2AfterPrescaler, &len);
    if (n!=2)
    {
         *fLog << warn << "WARNING - Couldn't read Trigger rates." << endl;
        return kFALSE;
    }
    str.Remove(0,len);
    str.Strip(TString::kBoth);

    // Read 18 dummy fields
    if (!InterpreteDummy(str))
      return kCONTINUE;

    // Read IPR
    if (!InterpreteIPR(str))
      return kCONTINUE;

    return str==(TString)"OVER" ? kTRUE : kCONTINUE;
}


// --------------------------------------------------------------------------
//
// Interprete the body of the TRIGGER-REPORT string
//  for OLD runs (older than may-04)
//
Bool_t MReportTrigger::InterpreteOldBody(TString &str)
{

  Int_t len, n;
  Float_t fPrescalerRates[100]; 

    const char *pos = str.Data();
    for (int i=0; i<19; i++)
      {
        n = sscanf(pos, " %f %n", &fPrescalerRates[i], &len);
        if (n!=1)
	  {
            *fLog << warn << "WARNING - Scaler Value #" << i << " missing." << endl;
            return kCONTINUE;
	  }
        pos += len;
      }

    n = sscanf(pos, " %f %f %n", &fL2BeforePrescaler, &fL2AfterPrescaler, &len);
    if (n!=2)
      {
        *fLog << warn << "WARNING - Couldn't read Trigger rates." << endl;
        return kFALSE;
      }
    pos += len;
    for (int i=0; i<11; i++)
      {
        Float_t dummy;
        n = sscanf(pos, " %f %n", &dummy/*fRates[i]*/, &len);
        if (n!=1)
	  {
            *fLog << warn << "WARNING - Rate #" << i << " missing." << endl;
            return kFALSE;
	  }
        pos += len;
      }
    str.Remove(0, pos-str.Data());
    str.Strip(TString::kBoth);
    
    return str==(TString)"OVER" ? kTRUE : kCONTINUE;
}
