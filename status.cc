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
!   Author(s): Thomas Bretz, 4/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  Status - Example how to use the MStatusDisplay
//
//  Use a camera file as input ----- PRELIMINARY
//
/////////////////////////////////////////////////////////////////////////////

#include "MStatusDisplay.h"
#include "MArray.h"
#include "MArgs.h"
#include "MLog.h"
#include "MLogManip.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MReadMarsFile.h"
#include "MGeomApply.h"
#include "MMcPedestalCopy.h"
#include "MMcPedestalNSBAdd.h"
//#include "MCerPhotCalc.h"
#include "MCerPhotAnal2.h"
//#include "MBlindPixelCalc.h"
#include "MSigmabarCalc.h"
#include "MImgCleanStd.h"
#include "MHillasCalc.h"
#include "MHillasSrcCalc.h"
//#include "MCT1SupercutsCalc.h"
#include "MHCamEvent.h"
#include "MFillH.h"
#include "MEvtLoop.h"
#include "MFDataMember.h"
#include "MCalibrate.h"
#include "MExtractSignal.h"
#include "MMcCalibrationUpdate.h"

#include <TApplication.h>
#include <TSystem.h>

using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "               status - MARS V" << MARSVER          << endl;
    gLog << "     MARS - Merging and Preprocessing Program"      << endl;
    gLog << "            Compiled on <" << __DATE__ << ">"       << endl;
    gLog << "               Using ROOT v" << ROOTVER             << endl;
    gLog << "   PROGRAM IS PRELIMINARY - FOR TEST CASES ONLY"    << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   status [-a0] [-vn] [-cn] inputfile[.root]" << endl << endl;
    gLog << "     input file:   Mars root file." << endl;
    gLog << "     -a0: Do not use Ansii codes." << endl;
    gLog << "     -vn: Verbosity level n [default=2]" << endl;
    gLog << "     -?/-h: This help" << endl << endl;
}


