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
!   Author(s): Thomas Bretz, 5/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
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

void readcurrents(const char *fname="../currents/dcs_arcturus.dat")
{
    MParList plist;

    MGeomCamMagic geomcam;
    MCameraDC     cur;
    MTaskList     tlist;

    plist.AddToList(&geomcam);
    plist.AddToList(&cur);
    plist.AddToList(&tlist);

    MReportFileRead read(fname);
    read.SetHasNoHeader();
    read.AddToList("MReportCurrents");

    tlist.AddToList(&read);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.PreProcess())
        return;

    MHCamera display(geomcam);
    display.Draw();
    gPad->SetLogy();

    int gifcnt = 0;

    while (tlist.Process())
    {
        // cur.Print();
        display.SetCamContent(cur);
        gPad->GetPad(1)->Modified();
        gPad->GetPad(1)->Update();
        // Remove the comments if you want to go through the file
        // event-by-event:
        if (!HandleInput())
            break;
    } 

    evtloop.PostProcess();
}
