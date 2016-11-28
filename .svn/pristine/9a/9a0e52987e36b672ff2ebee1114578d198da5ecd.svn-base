int underflow(const char *filename="/raid10/raw/2013/06/21/20130621_062.fits", const char *outname="underflow-check.root")
{
    // The input file must be a drs-gain file
    // The return value is
    //  0) Success
    //  1) Underflow detected
    //  3) Event loop stopped
    //  4) Display closed
    // ======================================================

    gLog.Separator("Underflow check");

    // ------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;

    MParList plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    MEvtLoop loop;
    loop.SetParList(&plist);
    loop.SetDisplay(d);

    // --------------------------------------------------------

    MRawFitsRead read1(filename);

    MGeomApply apply;

    tlist.AddToList(&read1);
    tlist.AddToList(&apply);

    MHDrsCalibration hcalib;

    MFillH fill(&hcalib);
    fill.SetNameTab("Check");

    tlist.AddToList(&fill);

    if (!loop.Eventloop())
        return 3;

    if (!loop.GetDisplay())
        return 4;

    // --------------------------------------------------------

    TString title = "--  Underflow check ";
    title += " (";
    title += filename;
    title += ")  --";
    d->SetTitle(title, kFALSE);
    d->SaveAs(outname);

    // --------------------------------------------------------

    return hcalib.GetNumUnderflows(1.3)>0 ? 1 : 0;
}
