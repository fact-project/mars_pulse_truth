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
!   Author(s): Thomas Bretz, 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJStar
//
// Resource file entries are case sensitive!
//
/////////////////////////////////////////////////////////////////////////////
#include "MJStar.h"

#include <TEnv.h>
#include <TFile.h>

// Core
#include "MLog.h"
#include "MLogManip.h"

#include "MDirIter.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MStatusDisplay.h"

// Histograms
#include "MHSectorVsTime.h"
#include "MHEffectiveOnTime.h"
#include "MHCamEvent.h"
#include "MBinning.h"

///NEEWWWW
#include "MH3.h"

// Tasks
#include "MReadReports.h"
#include "MReadMarsFile.h"
#include "MContinue.h"
#include "MGeomApply.h"
#include "MEventRateCalc.h"
#include "MImgCleanStd.h"
#include "MSrcPosCalc.h"
#include "MSrcPosCorrect.h"
#include "MHillasCalc.h"
#include "MMuonSearchParCalc.h"
#include "MMuonCalibParCalc.h"
#include "MFillH.h"
#include "MWriteRootFile.h"

// Filter
#include "MFDataPhrase.h"
#include "MFTriggerPattern.h"
#include "MFilterList.h"
#include "MFDataMember.h"
#include "MFDeltaT.h"
#include "MFSoftwareTrigger.h"

// Parameter container
#include "MMuonSetup.h"
#include "MObservatory.h"
//#include "MTriggerPattern.h"
#include "MPointingPosCalc.h"

ClassImp(MJStar);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets fRuns to 0, fExtractor to NULL, fDataCheck to kFALSE
//
MJStar::MJStar(const char *name, const char *title) : fMuonAnalysis(kTRUE)
{
    fName  = name  ? name  : "MJStar";
    fTitle = title ? title : "Standard analysis and reconstruction";
}

Bool_t MJStar::CheckEnvLocal()
{
    DisableMuonAnalysis(!GetEnv("MuonAnalysis", fMuonAnalysis));
    return kTRUE;
}

Bool_t MJStar::WriteResult()
{
    if (fPathOut.IsNull())
    {
        *fLog << inf << "No output path specified via SetPathOut - no output written." << endl;
        return kTRUE;
    }

    TObjArray cont;
    cont.Add(const_cast<TEnv*>(GetEnv()));
    cont.Add(const_cast<MSequence*>(&fSequence));

    TNamed cmdline("CommandLine", fCommandLine.Data());
    cont.Add(&cmdline);

    if (fDisplay)
    {
        TString title = "--  Star: ";
        title += fSequence.GetSequence();
        title += "  --";
        fDisplay->SetTitle(title, kFALSE);

        cont.Add(fDisplay);
    }

    const TString oname = Form("star%08d.root", fSequence.GetSequence());
    return WriteContainer(cont, oname, "RECREATE");
}

