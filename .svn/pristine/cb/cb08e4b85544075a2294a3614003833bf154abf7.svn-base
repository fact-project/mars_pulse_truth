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
!   Author(s): Hendrik Bartko, 03/2004 <mailto:hbartko@mppmu.mpg.de>
!              Markus Gaug,    03/2004 <mailto:markus@ifae.es>
!              Wolfgang Wittek 07/2004 <mailto:wittek@mppmu.mpg.de>
!              Daniel Mazin    08/2004 <mailto:mazin@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//-------------------------------------
//
//  changes by W. Wittek :
//      - call SetRange() and SetWindows() for 
//                        MExtractedFixedWindowPeakSearch
//      - call SetRange() and SetWindowSize() for MPedCalcPedRun
//      - call MPedCalcFromData in 3rd loop (data calibration)
//
//-------------------------------------



//const TString defpath = "/local/rootdata/2004_08_23/";

//  const TString defpath = "/mnt/data21a/Crab_August/";

   const TString defpath = "/home/pcmagic04/pratik/analysis/2004_08_14/";

//const TString defcontinuoslightfile =   
//    "/.magic/magicserv01/MAGIC/rootdata2/2004_04_22/20040813_32834_C_3EG1727+04-100CL_E.root";
const TString defcontinuoslightfile =   
    "/mnt/data21a/Crab_August/20040820_34060_C_CrabNebula-100CL_E.root";

//  ON data Mkn 421

const TString defrout = "34994_test.root";
const Int_t defpedr  = 10060; //      {17191,17192};
const Int_t defcalr  = 10051; //      {17193,17198,17200};
const Int_t defdatar = 23210; //      {17206,17207,17208};


//void calibrate_data_pedestal(const TString inpath=defpath, 
//               const Int_t pedrun=defpedr, 
//               const Int_t calrun=defcalr, 
//               const Int_t datarun=defdatar, 
//	       const TString continuoslightfile=defcontinuoslightfile,
//               const TString resname=defrout)

void calibrate_data_ped_standard(const TString inpath = defpath, 
                                 const Int_t pedrun = defpedr, 
                                 const Int_t calrun = defcalr, 
                                 const Int_t datarun = defdatar, 
	                         const TString continuoslightfile = defcontinuoslightfile,
                                 const TString resname = defrout)

