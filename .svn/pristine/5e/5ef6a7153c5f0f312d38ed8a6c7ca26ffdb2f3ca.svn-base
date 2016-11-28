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
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJCalibTest
//
// If the flag SetDataCheckDisplay() is set, only the most important distributions
//  are displayed. 
// Otherwise, (default: SetNormalDisplay()), a good selection of plots is given
//
/////////////////////////////////////////////////////////////////////////////
#include "MJCalibTest.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MTaskEnv.h"
#include "MEvtLoop.h"

#include "MHCamera.h"

#include "MSignalCam.h"
#include "MPedestalCam.h"
#include "MPedPhotCam.h"
#include "MBadPixelsCam.h"
#include "MBadPixelsTreat.h"
#include "MBadPixelsCalc.h"
#include "MBadPixelsMerge.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationRelTimeCam.h"
#include "MCalibrationQECam.h"
#include "MCalibrationTestCam.h"
#include "MCalibrationTestCalc.h"
#include "MHCamEvent.h"
#include "MHCalibrationTestCam.h"
#include "MHCalibrationTestTimeCam.h"
#include "MHCalibrationPix.h"

#include "MReadMarsFile.h"
#include "MRawFileRead.h"
#include "MGeomApply.h"
#include "MGeomCam.h"
#include "MExtractTimeAndChargeSpline.h"
#include "MExtractor.h"
#include "MExtractTime.h"
//#include "MExtractTimeFastSpline.h"
#include "MFCosmics.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MCalibrateData.h"
#include "MCalibrateRelTimes.h"

#include "MTriggerPattern.h"
#include "MTriggerPatternDecode.h"
#include "MFTriggerPattern.h"

#include "MStatusDisplay.h"

ClassImp(MJCalibTest);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets fUseCosmicsFilter to kTRUE, fExtractor to NULL, fTimeExtractor to NULL
// fDisplay to kNormalDisplay
//
MJCalibTest::MJCalibTest(const char *name, const char *title) 
    : fUseCosmicsFilter(kTRUE), fExtractor(NULL), fTimeExtractor(NULL),
      fDisplayType(kNormalDisplay), fGeometry("MGeomCamMagic")
{
    fName  = name  ? name  : "MJCalibTest";
    fTitle = title ? title : "Tool to extract, calibrate and test signals from a file";
}


