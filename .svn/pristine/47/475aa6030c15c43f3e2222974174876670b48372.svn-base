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
!   Author(s): Thomas Bretz, 11/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Rudy Bock, 11/2002 <mailto:rkb@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

void multidimdist2()
{
    //
    // This is an example program which reads image parameters for gammas 
    // and hadrons, builds a 'matrix' of look-alike events,
    // and then finds quality numbers and acceptances for the same sample

    // Create an empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;
    MTaskList tlistg;
    plist.AddToList(&tlistg);

    //
    // ------------- user attention -----------------
    //

    // cut used both for the matrices and the sample
    // for more information see the documentation of MF and MDataChain
    MF filterenergy("MMcEvt.fEnergy > 0");

    // matrix limitation for look-alike events (approximate number)
    MFEventSelector selector;
    selector.SetNumSelectEvts(2000);

    // setup an AND-Filterlist from the two filters to be used
    // in the event selection for the filling of the matrices
    MFilterList flist;
    flist.AddToList(&filterenergy);
    flist.AddToList(&selector);

    //
    // ---------------------------------------------------------------
    //  Now set up the tasks and tasklist (first event loop, gammas)
    // ---------------------------------------------------------------
    //

    // --------------- user change -----------------
    //  Give the names of the star-files to be read
    //   Here you give the trainings sample(s) for
    //                 the gammas
    //      If you have only _one_ single Theta
    //            remove MMcEvt.fTheta!
    // ---------------------------------------------
    MReadMarsFile readg("Events", "star_gammas0.root");
    readg.DisableAutoScheme();
    tlistg.AddToList(&readg);

    MHMatrix matrix("MatrixGammas");
    matrix.AddColumn("MHillas.fWidth");
    matrix.AddColumn("MHillas.fLength");
    matrix.AddColumn("MHillas.fWidth*MHillas.fLength/MHillas.fSize");
    matrix.AddColumn("abs(MHillas.fAsym)");
    matrix.AddColumn("abs(MHillas.fM3Long)");
    matrix.AddColumn("abs(MHillas.fM3Trans)");
    matrix.AddColumn("abs(MHillasSrc.fHeadTail)");
    matrix.AddColumn("MHillas.fConc");
    matrix.AddColumn("MHillas.fConc1");
    matrix.AddColumn("MHillasSrc.fDist");
    matrix.AddColumn("log10(MHillas.fSize)");
    matrix.AddColumn("MHillasSrc.fAlpha");
    matrix.AddColumn("MMcEvt.fTheta");
    plist.AddToList(&matrix);

    MFillH fillmatg("MatrixGammas");
    fillmatg.SetFilter(&flist);
    tlistg.AddToList(&flist);
    tlistg.AddToList(&fillmatg);

    //
    // --- Create and set up the eventloop (gammas) ---
    //
    MEvtLoop evtloop1;
    evtloop1.SetParList(&plist);

    //
    // --- Execute matrix buildup (gammas) ---
    //
    if (!evtloop1.Eventloop())
        return;

    tlistg.PrintStatistics();

    //
    // ------------------------------------------------------------------
    //                prepare second event loop, hadrons
    // ------------------------------------------------------------------
    //
    MTaskList tlisth;
    plist.Replace(&tlisth);

    // --------------- user change -----------------
    //  Give the names of the star-files to be read
    //   Here you give the trainings sample(s) for
    //                 the hadrons
    // ---------------------------------------------
    MReadMarsFile  readh("Events", "star_protons0.root");
    readh.DisableAutoScheme();
    tlisth.AddToList(&readh);

    MHMatrix matrixh("MatrixHadrons");
    matrixh.AddColumns(matrix.GetColumns());
    plist.AddToList(&matrixh); 

    MFillH fillmath("MatrixHadrons");
    fillmath.SetFilter(&flist);         // filter list
    tlisth.AddToList(&flist);
    tlisth.AddToList(&fillmath);

    //
    // Create and set up the eventloop (protons)
    //
    MEvtLoop evtloop2;
    evtloop2.SetParList(&plist);

    //
    // Execute matrix buildup (hadrons)
    //
    if (!evtloop2.Eventloop())
        return;

    //  sum up in log
    tlisth.PrintStatistics();

    matrix.Print("size");
    matrixh.Print("size");

    //
    // ----------------------------------------------------------
    //  Go through full sample again, now for getting hadronness
    //  (third event loop)
    // ----------------------------------------------------------
    //

    MTaskList tlist2;
    plist.Replace(&tlist2);

    // ------------------- user change --------------------
    //    Give the names of the star-files to be read as
    //  test samples   you should at least have one hadron
    //          and one gamma file available
    // ----------------------------------------------------
    MReadMarsFile read2("Events", "star_gammas0.root");
    read2.AddFile("star_protons0.root");
    read2.DisableAutoScheme();
    tlist2.AddToList(&read2);

    // ---------------- user attention -----------------
    //      Here you may change the algorithm used
    //    You can switch from Kernel to Next Neighbor
    //  or change the number of used shortest distances
    //  For kernel you should always set this to 0 (all)
    // -------------------------------------------------
    MMultiDimDistCalc calc;
    calc.SetUseNumRows(25);
    calc.SetUseKernelMethod(kFALSE);
    tlist2.AddToList(&calc);

    MFillH fillh("MHHadronness");
    fillh.SetFilter(&filterenergy);
    tlist2.AddToList(&filterenergy);
    tlist2.AddToList(&fillh);

    //
    // Execute analysis of gammas and hadrons
    //
    MProgressBar bar;
    MEvtLoop evtloop3;
    evtloop3.SetProgressBar(&bar);
    evtloop3.SetParList(&plist);

    if (!evtloop3.Eventloop())
        return;

    tlist2.PrintStatistics();

    plist.FindObject("MHHadronness")->DrawClone();
    plist.FindObject("MHHadronness")->Print();
}
