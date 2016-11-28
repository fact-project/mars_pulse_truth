
void InitBinnings(MParList *plist)
{
        gLog << "InitBinnings" << endl;

        //--------------------------------------------
        MBinning *binse = new MBinning("BinningE");
        //binse->SetEdgesLog(30, 1.0e2, 1.0e5);

	//This is Daniel's binning in energy:
        binse->SetEdgesLog(14, 296.296, 86497.6);
        plist->AddToList(binse);

        //--------------------------------------------

        MBinning *binssize = new MBinning("BinningSize");
        binssize->SetEdgesLog(50, 10, 1.0e5);
        plist->AddToList(binssize);

        MBinning *binsdistc = new MBinning("BinningDist");
        binsdistc->SetEdges(50, 0, 1.4);
        plist->AddToList(binsdistc);

        MBinning *binswidth = new MBinning("BinningWidth");
        binswidth->SetEdges(50, 0, 1.0);
        plist->AddToList(binswidth);

        MBinning *binslength = new MBinning("BinningLength");
        binslength->SetEdges(50, 0, 1.0);
        plist->AddToList(binslength);

        MBinning *binsalpha = new MBinning("BinningAlpha");
        binsalpha->SetEdges(100, -100, 100);
        plist->AddToList(binsalpha);

        MBinning *binsasym = new MBinning("BinningAsym");
        binsasym->SetEdges(50, -1.5, 1.5);
        plist->AddToList(binsasym);

        MBinning *binsm3l = new MBinning("BinningM3Long");
        binsm3l->SetEdges(50, -1.5, 1.5);
        plist->AddToList(binsm3l);

        MBinning *binsm3t = new MBinning("BinningM3Trans");
        binsm3t->SetEdges(50, -1.5, 1.5);
        plist->AddToList(binsm3t);

   
        //.....
        MBinning *binsb = new MBinning("BinningSigmabar");
        binsb->SetEdges( 100,  0.0,  50.0);
        plist->AddToList(binsb);

        MBinning *binssig = new MBinning("BinningSigma");
        binssig->SetEdges( 100,  0.0, 120.0);
        plist->AddToList(binssig);

        MBinning *binth = new MBinning("BinningTheta");
        // this is Daniel's binning in theta
        //Double_t yedge[8] = 
        //  {9.41, 16.22, 22.68, 28.64, 34.03, 38.84, 43.08, 44.99};
        // this is our binning
        //Double_t yedge[9] = 
        //               {0.0, 17.5, 23.5, 29.5, 35.5, 42., 50., 60., 70.};
        //TArrayD yed;
        //yed.Set(9,yedge);
        //binth->SetEdges(yed);
        
        //binth->SetEdges(     1, 0.0, 90.0);
        binth->SetEdgesCos( 10, 0.0, 90.0);
        plist->AddToList(binth);

        //MBinning *bincosth = new MBinning("BinningCosTheta");
        //Double_t yedge[9] = 
        //               {0.0, 17.5, 23.5, 29.5, 35.5, 42., 50., 60., 70.};
        //Double_t zedge[9]; 
        //for (Int_t i=0; i<9; i++)
	//{
	//zedge[8-i] = cos(yedge[i]/kRad2Deg);
	//}
        //TArrayD zed;
        //zed.Set(9,zedge);
        //bincosth->SetEdges(zed);
        //plist->AddToList(bincosth);

        MBinning *binsdiff = new MBinning("BinningDiffsigma2");
        binsdiff->SetEdges(100, -500.0, 1500.0);
        plist->AddToList(binsdiff);

        // robert ----------------------------------------------
        MBinning *binsalphaf = new MBinning("BinningAlphaFlux");
        binsalphaf->SetEdges(100, -100, 100);
        plist->AddToList(binsalphaf);

	MBinning *binsdifftime = new MBinning("BinningTimeDiff");
	binsdifftime->SetEdges(50, 0, 10);
	plist->AddToList(binsdifftime);

	MBinning *binstime = new MBinning("BinningTime");
	binstime->SetEdges(50, 44500, 61000);
	plist->AddToList(binstime);
}


