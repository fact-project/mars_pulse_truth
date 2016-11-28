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
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  STAR - STandard Analysis and Reconstruction
//
// This macro is the standard converter to convert raw data into image
// parameters. It is a demonstration how the star-executable implementation
// looks like.
//
// As an input you need a Merpp output file (raw data).
// All parameters are written to an output file called starfile.root.
// For the calculation an arbitrary signal extractor (MCerPhotAnal2/Calc)
// is used.
//
/////////////////////////////////////////////////////////////////////////////

void star()
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    MReadMarsFile read("Events");
    read.DisableAutoScheme();

    // ------------- user change -----------------
    read.AddFile("~/MC/test_sample/Gamma_zbin0_0_*.root");

    MGeomApply        apply;
    MMcPedestalCopy   pcopy;
    MMcPedestalNSBAdd pnsb;

    MCerPhotCalc      ncalc;
    MCerPhotAnal2     nanal;

    MFDataMember f1("MRawRunHeader.fRunType", '>', 255.5);
    MFDataMember f2("MRawRunHeader.fRunType", '<', 255.5);

    ncalc.SetFilter(&f1);
    nanal.SetFilter(&f2);

    MBlindPixelCalc   blind;
    blind.SetUseInterpolation();

    MSigmabarCalc     sgcal;
    MImgCleanStd      clean;
    MHillasCalc       hcalc;
    MHillasSrcCalc    scalc; // !!Preliminary!! Will be removed later!

    // ------------- user change -----------------
    MWriteRootFile write("starfile.root");

    tlist.AddToList(&read);
    tlist.AddToList(&f1);
    tlist.AddToList(&f2);
    tlist.AddToList(&apply);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pnsb);
    tlist.AddToList(&ncalc);
    tlist.AddToList(&nanal);
    tlist.AddToList(&blind);
    tlist.AddToList(&sgcal);
    tlist.AddToList(&clean);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&scalc);
    tlist.AddToList(&write);

    // ------------- user change -----------------
    // You may use the "UPDATE" option in the constructor
    // of MWriteRootFile to update an existing file.
    //
    // tlist.SetSerialNumber(1); // Serial number of telescope
    write.AddContainer(write.AddSerialNumber("MMcEvt"),       "Events", kFALSE);
    write.AddContainer(write.AddSerialNumber("MSigmabar"),    "Events");
    write.AddContainer(write.AddSerialNumber("MHillas"),      "Events");
    write.AddContainer(write.AddSerialNumber("MHillasExt"),   "Events");
    write.AddContainer(write.AddSerialNumber("MHillasSrc"),   "Events");
    write.AddContainer(write.AddSerialNumber("MNewImagePar"), "Events");
    write.AddContainer(write.AddSerialNumber("MSrcPosCam"),   "RunHeaders");
    write.AddContainer("MRawRunHeader", "RunHeaders");
    write.AddContainer("MMcRunHeader",  "RunHeaders", kFALSE);

    //
    // Create and set up the eventloop
    //
    MProgressBar bar;

    MEvtLoop evtloop;
    evtloop.SetProgressBar(&bar);
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();
}
