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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// dohtml.C
// ========
//
// This is a service macro used to create the html-documentation from
// source code (THtml)
//
// Add here all directories in which files are stored from which the
// documentation should be created an add all macros which should be
// converted to HTML.
//
///////////////////////////////////////////////////////////////////////////

void dohtml()
{
    //
    //  don't forget that the shared object must have been loaded
    //

    //
    // Do not print 'Info' messages from the root system such like
    // TCanvas::Print
    //
    gErrorIgnoreLevel=kWarning;

    //
    //   create the html document class
    //
    THtml html;

    TString sourcedir;
    sourcedir += "manalysis:";
    sourcedir += "mastro:";
    sourcedir += "mbadpixels:";
    sourcedir += "mbase:";
    sourcedir += "mcamera:";
    sourcedir += "mcalib:";
    sourcedir += "mcorsika:";
    sourcedir += "mdata:";
    sourcedir += "melectronics:";
    sourcedir += "mextralgo:";
    sourcedir += "mfbase:";
    sourcedir += "mfileio:";
    sourcedir += "mfilter:";
    sourcedir += "mfit:";
    sourcedir += "mgeom:";
    sourcedir += "mgui:";
    sourcedir += "mhbase:";
    sourcedir += "mhflux:";
    sourcedir += "mhft:";
    sourcedir += "mhist:";
    sourcedir += "mhistmc:";
    sourcedir += "mhcalib:";
    sourcedir += "mhvstime:";
    sourcedir += "mimage:";
    sourcedir += "mjobs:";
    sourcedir += "mjoptim:";
    sourcedir += "mjtrain:";
    sourcedir += "mmain:";
    sourcedir += "mmc:";
    sourcedir += "mmontecarlo:";
    sourcedir += "mmovie:";
    sourcedir += "mmuon:";
    sourcedir += "mpedestal:";
    sourcedir += "mpointing:";
    sourcedir += "mranforest:";
    sourcedir += "mraw:";
    sourcedir += "mreflector:";
    sourcedir += "mreport:";
    sourcedir += "msignal:";
    sourcedir += "msim:";
    sourcedir += "msimcamera:";
    sourcedir += "msimreflector:";
    sourcedir += "msql:";
    sourcedir += "mstarcam:";
    sourcedir += "mtools:";
    sourcedir += "mtrigger:";
    sourcedir += ".:";

    html.SetSourceDir(sourcedir);
    html.SetOutputDir("htmldoc");

    html.MakeAll(kTRUE);

    html.SetSourceDir("macros");
    html.Convert("merpp.C",                     "MARS - Merging and Preprocessing");
    html.Convert("tutorials/readraw.C",         "MARS - How To Read A Raw");
    html.Convert("rootlogon.C",                 "MARS - rootlogon.C");
    //html.Convert("readCT1.C",       "MARS - Read and display CT1 Events");
    html.Convert("readMagic.C",                 "MARS - Read and display Magic Events");
    //html.Convert("CT1Hillas.C",     "MARS - Calculate CT1 Hillas");
    html.Convert("MagicHillas.C",               "MARS - Calculate Magic Hillas");
//?    html.Convert("collarea.C",      "MARS - Calculate Collection Area from a MC root file");
    html.Convert("threshold.C",                 "MARS - Calculate Energy Threshold from a MC root file");
    html.Convert("trigrate.C",                  "MARS - Calculate Trigger Rate from a MC root file");
    html.Convert("star.C",                      "MARS - (St)andard (A)nalysis and (R)econstruction");
    html.Convert("starfield.C",                 "MARS - Display a starfield in the camera");
    html.Convert("starvisday.C",                "MARS - Display visibility of stars at a single day");
    html.Convert("starvisyear.C",               "MARS - Display visibility of a source around the year");
    html.Convert("tutorials/starplot.C",        "MARS - Plot parameters from file created with star.C");
    html.Convert("tutorials/readrfl.C",         "MARS - Example of reading reflector output with Mars");
    html.Convert("comprob.C",                   "MARS - Calculation of composite probabilities for G/H-Seperation");
    html.Convert("multidimdist.C",              "MARS - Calculation of multidimensional distances for G/H-Seperation");
    html.Convert("multidimdist2.C",             "MARS - Calculation of multidimensional distances for G/H-Seperation");
//    html.Convert("estimate.C",      "MARS - Shows results from the energy estimation");
//    html.Convert("estfit.C",        "MARS - Fits the coefficients of the energy estimator MEnergyEstParam");
    html.Convert("tutorials/plot.C",            "MARS - Plots 1D mars histogram");
    html.Convert("tutorials/plot2.C",           "MARS - Plots a 2D mars histogram");
    html.Convert("tutorials/testenv.C",         "MARS - Example to use TEnv and Mars Eventloops");
    html.Convert("triglvl2.C",                  "MARS - Example to use MMcTriggerLvl2 class, using filters and creating histograms");
    //html.Convert("status.C",        "MARS - Example to use the online display");
    //html.Convert("calibration.C",   "MARS - Example to use the calibration");
    html.Convert("tutorials/pedvsevent.C",      "MARS - Example to use MPedCalcPedRun");
    //html.Convert("pedphotcalc.C",   "MARS - Example to use MPedPhotCalc");
    html.Convert("tutorials/derotatedc.C",      "MARS - Example of plotting derotated dc currents");
    html.Convert("tutorials/evtrate.C",         "MARS - Example how to plot the event rate");
//    html.Convert("tutorials/pixvsevent.C",  "MARS - Example how to plot pixel value versus event number/time");
    html.Convert("tutorials/sectorvstime.C",    "MARS - Example how to plot mean value of a camera sector versus time");
    html.Convert("tutorials/pedestalvstime.C",  "MARS - Example how to plot pedestal/rms versus time");
    html.Convert("tutorials/pixfirerate.C",     "MARS - Example how to plot the firerate of a pixel (times above threshold)");
    html.Convert("tutorials/pixsatrate.C",      "MARS - Example how to plot the saturation rate of a pixel");
    html.Convert("tutorials/threshold.C",       "MARS - Example how to calculate the threshold");
    //html.Convert("calibration.C",   "MARS - Example how to use the calibrationa camera ");
    //html.Convert("bootcampstandardanalysis.C", "MARS - Example of the status of the standard analysis at the bootcamp");
    html.Convert("readIPR.C",                   "MARS - Example to read and display the IPRs from a (merpped) report file");
    html.Convert("tutorials/extendcam.C",       "MARS - Example of using MGeomCamExtend and MHexagonalFTCalc");
    html.Convert("tutorials/hft.C",             "MARS - Example of using MHexagonalFTCalc");
    html.Convert("tutorials/calendar.C",        "MARS - Example of producing a calendar sheet");
    html.Convert("tutorials/weights.C",         "MARS - Example of using weights with MFillH");
}
