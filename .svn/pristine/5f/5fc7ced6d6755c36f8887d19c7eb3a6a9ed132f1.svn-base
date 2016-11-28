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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Abelardo Moralejo, 2/2005 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

// 
// Example macro on how to calculate effective areas. The input is a file 
// containing Hillas parameters of the MC events surviving a certain kind 
// of analysis. It must also contain a tree called "OriginalMC" with a branch
// "MMcEvtBasic" and one entry per original simulated shower used to produce
// the events in that input file. The current (2/2005) camera simulation in 
// the cvs already writes out all events with the MMcEvtBasic container to
// the Events tree. In Mars/macros/mccalibrate.C and starmc2.C you can see
// how this branch is put to a separate tree "OriginalMC" and written out
// to the "star" file. This will not work with star files not containing this
// extra tree. Please contact moralejo@pd.infn.it in case of questions
// about this new approach to the calculation of effective areas.
//

void collarea(TString filename="star_gamma_test.root", TString outname="area.root")
{
  MStatusDisplay *d = new MStatusDisplay;
    // redirect logging output to GUI, kFALSE to disable stream to stdout
  d->SetLogStream(&gLog, kTRUE);

  //
  // First loop: fill the MHMcCollectionArea::fHistAll histogram containing
  // all the events produced at the Corsika level:
  //
  MParList  parlist;
  MTaskList tasklist;

  parlist.AddToList(&tasklist);

  //
  // Setup out tasks:
  //  - First we have to read the events
  //
  MReadMarsFile reader("OriginalMC", filename);
  reader.DisableAutoScheme();

  //
  // Create collection area object and add it to the task list
  //
  MHMcCollectionArea collarea;
  parlist.AddToList(&collarea);

  //
  // Set the COARSE binnings in which you will divide your data, to get
  // the collection area in these same binnings. You can set also this
  // binnings on an existing and filled MHMcCollectionAreaObject, and
  // call Calc again to get the effective area in other coarse bins.
  //
  MBinning binsTheta("binsTheta");
  MBinning binsEnergy("binsEnergy"); // name must be standardized!
  Int_t nbins = 8;
  TArrayD edges(nbins+1);
  edges[0] = 0;
  edges[1] = 10.;
  edges[2] = 20.;
  edges[3] = 30.;
  edges[4] = 40.;
  edges[5] = 50.;
  edges[6] = 60.;
  edges[7] = 70.;
  edges[8] = 80.;
  binsTheta.SetEdges(edges);              // Theta [deg]
  binsEnergy.SetEdgesLog(20, 2., 20000);  // Energy [GeV]
  parlist.AddToList(&binsTheta);
  parlist.AddToList(&binsEnergy);

  // Task which fills the necessary histograms in MHMcCollectionArea
  MMcCollectionAreaCalc effi;

  // Tentative energy spectrum. This may slightly modify the calculation
  // of effective areas. If not added to the parameter list, a flat spectrum
  // in dN/dE will be assumed. "x" stands for the energy. The name of the 
  // function must be standardized!

  TF1 Spectrum("Spectrum", "pow(x,-5.)");
  effi.SetSpectrum(&Spectrum);

  tasklist.AddToList(&reader);
  tasklist.AddToList(&effi);

  //
  // set up the loop for the processing
  //
  MEvtLoop magic;
  magic.SetParList(&parlist);

  //
  // Start to loop over all events
  //
  magic.SetDisplay(d);

  if (!magic.Eventloop())
    return;

  tasklist.PrintStatistics();


  //
  // Second loop: now fill the histogram MHMcCollectionArea::fHistSel
  //

  MParList parlist2;
  MTaskList tasklist2;
  parlist2.AddToList(&tasklist2);
  parlist2.AddToList((MHMcCollectionArea*)parlist->FindObject("MHMcCollectionArea"));

  MReadMarsFile reader2("Events", filename);
  reader2.DisableAutoScheme();

  tasklist2.AddToList(&reader2);

  // The same task as before, now will fill MHMcCollectionArea::fHistSel
  // and call MHMcCollectionArea::Calc in the PostProcess
  tasklist2.AddToList(&effi);

  //
  // set up the loop for the processing
  //
  MEvtLoop magic2;
  magic2.SetParList(&parlist2);

  //
  // Start to loop over all events
  //
  magic2.SetDisplay(d);

  if (!magic2.Eventloop())
    return;

  tasklist2.PrintStatistics();

  //
  // Now the histogram we wanted to get out of the data is
  // filled and can be displayed
  //
  if ((d = magic2.GetDisplay()))
    d->AddTab("Collection Area");
  else
    new TCanvas("CollArea", "Result of the collection area calculation");

  gStyle->SetPalette(1,0);
  gPad->SetLogx();
  TH2D* areaplot = ((MHMcCollectionArea*)parlist2.FindObject
		    ("MHMcCollectionArea"))->GetHistCoarse();

  areaplot->SetStats(0);
  areaplot->SetTitle("Effective area (m^{2})");
  areaplot->GetXaxis()->SetTitle("Energy (GeV)");
  areaplot->GetYaxis()->SetTitle("\\theta (deg)");
  areaplot->DrawCopy("zcol");

  //
  // Write histogram to a file in case an output
  // filename has been supplied
  //

  if (outname.IsNull())
    return;

  TFile f(outname, "recreate");
  if (!f)
    return;

  // Write to the output file:
  parlist2.FindObject("MHMcCollectionArea")->Write();
}
