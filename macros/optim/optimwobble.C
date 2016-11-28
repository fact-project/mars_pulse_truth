void optimwobble()
{
    MJOptimizeCuts opt("MHThetaSq");
    opt.SetDebug(2);
    opt.SetOptimizer(MJOptimize::kSimplex);

    // ------------------- Xi -----------------------
    opt.FixParameter( 0, 1.15136);
    opt.FixParameter( 8, 0.0681437);
    opt.FixParameter( 9, 2.62932);
    opt.FixParameter(10, 1.51279);
    opt.FixParameter(11, 0.0507821);
                  
    // --------------- Theta Sq ---------------------
    opt.FixParameter( 1, 0.215);

    // ----------------- Area -----------------------
    opt.SetParameter( 2, 0.21);
    opt.SetParameter( 3, 5.6);
    opt.SetParameter( 4, 0.083);

    // ----------------- M3L ------------------------
    opt.FixParameter( 5, -0.07);

    // ---------------- Slope -----------------------
    opt.FixParameter( 6, 7.2);
    opt.FixParameter( 7, 0.5);

    // ----------------- Fit ------------------------

    MAlphaFitter fit;
    fit.SetScaleMode(MAlphaFitter::kNone);
    fit.SetSignalFunction(MAlphaFitter::kThetaSq);
    fit.SetMinimizationStrategy(MAlphaFitter::kSignificanceLogExcess);

    // For wobble mode with three off-regions
    //  (sets also ScaleMode to kUser)
    fit.SetScaleUser(1./3);

    // ----------------- Cuts -----------------------

    MFMagicCuts cuts;
    cuts.SetThetaCut(MFMagicCuts::kOn);

    /* You can also try (switch on scaling):

     fit.SetScaleMode(MAlphaFitter::kBackground);
     fit.SetBackgroundFitMin(0.12);
     fit.SetBackgroundFitMax(0.5);

     cuts.SetThetaCut(MFMagicCuts::kOff);
     */

    // -----------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    opt.RunOnOff("ganymed00000001-summary.root", &cuts, &fit);
}
