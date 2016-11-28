#include "MLogManip.h"

int callisto_drstime(const char *timfile, const char *drs1024, const char *outfile="callisto-drstime.root")
{
    // ======================================================

    // Maximum number of events to be procesed
    Long_t max = 0;

    // ======================================================

    MDrsCalibration drscalib1024;
    if (!drscalib1024.ReadFits(drs1024))
        return 52;

    // ======================================================

    MStatusDisplay *d = new MStatusDisplay;

    // ======================================================

    gLog.Separator("Callisto");
    gLog << all;
    gLog << "DRS Timing " << timfile << '\n';
    gLog << "DRS 1024   " << drs1024 << '\n';
    gLog << endl;


    MDrsCalibrationTime timecam;

    gStyle->SetOptFit(kTRUE);

    // ======================================================

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

    MFillH fill0("MHDrsCalibrationTime");
    fill0.SetNameTab("DeltaT");

    tlist0.AddToList(&read0);
    tlist0.AddToList(&apply0);
    tlist0.AddToList(&drsapply0);
    tlist0.AddToList(&cont0);
    tlist0.AddToList(&fill0);

    if (!loop0.Eventloop(max))
        return 8;

    if (!loop0.GetDisplay())
        return 9;

    //MHDrsCalibrationTime *t = (MHDrsCalibrationTime*)plist0.FindObject("MHDrsCalibrationTime");
    //t->SetDisplay(d);
    //t->PlotAll();

    // ======================================================

    TString title = "--  DRS timing ";
    title += " [";
    title += timfile;
    title += "]  --";
    d->SetTitle(title, kFALSE);
    d->SaveAs(outfile);

    TFile file(outfile, "UPDATE");
    timecam.Write();

    return 0;
}
