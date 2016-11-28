void optimcrab()
{
    MJOptimize opt;
    opt.SetDebug(2);
    opt.SetOptimizer(MJOptimize::kSimplex);

    // ---------------------------------------------------

    opt.FixParameter(0,  -0.10011,   -5, 5);
    opt.FixParameter(1,   0.0669669,  -5, 5);

    opt.FixParameter(2,   0.923,  -5,5);
    opt.FixParameter(3,  -0.8900478,  -5, 5);

    opt.FixParameter(4,   0.215533, -5, 5);

    // ---------------------------------------------------
    opt.FixParameter(5,   0.106519,  -1, 1);
    opt.FixParameter(6,   0.5);
    // --------------------ALPHA--------------------------

    opt.FixParameter(7,   2.13579,   0, 20);
    opt.FixParameter(8,   4.78171,   0, 20);
    opt.FixParameter(9,   1.17461,   0, 20);
    // ---------------------AREA--------------------------

    opt.FixParameter(10,  0.263628,  0, 20);
    opt.FixParameter(11,  5.25951,   0, 20);
    opt.FixParameter(12,  0.111246,  0, 20);
    // --------------------D1/D2--------------------------

    opt.FixParameter(13,  0.188982,   0, 20);
    opt.FixParameter(14,  1.10405,    0, 20);

    opt.FixParameter(15,  0.0960984,  0, 10);
    opt.FixParameter(16,  0.0991635,  0, 10);

    opt.FixParameter(17,  0, -10, 10);
    opt.FixParameter(18,  0, -10, 10);
    // --------------------MD1/MD2--------------------------

    opt.SetParameter(20,  0.4, -10, 10);
    opt.FixParameter(21,  0,   -10, 10);
    opt.SetParameter(22,  1.0, -10, 10);
    opt.FixParameter(23,  0,   -10, 10);

    opt.SetParameter(24, -1,   -10, 10);
    opt.FixParameter(25,  0,   -10, 10);
    opt.FixParameter(26,  0,   -10, 10);

    // S=14.7, E=485, B=285
    // ---------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    MAlphaFitter fit;
    fit.SetScaleMode(MAlphaFitter::kNone);
    fit.SetSignalIntegralMax(99);
    //fit.SetMinimizationStrategy(MAlphaFitter::kExcess);
    fit.SetMinimizationStrategy(MAlphaFitter::kSignificanceLogExcess);
    //fit.SetMinimizationStrategy(MAlphaFitter::kSignificance);

    // A88A
    // DIST vs. atan(width/w, length/l)
    // Image cleaning with AT to get better size! (IMPORTANCE: E-est!)

    MFMagicCuts cuts;
    cuts.SetAlphaCut(MFMagicCuts::kWobble);
    opt.RunOnOff("wobblelza-abs.root", &cuts, &fit);
}
