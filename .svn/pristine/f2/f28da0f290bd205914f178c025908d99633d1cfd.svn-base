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
!   Author(s): Thomas Bretz, 4/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  Status - Example how to use the MStatusDisplay
//
//  Use a camera file as input
//
/////////////////////////////////////////////////////////////////////////////

void status()
{
    //
    // Update frequency by default = 1Hz
    //
    MStatusDisplay *d = new MStatusDisplay;

    // Set update time to 5s
    // d->SetUpdateTime(5000);

    // Disable online update
    // d->SetUpdateTime(-1);

    d->SetLogStream(&gLog, kTRUE);            // Disables output to stdout
    gLog.SetOutputFile("status.log", kTRUE);  // Enable output to file
    //gLog.EnableOutputDevice(MLog::eStdout); // Enable output to stdout again

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    // ------------- user change -----------------
    MDirIter files(".", "G*.root", -1);
    //files.Print("all");

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    read.AddFiles(files);

    MGeomApply        geomapl;

    MMcPedestalCopy   pcopy;
    MMcPedestalNSBAdd pnsb;
    MCerPhotCalc      ncalc;

    TArrayS blinds(0);

    MBlindPixelCalc blind;
    blind.SetPixelIndices(blinds);
    blind.SetUseInterpolation();

    MSigmabarCalc     sgcal;
    MImgCleanStd      clean;
    MHillasCalc       hcalc;
    MHillasSrcCalc    scalc; // !!Preliminary!! Will be removed later!

    MHCamEvent hist("PedestalRms");
    hist.SetType(1);
    plist.AddToList(&hist);

    // -------------------------------------------
    MHTriggerLvl0 trighi(254, "SaturationHi", "Saturation Rate of Hi Gains");
    trighi.SetType(1);

    MHCamEvent maxhi("MaxIdxHi", "Index of maximum hi-gain slice");
    MHCamEvent maxlo("MaxIdxLo", "Index of maximum lo-gain slice");
    maxhi.SetType(3);
    maxlo.SetType(4);
    plist.AddToList(&maxhi);
    plist.AddToList(&maxlo);


    MFillH fillhi(&trighi, "MRawEvtData");
    MFillH hfilhi("MaxIdxHi", "MRawEvtData");
    MFillH hfillo("MaxIdxLo", "MRawEvtData");
    MFillH hfill0("Uncleaned [MHCamEvent]", "MCerPhotEvt");
    MFillH hfill1("Pedestals [MHCamEvent]", "MPedestalCam");
    MFillH hfill2("PedestalRms", "MPedestalCam");
    MFillH hfill3("MHHillas", "MHillas");
    MFillH hfill4("MHHillasExt");
    MFillH hfill5("MHHillasExtSrc [MHHillasExt]", "MHillasSrc");
    MFillH hfill6("MHHillasSrc","MHillasSrc");
    MFillH hfill7("MHNewImagePar","MNewImagePar");
    MFillH hfill8("MHStarMap", "MHillas");
    MFillH hfill9("Cleaned [MHCamEvent]", "MCerPhotEvt");

    MContinue cont1("MNewImagePar.fNumCorePixels<0");

    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&hfilhi);
    tlist.AddToList(&hfillo);
    tlist.AddToList(&fillhi);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pnsb);
    tlist.AddToList(&ncalc);
    tlist.AddToList(&blind);
    tlist.AddToList(&hfill0);
    //tlist.AddToList(&sgcal);
    tlist.AddToList(&clean);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&cont1);
    tlist.AddToList(&scalc);
    tlist.AddToList(&hfill1);
    tlist.AddToList(&hfill2);
    tlist.AddToList(&hfill3);
    tlist.AddToList(&hfill4);
    tlist.AddToList(&hfill5);
    tlist.AddToList(&hfill6);
    tlist.AddToList(&hfill7);
    tlist.AddToList(&hfill8);
    tlist.AddToList(&hfill9);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    MHCamEvent *uncl = (MHCamEvent*)plist.FindObject("Uncleaned");
    MHCamEvent *hped = (MHCamEvent*)plist.FindObject("Pedestals");
    MHCamEvent *hrms = (MHCamEvent*)plist.FindObject("PedestalRms");
    uncl->PrintOutliers(3);
    hped->PrintOutliers(1.5);
    hrms->PrintOutliers(4);
    trighi.PrintOutliers(2.5);

    //
    // Make sure the display hasn't been deleted by the user while the
    // eventloop was running.
    //
    if ((d = evtloop.GetDisplay()))
    {
        // Save data in a postscriptfile (status.ps)
        //d->SaveAsPS();
        /*
         * ----------- Write status to a root file ------------
         *
         TFile file("status.root", "RECREATE");
         d->Write();
         file.Close();
         delete d;
         */
    }
    /*
     * ----------- Read status from a root file ------------
     *
     TFile file2("status.root", "READ");
     MStatusDisplay *d2 = new MStatusDisplay;
     d2->Read();
     */
}
