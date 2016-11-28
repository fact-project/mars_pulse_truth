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
!   Author(s): Javier López  04/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosFromFile
//
// Task to set the position of the source as a function of run number according
// to the positions read from an input file
//
//  Output Containers:
//    MSrcPosCam
//
//////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "MParList.h"
#include "MSrcPosFromFile.h"

#include "MRawRunHeader.h"
#include "MSrcPosCam.h"

#include "MLog.h"
#include "MLogManip.h"


ClassImp(MSrcPosFromFile);

using namespace std;

static const TString gsDefName  = "MSrcPosFromFile";
static const TString gsDefTitle = "Set the position of the (off axis) source according to input file";

// -------------------------------------------------------------------------
//
// Default constructor. cardpath is the name of the input file (.pos) where the
// source positions are stored in the format Run# x y (in mm). mode indicates whether
// to read or to save the positions of the source in/from the internal histogram
//
MSrcPosFromFile::MSrcPosFromFile(TString cardpath, OnOffMode_t mode, const char *name, const char *title)
  : fRawRunHeader(NULL), fSourcePositionFilePath(cardpath)
{
  fName  = name  ? name  : gsDefName.Data();
  fTitle = title ? title : gsDefTitle.Data();
  SetMode(mode);

  fRunList=0x0;
  fRunSrcPos=0x0;
  fRunMap=0x0;

  fLastRun  = 0;
  fFirstRun = 0;
  fLastValidSrcPosCam=0x0;

  SetInternalHistoName(TString(fName)+"Hist");
}
// -------------------------------------------------------------------------
//
// Destructor
//
MSrcPosFromFile::~MSrcPosFromFile()
{
  if(fRunList)
    delete [] fRunList;
  if(fRunSrcPos)
    delete [] fRunSrcPos;
  if(fRunMap)
    delete fRunMap;
}

// -------------------------------------------------------------------------
//
// Open and read the input file.
Int_t MSrcPosFromFile::PreProcess(MParList *pList)
{
  if(GetMode()==MSrcPlace::kOn)
    {
      // Count the number of runs in the card with the source poistions
      ReadSourcePositionsFile(kCount);
      
      if(!fRunList)
	fRunList = new Int_t[fNumRuns];
      if(!fRunSrcPos)
	fRunSrcPos = new MSrcPosCam[fNumRuns];
      if(!fRunMap)
	fRunMap = new TExMap(fNumRuns);
      
      // Read card with the source poistions
      ReadSourcePositionsFile(kRead);
    }

  if(!MSrcPlace::PreProcess(pList))
    return kFALSE;
  
  fRawRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
  if (!fRawRunHeader)
    {
      *fLog << err << AddSerialNumber("MRawRunHeader") << " not found ... aborting" << endl;
      return kFALSE;
    }
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// In case we enter a new run, look for the position in the read list
// If there is no value for that run, take the previous one
// 
Int_t MSrcPosFromFile::ComputeNewSrcPosition()
{
  const UInt_t run = fRawRunHeader->GetRunNumber();
  if(run!=fLastRun)
    {      
      fLastRun=run;
      MSrcPosCam* srcpos = (MSrcPosCam*)fRunMap->GetValue(run);

      *fLog << inf << "Source position for run " << run;

      if(srcpos)
	fLastValidSrcPosCam = srcpos;
      else if(run>fFirstRun)
	for(UInt_t irun=run-1; irun>=fFirstRun;irun--)
	  if((srcpos=(MSrcPosCam*)fRunMap->GetValue(irun)))
	    {
	      *fLog << inf << " not found in file. Taking position for run "<< irun;
	      fLastValidSrcPosCam = srcpos;
	      break;
	    }
      else if(fLastValidSrcPosCam)
	*fLog << inf << " not found in file. Taking previous position: ";
	
      if(!fLastValidSrcPosCam)
	{
	  *fLog << warn << "MSrcPosFromFile::ComputeNewSrcPosition warning: no value for the first run. Taking first found run in file, run number " << fFirstRun;
	  fLastValidSrcPosCam = (MSrcPosCam*)fRunMap->GetValue(fFirstRun);
	}

      *fLog << inf << "\tX\t" << setprecision(3) << fLastValidSrcPosCam->GetX();
      *fLog << inf << "\tY\t" << setprecision(3) << fLastValidSrcPosCam->GetY() << endl;      
    }

  GetOutputSrcPosCam()->SetXY(fLastValidSrcPosCam->GetX(),fLastValidSrcPosCam->GetY());

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read file with table of source positions as function of run
// 
Int_t MSrcPosFromFile::ReadSourcePositionsFile(UShort_t readmode)
{
  
  ifstream fin(fSourcePositionFilePath);
  if(!fin)
    {
      *fLog << err << "MSrcPosFromFile::ReadSourcePositionsFile. Cannot open file " << fSourcePositionFilePath << endl;
      return kFALSE;
    }
  
  UInt_t run;
  Float_t x,y;
  
  fNumRuns=0;
  
  *fLog << dbg << "MSrcPosFromFile::ReadSourcePositionsFile(" << readmode << ')' << endl;
  while(1)
    {
      fin >> run >> x >> y;
      if(!fFirstRun) fFirstRun=run;
      if(fin.eof())
	break;
      
      switch(readmode)
	{
	case kCount:
	  {
	    
	    *fLog << dbg << "Source position for run " << run;
	    *fLog << dbg << "\tX\t" << x << " mm";
	    *fLog << dbg << "\tY\t" << y << " mm" << endl;
	    
	    fNumRuns++;
	    break;
	  }
	case kRead:
	  {
	    fRunList[fNumRuns] = run;
	    fRunSrcPos[fNumRuns].SetXY(x,y);
	    fRunMap->Add(fRunList[fNumRuns],(Long_t)&(fRunSrcPos[fNumRuns]));
	    fNumRuns++;
	    break;
	  }
	default:
	  *fLog << err << "Read mode " << readmode << " node defined" << endl;
	  return kFALSE;
	}
    }
  
  fin.close();
  
  
  return kTRUE;
}