{
 cout << " inpath : " << inpath << endl;
 cout << " pedrun : " << pedrun << endl;
 cout << " calrun : " << calrun << endl;
 cout << " datarun : " << datarun << endl;
 cout << " continuoslightfile : " << continuoslightfile << endl;
 cout << " FILE to write : " << resname << endl;

  gLog.SetNoColors();
  // the parameters should be identical to the ones of MPedCalcPedRun

  Int_t WindowSize = 6;  // size of the chosen integration window for signal and time extraction
 
  MExtractFixedWindowPeakSearch extractor;
  extractor.SetRange(0, 14, 0, 14);
  extractor.SetWindows( WindowSize, WindowSize, 4);



  MExtractTimeHighestIntegral   timeext;
  timeext.SetRange(0, 14, 0, 14);
  timeext.SetWindowSize(WindowSize,WindowSize);


  MRunIter pruns;
  MRunIter cruns;
  MRunIter druns;
  
/*
  for (Int_t i=0;i<psize;i++) {
    cout << "Adding pedestal run: " << pedruns[i] << endl;
    pruns.AddRun(pedruns[i],inpath);
  }
  for (Int_t i=0;i<csize;i++) {
    cout << "Adding calibration run: " << calruns[i] << endl;
    cruns.AddRun(calruns[i],inpath);
  }
  for (Int_t i=0;i<dsize;i++) {
    cout << "Adding data run: " << dataruns[i] << endl;
    druns.AddRun(dataruns[i],inpath);
  }
*/

    cout << "Adding pedestal run: " << pedrun << endl;
    pruns.AddRun(pedrun,inpath);
    cout << "Adding calibration run: " << calrun << endl;
    cruns.AddRun(calrun,inpath);
    cruns.AddRun(10001,inpath);
cruns.AddRun(10002,inpath);
cruns.AddRun(10003,inpath);
cruns.AddRun(10004,inpath);
cruns.AddRun(10005,inpath);
cruns.AddRun(10006,inpath);
cruns.AddRun(10007,inpath);
cruns.AddRun(10008,inpath);
cruns.AddRun(10009,inpath);
cruns.AddRun(10010,inpath);
cruns.AddRun(10011,inpath);
cruns.AddRun(10012,inpath);
cruns.AddRun(10013,inpath);
cruns.AddRun(10014,inpath);
cruns.AddRun(10015,inpath);
cruns.AddRun(10016,inpath);
cruns.AddRun(10017,inpath);
cruns.AddRun(10018,inpath);
cruns.AddRun(10019,inpath);
cruns.AddRun(10020,inpath);
cruns.AddRun(10021,inpath);
cruns.AddRun(10022,inpath);
cruns.AddRun(10023,inpath);
cruns.AddRun(10024,inpath);
cruns.AddRun(10025,inpath);
cruns.AddRun(10026,inpath);
cruns.AddRun(10027,inpath);
cruns.AddRun(10028,inpath);
cruns.AddRun(10029,inpath);
cruns.AddRun(10030,inpath);
cruns.AddRun(10031,inpath);
cruns.AddRun(10032,inpath);
cruns.AddRun(10033,inpath);
cruns.AddRun(10034,inpath);
cruns.AddRun(10035,inpath);
cruns.AddRun(10036,inpath);
cruns.AddRun(10037,inpath);
cruns.AddRun(10038,inpath);
cruns.AddRun(10039,inpath);
cruns.AddRun(10040,inpath);
cruns.AddRun(10041,inpath);
cruns.AddRun(10042,inpath);
cruns.AddRun(10043,inpath);
cruns.AddRun(10044,inpath);
cruns.AddRun(10045,inpath);
cruns.AddRun(10046,inpath);
cruns.AddRun(10047,inpath);
cruns.AddRun(10048,inpath);
cruns.AddRun(10049,inpath);
cruns.AddRun(10050,inpath);


   cout << "Adding data run: " << datarun << endl;
    druns.AddRun(datarun,inpath);


  MStatusDisplay *display = new MStatusDisplay;
  display->SetUpdateTime(3000);
  display->Resize(850,700);

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit();

  /************************************/
  /* FIRST LOOP: PEDESTAL COMPUTATION */
  /************************************/
  
  MParList plist1;
  MTaskList tlist1;
  plist1.AddToList(&tlist1);
  
  // containers
  MPedestalCam   pedcam;
  MBadPixelsCam  badcam;
  //
  // for excluding pixels from the beginning:
  //
  // badcam.AsciiRead("badpixels.dat");


  plist1.AddToList(&pedcam);
  plist1.AddToList(&badcam);
    
  //tasks
  MReadMarsFile  read("Events");
  read.DisableAutoScheme();
  static_cast<MRead&>(read).AddFiles(pruns);

  MGeomApply     geomapl;

  // the parameters should be identical to the ones of the extractor
  MPedCalcPedRun pedcalc;
  pedcalc.SetRange(0,14,0,14);
  pedcalc.SetWindowSize( WindowSize, WindowSize);

  MGeomCamMagic  geomcam;
    
  tlist1.AddToList(&read);
  tlist1.AddToList(&geomapl);
  tlist1.AddToList(&pedcalc);

  // Create and execute the event looper
  MEvtLoop pedloop;
  pedloop.SetParList(&plist1);
  pedloop.SetDisplay(display);

  cout << "*************************" << endl;
  cout << "** COMPUTING PEDESTALS **" << endl;
  cout << "*************************" << endl;

  if (!pedloop.Eventloop())
    return;
  
  tlist1.PrintStatistics();

  // 
  // Now the short version: 
  //
  //
  // Now setup the new tasks for the calibration:
  // ---------------------------------------------------
  //
  MJCalibration     calloop;
  calloop.SetInput(&cruns);


//  const MCalibrationCam::PulserColor_t color=MCalibrationCam::kGREEN;
//cout << "COLOR = " << color << endl;


  //calloop.SetColor(color);  // neu
  //  calloop.SetFullDisplay();
  //
  calloop.SetExtractor(&extractor);
  //
  // Set the corr. cams:
  //
  calloop.SetBadPixels(badcam);
  //
  // The next two commands are for the display:
  //
  calloop.SetDisplay(display);
//  calloop.SetFullDisplay(); //new
  
  //
  // Apply rel. time calibration:
  //
    //calloop.SetRelTimeCalibration();
    //calloop.SetTimeExtractor(&timeext);
  //
  // Do the event-loop:
  //
  cout << "***************************" << endl;
  cout << "** COMPUTING CALIBRATION **" << endl;
  cout << "***************************" << endl;
  
  if (!calloop.Process(pedcam))
    return;
cout << " end of Process " << endl;

  badcam.Print();
  
  MBadPixelsCam          &badbad  = calloop.GetBadPixels();
  MCalibrationChargeCam  &calcam  = calloop.GetCalibrationCam();
  MCalibrationRelTimeCam &timecam = calloop.GetRelTimeCam();
  MCalibrationQECam      &qecam   = calloop.GetQECam();

  badbad.Print();

  /************************************************************************/
  /*                THIRD LOOP: DATA CALIBRATION INTO PHOTONS             */
  /************************************************************************/


  // First: a small loop to calculate the pedestals from the data for the first some events


  // Create an empty Parameter List and an empty Task List


  MParList plist3a;
  MTaskList tlist3a;
  plist3a.AddToList(&tlist3a);
  
  MPedestalCam        pedcamdata;
  pedcamdata.SetName("MPedestalCamFromData");
 

  plist3a.AddToList(&pedcamdata);
 

  MRawRunHeader       runhead3a;
  plist3a.AddToList(&geomcam );
  plist3a.AddToList(&runhead3a);


   
  //tasks
 
  MPedCalcFromLoGain peddatacalc;
 

  peddatacalc.SetPedContainerName("MPedestalCamFromData");
  peddatacalc.SetPedestalUpdate(kFALSE);
  peddatacalc.SetRange( 0, 11, 1, 14);
  peddatacalc.SetWindowSize( 12, 14);
  peddatacalc.SetNumEventsDump(500);
  peddatacalc.SetMaxHiGainVar(40);

 
  MReadMarsFile read3a("Events");
  read3a.DisableAutoScheme();
  static_cast<MRead&>(read3a).AddFiles(druns);
  
  tlist3a.AddToList(&read3a);
  tlist3a.AddToList(&geomapl);
  tlist3a.AddToList(&peddatacalc);


  MEvtLoop evtloop3a;
  evtloop3a.SetParList(&plist3a);
  
  cout << " event loop over the first 500 data events for pedestal calculation" << endl;

  if ( !evtloop3a.Eventloop(500) )
    return;

//   if (!evtloop3a.PreProcess())
//     return;
  
//   for (int i=0; i<500; i++){
//     if (!tlist3a.Process()) break;
//   }
  
//   evtloop3a.PostProcess();


  // Now the main third loop for the cosmics and pedestal calibration


  MParList  plist3;  
  MTaskList tlist3;
  plist3.AddToList(&tlist3);
 

  // containers
 
  MCerPhotEvt         photevt;
  MPedPhotCam         pedphotcam;

  MPedPhotCam         pedphotcamdata;
  pedphotcamdata.SetName("MPedPhotCamFromData");
 
  MSrcPosCam          srccam;
  MRawRunHeader       runhead;
  MExtractedSignalCam sigcam;
//  MSrcPosCam          pntcam;  //new!
//  pntcam.SetName("MPntPosCam");

//  MCameraDC     dccam;
//  MStarCam      starcam;

// changes by wolfgang:
  MObservatory  obs;
//  MStarCam      sourcecam;
//  sourcecam.SetName("MSourceCam");
//  MHTelAxisFromStars htelaxis;
  plist3.AddToList(&obs);
//  plist3.AddToList(&htelaxis);
//  plist3.AddToList(&sourcecam);
//  plist3.AddToList(&pntcam);  //new!
// end changes

  plist3.AddToList(&geomcam );
  plist3.AddToList(&pedcam);
  plist3.AddToList(&pedcamdata);
  plist3.AddToList(&calcam  );
  plist3.AddToList(&qecam   );
  plist3.AddToList(&badbad  );
  plist3.AddToList(&timecam );
  plist3.AddToList(&sigcam  );
  plist3.AddToList(&photevt);
  plist3.AddToList(&pedphotcam);
  plist3.AddToList(&pedphotcamdata);
  plist3.AddToList(&srccam);
  plist3.AddToList(&runhead);
//  plist3.AddToList(&dccam);
//  plist3.AddToList(&starcam);


  //tasks
//  MReadMarsFile read3("Events");

  MReadReports read3;
  read3.AddTree("Currents"); 
  read3.AddTree("Drive"); 
  read3.AddTree("Events","MTime.",kTRUE); 

//  read3.DisableAutoScheme();
  static_cast<MRead&>(read3).AddFiles(druns);

  read3.AddToBranchList("MReportCurrents.*");
  read3.AddToBranchList("MReportDrive.*");
  read3.AddToBranchList("MRawCrateArray.*");
  read3.AddToBranchList("MRawEvtData.*");
  read3.AddToBranchList("MRawEvtHeader.*");


  Float_t mindc = 0.9; //[uA]
  MCalibrateDC dccal;
  dccal.SetFileName(continuoslightfile);
  dccal.SetMinDCAllowed(mindc);

  const Int_t numblind = 5;
  const Short_t x[numblind] = { 47, 124, 470, 475, 571};
  const TArrayS blindpixels(numblind,(Short_t*)x);
  Float_t ringinterest = 100; //[mm]
  Float_t tailcut = 2.5;
  UInt_t integratedevents = 1;

  // We need the MAGIC mirror geometry from a MC header:
//  TString geometryfile = "../Mars/Gamma_zbin9_90_7_1480to1489_w0.root";
  
    TString geometryfile = "/.magic/magicserv01/MAGIC/mcdata/Period016/spot_1cm/standard/gamma/Gamma_zbin9_90_7_1740to1749_w0.root";

  // We need the Bright Star Catalog:

  // TString catalogfile = "mtemp/mmpi/macros/bsc5.dat";
  
  TString catalogfile = "bsc5.dat";
  
  MFindStars findstars;
  // findstars.SetBlindPixels(blindpixels);  ?? WHY?
  findstars.SetRingInterest(ringinterest);
  findstars.SetDCTailCut(tailcut);
  findstars.SetNumIntegratedEvents(integratedevents);
  findstars.SetMinuitPrintOutLevel(-1);
  findstars.SetGeometryFile(geometryfile);
  findstars.SetBSCFile(catalogfile);
  const Double_t ra  = MAstro::Hms2Rad(11, 4, 26);
  const Double_t dec = MAstro::Dms2Rad(38, 12, 36);
//  const Double_t ra  = MAstro::Hms2Rad(20, 0, 0);
//  const Double_t dec = MAstro::Dms2Rad(20, 48, 0);
  findstars.SetRaDec(ra,dec);
  findstars.SetLimMag(8);
  findstars.SetRadiusFOV(1.5);


  //$$$$$$$$$$$$$$$$ ww
/*
  MSourceDirections sdirs;
  sdirs.SetGeometryFile(geometryfile);
  const Double_t ra  = MAstro::Hms2Rad(11, 4, 26);
  const Double_t dec = MAstro::Dms2Rad(38, 12, 36);
  sdirs.SetRaDec(ra,dec);
  sdirs.AddDirection(ra,dec,1,"Mkn 421");
  const Double_t ra  = MAstro::Hms2Rad(11, 4, 31);
  const Double_t dec = MAstro::Dms2Rad(38, 14, 29);
  sdirs.AddDirection(ra,dec,1,"My_UMa 51");
  sdirs.SetRadiusFOV(3);

  Int_t InputType=1;
  MTelAxisFromStars telaxis;
  telaxis.SetInputType(InputType);
  MFillH fillhisto("MHTelAxisFromStars[MHTelAxisFromStars]","");
  htelaxis.SetInputType(InputType);
*/
  //$$$$$$$$$$$$$$$$ ww


  peddatacalc.SetPedestalUpdate(kTRUE);
  MArrivalTimeCalc2   timecalc;
 

  MCalibrateData      photcalc;     
  photcalc.SetCalibrationMode(MCalibrateData::kFfactor);  
  photcalc.EnablePedestalType(MCalibrateData::kEvent);
  photcalc.EnablePedestalType(MCalibrateData::kRun);


  MBadPixelsCalc  badcalc;
 
  MPointingPosCalc pntposcalc;
//  MPointingPosCalc mpntposcalc;  // not necessary, wolfgang calculates by himself
  
  // for the interpolation of the bad pixels comment the following lines in
  // done in the next step (Analysis.C)
  /*
  MBadPixelsTreat badtreat;
  badtreat.SetUseInterpolation();
  badtreat.SetProcessRMS();
  badtreat.SetNumMinNeighbors(int);
  */


  // set the names of the pedestal containers to be used:
//  extractor.SetNamePedContainer("MPedestalCamFromData");
  extractor.SetNamePedestalCam("MPedestalCamFromData");
  timeext.SetNamePedestalCam("MPedestalCamFromData");
//   photcalc.SetNamePedADCRunContainer("MPedestalCam");
//   photcalc.SetNamePedADCEventContainer("MPedestalCamFromData");
//   photcalc.SetNamePedPhotRunContainer("MPedPhotCam");
//   photcalc.SetNamePedPhotEventContainer("MPedPhotCamFromData");
  photcalc.SetNamePedADCContainer("MPedestalCamFromData");
  photcalc.SetNamePedPhotContainer("MPedPhotCamFromData");

  badcalc.SetNamePedPhotContainer("MPedPhotCamFromData");


  
  tlist3.AddToList(&read3);
  tlist3.AddToList(&geomapl);
//  tlist3.AddToList(&dccal,     "Currents");
//  tlist3.AddToList(&findstars, "Currents");
//  tlist3.AddToList(&sdirs);
//  tlist3.AddToList(&telaxis);
//  tlist3.AddToList(&mpntposcalc);
  tlist3.AddToList(&pntposcalc);
//  tlist3.AddToList(&fillhisto);  // changed by wolfgang
  
  tlist3.AddToList(&peddatacalc, "Events");
  tlist3.AddToList(&extractor,   "Events");
  tlist3.AddToList(&timeext,     "Events");
  tlist3.AddToList(&photcalc,    "Events");
  tlist3.AddToList(&badcalc,     "Events");
 

  MWriteRootFile write(resname);
  
  write.AddContainer("MGeomCam"              , "RunHeaders");
  write.AddContainer("MRawRunHeader"         , "RunHeaders");
//  write.AddContainer("MSrcPosCam"            , "RunHeaders");
  write.AddContainer("MCalibrationChargeCam" , "RunHeaders");
  write.AddContainer("MCalibrationQECam"     , "RunHeaders");
  write.AddContainer("MCalibrationRelTimeCam", "RunHeaders");

  write.AddContainer("MTime"         ,      "Events");
  write.AddContainer("MPedestalCam"  ,      "Events");
  write.AddContainer("MPedestalCamFromData","Events");
  write.AddContainer("MCerPhotEvt"   ,      "Events");
  write.AddContainer("MRawEvtHeader" ,      "Events");
  write.AddContainer("MBadPixelsCam" ,      "Events");
  write.AddContainer("MPedPhotCamFromData", "Events");
  write.AddContainer("MArrivalTimeCam",     "Events");
  
//  write.AddContainer("MStarCam",            "Events");
  write.AddContainer("MSrcPosCam",          "Events");

  write.AddContainer("MReportDrive",         "Events");  //new
//  write.AddContainer("MPntPosCam",           "Events"); 
  write.AddContainer("MPointingPos",         "Events"); 

  tlist3.AddToList(&write);
  
  // Create and execute eventloop
  MEvtLoop evtloop3;
  evtloop3.SetParList(&plist3);
    
  cout << "*************************************************************" << endl;
  cout << "***   COMPUTING DATA USING EXTRACTED SIGNAL (IN PHOTONS)  ***" << endl;
  cout << "*************************************************************" << endl;

  int maxevents = 3000;
  
  if (!evtloop3.Eventloop(maxevents))  
    return;  
  tlist3.PrintStatistics();

// changes by wolfgang
//  TObject *obj = plist3.FindObject("MHTelAxisFromStars");
//  obj->DrawClone();

}

