#include "MLogManip.h"

int callisto(const char *seqfile="seq/2012/01/23/20120123_023.seq", const char *outpath = "output",
//             TString drstime = "/gpfs0/fact/processing/drs_time_calib/20111115.time.drs.fits",
             const char *drstime = "/gpfs0/fact/processing/drs_time_calib/20111115.time.drs.fits",
             const char *delays="resources/delays-20150217.txt")
{
    // ======================================================

    // true:  Display correctly mapped pixels in the camera displays
    //        but the value-vs-index plot is in software/spiral indices
    // false: Display pixels in hardware/linear indices,
    //        but the order is the camera display is distorted
    bool usemap = true;

    // map file to use (get that from La Palma!)
//    const char *map = usemap ? "/home/fact/FACT++/FACTmap111030.txt" : NULL;
    const char *map = usemap ? "/scratch/fact/FACTmap111030.txt" : NULL;

    // ------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;

    // ------------------------------------------------------

    // ------------------------------------------------------

    // Calib: 51 / 90 / 197 (20% TH)
    // Data:  52 / 64 / 104 (20% TH)

    // Extraction range in slices. It will always(!) contain the full range
    // of integration
    const int first_slice =  25; //  10ns
    const int last_slice  = 225; // 125ns

    const double heighttm   = 0.5; // IntegralAbs { 1.5pe * 9.6mV/pe } / IntegralRel { 0.5 }

    Long_t max  =    0;  // All
    Long_t max0 =  max;  // Time marker
    Long_t max1 =  max;  // Light pulser
    //Long_t max2 = 3000;  // Calibration ratio
    Long_t max3 =  max;  // Pedestal Rndm
    Long_t max4 =  max;  // Pedestal Ext
    Long_t max5 =  max;  // Data

    // ========================= Result ==================================

    //double scale = 0.1;
    double scale = 0.1024;

    // ======================================================

    if (map && gSystem->AccessPathName(map, kFileExists))
    {
        gLog << err << "ERROR - Cannot access mapping file '" << map << "'" << endl;
        return 1;
    }

    // The sequence file which defines the files for the analysis
    MSequence seq(seqfile);
    if (!seq.IsValid())
    {
        gLog << err << "ERROR - Sequence '" << seqfile << "' invalid!" << endl;
        return 2;
    }

    MBadPixelsCam badpixels;
    badpixels.InitSize(1440);
    badpixels[ 424].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 583].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 830].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 923].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[1208].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[1399].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    //  Twin pixel
    //     113
    //     115
    //     354
    //     423
    //    1195
    //    1393

    // broken drs-board:
    //   https://www.fact-project.org/logbook/showthread.php?tid=3521
    // in the analysis effect visible 10.1. - 25.5.2015
    // according to logbook:
    //   first report of problem:
    //   https://www.fact-project.org/logbook/showthread.php?tid=2772&pid=15756#pid15756
    //   repair action:
    //   https://www.fact-project.org/logbook/showthread.php?tid=3298
    // affected pixels:
    //   SW: 1193 1194 1195 1391 1392 1393 1304 1305 1306
    if (seq.GetNight().GetNightAsInt()>20141114 && seq.GetNight().GetNightAsInt()<20150526)
    {
        gLog << "Exclude broken drs-board..." << endl;
        badpixels[1193].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1194].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1195].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1391].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1392].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1393].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1304].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1305].SetUnsuitable(MBadPixelsPix::kUnsuitable);
        badpixels[1306].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    }

    // --------------------------------------------------------------------------------

    gLog.Separator("Callisto");
    gLog << all << "Calibrate data of sequence '" << seq.GetFileName() << "'" << endl;
    gLog << endl;

    // ------------------------------------------------------

    ostringstream drsname;
    drsname << gSystem->DirName(seqfile) << "/";
    drsname << seq.GetNight().GetNightAsInt() << "_";
    drsname << Form("%03d", seq.GetDrsSequence()) << ".drs.seq";

    MSequence drs(drsname.str().c_str());
    if (!drs.IsValid())
    {
        gLog << err << "ERROR - DRS sequence invalid!" << endl;
        return 3;
    }

    gLog << all << "DRS sequence file: " << drsname.str() << '\n' << endl;

    TString drsfile = seq.GetFileName(0, MSequence::kRawDrs);
    if (drsfile.IsNull())
    {
        gLog << err << "No DRS file available in sequence." << endl;
        return 4;
    }

    TString drs1024 = drs.GetFileName(0, MSequence::kFitsDrs);
    TString pedfile = seq.GetFileName(0, MSequence::kFitsPed);
    TString calfile = seq.GetFileName(0, MSequence::kFitsCal);

    gLog << all;
    gLog << "DRS calib     300: " << drsfile << '\n';
    gLog << "DRS calib    1024: " << drs1024 << "\n\n";
    gLog << "DRS calib    Time: " << drstime << "\n\n";

    MDrsCalibrationTime drscalibtime;
    if (!drscalibtime.ReadFits(drstime)))
        return 5;

    MDrsCalibration drscalib300;
    if (!drscalib300.ReadFits(drsfile.Data()))
        return 6;

    MDrsCalibration drscalib1024;
    if (!drscalib1024.ReadFits(drs1024.Data()))
        return 7;

    gLog << all;
    gLog << "Pedestal     file: " << pedfile << '\n';
    gLog << "Light Pulser file: " << calfile << '\n' << endl;

    // ------------------------------------------------------

    MDirIter iter;
    if (seq.GetRuns(iter, MSequence::kFitsDat)<=0)
    {
        gLog << err << "ERROR - Sequence valid but without files." << endl;
        return 8;
    }
    iter.Print();

    // ======================================================

