void plotlpsimple()
{
    MParList plist;

    MDrsCalibration calib;
    if (!calib.ReadFits("/loc_data/raw/2011/10/30/20111030_023.drs.fits"))
        return;
    plist.AddToList(&calib);

    MTaskList tlist;
    plist.AddToList(&tlist);

    MRawFitsRead read1;
    read1.AddFile("/loc_data/raw/2011/10/30/20111030_028.fits");
    read1.LoadMap("../FACT++/FACTmap111030.txt");
    tlist.AddToList(&read1);

    MGeomApply apply;
    tlist.AddToList(&apply);

    MDrsCalibApply drscal;
    tlist.AddToList(&drscal);

    MExtractTimeAndChargeSpline extractor;
    extractor.SetRange(180, 240);   // 0, 1023
    extractor.SetRiseTimeHiGain(13);
    extractor.SetFallTimeHiGain(23);
    extractor.SetChargeType(MExtralgoSpline::kIntegralRel); // 1.4V-1.8V   T=290
    extractor.SetSaturationLimit(600000);
    extractor.SetNoiseCalculation(kFALSE);
    tlist.AddToList(&extractor);

    MHCamEvent evt2(0, "Extra'd", "Extracted Calibration Signal;;S [cnts/sl]");
    MHCamEvent evt9(4, "ArrTm",   "Extracted ArrivalTime;;T");

    MHSectorVsTime histrms;
    MHSectorVsTime histtm;
    histrms.SetTitle("Pulser Amplitude (maximum extracted with spline);;Amplitude [mV]");
    histrms.SetType(0);
    histtm.SetTitle("Pulser arrival bin (leading edge extracted with spline);;Arrival time [slice]");
    histtm.SetType(0);

    MFillH fill2(&evt2,      "MExtractedSignalCam", "FillExtractedSignal");
    MFillH fill9(&evt9,      "MArrivalTimeCam",     "FillArrivalTime");
    MFillH fillrms(&histrms, "MExtractedSignalCam", "FillAmplVsTime");
    MFillH filltm(&histtm,   "MArrivalTimeCam",     "FillATVsTime");

    tlist.AddToList(&fill2);
    tlist.AddToList(&fill9);
    tlist.AddToList(&fillrms);
    tlist.AddToList(&filltm);

    MStatusDisplay *d = new MStatusDisplay;

    MEvtLoop loop;
    loop.SetParList(&plist);
    loop.SetDisplay(d);
    loop.Eventloop();
}
