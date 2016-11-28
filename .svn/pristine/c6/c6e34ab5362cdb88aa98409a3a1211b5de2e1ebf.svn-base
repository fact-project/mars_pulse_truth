#include <TClass.h>
#include <TApplication.h>

#include "MAGIC.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MArray.h"
#include "MParContainer.h"

#ifdef HAVE_XPM
#include "MLogo.h"
#endif

#include "MCameraDisplay.h"
#include "MEventDisplay.h"

using namespace std;

// **********************************************************************
//
//    MARS main program  
//
//    The only job of the main program is the initialization of ROOT and 
//    the start of the GUI interface for the mars program
//
//    started by  h. kornmayer      january, 3rd  2001 
static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "                    MARS V" << MARSVER              << endl;
    gLog << "   Modular Analysis and Reconstruction Software"    << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " mars [options] filename|number" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   filename: Name of a merpped raw file, a calibrated (Y) file," << endl;
    gLog << "             sequence file or sequence number." << endl;
    gLog << " Options:" << endl;
    gLog << "   --ind=path                Path to files, valid if sequence file or number given." << endl;
    gLog << "   --config=mars.rc:         Change configuration file" << endl;
    gLog << "                             <not yet implemented!>" << endl;
    gLog.Usage();
    gLog << "   -?/-h:                    This help" << endl << endl;
}

/*
Bool_t InflateRun(TString &run, Bool_t ismc)
{
    if (run.IsDigit())
    {
        const Int_t numrun = run.Atoi();
        seq = "/magic/";
        if (ismc)
            seq += ismc ? "montecarlo/" : "rawfiles/";
        seq += Form("sequences/%04d/sequence%08d.txt", numseq/10000, numseq);
        gLog << inf << "Inflated sequence file: " << seq << endl;
    }

    if (!gSystem->AccessPathName(seq, kFileExists))
        return kTRUE;

    gLog << err << "Sorry, sequence file '" << seq << "' doesn't exist." << endl;
    return kFALSE;
}
*/

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

    if (arg.HasOnly("-V") || arg.HasOnly("--version"))
        return 0;

    if (arg.HasOnly("-?") || arg.HasOnly("-h") || arg.HasOnly("--help"))
    {
        Usage();
        return 2;
    }

    //const Bool_t  kIsSequence = arg.HasOnlyAndRemove("--seq");
    const TString kConfig  = arg.GetStringAndRemove("--config=", "mars.rc");
    const TString kInpathD = arg.GetStringAndRemove("--ind=", "");

    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - Unknown commandline options..." << endl;
        arg.Print("options");
        gLog << endl;
        return 2;
    }

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()>1)
    {
        gLog << warn << "WARNING - Too many arguments..." << endl;
        Usage();
        return 2;
    }

    //
    // This is to make argv[i] more readable inside the code
    //
    const TString kFilename = arg.GetArgumentStr(0);

    //if (!kIsSequence && !InflateRun(kFilename))
    //    return 3;

#ifdef HAVE_XPM
    MLogo logo;
    logo.Popup();
#endif

    //
    // initialize ROOT
    //
    TApplication app("mars", &argc, argv);
    if (gROOT->IsBatch() || !gClient)
    {
        gLog << err << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    //
    // Switch of TObject Streamer in our base classes derived from TObject
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

#ifdef HAVE_XPM
    logo.Popdown();
#endif

    /*
    MEnv env(kConfig);
    if (!env.IsValid())
    {
        gLog << err << "Configuration file " << kConfig << " not found." << endl;
        return 0xfe;
    }
    */

    //
    // start the main window
    //
    if (kFilename.IsNull())
        new MCameraDisplay;
    else
    {
        MEventDisplay *d = new MEventDisplay(kFilename, kInpathD);
        d->SetBit(MStatusDisplay::kExitLoopOnExit);
        d->SetTitle(kFilename);
    }

    //
    // run the application
    //
    app.Run();

    gLog << all << endl;

    return 0;
}
