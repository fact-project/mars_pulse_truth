#include <TSystem.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MTime.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateArray.h"
#include "MInputStreamID.h"

#include "MMcEvt.hxx"
#include "MMcTrig.hxx"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// This is an demonstration how to read in a merpped root file
// This is a demonstration how to use root, not how you should
// read a merpped file!
//
/////////////////////////////////////////////////////////////////////////////

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "              ReadRaw - MARS V" << MARSVER          << endl;
    gLog << "       MARS - Read and print raw data files"        << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   readraw [-h] [-?] [-vn] [-dec] [-a0] inputfile[.root]" << endl << endl;
    gLog << "     input file:   Magic DAQ binary file." << endl;
    gLog.Usage();
    gLog << "     -d, --dec: print data in decimal values" << endl;
    gLog << "     -a, --no-colors: Do not use Ansii color codes" << endl;
    gLog << "     -?,-h,--help: This help" << endl << endl;
}

void EnableBranch(TTree *t, TString name, void *ptr)
{
    if (!t->GetBranch(name+"."))
        return;

    t->GetBranch(name+".")->SetAddress(ptr);
    gLog << " Found '" << name << "'" << endl;
}

int main(int argc, char **argv)
{
    if (!MARS::CheckRootVer())
        return 0xff;

    MLog::RedirectErrorHandler(MLog::kColor);

    // Evaluate arguments
    MArgs arg(argc, argv);
    gLog.Setup(arg);

    StartUpMessage();

    // check for the right usage of the program
    if (arg.HasOption("-?") || arg.HasOption("-h") || arg.HasOption("--help") ||
        arg.GetNumArguments()!=1)
    {
        Usage();
        return 2;
    }

    arg.RemoveRootArgs();

    // Set usage of decimal values
    const bool kDecimal = arg.HasOnlyAndRemove("-d") || arg.HasOnlyAndRemove("--dec");

    //
    // check for unidentified options
    //
    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - unknown commandline options..." << endl;
        arg.Print("options");
        gLog << endl;
    }

    //
    // Initialize Non-GUI (batch) mode
    //
    gROOT->SetBatch();

    //
    // This is to make argv[i] more readable insidethe code
    //
    TString kNamein = arg.GetArgumentStr(0);
  
    //
    // check whether the given files are OK.
    //
    if (gSystem->AccessPathName(kNamein, kFileExists))
    {
        if (!kNamein.EndsWith(".root"))
            kNamein += ".root";

        if (gSystem->AccessPathName(kNamein, kFileExists))
        {
            gLog << err << "Sorry, the input file '" << kNamein << "' doesn't exist." << endl;
            return 2;
        }
    }

    //
    //  open the file
    //
    gLog << inf << " Open the file '" << kNamein << "'" << endl;
    TFile input(kNamein, "READ");

    //
    // open the Run Header and read in
    //
    gLog << " Check for Tree 'RunHeaders'" << endl;
    TTree *runtree = (TTree*)input.Get("RunHeaders");
    if (!runtree)
        gLog << warn << " WARNING - This file has no Tree 'RunHeaders'" << endl << endl;
    else
    {
        gLog << " Entries in Tree RunHeaders: " << dec << runtree->GetEntries() << endl;

        MRawRunHeader *runheader = NULL;
        runtree->GetBranch("MRawRunHeader.")->SetAddress(&runheader);
        runtree->GetEvent(0);
        runheader->Print();
    }

    //
    // open the DataTree and read in 
    //
    gLog << inf << " Check the Tree 'Events'" << endl ;
    TTree *evttree = (TTree*)input.Get("Events") ;
    if (!evttree)
    {
        gLog << err << "Tree 'Events' not found in file... exit!" << endl;
        return 2;
    }

    //
    //  check the branches in the Tree 
    //
    gLog << " Check all the Branches in the Tree." << endl;
    gLog << endl;

    MRawEvtHeader  *evtheader = NULL;
    MTime          *evttime   = NULL;
    MRawEvtData    *evtdata   = NULL;
    MRawEvtData    *evtdata2  = NULL;
    MRawCrateArray *evtcrate  = NULL;
    MMcEvt         *evtmc     = NULL;
    MMcTrig        *trigmc    = NULL;

    EnableBranch(evttree, "MRawEvtHeader",  &evtheader);
    EnableBranch(evttree, "MTime",          &evttime);
    EnableBranch(evttree, "MRawEvtData",    &evtdata);
    EnableBranch(evttree, "MRawEvtData2",   &evtdata2);
    EnableBranch(evttree, "MRawCrateArray", &evtcrate);
    EnableBranch(evttree, "MMcEvt",         &evtmc);
    EnableBranch(evttree, "MMcTrig",        &trigmc);

    //
    // loop over all entries 
    //
    const Int_t nent = (Int_t)evttree->GetEntries();

    gLog << " Entries in Tree Data: " << dec << nent << endl;
    gLog << endl;

    for (Int_t i = 0; i<nent; i++)
    {
        gLog << all << "Entry: " << i << endl;

        //
        // readin event with the selected branches
        //
        evttree->GetEvent(i);

        if (evtmc)
            evtmc->Print();
        if (trigmc)
            trigmc->Print("short");
        if (evtheader)
            evtheader->Print();
        if (evttime)
            evttime->Print();
        if (evtcrate)
            evtcrate->Print();
        if (evtdata)
            evtdata->Print(kDecimal?"dec":"hex");
        if (evtdata2)
            evtdata2->Print(kDecimal?"dec":"hex");

        gLog << endl;
    } 
    
    // end of small readin program

    return 0;
}
