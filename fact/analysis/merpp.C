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

int merpp(const char *seqfile="sequences/20111205_013.seq", const char *path = "output")
{
    // The sequence file which defines the files for the analysis
    MSequence seq(seqfile);
    if (!seq.IsValid())
    {
        gLog << err << "ERROR - Sequence invalid!" << endl;
        return 1;
    }

    // ------------------------------------------------------

    gLog.Separator("Merpp");
    gLog << all;
    gLog << "Merge slow control data of sequence ";
    gLog << seq.GetFileName() << endl;
    gLog << endl;
    gLog << "Path:  " << path << endl;

    MDirIter iter;
    if (seq.GetRuns(iter, MSequence::kFactImg, path)<=0)
    {
        gLog << err << "ERROR - Sequence valid but without files." << endl;
        return 2;
    }

    iter.Print();

    while (1)
    {
        TString fname = iter.Next();
        if (fname.IsNull())
            break;

        merpp(fname, "Weather",      "MAGIC_WEATHER_DATA");
        merpp(fname, "Drive",        "DRIVE_CONTROL_TRACKING_POSITION");
        merpp(fname, "Rates",        "FTM_CONTROL_TRIGGER_RATES");
        merpp(fname, "Temperatures", "FSC_CONTROL_TEMPERATURE");
        merpp(fname, "Humidity",     "FSC_CONTROL_HUMIDITY");
    }

    return 0;
}

int merpp(const ULong64_t seqnum, const char *path="output")
{
    UInt_t night = seqnum/1000;
    UInt_t num   = seqnum%1000;

    TString file = Form("/scratch/fact/sequences/%04d/%02d/%02d/%06d_%03d.seq",
                        night/10000, (night/100)%100, night%100, num);

    return merpp(file.Data(), path);
}
