//   triglvl2.C
//  Macro to use the class MMcTriggerLvl2, which calculates the 
//   2nd level trigger (L2T) selection parameters.
//  Filters to select events using these parameter and 
//  histograms with selection variables distributions are also created.
//
//   Inputs:
//      - filename      name of data file
//      - CompactNN     number of NN to define a compact pixel
//      - fValue        comparision value for the filter
//
//  23/04/2003 Added example of MFEnergySlope filter
//
//  
void triglvl2(char *filename = "Gamma.root")
  //    USER:   Data File Name    ---^
{ 
    //
    // first we have to create our empty lists
    //
    MParList  parlist;
    MTaskList tasklist;

    parlist.AddToList(&tasklist);

    // 
    // Setup our tasks:
    //  - First we have to read the events
    //  - Then we can fill the efficiency histograms
    //
    

    MReadMarsFile reader("Events", filename);
    reader.DisableAutoScheme();
     //  reader.EnableBranch("fEnergy");
     //  reader.EnableBranch("fImpact"); reader.EnableBranch("fTimeFirst[4]");
     //  reader.EnableBranch("fPixelsFirst[73][4]");
   
    tasklist.AddToList(&reader);
    MGeomCamMagic geocam;
    parlist.AddToList(&geocam);

    //    MHillas hillas;
    // parlist.AddToList(&hillas);

    // An instance of the class MMcTriggerLvl2 is created and added to the 
    //  parameter list
    MMcTriggerLvl2 cell;     
    parlist.AddToList(&cell);

    MMcEvt mevt;
    parlist.AddToList(&mevt);

    // Set the number of Next Neighbourhoods that define a compact pixel
    //
    cell.SetCompactNN(3);
    //  USER:       --^

    //
    //  A filter to select events using the L2T parameters is created
    //
    // MF lvl2filter("MMcTriggerLvl2.fPseudoSize > 25 && MMcTriggerLvl2.fPseudoSize < 31");
    //    MF lvl2filter("MMcTriggerLvl2.fSizeBiggerCell > 34");
    MF lvl2filter("MMcTriggerLvl2.fSizeBiggerCell > 18");
 
    //
    // A second filter is created using the class MFTriggerLvl2
    //
    MFTriggerLvl2 fTrig("MMcTriggerLvl2", '>', 8);
    //      USER:       fValue to be compared --^

    //
    // A selection on the number and energy of the events 
    //
    MF energyfilter("MMcEvt.fEnergy > 100");
    MFEventSelector selector;
    //selector.SetNumSelectEvts(4000);


    // Filter to select events according to a give slope
    MFEnergySlope eslope;

    eslope.SetMcMinEnergy(50.);
    eslope.SetMcMaxEnergy(400.);
    eslope.SetNewSlope(-.5);

    // A filter list is created; the filters created can be added to the list
    //
    MFilterList flist;
    //flist.AddToList(&energyfilter);
    flist.AddToList(&lvl2filter);
    //   flist.AddToList(&selector);
    //   flist.AddToList(&eslope);
    //   flist.AddToList(&fTrig);

    //
    // The task to calculate the L2T parameter is added to the task list
    //
    MMcTriggerLvl2Calc calcps("MMcTriggerLvl2","MMcTriggerLvl2");  
    tasklist.AddToList(&calcps);

    //
    // The filter list is added to the task list
    //
    tasklist.AddToList(&flist);


    MContinue L1offline ("MMcTriggerLvl2.fLutPseudoSize < 6");
    //    tasklist.AddToList(&L1offline);

    //
    //  Task to calculate and plot the effective area
    //
    MMcCollectionAreaCalc effi;
    tasklist.AddToList(&effi);    
    //
    // The filter list selects events for the effective area calculation
    //
    effi.SetFilter(&flist);
 

    //
    // Filling of histos for MHMcTriggerLvl2
    //
    MFillH hfill1("MHMcTriggerLvl2","MMcTriggerLvl2");
    tasklist.AddToList(&hfill1);
    //hfill1.SetFilter(&flist);
    //MFillH hfill2("MHMcTriggerLvl2", &mevt, &cell);
    //tasklist.AddToList(&hfill2);
    //hfill2.SetFilter(&flist);

    
    
    //
    // set up the loop for the processing
    //
    MEvtLoop magic;
    magic.SetParList(&parlist);

    
    //
    // Start to loop over all events
    //
    MProgressBar bar;
    magic.SetProgressBar(&bar);


    if (!magic.Eventloop())
      return;
    /*
    if (!magic.PreProcess())
    return;
    
    while (tasklist.Process())
      {
	cout<< mevt.GetEnergy()<<endl;
	
	cell.Print();
      }
    */
    //fMcEvt = (MMcEvt*)parlist->FindObject("MMcEvt");
    //if (!fMcEvt)
    //{
    //cout << "MMcEvt not found... exit." << endl;
    //*fLog << err << dbginf << "MMcEvt not found... exit." << endl;
    //  return kFALSE;
    //}
    //   cout << "fMcEvt = " << fMcEvt << endl;

    //parlist.FindObject("MHMcTriggerLvl2")->Fill((Double_t) fMcEvt->GetEnergy(), cell);


    tasklist.PrintStatistics();
 
    //
    // Now the histogram we wanted to get out of the data is
    // filled and can be displayd
    //
    

    //parlist.FindObject("MHMcTriggerLvl2")->DrawClone("sbc");
    //parlist.FindObject("MHMcTriggerLvl2")->DrawClone("lps");
    //parlist.FindObject("MHMcTriggerLvl2")->DrawClone();
    //parlist.FindObject("MHMcTriggerLvl2")->DrawClone("energy");
    //parlist.FindObject("MHMcTriggerLvl2")->DrawClone("lut-energy");
    //parlist.FindObject("MHMcCollectionArea")->DrawClone();
    // Returns histogram of the class MHMcTriggerLvl2  
    
    MHMcTriggerLvl2 *htrig = (MHMcTriggerLvl2 *)parlist.FindObject("MHMcTriggerLvl2");
    MHMcCollectionArea *collarea = (MHMcCollectionArea *)parlist.FindObject("MHMcCollectionArea");
    TH1F *hps = (TH1F *)htrig->GetHistByName("fHistPseudoSize");
    TH1F *hlps = (TH1F *)htrig->GetHistByName("fHistLutPseudoSize");
    TH1F *hsbc = (TH1F *)htrig->GetHistSizeBiggerCell();
    TH1F *hpsn = (TH1F *)htrig->GetHistByName("fHistPseudoSizeNorm");
    TH1F *hlpsn = (TH1F *)htrig->GetHistByName("fHistLutPseudoSizeNorm");
    TH1F *hsbcn = (TH1F *)htrig->GetHistSizeBiggerCellNorm();
    TH2D *hlpse = (TH2D *)htrig->GetHistByName("fHistLutPseudoSizeEnergy");
    TH2D *hpse = (TH2D *)htrig->GetHistByName("fHistPseudoSizeEnergy");
    TH2D *hsbce = (TH2D *)htrig->GetHistSizeBiggerCellEnergy();
    TH1D *hcollarea = collarea->GetHist();

    // Please set rootfile as the directory where you want to store
    // your file.root containing histograms
    rootfile = new TString(filename);
    if (rootfile->Index("/*.root",1)>=0){
      rootfile->Resize(rootfile->Index("/*.root",1));
      rootfile[0]+="_sbcmag18_CNN3.root";
    }
      
    hfile = new TFile((char *)rootfile[0], "RECREATE");
    hlps->Write();
    hps->Write();
    hsbc->Write();
    hlpsn->Write();
    hpsn->Write();
    hsbcn->Write();
    hlpse->Write();
    hpse->Write();
    hsbce->Write();
    hcollarea->Write();
    hfile->Close();
    cout << "Histograms stored into " << rootfile[0] << endl;
    

}



