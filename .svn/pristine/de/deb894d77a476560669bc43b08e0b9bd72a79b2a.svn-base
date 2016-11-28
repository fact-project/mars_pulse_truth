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
!   Author(s):  Abelardo Moralejo 1/2004 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  STARMCSTEREO - STandard Analysis and Reconstruction (for MC stereo files)
//
//  This macro is the standard converter to convert raw data from stereo 
//  camera simulation into image parameters
//
/////////////////////////////////////////////////////////////////////////////

void starmcstereo(Int_t ct1 = 1, Int_t ct2 = 2)
{
  // ------------- user change -----------------

  TString* CalibrationFilename = 0;

  // Calibration file: a file with no added noise. Comment out next line if you
  // do not want to calibrate the data (means SIZE will be in ADC counts)

  CalibrationFilename = new TString("nonoise/Gamma_20_0_7_200000to200009_XX_w0.root");

  Char_t* AnalysisFilename = "Gamma_20_0_7_*_XX_w0.root";  // File to be analyzed
  Char_t* OutFileTag      = "gammas";           // Output file tag

  // First open input files to check that the required telescopes 
  // are in the file, and get telescope coordinates.

  TChain *rh = new TChain("RunHeaders");
  rh->Add(AnalysisFilename);
  MMcCorsikaRunHeader *corsrh = new MMcCorsikaRunHeader();
  rh->SetBranchAddress("MMcCorsikaRunHeader.", &corsrh);
  rh->GetEvent(0); 
  // We assume that all the read files will have the same telescopes inside, 
  // so we look only into the first runheader.
  Int_t allcts = corsrh->GetNumCT();
  if (ct1 >  allcts || ct2 >  allcts)
    {
      cout << endl << "Wrong CT id number, not contained in input file!" << endl;
      return;
    }
  // Set telescope coordinates as read from first runheader:
  Float_t ctx[2];
  Float_t cty[2];
  ctx[0] = ((*corsrh)[ct1-1])->GetCTx();
  cty[0] = ((*corsrh)[ct1-1])->GetCTy();
  ctx[1] = ((*corsrh)[ct2-1])->GetCTx();
  cty[1] = ((*corsrh)[ct2-1])->GetCTy();

  // Now find out number of pixels in each camera:
  MMcConfigRunHeader* confrh1 = new MMcConfigRunHeader();
  MMcConfigRunHeader* confrh2 = new MMcConfigRunHeader();
  rh->SetBranchAddress("MMcConfigRunHeader;1.", &confrh1);
  rh->SetBranchAddress("MMcConfigRunHeader;2.", &confrh2);
  rh->GetEvent(0);
  Int_t npix[2];
  npix[0] = confrh1->GetNumPMTs();
  npix[1] = confrh2->GetNumPMTs();

  rh->Delete();


  Int_t CT[2] = {ct1, ct2};  // Only 2-telescope analysis for the moment
  Int_t NCTs = 2;


  // ------------- user change -----------------

  Float_t BinsHigh[2] = {0, 79};
  Float_t BinsLow[2]  = {0, 79};  // FADC slices (2GHz sampling)
  Float_t CleanLev[2] = {7., 5.}; // Units: phes (absolute cleaning will be used later!) 
  // Tail cuts for the analysis loop. In the first (calibration) loop they will not 
  // be used; we run over a noiseless file and we want to accept all pixels with any 
  // number of phes.


  MImgCleanStd**     clean = new MImgCleanStd*[NCTs];  

  MImgCleanStd* clean[0] = new MImgCleanStd(1.,1.);
  MImgCleanStd* clean[1] = new MImgCleanStd(1.,1.); 
  // Just dummy levels. Since the calibration file will be a noiseless file, RMS of 
  // pedestal will be 0, and all events will be accepted, regardless of the cleaning level.
  // For some reason the above lines do not work if made on a loop! (???)
  clean[0]->SetSerialNumber(CT[0]);
  clean[1]->SetSerialNumber(CT[1]);

  MExtractTimeAndChargeSpline* sigextract = new MExtractTimeAndChargeSpline[NCTs];
  MMcCalibrationUpdate*  mccalibupdate = new MMcCalibrationUpdate[NCTs];
  MCalibrateData* calib = new MCalibrateData[NCTs];
  MMcCalibrationCalc* mccalibcalc = new MMcCalibrationCalc[NCTs];

  // -------------------------------------------
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;
  MTaskList tlist;
  plist.AddToList(&tlist);

  MSrcPosCam src[NCTs];
  MBadPixelsCam badpix[NCTs];

  Float_t hi2lowratio = 10.0;

  for (Int_t i = 0; i < NCTs; i++)
    {
      TString s = "MSrcPosCam;";
      s += CT[i];
      src[i].SetName(s);
      src[i].SetReadyToSave();
      plist.AddToList(&(src[i]));

      TString b = "MBadPixelsCam;";
      b += CT[i];
      badpix[i].SetName(b);
      badpix[i].InitSize(npix[i]);
      badpix[i].SetReadyToSave();
      plist.AddToList(&(badpix[i]));

      sigextract[i].SetSerialNumber(CT[i]);
      sigextract[i].SetRange(BinsHigh[0], BinsHigh[1], BinsLow[0], BinsLow[1]);
      sigextract[i].SetRiseTimeHiGain(0.5);
      sigextract[i].SetFallTimeHiGain(0.5);
      sigextract[i].SetLoGainStretch(1.);

      mccalibupdate[i].SetSerialNumber(CT[i]);
      mccalibupdate[i].SetUserLow2HiGainFactor(hi2lowratio);
      mccalibupdate[i].SetSignalType(MCalibrateData::kPhe);

      calib[i].SetSerialNumber(CT[i]);
      calib[i].SetCalibConvMinLimit(0.);
      calib[i].SetCalibConvMaxLimit(100.); // Override limits for real data  
      calib[i].SetCalibrationMode(MCalibrateData::kFfactor);
      // Do not change CalibrationMode (just indicates where the cal. constants will be stored)
      calib[i].SetSignalType(mccalibupdate[i].GetSignalType());

      mccalibcalc[i].SetSerialNumber(CT[i]);
      mccalibcalc[i].SetMinSize(200); 
      // Minimum SIZE for an event to be used in the calculation of calibration constants.
      // Units are ADC counts, and value depends on signal extractor!
    }


  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //
  MReadMarsFile read("Events");
  read.DisableAutoScheme();

  if (CalibrationFilename)
    read.AddFile(CalibrationFilename->Data());


  MGeomApply*        apply = new MGeomApply[NCTs];
  MMcPedestalCopy*   pcopy = new MMcPedestalCopy[NCTs];
  MHillasCalc*       hcalc = new MHillasCalc[NCTs];

  TString outfile = "star_";
  outfile += CT[0];
  outfile += "_";
  outfile += CT[1];

  //
  // We have two output files (will be later train and test sampls for random forest)
  //
  outfile += "_";
  outfile += OutFileTag;
  outfile += "_train.root";
  MWriteRootFile    write1(outfile);

  outfile = "star_";
  outfile += CT[0];
  outfile += "_";
  outfile += CT[1];

  outfile += "_";
  outfile += OutFileTag; 
  outfile += "_test.root";

  MWriteRootFile write2(outfile);

  for (Int_t i = 0; i < NCTs; i++)
    {
      apply[i]->SetSerialNumber(CT[i]);
      pcopy[i]->SetSerialNumber(CT[i]);

      hcalc[i]->SetSerialNumber(CT[i]);
      hcalc[i].Disable(MHillasCalc::kCalcHillasSrc); 
      // Source-dependent parameters not needed in the first loop (calibration)

      write1.SetSerialNumber(CT[i]);
      write2.SetSerialNumber(CT[i]);

      write1.AddContainer("MMcEvt",       "Events");
      write1.AddContainer("MHillas",      "Events");
      write1.AddContainer("MHillasExt",   "Events");
      write1.AddContainer("MHillasSrc",   "Events");
      write1.AddContainer("MNewImagePar", "Events");
      write1.AddContainer("MSrcPosCam",   "Events");
      write2.AddContainer("MMcEvt",       "Events");
      write2.AddContainer("MHillas",      "Events");
      write2.AddContainer("MHillasExt",   "Events");
      write2.AddContainer("MHillasSrc",   "Events");
      write2.AddContainer("MNewImagePar", "Events");
      write2.AddContainer("MSrcPosCam",   "Events");
    }

  MStereoPar* mstereo = new MStereoPar;
  plist.AddToList(mstereo);

  write1.AddContainer(mstereo, "Events");
  write2.AddContainer(mstereo, "Events");
  // We use MWriteRootFile::AddContainer(MParContainer* ,...) instead
  // of using the container name as above, because in the former case the 
  // serial number tag (indicating the telescope id) is added to the 
  // container name, which is fine for containers of which there is one
  // per telescope. However, the container MStereoPar is unique, since it
  // is filled with information coming from both telescopes.

  write1.AddContainer("MRawRunHeader", "RunHeaders");
  write1.AddContainer("MMcRunHeader",  "RunHeaders");

  write2.AddContainer("MRawRunHeader", "RunHeaders");
  write2.AddContainer("MMcRunHeader",  "RunHeaders");

  tlist.AddToList(&read);

  // Skip untriggered events (now camera simulation output contains by default all simulated events)
  MContinue* trigger = new MContinue("(MMcTrig;1.fNumFirstLevel<1) && (MMcTrig;2.fNumFirstLevel<1)","Events");
  tlist.AddToList(trigger);

  for (i = 0; i < NCTs; i++)
    {
      tlist.AddToList(&(apply[i]));
      tlist.AddToList(&(pcopy[i]));
      tlist.AddToList(&(sigextract[i]));
      tlist.AddToList(&(mccalibupdate[i]));
      tlist.AddToList(&(calib[i]));
      tlist.AddToList(clean[i]);
      tlist.AddToList(&(hcalc[i]));
      tlist.AddToList(&(mccalibcalc[i]));
    }


  MF filter1("{MMcEvt;1.fEvtNumber%2}<0.5");
  MF filter2("{MMcEvt;1.fEvtNumber%2}>0.5");
  //
  // ^^^^ Filters to divide output in two: test and train samples.
  //
  write1.SetFilter (&filter1);
  write2.SetFilter (&filter2);

  //
  // Create and set up the eventloop
  //
  MProgressBar bar;
  bar.SetWindowName("Calibrating");

  MEvtLoop evtloop;
  evtloop.SetProgressBar(&bar);
  evtloop.SetParList(&plist);

  //
  // First loop: calibration loop. Go over MC events simulated with 
  // no noise, to correlate SIZE with the number of phes and get the
  // conversion factor (this is done by MMcCalibrationCalc).
  //
  if (CalibrationFilename)
    {
      if (!evtloop.Eventloop())
	return;
    }

  tlist.PrintStatistics();

  ///////////////////////////////////////////////////////////////////////


  // Now prepare the second loop: go over the events you want to analyze.
  // This time the MMcCalibrationUpdate tasks will apply the previously
  // calculated calibration factors.

  // First substitute the reading task:
  MReadMarsFile read2("Events");
  read2.AddFile(AnalysisFilename);  
  read2.DisableAutoScheme();
  tlist.AddToListBefore(&read2, &read);
  tlist.RemoveFromList(&read);

  // Delete cleaning tasks and create new ones with absolute cleaning method:
    for (Int_t i= 0; i < NCTs; i++ )
    {
      tlist.RemoveFromList(clean[i]);
      delete clean[i];
    }

  // New cleaning tasks:
  clean[0] = new MImgCleanStd(CleanLev[0], CleanLev[1]);
  clean[1] = new MImgCleanStd(CleanLev[0], CleanLev[1]);
  clean[0]->SetMethod(MImgCleanStd::kAbsolute); 
  clean[1]->SetMethod(MImgCleanStd::kAbsolute); 
  clean[0]->SetSerialNumber(CT[0]);
  clean[1]->SetSerialNumber(CT[1]);
  tlist.AddToListBefore(clean[0],&(hcalc[0]));
  tlist.AddToListBefore(clean[1],&(hcalc[1]));

  tlist.RemoveFromList(&(mccalibcalc[0]));
  tlist.RemoveFromList(&(mccalibcalc[1])); // Remove calibration tasks from list.

  // Now calculate also source-dependent Hillas parameters:
  for (i = 0; i < NCTs; i++)
    hcalc[i].Enable(MHillasCalc::kCalcHillasSrc);

  // Add task to calculate stereo parameters:
  MStereoCalc stereocalc;
  stereocalc.SetCTids(CT[0],CT[1]);
  stereocalc.SetCT1coor(ctx[0],cty[0]);
  stereocalc.SetCT2coor(ctx[1],cty[1]);
  tlist.AddToList(&stereocalc);

  // Add writing tasks:
  tlist.AddToList(&filter1);
  tlist.AddToList(&write1);
  tlist.AddToList(&filter2);
  tlist.AddToList(&write2);

  bar.SetWindowName("Analyzing");

  if (!evtloop.Eventloop())
    return;

  tlist.PrintStatistics();

  for (Int_t i= 0; i < NCTs; i++ )
    delete clean[i];

  plist.FindObject("MCalibrationChargeCam;1")->Write();
  plist.FindObject("MCalibrationChargeCam;2")->Write();

  plist.FindObject("MCalibrationQECam;1")->Write();
  plist.FindObject("MCalibrationQECam;2")->Write();

  return;
}