void CamDraw(TCanvas &c, MHCamera &cam, MCamEvent &evt, Int_t i, Int_t j, Int_t fit)
{

  c.cd(i);
  gPad->SetBorderMode(0);
  MHCamera *obj1=(MHCamera*)cam.DrawCopy("hist");
  //  obj1->AddNotify(evt);
  
  c.cd(i+j);
  gPad->SetBorderMode(0);
  obj1->Draw();
  ((MHCamera*)obj1)->SetPrettyPalette();

  if (fit != 0)
    {
      c.cd(i+2*j);
      gPad->SetBorderMode(0);
      TH1D *obj2 = (TH1D*)obj1->Projection(obj1.GetName());
      
//      obj2->Sumw2();
      obj2->Draw();
      obj2->SetBit(kCanDelete);

      const Double_t min   = obj2->GetBinCenter(obj2->GetXaxis()->GetFirst());
      const Double_t max   = obj2->GetBinCenter(obj2->GetXaxis()->GetLast());
      const Double_t integ = obj2->Integral("width")/2.5066283;
      const Double_t mean  = obj2->GetMean();
      const Double_t rms   = obj2->GetRMS();
      const Double_t width = max-min;

      if (rms == 0. || width == 0. )
        return;
      
      switch (fit)
        {
        case 1:
          TF1 *sgaus = new TF1("sgaus","gaus(0)",min,max);
          sgaus->SetBit(kCanDelete);
          sgaus->SetParNames("Area","#mu","#sigma");
          sgaus->SetParameters(integ/rms,mean,rms);
          sgaus->SetParLimits(0,0.,integ);
          sgaus->SetParLimits(1,min,max);
          sgaus->SetParLimits(2,0,width/1.5);
          obj2->Fit("sgaus","QLR");
          obj2->GetFunction("sgaus")->SetLineColor(kYellow);
          break;

        case 2:
          TString dgausform = "([0]-[3])/[2]*exp(-0.5*(x-[1])*(x-[1])/[2]/[2])";
          dgausform += "+[3]/[5]*exp(-0.5*(x-[4])*(x-[4])/[5]/[5])";
          TF1 *dgaus = new TF1("dgaus",dgausform.Data(),min,max);
          dgaus->SetBit(kCanDelete);
          dgaus->SetParNames("A_{tot}","#mu_{1}","#sigma_{1}","A_{2}","#mu_{2}","#sigma_{2}");
          dgaus->SetParameters(integ,(min+mean)/2.,width/4.,
                               integ/width/2.,(max+mean)/2.,width/4.);
          // The left-sided Gauss 
          dgaus->SetParLimits(0,integ-1.5,integ+1.5);
          dgaus->SetParLimits(1,min+(width/10.),mean);
          dgaus->SetParLimits(2,0,width/2.);
          // The right-sided Gauss 
          dgaus->SetParLimits(3,0,integ);
          dgaus->SetParLimits(4,mean,max-(width/10.));
          dgaus->SetParLimits(5,0,width/2.);
          obj2->Fit("dgaus","QLRM");
          obj2->GetFunction("dgaus")->SetLineColor(kYellow);
          break;
          
        case 3:
          TString tgausform = "([0]-[3]-[6])/[2]*exp(-0.5*(x-[1])*(x-[1])/[2]/[2])";
          tgausform += "+[3]/[5]*exp(-0.5*(x-[4])*(x-[4])/[5]/[5])";
          tgausform += "+[6]/[8]*exp(-0.5*(x-[7])*(x-[7])/[8]/[8])";
          TF1 *tgaus = new TF1("tgaus",tgausform.Data(),min,max);
          tgaus->SetBit(kCanDelete);
          tgaus->SetParNames("A_{tot}","#mu_{1}","#sigma_{1}",
                             "A_{2}","#mu_{2}","#sigma_{2}",
                             "A_{3}","#mu_{3}","#sigma_{3}");
          tgaus->SetParameters(integ,(min+mean)/2,width/4.,
                               integ/width/3.,(max+mean)/2.,width/4.,
                               integ/width/3.,mean,width/2.);
          // The left-sided Gauss 
          tgaus->SetParLimits(0,integ-1.5,integ+1.5);
          tgaus->SetParLimits(1,min+(width/10.),mean);
          tgaus->SetParLimits(2,width/15.,width/2.);
          // The right-sided Gauss 
          tgaus->SetParLimits(3,0.,integ);
          tgaus->SetParLimits(4,mean,max-(width/10.));
          tgaus->SetParLimits(5,width/15.,width/2.);
          // The Gauss describing the outliers
          tgaus->SetParLimits(6,0.,integ);
          tgaus->SetParLimits(7,min,max);
          tgaus->SetParLimits(8,width/4.,width/1.5);
          obj2->Fit("tgaus","QLRM");
          obj2->GetFunction("tgaus")->SetLineColor(kYellow);
          break;
        case 4:
          obj2->Fit("pol0","Q");
          obj2->GetFunction("pol0")->SetLineColor(kYellow);
          break;
        case 9:
          break;
        default:
          obj2->Fit("gaus","Q");
          obj2->GetFunction("gaus")->SetLineColor(kYellow);
          break;
        }
      
        TArrayI s0(3);
        s0[0] = 6;
        s0[1] = 1;
        s0[2] = 2;

        TArrayI s1(3);
        s1[0] = 3;
        s1[1] = 4;
        s1[2] = 5;

        TArrayI inner(1);
        inner[0] = 0;

        TArrayI outer(1);
        outer[0] = 1;

        // Just to get the right (maximum) binning
        TH1D *half[4];
        half[0] = obj1->ProjectionS(s0, inner, "Sector 6-1-2 Inner");
        half[1] = obj1->ProjectionS(s1, inner, "Sector 3-4-5 Inner");
        half[2] = obj1->ProjectionS(s0, outer, "Sector 6-1-2 Outer");
        half[3] = obj1->ProjectionS(s1, outer, "Sector 3-4-5 Outer");

        for (int i=0; i<4; i++)      
        {
            half[i]->SetLineColor(kRed+i);
            half[i]->SetDirectory(0);
            half[i]->SetBit(kCanDelete);
            half[i]->Draw("same");
        }

      gPad->Modified();
      gPad->Update();
      
    }
}


