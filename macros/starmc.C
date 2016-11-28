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
//  STARMC - STandard Analysis and Reconstruction (MC example)
//
//  This macro is a version of the standard converter to convert raw data 
//  into image  parameters, made to show how to run analysis on MC files. 
//
/////////////////////////////////////////////////////////////////////////////

#include "MImgCleanStd.h"

void starmc()
{
  //
  // This is a demonstration program which calculates the image 
  // parameters from Magic Monte Carlo files (output of camera).

  TString* CalibrationFilename;
  TString* OutFilename1;
  TString* OutFilename2;

  // ------------- user change -----------------
  //
  // Comment line starting "CalibrationFileName" to disable calibration. In that 
  // case the units of the MHillas.fSize parameter will be ADC counts (rather, 
  // equivalent ADC counts in inner pixels, since we correct for the possible 
  // differences in gain of outer pixels)
  //
  CalibrationFilename = new TString("nonoise/Gamma_zbin0_0_7_1000to1009_w0.root");
  // File to be used in the calibration (must be a camera file without added noise)

  Char_t* AnalysisFilename = "Gamma_zbin0_0_*.root";  // File to be analyzed


  // ------------- user change -----------------
  //
  // Change output file names as desired. If you want only one output, comment
  // the initialization of OutFilename2.

  OutFilename1 = new TString("star_train.root");   // Output file name 1 (test)
  OutFilename2 = new TString("star_test.root"); // Output file name 2 (train)
  //
  // Fraction of events (taken at random) which one wants to process from the 
  // file to be analyzed (useful to make smaller files if starting sample is
  // too large).
  //
  Float_t accepted_fraction = 1.;

  // USER CHANGE: tail cuts for image analysis

  Float_t CleanLev[2] = {10., 5.}; // Tail cuts for the analysis loop
  MImgCleanStd clean2(CleanLev[0], CleanLev[1]); // Applies tail cuts to image.
  clean2.SetMethod(MImgCleanStd::kAbsolute); 

  //  USER CHANGE: signal extraction

  //  MExtractTimeAndChargeDigitalFilter sigextract;
  //  sigextract.SetNameWeightsFile("/users/emc/moralejo/Mars/msignal/MC_weights.dat");
  //  sigextract.SetRange(1, 14, 3, 14);

  MExtractTimeAndChargeSpline sigextract;
  sigextract.SetChargeType(MExtractTimeAndChargeSpline::kIntegral);
  sigextract.SetRiseTimeHiGain(0.5);
  sigextract.SetFallTimeHiGain(0.5);

  // USER CHANGE: high to low gain ratio. DEPENDS on EXTRACTOR!!
  // One should calculate somewhere else what this factor is for each extractor!

  //  Float_t hi2low = 10.83; 
  // value for digital filter, HG window 4, LG window 6

  Float_t hi2low = 11.28; 
  // value for spline with risetime 0.5, fall time 0.5, low gain stretch 1.5


  ////////// Calibration //////////
  MMcCalibrationUpdate  mccalibupdate;
  mccalibupdate.SetUserLow2HiGainFactor(hi2low);

  ///// USER CHANGE: calibrate in photons or phe- :
  mccalibupdate.SetSignalType(MCalibrateData::kPhe);
  //  mccalibupdate.SetSignalType(MCalibrateData::kPhot);


  MImgCleanStd  clean(0.,0.); 
  // All pixels containing any photon will be accepted. This is what we want
  // for the calibration loop (since no noise is added)
  clean.SetMethod(MImgCleanStd::kAbsolute); 
  // In absolute units (phot or phe- as chosen below)

  MSrcPosCam src;
  //
  // ONLY FOR WOBBLE MODE!! : set the rigt source position on camera!
  //   src.SetX(120.);   // units: mm. Value for MC w+ files
  //   src.SetX(-120.);  // units: mm. Value for MC w- files

  src.SetReadyToSave();

  // -------------------------------------------

  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;

  MTaskList tlist;

  plist.AddToList(&tlist);

  src.SetReadyToSave();
  plist.AddToList(&src);

  MBadPixelsCam badpix;
  plist.AddToList(&badpix);


  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //
  MReadMarsFile read("Events");

  if (CalibrationFilename)
    read.AddFile(CalibrationFilename->Data());

  read.DisableAutoScheme();

  MGeomApply geom; // Reads in geometry from MC file and sets the right sizes for
  // several parameter containers.

  MMcPedestalCopy   pcopy; 
  // Copies pedestal data from the MC file run fadc header to the MPedestalCam container.

  MPointingPosCalc pointcalc;
  // Creates MPointingPos object and fill it with the telescope orientation
  // information taken from MMcEvt.

  MCalibrateData calib; // Applies calibration to the data
  calib.SetCalibrationMode(MCalibrateData::kFfactor);
  // Do not change the CalibrationMode above for MC...!
  calib.SetSignalType(mccalibupdate.GetSignalType());

  //    MBlindPixelCalc   blind;
  //    blind.SetUseInterpolation();

  MHillasCalc       hcalc; // Calculates Hillas parameters not dependent on source position.
  hcalc.Disable(MHillasCalc::kCalcHillasSrc);

  MMcCalibrationCalc mccalibcalc;

  tlist.AddToList(&read);
  tlist.AddToList(&geom);
  tlist.AddToList(&pcopy);
  tlist.AddToList(&pointcalc);

  tlist.AddToList(&sigextract);
  tlist.AddToList(&mccalibupdate);
  tlist.AddToList(&calib);
  tlist.AddToList(&clean);
  //    tlist.AddToList(&blind);
  tlist.AddToList(&hcalc);

  tlist.AddToList(&mccalibcalc);

  //
  // Open output files:
  //

  MWriteRootFile write1(OutFilename1->Data()); // Writes output1
  write1.AddContainer("MRawRunHeader", "RunHeaders");
  write1.AddContainer("MMcRunHeader",  "RunHeaders");
  write1.AddContainer("MSrcPosCam",    "RunHeaders");
  write1.AddContainer("MGeomCam",      "RunHeaders");
  write1.AddContainer("MMcConfigRunHeader",  "RunHeaders");
  write1.AddContainer("MMcCorsikaRunHeader", "RunHeaders");
  write1.AddContainer("MMcFadcHeader",  "RunHeaders");
  write1.AddContainer("MMcTrigHeader",  "RunHeaders");


  write1.AddContainer("MMcEvt",        "Events");
  write1.AddContainer("MHillas",       "Events");
  write1.AddContainer("MHillasExt",    "Events");
  write1.AddContainer("MHillasSrc",    "Events");
  write1.AddContainer("MImagePar",     "Events");
  write1.AddContainer("MNewImagePar",  "Events");
  write1.AddContainer("MConcentration","Events");
  write1.AddContainer("MPointingPos",  "Events");

  if (OutFilename2)
    {
      MWriteRootFile write2(OutFilename2->Data()); // Writes output2
      write2.AddContainer("MRawRunHeader", "RunHeaders");
      write2.AddContainer("MMcRunHeader",  "RunHeaders");
      write2.AddContainer("MSrcPosCam",    "RunHeaders");
      write2.AddContainer("MGeomCam",      "RunHeaders");
      write2.AddContainer("MMcConfigRunHeader",  "RunHeaders");
      write2.AddContainer("MMcCorsikaRunHeader", "RunHeaders");
      write2.AddContainer("MMcFadcHeader",  "RunHeaders");
      write2.AddContainer("MMcTrigHeader",  "RunHeaders");


      write2.AddContainer("MMcEvt",        "Events");
      write2.AddContainer("MHillas",       "Events");
      write2.AddContainer("MHillasExt",    "Events");
      write2.AddContainer("MHillasSrc",    "Events");
      write2.AddContainer("MImagePar",     "Events");
      write2.AddContainer("MNewImagePar",  "Events");
      write2.AddContainer("MConcentration","Events");
      write2.AddContainer("MPointingPos",  "Events");

      //
      // Divide output in train and test samples, using the event number
      // (odd/even) to achieve otherwise unbiased event samples:
      //
      
      MF filter1("{MMcEvt.fEvtNumber%2}>0.5");
      MF filter2("{MMcEvt.fEvtNumber%2}<0.5");

      write1.SetFilter(&filter1);
      write2.SetFilter(&filter2);
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
    }

  //
  // Second loop: analysis loop
  //

  //
  // Change the read task by another one which reads the file we want to analyze:
  //

  MReadMarsFile read2("Events");
  read2.AddFile(AnalysisFilename);
  read2.DisableAutoScheme();
  tlist.AddToListBefore(&read2, &read);
  tlist.RemoveFromList(&read);

  tlist.AddToListBefore(&clean2, &clean);
  tlist.RemoveFromList(&clean);

  //
  // Analyzed only the desired fraction of events, skip the rest:
  //
  MFEventSelector eventselector;
  Float_t rejected_fraction = 1. - accepted_fraction; 
  eventselector.SetSelectionRatio(rejected_fraction);
  MContinue skip(&eventselector);
  tlist.AddToListBefore(&skip, &sigextract);

  bar.SetWindowName("Analyzing...");

  tlist.RemoveFromList(&mccalibcalc); // Removes calibration task from list.

  hcalc.Enable(MHillasCalc::kCalcHillasSrc);

  // Add tasks to write output:

  if (OutFilename2)
    {
      tlist.AddToList(&filter1);
      tlist.AddToList(&filter2);
      tlist.AddToList(&write2); 
    }

  tlist.AddToList(&write1); 

  if (!evtloop.Eventloop())
    return;


  tlist.PrintStatistics();
}