void MJCalibTest::DisplayResult(MParList &plist)
{
  if (!fDisplay)
    return;
  
  //
  // Get container from list
  //
  MGeomCam  &geomcam = *(MGeomCam*) plist.FindObject("MGeomCam");
  MHCalibrationTestCam &testcam = *(MHCalibrationTestCam*)plist.FindObject("MHCalibrationTestCam");
  
  // Create histograms to display
  MHCamera disp1 (geomcam, "Test;PhotoElectrons",    "Mean equiv. phes");
  MHCamera disp2 (geomcam, "Test;SigmaPhes",         "Sigma equiv.phes");
  MHCamera disp3 (geomcam, "Test;PhesPerArea",       "Equiv. Phes per Area");
  MHCamera disp4 (geomcam, "Test;SigmaPhotPerArea",  "Sigma equiv. Phes per Area");
  MHCamera disp5 (geomcam, "Test;Phot",              "Calibrated Phes from Fit");
  MHCamera disp6 (geomcam, "Test;PhotPerArea",       "Calibrated Phes per Area from Fit");
  MHCamera disp7 (geomcam, "Test;NotInterpolate",    "Not interpolated pixels");
  MHCamera disp8 (geomcam, "Test;DeviatingPhots",    "Deviating Number Phes");
  MHCamera disp9 (geomcam, "Test;Arr.Times",         "Mean of calibrated Arr.Times");
  MHCamera disp10(geomcam, "Test;SigmaArr.Times",    "Sigma of calibrated Arr.Times");

  // Fitted charge means and sigmas
  disp1.SetCamContent(testcam,  0);
  disp1.SetCamError(  testcam,  1);
  disp2.SetCamContent(testcam,  2);
  disp2.SetCamError(  testcam,  3);
  disp3.SetCamContent(testcam,  7);
  disp3.SetCamError(  testcam,  8);
  disp4.SetCamContent(testcam,  9);
  disp4.SetCamError(  testcam,  10);

  disp5.SetCamContent(fTestCam,  0);
  disp5.SetCamError(  fTestCam,  1);
  disp6.SetCamContent(fTestCam,  2);
  disp6.SetCamError(  fTestCam,  3);
  disp7.SetCamError(  fTestCam,  4);

  disp8.SetCamError(  fBadPixels, 22);

  disp9.SetCamContent(fTestTimeCam,  0);
  disp9.SetCamError(  fTestTimeCam,  1);
  disp10.SetCamContent(fTestTimeCam,  2);
  disp10.SetCamError(  fTestTimeCam,  3);


  disp1.SetYTitle("Phes");
  disp2.SetYTitle("\\sigma_{phe}");
  disp3.SetYTitle("Phes per area [mm^{-2}]");
  disp4.SetYTitle("\\sigma_{phe} per area [mm^{-2}]");

  disp5.SetYTitle("Phes");
  disp6.SetYTitle("Phes per area [mm^{-2}]");
  disp7.SetYTitle("[1]");
  disp8.SetYTitle("[1]");
  
  disp9.SetYTitle("Mean Arr.Times [FADC units]");
  disp10.SetYTitle("\\sigma_{t} [FADC units]");
  
  TCanvas &c = fDisplay->AddTab("TestPhes");
  c.Divide(4,4);
  
  disp1.CamDraw(c, 1, 4, 2, 1);
  disp2.CamDraw(c, 2, 4, 2, 1);        
  disp3.CamDraw(c, 3, 4, 1, 1);        
  disp4.CamDraw(c, 4, 4, 2, 1);        
  
  /*

  TCanvas &c2 = fDisplay->AddTab("TestResult");
  c2.Divide(2,4);

  disp5.CamDraw(c2, 1, 2, 2, 1);
  disp6.CamDraw(c2, 2, 2, 2, 1);        

  */

  TCanvas &c3 = fDisplay->AddTab("TestDefects");
  c3.Divide(2,2);

  disp7.CamDraw(c3, 1, 2, 0);
  disp8.CamDraw(c3, 2, 2, 0);        

  //
  // Display times
  // 
  TCanvas &c4 = fDisplay->AddTab("TestTimes");
  c4.Divide(2,4);

  disp9.CamDraw(c4, 1, 2,  5, 1);
  disp10.CamDraw(c4, 2, 2,  5, 1);        

  return;

}

// --------------------------------------------------------------------------
//
// Retrieve the output file written by WriteResult()
// 
const char* MJCalibTest::GetOutputFile() const
{
    return Form("%s/calib%08d.root", fPathOut.Data(), fSequence.GetSequence());
}

Bool_t MJCalibTest::ReadCalibration(TObjArray &l, MBadPixelsCam &cam, MExtractor* &ext1, MExtractor* &ext2, TString &geom) const
{

  const TString fname = GetOutputFile();
  
  *fLog << inf << "Reading from file: " << fname << endl;

  TFile file(fname, "READ");
  if (!file.IsOpen())
    {
      *fLog << err << dbginf << "ERROR - Could not open file " << fname << endl;
      return kFALSE;
    }
  
  TObject *o = file.Get("ExtractSignal");
  if (o && !o->InheritsFrom(MExtractor::Class()))
    {
      *fLog << err << dbginf << "ERROR - ExtractSignal read from " << fname << " doesn't inherit from MExtractor!" << endl;
      return kFALSE;
    }
  ext1 = o ? (MExtractor*)o->Clone() : NULL;
  
  o = file.Get("ExtractTime");
  if (o && !o->InheritsFrom(MExtractor::Class()))
    {
      *fLog << err << dbginf << "ERROR - ExtractTime read from " << fname << " doesn't inherit from MExtractor!" << endl;
      return kFALSE;
    }
  ext2 = o ? (MExtractor*)o->Clone() : NULL;
  if (!ext1 && !ext2)
    {
      *fLog << err << dbginf << "ERROR - Neither ExtractSignal nor ExrtractTime found in " << fname << "!" << endl;
      return kFALSE;
    }
  
  o = file.Get("MGeomCam");
  if (o && !o->InheritsFrom(MGeomCam::Class()))
    {
      *fLog << err << dbginf << "ERROR - MGeomCam read from " << fname << " doesn't inherit from MGeomCam!" << endl;
      return kFALSE;
    }
  geom = o ? o->ClassName() : "";
  
  TObjArray cont(l);
  cont.Add(&cam);
  return ReadContainer(cont);
}

