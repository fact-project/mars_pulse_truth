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
!   Author(s): Thomas Hengstebeck 3/2003 <mailto:hengsteb@alwa02.physik.uni-siegen.de>!
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

void RanForest()
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    MReadMarsFile  read("Events", "~/MagicData/data1/CT1Data/ONTrain/*.root");
    read.DisableAutoScheme();
    tlist.AddToList(&read);

    MFParticleId fgamma("MMcEvt", '=', MMcEvt::kGAMMA);
    tlist.AddToList(&fgamma);

    MFParticleId fhadrons("MMcEvt", '!', MMcEvt::kGAMMA);
    tlist.AddToList(&fhadrons);

    MHMatrix matrixg("MatrixGammas");
    matrixg.AddColumn("cos(MMcEvt.fTelescopeTheta)");
    matrixg.AddColumn("MSigmabar.fSigmabar");
    matrixg.AddColumn("log10(Hillas.fSize)");
    matrixg.AddColumn("HillasSrc.fDist");
    matrixg.AddColumn("Hillas.fWidth");
    matrixg.AddColumn("Hillas.fLength");
    matrixg.AddColumn("log10(Hillas.fSize)/Hillas.fWidth*Hillas.fLength");
    matrixg.AddColumn("abs(Hillas.fM3Long)");
    matrixg.AddColumn("Hillas.fConc");
    matrixg.AddColumn("Hillas.fConc1");

    //matrixg.AddColumn("abs(Hillas.fAsym)");
    //matrixg.AddColumn("abs(Hillas.fM3Trans)");
    //matrixg.AddColumn("abs(HillasSrc.fHeadTail)");
    //matrixg.AddColumn("abs(HillasSrc.fAlpha)");

    plist.AddToList(&read);

    plist.AddToList(&matrixg);

    MHMatrix matrixh("MatrixHadrons");
    matrixh.AddColumns(matrixg.GetColumns());
    plist.AddToList(&matrixh);

    MFillH fillmatg("MatrixGammas");
    fillmatg.SetFilter(&fgamma);
    tlist.AddToList(&fillmatg);

    MFillH fillmath("MatrixHadrons");
    fillmath.SetFilter(&fhadrons);
    tlist.AddToList(&fillmath);

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

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // second event loop: the trees of the random forest are grown,
    // the event loop is now actually a tree loop (loop of training
    // process)
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------

    MTaskList tlist2;
    plist.Replace(&tlist2);

    MRanForestGrow rfgrow2;
    rfgrow2.SetNumTrees(10);
    rfgrow2.SetNumTry(3);
    rfgrow2.SetNdSize(10);

    MWriteRootFile rfwrite2("RF.root");
    rfwrite2.AddContainer("MRanTree","Tree");       //save all trees
    MFillH fillh2("MHRanForestGini");

    tlist2.AddToList(&rfgrow2);
    tlist2.AddToList(&rfwrite2);
    tlist2.AddToList(&fillh2);

    // gRandom is accessed from MRanForest (-> bootstrap aggregating)
    // and MRanTree (-> random split selection) and should be initialized
    // here if you want to set a certain random number generator
    if(gRandom)
        delete gRandom;
    gRandom = new TRandom3(0);

    //
    // Execute tree growing (now the eventloop is actually a treeloop)
    //
    if (!evtloop.Eventloop())
        return;

    tlist2.PrintStatistics();

    plist.FindObject("MHRanForestGini")->DrawClone();

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // third event loop: the control sample (star2.root) is processed
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

    MTaskList tlist3;

    plist.Replace(&tlist3);

    MReadMarsFile  read3("Events", "~/MagicData/data1/CT1Data/ONTest/*.root");
    read3.DisableAutoScheme();
    tlist3.AddToList(&read3);

    MRanForestCalc calc;
    tlist3.AddToList(&calc);

    MFillH fillh3a("MHHadronness");
    MFillH fillh3b("MHRanForest");

    tlist3.AddToList(&fillh3a);
    tlist3.AddToList(&fillh3b);

    MProgressBar bar;
    evtloop.SetProgressBar(&bar);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist3.PrintStatistics();

    plist.FindObject("MHRanForest")->DrawClone();
    plist.FindObject("MHHadronness")->DrawClone();
    plist.FindObject("MHHadronness")->Print();//*/
}
