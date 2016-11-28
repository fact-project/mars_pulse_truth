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
!   Author(s): Marcos Lopez 03/2004 <mailto:marcos@gae.ucm.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MExtrapolatePointingPos
//
//   In the PreProcess, read the drive report and store it in an TSpline.
//   In the Process, use the TSpline to calculate the PointingPos for the 
//   time of the current event.
// 
//  Input Containers:
//    MRawEvtData
//    MReportDrive
//    MTimeDrive
//    MTime
//
//  Output Containers:
//    MPointingPos
//
//
/////////////////////////////////////////////////////////////////////////////

#include "MPointingPosInterpolate.h"

#include "MLog.h"
#include "MLogManip.h"

#include <TSpline.h>

#include "MTaskList.h"
#include "MParList.h"
#include "MEvtLoop.h"
#include "MReadReports.h"
#include "MReportDrive.h"
#include "MPointingPos.h"
#include "MTime.h"
#include "MRawRunHeader.h"
#include "MDirIter.h"

#include <TCanvas.h>

ClassImp(MPointingPosInterpolate);

using namespace std;

const Int_t MPointingPosInterpolate::fgNumStartEvents = 1;
// --------------------------------------------------------------------------
//
//  Constructor
//
MPointingPosInterpolate::MPointingPosInterpolate(const char *name, const char *title)
  : fEvtTime(NULL), fPointingPos(NULL), fRunHeader(NULL), fDirIter(NULL), 
    fSplineZd(NULL), fSplineAz(NULL), fRa(0.), fDec(0.),
    fTimeMode(MPointingPosInterpolate::kEventTime)

{
    fName  = name  ? name  : "MPointingPosInterpolate";
    fTitle = title ? title : "Task to interpolate the pointing positons from drive reports";

    fFilename = "";
    fDebug = kFALSE;

    SetNumStartEvents();
}


MPointingPosInterpolate::~MPointingPosInterpolate()
{
  Clear();
}

void MPointingPosInterpolate::Clear(Option_t *o)
{
  if(fSplineZd)
    delete fSplineZd;
  if(fSplineAz)
    delete fSplineAz;
}

