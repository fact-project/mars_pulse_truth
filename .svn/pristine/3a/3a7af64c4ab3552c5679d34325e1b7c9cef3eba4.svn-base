/* More explanations at the end of the file */
void optimdisp()
{
    MJOptimizeDisp opt;                     // Initialize optimization class
    opt.SetDebug(2);                        // Choose the level of output
    opt.SetOptimizer(MJOptimize::kMigrad);  // Choose the fit algorithm (see MJOptimize)
    opt.EnableTestTrain();                  // Split sample into test and train

    // -------------------- Setup ----------------------------

    opt.AddParameter("1-MHillas.fWidth/MHillas.fLength");   // M[0]
    opt.AddParameter("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg"); // M[1]
    opt.AddParameter("MNewImagePar.fLeakage1");             // M[2]
    opt.AddParameter("log10(MHillas.fSize)");               // M[3]

    // -------------- Parametrization --------------------

    char *r = "M[0]*([0] + [1]*M[1] + [2]*M[2] + (M[3]>[3])*[4]*(M[3]-[3])^2)";

    opt.FixParameter(0, 1.15136);
    opt.FixParameter(1, 0.0681437);
    opt.FixParameter(2, 2.62932);
    opt.FixParameter(3, 1.51279);
    opt.FixParameter(4, 0.0507821);

    opt.AddPreCut("MNewImagePar.fLeakage1>0");
    //opt.AddPreCut("log10(MHillas.fSize)<2.5"); //3.2

    opt.AddPreCut("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)*MGeomCam.fConvMm2Deg>-0.07");
    opt.AddPreCut("DataType.fVal>0.5");

    //opt.AddPreCut("abs(MHillasSrc.fDCA*MGeomCam.fConvMm2Deg)<0.2");
    //opt.AddPreCut("(MHillasSrc.fDist*MGeomCam.fConvMm2Deg-0.5)*7.2>MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg");

    // -------------------- Run ----------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    /*
     -------------------- Magic-Cuts ----------------------
     MFMagicCuts cuts;
     cuts.SetHadronnessCut(MFMagicCuts::kArea);
     cuts.SetThetaCut(MFMagicCuts::kNone);

     TArrayD arr(10);
     arr[0]=  1.3245;
     arr[1]=  0.208700;
     arr[2]=  0.0836169;
     arr[3]=  5.63973;
     arr[4]=  0.0844195;
     arr[5]= -0.07;
     arr[6]=  13.2;
     arr[7]=  1.0;
     arr[8]=  7.2;
     arr[9]=  0.5;

     cuts.SetVariables(arr);

     opt.AddPreCut(&cuts);

     -------------------- Energy Slope --------------------
     MFEnergySlope slope(-2.8);
     opt.AddPreCut(&slope);

     -------------------- Other cuts ----------------------
     */

    // opt.SetPathOut("optimdisp.root");
    opt.RunDisp("ganymed00000001-summary.root", r);
}

/* ------------------ Good strategy -------------------

   Par  |   0    1     2      3     4     |  Cut
 -------+---------------------------------+-----------------------
  Fit 1 |  1.3  0.1  fix=0  fix=0  fix=0  | Leak1==0 lgSize<2.5
  Fit 2 |  fix  fix  fix=0   2.5    0.2   | Leak1==0
  Fit 2 |  fix  fix   1.8    fix    fix   | Leak1>0
  Fit 3 |  free free  fix    fix    fix   | -/-
  Fit 4 |  fix  fix   fix    free   free  | -/-
  Fit 5 |  fix  fix   free   fix    fix   | -/-
 -------+---------------------------------+-----------------------
*/
