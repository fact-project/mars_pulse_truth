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
//  STARMC2 - STandard Analysis and Reconstruction (MC example)
//
//  This macro converts into image  parameters an input file of MC data 
//  previously calibrated (see mccalibrate.C).
//  
//
/////////////////////////////////////////////////////////////////////////////

#include "MImgCleanStd.h"

void starmc2()
{
  //  Char_t* AnalysisFilename = "calibrated_gamma_train.root"; // File to be analyzed
  Char_t* AnalysisFilename = "calibrated_gamma_test.root"; // File to be analyzed

  TString* OutFilename;

  // Output file name
  //  OutFilename = new TString("star_gamma_train.root");   // Output file name
  OutFilename = new TString("star_gamma_test.root");   // Output file name

  MImgCleanStd      clean(10., 5.); // Applies tail cuts to image.
  clean.SetMethod(MImgCleanStd::kAbsolute);

  // WARNING: the tightness of the tail cuts depends on the signal extraction method
  // used in mccalibrate.C!! (some methods result in positively biased signals)

  // ------------------------------------------------------------------

  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;

  MTaskList tlist;

  plist.AddToList(&tlist);

  MSrcPosCam src;
  //
  // FOR WOBBLE MODE!! Set source position on camera here.
  //  src.SetX(120.);  
  // units: mm. This 120 mm correspond to MC wobble mode w+ for zbin>0
  //

  src.SetReadyToSave();

  plist.AddToList(&src);

  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //
  MReadMarsFile read("Events");

  read.AddFile(AnalysisFilename);
  read.DisableAutoScheme();


  MHillasCalc       hcalc; // Calculates Hillas parameters not dependent on source position.
  hcalc.Enable(MHillasCalc::kCalcHillasSrc);

  tlist.AddToList(&read);
  tlist.AddToList(&clean);
  tlist.AddToList(&hcalc);

  //
  // Open output file(s):
  //
  MWriteRootFile write(OutFilename->Data()); // Writes output   
  //
  // Add MC containers only if they exist. 
  // In this way you can also run on real calibrated data.
  //
  write.AddContainer("MRawRunHeader", "RunHeaders");
  write.AddContainer("MMcRunHeader",  "RunHeaders", kFALSE);
  write.AddContainer("MGeomCam",      "RunHeaders", kFALSE);
  write.AddContainer("MMcConfigRunHeader",  "RunHeaders", kFALSE);
  write.AddContainer("MMcCorsikaRunHeader", "RunHeaders", kFALSE);
  write.AddContainer("MMcFadcHeader",  "RunHeaders", kFALSE);
  write.AddContainer("MMcTrigHeader",  "RunHeaders", kFALSE);
       
  write.AddContainer("MMcEvt",        "Events", kFALSE);
  write.AddContainer("MPointingPos",  "Events", kFALSE);
  write.AddContainer("MMcTrig",       "Events", kFALSE);
  write.AddContainer("MSrcPosCam",    "Events", kFALSE);
  write.AddContainer("MRawEvtHeader", "Events");
  write.AddContainer("MHillas",       "Events");
  write.AddContainer("MHillasExt",    "Events");
  write.AddContainer("MHillasSrc",    "Events");
  write.AddContainer("MImagePar",     "Events");
  write.AddContainer("MNewImagePar",  "Events");
  write.AddContainer("MConcentration","Events");

  //
  // Next line makes that the tree "OriginalMC" containing the MMcEvtBasic
  // container for all produced Corsika events from which the events in the
  // normal "Events" tree derive:
  //
  write.AddCopySource("OriginalMC");

  tlist.AddToList(&write);

  //
  // analysis loop
  //

  MEvtLoop evtloop;
  MProgressBar bar;
  bar.SetWindowName("Analyzing...");
  evtloop.SetProgressBar(&bar);
  evtloop.SetParList(&plist);

  if (!evtloop.Eventloop())
    return;

  tlist.PrintStatistics();

  return;
}
