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
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJExtractSignal
//
/////////////////////////////////////////////////////////////////////////////
#include "MJExtractSignal.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"
#include "MPrint.h"

#include "MHCamera.h"

#include "MPedestalCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationQECam.h"
#include "MHCamEvent.h"

#include "MReadMarsFile.h"
#include "MGeomApply.h"
#include "MBadPixelsMerge.h"
#include "MExtractSignal.h"
#include "MFillH.h"
#include "MCalibrateData.h"
#include "MPedPhotCalc.h"
#include "MWriteRootFile.h"

#include "MExtractSlidingWindow.h"
#include "MExtractor.h"
#include "MExtractTime.h"
#include "MExtractTimeFastSpline.h"

#include "MJExtractSignal.h"
#include "MStatusDisplay.h"


ClassImp(MJExtractSignal);

using namespace std;

MJExtractSignal::MJExtractSignal(const char *name, const char *title)
    : fRuns(0), fExtractor(NULL), fTimeExtractor(NULL)
{
    fName  = name  ? name  : "MJExtractSignal";
    fTitle = title ? title : "Tool to create a pedestal file (MPedestalCam)";
}

Bool_t MJExtractSignal::WriteResult()
{
    if (fOutputPath.IsNull())
        return kTRUE;

    const TString oname = GetOutputFileP();

    *fLog << inf << "Writing to file: " << oname << endl;

    TFile file(oname, "RECREATE");

    if (fDisplay && fDisplay->Write()<=0)
    {
        *fLog << err << "Unable to write MStatusDisplay to " << oname << endl;
        return kFALSE;
    }

    if (fPedPhotCam.Write()<=0)
    {
        *fLog << err << "Unable to write MPedPhotCam to " << oname << endl;
        return kFALSE;
    }

    if (fBadPixels.Write()<=0)
    {
        *fLog << err << "Unable to write MBadPixelsCam to " << oname << endl;
        return kFALSE;
    }

    return kTRUE;

}

void MJExtractSignal::SetOutputPath(const char *path)
{
    fOutputPath = path;
    if (fOutputPath.EndsWith("/"))
        fOutputPath = fOutputPath(0, fOutputPath.Length()-1);
}

TString MJExtractSignal::GetOutputFileD() const
{
    if (!fRuns)
        return "";

    return Form("%s/%s-F3.root", (const char*)fOutputPath, (const char*)fRuns->GetRunsAsFileName());
}
TString MJExtractSignal::GetOutputFileP() const
{
    if (!fRuns)
        return "";

    return Form("%s/%s-F2.root", (const char*)fOutputPath, (const char*)fRuns->GetRunsAsFileName());
}

Bool_t MJExtractSignal::ProcessD(MPedestalCam &pedcam)
{
    const TString fname = GetOutputFileD();

    if (gSystem->AccessPathName(fname, kFileExists))
        return ProcessFileD(pedcam);

    return kTRUE;
}

