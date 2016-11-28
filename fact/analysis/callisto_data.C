#include "MLogManip.h"

int callisto_data(const char *datafile, const char *drsfile,
                  const char *drstime, const char *delays,
                  const char *outpath=".")
{
    // ======================================================

    // true:  Display correctly mapped pixels in the camera displays
    //        but the value-vs-index plot is in software/spiral indices
    // false: Display pixels in hardware/linear indices,
    //        but the order is the camera display is distorted
    bool usemap = drstime ? true : false;

    // map file to use (get that from La Palma!)
    const char *mmap = usemap ? "FACTmap111030.txt" : NULL;

    //const char *pulse_template = "template-pulse.root";

    // ------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;

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

    // ------------------------------------------------------

    // nominal gain as extracted with gain extraction
    // That is the number which describes
    // the pulse amplitude in the file!
    double gain = 241.;

    // integrate around maximum or from leading edge on?
    const bool maximum = true;

    // ------------------------------------------------------

    // Calib: 51 / 90 / 197 (20% TH)
    // Data:  52 / 64 / 104 (20% TH)

    // Extraction range in slices. It will always(!) contain the full range
    // of integration
    const int first_slice =  20; //  10ns
    const int last_slice  = 250; // 125ns

    // Note that rise and fall time mean different things whether you use IntegralFixed or IntegraRel:
    //
    //  IntegralFixed:
    //    * fRiseTime: Number of slices left  from arrival time
    //    * fFallTime: Number of slices right from arrival time
    //  IntegralRel:
    //    * fRiseTime: Number of slices left  from maximum time
    //    * fFallTime: Number of slices right from maximum time
    //
    const int rise_time_cal = maximum ?  40 :  10;
    const int fall_time_cal = maximum ? 120 : 160;

    const int rise_time_dat = maximum ?   5 :   0; 
    const int fall_time_dat = maximum ?  25 :  30; 

    // Extraction type: Extract integral and half leading edge

    const int type = maximum ? (MExtralgoSpline::kIntegralRel) : (MExtralgoSpline::kIntegralFixed);
    //const int type = MExtralgoSpline::kIntegralFixed;

    const double heighttm = 0.5; // IntegralAbs { 1.5pe * 9.6mV/pe } / IntegralRel { 0.5 }

    Long_t max = 0;  // All

    // ======================================================

    if (mmap && gSystem->AccessPathName(mmap, kFileExists))
    {
        gLog << err << "ERROR - Cannot access mapping file '" << mmap << "'" << endl;
        return 11;
    }

    // -------------------------------------------------------

    MDrsCalibrationTime timecam;
    if (drstime && !timecam.ReadFits(drstime))
    {
        gLog << err << "ERROR - Could not get MDrsCalibrationTime from " << drstime << endl;
        return 21;
    }

    if (delays)
    {
        TGraph g(delays);
        if (g.GetN()!=1440)
        {
            gLog << err << "Error reading file " << delays << endl;
            return 22;
        }

        timecam.SetDelays(g);
    }

    // ------------------------------------------------------

    MDrsCalibration drscalib300;
    if (drsfile && !drscalib300.ReadFits(drsfile))
        return 31;

    // -------------------------------------------------------

    if (drstime && delays)
    {
        TGraph g(delays);
        if (g.GetN()!=1440)
        {
            gLog << err << "Error reading file " << delays << endl;
            return 41;
        }

        timecam.SetDelays(g);
    }

    // -------------------------------------------------------

    // Range in which function/spline is defined
    int minx = -10;
    int maxx = 125;

    // Number of points
    int N = (maxx-minx)*2; // two points per nanosecond (2GHz)

    TF1 f("pulse", "[0]*(1-exp(-[1]*x*x))*exp(-[2]*x)*(x>0)", float(minx), float(maxx));
    f.SetNpx(N);

    // Normalize pulse
    f.SetParameters(1.9*gain/30, 0.10, 0.053);

    // Convert to graph
    TGraph g(&f);

    // Convert to float
    MArrayF data(g.GetN());
    for (int i=0; i<g.GetN(); i++)
        data[i] = g.GetY()[i];

    // Define spline
    MArrayF der1(g.GetN());
    MArrayF der2(g.GetN());

    MExtralgoSpline spline(data.GetArray(), data.GetSize(),
                           der1.GetArray(), der2.GetArray());

    spline.SetRiseFallTime(rise_time_dat, fall_time_dat);
    spline.SetExtractionType(type);
    spline.SetHeightTm(heighttm);

    // Estimate where the maximum is and extract signal
    Long64_t maxi = TMath::LocMax(g.GetN(), g.GetY());
    spline.Extract(maxi);

    // Scale factor for signal extraction
    double scale = 1./spline.GetSignal();

    // ======================================================

    gLog.Separator("Callisto");
    gLog << all;
    gLog << "Data file: " << datafile << '\n';
    if (drsfile)
        gLog << "DRS 300:   " << drsfile  << '\n';
    if (drstime)
        gLog << "DRS Time:  " << drstime  << '\n';
    if (delays)
        gLog << "Delays:    " << delays   << '\n';
    gLog << "Outpath:   " << outpath  << '\n';
    gLog << endl;

    // ------------------------------------------------------

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

    gStyle->SetOptFit(kTRUE);

    // ===================================================================

    gLog << endl;
    gLog.Separator("Calibrating data");

    MTaskList tlist5;

    MParList plist5;
    plist5.AddToList(&tlist5);
    plist5.AddToList(&badpixels);
    if (drsfile)
        plist5.AddToList(&drscalib300);
    if (drstime)
        plist5.AddToList(&timecam);

    MEvtLoop loop5("CalibratingData");
    loop5.SetDisplay(d);
    loop5.SetParList(&plist5);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read5(datafile);
    if (mmap)
        read5.LoadMap(mmap);

    MFillH fill5a(&hrate);

    MGeomApply apply5;

    MDrsCalibApply drsapply5;

    MFDataPhrase filterdat("(MRawEvtHeader.GetTriggerID&0xff00)==0",     "SelectDat");
    MFDataPhrase filtercal("(MRawEvtHeader.GetTriggerID&0xff00)==0x100", "SelectCal");
    MFDataPhrase filterped("(MRawEvtHeader.GetTriggerID&0xff00)==0x400", "SelectPed");
    MFDataPhrase filterncl("(MRawEvtHeader.GetTriggerID&0xff00)!=0x100", "SelectNonCal");

    //MContinue cont4("MRawEvtHeader.GetTriggerID!=4", "SelectData");

    // ---

    MExtractTimeAndChargeSpline extractor5dat;
    extractor5dat.SetRange(first_slice, last_slice);
    extractor5dat.SetRiseTimeHiGain(rise_time_dat);
    extractor5dat.SetFallTimeHiGain(fall_time_dat);
    extractor5dat.SetHeightTm(heighttm);
    extractor5dat.SetChargeType(type);
    extractor5dat.SetSaturationLimit(600000);
    extractor5dat.SetNoiseCalculation(kFALSE);

    MExtractTimeAndChargeSpline extractor5cal;
    extractor5cal.SetRange(first_slice, last_slice);
    extractor5cal.SetRiseTimeHiGain(rise_time_cal);
    extractor5cal.SetFallTimeHiGain(fall_time_cal);
    extractor5cal.SetHeightTm(heighttm);
    extractor5cal.SetChargeType(type);
    extractor5cal.SetSaturationLimit(600000);
    extractor5cal.SetNoiseCalculation(kFALSE);

    MExtractTimeAndChargeSpline extractor5tm("ExtractTM");
    extractor5tm.SetRange(last_slice, 294);
    extractor5tm.SetRiseTimeHiGain(1);
    extractor5tm.SetFallTimeHiGain(1);
    extractor5tm.SetHeightTm(0.5);
    extractor5tm.SetChargeType(MExtralgoSpline::kAmplitudeRel);
    extractor5tm.SetSaturationLimit(600000);
    extractor5tm.SetNoiseCalculation(kFALSE);
    extractor5tm.SetNameSignalCam("TimeMarkerAmplitude");
    extractor5tm.SetNameTimeCam("TimeMarkerTime");

    //extractor5dat.SetFilter(&filterncl);
    //extractor5cal.SetFilter(&filtercal);
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
    //MFillH fill5s(&evt5s, "UncalTimeMarker",   "FillExtTM");
    //MFillH fill5t(&evt5t, "TimeMarker",        "FillCalTM");
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

    TString fname = gSystem->ConcatFileName(outpath, gSystem->BaseName(datafile));
    fname.ReplaceAll(".fits.gz", "_C.root");
    fname.ReplaceAll(".fits.fz", "_C.root");
    fname.ReplaceAll(".fits",    "_C.root");

    // The second rule is for the case reading raw-files!
    MWriteRootFile write5(fname, "RECREATE", "Calibrated Data", 2);
    write5.AddContainer("MRawRunHeader", "RunHeaders");
    write5.AddContainer("MGeomCam",      "RunHeaders");
    write5.AddContainer("MSignalCam",    "Events");
    write5.AddContainer("MTime",         "Events");
    write5.AddContainer("MRawEvtHeader", "Events");

    write5.AddContainer("MMcCorsikaRunHeader", "RunHeaders", kFALSE);
    write5.AddContainer("MCorsikaRunHeader",   "RunHeaders", kFALSE);
    write5.AddContainer("MMcRunHeader",        "RunHeaders", kFALSE);
    write5.AddContainer("MCorsikaEvtHeader",   "Events", kFALSE);
    write5.AddContainer("MMcEvt",              "Events", kFALSE);
    write5.AddContainer("IncidentAngle",       "Events", kFALSE);
    write5.AddContainer("MPointingPos",        "Events", kFALSE);
    write5.AddContainer("MSimSourcePos",       "Events", kFALSE);
    write5.AddContainer("MTime",               "Events", kFALSE);
    write5.AddContainer("MSrcPosCam",          "Events", kFALSE);


    //write.AddContainer("MTriggerPattern", "Events");

    // ------------------ Setup histograms and fill tasks ----------------

    //MTaskList tlist5tm;
    //tlist5tm.AddToList(&extractor5tm);
    //tlist5tm.AddToList(&calctm5tm);
    //tlist5tm.AddToList(&fill5m);
    //tlist5tm.AddToList(&fill5n);
    //tlist5tm.AddToList(&fill5q);
    //tlist5tm.AddToList(&fill5r);
    //tlist5tm.AddToList(&fill5s);
    //tlist5tm.AddToList(&fill5t);
    //tlist5tm.AddToList(&fill5u);
    //tlist5tm.AddToList(&fill5v);
    //tlist5tm.AddToList(&fill5w);
    //tlist5tm.AddToList(&fill5x);
    //tlist5tm.SetFilter(&filtercal);

    //MTaskList tlist5dat;
    //tlist5dat.AddToList(&fill5b);
    //tlist5dat.AddToList(&fill5c);
    //tlist5dat.AddToList(&fill5d);
    //tlist5dat.AddToList(&fill5e);
    //tlist5dat.SetFilter(&filterdat);

    tlist5.AddToList(&read5);
    tlist5.AddToList(&apply5);
    tlist5.AddToList(&drsapply5);
    //tlist5.AddToList(&filterncl);
    //tlist5.AddToList(&filterdat);
    //tlist5.AddToList(&filtercal);
    //tlist5.AddToList(&filterped);
    //tlist5.AddToList(&fill5a);
    tlist5.AddToList(&extractor5dat);
    //tlist5.AddToList(&extractor5cal);
    tlist5.AddToList(&calctm5);
    //tlist5.AddToList(&tlist5tm);
    tlist5.AddToList(&conv5);
    tlist5.AddToList(&treat5);
    //tlist5.AddToList(&fill5ped);
    //tlist5.AddToList(&fill5cal);
    //tlist5.AddToList(&tlist5dat);
    tlist5.AddToList(&write5);

    if (!loop5.Eventloop(max))
        return 18;

    if (!loop5.GetDisplay())
        return 19;

    // ============================================================

    TFile *ofile = gROOT->GetListOfFiles()->FindObject(fname);
    if (!ofile || !ofile->IsOpen() || ofile->IsZombie())
    {
        gLog << err << "File " << fname << " not found" << endl;
        return 20;
    }

    TString title = "--  Calibrated signal [";
    title += datafile;
    title += "] --";
    d->SetTitle(title, kFALSE);

    ofile->cd();
    d->Write();

    return 0;
}
