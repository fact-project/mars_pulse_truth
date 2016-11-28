#include <TClass.h>
#include <TSystem.h>
#include <TVector2.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MPrint.h"

#include "MCorsikaRead.h"

#include "MWriteRootFile.h"


using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "            ReadCorsika - MARS V" << MARSVER        << endl;
    gLog << "     MARS - Read and print corsika data files"      << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   readcorsika [-h] [-?] [-vn] [-dec] [-a0] [-A=arrayNo] [-T=telescopeNo] inputfile[.raw] [outputfile[.root]]" << endl << endl;
    gLog << "     inputfile:     corsika raw file or eventio file" << endl;
    gLog << "     outputfile:    root file" << endl;
    gLog << "   -A=arrayNo:      use data only of array number arrayNo" << endl;
    gLog << "   -T=telescopeNo:  use data only of telescope number telescopeNo (used only for eventio input file)" << endl;
    gLog << "   -ff                       Force reading of file even if problems occur" << endl;
    gLog.Usage();
//    gLog << "     -f:  force reading of runheader" << endl;
    gLog << "     -?, -h, --help: This help" << endl << endl;
}

int main(int argc, char **argv)
{
    if (!MARS::CheckRootVer())
        return 0xff;

    MLog::RedirectErrorHandler(MLog::kColor);

    //
    // Evaluate arguments
    //
    MArgs arg(argc, argv);
    gLog.Setup(arg);

    StartUpMessage();

    if (arg.HasOnly("-?") || arg.HasOnly("-h") || arg.HasOnly("--help"))
    {
        Usage();
        return 2;
    }

    arg.RemoveRootArgs();

    const Int_t  kCompLvl = arg.GetIntAndRemove("--comp=", 1);
    const Int_t  kArrayNo = arg.GetIntAndRemove("-A=", -1);
    const Int_t  kTelNo   = arg.GetIntAndRemove("-T=", -1);
    const Bool_t kForce   = arg.HasOnlyAndRemove("-f");
    const Bool_t kForceRd = arg.HasOnlyAndRemove("-ff");


    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()<1 || arg.GetNumArguments()>2)
    {
        Usage();
        return 2;
    }

    //
    // This is to make argv[i] more readable insidethe code
    //
    TString kNamein  = arg.GetArgumentStr(0);
    TString kNameout = arg.GetArgumentStr(1);
  
//    if (!kNamein.EndsWith(".raw") && !kNamein.EndsWith(".raw.gz"))
//        kNamein += ".raw";

    if (!kNameout.IsNull() && !kNameout.EndsWith(".root"))
        kNameout += ".root";

    //
    // Initialize Non-GUI (batch) mode
    //
    TObject::Class()->IgnoreTObjectStreamer();
    TVector2::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    gROOT->SetBatch();

    //
    // check whether the given files are OK.
    //
    if (gSystem->AccessPathName(kNamein, kFileExists))
    {
        gLog << err << "Sorry, the input file '" << kNamein << "' doesn't exist." << endl;
        return 2;
    }

    //
    //  open the file
    //
    gLog << " Open the file '" << kNamein << "'" << endl;


    //
    // create a (empty) list of parameters which can be used by the tasks
    // and an (empty) list of tasks which should be executed
    //
    MParList plist;

    MTaskList tasks;
    tasks.SetOwner();
    plist.AddToList(&tasks);

    //
    // ---- The following is only necessary to supress some output ----
    //
    /*
    MCorsikaRunHeader runheader;
    plist.AddToList(&runheader);

    MCorsikaEvtHeader evtheader;
    plist.AddToList(&evtheader);

    MCorsikaData evtdata;
    plist.AddToList(&evtdata);
    */
    //
    // create the tasks which should be executed and add them to the list
    // in the case you don't need parameter containers, all of them can
    // be created by MCorsikaRead::PreProcess
    //
    MCorsikaRead read(kNamein);
    read.SetForceMode(kForceRd);
    read.SetArrayIdx(kArrayNo);
    read.SetTelescopeIdx(kTelNo);
    tasks.AddToList(&read);

    MPrint print0;
    MPrint print1("MCorsikaEvtHeader", "", "PrintEvtHeader");
    MPrint print4("MPhotonEvent",      "", "PrintEvent");

    MWriteRootFile write(kNameout, kForce?"RECREATE":"NEW", "Corsika File", kCompLvl);
    write.AddContainer("MCorsikaRunHeader", "RunHeaders");
    write.AddContainer("MCorsikaEvtHeader", "Events");
    write.AddContainer("MPhotonEvent",      "Events");

    if (kNameout.IsNull())
    {
        tasks.AddToList(&print0);
        tasks.AddToList(&print1);
        if (gLog.GetDebugLevel()>2)
            tasks.AddToList(&print4);
    }
    else
        tasks.AddToList(&write);

    //
    // create the looping object and tell it about the parameters to use
    // and the tasks to execute
    //
    MEvtLoop magic;
    magic.SetParList(&plist);

    //
    // Start the eventloop which reads the raw file (MCorsikaRead) and
    // write all the information into a root file (MCorsikaFileWrite)
    //
    // between reading and writing we can do, transformations, checks, etc.
    // (I'm think of a task like MCorsikaDataCheck)
    //
    if (!magic.Eventloop())
    {
        gLog << err << "ERROR: Reading Corsika file failed!" << endl;
        return 2;
    }

    gLog << all << "Reading Corsika file finished successfull!" << endl;

    // end of small readin program

    return 0;
}
