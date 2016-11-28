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
!   Author(s): Abelardo Moralejo <mailto:moralejo@pd.infn.it>
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// pixfirerate.C
// =============
//
// This macro can help to  find "hot" pixels firing too often
// or pixels firing too rarely. It plots camera displays showing how many 
// times the FADC integral of each pixel has been found to be above pedestal
// by 10, 20, 50, 100 or 200 ADC counts. As "pedestal"  we now use simply 
// the signal in the first ADC slice, which seemed reasonable from a first
// look at the available test data.
//
/////////////////////////////////////////////////////////////////////////////

void pixfirerate(TString filename="rawfile.root")
{
    //
    // Update frequency by default = 1Hz
    //
    MStatusDisplay *d = new MStatusDisplay;

    // Set update time to 5s
    // d->SetUpdateTime(5000);

    // Disable online update
    // d->SetUpdateTime(-1);

    d->SetLogStream(&gLog, kTRUE); // Disables output to stdout
    gLog.SetOutputFile("status.log", kTRUE); // Enable output to file
    //gLog.EnableOutputDevice(MLog::eStdout); // Enable output to stdout again

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MTaskList tlist;
    MParList  plist;
    plist.AddToList(&tlist);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    read.AddFile(filename);
    tlist.AddToList(&read);

    MGeomApply geomapl;
    tlist.AddToList(&geomapl);

    // A list of threshold which should be displayed. The last entry
    // MUST be -1.
    Double_t threshold[] = { 10, 20, 100, 200, -1 };

    Int_t cnt = 0;
    while (threshold[cnt]>0) cnt++;

    // Create the corresponding fill tasks and containers
    for (int i=0; i<cnt; i++)
    {
        TString name = "Above ";
        TString num;
        num += threshold[i];
        name += num.Strip(TString::kBoth);
        TString title = "Firerate [%] of pixels with signal > ";
        title += num.Strip(TString::kBoth);

        MHTriggerLvl0 *trigmap = new MHTriggerLvl0(threshold[i], name, title);
        MFillH *fillh = new MFillH(trigmap, "MRawEvtData");
        trigmap->SetBit(kCanDelete);
        fillh->SetBit(kCanDelete);
        plist.AddToList(trigmap);
        tlist.AddToList(fillh);
    }

    // create the eventloop
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    //
    // Make sure the display hasn't been deleted by the user while the
    // eventloop was running.
    //
    if ((d = evtloop.GetDisplay()))
    {
        // Save data in a postscriptfile (status.ps)
        d->SaveAsPS();
        /*
         * ----------- Write status to a root file ------------
         *
         TFile file("status.root", "RECREATE");
         d->Write();
         file.Close();
         delete d;
         */
    }

}
