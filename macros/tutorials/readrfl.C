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

Bool_t JumpTo(MTaskList &tlist, MReadRflFile &read, int runno, int evtno)
{
    if (runno<0 || evtno<0)
        return tlist.Process();

    return read.SearchFor(runno, evtno);
}

void readrfl(int runno=-1, int evtno=-1, const char *fname="~msmeyer/MC/MagicSoft/Simulation/Detector/ReflectorII/resultsQi/000302.rfl")
{
    MParList plist;

    MGeomCamMagic geomcam;
    MRflEvtData   event;
    MRflEvtHeader evthead;
    MRflRunHeader runhead;
    MTaskList     tlist;

    plist.AddToList(&geomcam);
    plist.AddToList(&event);
    plist.AddToList(&evthead);
    plist.AddToList(&runhead);
    plist.AddToList(&tlist);

    MReadRflFile read(fname);
    tlist.AddToList(&read);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.PreProcess())
        return;

    MHCamera display(geomcam);
    display.Draw();
    gPad->cd(1);
    event.Draw();



    cout << "Runno: " << runno << "  Eventno: " << evtno << endl;

    runhead.Print();

    while (JumpTo(tlist, read, runno, evtno))
    {
        runno = -1;

        cout << "Run #" << runhead.GetRunNumber() << "   ";
        cout << "Event #" << evthead.GetEvtNumber() << endl;

        evthead.Print();

	display.SetCamContent(event);

        if (display.GetMean()<1e-5)
            continue;

	gPad->Modified();
	gPad->Update();
	
        if (!HandleInput())
            break;
    } 

    evtloop.PostProcess();
}