/*
    MStatusArray arrt, arrp;

    TFile ft(lp_template);
    if (arrt.Read()<=0)
    {
        gLog << err << "ERROR - Reading LP template from " << lp_template << endl;
        return 100;
    }

    MHCamera *lpref = (MHCamera*)arrt.FindObjectInCanvas("ExtCalSig;avg", "MHCamera", "Cam");
    if (!lpref)
    {
        gLog << err << "ERROR - LP Template not found in " << lp_template << endl;
        return 101;
    }
    lpref->SetDirectory(0);

    MHCamera *gain = (MHCamera*)arrt.FindObjectInCanvas("gain", "MHCamera", "Gain");
    if (!gain)
    {
        gLog << err << "ERROR - Gain not found in " << lp_template << endl;
        return 101;
    }
    gain->SetDirectory(0);

    TFile fp(pulse_template);
    if (arrp.Read()<=0)
    {
        gLog << err << "ERROR - Reading Pulse template from " << pulse_template << endl;
        return 102;
    }

    TH1F *hpulse = (TH1F*)arrp.FindObjectInCanvas("hPixelEdgeMean0_0", "TH1F", "cgpPixelPulses0");
    if (!hpulse)
    {
        gLog << err << "ERROR - Pulse Template not found in " << pulse_template << endl;
        return 103;
    }
    hpulse->SetDirectory(0);
*/
    // ======================================================

    // Plot the trigger pattern rates vs. run-number
    MH3 hrate("MRawRunHeader.GetFileID", "MRawEvtHeader.GetTriggerID&0xff00");
    hrate.SetWeight("1./TMath::Max(MRawRunHeader.GetRunLength,1)");
    hrate.SetName("Rate");
    hrate.SetTitle("Event rate [Hz];File Id;Trigger Type;");
    hrate.InitLabels(MH3::kLabelsXY);
    hrate.DefineLabelY(    0, "Data"); // What if TriggerID==0 already???
    hrate.DefineLabelY(0x100, "Cal");
    hrate.DefineLabelY(0x400, "Ped");
    // hrate.DefaultLabelY("ERROR");

//    MDrsCalibrationTime timecam;

    gStyle->SetOptFit(kTRUE);

    // ======================================================
