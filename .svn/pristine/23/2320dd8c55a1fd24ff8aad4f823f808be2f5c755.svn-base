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
!   Author(s): Javier López, 05/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

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

Double_t fitfunc(Double_t *x, Double_t *par);


void psffromstars(const TString filename="dc_*.root", const TString directory="/nfs/magic/CaCodata/online_data/Period015/cacadata/2004_03_21/", const UInt_t numEvents = 100000000)
{

  gStyle->SetOptFit(1);
  
  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;
  
  MTaskList tlist;
  plist.AddToList(&tlist);


  MGeomCamMagic geomcam;
  MCameraDC     dccam;
  MStarLocalCam starcam;
  MHPSFFromStars mhpsf;

  plist.AddToList(&geomcam);
  plist.AddToList(&dccam);
  plist.AddToList(&starcam);
  plist.AddToList(&mhpsf);

  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //

  // Reads the trees of the root file and the analysed branches
  MReadReports read;
  read.AddTree("Currents"); 
  read.AddFile(directory+filename);     // after the reading of the trees!!!
  read.AddToBranchList("MReportCurrents.*");

  MGeomApply geomapl;
  TString continuoslightfile = 
    //    "/home/Javi/mnt_magic_data/CaCo/rootdata/Miscellaneous/Period016/2004_04_16/dc_2004_04_16_04_46_18_22368_Off3c279-2CL100.root";
    "/nfs/magic/CaCodata/rootdata/Miscellaneous/Period016/2004_04_16/dc_2004_04_16_04_46_18_22368_Off3c279-2CL100.root";

  Float_t mindc = 0.7; //[uA]
  MCalibrateDC dccal;
  dccal.SetFileName(continuoslightfile);
  dccal.SetMinDCAllowed(mindc);

  const Int_t numblind = 1;
  const Short_t x[numblind] = { 124};
  const TArrayS blindpixels(numblind,(Short_t*)x);
  Float_t ringinterest = 100; //[mm]
  Float_t tailcut = 3.5;
  UInt_t integratedevents = 10;

  MFindStars findstars;
  findstars.SetBlindPixels(blindpixels);
  findstars.SetRingInterest(ringinterest);
  findstars.SetDCTailCut(tailcut);
  findstars.SetNumIntegratedEvents(integratedevents);
  findstars.SetMinuitPrintOutLevel(-1);

  MFillH fpsf("MHPSFFromStars","MStarLocalCam");
  
  tlist.AddToList(&geomapl);
  tlist.AddToList(&read);
  tlist.AddToList(&dccal);
  tlist.AddToList(&findstars, "Currents");
  tlist.AddToList(&fpsf, "Currents");

  //
  // Create and setup the eventloop
  //
  MEvtLoop evtloop;
  evtloop.SetParList(&plist);
  
//   MProgressBar bar;
//   evtloop.SetProgressBar(&bar);
  
  //
  // Execute your analysis
  //
  
  if (!evtloop.Eventloop(numEvents))
    return;

  tlist.PrintStatistics();

  //Draw results

  MStatusDisplay *d = new MStatusDisplay;
  d->SetTitle(Form("- %s -",filename));
  d->SetLogStream(&gLog, kTRUE);            // Disables output to stdout

  // Create a default canvas
  TCanvas &c1 = d.AddTab("Star spot Position");
  mhpsf.Draw("mean");
  TCanvas &c2 = d.AddTab("Star's Path");
  mhpsf.Draw("camera");
  TCanvas &c3 = d.AddTab("Star spot Size");
  mhpsf.Draw("sigma");
  TCanvas &c4 = d.AddTab("Star XY Projection");
  mhpsf.Draw("projection");
  
  d->Print("./psf.ps");
  
  //  if (!HandleInput()) {}

}