Bool_t MJStar::Process()
{
    if (!fSequence.IsValid())
    {
        *fLog << err << "ERROR - Sequence invalid!" << endl;
        return kFALSE;
    }

    // --------------------------------------------------------------------------------

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate image parameters of sequence ";
    *fLog << fSequence.GetFileName() << endl;
    *fLog << endl;

    if (!CheckEnv())
        return kFALSE;
 
    // --------------------------------------------------------------------------------

    MDirIter iter;
    if (fSequence.GetRuns(iter, MSequence::kCalibrated)<=0)
    {
        *fLog << err << "ERROR - Sequence valid but without files." << endl;
        return kFALSE;
    }

    // Setup Parlist
    MParList plist;
    plist.AddToList(this); // take care of fDisplay!

    MObservatory obs;
    plist.AddToList(&obs);

    MMuonSetup muonsetup;
    plist.AddToList(&muonsetup);

    // Setup binnings for muon analysis
    MBinning bins1("BinningRadius");
    MBinning bins2("BinningArcWidth");
    MBinning bins3("BinningRingBroadening");
    MBinning bins4("BinningSizeVsArcRadius");
    MBinning bins5("BinningMuonWidth");
    MBinning bins6("BinningArcPhi");
    MBinning binsd("BinningDist");
    plist.AddToList(&bins1);
    plist.AddToList(&bins2);
    plist.AddToList(&bins3);
    plist.AddToList(&bins4);
    plist.AddToList(&bins5);
    plist.AddToList(&bins6);
    plist.AddToList(&binsd);


    // Setup Tasklist
    MTaskList tlist;
    plist.AddToList(&tlist);

    MReadReports readreal;
    readreal.AddTree("Events", "MTime.", MReadReports::kMaster);
    readreal.AddTree("Drive",            MReadReports::kRequired);
    readreal.AddTree("Starguider",       MReadReports::kRequired);
    readreal.AddTree("Camera",           MReadReports::kRequired);
    readreal.AddTree("Currents");
    readreal.AddTree("CC");
    readreal.AddTree("Rec");
    readreal.AddTree("Trigger");
    readreal.AddTree("Pyrometer");
    readreal.AddFiles(iter);

    MReadMarsFile readmc("Events");
    readmc.DisableAutoScheme();
    readmc.AddFiles(iter);

    // ------------------ Setup general tasks ----------------

    MFDeltaT               fdeltat;
    MContinue              cont(&fdeltat, "FilterDeltaT", "Filter events with wrong timing");
    cont.SetInverted();

    MGeomApply             apply; // Only necessary to craete geometry
    MEventRateCalc         rate;
    rate.SetNumEvents(1200);

    MFSoftwareTrigger swtrig;
    MContinue contsw(&swtrig, "FilterSwTrigger", "Software trigger");
    contsw.SetInverted();

    MImgCleanStd           clean;
    clean.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    MSrcPosCalc poscalc;
    MHillasCalc hcalc;
    hcalc.Disable(MHillasCalc::kCalcConc);

    // ------------------ Setup histograms and fill tasks ----------------
    MHCamEvent evt0a(0, "Cleaned",   "Average signal after Cleaning;;S [phe]");
    MHCamEvent evt0c(0, "Sparkless", "Average signal after Cleaning and Spark cuts;;S [phe]");
    MHCamEvent evt0d(0, "Sparks",    "Average signal after Cleaning for Spark cuts;;S [phe]");
    MHCamEvent evt0b(0, "UsedPix",   "Fraction of Events in which Pixels are used;;Fraction");
    evt0a.SetErrorSpread(kFALSE);
    evt0b.SetErrorSpread(kFALSE);
    evt0c.SetErrorSpread(kFALSE);
    evt0d.SetErrorSpread(kFALSE);
    evt0b.SetThreshold(0);

    MFillH fillvs("MHRate",           "MTime",           "FillEventRate");
    MFillH fillp1("MHPointing",       "MTimeDrive",      "FillDrive");
    MFillH fillp2("MHPointing",       "MTimeStarguider", "FillStarguider");
    fillp1.SetBit(MFillH::kDoNotDisplay);
    fillp1.SetBit(MFillH::kCanSkip);
    fillp2.SetBit(MFillH::kCanSkip);

    // Needed in parameter list for ReadEnv
    MHEffectiveOnTime hontime;
    plist.AddToList(&hontime);

    MFillH fill0a(&evt0a,             "MSignalCam",      "FillSignalCam");
    MFillH fill0b(&evt0b,             "MSignalCam",      "FillCntUsedPixels");
    MFillH fill0c(&evt0c,             "MSignalCam",      "FillSignalCamSparkless");
    MFillH fill0d(&evt0d,             "MSignalCam",      "FillSignalCamSparks");
    MFillH fill1("MHHillas",          "MHillas",         "FillHillas");
    MFillH fill2("MHHillasExt",       "",                "FillHillasExt");
    MFillH fill3("MHHillasSrc",       "MHillasSrc",      "FillHillasSrc");
    MFillH fill4("MHImagePar",        "MImagePar",       "FillImagePar");
    MFillH fill5("MHNewImagePar",     "MNewImagePar",    "FillNewImagePar");
    MFillH fill9("MHEffectiveOnTime", "MTime",           "FillEffOnTime");

    //fillvs.SetNameTab("Rate");
    fill9.SetNameTab("EffOnTime");
    fill0c.SetNameTab("Sparkless");
    fill0d.SetNameTab("Sparks");

    // FIXME: This should be replaced by a cut working for all triggers!
    MTriggerPattern par;   // An empty trigger pattern for older files
    plist.AddToList(&par); // without MTriggerPattern stored in the file

    // For the effective on-time calculation and the muon statistics
    // we don't want SUM-only events
    MFTriggerPattern fsum;
    fsum.SetDefault(kTRUE);
    fsum.DenyAll();
    fsum.RequireTriggerLvl1();
    fsum.AllowTriggerLvl2();
    fsum.AllowSumTrigger();
    fill9.SetFilter(&fsum);

    // Plot the trigger pattern rates vs. run-number
    MH3 hrate("MRawRunHeader.GetFileID", "MTriggerPattern.GetUnprescaled");
    if (fSequence.IsMonteCarlo())
        hrate.SetWeight("100./MMcRunHeader.fNumSimulatedShowers");
    else
        hrate.SetWeight("1./TMath::Max(MRawRunHeader.GetRunLength,1)");
    hrate.SetName("Rate");
    hrate.SetTitle("Event rate after cleaning [Hz];File Id;Trigger Type;");
    hrate.InitLabels(MH3::kLabelsX);
    hrate.DefaultLabelY("ERROR");
    hrate.DefineLabelY( 0, "0");        //  0: No pattern
    hrate.DefineLabelY( 1, "Trig");     //  1: Lvl1
    hrate.DefineLabelY( 4, "Trig");     //  4: Lvl2
    hrate.DefineLabelY( 5, "Trig");     //     Lvl2+Lvl1
    hrate.DefineLabelY(32, "Sum");      // 32: Sum
    hrate.DefineLabelY(33, "Trig");     //     Sum+Lvl1
    hrate.DefineLabelY(36, "Trig");     //     Sum+Lvl2
    hrate.DefineLabelY(37, "Trig");     //     Sum+Lvl1+Lvl2

    MFillH frate(&hrate, "", "FillRate");
    frate.SetDrawOption("box");

    // ------------------ Setup write task ----------------

    // Effective on-time need its own not to be skipped by (eg) image cleaning
    // Muons needs its own to have a unique SetReadyToSave
    const TString rule(Form("s/(([0-9]+_)?(M[12]_)?[0-9.]+)_Y_(.*[.]root)$/%s\\/$1_I_$4/", Esc(fPathOut).Data()));
    MWriteRootFile write( 2, rule, fOverwrite?"RECREATE":"NEW", "Image parameters");
    MWriteRootFile writet(2, rule, fOverwrite?"RECREATE":"NEW", "Image parameters"); // EffectiveOnTime
    MWriteRootFile writem(2, rule, fOverwrite?"RECREATE":"NEW", "Image parameters"); // Muons
    writem.SetName("WriteMuons");

    // Data
    write.AddContainer("MHillas",                   "Events");
    write.AddContainer("MHillasExt",                "Events");
    write.AddContainer("MHillasSrc",                "Events");
    write.AddContainer("MImagePar",                 "Events");
    write.AddContainer("MNewImagePar",              "Events");
    write.AddContainer("MRawEvtHeader",             "Events");
    write.AddContainer("MPointingPos",              "Events");
    write.AddContainer("MTriggerPattern",           "Events");

    // Run Header
    write.AddContainer("MRawRunHeader",             "RunHeaders");
//    write.AddContainer("MBadPixelsCam",             "RunHeaders");
    write.AddContainer("MGeomCam",                  "RunHeaders");
    write.AddContainer("MObservatory",              "RunHeaders");

    // Muon Setup
    write.AddContainer("BinningRadius",             "RunHeaders");
    write.AddContainer("BinningArcWidth",           "RunHeaders");
    write.AddContainer("BinningRingBroadening",     "RunHeaders");
    write.AddContainer("BinningSizeVsArcRadius",    "RunHeaders");
    write.AddContainer("MMuonSetup",                "RunHeaders");

    if (fSequence.IsMonteCarlo())
    {
        // Monte Carlo Data
        write.AddContainer("MMcEvt",                "Events");
        write.AddContainer("MMcTrig",               "Events", kFALSE);
        write.AddContainer("MCorsikaEvtHeader",     "Events", kFALSE);
        write.AddContainer("MSrcPosCam",            "Events");
        // Monte Carlo Run Headers
        write.AddContainer("MMcRunHeader",          "RunHeaders");
        write.AddContainer("MMcTrigHeader",         "RunHeaders", kFALSE);
        write.AddContainer("MMcFadcHeader",         "RunHeaders", kFALSE);
        write.AddContainer("MMcConfigRunHeader",    "RunHeaders", kFALSE);
        write.AddContainer("MMcCorsikaRunHeader",   "RunHeaders", kFALSE);
        write.AddContainer("MCorsikaRunHeader",     "RunHeaders", kFALSE);
    }
    else
    {
        // Event time
        write.AddContainer("MTime", "Events");

        // Slow-control trees with time information
        write.AddTree("Drive");
        write.AddTree("Starguider", kFALSE);
        write.AddTree("Pyrometer",  kFALSE);
        writet.AddContainer("MEffectiveOnTime",     "EffectiveOnTime");
        writet.AddContainer("MTimeEffectiveOnTime", "EffectiveOnTime");
    }

    // What to write in muon tree
    writem.AddContainer("MMuonSearchPar",           "Muons");
    writem.AddContainer("MMuonCalibPar",            "Muons");
    writem.AddContainer("MHillas",                  "Muons");
    writem.AddContainer("MHillasExt",               "Muons");
    writem.AddContainer("MHillasSrc",               "Muons");
    writem.AddContainer("MImagePar",                "Muons");
    writem.AddContainer("MNewImagePar",             "Muons");
    writem.AddContainer("MRawEvtHeader",            "Muons");
    writem.AddContainer("MPointingPos",             "Muons");
    if (fSequence.IsMonteCarlo())
    {
        // Monte Carlo Data
        writem.AddContainer("MMcEvt",               "Muons");
        writem.AddContainer("MMcTrig",              "Muons", kFALSE);
    }

    if (fSequence.IsMonteCarlo())
    {
        if (fMuonAnalysis)
            writem.AddCopySource("OriginalMC");
        else
            write.AddCopySource("OriginalMC");
    }

    MTaskList tlist2("Events");
    tlist2.AddToList(&apply);
    if (!fSequence.IsMonteCarlo())
        tlist2.AddToList(&cont);
    tlist2.AddToList(&contsw);
    if (!fSequence.IsMonteCarlo())
    {
        // Calibration events don't enter star at all.
        tlist2.AddToList(&rate);
        tlist2.AddToList(&fillvs);
        tlist2.AddToList(&fsum);
        tlist2.AddToList(&fill9);
        tlist2.AddToList(&writet);
    }

    // Spark cut:
    //  This cut is a little bit different from the default cut in the
    //  ganymed.rc, because the cut in ganymed.rc also suppresses a
    //  little background, while the cut here only shows sparks
    MFDataPhrase fsparks("log10(MNewImagePar.fConcCOG)<-0.45*(log10(MHillas.fSize)-2.5)-0.24", "SparkCut");
    //fill0b.SetFilter(&fsparks);
    fill0c.SetFilter(&fsparks);

    // Inverted spark cut (need not to be a member of the task list
    // because it fsparks is
    MFilterList fnsparks(&fsparks);
    fill0d.SetFilter(&fnsparks);

    tlist2.AddToList(&clean);
    tlist2.AddToList(&poscalc);

    MFillH fillsp("MHSrcPosCam", "MSrcPosCam", "FillSrcPosCam");
    fillsp.SetNameTab("Src");
    if (fSequence.IsMonteCarlo())
        tlist2.AddToList(&fillsp);


    tlist2.AddToList(&hcalc);
    tlist2.AddToList(&fsparks);
    tlist2.AddToList(&fill0a);
    tlist2.AddToList(&fill0c);
    tlist2.AddToList(&fill0d);
    tlist2.AddToList(&fill0b);
    tlist2.AddToList(&frate);
    tlist2.AddToList(&fill1);
    tlist2.AddToList(&fill2);
    tlist2.AddToList(&fill3);
    tlist2.AddToList(&fill4);
    tlist2.AddToList(&fill5);

    // ----------------------- Muon Analysis ----------------------
    // Filter to start muon analysis
    MFDataPhrase fmuon1("MHillas.fSize>150 && MNewImagePar.fConcCOG<0.1", "MuonPreCut");
    // Filter to calculate further muon parameters
    MFDataPhrase fmuon2("(MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg>0.6) && (MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg<1.35) &&"
                        "(MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg<0.152)", "MuonSearchCut");
    // Filter to fill the MHMuonPar
    MFDataPhrase fmuon3("(MMuonCalibPar.fArcPhi>190) && (MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg<0.118) &&"
                        "(MMuonCalibPar.fArcWidth<0.20) && (MMuonCalibPar.fArcWidth>0.04) &&"
                        "MMuonCalibPar.fRelTimeSigma<1.5",
                        "MuonFinalCut");
    // Filter to write Muons to Muon tree
    MFDataMember fmuon4("MMuonCalibPar.fArcPhi", '>', -0.5, "MuonWriteCut");
    writem.SetFilter(&fmuon4);

    MMuonSearchParCalc muscalc;
    muscalc.SetFilter(&fmuon1);

    MMuonCalibParCalc mcalc;
    mcalc.SetFilter(&fmuon2);

    MFillH fillmuon("MHSingleMuon", "", "FillMuon");
    MFillH fillmpar("MHMuonPar",    "", "FillMuonPar");
    fillmuon.SetFilter(&fmuon2);
    fillmpar.SetFilter(&fmuon3);
    fillmuon.SetBit(MFillH::kDoNotDisplay);

    if (fMuonAnalysis)
    {
        tlist2.AddToList(&fmuon1);
        tlist2.AddToList(&muscalc);
        tlist2.AddToList(&fmuon2);
        tlist2.AddToList(&fillmuon);
        tlist2.AddToList(&mcalc);
        tlist2.AddToList(&fmuon3);
        tlist2.AddToList(&fillmpar);
        tlist2.AddToList(&fmuon4);
        tlist2.AddToList(&writem);
    }

    // ------------------------------------------------------------

    // Initialize histogram
    MHSectorVsTime histdc, histrms;
    histdc.SetNameTime("MTimeCamera");
    histdc.SetTitle("Average DC currents of all pixels vs time;;<I> [nA]");
    histdc.SetMinimum(0);
    histdc.SetMaximum(10);
    histrms.SetNameTime("MTimeCamera");
    histrms.SetTitle("Average pedestal rms of all pixels vs time;;<\\sigma_{p}> [phe]");
    histrms.SetType(5);
    histrms.SetMinimum(0);
    histrms.SetMaximum(10);

    /*
     // Define area index [0=inner, 1=outer]
     // TArrayI inner(1); inner[0] = 0; histdc.SetAreaIndex(inner);
     */

    // Task to fill the histogram
    MFillH filldc(&histdc,   "MCameraDC",                 "FillDC");
    MFillH fillrms(&histrms, "MPedPhotFromExtractorRndm", "FillPedRms");
    //MFillH filltst("MHTest", "MTime", "FillTest");
    filldc.SetNameTab("Currents");
    fillrms.SetNameTab("MeanRms");
    //filltst.SetNameTab("Test");

    MFillH fillw1("MHWeather", "MTimeCC",        "FillWeather");
    MFillH fillw2("MHWeather", "MTimePyrometer", "FillPyrometer");
    fillw2.SetBit(MFillH::kDoNotDisplay);
    fillw2.SetBit(MFillH::kCanSkip);

    // instantiate camera histogram containers
    MHCamEvent evtdt(0, "Thresholds", "Average Discriminator Thresholds;;DT [au]");
    plist.AddToList(&evtdt);

    // instantiate fill tasks
    MFillH filldt1("Thresholds", "MCameraTH", "FillDT-CC");
    MFillH filldt2("Thresholds", "MCameraTH", "FillDT-Rec");
    filldt1.SetNameTab("DT");
    filldt2.SetBit(MFillH::kDoNotDisplay);
    filldt2.SetBit(MFillH::kCanSkip);
    filldt1.SetBit(MFillH::kCanSkip);

    MHSectorVsTime histipr;

    histipr.SetNameTime("MTimeTrigger");
    histipr.SetTitle("Mean of all IPR;;<IPR> [Hz]");
    histipr.SetMinimum(0);
    //histipr.SetUseMedian();

    // Task to fill the histogram
    MFillH fillipr(&histipr, "MTriggerIPR", "FillIPR");
    fillipr.SetNameTab("IPR");

    MPointingPosCalc pcalc;

    // ------------------------------------------------------------

    tlist.AddToList(fSequence.IsMonteCarlo() ? (MTask*)&readmc : (MTask*)&readreal);
    tlist.AddToList(&pcalc,  fSequence.IsMonteCarlo() ? "Events" : "Drive");
    //tlist.AddToList(&filltst, "Events");
    tlist.AddToList(&tlist2, "Events");
    if (!fSequence.IsMonteCarlo())
    {
        // initiate task list
        tlist.AddToList(&fillw1,  "CC");
        tlist.AddToList(&fillw2,  "Pyrometer");
        tlist.AddToList(&fillp1,  "Drive");
        tlist.AddToList(&fillp2,  "Starguider");
        tlist.AddToList(&fillrms, "Camera");
        tlist.AddToList(&filldc,  "Camera");
        tlist.AddToList(&fillipr, "Trigger");
        tlist.AddToList(&filldt1, "CC");   // Old files: Receiver information in CC-Tree  (Mars<=2.0)
        tlist.AddToList(&filldt2, "Rec");  // New files: Receiver information in Rec-Tree (Mars >2.0)
    }
    if (!HasNullOut())
        tlist.AddToList(&write);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    if (!WriteResult())
        return kFALSE;

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;;

    return kTRUE;
}
