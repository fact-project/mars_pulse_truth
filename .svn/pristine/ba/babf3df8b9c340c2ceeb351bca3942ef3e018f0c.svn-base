#include <TSystem.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MTime.h"
#include "MPrint.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateArray.h"
#include "MRawFitsRead.h"


//#include "MInputStreamID.h"
//#include "MMcEvt.hxx"
//#include "MMcTrig.hxx"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// This is an demonstration how to read in a unmerpped daq file
//
/////////////////////////////////////////////////////////////////////////////

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "              ReadDaq - MARS V" << MARSVER          << endl;
    gLog << "       MARS - Read and print daq data files"        << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   readdaq [-h] [-?] [-vn] [-dec] [-a0] inputfile[.raw|.fits]" << endl << endl;
    gLog << "     input file:   Magic DAQ binary file." << endl;
    gLog.Usage();
    gLog << "     -d1: print data in decimal values" << endl;
    gLog << "     -c1: print MRawCrateArray data" << endl;
    gLog << "     -ff: force reading of broken runheader" << endl;
    gLog << "     -f:  force reading of files with problems" << endl;
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

    //
    // Set verbosity to highest level.
    //
    const Bool_t kDecimal    = arg.HasOption("-d") && arg.GetIntAndRemove("-d")==1;
    const Bool_t kPrintArray = arg.HasOption("-c") && arg.GetIntAndRemove("-c")==1;
    const Bool_t kForce      = arg.HasOnlyAndRemove("-ff");
    const Bool_t kForceMode  = arg.HasOnlyAndRemove("-f");

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()!=1)
    {
        Usage();
        return 2;
    }

    //
    // This is to make argv[i] more readable insidethe code
    //
    TString kNamein = arg.GetArgumentStr(0);
  
    if (!kNamein.EndsWith(".raw")  && !kNamein.EndsWith(".raw.gz") &&
        !kNamein.EndsWith(".fits") && !kNamein.EndsWith(".fits.gz"))
        kNamein += ".raw";

    //
    // Initialize Non-GUI (batch) mode
    //
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
    MRawRunHeader runheader;
    plist.AddToList(&runheader);

    MRawEvtHeader evtheader;
    plist.AddToList(&evtheader);

    MRawEvtData evtdata;
    plist.AddToList(&evtdata);

    MRawEvtData evtdata2("MRawEvtData2");
    plist.AddToList(&evtdata2);

    MRawCrateArray cratearray;
    plist.AddToList(&cratearray);

    MTime evttime;
    plist.AddToList(&evttime);

    //
    // create the tasks which should be executed and add them to the list
    // in the case you don't need parameter containers, all of them can
    // be created by MRawFileRead::PreProcess
    //
    MRawFileRead *read = MRawFitsRead::IsFits(kNamein) ? new MRawFitsRead(kNamein) : new MRawFileRead(kNamein);
    read->SetBit(kCanDelete);
    read->SetForce(kForce);
    read->SetForceMode(kForceMode);
    tasks.AddToList(read);

    MPrint print0;
    MPrint print1("MRawEvtHeader",  "nogains",            "PrintEvtHeader");
    MPrint print2("MTime",          "",                   "PrintTime");
    MPrint print3("MRawCrateArray", "",                   "PrintCrateArray");
    MPrint print4("MRawEvtData",    kDecimal?"dec":"hex", "PrintEvtData");
    MPrint print5("MRawEvtData2",   kDecimal?"dec":"hex", "PrintEvtData2");

    tasks.AddToList(&print0);
    tasks.AddToList(&print1);
    tasks.AddToList(&print2);
    if (kPrintArray)
        tasks.AddToList(&print3);
    tasks.AddToList(&print4);
    tasks.AddToList(&print5);

    //
    // create the looping object and tell it about the parameters to use
    // and the tasks to execute
    //
    MEvtLoop magic;
    magic.SetParList(&plist);

    //
    // Start the eventloop which reads the raw file (MRawFileRead) and
    // write all the information into a root file (MRawFileWrite)
    //
    // between reading and writing we can do, transformations, checks, etc.
    // (I'm think of a task like MRawDataCheck)
    //
    if (!magic.Eventloop())
    {
        gLog << err << "ERROR: Reading DAQ file failed!" << endl;
        return 2;
    }

    gLog << all << "Reading DAQ file finished successfull!" << endl;

    // end of small readin program

    return 0;
}