/*
    gLog << endl;
    gLog.Separator("Processing DRS timing calibration run");

    MTaskList tlist0;

    MParList plist0;
    plist0.AddToList(&tlist0);
    plist0.AddToList(&drscalib1024);
    plist0.AddToList(&timecam);

    MEvtLoop loop0("DetermineTimeCal");
    loop0.SetDisplay(d);
    loop0.SetParList(&plist0);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read0(timfile);

    MContinue cont0("MRawEvtHeader.GetTriggerID!=33792", "SelectTim");

    MGeomApply apply0;

    MDrsCalibApply drsapply0;
    //drsapply0.SetRemoveSpikes(4);

    MFillH fill0("MHDrsCalibrationTime");
    fill0.SetNameTab("DeltaT");

    tlist0.AddToList(&read0);
    tlist0.AddToList(&apply0);
    tlist0.AddToList(&drsapply0);
    tlist0.AddToList(&cont0);
    tlist0.AddToList(&fill0);

    if (!loop0.Eventloop(max0))
        return 8;

    if (!loop0.GetDisplay())
        return 9;
*/
    /*
     MHDrsCalibrationT *t = (MHDrsCalibrationT*)plist4.FindObject("MHDrsCalibrationT");
     t->SetDisplay(d);
     t->PlotAll();
     */

    // ======================================================

    gLog << endl;
    gLog.Separator("Processing external light pulser run");

    MTaskList tlist1;

    MParList plist1;
    plist1.AddToList(&tlist1);
    plist1.AddToList(&drscalib300);
    plist1.AddToList(&badpixels);
    plist1.AddToList(&drscalibtime);

    MEvtLoop loop1("DetermineCalConst");
    loop1.SetDisplay(d);
    loop1.SetParList(&plist1);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read1;
    read1.LoadMap(map);
    read1.AddFile(calfile);

    MContinue cont1("(MRawEvtHeader.GetTriggerID&0xff00)!=0x100", "SelectCal");

    MGeomApply apply1;

    MDrsCalibApply drsapply1;
    //drsapply1.SetRemoveSpikes(4);

    MFilterData filterdata1;

    // ---

    MExtractFACT extractor1b("ExtractPulse");
    extractor1b.SetRange(first_slice, last_slice);
    extractor1b.SetNoiseCalculation(kFALSE);

    // ---

    MHCamEvent evt1f(0, "ExtCalSig", "Extracted calibration signal;;S [mV·sl]");
    MHCamEvent evt1g(4, "ExtCalTm",  "Extracted arrival times;;T [sl]");
    MHCamEvent evt1h(6, "CalCalTm",  "Calibrated arrival times;;T [sl]");

    MHSectorVsTime hist1rmsb("ExtSigVsTm");
    MHSectorVsTime hist1tmb("CalTmVsTm");
    hist1rmsb.SetTitle("Extracted calibration vs event number;;S [mV·sl]");
    hist1rmsb.SetType(0);
    hist1tmb.SetTitle("Extracted arrival time vs event number;;T [sl]");
    //hist1tmb.SetType(4);
    hist1tmb.SetType(6);

    MFillH fill1f(&evt1f, "MExtractedSignalCam", "FillExtSig");
    MFillH fill1g(&evt1g, "MArrivalTimeCam",     "FillExtTm");
    MFillH fill1h(&evt1h, "MSignalCam",          "FillCalTm");
    MFillH fill1r(&hist1rmsb, "MExtractedSignalCam", "FillExtSigVsTm");
    //MFillH fill1j(&hist1tmb,  "MArrivalTimeCam",     "FillExtTmVsTm");
    MFillH fill1j(&hist1tmb,  "MSignalCam",     "FillCalTmVsTm");

    fill1f.SetDrawOption("gaus");
    fill1h.SetDrawOption("gaus");

    // ---

    MCalibrateDrsTimes calctm1a("CalibrateCalEvents");
    calctm1a.SetNameUncalibrated("UncalibratedTimes");

    MBadPixelsTreat treat1;
    treat1.SetProcessPedestalRun(kFALSE);
    treat1.SetProcessPedestalEvt(kFALSE);

    // ---

    MHCamEvent evt1c(6, "ExtCalTmShift", "Relative extracted arrival time of calibration pulse (w.r.t. event-median);;\\Delta T [ns]");
    MHCamEvent evt1d(6, "CalCalTmShift", "Relative calibrated arrival time of calibration pulse (w.r.t. event-median);;\\Delta T [ns]");

    evt1c.SetMedianShift();
    evt1d.SetMedianShift();

    MFillH fill1c(&evt1c, "UncalibratedTimes", "FillExtCalTm");
    MFillH fill1d(&evt1d, "MSignalCam",        "FillCalCalTm");
    fill1d.SetDrawOption("gaus");

    // ------------------ Setup eventloop and run analysis ---------------

    tlist1.AddToList(&read1);
    tlist1.AddToList(&apply1);
    tlist1.AddToList(&drsapply1);
    tlist1.AddToList(&cont1);
    tlist1.AddToList(&filterdata1);
    tlist1.AddToList(&extractor1b);
    tlist1.AddToList(&calctm1a);
    tlist1.AddToList(&treat1);
    tlist1.AddToList(&fill1f);
    tlist1.AddToList(&fill1g);
    tlist1.AddToList(&fill1h);
    tlist1.AddToList(&fill1r);
    tlist1.AddToList(&fill1j);
    tlist1.AddToList(&fill1c);
    tlist1.AddToList(&fill1d);

    //if (!loop1.Eventloop(max1))
    //    return 10;

    //if (!loop1.GetDisplay())
    //    return 11;

    if (delays)
    {
        TGraph g(delays);
        if (g.GetN()!=1440)
        {
            gLog << err << "Error reading file " << delays << endl;
            return 41;
        }

        drscalibtime.SetDelays(g);
    }

    // ======================================================

    gLog << endl;
    gLog.Separator("Extracting random pedestal");

    MTaskList tlist3;

    MParList plist3;
    plist3.AddToList(&tlist3);
    plist3.AddToList(&drscalib300);
    plist3.AddToList(&badpixels);
    plist3.AddToList(&drscalibtime);

    MEvtLoop loop3("DetermineRndmPed");
    loop3.SetDisplay(d);
    loop3.SetParList(&plist3);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read3;
    read3.LoadMap(map);
    read3.AddFile(pedfile);

    MFillH fill3a(&hrate);

    MContinue cont3("(MRawEvtHeader.GetTriggerID&0xff00)!=0x400", "SelectPed");

    MGeomApply apply3;

    MDrsCalibApply drsapply3;
    //drsapply3.SetRemoveSpikes(4);

    MFilterData filterdata3;

    //---

    MExtractFACT extractor3;
    extractor3.SetRange(first_slice, last_slice);
    extractor3.SetNoiseCalculation(kTRUE);

    MCalibrateFact conv3;
    conv3.SetScale(scale);
    //conv3.SetCalibConst(calib);

    MBadPixelsTreat treat3;
    treat3.SetProcessPedestalRun(kFALSE);
    treat3.SetProcessPedestalEvt(kFALSE);
    treat3.SetProcessTimes(kFALSE);

    MHCamEvent evt3b(0, "PedRdm","Interpolated random pedestal;;Signal [~phe]");
    //evt2b.SetErrorSpread(kFALSE);

    MFillH fill3b(&evt3b, "MSignalCam", "FillPedRdm");
    fill3b.SetDrawOption("gaus");

    // ------------------ Setup eventloop and run analysis ---------------

    tlist3.AddToList(&read3);
    tlist3.AddToList(&apply3);
    tlist3.AddToList(&drsapply3);
    tlist3.AddToList(&cont3);
    tlist3.AddToList(&filterdata3);
    tlist3.AddToList(&extractor3);