void DeleteBinnings(MParList *plist)
{
        gLog << "DeleteBinnings" << endl;

        TObject *bin;

        //--------------------------------------------
        bin = plist->FindObject("BinningE");
        if (bin) delete bin;

        //--------------------------------------------

        bin = plist->FindObject("BinningSize");
        if (bin) delete bin;

        bin = plist->FindObject("BinningDist");
        if (bin) delete bin;

        bin = plist->FindObject("BinningWidth");
        if (bin) delete bin;

        bin = plist->FindObject("BinningLength");
        if (bin) delete bin;

        bin = plist->FindObject("BinningAlpha");
        if (bin) delete bin;

        bin = plist->FindObject("BinningAsym");
        if (bin) delete bin;

        bin = plist->FindObject("BinningM3Long");
        if (bin) delete bin;

        bin = plist->FindObject("BinningM3Trans");
        if (bin) delete bin;

        //.....
        bin = plist->FindObject("BinningSigmabar");
        if (bin) delete bin;

        bin = plist->FindObject("BinningSigma");
        if (bin) delete bin;

        bin = plist->FindObject("BinningTheta");
        if (bin) delete bin;

        bin = plist->FindObject("BinningCosTheta");
        if (bin) delete bin;

        bin = plist->FindObject("BinningDiffsigma2");
        if (bin) delete bin;


        // robert ----------------------------------------------
        bin = plist->FindObject("BinningAlphaFlux");
        if (bin) delete bin;

        bin = plist->FindObject("BinningTimeDiff");
        if (bin) delete bin;

        bin = plist->FindObject("BinningTime");
        if (bin) delete bin;
}



