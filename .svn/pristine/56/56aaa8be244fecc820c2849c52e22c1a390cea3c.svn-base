/////////////////////////////////////////////////////////////////////////////
//
//  mirrorSimulation
//
//
// This program performs a simulation of the mirrors (in detail all processes up
// to the camera front window.
//
// It is meaned only for test purposes of the mirror simulation. At the moment it
// is a copy of the normal ceres simulation (revision 18447), with all processor removed behind
// MSimAbsorption absapd (so the absorption by the pde of the SiPMs).
//
// It doesn't provide any graphical output, and the only output is a ASCII file
// with the information of the photons.
//
//
/////////////////////////////////////////////////////////////////////////////

#include <TClass.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TObjectTable.h>

#include <TVector2.h>

#include "MArray.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MStatusDisplay.h"

#include "MEnv.h"
#include "MArgs.h"
#include "MDirIter.h"

#include "MJSimReflector.h"


using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "====================================================" << endl;
    gLog << "                MirrorSimulation - MARS V" << MARSVER           << endl;
    gLog << " MARS - Camera Electronics and REflector Simulation" << endl;
    gLog << "    Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "====================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the no usage description at the moment:" << endl;
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

    if (arg.HasOnly("-V") || arg.HasOnly("--version"))
        return 0;

    if (arg.HasOnly("-?") || arg.HasOnly("-h") || arg.HasOnly("--help"))
    {
        Usage();
        return 2;
    }

    const Bool_t  kForce      = arg.HasOnlyAndRemove("-ff");
    const Bool_t  kOverwrite  = arg.HasOnlyAndRemove("-f");
    const Bool_t  kBatch      = kTRUE;

    const TString kConfig     = arg.GetStringAndRemove("--config=", "reflector.rc");
    const TString kInpath     = arg.GetStringAndRemove("--ind=", "");
    const TString kOutpath    = arg.GetStringAndRemove("--out=", ".");
    const TString kOutfile    = arg.GetStringAndRemove("--outf=", "");

    const Int_t   kRunNumber  = arg.GetIntAndRemove("--run-number=", -1);

    const TString kOpMode     = arg.GetStringAndRemove("--mode=", "data");

    Int_t opmode = -1;
    if (TString("data").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimReflector::kModeData;
    if (TString("pointrun").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimReflector::kModePointRun;
    if (TString("pedestal").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimReflector::kModePed;
    if (TString("calibration").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimReflector::kModeCal;

    if (opmode<0)
    {
        gLog << err << "ERROR - Wrong mode specified..." << endl;
        Usage();
        return 2;
    }

    //
    // check for the right usage of the program (number of arguments)
    if (arg.GetNumArguments()<1 && opmode==MJSimReflector::kModeData)
    {
        gLog << warn << "WARNING - Wrong number of arguments..." << endl;
        Usage();
        return 2;
    }

    if (arg.GetNumArguments()>0 && opmode!=MJSimReflector::kModeData)
    {
        gLog << warn << "WARNING - No arguments allowed in this mode..." << endl;
        Usage();
        return 2;
    }

    //
    // Now we access/read the resource file. This will remove all
    // --rc= from the list of arguments.
    //
    MEnv env(kConfig, "ceres.rc");
    if (!env.IsValid())
    {
        gLog << err << "ERROR - Reading resource file " << kConfig << "." << endl;
        return 0xfe;
    }

    //
    // check for the right usage of the program (number of options)
    //
    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - Unknown commandline options..." << endl;
        arg.Print("options");
        gLog << endl;
        return 2;
    }

    //
    // Setup sequence file and check for its existance
    //
    TString kSequence = arg.GetNumArguments()==1 ? arg.GetArgumentStr(0) : "";

    //
    // Check if the first argument (if any) is a sequence file or not
    //
    if (!kSequence.EndsWith(".txt"))
        kSequence = "";

    //
    // Something special for datacenter access
    //
    if (!kSequence.IsNull() && !MSequence::InflateSeq(kSequence, kTRUE))
        return 2;

    //
    // Setup sequence and check its validity
    //
    MSequence seq(kSequence, kInpath);
    if (!kSequence.IsNull())
    {
//        if (kPrintSeq)
//        {
//            gLog << all;
//            gLog.Separator(kSequence);
//            seq.Print();
//            gLog << endl;
//        }
        if (seq.IsValid() && !seq.IsMonteCarlo())
        {
            gLog << err << "Sequence is not a Monte Carlo Sequence." << endl << endl;
            return 2;
        }
        if (!seq.IsValid())
        {
            gLog << err << "Sequence read but not valid!" << endl << endl;
            return 2;
        }

        //
        // Process print options
        //
//        if (kPrintFiles)
//            PrintFiles(seq, kInpath, kFALSE);
//        if (kPrintFound)
//            PrintFiles(seq, kInpath, kTRUE);
    }
    else
    {
//        if (kPrintFiles)
//            PrintFiles(arg, kFALSE);
//        if (kPrintFound)
//            PrintFiles(arg, kTRUE);
    }

    //
    // Initialize root
    //
    TVector2::Class()->IgnoreTObjectStreamer();
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("reflector", &argc, argv);
    if ((!gROOT->IsBatch() && !gClient) || (gROOT->IsBatch() && !kBatch))
    {
        gLog << err << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    {
        MJSimReflector job("SimReflector");
        //job.SetSequence(seq);
        job.SetEnv(&env);
        job.SetOverwrite(kOverwrite);
        job.SetPathOut(kOutpath);
        job.SetFileOut(kOutfile);
        job.SetForceMode(kForce);
        job.SetMode(opmode);
        job.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        job.SetRunNumber(kRunNumber);

        if (!job.Process(arg, seq))
        {
            gLog << err << "Calculation of reflector failed." << endl << endl;
            return 2;
        }

        if (!job.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }
    }

//    if (kBatch || kQuit)
//        delete d;
//    else
//    {
//        // From now on each 'Close' means: Terminate the application
//        d->SetBit(MStatusDisplay::kExitLoopOnClose);

//        // Wait until the user decides to exit the application
//        app.Run(kFALSE);
//    }

    if (TObject::GetObjectStat())
    {
        TObject::SetObjectStat(kFALSE);
        gObjectTable->Print();
    }

    return 0;
}
