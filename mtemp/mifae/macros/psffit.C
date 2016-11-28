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
!   Author(s): Javier López, 04/2004 <mailto:jlopez@ifae.es>
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


void psffit(const TString filename="dc_2004_03_19_00_36_50_20781_Mrk421.root", const TString directory="/nfs/magic/CaCodata/rootdata/Mrk421/Period015/2004_03_19/", const UInt_t numEvents = 0)
{

  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;
  
  MTaskList tlist;
  plist.AddToList(&tlist);


  MGeomCamMagic geomcam;
  MCameraDC     dccam;
  MPSFFit       psffit;

  plist.AddToList(&geomcam);
  plist.AddToList(&dccam);
  plist.AddToList(&psffit);

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

  const Int_t numrings = 3;
  const Int_t numblind = 23;
  const Short_t x[numblind] = {  8,  27, 224, 279, 339,
			       507, 508, 509, 510, 511, 512, 513, 514,
			       543,
			       559, 560, 561, 562, 563, 564, 565, 566, 567};
  const TArrayS blindpixels(numblind,(Short_t*)x);
  MPSFFitCalc psfcalc;
  //psfcalc.SetImgCleanMode(MPSFFitCalc::kRing);
  psfcalc.SetImgCleanMode(MPSFFitCalc::kCombined);
  psfcalc.SetNumRings(numrings);
  psfcalc.SetBlindPixels(blindpixels);


  tlist.AddToList(&geomapl);
  tlist.AddToList(&read);
  tlist.AddToList(&psfcalc, "Currents");

  //
  // Create and setup the eventloop
  //
  MEvtLoop evtloop;
  evtloop.SetParList(&plist);
     
  //
  // Execute your analysis
  //

  if (numEvents > 0)
  {
      if (!evtloop.Eventloop(numEvents))
	  return;
  }
  else
  {
      if (!evtloop.PreProcess())
	  return;
      
      MHCamera display(geomcam);
      display.SetPrettyPalette();
      display.Draw();
      gPad->cd(1);
      psffit.Draw();
      
      while (tlist.Process())
      {
	  display.SetCamContent(dccam);
	  gPad->Modified();
	  gPad->Update();
          psffit.Print();
	  // Remove the comments if you want to go through the file
	  // event-by-event:
	  if (!HandleInput())
	      break;
      } 

      evtloop.PostProcess();
  }

  tlist.PrintStatistics();

  psffit.Print();
  cout << "RUN " << psffit.GetMeanMinorAxis() << ' ' << psffit.GetSigmaMinorAxis() << ' ' <<  psffit.GetMeanMajorAxis()  << ' ' <<  psffit.GetSigmaMajorAxis() << ' ' << psffit.GetChisquare() << endl;
  
}


