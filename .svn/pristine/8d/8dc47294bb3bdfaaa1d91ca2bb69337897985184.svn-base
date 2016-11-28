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
!   Author(s): Thomas Bretz,  12/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// extendcam.C
//
// This macro demonstrates how to extend the magic camera and performes
// a fourier transformation on it
//
/////////////////////////////////////////////////////////////////////////////////

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

TCanvas   *c;
MHCamera *d1=new MHCamera(MGeomCamMagic());
MHCamera *d2=new MHCamera;
MHCamera *d3=new MHCamera;
MHCamera *d4=new MHCamera;
MHCamera *d5=new MHCamera;
MHCamera *d6=new MHCamera(MGeomCamMagicXT());

MSignalCam *evt=0;
MMcEvt *mc;

MHexagonFreqSpace *fFreq1;
MHexagonFreqSpace *fFreq2;

Int_t PreProcess(MParList *plist)
{
    fFreq1 = (MHexagonFreqSpace*)plist->FindObject("MHexagonFreqSpace1");
    if (!fFreq1)
        return kFALSE;
    fFreq2 = (MHexagonFreqSpace*)plist->FindObject("MHexagonFreqSpace2");
    if (!fFreq2)
        return kFALSE;

    //Für Real-Raum
    evt = (MSignalCam*)plist->FindObject("MSignalCam");
    if (!evt)
    {
        cout << "Fehler: MSignalCam" << endl;
        return kFALSE;
    }

    mc = (MMcEvt*)plist->FindObject("MMcEvt");
    if (!mc)
    {
        cout << "Fehler: MMcEvt" << endl;
        return kFALSE;
    }

    c = new TCanvas("Events", "Real Events", 900, 600);
    c->SetBorderMode(0);
    c->Divide(3,2);

    MGeomCam *geomfreq=fFreq1->NewGeomCam();
    geomfreq->SetName("FreqSpace");

    c->cd(1);
    d1->SetPrettyPalette();
    d1->Draw();
    c->cd(2);
    d2->SetGeometry(MGeomCamMagicXT());
    d2->SetPrettyPalette();
    d2->Draw();
    c->cd(4);
    d3->SetGeometry(*geomfreq);
    d3->SetPrettyPalette();
    d3->Draw();
    c->cd(5);
    d4->SetGeometry(MGeomCamMagicXT());
    d4->SetPrettyPalette();
    d4->Draw();
    c->cd(3);
    d5->SetGeometry(MGeomCamMagicXT());
    d5->SetPrettyPalette();
    d5->Draw();
    c->cd(6);
    //d6->SetGeometry(MGeomCamMagicXT());
    d6->SetPrettyPalette();
    d6->Draw();
    return kTRUE;
}

MClone *cl1;
MClone *cl2;
MClone *cl3;
MClone *cl4;

Int_t Process()
{
    d1->SetCamContent(*(MSignalCam*)cl1->GetClone());
    d2->SetCamContent(*(MSignalCam*)cl2->GetClone());
    d3->SetCamContent(*fFreq2);
    d4->SetCamContent(*(MSignalCam*)cl4->GetClone());
    d5->SetCamContent(*(MSignalCam*)cl3->GetClone());
    d6->SetCamContent(*evt);

    d2->SetMaximum(-1111);
    d4->SetMaximum(-1111);
    d2->SetMinimum(0);
    d4->SetMinimum(0);
    d2->SetMaximum(TMath::Max(d2->GetMaximum(), d4->GetMaximum()));
    d4->SetMaximum(d2->GetMaximum());

    for (int i=1; i<7; i++)
    {
        c->GetPad(i)->GetPad(1)->Modified();
        c->GetPad(i)->GetPad(1)->Update();
    }

    c->Modified();
    c->Update();


    return HandleInput();
}

void extendcam()
{
    MParList  plist;
    MTaskList tlist;

    MGeomCamMagicXT geom("MGeomCamMagicXT");
    plist.AddToList(&geom);

    plist.AddToList(&tlist);

    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    read.AddFile("/home/tbretz/Software/mcwobble/lza/cal/19*.root");
    //read.AddFile("/home/hoehne/data/mcdata/gammas/calib/calib_gamma_zbin0.root");
    //read.AddFile("/home/hoehne/data/mcdata/hadrons/calib/calib_proton_zbin0.root");


    // Setup intercative task calling the functions defined above
    MGeomApply           apply; //necessary for evt
    MGeomCamMagicEnhance enhance;
    // enhance.SetNameCerPhotEvtOut("MCerPhotEvt2");

    MClone clone1("MSignalCam", "Clone1");
    MClone clone2("MSignalCam", "Clone2");
    MClone clone3("MSignalCam", "Clone3");
    MClone clone4("MSignalCam", "Clone4");

    cl1 = &clone1;
    cl2 = &clone2;
    cl3 = &clone3;
    cl4 = &clone4;

    MHexagonalFTCalc     ftcalc;
    ftcalc.SetNameGeomCam("MGeomCamMagicXT");
    // ftcalc.SetNameCerPhotEvt("MCerPhotEvt2");

    MTaskInteractive mytask;
    mytask.SetPreProcess(PreProcess);
    mytask.SetProcess(Process);

    MImgCleanStd clean1(8, 4.5);
    clean1.SetMethod(MImgCleanStd::kAbsolute);
    clean1.SetNameGeomCam("MGeomCamMagicXT");
    clean1.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    MImgCleanStd clean2(8, 4.5);
    clean2.SetMethod(MImgCleanStd::kAbsolute);
    clean2.SetNameGeomCam("MGeomCamMagicXT");
    clean2.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    // Setup your tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&clone1);
    tlist.AddToList(&enhance);
    tlist.AddToList(&clone2);
    tlist.AddToList(&clean1);
    tlist.AddToList(&clone3);
    tlist.AddToList(&ftcalc);
    tlist.AddToList(&clone4);
    tlist.AddToList(&clean2);
    tlist.AddToList(&mytask);

    // Run your analysis
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop())
        return;

    // Print statistics information about your loop
    tlist.PrintStatistics();
}

