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
!   Author(s): Thomas Bretz, 5/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Rudy Bock, 5/2002 <mailto:rkb@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

void multidimdist()
{
    //
    // This is a demonstration program which calculates the Hillas
    // parameter out of a Magic root file.

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

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
    //                 the hadrons
    // ---------------------------------------------
    MReadMarsFile read("Events");
    read.AddFile("star_gammas.root");
    read.AddFile("star_protons.root");
    read.DisableAutoScheme();
    tlistg.AddToList(&read);

    MFParticleId fgamma("MMcEvt", '=', kGAMMA);
    tlist.AddToList(&fgamma);

    MFParticleId fhadrons("MMcEvt", '!', kGAMMA);
    tlist.AddToList(&fhadrons);

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

    MHMatrix matrix2("MatrixHadrons");
    matrix2.AddColumns(matrix.GetColumns());
    plist.AddToList(&matrix2);

    MFillH fillmat("MatrixGammas");
    fillmat.SetFilter(&fgamma);
    tlist.AddToList(&fillmat);

    MFillH fillmat2("MatrixHadrons");
    fillmat2.SetFilter(&fhadrons);
    tlist.AddToList(&fillmat2);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    matrix.Print("size");
    matrix2.Print("size");

    // ---------------------------------------------------------

    MTaskList tlist2;

    plist.Replace(&tlist2);

    MReadMarsFile read2("Events");
    read2.("gammas2.root");
    read2.AddFile("hadrons2.root");
    read2.DisableAutoScheme();
    tlist2.AddToList(&read2);

    MMultiDimDistCalc calc;
    calc.SetUseNumRows(0);
    calc.SetUseKernelMethod(kTRUE);
    tlist2.AddToList(&calc);

    MFillH fillh("MHHadronness");

    /*
     MF filter("MMcEvt.fEnergy < 100");
     fillh.SetFilter(&filter);
     tlist2.AddToList(&filter);
     */

    tlist2.AddToList(&fillh);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist2.PrintStatistics();

    plist.FindObject("MHHadronness")->DrawClone();
    plist.FindObject("MHHadronness")->Print();
}
