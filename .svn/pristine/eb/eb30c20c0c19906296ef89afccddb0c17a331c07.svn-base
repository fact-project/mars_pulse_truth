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
//  MJCalibrateSignal
//
// This class is reading the output written by callisto. It calibrates
// signal and time.
//
// The signal and time extractors are read from the callisto-output. In
// pricipal you could overwrite these default using the resource file,
// but this is NOT recommended!
//
/////////////////////////////////////////////////////////////////////////////
#include "MJCalibrateSignal.h"

#include <TEnv.h>
#include <TFile.h>

// Core
#include "MLog.h"
#include "MLogManip.h"

#include "MDirIter.h"
#include "MTaskList.h"
#include "MParList.h"
#include "MEvtLoop.h"

#include "MStatusDisplay.h"

// General containers
#include "MGeomCam.h"
#include "MBadPixelsCam.h"
#include "MCalibConstCam.h"
#include "MPedestalCam.h"
#include "MArrivalTimeCam.h"

// General histograms
#include "MH3.h"
#include "MHCamEvent.h"
#include "MHVsTime.h"

// Calibration containers
#include "MCalibrationQECam.h"
#include "MCalibrationBlindCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationRelTimeCam.h"
#include "MCalibrationChargePINDiode.h"

// Calibration histograms
#include "MHCalibrationChargeCam.h"
#include "MHCalibrationChargeBlindCam.h"
#include "MHCalibrationChargePINDiode.h"
#include "MHCalibrationRelTimeCam.h"

// Tasks
#include "MReadMarsFile.h"
#include "MRawFileRead.h"
#include "MTaskEnv.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MGeomApply.h"
#include "MExtractTimeAndCharge.h"
#include "MTriggerPatternDecode.h"
#include "MCalibrationPatternDecode.h"
#include "MCalibrationChargeCalc.h"
#include "MCalibrationRelTimeCalc.h"
#include "MCalibCalcFromPast.h"
#include "MPedestalSubtract.h"
#include "MPedCalcFromLoGain.h"
#include "MCalibrateData.h"
#include "MExtractPINDiode.h"
#include "MExtractBlindPixel.h"
#include "MCalibrateRelTimes.h"
#include "MBadPixelsCalc.h"
#include "MBadPixelsTreat.h"
#include "MWriteRootFile.h"

// Filter
#include "MFTriggerPattern.h"
#include "MFCosmics.h"
#include "MFilterList.h"
#include "MFDataPhrase.h"

// Classes for writing movies
#include "MFEvtNumber.h"
#include "MMoviePrepare.h"
#include "MMovieWrite.h"
#include "MImgCleanStd.h"


ClassImp(MJCalibrateSignal);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fIsInterlaced to kTRUE
// - fIsRelTimesUpdate to kFALSE
// - fIsHiLoCalibration to kFALSE
//
MJCalibrateSignal::MJCalibrateSignal(const char *name, const char *title)
    : fExtractor(0), fIsInterlaced(kTRUE), fIsRelTimesUpdate(kTRUE), fIsMovieMode(kFALSE), fIsTestMode(kFALSE)
{
    fName  = name  ? name  : "MJCalibrateSignal";
    fTitle = title ? title : "Tool to calibrate data";
}

MJCalibrateSignal::~MJCalibrateSignal()
{
    if (fExtractor)
        delete fExtractor;
}

void MJCalibrateSignal::SetExtractor(const MExtractor *ext)
{
    if (fExtractor)
        delete fExtractor;

    fExtractor = ext ? (MExtractor*)ext->Clone() : NULL;
}

Bool_t MJCalibrateSignal::WriteResult() const
{
    if (IsNoStorage() || fIsMovieMode)
        return kTRUE;

    // FIXME: This is not nice because it will update the signal
    // file always. Which might make the usage outside of
    // callisto difficult.
    TObjArray cont;
    cont.Add(const_cast<TEnv*>(GetEnv()));
    cont.Add(const_cast<MSequence*>(&fSequence));

    TNamed cmdline("CommandLine", fCommandLine.Data());
    cont.Add(&cmdline);

    if (fDisplay)
    {
        TString title = "--  Calibrate Signal: ";
        title += fSequence.GetSequence();
        title += "  --";
        fDisplay->SetTitle(title, kFALSE);

        cont.Add(fDisplay);
    }

    const TString name(Form("signal%08d.root", fSequence.GetSequence()));
    return WriteContainer(cont, name, "UPDATE");
}