Bool_t MJExtractSignal::ProcessFileD(MPedestalCam &pedcam)
{

  if (!fRuns)
    {
      *fLog << err << "No Runs choosen... abort." << endl;
      return kFALSE;
    }
  if (fRuns->GetNumRuns() != fRuns->GetNumEntries())
    {
        *fLog << err << "Number of files found doesn't match number of runs... abort." << endl;
        return kFALSE;
    }
  
  *fLog << inf;
  fLog->Separator(GetDescriptor());
  *fLog << "Calculate MExtractedSignalCam from Runs " << fRuns->GetRunsAsString() << endl;
  *fLog << endl;
  
  // Setup Lists
  MParList plist;
  plist.AddToList(&pedcam);
  
  MTaskList tlist;
  plist.AddToList(&tlist);

  // Setup Parameters
  
  // Make sure, that at least an empty MBadPixelsCam is available
  // This is necessary for input which don't contain a MBadPixelsCam
  MBadPixelsCam badcam;
  plist.AddToList(&badcam);
  
  // Setup Task-lists
  MReadMarsFile read("Events");
  read.DisableAutoScheme();
  static_cast<MRead&>(read).AddFiles(*fRuns);
  
  MGeomApply               apply; // Only necessary to craete geometry
  MBadPixelsMerge          merge(&fBadPixels);
  MExtractTimeFastSpline   exttime;
  MExtractSlidingWindow    extcharge; // Only for the cosmics filter
  
  MHCamEvent evt("ExtSignal");
  evt.SetType(0);
  MFillH fill(&evt, "MExtractedSignalCam");
  
  MWriteRootFile write(GetOutputFileD(), "RECREATE", fRuns->GetRunsAsString(), 2);
  write.AddContainer("MExtractedSignalCam", "Events");
  write.AddContainer("MArrivalTimeCam",     "Events");
  write.AddContainer("MTime",               "Events");
  write.AddContainer("MRawEvtHeader",       "Events");
  write.AddContainer("MPedestalCam",        "RunHeaders");
  write.AddContainer("MRawRunHeader",       "RunHeaders");
  write.AddContainer("MBadPixelsCam",       "RunHeaders");
  
  tlist.AddToList(&read);
  tlist.AddToList(&apply);
  tlist.AddToList(&merge);
  
  if (fTimeExtractor)
    tlist.AddToList(fTimeExtractor);
  else
  {
      *fLog << warn << GetDescriptor() 
            << ": No extractor has been chosen, take default MExtractTimeFastSpline " << endl;
      tlist.AddToList(&exttime);
  }

  if (fExtractor)
    tlist.AddToList(fExtractor);
  else
  {
      *fLog << warn << GetDescriptor() 
            << ": No extractor has been chosen, take default MExtractSlidingWindow " << endl;
      tlist.AddToList(&extcharge);
  }

//  MPrint print("MExtractedSignalCam");
//  tlist.AddToList(&print);
  
  if (TestBit(kEnableGraphicalOutput))
    tlist.AddToList(&fill);
  tlist.AddToList(&write);
  
  // Create and setup the eventloop
  MEvtLoop evtloop(fName);
  evtloop.SetParList(&plist);
  evtloop.SetDisplay(fDisplay);
  evtloop.SetLogStream(fLog);
  
  // Execute first analysis
  if (!evtloop.Eventloop())
    {
      *fLog << err << GetDescriptor() << ": Failed." << endl;
      return kFALSE;
    }
  
  tlist.PrintStatistics();
  
  //DisplayResult(plist);
  
  //if (!WriteResult())
  //    return kFALSE;
  
  *fLog << inf << GetDescriptor() << ": Done." << endl;
  
  return kTRUE;
}

Bool_t MJExtractSignal::ReadPedPhotCam()
{
    const TString fname = GetOutputFileP();

    if (gSystem->AccessPathName(fname, kFileExists))
    {
        *fLog << err << "Input file " << fname << " doesn't exist." << endl;
        return kFALSE;
    }

    *fLog << inf << "Reading from file: " << fname << endl;

    TFile file(fname, "READ");
    if (fPedPhotCam.Read()<=0)
    {
        *fLog << "Unable to read MPedPhotCam from " << fname << endl;
        return kFALSE;
    }

    if (file.FindKey("MBadPixelsCam"))
    {
        MBadPixelsCam bad;
        if (bad.Read()<=0)
        {
            *fLog << "Unable to read MBadPixelsCam from " << fname << endl;
            return kFALSE;
        }
        fBadPixels.Merge(bad);
    }

    if (fDisplay /*&& !fDisplay->GetCanvas("Pedestals")*/) // FIXME!
        fDisplay->Read();

    return kTRUE;
}

Bool_t MJExtractSignal::ProcessP(MPedestalCam &pedcam, MCalibrationChargeCam &calcam, MCalibrationQECam &qecam)
{
    if (!ReadPedPhotCam())
        return ProcessFileP(pedcam, calcam, qecam);

    return kTRUE;
}

Bool_t MJExtractSignal::ProcessFileP(MPedestalCam &pedcam, MCalibrationChargeCam &calcam, MCalibrationQECam &qecam)
{
    if (!fRuns)
    {
        *fLog << err << "No Runs choosen... abort." << endl;
        return kFALSE;
    }
    if (fRuns->GetNumRuns() != fRuns->GetNumEntries())
    {
        *fLog << err << "Number of files found doesn't match number of runs... abort." << endl;
        return kFALSE;
    }

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate MExtractedSignalCam from Runs " << fRuns->GetRunsAsString() << endl;
    *fLog << endl;

    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    static_cast<MRead&>(read).AddFiles(*fRuns);

    // Setup Tasklist
    MParList plist;
    plist.AddToList(&pedcam);
    plist.AddToList(&calcam);
    plist.AddToList(&qecam);
    plist.AddToList(&fPedPhotCam);
    plist.AddToList(&fBadPixels);

    MTaskList tlist;
    plist.AddToList(&tlist);

    MGeomApply      apply; // Only necessary to craete geometry
    MBadPixelsMerge merge(&fBadPixels);
    MExtractSignal  extract;
    MCalibrateData  calib;
    MPedPhotCalc    calc;

    MHCamEvent evt1("ExtOffset");
    MHCamEvent evt2("CalOffset");
    evt1.SetType(0);
    evt2.SetType(0);
    MFillH fill1(&evt1, "MExtractedSignalCam", "FillExtractedSignal");
    MFillH fill2(&evt2, "MCerPhotEvt",         "FillCerPhotEvt");

    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&merge);
    tlist.AddToList(&extract);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill1);
    tlist.AddToList(&calib);
    tlist.AddToList(&calc);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill2);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);

    // Execute first analysis
    if (!evtloop.Eventloop())
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    tlist.PrintStatistics();

    //DisplayResult(plist);

    if (!WriteResult())
        return kFALSE;

    *fLog << inf << GetDescriptor() << ": Done." << endl;

    return kTRUE;
}

