#include "MLogManip.h"

void merpp(TString &froot, const char *id, const char *faux)
{
    gLog.Separator(froot+" - "+TString(id));

    TFile file(froot);
    if (file.IsZombie())
    {
        gLog << err << "merpp: Invalid file '" << froot << "'" << endl;
        return;
    }

    MRawRunHeader *h = NULL;
    TTree *tree = file.Get("RunHeaders");
    if (!tree)
    {
        gLog << err << "merpp: Tree 'RunHeaders' not found... skipped." << endl;
        return;
    }

    if (tree->GetEntries()!=1)
    {
        gLog << err << "merpp: Number of RunHeaders do not match 1... skipped." << endl;
        return;
    }

    tree->SetBranchAddress("MRawRunHeader.", &h);
    tree->GetEntry(0);

    if (file.Get(id))
    {
        gLog << warn << "WARNING - Tree '" << id << "' already existing... skipped." << endl;
        return;
    }

    file.Close();

    UInt_t night = h->GetRunNumber();

    TString ffits = Form("/fact/aux/%4d/%02d/%02d/%6d.%s.fits",
                         night/10000, (night/100)%100, night%100, night, faux);

    TString report = Form("MReport%s", id);
    TString time   = Form("MTime%s",   id);

    gLog << all;
    gLog << " --- Fits file: " << ffits << endl;
    gLog << " --- Root file: " << froot << endl;
    gLog << " --- Tree:      " << id << " (" << faux << ")" << endl;

    MReportFitsRead read(ffits);
    read.SetReportName(report);
    read.SetTimeStart(h->GetRunStart());
    read.SetTimeStop(h->GetRunEnd());

    // FIXME: Write also last event BEFORE start of run

    MWriteRootFile write(froot, "UPDATE");
    write.AddContainer(report, id);
    write.AddContainer(time,   id);

    MParList plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    tlist.AddToList(&read);
    tlist.AddToList(&write);

    MEvtLoop loop;
    loop.SetParList(&plist);

    if (!loop.Eventloop())
        return;
}

int merpp_file(const char *datafile)
{
    // ------------------------------------------------------

    gLog.Separator("Merpp");
    gLog << all;
    gLog << "Merge slow control data of ";
    gLog << datafile << endl;
    gLog << endl;

    merpp(datafile, "Weather",      "MAGIC_WEATHER_DATA");
    merpp(datafile, "Drive",        "DRIVE_CONTROL_TRACKING_POSITION");
    merpp(datafile, "Rates",        "FTM_CONTROL_TRIGGER_RATES");
    merpp(datafile, "Temperatures", "FSC_CONTROL_TEMPERATURE");
    merpp(datafile, "Humidity",     "FSC_CONTROL_HUMIDITY");

    return 0;
}
