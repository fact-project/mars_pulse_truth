void rfenergyest()
{
    MSequence seqtst("~/Software/mc/sequencemc-test.txt");
    MSequence seqtrn("~/Software/mc/sequencemc-train.txt");

    if (!seqtst.IsValid())
    {
        cout << "Test sequence not valid" << endl;
        return;
    }
    if (!seqtrn.IsValid())
    {
        cout << "Train sequence not valid" << endl;
        return;
    }

    // --------------------- Setup files --------------------
    MReadMarsFile read("Events");
    MReadMarsFile read2("Events");
    read.DisableAutoScheme();
    read2.DisableAutoScheme();

    MDirIter iter, iter2;
    seqtrn.SetupDatRuns(iter,  MSequence::kImages, "~/Software/mc/img-abs");
    seqtst.SetupDatRuns(iter2, MSequence::kImages, "~/Software/mc/img-abs");

    read.AddFiles(iter);
    read2.AddFiles(iter2);

    // ----------------------- Setup RF ----------------------
    MHMatrix train("Train");
    train.AddColumn("MHillas.fSize");
    train.AddColumn("MHillasSrc.fDist");
    train.AddColumn("MPointingPos.fZd");
    /*
     train.AddColumn("MImagePar.fSizeSinglePixels");
     train.AddColumn("MHillas.GetArea");
     train.AddColumn("MNewImagePar.fLeakage1");
     train.AddColumn("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)");
     */

    // ------------------------------------------------------------

    // Last column must contain energy
    train.AddColumn("MMcEvt.fImpact/100");
    train.AddColumn("MMcEvt.fTelescopeTheta*kRad2Deg");
    train.AddColumn("MMcEvt.fEnergy");

    MStatusDisplay *d = new MStatusDisplay;

    // ----------------------- Fill Matrix RF ----------------------

    if(gRandom)
        delete gRandom;
    gRandom = new TRandom3();

    MTFillMatrix fill;
    fill.SetDisplay(d);
    fill.SetDestMatrix1(&train, 10000);//99999999);
    fill.SetReader(&read);

    /* ---------- It doesn't seem to improve anything ----------
     MFMagicCuts cuts;

     cuts.SetHadronnessCut(MFMagicCuts::kArea);
     cuts.SetThetaCut(MFMagicCuts::kOn);

     TArrayD arr(10);
     arr[0]=  1.3245;
     arr[1]=  0.208700;
     arr[2]=  0.229200;
     arr[3]=  5.305200;
     arr[4]=  0.098930;
     arr[5]= -0.082950;
     arr[6]=  8.2957;
     arr[7]=  0.8677;

     cuts.SetVariables(arr);

     fill.AddPreCut(&cuts);

     --------------- Use the cuts also in test-loop ----------------
     */

    if (!fill.Process())
        return;

    // ------------------------ Train RF --------------------------

    MRFEnergyEst rf;
    rf.SetDisplay(d);
    rf.SetFileName("rfenergys.root");

    MBinning b(32, 10, 100000, "BinningEnergyEst", "log");
    /*
    if (!rf.TrainMultiRF(train, b.GetEdgesD()))    // classification with one tree per bin
        return;

    if (!rf.TrainSingleRF(train, b.GetEdgesD()))   // classification into different bins
        return;
    */
    if (!rf.TrainSingleRF(train))                  // regression (best choice)
        return;


    gLog.Separator("Test");

    // --------------------- Display result ----------------------
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);
    plist.AddToList(&b);

    MHEnergyEst hist;
    //MContinue cont(&cuts);
    //cont.SetInverted();
    MFillH fillh(&hist);

    tlist.AddToList(&read2);
    //tlist.AddToList(&cont);
    tlist.AddToList(&rf);
    tlist.AddToList(&fillh);

    MEvtLoop loop;
    loop.SetDisplay(d);
    loop.SetParList(&plist);

    if (!loop.Eventloop())
        return;

    //d->SaveAsPS("rfenergys.ps");
}
