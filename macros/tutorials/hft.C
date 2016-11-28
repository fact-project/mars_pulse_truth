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
!   Author(s): Thomas Bretz  12/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2005
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// hft.C
//
// example of using MHexagonal*
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
MHCamera display[5];

TCanvas   *c;
MParList  *fParList;
MTaskList *fTaskList;

MHexagonalFTCalc  hft;
MHexagonFreqSpace *fFreq1;
MHexagonFreqSpace *fFreq2;

MGeomCam *geomfreq;

TH1F histo("", "", 35, -0.5, 35);

//
// Called like all PreProcess functions of tasks. Get the access to
// the containers necessary for you.
//
Int_t PreProcess(MParList *plist)
{
    fFreq1 = (MHexagonFreqSpace*)plist->FindObject("MHexagonFreqSpace1");
    if (!fFreq1)
        return kFALSE;
    fFreq2 = (MHexagonFreqSpace*)plist->FindObject("MHexagonFreqSpace2");
    if (!fFreq2)
        return kFALSE;

    geomfreq=fFreq1->NewGeomCam();
    geomfreq->SetName("FreqSpace");
    plist->AddToList(geomfreq);

    //    return kTRUE;

    // Get parameter and tasklist, see Process
    fParList = plist;
    fTaskList = (MTaskList*)plist->FindObject("MTaskList");

    // Get camera geoemtry
    MGeomCam *geomcam = (MGeomCam*)plist->FindObject("MGeomCam");

    // setup canvas and camera-histograms
    c = new TCanvas("Events", "Real Events", 900, 600);
    c->SetBorderMode(0);
    c->Divide(3,2);
    for (int i=0; i<3; i++)
    {
        display[i].SetGeometry(*geomfreq);
        display[i].SetPrettyPalette();
        c->cd(i+1);
        display[i].Draw();
    }

    display[1].SetName("In");
    display[2].SetName("Out");
    display[0].SetName("Freq");

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
    MClone *clone1 = (MClone*)fTaskList->FindObject("Clone1");
    MClone *clone2 = (MClone*)fTaskList->FindObject("Clone2");
    MGeomCam *geom = (MGeomCam*)fParList->FindObject("MGeomCam");

    // Fill the data into your camera-histograms
    display[1].SetCamContent(*fFreq1, 1);
    display[2].SetCamContent(*fFreq1, 2);
    display[0].SetCamContent(*fFreq2, 0);

    display[1].SetMaximum(-1111);
    display[2].SetMaximum(-1111);
    display[1].SetMinimum(0);
    display[2].SetMinimum(0);
    display[1].SetMaximum(TMath::Max(display[1].GetMaximum(),
                                     display[2].GetMaximum()));
    display[2].SetMaximum(display[1].GetMaximum());

    MHexagonalFT *ft = &hft.GetHFT();

    histo.Reset();
    for (int i=0; i<ft->GetNumKnots(); i++)
    {
        Double_t val=0;
        fFreq2->GetPixelContent(val, i, *display[1].GetGeometry());
        histo.Fill(ft->GetRow(i), val/(i+1));
    }

    c->cd(4);
    histo.Draw();

    c->cd(5);

    TH1D *obj1 = (TH1D*)display[1].Projection("Proj1");
    obj1->SetLineColor(kBlue);
    obj1->Draw();
    obj1 = (TH1D*)display[2].Projection("Proj2");
    obj1->Draw("same");

    c->cd(6);
    display[0].DrawProjection();

    // Update the display
    for (int i=1; i<=3; i++)
    {
        c->GetPad(i)->GetPad(1)->Modified();
        c->GetPad(i)->GetPad(1)->Update();
    }

    return HandleInput();
}

//
// Called like all PostProcess functions of tasks. Delete
// instanciated objects.
//
Int_t PostProcess()
{
    delete c;
    delete geomfreq;
}

void hft(const char *fname="/home/tbretz/Software/mcwobble/lza/cal/19*.root")
{
    // Setup parameter- and tasklist
    MParList  plist;
    MTaskList tlist;

    plist.AddToList(&tlist);

    // setup reading task
    MReadMarsFile read("Events", fname);
    read.DisableAutoScheme();

    // Clone MCerPhotEvt befor eimage cleaning
    MClone clone1("MSignalCam", "Clone1");

    // Setup image cleaning
    MImgCleanStd clean(8.5, 4);
    clean.SetMethod(MImgCleanStd::kAbsolute);
    clean.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    // Clone MCerPhotEvt befor eimage cleaning
    MClone clone2("MSignalCam", "Clone2");

    // Setup intercative task calling the functions defined above
    MTaskInteractive  mytask;

    mytask.SetPreProcess(PreProcess);
    mytask.SetProcess(Process);

    // Setup your tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&clone1);
    tlist.AddToList(&hft);
    tlist.AddToList(&clone2);
    tlist.AddToList(&clean);
    tlist.AddToList(&mytask);

    // Run your analysis
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop())
        return;

    // Print statistics information about your loop
    tlist.PrintStatistics();
}