//    tlist3.AddToList(&fill3a);
    tlist3.AddToList(&conv3);
    tlist3.AddToList(&treat3);
    tlist3.AddToList(&fill3b);

    if (!loop3.Eventloop(max3))
        return 14;

    if (!loop3.GetDisplay())
        return 15;

    // ======================================================

    gLog << endl;
    gLog.Separator("Extracting pedestal");

    MTaskList tlist4;

    MParList plist4;
    plist4.AddToList(&tlist4);
    plist4.AddToList(&drscalib300);
    plist4.AddToList(&badpixels);
    plist4.AddToList(&drscalibtime);

    MEvtLoop loop4("DetermineExtractedPed");
    loop4.SetDisplay(d);
    loop4.SetParList(&plist4);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read4;
    read4.LoadMap(map);
    read4.AddFile(pedfile);

    MContinue cont4("(MRawEvtHeader.GetTriggerID&0xff00)!=0x400", "SelectPed");

    MGeomApply apply4;

    MDrsCalibApply drsapply4;
    //drsapply4.SetRemoveSpikes(4);

    MFilterData filterdata4;

    MExtractFACT extractor4;
    extractor4.SetRange(first_slice, last_slice);
    extractor4.SetNoiseCalculation(kFALSE);

    MCalibrateFact conv4;
    conv4.SetScale(scale);
    //conv4.SetCalibConst(calib);

    MBadPixelsTreat treat4;
    treat4.SetProcessPedestalRun(kFALSE);
    treat4.SetProcessPedestalEvt(kFALSE);

    MHCamEvent evt4b(0, "PedExt","Interpolated extracted pedestal;;Signal [~phe]");
    //evt4b.SetErrorSpread(kFALSE);

    MFillH fill4b(&evt4b, "MSignalCam", "FillPedExt");
    fill4b.SetDrawOption("gaus");

    // ------------------ Setup eventloop and run analysis ---------------

    tlist4.AddToList(&read4);
    tlist4.AddToList(&apply4);
    tlist4.AddToList(&drsapply4);
    tlist4.AddToList(&cont4);
    tlist4.AddToList(&filterdata4);
    tlist4.AddToList(&extractor4);
    tlist4.AddToList(&conv4);
    tlist4.AddToList(&treat4);
    tlist4.AddToList(&fill4b);

    if (!loop4.Eventloop(max4))
        return 15;

    if (!loop4.GetDisplay())
        return 16;

    // ===================================================================

    gLog << endl;
    gLog.Separator("Extracting and calibration data");

    MTaskList tlist5;

    MParList plist5;
    plist5.AddToList(&tlist5);
    plist5.AddToList(&drscalib300);
    plist5.AddToList(&badpixels);
    plist5.AddToList(&drscalibtime);

    MEvtLoop loop5("CalibratingData");
    loop5.SetDisplay(d);
    loop5.SetParList(&plist5);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read5;
    read5.LoadMap(map);
    read5.AddFiles(iter);

    MFillH fill5a(&hrate);

    MGeomApply apply5;

    MDrsCalibApply drsapply5;
    //drsapply5.SetRemoveSpikes(4);

    MTreatSaturation treatsat5;

    MFilterData filterdata5;

    MFDataPhrase filterdat("(MRawEvtHeader.GetTriggerID&0xff00)==0",     "SelectDat");
    MFDataPhrase filtercal("(MRawEvtHeader.GetTriggerID&0xff00)==0x100", "SelectCal");
    MFDataPhrase filterped("(MRawEvtHeader.GetTriggerID&0xff00)==0x400", "SelectPed");
    MFDataPhrase filterncl("(MRawEvtHeader.GetTriggerID&0xff00)!=0x100", "SelectNonCal");

    //MContinue cont4("MRawEvtHeader.GetTriggerID!=4", "SelectData");

    // ---

    MExtractFACT extractor5dat;
    extractor5dat.SetRange(first_slice, last_slice);
    extractor5dat.SetNoiseCalculation(kFALSE);

    MExtractFACT extractor5cal;
    extractor5cal.SetRange(first_slice, last_slice);
    extractor5cal.SetNoiseCalculation(kFALSE);

    MExtractFACT extractor5tm("ExtractTM");
    extractor5tm.SetRange(last_slice, 294);
    extractor5tm.SetNoiseCalculation(kFALSE);
    extractor5tm.SetNameSignalCam("TimeMarkerAmplitude");
    extractor5tm.SetNameTimeCam("TimeMarkerTime");

    extractor5dat.SetFilter(&filterncl);
    extractor5cal.SetFilter(&filtercal);
    //extractor4tm.SetFilter(&filtercal);

    // ---
    MCalibrateFact conv5;
    conv5.SetScale(scale);
    //conv5.SetCalibConst(calib);

    MCalibrateDrsTimes calctm5;
    calctm5.SetNameUncalibrated("UncalibratedTimes");

    MCalibrateDrsTimes calctm5tm("CalibrateTimeMarker");
    calctm5tm.SetNameArrivalTime("TimeMarkerTime");
    calctm5tm.SetNameUncalibrated("UncalTimeMarker");
    calctm5tm.SetNameCalibrated("TimeMarker");
    calctm5tm.SetTimeMarker();
    //calctm4tm.SetFilter(&filtercal);

    MHCamEvent evt5m(6, "ExtTm",      "Extracted arrival times of calibration pulse;;\\Delta T [ns]");
    MHCamEvent evt5n(6, "CalTm",      "Calibrated arrival times of calibration pulse;;\\Delta T [ns]");
    MHCamEvent evt5q(6, "ExtTmShift", "Relative extracted arrival times of calibration pulse (w.r.t. event-median);;\\Delta T [ns]");
    MHCamEvent evt5r(6, "CalTmShift", "Relative calibrated arrival times of calibration pulse (w.r.t. event-median);;\\Delta T [ns]");
    MHCamEvent evt5s(6, "ExtTM",      "Extracted absolute time marker position;;T [sl]");
    MHCamEvent evt5t(6, "CalTM",      "Calibrated absolute time marker position;;T [ns]");
    MHCamEvent evt5u(6, "ExtTMshift", "Relative extracted time marker position (w.r.t. event-median);;\\Delta T [ns]");
    MHCamEvent evt5v(6, "CalTMshift", "Relative calibrated time marker position (w.r.t. event-median);;\\Delta T [ns]");
    MHCamEvent evt5w(6, "ExtDiff",    "Difference between extracted arrival time of time marker and calibration pulse;;\\Delta T [ns]");
    MHCamEvent evt5x(6, "CalDiff",    "Difference between calibrated arrival time of time marker and calibration pulse;;\\Delta T [ns]");

    evt5w.SetNameSub("UncalibratedTimes");
    evt5x.SetNameSub("MSignalCam");

    evt5q.SetMedianShift();
    evt5r.SetMedianShift();
    evt5u.SetMedianShift();
    evt5v.SetMedianShift();
    //evt4w.SetMedianShift();
    //evt4x.SetMedianShift();

    MFillH fill5m(&evt5m, "UncalibratedTimes", "FillExtTm");
    MFillH fill5n(&evt5n, "MSignalCam",        "FillCalTm");
    MFillH fill5q(&evt5q, "UncalibratedTimes", "FillExtTmShift");
    MFillH fill5r(&evt5r, "MSignalCam"       , "FillCalTmShift");
    MFillH fill5s(&evt5s, "UncalTimeMarker",   "FillExtTM");
    MFillH fill5t(&evt5t, "TimeMarker",        "FillCalTM");
    MFillH fill5u(&evt5u, "UncalTimeMarker",   "FillExtTMshift");
    MFillH fill5v(&evt5v, "TimeMarker",        "FillCalTMshift");
    MFillH fill5w(&evt5w, "UncalTimeMarker",   "FillExtDiff");
    MFillH fill5x(&evt5x, "TimeMarker",        "FillCalDiff");

    fill5m.SetDrawOption("gaus");
    fill5n.SetDrawOption("gaus");
    fill5q.SetDrawOption("gaus");
    fill5r.SetDrawOption("gaus");
    //fill5s.SetDrawOption("gaus");
    //fill5t.SetDrawOption("gaus");
    //fill5u.SetDrawOption("gaus");
    //fill5v.SetDrawOption("gaus");
    //fill5w.SetDrawOption("gaus");
    //fill5x.SetDrawOption("gaus");


    MBadPixelsTreat treat5;
    treat5.SetProcessPedestalRun(kFALSE);
    treat5.SetProcessPedestalEvt(kFALSE);

    MHSectorVsTime hist5cal("CalVsTm");
    MHSectorVsTime hist5ped("PedVsTm");
    hist5cal.SetTitle("Median calibrated calibration signal vs event number;;Signal [~phe]");
    hist5ped.SetTitle("Median calibrated pedestal signal vs event number;;Signal [~phe]");
    hist5cal.SetType(0);
    hist5ped.SetType(0);
    hist5cal.SetMinimum(0);
    hist5ped.SetMinimum(0);
    hist5cal.SetUseMedian();
    hist5ped.SetUseMedian();
    hist5cal.SetNameTime("MTime");
    hist5ped.SetNameTime("MTime");

    MFillH fill5cal(&hist5cal, "MSignalCam", "FillCalVsTm");
    MFillH fill5ped(&hist5ped, "MSignalCam", "FillPedVsTm");
    fill5cal.SetFilter(&filtercal);
    fill5ped.SetFilter(&filterped);

    MHCamEvent evt5b(0, "ExtSig",   "Extracted signal;;S [mV·sl]");
    MHCamEvent evt5c(0, "CalSig",   "Calibrated and interpolated signal;;S [~phe]");
    MHCamEvent evt5d(4, "ExtSigTm", "Extracted time;;T [sl]");
    MHCamEvent evt5e(6, "CalSigTm", "Calibrated and interpolated time;;T [ns]");

    MFillH fill5b(&evt5b, "MExtractedSignalCam", "FillExtSig");
    MFillH fill5c(&evt5c, "MSignalCam",          "FillCalSig");
    MFillH fill5d(&evt5d, "MArrivalTimeCam",     "FillExtTm");
    MFillH fill5e(&evt5e, "MSignalCam",          "FillCalTm");

    fill5c.SetDrawOption("gaus");
    fill5d.SetDrawOption("gaus");
    fill5e.SetDrawOption("gaus");

    /*
    fill4b.SetFilter(&filterdat);
    fill4c.SetFilter(&filterdat);
    fill4d.SetFilter(&filterdat);
    fill4e.SetFilter(&filterdat);
    */

    //MFSoftwareTrigger swtrig;
    //MContinue contsw(&swtrig, "FilterSwTrigger", "Software trigger");
    //contsw.SetInverted();

    const TString fname(Form("s/([0-9]+_[0-9]+)[.]fits([.][fg]z)?$/%s\\/$1_C.root/",
                             MJob::Esc(outpath).Data()));

    // The second rule is for the case reading raw-files!
    MWriteRootFile write5(2, fname, "RECREATE", "Calibrated Data");
    write5.AddContainer("MRawRunHeader", "RunHeaders");
    write5.AddContainer("MGeomCam",      "RunHeaders");
    write5.AddContainer("MSignalCam",    "Events");
    write5.AddContainer("MTime",         "Events");
    write5.AddContainer("MRawEvtHeader", "Events");
    //write.AddContainer("MTriggerPattern", "Events");

    // ------------------ Setup histograms and fill tasks ----------------

    MContinue test;
    test.SetFilter(&filterncl);

    MTaskList tlist5tm;
    tlist5tm.AddToList(&extractor5tm);
    tlist5tm.AddToList(&calctm5tm);
    tlist5tm.AddToList(&fill5m);
    tlist5tm.AddToList(&fill5n);
    tlist5tm.AddToList(&fill5q);
    tlist5tm.AddToList(&fill5r);
    //tlist5tm.AddToList(&fill5s);
    //tlist5tm.AddToList(&fill5t);
    tlist5tm.AddToList(&fill5u);
    tlist5tm.AddToList(&fill5v);
    tlist5tm.AddToList(&fill5w);
    tlist5tm.AddToList(&fill5x);
    tlist5tm.SetFilter(&filtercal);

    MTaskList tlist5dat;
    tlist5dat.AddToList(&fill5b);
    tlist5dat.AddToList(&fill5c);
    tlist5dat.AddToList(&fill5d);
    tlist5dat.AddToList(&fill5e);
    tlist5dat.SetFilter(&filterdat);

    tlist5.AddToList(&read5);
    tlist5.AddToList(&apply5);
    tlist5.AddToList(&drsapply5);
    tlist5.AddToList(&filterncl);
    //tlist5.AddToList(&test);
    tlist5.AddToList(&filterdat);
    tlist5.AddToList(&filtercal);
    tlist5.AddToList(&filterped);
    tlist5.AddToList(&fill5a);
    tlist5.AddToList(&treatsat5);
    tlist5.AddToList(&filterdata5);
    tlist5.AddToList(&extractor5dat);
    tlist5.AddToList(&extractor5cal);
    tlist5.AddToList(&calctm5);
    tlist5.AddToList(&tlist5tm);
    tlist5.AddToList(&conv5);
    tlist5.AddToList(&treat5);
    tlist5.AddToList(&fill5ped);
    tlist5.AddToList(&fill5cal);
    tlist5.AddToList(&tlist5dat);
    tlist5.AddToList(&write5);

    if (!loop5.Eventloop(max5))
        return 18;

    if (!loop5.GetDisplay())
        return 19;

    TString title = "--  Calibrated signal #";
    title += seq.GetSequence();
    title += " (";
    title += drsfile;
    title += ")  --";
    d->SetTitle(title, kFALSE);

    TString path;
    path += Form("%s/20%6d_%03d-calibration.root", outpath,
                 seq.GetSequence()/1000, seq.GetSequence()%1000);

    d->SaveAs(path);

    return 0;
}