// ---------------------------------------------------------------------------
//
// Read the drive report file for the whole night, a build from it the splines
//
Bool_t MPointingPosInterpolate::ReadDriveReport()
{

    *fLog << inf << "Loading report file \"" << fFilename << "\" into TSpline..." << endl;

    if (!fDebug)
        gLog.SetNullOutput();

    //
    // ParList
    //
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);


    //
    // TaskList
    //
    MReadReports read;
    read.AddTree("Drive");
    if (fDirIter)
      read.AddFiles(*fDirIter);
    else 
      read.AddFile(fFilename);     // after the reading of the trees!!!

    read.AddToBranchList("MReportDrive.*");
    
    tlist.AddToList(&read);
    
    //
    // EventLoop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.PreProcess())
    {
        gLog.SetNullOutput(kFALSE);
        return kFALSE;
    }

    TArrayD reportTime(fNumStartEvents);
    TArrayD currentZd(fNumStartEvents);
    TArrayD currentAz(fNumStartEvents); 
    TArrayD nominalZd(fNumStartEvents);
    TArrayD nominalAz(fNumStartEvents);
    
    Int_t n=1;
    while (tlist.Process())
    { 
	MReportDrive* report = (MReportDrive*)plist.FindObject("MReportDrive");
	MTime* reporttime = (MTime*)plist.FindObject("MTimeDrive");
	
	if(n==1)
	  fFirstDriveTime = *reporttime;
	else 
	  fLastDriveTime = *reporttime;

	//
	// Update the number of entries
	//
	if (n>fNumStartEvents)
	  {
	    reportTime.Set(n);
	    currentZd.Set(n);
	    currentAz.Set(n); 
	    nominalZd.Set(n);
	    nominalAz.Set(n);
	  }
	//
	// Sometimes there are two reports with the same time
	//
	if (n>1)
	  if (reporttime->GetTime() == reportTime[n-2])
	    { 
	      *fLog << warn <<"["<< GetName() 
		    << "]: Warning: this report has the same time that the previous one...skipping it " << endl;
	      continue;
	    }

	reportTime[n-1] = reporttime->GetTime();
	currentZd [n-1] = report->GetCurrentZd();
	currentAz [n-1] = report->GetCurrentAz();
	nominalZd [n-1] = report->GetNominalZd();
	nominalAz [n-1] = report->GetNominalAz();
	fRa             = report->GetRa();
	fDec            = report->GetDec();

	if (fDebug)
	  {
	    *fLog << " GetTime(): " << reporttime->GetTime() << endl;
	    *fLog << " GetCurrentZd(): " << report->GetCurrentZd() << endl;
	  }
	n++;
    }

    tlist.PrintStatistics();

    gLog.SetNullOutput(kFALSE);

    *fLog << inf << GetDescriptor() << ": loaded " << n-1 << " ReportDrive from "
	  << fFirstDriveTime << " to " << fLastDriveTime << endl;

    if (fDebug)
      {
     	for (int i=0;i<reportTime.GetSize();i++)
	  *fLog << i            << " " << reportTime[i] << " " 
 	      	<< currentZd[i] << " " << currentAz[i]  << " " 
		<< nominalZd[i] << " " << nominalAz[i]  << endl;
      }

    fSplineZd = new TSpline3("zenith",
			     reportTime.GetArray(), nominalZd.GetArray(), n-1);
    fSplineAz = new TSpline3("azimuth",
			     reportTime.GetArray(), nominalAz.GetArray(), n-1);

    
    if (fDebug)
    {
        *fLog << n-1 << " " << reportTime.GetSize() << endl;
        *fLog << n-1 << " " << nominalZd.GetSize() << endl;
        *fLog << fFirstDriveTime.GetTime() << " " << fSplineZd->Eval((fFirstDriveTime.GetTime()+fLastDriveTime.GetTime())/2.) << endl;
	TCanvas* c = new TCanvas();
      	c->Divide(2,1);
      	c->cd(1);
   	fSplineZd->Draw();
   	c->cd(2);
   	fSplineAz->Draw();
   	c->Modified();
   	c->Update();
        c->SaveAs("pointing.root");
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
//  Input:
//  - MTime
// 
//  Output:
//  - MPointingPos
//
Int_t MPointingPosInterpolate::PreProcess( MParList *pList )
{

    Clear();

    fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!fRunHeader)
    {
        *fLog << err << "MRunHeader not found... aborting." << endl;
        return kFALSE;
    }
    
    fEvtTime = (MTime*)pList->FindObject("MTime");
    if (!fEvtTime)
    {
        *fLog << err << "MTime not found... aborting." << endl;
        return kFALSE;
    }

    fPointingPos = (MPointingPos*)pList->FindCreateObj("MPointingPos");
    if (!fPointingPos)
      return kFALSE;

    if (fFilename.IsNull() && !fDirIter)
      {
	*fLog << err << "File name is empty or no MDirIter has been handed over... aborting" << endl;
	return kFALSE;
      }

    if( !ReadDriveReport() )
	return kFALSE;

    return kTRUE;
}


