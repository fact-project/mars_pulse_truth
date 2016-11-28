void trainseparation()
{
    MDataSet settrn("mctrain.txt");
    MDataSet settst("mctest.txt");
    settrn.SetNumAnalysis(1);
    settst.SetNumAnalysis(2);

    // alternatively use:
    //MDataSet settrn("mctrain.txt", "/magic/montacrlo/sequences", "/magic/montecarlo/star");
    //MDataSet settst("mctest.txt" , "/magic/montacrlo/sequences", "/magic/montecarlo/star");

    MJTrainSeparation opt;
    //opt.SetDebug();

    // ------- Parameters to train Random Forest --------
    opt.AddParameter("MHillas.fSize");
    opt.AddParameter("MHillas.fWidth");
    opt.AddParameter("MHillas.fLength");
    opt.AddParameter("TMath::Abs(MHillasExt.fM3Long)");
    opt.AddParameter("MNewImagePar.fConc");
    opt.AddParameter("MNewImagePar.fConc1");

    // -------------------- Run ----------------------------
    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    /*
     -------------------- Energy Slope --------------------
     MFEnergySlope slope(-2.8);
     opt.AddPreCut(&slope);

     -------------------- Other cuts ----------------------
     opt.AddPreCut("MHillas.fSize>200");
     opt.AddPreCut("MHillasSrc.fDCA*MGeomCam.fConvMm2Deg<0.3");
    */

    opt.SetDataSetTrain(settrn, 10000, 10000);
    //opt.SetDataSetTest(settst);

    opt.Train("rf-separation.root");
}

/*
 // SequencesOn:   Monte Carlo sequences (gammas)
 // SequencesOff:  Data sequences (background)

 // Use:
 // opt.AddPreCut    to use cut for test and training
 // opt.AddTestCut   to use cut for test only
 // opt.AddTrainCut  to use cut for train only
 */
