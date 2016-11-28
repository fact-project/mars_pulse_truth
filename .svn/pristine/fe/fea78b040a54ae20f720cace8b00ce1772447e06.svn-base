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
!   Author(s): Thomas Hengstebeck, 3/2003 <mailto:hengsteb@alwa02.physik.uni-siegen.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

void RanForest2()
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);
    //
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // first event loop: the trees of the random forest are read in
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    //
    MReadTree read("Tree","RF.root");
    read.DisableAutoScheme();

    MRanForestFill rffill;
    rffill.SetNumTrees(100);

    tlist.AddToList(&read);
    tlist.AddToList(&rffill);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute tree reading (now the eventloop is actually a treeloop)
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // second event loop: the control sample is processed
    // through the previously grown random forest,
    //
    // the histograms MHHadronness (quality of g/h-separation) and
    // MHRanForest are created and displayed.
    // MHRanForest shows the convergence of the classification error
    // as function of the number of grown (and combined) trees
    // and tells the user how many trees are actually needed in later
    // classification tasks.
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------

    MTaskList tlist2;

    plist.Replace(&tlist2);

    MReadMarsFile  read2("Events", "~/MagicData/data1/CT1Data/ONTest/*.root");
    read2.DisableAutoScheme();
    tlist2.AddToList(&read2);

    MRanForestCalc calc;
    tlist2.AddToList(&calc);

    MFillH fillh2a("MHHadronness");
    MFillH fillh2b("MHRanForest");

    tlist2.AddToList(&fillh2a);
    tlist2.AddToList(&fillh2b);

    //
    // Execute your analysis
    //
    MProgressBar bar;
    evtloop.SetProgressBar(&bar);

    if (!evtloop.Eventloop())
        return;

    tlist2.PrintStatistics();

    plist.FindObject("MHRanForest")->DrawClone();
    plist.FindObject("MHHadronness")->DrawClone();
    plist.FindObject("MHHadronness")->Print();
}