// --------------------------------------------------------------------------
//
//  Get the run start time, and get the pointing position for that time
//
Int_t MPointingPosInterpolate::Process()
{

    //const Int_t run = fRunHeader->GetRunNumber();
  const MTime &StartRunTime = fRunHeader->GetRunStart();
  const MTime &EndRunTime   = fRunHeader->GetRunEnd();
  Int_t time = StartRunTime.GetTime();

   switch(fTimeMode)
    {
     case kRunTime:
         //
         // Check that we have drive report for this time
         //

         time = (EndRunTime.GetTime() + StartRunTime.GetTime())/2;

         if( StartRunTime<fFirstDriveTime || StartRunTime>fLastDriveTime)
         {
             *fLog << err << GetDescriptor() << ": Run time " << StartRunTime
                   << " outside range of drive reports  (" << fFirstDriveTime
                   << ", " << fLastDriveTime << ")" << endl;

            if ( *fEvtTime<fFirstDriveTime )  time = fFirstDriveTime.GetTime();
            if ( *fEvtTime>fLastDriveTime )   time = fLastDriveTime.GetTime();

             *fLog << " PointingPos: time = " << time << " (" << *fEvtTime << ")  (zd, az) = (" 
		   << fSplineZd->Eval( time )<< ", "  <<fSplineAz->Eval( time )<< ")" << endl;
         }
         break;

    case kEventTime:
      
        time = fEvtTime->GetTime();       

        //
        // Check that we have drive report for this time
        //
        if( *fEvtTime<fFirstDriveTime || *fEvtTime>fLastDriveTime)
        {
	    if (fDebug)
	      {
		*fLog << err << GetDescriptor() << ": Run time = "
		      << *fEvtTime << " outside range of drive reports  ("
    	              << fFirstDriveTime << ", "<< fLastDriveTime << ")" << endl;
               }		

	    if ( *fEvtTime < (fFirstDriveTime) )   time = fFirstDriveTime.GetTime();
	    else                                   time = fLastDriveTime.GetTime();
		
        }
        break;
    }

   if (fDebug)
     {
       *fLog << " real time : " << time 
	     << " first time: " << fFirstDriveTime.GetTime()
	     << " last time: " << fLastDriveTime.GetTime() << endl;
       *fLog << " PointingPos: time = " << time << " (" << *fEvtTime << ")  (zd, az) = (" 
	     << fSplineZd->Eval( time )<< ", "  <<fSplineAz->Eval( time )<< ")" << endl;
     }
    //
    // Check that we have drive report for this time
    //
    //if( *StartRunTime<fFirstDriveTime || *StartRunTime>fLastDriveTime)
    if( StartRunTime>fLastDriveTime)
    {
	*fLog << err << GetDescriptor() << ": Run time " << StartRunTime
	      << " outside range of drive reports  (" << fFirstDriveTime 
	      << ", " << fLastDriveTime << ")" << endl;
	return kERROR;
    }

    const Double_t zd = fSplineZd->Eval( time );
    const Double_t az = fSplineAz->Eval( time );

    if(TMath::Abs(zd)>90 || TMath::Abs(az)>360)
      {
        *fLog << warn << GetDescriptor() << ": Wrong Interpolated Pointing Position." << endl;
	*fLog << " PointingPos: time = " << time << " (" << *fEvtTime << ")  (zd, az, ra, dec) = (" 
	      << zd << ", "  << az << "," << fRa << "," << fDec << ")" << endl;
      }

    fPointingPos->SetLocalPosition( zd, az );
    //fPointingPos->SetSkyPosition( fRa*TMath::DegToRad()/15, fDec*TMath::DegToRad());
    fPointingPos->SetSkyPosition( fRa, fDec);

    return kTRUE;
}



Int_t MPointingPosInterpolate::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "NumStartEvents", print))
      {
	SetNumStartEvents(GetEnvValue(env, prefix, "NumStartEvents", fNumStartEvents));
	rc = kTRUE;
      }

    if (IsEnvDefined(env, prefix, "TimeMode", print))
      {

	TString dat(GetEnvValue(env, prefix, "TimeMode", ""));
	dat.ToLower();

	rc = kTRUE;

	if (dat.Contains("eventtime"))
	  SetTimeMode(kEventTime);
	else if (dat.Contains("runtime"))
	  SetTimeMode(kRunTime);
	else
	  rc = kFALSE;
      }
    return rc;
}