/*
Bool_t MJExtractSignal::ProcessFile(MPedestalCam *pedcam, MCalibrationChargeCam *calcam)
{
    if (!fRuns)
    {
        *fLog << err << "No Runs choosen... abort." << endl;
        return kFALSE;
    }
    if (fRuns->GetNumRuns() != fRuns->GetNumEntries())
    {
        *fLog << err << "Number of files found doesn't match number of runs... abort." << endl;
        return kFALSE;
    }

    Int_t type = 0;
    if (pedcam &&  calcam) type = 2;
    if (pedcam && !calcam) type = 3;

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculating from Runs " << fRuns->GetRunsAsString() << endl;
    *fLog << endl;

    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    static_cast<MRead&>(read).AddFiles(*fRuns);

    // Setup Tasklist
    MParList plist;
    switch (type)
    {
    case 2:
        plist.AddToList(calcam);
        plist.AddToList(&fPedPhotCam);
    case 3:
        plist.AddToList(pedcam);
    }

    MTaskList tlist;
    plist.AddToList(&tlist);

    MGeomApply     apply; // Only necessary to craete geometry
    MExtractSignal extract;
    MCalibrateData calib;
    MPedPhotCalc   calc;


    MHCamEvent evt1("ExtOffset");
    MHCamEvent evt2("CalOffset");
    evt1.SetType(0);
    evt2.SetType(0);
    MFillH fill1(&evt1, "MExtractedSignalCam", "FillExtractedSignal");
    MFillH fill2(&evt2, "MCerPhotEvt",         "FillCerPhotEvt");

    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&extract);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill1);
    tlist.AddToList(&calib);
    tlist.AddToList(&calc);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill2);


    MHCamEvent evt("ExtSignal");
    evt.SetType(0);
    MFillH fill(&evt, "MExtractedSignalCam");

    MWriteRootFile write(GetOutputFileD(), "RECREATE", fRuns->GetRunsAsString(), 2);
    write.AddContainer("MExtractedSignalCam", "Events");
    write.AddContainer("MTime",               "Events");
    write.AddContainer("MRawRunHeader",       "RunHeaders");
    write.AddContainer("MPedestalCam",        "RunHeaders");

    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&extract);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill);
    tlist.AddToList(&write);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);

    // Execute first analysis
    if (!evtloop.Eventloop())
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    tlist.PrintStatistics();

    //DisplayResult(plist);

    //if (!WriteResult())
    //    return kFALSE;

    *fLog << inf << GetDescriptor() << ": Done." << endl;

    return kTRUE;


    // ------------------------------------------------------

    MGeomApply     apply; // Only necessary to craete geometry
    MExtractSignal extract;
    MCalibrateData  calib;
    MPedPhotCalc   calc;

    MHCamEvent evt1("ExtOffset");
    MHCamEvent evt2("CalOffset");
    evt1.SetType(0);
    evt2.SetType(0);
    MFillH fill1(&evt1, "MExtractedSignalCam", "FillExtractedSignal");
    MFillH fill2(&evt2, "MCerPhotEvt",         "FillCerPhotEvt");

    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&extract);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill1);
    tlist.AddToList(&calib);
    tlist.AddToList(&calc);
    if (TestBit(kEnableGraphicalOutput))
        tlist.AddToList(&fill2);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);

    // Execute first analysis
    if (!evtloop.Eventloop())
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    tlist.PrintStatistics();

    //DisplayResult(plist);

    if (!WriteResult())
        return kFALSE;

    *fLog << inf << GetDescriptor() << ": Done." << endl;

    return kTRUE;
    }
    */
