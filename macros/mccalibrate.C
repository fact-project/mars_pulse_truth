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
   !   Author(s): Abelardo Moralejo 1/2004 <mailto:moralejo@pd.infn.it>
   !              Thomas Bretz  5/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
   !
   !   Copyright: MAGIC Software Development, 2000-2004
   !
   !
   \* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMCALIBRATE - Calibration of MC data
//
//  This macro converts raw MC data into calibrated data (photons per pixel) 
//  It optionally divides the output into a train and a test sample
//
/////////////////////////////////////////////////////////////////////////////

#include "MImgCleanStd.h"

void mccalibrate()
{
  //
  // This macro reads in MC camera files and produces and output with 
  // calibrated events (signal in photons for all pixels, in MCerPhotEvt 
  // containers).
  //

  // ------------- user change -----------------
  TString* CalibrationFilename;

  CalibrationFilename = new TString("/users/emc/moralejo/mcdata/Period021_0.73_mirror/gammas_nonoise/Gamma_*root");  // File to be used for the calibration (must be a camera file without added noise)

  Char_t* AnalysisFilename = "Gamma_zbin*w0.root";  // File to be analyzed


  TString* OutFilename1;
  TString* OutFilename2;

  // Output file names
  OutFilename1 = new TString("calibrated_gamma_train.root");
  OutFilename2 = new TString("calibrated_gamma_test.root"); 

  // To get only one output file, just comment out the second 
  // one of the above lines


  //
  //  USER CHANGE: Set signal extractor
  //
  //    MExtractFixedWindowPeakSearch sigextract;
  //    sigextract.SetWindows(6, 6, 4);
  //

  //    MExtractTimeAndChargeDigitalFilter sigextract;
  //    sigextract.SetNameWeightsFile("/users/emc/moralejo/Mars/msignal/MC_weights.dat");
  //    sigextract.SetRange(1, 14, 3, 14);

  MExtractTimeAndChargeSpline sigextract;
  sigextract.SetRiseTimeHiGain(0.5);
  sigextract.SetFallTimeHiGain(0.5);


  // USER CHANGE: high to low gain ratio. DEPENDS on EXTRACTOR!!
  // One should calculate somewhere else what this factor is for each extractor!
  Float_t hi2low = 12.; // tentative value for spline with risetime 0.5, fall time 0.5

  MMcCalibrationUpdate  mccalibupdate;
  mccalibupdate.SetUserLow2HiGainFactor(hi2low);
  ///// User change: calibrate in photons or phe- :
  mccalibupdate.SetSignalType(MCalibrateData::kPhe);
  //  mccalibupdate.SetSignalType(MCalibrateData::kPhot);

  // ---------------------------------------------------------------------
  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;

  MTaskList tlist;

  plist.AddToList(&tlist);

  MBadPixelsCam badpix;
  plist.AddToList(&badpix);  // Not used for now.
 
  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //
  MReadMarsFile read("Events");

  if (CalibrationFilename)
    read.AddFile(CalibrationFilename->Data());

  read.DisableAutoScheme();

  MGeomApply geom; 
  // Reads in geometry from MC file and sets the right sizes for
  // several parameter containers.

  MMcPedestalCopy   pcopy; 
  // Copies pedestal data from the MC file run fadc header to the 
  // MPedestalCam container.

  MPointingPosCalc pointcalc;
  // Creates MPointingPos object and fills it with the telescope 
  // orientation information taken from MMcEvt.

  MCalibrateData calib; 
  //
  // MCalibrateData transforms signals from ADC counts into photons or phe-
  // (this can be selected anove). In the first loop it applies a "dummy" 
  // calibration supplied by MMcCalibrationUpdate, just to equalize inner 
  // and outer pixels, and calculates SIZE in "equivalent number of inner 
  // ADC counts". At the end of the first loop, in the PostProcess of 
  // MMcCalibrationCalc (see below) the true calibration constants
  // are calculated.
  //
  calib.SetCalibrationMode(MCalibrateData::kFfactor);
  // Do not change the CalibrationMode above for MC...!

  // Now set also whether to calibrate in photons or phe-:
  calib.SetSignalType(mccalibupdate.GetSignalType());

  MImgCleanStd clean;
  //
  // Applies tail cuts to image. Since the calibration is performed on 
  // noiseless camera files, the precise values of the cleaning levels 
  // are unimportant (in any case, only pixels without any C-photon will
  // be rejected).
  //

  MHillasCalc hcalc; 
  hcalc.Disable(MHillasCalc::kCalcHillasSrc);
  // Calculates Hillas parameters not dependent on source position.

  MMcCalibrationCalc mccalibcalc; 
  // Calculates calibration constants to convert from ADC counts to photons.

  tlist.AddToList(&read);
  tlist.AddToList(&geom);

  MF notrigger("MMcTrig.fNumFirstLevel<1");
  MContinue skipnotrig(&notrigger);
  tlist.AddToList(&skipnotrig);

  tlist.AddToList(&pcopy);
  tlist.AddToList(&pointcalc);
  tlist.AddToList(&sigextract);
  tlist.AddToList(&mccalibupdate);
  tlist.AddToList(&calib);
  tlist.AddToList(&clean);
  tlist.AddToList(&hcalc);

  tlist.AddToList(&mccalibcalc);

  //
  // Open output files:
  //
  MWriteRootFile write1(OutFilename1->Data()); // Writes output

  MWriteRootFile write1_b(OutFilename1->Data());
  write1_b.AddContainer("MMcEvtBasic", "OriginalMC", kFALSE);
  // Add the MMcEvtBasic container only in case it exists in 
  // the input camera files

  write1.AddContainer("MGeomCam",            "RunHeaders");
  write1.AddContainer("MMcConfigRunHeader",  "RunHeaders");
  write1.AddContainer("MMcCorsikaRunHeader", "RunHeaders");
  write1.AddContainer("MMcFadcHeader",       "RunHeaders");
  write1.AddContainer("MMcRunHeader",        "RunHeaders");
  write1.AddContainer("MMcTrigHeader",       "RunHeaders");
  write1.AddContainer("MRawRunHeader",       "RunHeaders");

  write1.AddContainer("MMcEvt",        "Events");
  write1.AddContainer("MMcTrig",       "Events");
  write1.AddContainer("MPointingPos",  "Events");
  write1.AddContainer("MRawEvtHeader", "Events");
  write1.AddContainer("MCerPhotEvt",   "Events");
  write1.AddContainer("MPedPhotCam",   "Events");

  if (OutFilename2)
    {
      MWriteRootFile write2(OutFilename2->Data()); // Writes output    

      MWriteRootFile write2_b(OutFilename2->Data());
      write2_b.AddContainer("MMcEvtBasic", "OriginalMC", kFALSE);
      // Add the MMcEvtBasic container only in case it exists in 
      // the input camera files

      write2.AddContainer("MGeomCam",            "RunHeaders");
      write2.AddContainer("MMcConfigRunHeader",  "RunHeaders");
      write2.AddContainer("MMcCorsikaRunHeader", "RunHeaders");
      write2.AddContainer("MMcFadcHeader",       "RunHeaders");
      write2.AddContainer("MMcRunHeader",        "RunHeaders");
      write2.AddContainer("MMcTrigHeader",       "RunHeaders");
      write2.AddContainer("MRawRunHeader",       "RunHeaders");

      write2.AddContainer("MMcEvt",        "Events");
      write2.AddContainer("MMcTrig",       "Events");
      write2.AddContainer("MPointingPos",  "Events");
      write2.AddContainer("MRawEvtHeader", "Events");
      write2.AddContainer("MCerPhotEvt",   "Events");
      write2.AddContainer("MPedPhotCam",   "Events");

      //
      // Divide output in train and test samples, using the event number
      // (odd/even) to achieve otherwise unbiased event samples:
      //
      MF filter1("{MMcEvt.fEvtNumber%2}>0.5");
      MF filter2("{MMcEvt.fEvtNumber%2}<0.5");

      write1.SetFilter(&filter1);
      write2.SetFilter(&filter2);

      write1_b.SetFilter(&filter1);
      write2_b.SetFilter(&filter2);
    }

  //
  // First loop: Calibration loop
  //

  MProgressBar bar;
  bar.SetWindowName("Calibrating...");

  MEvtLoop evtloop;
  evtloop.SetProgressBar(&bar);
  evtloop.SetParList(&plist);

  if (CalibrationFilename)
    {
      if (!evtloop.Eventloop())
	return;
      mccalibcalc->GetHistADC2PhotEl()->Write();
      mccalibcalc->GetHistPhot2PhotEl()->Write();
      // Writes out the histograms used for calibration. In case of
      // aslit output in train and test file, this is written only
      // in the test file for now.
    }

  //
  // Second loop: apply calibration factors to MC events in the 
  // file to be anlyzed:
  //

  //
  // Change the read task by another one which reads the file we want to analyze:
  //

  MReadMarsFile read2("Events");
  read2.AddFile(AnalysisFilename);
  read2.DisableAutoScheme();
  tlist.AddToListBefore(&read2, &read, "All");
  tlist.RemoveFromList(&read);

  // Now add tasks to write MMcEvtBasic to the OriginalMC tree:
  tlist.AddToListBefore(&write1_b, &skipnotrig, "All");
  if (OutFilename2)
    tlist.AddToListBefore(&write2_b, &skipnotrig, "All");


  if (OutFilename2) // Add filters to split output in train and test
    {
      tlist.AddToListBefore(&filter1, &write1_b, "All");
      tlist.AddToListBefore(&filter2, &write1_b, "All");
    }

  bar.SetWindowName("Writing...");

  tlist.RemoveFromList(&clean);
  tlist.RemoveFromList(&hcalc);
  tlist.RemoveFromList(&mccalibcalc);

  tlist.AddToList(&write1);

  if (OutFilename2)
    tlist.AddToList(&write2); 
  // Add tasks to write the Events and RunHeaders trees to output.

  if (!evtloop.Eventloop())
    return;


  tlist.PrintStatistics();
}