Bool_t MJCalibrateSignal::ReadCalibration(TObjArray &l, MBadPixelsCam &cam, MExtractor* &ext2, MExtractor* &ext3, TString &geom) const
{
    TString fname = Form("%s/calib%08d.root", fPathIn.Data(), fSequence.GetSequence());

    *fLog << inf << "Reading from file: " << fname << endl;

    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << dbginf << "ERROR - Could not open file " << fname << endl;
        return kFALSE;
    }

    TObject *o = file.Get("ExtractSignal");
    if (o && !o->InheritsFrom(MExtractor::Class()))
    {
        *fLog << err << dbginf << "ERROR - ExtractSignal read from " << fname << " doesn't inherit from MExtractor!" << endl;
        return kFALSE;
    }
    ext3 = o ? (MExtractor*)o->Clone() : NULL;

    o = file.Get("ExtractTime");
    if (o && !o->InheritsFrom(MExtractor::Class()))
    {
        *fLog << err << dbginf << "ERROR - ExtractTime read from " << fname << " doesn't inherit from MExtractor!" << endl;
        return kFALSE;
    }
    ext2 = o ? (MExtractor*)o->Clone() : NULL;
    if (!ext3 && !ext2)
    {
        *fLog << err << dbginf << "ERROR - Neither ExtractSignal nor ExrtractTime found in " << fname << "!" << endl;
        return kFALSE;
    }

    o = file.Get("MGeomCam");
    if (o && !o->InheritsFrom(MGeomCam::Class()))
    {
        *fLog << err << dbginf << "ERROR - MGeomCam read from " << fname << " doesn't inherit from MGeomCam!" << endl;
        return kFALSE;
    }
    geom = o ? o->ClassName() : "";

    TObjArray cont(l);
    cont.Add(&cam);
    return ReadContainer(cont);
}

// --------------------------------------------------------------------------
//
// MJCalibration allows to setup several option by a resource file:
//   MJCalibrateSignal.RawData: yes,no
//
// For more details see the class description and the corresponding Getters
//
Bool_t MJCalibrateSignal::CheckEnvLocal()
{
    SetInterlaced(GetEnv("Interlaced", fIsInterlaced));
    SetRelTimesUpdate(GetEnv("RelTimesUpdate", fIsRelTimesUpdate));
    SetMovieMode(GetEnv("MovieMode", fIsMovieMode));

    return MJCalib::CheckEnvLocal();
}

