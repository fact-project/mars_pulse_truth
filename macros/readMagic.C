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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// readMagic.C
// ===========
//
// This is a demonstration program showing how to do particular processing
// on a single event basis. Here we simply display uncleand und cleand
// images.
// Therefor MInteractiveTask is used, which gives you the possibility
// to develop new tasks without the need of compilation.
// The input is a merpp raw-data file.
//
///////////////////////////////////////////////////////////////////////////

Bool_t HandleInput()
{
    // This must be there to get accesss to the GUI while the macro
    // is still running!

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

//
// Setup the data-members of your 'virtual' class
//
MHCamera display[4];

TCanvas   *c;
MParList  *fParList;
MTaskList *fTaskList;

//
// Called like all PreProcess functions of tasks. Get the access to
// the containers necessary for you.
//
Int_t PreProcess(MParList *plist)
{
    // Get parameter and tasklist, see Process
    fParList = plist;
    fTaskList = (MTaskList*)plist->FindObject("MTaskList");

    // Get camera geoemtry
    MGeomCam *geomcam = (MGeomCam*)plist->FindObject("MGeomCam");

    // setup canvas and camera-histograms
    c = new TCanvas("Events", "Real Events", 600, 600);
    c->SetBorderMode(0);
    c->Divide(2,2);
    for (int i=0; i<4; i++)
    {
        display[i].SetGeometry(*geomcam);
        c->cd(i+1);
        display[i].Draw();
        gPad->cd(1);
        plist->FindObject("MHillas")->Draw();
    }
    return kTRUE;
}

//
// Called like all Process functions of tasks. Process a single
// event - here display it.
//
Int_t Process()
{
    // For simplicity we search in the Process function for the
    // objects. This is deprectaed! Store the pointers to the objects
    // as data member and get the pointers in PreProcess.
    MReadMarsFile *read = (MReadMarsFile*)fTaskList->FindObject("MRead");
    MClone *clone = (MClone*)fTaskList->FindObject("MClone");
    MImgCleanStd *clean = (MImgCleanStd*)fTaskList->FindObject("MImgCleanStd");
    MGeomCam *geom = (MGeomCam*)fParList->FindObject("MGeomCam");

    // Ouput event number
    cout << "Event #" << read->GetNumEntry() << ":" << endl;

    // Fill the data into your camera-histograms
    display[0].SetCamContent(*(MCerPhotEvt*)clone->GetClone());
    display[1].SetCamContent(*(MCerPhotEvt*)fParList->FindObject("MCerPhotEvt"));
    display[2].SetCamContent(*(MCameraData*)fParList->FindObject("MCameraData"));
    display[3].SetCamContent(*(MCameraData*)fParList->FindObject("MCameraData"));

    // Setup the cleaning level histogram
    TArrayF lvl(2);
    lvl[0] = clean->GetCleanLvl2();
    lvl[1] = clean->GetCleanLvl1();
    display[3].SetLevels(lvl);

    // Update the display
    for (int i=1; i<=4; i++)
    {
        c->GetPad(i)->GetPad(1)->Modified();
        c->GetPad(i)->GetPad(1)->Update();
    }

    // print the data on the console
    ((MHillas*)fParList->FindObject("MHillas"))->Print(*geom);
    ((MHillasExt*)fParList->FindObject("MHillasExt"))->Print(*geom);
    ((MNewImagePar*)fParList->FindObject("MNewImagePar"))->Print(*geom);

    // wait for 'return'
    return HandleInput();
}

//
// Called like all PostProcess functions of tasks. Delete
// instanciated objects.
//
Int_t PostProcess()
{
    delete c;
}

void readMagic(const char *fname="../Proton*.root")
{
    // Setup parameter- and tasklist
    MParList  plist;
    MTaskList tlist;

    plist.AddToList(&tlist);

    // setup reading task
    MReadMarsFile read("Events", fname);
    read.DisableAutoScheme();

    // setup a task making sure that all arrays are resized correctly
    MGeomApply geomapl;

    // Setup a print task calling TObject::Print
    MPrint print1("MMcEvt");
    MPrint print2("MRawEvtHeader");
    // Skip them if conatainers are not found
    print1.EnableSkip();
    print2.EnableSkip();

    // Copy MC pedestals to apropriate conatiners (if MC file)
    MMcPedestalCopy   pcopy;
    MMcPedestalNSBAdd pnsb;

    // Calculate signal and pedestal from data (use MCerPhotCalc
    // in case of MC files)
    MCerPhotAnal2     ncalc;

    // Blind Pixel Treatment (deprecated, will be replaced by
    // MBadPixel*)
    MBlindPixelCalc   blind;
    blind.SetUseInterpolation();

    // Clone MCerPhotEvt befor eimage cleaning
    MClone            clone("MCerPhotEvt");

    // Setup image cleaning
    MImgCleanStd      clean;

    // Setup calculation of Image parameters
    MHillasCalc       hcalc;

    // Setup intercative task calling the functions defined above
    MTaskInteractive  mytask;

    mytask.SetPreProcess(PreProcess);
    mytask.SetProcess(Process);

    // Setup your tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&print1);
    tlist.AddToList(&print2);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pnsb);
    tlist.AddToList(&ncalc);
    tlist.AddToList(&blind);
    tlist.AddToList(&clone);
    tlist.AddToList(&clean);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&mytask);

    // Run your analysis
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop())
        return;

    // Print statistics information about your loop
    tlist.PrintStatistics();
}