Bool_t MJCalibTest::Process(MPedestalCam &pedcam)
{
    if (!fSequence.IsValid())
    {
	  *fLog << err << "ERROR - Sequence invalid..." << endl;
	  return kFALSE;
    }

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate calibration test from Sequence #";
    *fLog << fSequence.GetSequence() << endl << endl;

  CheckEnv();
  
  *fLog << inf;
  fLog->Separator(GetDescriptor());
  
  *fLog << "Calibrate Calibration data from ";
  *fLog << "Sequence #" << fSequence.GetSequence() << endl;
  *fLog << endl;
  
  MDirIter iter;

  if (fSequence.IsValid())
    {
        if (fSequence.SetupCalRuns(iter, 0, !fSequence.IsMonteCarlo())<=0)
            return kFALSE;
    }
  
  MCalibrationChargeCam      calcam;
  MCalibrationQECam          qecam;
  MCalibrationRelTimeCam     tmcam;
  MBadPixelsCam              badpix;
  
  TObjArray calibcont;
  calibcont.Add(&calcam);
  calibcont.Add(&qecam);
  calibcont.Add(&tmcam);
    
  MExtractor *extractor1=0;
  MExtractor *extractor2=0;
  TString geom;

  if (!ReadCalibration(calibcont, badpix, extractor1, extractor2, geom))
    {
      *fLog << err << "Could not read calibration constants " << endl;
      return kFALSE;
    }
  
  *fLog << all;
  if (extractor1)
    {
      *fLog << underline << "Signal Extractor found in calibration file" << endl;
      extractor1->Print();
      *fLog << endl;
    }
  else
    *fLog << inf << "No Signal Extractor: ExtractSignal in file." << endl;
  
  if (extractor2)
    {
      *fLog << underline << "Time Extractor found in calibration file" << endl;
      extractor2->Print();
      *fLog << endl;
    }
  else
    *fLog << inf << "No Time Extractor: ExtractTime in file." << endl;
  
  if (!geom.IsNull())
    *fLog << inf << "Camera geometry found in file: " << geom << endl;
  else
    *fLog << inf << "No Camera geometry found using default <MGeomCamMagic>" << endl;

  if (fExtractor)
    extractor1 = fExtractor;
  if (fTimeExtractor)
    extractor2 = fTimeExtractor;

  // Setup Lists
  MParList plist;
  plist.AddToList(this); // take care of fDisplay!
  plist.AddToList(&fTestCam);
  plist.AddToList(&fTestTimeCam);
  plist.AddToList(&badpix);
  plist.AddToList(&pedcam);
  plist.AddToList(&calcam);
  plist.AddToList(&qecam);
  plist.AddToList(&tmcam);

  MSignalCam           cerphot;
  MPedPhotCam          pedphot;
  MHCalibrationTestCam testcam;

  plist.AddToList(&cerphot);
  plist.AddToList(&pedphot);
  plist.AddToList(&testcam);

  pedcam.SetName("MPedestalFundamental");
  
  MTaskList tlist;
  plist.AddToList(&tlist);

  // Setup Task-lists
  MRawFileRead rawread(NULL);
  MReadMarsFile read("Events");
  read.DisableAutoScheme();

  if (!fSequence.IsMonteCarlo())
    rawread.AddFiles(iter);
  else
    static_cast<MRead&>(read).AddFiles(iter);

  // Check for interleaved events
  // This will make that for data with version less than 5, where trigger
  // patterns were not yet correct, all the events in the file will be
  // processed. For those data there are no interleaved calibration events,
  // so it makes no sense to run this test on a _D_ file. So we assume it
  // will be a _C_ file, and accept all events.
  MTriggerPatternDecode decode;
  MFTriggerPattern      fcalib;
  fcalib.DenyCalibration();
  fcalib.SetDefault(kFALSE);
  MContinue conttp(&fcalib, "ContTrigPattern");

  MGeomApply            apply; // Only necessary to craete geometry
  if (!geom.IsNull())
    apply.SetGeometry(geom);  
  MBadPixelsMerge       merge(&badpix);

//  MExtractTimeAndChargeSlidingWindow extrsw;
//  MExtractTimeFastSpline             extime;

  MExtractTimeAndChargeSpline spline;

  MTaskEnv taskenv1("ExtractSignal");
  MTaskEnv taskenv2("ExtractTime");

  if (extractor1)
      taskenv1.SetDefault(extractor1);

  if (extractor2)
      taskenv2.SetDefault(fTimeExtractor);
  else if (!(extractor1->InheritsFrom("MExtractTimeAndCharge")))
    {
      spline.SetWindowSize(8,8);
      taskenv2.SetDefault(&spline);
      *fLog << warn << GetDescriptor() 
            << ": No extractor has been chosen, take default MExtractTimeAndChargeSlidingWindow " << endl;
    }


  MCalibrateData        photcalc;
  photcalc.AddPedestal("Fundamental");
  photcalc.SetCalibrationMode(MCalibrateData::kFfactor);
  photcalc.SetPedestalFlag(MCalibrateData::kEvent);
  photcalc.SetSignalType(MCalibrateData::kPhe);

  MCalibrateRelTimes    caltimes;
  MBadPixelsCalc        badcalc;
  MBadPixelsTreat       badtreat;
  badtreat.SetProcessTimes(kFALSE);

  badcalc.SetNamePedPhotCam("MPedPhotFundamental");
  badtreat.SetUseInterpolation();
  badtreat.AddNamePedPhotCam("MPedPhotFundamental");

  MCalibrationTestCalc  testcalc;

  MHCamEvent evt0(0,"Signal", "Un-Calibrated Signal;;S [FADC cnts]" );
  MHCamEvent evt1(0,"CalSig", "Cal. and Interp. Sig. by Pixel Size Ratio;;S [phe]");
  MHCamEvent evt2(6,"Times" , "Arrival Time;;T [slice]");

  MFillH fill0(&evt0, "MExtractedSignalCam", "FillUncalibrated");
  MFillH fill1(&evt1, "MSignalCam", "FillCalibrated");
  MFillH fill2(&evt2, "MSignalCam", "FillTimes");
  
  MFillH fillcam("MHCalibrationTestCam",     "MSignalCam" ,"FillTest");
  MFillH filltme("MHCalibrationTestTimeCam", "MSignalCam", "FillTestTime");
  fillcam.SetBit(MFillH::kDoNotDisplay);
  filltme.SetBit(MFillH::kDoNotDisplay);

  MFCosmics cosmics;
  cosmics.SetNamePedestalCam("MPedestalFundamental");
  MContinue contcos(&cosmics,"ContCosmics");
  
  tlist.AddToList(&read);
  tlist.AddToList(&decode);
  tlist.AddToList(&apply);
  tlist.AddToList(&merge);
  //  tlist.AddToList(&conttp);
  tlist.AddToList(&taskenv1);
  if (!extractor1->InheritsFrom("MExtractTimeAndCharge"))
    tlist.AddToList(&taskenv2);

  tlist.AddToList(&contcos);
  tlist.AddToList(&fill0);
  tlist.AddToList(&photcalc);
  tlist.AddToList(&caltimes);
  tlist.AddToList(&badcalc);
  tlist.AddToList(&badtreat);
  tlist.AddToList(&fill1);
  tlist.AddToList(&fill2);
  tlist.AddToList(&fillcam);
  tlist.AddToList(&filltme);
  tlist.AddToList(&testcalc);
  
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
  
  if (fIsPixelCheck)
    {
      MHCalibrationTestCam *hcam = (MHCalibrationTestCam*)plist.FindObject("MHCalibrationTestCam");
      MHCalibrationPix &pix1 = (*hcam)[fCheckedPixId];
      pix1.DrawClone("");
      
      MHCalibrationTestTimeCam *hccam = (MHCalibrationTestTimeCam*)plist.FindObject("MHCalibrationTestTimeCam");
      MHCalibrationPix &pix11 = (*hccam)[fCheckedPixId];
      pix11.DrawClone("");
    }

  DisplayResult(plist);

  if (!WriteResult())
    return kFALSE;

  *fLog << inf << GetDescriptor() << ": Done." << endl;
  
  return kTRUE;
}

Bool_t MJCalibTest::WriteResult()
{
    
    if (fPathOut.IsNull())
        return kTRUE;
    
    const TString oname(GetOutputFile());

    *fLog << inf << "Writing to file: " << oname << endl;
    
    TFile file(oname, "UPDATE");
    
    if (fDisplay && fDisplay->Write()<=0)
    {
        *fLog << err << "Unable to write MStatusDisplay to " << oname << endl;
        return kFALSE;
    }

    if (fTestCam.Write()<=0)
    {
        *fLog << err << "Unable to write MCalibrationTestCam to " << oname << endl;
        return kFALSE;
    }

    if (fTestTimeCam.Write()<=0)
    {
        *fLog << err << "Unable to write MCalibrationTestCam to " << oname << endl;
        return kFALSE;
    }

    return kTRUE;

}