int main(int argc, char **argv)
{
    StartUpMessage();

    //
    // Evaluate arguments
    //
    MArgs arg(argc, argv);

    if (arg.HasOption("-?") || arg.HasOption("-h"))
    {
        Usage();
        return -1;
    }

    //
    // Set verbosity to highest level.
    //
    gLog.SetDebugLevel(arg.HasOption("-v") ? arg.GetIntAndRemove("-v") : 2);

    if (arg.HasOption("-a") && arg.GetIntAndRemove("-a")==0)
        gLog.SetNoColors();

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()!=1)
    {
        Usage();
        return -1;
    }

    //
    // This is to make argv[i] more readable insidethe code
    //
    TString kNamein = arg.GetArgumentStr(0);

    if (!kNamein.EndsWith(".root"))
        kNamein += ".root";

    //
    // check whether the given files are OK.
    //
    if (gSystem->AccessPathName(kNamein, kFileExists))
    {
        gLog << err << "Sorry, the input file '" << kNamein << "' doesn't exist." << endl;
        return -1;
    }

    TApplication app("Status", &argc, argv);
    if (gROOT->IsBatch() || !gClient)
    {
        gLog << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    //
    // Update frequency by default = 1Hz
    //
    MStatusDisplay *d = new MStatusDisplay;

    // From now on each 'Exit' means: Terminate the application
    d->SetBit(MStatusDisplay::kExitLoopOnExit);

    // Set update time to 5s
    // d->SetUpdateTime(5000);

    // Disable online update
    // d->SetUpdateTime(-1);

    d->SetLogStream(&gLog, kTRUE);            // Disables output to stdout
    gLog.SetOutputFile("status.log", kTRUE);  // Enable output to file
    //gLog.EnableOutputDevice(MLog::eStdout); // Enable output to stdout again

    // Set input file as display title (bottom line in ps file)
    d->SetTitle(kNamein);

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    /*
     MSrcPosCam src;
     src.SetXY(1./geomcam.GetConvMm2Deg(), 0);
     plist.AddToList(&src);
     */

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    MReadMarsFile read("Events");
    //read.DisableAutoScheme();

    // ------------- user change -----------------
    read.AddFile(kNamein);

    MGeomApply        geomapl;
    MMcPedestalCopy   pcopy;
    MMcPedestalNSBAdd pnsb;
    //MCerPhotCalc      ncalc;
    MCerPhotAnal2     nanal;

    MFDataMember f1("MRawRunHeader.fRunType", '>', 255.5);
    MFDataMember f2("MRawRunHeader.fRunType", '<', 255.5);
    f1.SetName("MFMonteCarlo");
    f2.SetName("MFRealData");

    MExtractSignal extra;
    extra.SetRange(5, 9, 5, 9);

    MMcCalibrationUpdate mcupd;
    mcupd.SetOuterPixelsGainScaling(kFALSE);

    MCalibrate calib;

    // MC
    extra.SetFilter(&f1);
    mcupd.SetFilter(&f1);
    calib.SetFilter(&f1);
    nanal.SetFilter(&f2);

    /*
     TArrayS blinds(6);
     blinds[0] =   0;
     blinds[1] = 195;
     blinds[2] = 227;
     blinds[3] = 248;
     blinds[4] = 271;
     blinds[5] = 291;

     blinds[3] =  51;
     blinds[4] =  56;
     blinds[5] = 112;
     blinds[6] =  31;
     blinds[7] = 116;
     blinds[8] = 507;
     blinds[9] = 559;
     blinds[10]= 291; // 311, 119, 54, 85, 125, 92, 133, 224

     MBlindPixelCalc blind;
     blind.SetPixelIndices(blinds);
     //blind.SetUseInterpolation();
     */

    // MSigmabarCalc     sgcal;
    MImgCleanStd      clean;
    MHillasCalc       hcalc;
    MHillasSrcCalc    scalc; // !!Preliminary!! Will be removed later!
    // MCT1SupercutsCalc calc1;

    // -------------------------------------------

    MHCamEvent hist("PedestalRms");
    hist.SetType(1);
    plist.AddToList(&hist);

    // -------------------------------------------

    MHCamEvent maxhi("MaxIdxHi", "Index of slice with maximum content (hi-gain)");
    MHCamEvent maxlo("MaxIdxLo", "Index of slice with maximum content (lo-gain)");
    maxhi.SetType(3);
    maxlo.SetType(4);
    plist.AddToList(&maxhi);
    plist.AddToList(&maxlo);

    // -------------------------------------------

    MFillH hfilhi("MaxIdxHi", "MRawEvtData");
    MFillH hfillo("MaxIdxLo", "MRawEvtData");
    MFillH hfill0("Uncleaned [MHCamEvent]", "MCerPhotEvt");
    MFillH hfill1("Pedestals [MHCamEvent]", "MPedestalCam");
    MFillH hfill2("PedestalRms", "MPedestalCam");
    MFillH hfill3("MHHillas", "MHillas");
    MFillH hfill4("MHHillasExt");
    MFillH hfill5("MHHillasExtSrc [MHHillasExt]", "MHillasSrc");
    MFillH hfill6("MHHillasSrc","MHillasSrc");
    MFillH hfill7("MHNewImagePar","MNewImagePar");
    //MFillH hfill8a("MHStarMap", "MHillas");
    //MFillH hfill8b("MHStarMap2", "MHillas");
    MFillH hfill9("Cleaned [MHCamEvent]", "MCerPhotEvt");
    //MFillH hfill10("MHHadronness", "MHadronness");
    //MFillH hfill11("MHSigmaTheta");

    tlist.AddToList(&read);
    tlist.AddToList(&f1);
    tlist.AddToList(&f2);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pnsb);

    tlist.AddToList(&extra);
    tlist.AddToList(&mcupd);
    tlist.AddToList(&calib);

    tlist.AddToList(&nanal);
    //tlist.AddToList(&blind);
    tlist.AddToList(&hfilhi);
    tlist.AddToList(&hfillo);
    tlist.AddToList(&hfill0);
    //tlist.AddToList(&sgcal);
    tlist.AddToList(&clean);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&scalc);
    //tlist.AddToList(&calc1);
    tlist.AddToList(&hfill1);
    tlist.AddToList(&hfill2);
    tlist.AddToList(&hfill3);
    tlist.AddToList(&hfill4);
    tlist.AddToList(&hfill5);
    tlist.AddToList(&hfill6);
    tlist.AddToList(&hfill7);
    //tlist.AddToList(&hfill8a);
    //tlist.AddToList(&hfill8b);
    tlist.AddToList(&hfill9);
    //tlist.AddToList(&hfill10);
    //tlist.AddToList(&hfill11);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return 1;

    tlist.PrintStatistics();

    if (!evtloop.GetDisplay())
        return 0;

    // From now on each 'Close' means: Terminate the application
    d->SetBit(MStatusDisplay::kExitLoopOnClose);

    // Wait until the user decides to exit the application
    app.Run(kFALSE);
    return 0;
}
