void optimenergy()
{
    MJOptimizeEnergy opt;
    opt.SetDebug(2);
    opt.SetOptimizer(MJOptimize::kMigrad);
    opt.EnableTestTrain();

    // -------------------- Setup ----------------------------
    opt.AddParameter("MHillas.fSize");

    opt.FixParameter(0, 0.9, 0, 2);  //0.562676

    char *r = "[0]*M[0]";

    // -------------------- Run ----------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    /*
     -------------------- Magic-Cuts ----------------------
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

     opt.AddPreCut(&cuts);

     -------------------- Energy Slope --------------------
     MFEnergySlope slope(-2.8);
     opt.AddPreCut(&slope);

     -------------------- Other cuts ----------------------
     opt.AddPreCut("MPointingPos.fZd<7");
    */

    // opt.SetPathOut("optimenergy.root");
    opt.RunEnergy("ganymedmcpart.root", r);
}