//************************************************************************
void PaddingExample()
{
      gLog.SetNoColors();

      if (gRandom)
        delete gRandom;
      gRandom = new TRandom3(0);

      //-----------------------------------------------
      TString tag = "080000";

      const char *offfile  = "*OffCrab11*";

      const char *onfile  = "*CrabNebula*";

      const char *mcfile = "calibrated_MCdata2";

      //-----------------------------------------------

      // path for input for Mars

     if (tag == "080000")  
     { 
       TString inPathON  = "/.magic/magicserv01/MAGIC/calibrateddata/2004_09_21/";
       TString inPathOFF = "/.magic/magicserv01/MAGIC/calibrateddata/2004_09_21/";
       TString inPathMC  = "/.magic/data21a/mase/Mars/Mars041103/DataCalibUV/";

     }

      // path for output from Mars
     TString outPath = "/.magic/data21a/wittek/PaddedData/";
//     outPath += tag;
//     outPath += "/";

      //-----------------------------------------------


    //************************************************************************

    // Job A : 
    //  - produce MHSigmaTheta plots for ON, OFF and MC data
    //  - write out (or read in) these MHSigmaTheta plots
    //  - read ON (or OFF or MC) data
    //  - pad the events; 
    //  - write root file for ON (or OFF or MC) data (ON1.root, ...);

    Bool_t JobA    = kTRUE;  
    Bool_t GPadON  = kTRUE;    // \  generate Pad histograms 
    Bool_t GPadOFF = kFALSE;    //  | and write them onto disk
    Bool_t GPadMC  = kFALSE;    // /
    Bool_t Merge   = kFALSE;   // read the Pad histograms, merge them
                               // and write them onto disk
    Bool_t Wout    = kFALSE;   // \  read in merged padding histograms and
                               //  | write out root file of padded data
                               // /  (ON1.root, OFF1.root or MC1.root) 
    TString typeInput("ON");
    //TString typeInput("OFF");
    //TString typeInput("MC");




    //************************************************************************

    
  //---------------------------------------------------------------------
  // Job A
  //=========

    //  - produce the histograms "sigmabar versus Theta", etc. 
    //    for ON, OFF and MC data (to be used for the padding)
    //
    //  - write root file of padded ON (OFF, MC) events (ON1.root, ...) 
    //    (after the standard cuts, before the g/h separation)


 if (JobA)
 {
    gLog << "=====================================================" << endl;
    gLog << "Macro ONOFFAnalysis : Start of Job A" << endl;
    gLog << "" << endl;
    gLog << "Macro ONOFFAnalysis : JobA, GPadON, GPadOFF, GPadMC, Merge, Wout = " 
         << (JobA    ? "kTRUE" : "kFALSE")  << ",  " 
         << (GPadON  ? "kTRUE" : "kFALSE")  << ",  " 
         << (GPadOFF ? "kTRUE" : "kFALSE")  << ",  " 
         << (GPadMC  ? "kTRUE" : "kFALSE")  << ",  " 
         << (Merge   ? "kTRUE" : "kFALSE")  << ",  " 
         << (Wout    ? "kTRUE" : "kFALSE")  << endl;
    
    //--------------------------------------------------


    // name of MPedPhotCam container
    TString fNamePedPhotCam("MPedPhotCam");


    //************************************************************
    // generate histograms to be used in the padding
    // 
    // read ON, OFF and MC data files
    // generate (or read in) the padding histograms for ON, OFF and MC data
    //

    MPad pad;
    pad.SetName("MPad");

    //--------------------------------------------------
    // names of ON and OFF files to be read
    // for generating the histograms to be used in the padding 
    TString fileON  = inPathON;
    fileON += onfile;
    fileON += ".root";

    TString fileOFF = inPathOFF;
    fileOFF += offfile;
    fileOFF += ".root";

    TString fileMC = inPathMC;
    fileMC += mcfile;
    fileMC += ".root";

    //--------------------------------------------------
    // name of files to contain the paddding histograms of ON, OFF and MC data
      TString NamePadON(outPath);
      NamePadON += "PadON";
      NamePadON += ".root";

      TString NamePadOFF(outPath);
      NamePadOFF += "PadOFF";
      NamePadOFF += ".root";

      TString NamePadMC(outPath);
      NamePadMC += "PadMC";
      NamePadMC += ".root";

    // name of file to conatin the merged histograms for the padding
    TString outNameSigTh = outPath;
    outNameSigTh += "SigmaTheta";
    outNameSigTh += ".root";

    //--------------------------------------------------

    if (GPadON || GPadOFF || GPadMC)
    {
      // generate the padding histograms
      gLog << "=====================================================" << endl;
      gLog << "Start generating the padding histograms" << endl;


    gLog << "fileON, fileOFF, fileMC = " << fileON << ",  " 
         << fileOFF << ",  " << fileMC   << endl;



    //--------------------------------------------------
      MMakePadHistograms makepad;
      makepad.SetMaxEvents(-1);
      makepad.SetNamePedPhotCam(fNamePedPhotCam);
      makepad.SetPedestalLevel(2.0);
      makepad.SetUseInterpolation(kTRUE);
      makepad.SetProcessPedestal(kTRUE);
      makepad.SetProcessTime(kFALSE);

      //-----------------------------------------
      // ON events

      if (GPadON)
      {
        makepad.SetDataType("ON");
        makepad.SetNameInputFile(fileON);
        makepad.SetNameOutputFile(NamePadON);
        makepad.MakeHistograms();
      }

      //-----------------------------------------
      // OFF events

      if (GPadOFF)
      {
        makepad.SetDataType("OFF");
        makepad.SetNameInputFile(fileOFF);
        makepad.SetNameOutputFile(NamePadOFF);
        makepad.MakeHistograms();
      }

      //-----------------------------------------
      // MC events

      if (GPadMC)
      {
        makepad.SetDataType("MC");
        makepad.SetNameInputFile(fileMC);
        makepad.SetNameOutputFile(NamePadMC);
        makepad.MakeHistograms();
      }

      //-----------------------------------------


      gLog << "" << endl;
      gLog << "End of generating the padding histograms" << endl;
      gLog << "=====================================================" << endl;
    }

    //************************************************************

    if (Merge)
    {
      gLog << "=====================================================" << endl;
      gLog << "Start of merging the padding histograms" << endl;
      gLog << "" << endl;

      //pad.MergeONOFFMC(NamePadON, NamePadOFF, NamePadMC, outNameSigTh);
      //pad.MergeONOFFMC(NamePadON, "", NamePadMC, outNameSigTh);
      pad.MergeONOFFMC(NamePadON, NamePadOFF, "", outNameSigTh);
      //pad.MergeONOFFMC("", NamePadOFF, NamePadMC, outNameSigTh);

      gLog << "" << endl;
      gLog << "End of merging the padding histograms" << endl;
      gLog << "=====================================================" << endl;
    }
    // end of Merge



    //************************************************************

  if (Wout)
  {
    // read the target padding histograms ---------------------------
    pad.ReadPaddingDist(outNameSigTh);


    gLog << "=====================================================" << endl;
    gLog << "Start the padding" << endl;

    //--------------------------------------------------
    // type of data to be padded 
    gLog << "typeInput = " << typeInput << endl;

    //-------------------------------------------
    // name of input root file
    if (typeInput == "ON")
      TString filenamein(fileON);
    else if (typeInput == "OFF")
      TString filenamein(fileOFF);
    else if (typeInput == "MC")
      TString filenamein(fileMC);
    gLog << "data to be padded : " << filenamein << endl;

    // name of output root file
    TString outNameImage = outPath;
    outNameImage += "Hillas";
    outNameImage += typeInput;
    outNameImage += "1.root";
    gLog << "padded data to be written onto : " << outNameImage << endl;

    //-----------------------------------------------------------
    pad.SetDataType(typeInput);
    pad.SetNamePedPhotCam(fNamePedPhotCam);

    MTaskList tliston;
    MParList pliston;

    // geometry is needed in  MHHillas... classes 
    MGeomCam *fGeom = 
             (MGeomCam*)pliston->FindCreateObj("MGeomCamMagic", "MGeomCam");

    //-------------------------------------------
    // create the tasks which should be executed 
    //

    MReadReports read;
    read.AddTree("Drive");
    read.AddTree("Events","MTime.",kTRUE);
    static_cast<MRead&>(read).AddFile(filenamein);

    read.AddToBranchList("MReportDrive.*");
    read.AddToBranchList("MRawEvtHeader.*");


//    MReadMarsFile read("Events", filenamein);
//    read.DisableAutoScheme();

    MGeomApply        apply;

    // a way to find out whether one is dealing with MC :
    //MFDataMember f1("MRawRunHeader.fRunType", '>', 255.5);  // MC
    //f1.SetName("Select MC");
    //MFDataMember f2("MRawRunHeader.fRunType", '<', 255.5);  // data
    //f2.SetName("Select Data");



      MBadPixelsCalc badcalc;
      badcalc.SetNamePedPhotContainer(fNamePedPhotCam);
      badcalc.SetPedestalLevel(2.0);
      badcalc.SetName("BadCalc");

      MBadPixelsTreat badtreat;
      badtreat.SetNamePedPhotCam(fNamePedPhotCam);
      badtreat.SetUseInterpolation(kTRUE);
      badtreat.SetProcessPedestal(kTRUE);
      badtreat.SetProcessTimes(kFALSE);
      badtreat.SetName("BadTreat");

      MPointingPosCalc mpointcalc;

      MFSelBasic selbasic;
      selbasic.SetCuts(20.0, 0.0, 25.8419, 90.0, 126.0);

      MContinue contbasic(&selbasic);
      contbasic.SetName("SelBasic");

      MHBadPixels bad("BadPixels");
      bad.SetNamePedPhotCam(fNamePedPhotCam);
      MFillH fillbad("BadPixels[MHBadPixels]", "MBadPixelsCam");
      fillbad.SetName("FillBad");

      MHSigmaTheta sigth("SigmaTheta");
      sigth.SetNamePedPhotCam(fNamePedPhotCam);
      MFillH fillsigtheta ("SigmaTheta[MHSigmaTheta]", "");
      fillsigtheta.SetName("FillSigTheta");    


    MImgCleanStd    clean(3.0, 2.5);
    clean.SetMethod(MImgCleanStd::kScaled);
    clean.SetCleanRings(1); 
    clean.SetName("Clean");
    clean.SetNamePedPhotCam(fNamePedPhotCam);

    // calculation of  image parameters ---------------------
    TString fHilName    = "MHillas";
    TString fHilNameExt = "MHillasExt";
    TString fHilNameSrc = "MHillasSrc";
    TString fImgParName = "MNewImagePar";

    MHillasCalc    hcalc;
    hcalc.SetNameHillas(fHilName);
    hcalc.SetNameHillasExt(fHilNameExt);
    hcalc.SetNameNewImagePar(fImgParName);

    MHillasSrcCalc hsrccalc("MSrcPosCam", fHilNameSrc);
    hsrccalc.SetInput(fHilName);

    MFillH hfill1("MHHillas",    fHilName);
    hfill1.SetName("HHillas");

    MFillH hfill2("MHStarMap",   fHilName);
    hfill2.SetName("HStarMap");

    MFillH hfill3("MHHillasExt",   fHilNameSrc);
    hfill3.SetName("HHillasExt");

    MFillH hfill4("MHHillasSrc",   fHilNameSrc);
    hfill4.SetName("HHillasSrc");

    MFillH hfill5("MHNewImagePar", fImgParName);
    hfill5.SetName("HNewImagePar");
    // --------------------------------------------------

    MFSelStandard selstandard(fHilNameSrc);
    selstandard.SetHillasName(fHilName);
    selstandard.SetImgParName(fImgParName);
    selstandard.SetCuts(500, 2, 20, 0.0, 5.0, 0.001, 0.001);
    MContinue contstandard(&selstandard);
    contstandard.SetName("SelStandard");

      
    /*
      MWriteRootFile write(outNameImage);

      write.AddContainer("MRawRunHeader", "RunHeaders");
      //write.AddContainer("MMcRunHeader",  "RunHeaders", kFALSE);
      //write.AddContainer("MTime",         "Events");
      write.AddContainer("MPointingPos", "Events");
      write.AddContainer("MSrcPosCam",    "Events");
      write.AddContainer("MHillas",       "Events");
      write.AddContainer("MHillasExt",    "Events");
      write.AddContainer("MHillasSrc",    "Events");
      write.AddContainer("MNewImagePar",  "Events");
      write.AddContainer("MTime",         "Events");
      write.AddContainer("MImagePar",     "Events");

      write.AddContainer("MReportDrive",  "Drive", kFALSE);
      write.AddContainer("MTimeDrive",    "Drive", kFALSE);  //new
    */

    //*****************************
    // entries in MParList
    
    pliston.AddToList(&tliston);
    InitBinnings(&pliston);
    pliston.AddToList(&bad);
    pliston.AddToList(&sigth);

    //*****************************
    // entries in MTaskList
    
    tliston.AddToList(&read);
    //tliston.AddToList(&f1);
    //tliston.AddToList(&f2);
    tliston.AddToList(&apply);
    tliston.AddToList(&mpointcalc, "Events");

//    tliston.AddToList(&badcalc); done in callisto
//    tliston.AddToList(&badtreat); done in callisto

    tliston.AddToList(&contbasic, "Events");
    //tliston.AddToList(&pad, "Events"); 

    
    tliston.AddToList(&fillbad, "Events");
    tliston.AddToList(&fillsigtheta, "Events");
    tliston.AddToList(&clean, "Events");

    tliston.AddToList(&hcalc, "Events");
    tliston.AddToList(&hsrccalc, "Events");

    tliston.AddToList(&hfill1, "Events");
    tliston.AddToList(&hfill2, "Events");
    tliston.AddToList(&hfill3, "Events");
    tliston.AddToList(&hfill4, "Events");
    tliston.AddToList(&hfill5, "Events");

    tliston.AddToList(&contstandard, "Events");
    //tliston.AddToList(&write);
    

    //*****************************

    //-------------------------------------------
    // Execute event loop
    //
    MProgressBar bar;
    MEvtLoop evtloop;
    evtloop.SetParList(&pliston);
    //evtloop.ReadEnv(env, "", printEnv);
    evtloop.SetProgressBar(&bar);
    //  evtloop.Write();

    Int_t maxevents = -1;
    //Int_t maxevents = 2000;
    if ( !evtloop.Eventloop(maxevents) )
        return;

    tliston.PrintStatistics(0, kTRUE);


    //-------------------------------------------
    // Display the histograms

    pliston.FindObject("SigmaTheta", "MHSigmaTheta")->DrawClone();
    pliston.FindObject("BadPixels",  "MHBadPixels")->DrawClone();

    pliston.FindObject("MHHillas")->DrawClone();
    pliston.FindObject("MHHillasExt")->DrawClone();
    pliston.FindObject("MHHillasSrc")->DrawClone();
    pliston.FindObject("MHNewImagePar")->DrawClone();
    pliston.FindObject("MHStarMap")->DrawClone();

    //DeleteBinnings(&pliston);

      gLog << "End of padding" << endl;
      gLog << "=====================================================" << endl;
  }  


    gLog << "Macro ONOFFAnalysis : End of Job A" << endl;
    gLog << "===================================================" << endl;
 }

}







   





















