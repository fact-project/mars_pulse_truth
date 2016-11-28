int drscal(const char *drsname="~dorner/mysequences/2012/02/28/20120228_200.drs.seq", const char *outpath = "output")
{
    // ======================================================

    const char *map = "/scratch_nfs/FACTmap111030.txt";

    // ======================================================

    if (map && gSystem->AccessPathName(map, kFileExists))
    {
        gLog << "ERROR - Cannot access mapping file '" << map << "'" << endl;
        return 1;
    }

    // The sequence file which defines the files for the analysis
    MSequence drs(drsname);
    if (!drs.IsValid())
    {
        gLog << "ERROR - Sequence '" << drsname << "' invalid!" << endl;
        return 2;
    }

    // DrsRuns 1
    // CalRuns 1
    // DrsFiles 1 [1024], PedRuns 1 [300]

    // --------------------------------------------------------------------------------

    gLog.Separator("DRS Calibration");
    gLog << "Calculate drs calibration constants of drs sequence:\n" << drs.GetFileName() << endl;
    gLog << endl;

    // ------------------------------------------------------

    TString drsped  = drs.GetFileName(0, MSequence::kFitsDrsRuns);
    TString drsgain = drs.GetFileName(0, MSequence::kFitsCal);
    TString drs1024 = drs.GetFileName(1, MSequence::kFitsDrsRuns);
    TString drs300  = drs.GetFileName(0, MSequence::kFitsPed);

    gLog << "DRS ped  1024: " << drsped   << '\n';
    gLog << "DRS gain 1024: " << drsgain  << '\n';
    gLog << "DRS ped  1024: " << drs1024  << "\n";
    gLog << "DRS ped   300: " << drs300   << endl;

    // ======================================================

    MStatusDisplay *d = new MStatusDisplay;

    MParList plist;

    MHDrsCalibration hist;
    plist.AddToList(&hist);

    hist.SetOutputPath(outpath);

    MTaskList tlist;
    plist.AddToList(&tlist);

    MEvtLoop loop;
    loop.SetParList(&plist);
    loop.SetDisplay(d);

    // --------------------------------------------------------

    gLog.Separator("Calculating cell offset");

    MRawFitsRead read1(drsped);
    read1.LoadMap(map);

    MGeomApply apply;

    tlist.AddToList(&read1);
    tlist.AddToList(&apply);

    MFillH fill("MHDrsCalibration");
    fill.SetNameTab("Ped1");

    tlist.AddToList(&fill);

    if (!loop.Eventloop())
        return 3;

    if (!loop.GetDisplay())
        return 4;

    // --------------------------------------------------------

    gLog.Separator("Calculating cell gain");

    MRawFitsRead read2(drsgain);
    read2.LoadMap(map);
    tlist.Replace(&read2);
    fill.SetNameTab("Gain");

    if (!loop.Eventloop())
        return 5;

    if (!loop.GetDisplay())
        return 6;

    // --------------------------------------------------------

    gLog.Separator("Calculating drs constants for roi=1024");

    fill.SetNameTab("Ped2-1024");

    // I think one HAS to use the same file than for step one
    // this is the only way to get rid of a systematic shift.
    MRawFitsRead read3(drs1024);
    read3.LoadMap(map);
    tlist.Replace(&read3);

    if (!loop.Eventloop())
        return 7;

    if (!loop.GetDisplay())
        return 8;

    // --------------------------------------------------------

    gLog.Separator("Calculating drs constants for roi=300");

    fill.SetNameTab("Ped2-300");

    if (!hist.ResetSecondaryBaseline())
    {
        cout << "ERROR - Reset failed." << endl;
        return 100;
    }

    // This would just result in a wrong consistency check for the roi
    plist.Remove((MParContainer*)plist.FindObject("MRawRunHeader"));
    plist.Remove((MParContainer*)plist.FindObject("MRawEvtData"));

    // I think one HAS to use the same file than for step one
    // this is the only way to get rid of a systematic shift.
    MRawFitsRead read4(drs300);
    read4.LoadMap(map);
    tlist.Replace(&read4);

    if (!loop.Eventloop())
        return 9;

    if (!loop.GetDisplay())
        return 10;

    // --------------------------------------------------------

    TString title = "--  DRS Calibration #";
    title += drs.GetSequence();
    title += " (";
    title += drsname;
    title += ")  --";
    d->SetTitle(title, kFALSE);

    TString path;
    path += Form("%s/20%6d_%03d-drs.root", outpath,
                 drs.GetSequence()/1000, drs.GetSequence()%1000);

    d->SaveAs(path);

    return 0;

}