Bool_t MJCalibrateSignal::Process(MPedestalCam &pedcamab, MPedestalCam &pedcambias,
                                  MPedestalCam &pedcamextr)
{
    if (!fSequence.IsValid())
    {
	  *fLog << err << "ERROR - Sequence invalid..." << endl;
	  return kFALSE;
    }

    // --------------------------------------------------------------------------------

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate calibrated data from Sequence #";
    *fLog << fSequence.GetSequence() << endl << endl;


    if (!CheckEnv())
        return kFALSE;

    // --------------------------------------------------------------------------------

    MDirIter iter;
    if (fSequence.IsValid())
    {
        if (fSequence.GetRuns(iter, fIsTestMode?MSequence::kRawCal:MSequence::kRawDat)<=0)
            return kFALSE;
    }

    // Read File
    MHCalibrationChargeCam      hchacam;
    MHCalibrationChargeBlindCam hbndcam;
    MHCalibrationChargePINDiode hpndiod;
    MHCalibrationRelTimeCam     hrelcam;
    //MHCalibrationHiLoCam        hilocam;
    //MHCalibrationPulseTimeCam   hpulcam;

    hchacam.SetOscillations(kFALSE);
    hbndcam.SetOscillations(kFALSE);
    hrelcam.SetOscillations(kFALSE);

    MCalibrationChargeCam      calcam;
    MCalibrationQECam          qecam;
    MCalibrationBlindCam       bndcam;
    MCalibrationChargePINDiode pind;
    MCalibrationRelTimeCam     tmcam;
    //MCalibrationHiLoCam        hilcam;
    //MCalibrationPulseTimeCam   pulcam;

    MBadPixelsCam              badpix;

    TObjArray interlacedcont;
    if (fIsInterlaced)
    {
        interlacedcont.Add(&hchacam);
	if (IsUseBlindPixel())
            interlacedcont.Add(&hbndcam);
        if (IsUsePINDiode())  
            interlacedcont.Add(&hpndiod);
        if (fIsRelTimesUpdate)
            interlacedcont.Add(&hrelcam);
    }

    MExtractor *extractor1=fExtractor;
    MExtractor *extractor2=0;
    MExtractor *extractor3=0;
    TString geom;

    TObjArray calibcont;
    calibcont.Add(&calcam);
    calibcont.Add(&qecam);
    calibcont.Add(&bndcam);
    calibcont.Add(&tmcam);
    if (IsUseBlindPixel())
        calibcont.Add(&bndcam);

    if (!ReadCalibration(calibcont, badpix, extractor2, extractor3, geom))
        return kFALSE;

    *fLog << all;
    if (!geom.IsNull())
        *fLog << inf << "Camera geometry found in file: " << geom << endl;
    else
        *fLog << inf << "No Camera geometry found using default <MGeomCamMagic>" << endl;

    if (extractor3)
    {
        *fLog << underline << "Signal Extractor found in calibration file" << endl;
        extractor3->Print();
        *fLog << endl;
    }
    else
        *fLog << inf << "No Signal Extractor: ExtractSignal in file." << endl;


    if (extractor1)
    {
      *fLog << underline << "Modified Signal Extractor set by user." << endl;
      extractor1->Print();
      *fLog << endl;
    }
    else
    {
        *fLog << inf << "No modified Signal Extractor set by user... using ExtractSignal." << endl;
        extractor1 = extractor3 ? (MExtractor*)extractor3->Clone() : 0;
    }

    if (extractor2)
    {
        *fLog << underline << "Time Extractor found in calibration file" << endl;
        extractor2->Print();
        *fLog << endl;
    }
    else
        *fLog << inf << "No Time Extractor: ExtractTime in file." << endl;

    // This is necessary for the case in which it is not in the files
    MCalibConstCam constcam;

    //MBadPixelsCam badcam;
    constcam.SetBadPixels(&badpix);

    // Setup Parlist
    MParList plist;
    plist.AddToList(this); // take care of fDisplay!
    plist.AddToList(&badpix);
    plist.AddToList(&constcam);
    //plist.AddToList(&hilcam);
    plist.AddToList(&calibcont);       // Using AddToList(TObjArray *)
    plist.AddToList(&interlacedcont);  // Using AddToList(TObjArray *)
    //plist.AddToList(&pulcam);

    // Setup Tasklist
    MTaskList tlist;
    plist.AddToList(&tlist);

    MReadMarsFile readmc("Events");
    readmc.DisableAutoScheme();

    MRawFileRead rawread(NULL);
    rawread.SetForceMode(); // Ignore broken time-stamps

    MRead *read = fSequence.IsMonteCarlo() ? (MRead*)&readmc : (MRead*)&rawread;
    read->AddFiles(iter);

    const TString fname(Form("s/(([0-9]+_)?(M[12]_)?[0-9.]+)_[CD]_(.*[.])(raw|raw[.]gz|root)$/%s\\/$1_Y_$4root/",
                             Esc(fPathOut).Data()));

    // Skips MC which have no contents. This are precisely the
    // events which fullfilled the MC Lvl1 trigger and an
    // arbitrary cut (typically at 50phe) to speed up simulation
    MContinue contmc("MRawEvtData.GetNumPixels<0.5", "ContEmptyMC");

    //MPointingPosInterpolate pextr;
    //pextr.AddFiles(&iter);

    MGeomApply             apply; // Only necessary to create geometry
    if (!geom.IsNull())
        apply.SetGeometry(geom);
    //MBadPixelsMerge        merge(&badpix);

    // Make sure that pedcamab has the correct name
    pedcamab.SetName("MPedestalFundamental");
    pedcamextr.SetName("MPedestalFromExtractorRndm");
    pedcambias.SetName("MPedestalFromExtractor");
    plist.AddToList(&pedcamextr);
    plist.AddToList(&pedcambias);
    plist.AddToList(&pedcamab);

    MArrivalTimeCam timecam;
    plist.AddToList(&timecam);

    // Check for interleaved events
    MCalibrationPatternDecode caldec;
    MTriggerPatternDecode     decode;

    MH3 hpat("MRawRunHeader.GetFileID", "MTriggerPattern.GetUnprescaled");
    if (fSequence.IsMonteCarlo())
        hpat.SetWeight("100./MMcRunHeader.fNumSimulatedShowers");
    else
        hpat.SetWeight("1./TMath::Max(MRawRunHeader.GetRunLength,1)");
    hpat.SetName("TrigPat");
    hpat.SetTitle("Rate of the trigger pattern [Hz];File Id;Trigger Pattern;Rate [Hz]");
    hpat.InitLabels(MH3::kLabelsXY);
    //hpat.DefineLabelsY("1=Lvl1;2=Cal;3=Cal;4=Lvl2;5=Cal;7=Cal;8=Ped;9=Ped+Trig;13=Ped+Trig;16=Pin;32=Sum");
    hpat.DefaultLabelY("UNKNOWN");
    hpat.DefineLabelY( 0, "0");        //  0: No pattern
    hpat.DefineLabelY( 1, "Trig");     //  1: Lvl1
    hpat.DefineLabelY( 2, "Cal");      //  2: Cal
    hpat.DefineLabelY( 3, "Cal");      //     Cal+Lvl1
    hpat.DefineLabelY( 4, "Trig");     //  4: Lvl2
    hpat.DefineLabelY( 5, "Trig");     //     Lvl2+Lvl1
    hpat.DefineLabelY( 7, "Cal");      //     Lvl2+Cal+Lvl
    hpat.DefineLabelY( 8, "Ped");      //  8: Ped
    hpat.DefineLabelY( 9, "Ped+Trig"); //     Ped+Lvl1
    hpat.DefineLabelY(10, "Ped+Cal");  //     Ped+Cal
    hpat.DefineLabelY(12, "Ped+Trig"); //     Ped+Lvl2
    hpat.DefineLabelY(13, "Ped+Trig"); //     Ped+Lvl2+Lvl1
    hpat.DefineLabelY(16, "Pin");      // 16: Pin
    hpat.DefineLabelY(32, "Sum");      // 32: Sum
    hpat.DefineLabelY(33, "Trig");     //     Sum+Lvl1
    hpat.DefineLabelY(34, "Cal");      //     Sum+Cal
    hpat.DefineLabelY(35, "Cal");      //     Sum+Cal+Lvl1
    hpat.DefineLabelY(36, "Trig");     //     Sum+Lvl2
    hpat.DefineLabelY(37, "Trig");     //     Sum+Lvl1+Lvl2
    hpat.DefineLabelY(39, "Cal");      //     Sum+Lvl2+Cal+Lvl1

    MFillH fillpat(&hpat, "", "FillPattern");
    fillpat.SetDrawOption("box");

    // This will make that for data with version less than 5, where
    // trigger patterns were not yet correct, all the events in the real
    // data file will be processed. In any case there are no interleaved
    // calibration events in such data, so this is fine.
    // We allow only cosmics triggered events to pass (before prescaling)
    MFTriggerPattern fcalped("SelectTrigEvts");
    fcalped.SetInverted();
    fcalped.SetDefault(kTRUE);
    fcalped.DenyAll();
    fcalped.AllowTriggerLvl1();
    fcalped.AllowTriggerLvl2();
    fcalped.AllowSumTrigger();
    if (fIsTestMode)
        fcalped.AllowCalibration();

    // This will skip interleaved events with a cal- or ped-trigger
    MContinue contcalped(&fcalped, "ContNonTrigger");

    // Create the pedestal subtracted raw-data
    MPedestalSubtract pedsub;
    pedsub.SetPedestalCam(&pedcamab);

    // Do signal and pedestal calculation
    MPedCalcFromLoGain     pedlo1("MPedCalcFundamental");
    pedlo1.SetPedestalUpdate(kTRUE);
    pedlo1.SetNamePedestalCamOut("MPedestalFundamental");

    MPedCalcFromLoGain     pedlo2("MPedCalcWithExtractorRndm");
    pedlo2.SetPedestalUpdate(kTRUE);
    pedlo2.SetRandomCalculation(kTRUE);
    pedlo2.SetNamePedestalCamOut("MPedestalFromExtractorRndm");

    MPedCalcFromLoGain     pedlo3("MPedCalcWithExtractor");
    pedlo3.SetPedestalUpdate(kTRUE);
    pedlo3.SetRandomCalculation(kFALSE);
    pedlo3.SetNamePedestalCamOut("MPedestalFromExtractor");

    if (!extractor1)
    {
        *fLog << err << "ERROR - extractor1 == NULL" << endl;
        return kFALSE;
    }

    // Setup to use the hi-gain extraction window in the lo-gain
    // range (the start of the lo-gain range is added automatically
    // by MPedCalcFromLoGain)
    //
    // The window size of the extractor is not yet initialized,
    // so we have to stick to the extraction range
    //
    // Even if we would like to use a range comparable to the
    // hi-gain extraction we use the lo-gain range to make
    // sure that exclusions (eg. due to switching noise)
    // are correctly handled.
    //
    pedlo1.SetRangeFromExtractor(*extractor1);

    if (extractor1->InheritsFrom("MExtractTimeAndCharge"))
    {
        pedlo2.SetExtractor((MExtractTimeAndCharge*)extractor1);
        pedlo3.SetExtractor((MExtractTimeAndCharge*)extractor1);
    }
    else
    {
        pedlo2.SetRangeFromExtractor(*extractor1);
        pedlo3.SetRangeFromExtractor(*extractor1);
    }

    //------------------------------
    //
    // this is the filter to find pedestal events. For the Siegen FADC
    // these are all events which are not calibration events because
    // the pedestal is extracted from the lo-gain signal. For MUX
    // data this are artifiially triggered events with the pedestal
    // trigger flag, and for safty without Lvl1 or Lvl2 flag)
    //
    // For the time "before" the trigger pattern all events (Siegen FADC)
    // can be considered to be pedestal, because this was also the time
    // without artifially calibration events
    //
    // Deny or allow is done before prescaling.
    //
    MFTriggerPattern fped("SelectPedestals");
    fped.SetDefault(kTRUE);
    fped.DenyCalibration();
    if (!extractor1->HasLoGain())
    {
        fped.DenyAll();
        fped.RequirePedestal();
    }

    //------------------------------
    //
    // Apply a filter against cosmics (this is to make sure that the
    // trigger system was working properly and no empty events survive)
    // For every event 5% of the pixel must not be empty. In PostProcess
    // an error is raised if more than 50% of the events were skipped.
    //
    // Since unsuitable pixels are not counted in the ideal case
    // there is no empty pixel at all.
    //
    MFCosmics fcosmicscal;
    fcosmicscal.SetNamePedestalCam("MPedestalFundamental"); //CORRECT?
    fcosmicscal.SetMaxEmptyPixels(0.05);
    fcosmicscal.SetMaxAcceptedFraction(0.5);

    MContinue contcoscal(&fcosmicscal, "ContCosmicsCal");

    //------------------------------
    //
    // Remove events which are too bright. These events could either
    // be calibration events or events which produce many many
    // saturating low-gains and thus spoil the dead pixel plot.
    // These events don't seem to be physical events so we can fairly
    // remove them (they are no analysable gammas anyway)
    // So we remove all events which have less than 5% empty pixels.
    //
    MFCosmics fcosmicsbright;
    fcosmicsbright.SetNamePedestalCam("MPedestalFundamental");
    fcosmicsbright.SetMaxEmptyPixels(0.10);
    fcosmicsbright.SetMinAcceptedFraction(0.9);

    MContinue contbright(&fcosmicsbright, "ContBrightEvts");
    contbright.SetInverted();

    //------------------------------
    //
    // Thie signal extractors
    //
    MTaskEnv taskenv1("ExtractSignal");
    MTaskEnv taskenv2("ExtractTime");
    MTaskEnv taskenv3("ExtractInterlaced");
    taskenv1.SetDefault(extractor1);
    taskenv2.SetDefault(extractor2);
    taskenv3.SetDefault(extractor3);

    //
    // This is new calibration to photo-electrons, hard-coded
    // as decided at the Wuerzburg software meeting 26.01.05
    //
    MCalibrateData calib;
    calib.SetSignalType(MCalibrateData::kPhe);
    //calib.AddPedestal("Fundamental");
    calib.AddPedestal("FromExtractor");
    calib.AddPedestal("FromExtractorRndm");
    calib.SetPedestalFlag(MCalibrateData::kEvent);

    //----------------------------------------------------------

    MExtractPINDiode        pinext;
    MExtractBlindPixel      bldext;

    // Execute for all events with the calibration trigger. If no
    // trigger pattern is available do not execute it
    // The selection is done before prescaling.
    MFTriggerPattern        fcalib("SelectCalEvts");
    fcalib.SetDefault(kFALSE);
    fcalib.DenyAll();
    fcalib.RequireCalibration();
    fcalib.AllowTriggerLvl1();
    fcalib.AllowTriggerLvl2();
    fcalib.AllowSumTrigger();

    MCalibrationChargeCalc  chcalc;
    chcalc.SetContinousCalibration();
    chcalc.SetExtractor(extractor3);

    MCalibrationRelTimeCalc recalc;
    MCalibCalcFromPast      pacalc;

    chcalc.SetPedestals(&pedcamextr);

    pacalc.SetChargeCalc(&chcalc);
    if (fIsRelTimesUpdate)
      pacalc.SetRelTimeCalc(&recalc);
    pacalc.SetCalibrate(&calib);

    //
    // Calibration histogramming
    //
    MFillH filpin(&hpndiod, "MExtractedSignalPINDiode",   "FillPINDiode");
    MFillH filbnd(&hbndcam, "MExtractedSignalBlindPixel", "FillBlindCam");
    MFillH filcam(&hchacam, "MExtractedSignalCam",        "FillChargeCam");
    MFillH filtme(&hrelcam, "MArrivalTimeCam",            "FillRelTime");
    //MFillH filhil(&hilocam, "MExtractedSignalCam",        "FillHiLoRatio");
    //MFillH filpul(&hpulcam, "MRawEvtData",                "FillPulseTime");
    filpin.SetBit(MFillH::kDoNotDisplay);
    filbnd.SetBit(MFillH::kDoNotDisplay);
    filcam.SetBit(MFillH::kDoNotDisplay);
    filtme.SetBit(MFillH::kDoNotDisplay);
    //filhil.SetBit(MFillH::kDoNotDisplay);
    //filpul.SetBit(MFillH::kDoNotDisplay);

    MCalibrateRelTimes caltm;
    MBadPixelsCalc     bpcal;
    MBadPixelsTreat    treat;

    //bpcal.SetNamePedPhotCam("MPedPhotFromExtractor");
    bpcal.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    //treat.AddNamePedPhotCam("MPedPhotFundamental");
    treat.AddNamePedPhotCam("MPedPhotFromExtractor");
    treat.AddNamePedPhotCam("MPedPhotFromExtractorRndm");
    if (!extractor2 && !extractor1->InheritsFrom("MExtractTimeAndCharge"))
        treat.SetProcessTimes(kFALSE);

    MHCamEvent evt0(  0, "PedFLG",     "Fundamental Pedestal from Lo Gain;;P [cnts/sl]");
    MHCamEvent evt1(  2, "PedRmsFLG",  "RMS from Extractor applied to ped.;;\\sigma_{p} [cnts/sl]");
    MHCamEvent evt2(  0, "Extra'd",    "Extracted Signal;;S [cnts/sl]");
    // MHCamEvent evt3(4, "PedPhot",    "Calibrated Pedestal;;P [phe]");
    MHCamEvent evt4(  5, "PedRMS",     "Calibrated RMS from Extractor applied to ped.;;\\sigma_{p} [phe]");
    MHCamEvent evt5(  0, "Interp'd",   "Interpolated Signal scaled with A/A_{0};;S [phe]");
    MHCamEvent evt6(102, "Unsuitable", "Fraction of unsuitable events per Pixel;;[1]");
    // MHCamEvent evt7(  6, "Times",      "Calibrated Arrival Time;;T [fadc sl]");
    MHCamEvent evt8(  0, "Conv",       "Calibration Conv. Factors;;[phe/cnts]");
    MHCamEvent evt9(  7, "PulsePos",   "Pulse Position of cosmics (>50phe);;T [ns]");
    MHCamEvent evtR(  4, "HiLoCal",    "Hi-/Lo-Gain ratio;;Ratio");
    MHCamEvent evtO(  7, "HiLoOff",    "Lo-/Hi-Gain Offset;;Offset");
    MHCamEvent evtC(  4, "CalPos",     "Extracted pulse position of calibration pulses;;T [sl]");
    //MHCamEvent evt2(0, "Extra'd", "Extracted Calibration Signal;;S [cnts/sl]");
    evt2.SetErrorSpread(kFALSE);
    evt5.SetErrorSpread(kFALSE);
    evt6.SetErrorSpread(kFALSE);
    evt6.SetThreshold();

    MFillH fill0(&evt0, "MPedestalFundamental",          "FillPedFLG");
    MFillH fill1(&evt1, "MPedestalFromExtractorRndm",    "FillPedRmsFLG");
    MFillH fill2(&evt2, "MExtractedSignalCam",           "FillExtracted");
    // MFillH fill3(&evt3, "MPedPhotFundamental",  "FillPedPhot");
    MFillH fill4(&evt4, "MPedPhotFromExtractorRndm",     "FillPedRMS");
    MFillH fill5(&evt5, "MSignalCam",                    "FillInterpolated");
    MFillH fill6(&evt6, "MBadPixelsCam",                 "FillUnsuitable");
    // MFillH fill7(&evt7, "MSignalCam",                    "FillTimes");
    MFillH fill8(&evt8, "MCalibConstCam",                "FillConv");
    MFillH fill9(&evt9, "MSignalCam",                    "FillPulsePos");
    MFillH fillR(&evtR, "MExtractedSignalCam",           "FillHiLoCal");
    MFillH fillO(&evtO, "MArrivalTimeCam",               "FillHiLoOff");
    MFillH fillC(&evtC, "MArrivalTimeCam",               "FillCalPos");
    //MFillH fill2(&evt2, "MExtractedSignalCam", "FillExtractedSignal");

    MHVsTime histbp("MBadPixelsCam.GetNumUnsuitable");
    histbp.SetName("BadPixTm");
    histbp.SetTitle("Number of unsuitable pixels;;N");
    histbp.SetMinimum(0);

    MHVsTime histdp("MSignalCam.GetNumPixelsUnmapped");
    histdp.SetName("DeadPixTm");
    histdp.SetTitle("Number of dead/unmapped pixels;;N");
    histdp.SetMinimum(0);

    // Task to fill the histogram
    MFillH fillB(&histbp, "MTime", "FillBadPixTm");
    MFillH fillD(&histdp, "MTime", "FillDeadPixTm");
    fillB.SetNameTab("BadPixTm");
    fillD.SetNameTab("DeadPixTm");

    /*
     MFillH fillP("MHPulseShape", "", "FillPulseShape");
     fillP.SetNameTab("Pulse");
     */

    /*
     MHVsTime hbadpix("MBadPixelsCam.GetNumUnsuitable");
     hbadpix.SetNumEvents(50);
     MFillH fillB(&hbadpix, "MTime");
     */

    MTaskEnv fillflorian("FinalFantasy");
    fillflorian.SetDefault();

    // The second rule is for the case reading raw-files!
    MWriteRootFile write(2, fname, fOverwrite?"RECREATE":"NEW", "Calibrated Data");
    // Run Header
    write.AddContainer("MRawRunHeader",             "RunHeaders");
//    write.AddContainer("MBadPixelsCam",             "RunHeaders");
    write.AddContainer("MGeomCam",                  "RunHeaders");
    // Monte Carlo Headers
    write.AddContainer("MMcRunHeader",              "RunHeaders", kFALSE);
    write.AddContainer("MMcFadcHeader",             "RunHeaders", kFALSE);
    write.AddContainer("MMcTrigHeader",             "RunHeaders", kFALSE);
    write.AddContainer("MMcConfigRunHeader",        "RunHeaders", kFALSE);
    write.AddContainer("MMcCorsikaRunHeader",       "RunHeaders", kFALSE);
    write.AddContainer("MCorsikaRunHeader",         "RunHeaders", kFALSE);
    // Monte Carlo
    write.AddContainer("MMcEvt",                    "Events",     kFALSE);
    write.AddContainer("MMcTrig",                   "Events",     kFALSE);
    write.AddContainer("MCorsikaEvtHeader",         "Events",     kFALSE);
    // Data tree
    write.AddContainer("MSignalCam",                "Events");
    // write.AddContainer("MPedPhotFundamental",               "Events");
    write.AddContainer("MPedPhotFromExtractor",     "Events");
    write.AddContainer("MPedPhotFromExtractorRndm", "Events");
    write.AddContainer("MTime",                     "Events",     kFALSE);
    write.AddContainer("MRawEvtHeader",             "Events");
    write.AddContainer("MTriggerPattern",           "Events");

    // Trees with slow-control information (obsolete
    //  if we don't read merpped root-files)
    write.AddTree("Trigger",    kFALSE);
    write.AddTree("Drive",      kFALSE);
    write.AddTree("CC",         kFALSE);
    write.AddTree("Pyrometer",  kFALSE);
    write.AddTree("Currents",   kFALSE);
    write.AddTree("Camera",     kFALSE);
    // Slow-Control: Current-tree
    write.AddContainer("MCameraDC",          "Currents",   kFALSE);
    // Slow-Control: Camera-tree
    write.AddContainer("MCameraAUX",         "Camera",     kFALSE);
    write.AddContainer("MCameraCalibration", "Camera",     kFALSE);
    write.AddContainer("MCameraCooling",     "Camera",     kFALSE);
    write.AddContainer("MCameraHV",          "Camera",     kFALSE);
    write.AddContainer("MCameraLV",          "Camera",     kFALSE);
    write.AddContainer("MCameraLids",        "Camera",     kFALSE);

    // Write the special MC tree
    MWriteRootFile writemc(2, fname, fOverwrite?"RECREATE":"NEW", "Calibrated Data");
    writemc.SetName("WriteMC");
    writemc.AddContainer("MMcEvtBasic", "OriginalMC", kFALSE);
    if (fSequence.IsMonteCarlo())
        writemc.AddCopySource("OriginalMC", kFALSE);

    // Write the special calib tree
    /*
    MWriteRootFile writecal(2, fname, fOverwrite?"RECREATE":"NEW");
    writecal.SetName("WriteCalib");
    writecal.AddContainer("MBadPixelsCam",          "Calib");
    writecal.AddContainer("MCalibrationChargeCam",  "Calib");
    writecal.AddContainer("MCalibrationRelTimeCam", "Calib");
    */

    //-----------------------------------------------------------
    // Build tasklist

    MTaskList tlist2("AllEvents");

    tlist2.AddToList(&caldec);
    tlist2.AddToList(&decode);
    tlist2.AddToList(&fillpat);
    tlist2.AddToList(&apply);
    //tlist2.AddToList(&merge);
    tlist2.AddToList(&pedsub);

    //-----------------------------------------------------------
    // Pedestal extraction

    MTaskList tlist3("PedEvents");
    tlist3.SetFilter(&fped);         // Deny events with cal-trigger

    tlist2.AddToList(&fped);         // If no lo-gain require ped-trigger
    tlist2.AddToList(&tlist3);       //  and deny cosmics (lvl1/2) trigger

    tlist3.AddToList(&pedlo1);       // extract pedestal events
    tlist3.AddToList(&pedlo2);       // extract pedestal events
    tlist3.AddToList(&pedlo3);       // extract pedestal events
    tlist3.AddToList(&fill0);        // fill pedestal events
    tlist3.AddToList(&fill1);        // fill pedestal events

    //-----------------------------------------------------------
    // Calibration

    MTaskList tlist4("CalEvents");
    tlist4.SetFilter(&fcalib);       // process only events with cal-trigger

    //MFDataPhrase filcalco("MCalibrationConstCam.IsReadyToSave>0.5", "CalibConstFilter");
    if (fIsInterlaced && !fIsTestMode)
    {
        // The task list is executed for all events with the calibration
        // trigger
        tlist2.AddToList(&fcalib);     // MFTriggerPattern
        tlist2.AddToList(&tlist4);

        tlist4.AddToList(&taskenv3);
        tlist4.AddToList(&contcoscal); // MContinue/ContCosmicsCal
        if (IsUsePINDiode())
            tlist4.AddToList(&pinext); // MExtractPINDiode
        if (IsUseBlindPixel())
            tlist4.AddToList(&bldext); // MExtractBlindPixel
        tlist4.AddToList(&pacalc);     // MCalibCalcFromPast
        /*
         tlist3.AddToList(&filcalco);   // CalibConstFilter (IsReadyToSave)
         fill8.SetFilter(&filcalco);
         tlist3.AddToList(&fill8);      // FillConvUpd
         */

        tlist4.AddToList(&filcam);     // FillChargeCam
	if (fIsRelTimesUpdate)
	  tlist4.AddToList(&filtme);   // FillRelTime
        if (IsUseBlindPixel())
            tlist4.AddToList(&filbnd); // FillBlindCam
        if (IsUsePINDiode())
            tlist4.AddToList(&filpin); // FillPINDiode
        tlist4.AddToList(&chcalc);     // MCalibrationChargeCalc
	if (fIsRelTimesUpdate)
            tlist4.AddToList(&recalc); // MCalibrationRelTimeCam

        tlist4.AddToList(&fillC);      // FillCalPos

        //tlist3.AddToList(&writecal);   // MWriteRootFile
    }

    //-----------------------------------------------------------
    // Cosmics extraction

    // remove all events with a cal- or ped-trigger (no matter
    // whether they have lvl1 or lvl2 or any other flag
    tlist2.AddToList(&contcalped);

    // Extract the signal
    if (extractor1)
      tlist2.AddToList(&taskenv1);

    // remove all events which definitly don't have a signal
    // using MFCosmics (ContCosmicsPed)
    // tlist2.AddToList(&contcosped);

    // Extract arrival time (if a dedicated extrator given)
    if (extractor2)
        tlist2.AddToList(&taskenv2);

    // Check if we have an extremely bright event (remove them,
    // they are presumably errornous events or calibration events)
    if (!fIsTestMode)
        tlist2.AddToList(&contbright);

    /*
    if (fIsHiLoCalibration)
    {
        plist.AddToList(&hilocam);
        tlist2.AddToList(&filhil);
        }

    if (fIsPulsePosCheck)
    {
        plist.AddToList(&hpulcam);
        tlist2.AddToList(&filpul);
    }
    */

    tlist2.AddToList(&fill2);
    tlist2.AddToList(&fill8);        // FillConv
    tlist2.AddToList(&calib);        // MCalibrateData
    if (extractor2 || extractor1->InheritsFrom("MExtractTimeAndCharge"))
        tlist2.AddToList(&caltm);

    tlist2.AddToList(&bpcal);        // MBadPixelsCalc
    tlist2.AddToList(&treat);        // MBadPixelsTreat
    tlist2.AddToList(&fill6);
    //    tlist2.AddToList(&fill3);
    tlist2.AddToList(&fill4);
    tlist2.AddToList(&fill5);
    //if (extractor2 || extractor1->InheritsFrom("MExtractTimeAndCharge"))
    //    tlist2.AddToList(&fill7);
    tlist2.AddToList(&fill9);
    if (!fSequence.IsMonteCarlo())
    {
        tlist2.AddToList(&fillB);
        tlist2.AddToList(&fillD);
    }
    if (extractor1->HasLoGain())
    {
        tlist2.AddToList(&fillR);
        tlist2.AddToList(&fillO);
    }

    /*
     MFillH fillC("MHCleaning", "", "FillClean");
     tlist2.AddToList(&fillC);

     //tlist2.AddToList(&fillP);
     */

    // ----- Start: Code for encoding movies -----

    MMoviePrepare movprep;
    MMovieWrite   movwrite;
    movprep.SetRangeFromExtractor(*extractor1);

    //MFDataPhrase movfilt("MMovieData.fMax>150");
    MFDataPhrase movfilt("MMovieData.fMax>5*MMovieData.fMedianPedestalRms", "MovieFilter");

    MImgCleanStd movclean(8.5, 4.0);
    movclean.SetMethod(MImgCleanStd::kAbsolute);

    //movprep.SetFilter(&evtnum);
    movclean.SetFilter(&movfilt);
    movwrite.SetFilter(&movfilt);

    MTaskList tlistmov("MovieEncoder");
    tlistmov.AddToList(&movprep);
    tlistmov.AddToList(&movfilt);
    tlistmov.AddToList(&movclean);
    tlistmov.AddToList(&movwrite);

    MFEvtNumber evtnum;
    //evtnum.SetSelector("ThetaSquared.fVal<0.04");
    //evtnum.SetFileName("ganymed00000001.root");
    tlistmov.SetFilter(&evtnum);

    if (fIsMovieMode)
    {
        tlist2.AddToList(&evtnum);
        tlist2.AddToList(&tlistmov);
    }

    // ----- End: Code for encoding movies -----

    tlist2.AddToList(&fillflorian);

    // Setup List for Drive-tree
    //MPointingPosCalc pcalc;

    // Now setup main tasklist
    tlist.AddToList(read);

    if (fSequence.IsMonteCarlo())
    {
        if (!fIsMovieMode && !HasNullOut())
            tlist.AddToList(&writemc);
        tlist.AddToList(&contmc);
    }

    //if (IsUseRootData())
    //  tlist2.AddToList(&pextr);
    tlist.AddToList(&tlist2, fSequence.IsMonteCarlo() ? "Events" : "All");

    //if (fSequence.IsMonteCarlo())
    //  tlist.AddToList(&pcalc, "Drive");

    if (!fIsMovieMode && !HasNullOut())
        tlist.AddToList(&write);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    // Execute first analysis
    const Bool_t rc = evtloop.Eventloop(fMaxEvents);

    // make sure owned object are deleted
    if (extractor1 && extractor1!=fExtractor)
        delete extractor1;
    if (extractor2)
        delete extractor2;
    if (extractor3)
        delete extractor3;

    // return if job failed
    if (!rc)
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    // if everything went ok write and display result
    DisplayResult(plist);

    /*
    if (fIsPixelCheck)
      {
        if (fIsPulsePosCheck)
            hpulcam[fCheckedPixId].DrawClone("");

	//if (fIsHiLoCalibration)
	//    hilocam[fCheckedPixId].DrawClone("");
      }
    interlacedcont.Add(&pulcam);
    */

    //if (fIsHiLoCalibration)
    //  interlacedcont.Add(&hilcam);

    //if (fIsPulsePosCheck)
    //    interlacedcont.Add(plist.FindObject("MHCalibrationPulseTimeCam"));

    //if (fIsHiLoCalibration)
    //    interlacedcont.Add(plist.FindObject("MHCalibrationHiLoCam"));

    if (!WriteResult())
        return kFALSE;

    // return if job went ok
    *fLog << all << GetDescriptor() << ": Done." << endl;
    *fLog << endl << endl;

    return kTRUE;
}


void MJCalibrateSignal::DisplayResult(MParList &plist)
{
    /*
    if (!fDisplay || !fIsHiLoCalibration)
        return;

    MCalibrationHiLoCam *hcam = (MCalibrationHiLoCam*)plist.FindObject("MCalibrationHiLoCam");
    MGeomCam            *geom = (MGeomCam*)plist.FindObject("MGeomCam");
    if (!hcam || !geom)
        return;

    // Create histograms to display
    MHCamera disp1(*geom, "HiLoConv", "Ratio Amplification HiGain vs. LoGain (Charges)");
    MHCamera disp2(*geom, "HiLoDiff", "Arrival Time Diff. HiGain vs. LoGain (Times)");

    disp1.SetCamContent(*hcam, 0);
    disp1.SetCamError(  *hcam, 1);
    disp2.SetCamContent(*hcam, 5);
    disp2.SetCamError(  *hcam, 6);

    disp1.SetYTitle("R [1]");
    disp2.SetYTitle("\\Delta T [FADC sl.]");

    TCanvas &c1 = fDisplay->AddTab("HiLoConv");
    c1.Divide(2,3);

    disp1.CamDraw(c1, 1, 2, 1);
    disp2.CamDraw(c1, 2, 2, 1);
*/
}

