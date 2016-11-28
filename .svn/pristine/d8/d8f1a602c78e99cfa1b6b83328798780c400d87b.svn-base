void ReadSetup(TString fname, MAstroCamera &cam)
{
    MMcConfigRunHeader *config=0;
    MGeomCam           *geom=0;

    TFile file(fname);
    TTree *tree = (TTree*)file.Get("RunHeaders");
    tree->SetBranchAddress("MMcConfigRunHeader", &config);
    if (tree->GetBranch("MGeomCam"))
        tree->SetBranchAddress("MGeomCam", &geom);
    tree->GetEntry(0);

    cam.SetMirrors(*config->GetMirrors());
    cam.SetGeom(*geom);
}

void findstars(const TString filename="20040422_23213_D_Mrk421_E.root", const TString directory="/data/MAGIC/Period016/rootdata2/2004_04_22/", const UInt_t numEvents = 0)
{

  MParList  plist;
  MTaskList tlist;
  plist.AddToList(&tlist);

  MGeomCamMagic geomcam;
  MCameraDC     dccam;
  MStarLocalCam starcam;

  plist.AddToList(&geomcam);
  plist.AddToList(&dccam);
  plist.AddToList(&starcam);

  // Reads the trees of the root file and the analysed branches
  MReadReports read;
  read.AddTree("Currents"); 
  read.AddTree("Drive"); // If you do not include Drive info, the MFindStars class
                         // will not use the star catalog method
  read.AddFile(directory+filename); // after the reading of the trees!!!
  read.AddToBranchList("MReportCurrents.*");
  read.AddToBranchList("MReportDrive.*");

  MGeomApply geomapl;
  TString continuoslightfile =   
     "/data/MAGIC/Period016/rootdata/2004_04_16/20040416_22368_P_Off3c279-2CL100_E.root";
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
  TString geometryfile = "/mcdata/standard/camera/NSB_013/Gamma/Gamma_zbin9_90_7_1480to1489_w0.root";

  // We need the Bright Star Catalog:
  TString catalogfile = "/home/rwagner/bsc5.dat";

  MFindStars findstars;
  findstars.SetBlindPixels(blindpixels);
  findstars.SetRingInterest(ringinterest);
  findstars.SetDCTailCut(tailcut);
  findstars.SetNumIntegratedEvents(integratedevents);
  findstars.SetMinuitPrintOutLevel(-1);
  findstars.SetGeometryFile(geometryfile);
  findstars.SetBSCFile(catalogfile);
  const Double_t ra  = MAstro::Hms2Rad(11, 4, 26);
  const Double_t dec = MAstro::Dms2Rad(38, 12, 36);
  findstars.SetRaDec(ra,dec);
  findstars.SetLimMag(8);
  findstars.SetRadiusFOV(1.5);

  tlist.AddToList(&geomapl);
  tlist.AddToList(&read);
  tlist.AddToList(&dccal);
  tlist.AddToList(&findstars, "Currents");
  
  // The following lines you only need if in addition you want to display
  // independent MAstroCamera output
  //
  //  TString fname = "/mcdata/standard/camera/NSB_013/Gamma/Gamma_zbin9_90_7_1480to1489_w0.root";
  //  MObservatory magic1;      
  //  const Double_t ra  = MAstro::Hms2Rad(11, 4, 26); //Mkn421
  //  const Double_t dec = MAstro::Dms2Rad(38, 12, 36);
  //
  //  MAstroCamera stars;        
  //  ReadSetup(fname, stars); 
  //  stars.SetLimMag(9);
  //  stars.SetRadiusFOV(3);
  //  stars.SetRaDec(ra, dec);
  //  stars.ReadBSC("/home/rwagner/bsc5.dat");
  //  stars.SetObservatory(magic1);

  MEvtLoop evtloop;
  evtloop.SetParList(&plist);
     
  if (!evtloop.PreProcess())
    return;
  
  MHCamera display(geomcam);
  display.SetPrettyPalette();
  display.Draw();
  gPad->cd(1);
  starcam.Draw();
  
  UInt_t numevents=0;
  
  while (tlist.Process())
    {
      numevents++;
      if (numevents%integratedevents==0)
	{
	  display.SetCamContent(findstars.GetDisplay());
	  gPad->Modified();
	  gPad->Update();	      
        // This line prints the results:
	// 	  starcam.Print();
	// This is how to access the TList of stars:
	// 	  TList* starlist = starcam.GetList();

	// This is how to iterate over stars found:
	// 	  TIter Next(starlist);
	// 	  MStarLocalPos* star;
	// 	  UInt_t starnum = 0;
	// 	  cout << filename << " ";
	// 	  cout << "Iterating over list" << endl;
	// 	  while ((star=(MStarLocalPos*)Next())) 
	// 	    {
	// 	      cout << "star[" << starnum << "] ";
	// 	      cout << star->GetMeanX() << " " 
	// 		   << star->GetMeanY() << " ";
	// 	      starnum++;
	// 	    }
	// 	  cout << endl;
	
	}//integratedevents

      MTime time;
      time.Set(2004, 4, 22, 21, 51, 15);
      
      //superimpose star picture
      //       stars.SetTime(time);
      //       TObject *o = stars.Clone();
      //       o->SetBit(kCanDelete);
      //       o->Draw();
      
      if (!HandleInput())
        break;

    }

  evtloop.PostProcess();
    
  tlist.PrintStatistics();

}


Bool_t HandleInput()
{
    TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);

    while (1)
    {
        //
        // While reading the input process gui events asynchronously
        //
        timer.TurnOn();
        TString input = Getline("Type 'q' to exit, <return> to go on: ");
        timer.TurnOff();

        if (input=="q\n")
            return kFALSE;

        if (input=="\n")
            return kTRUE;
    };

    return kFALSE;
}
