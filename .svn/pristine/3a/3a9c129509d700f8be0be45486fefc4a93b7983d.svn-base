#include <TROOT.h>
#include <TClass.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TApplication.h>
#include <TObjectTable.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MEnv.h"
#include "MArgs.h"
#include "MArray.h"
#include "MDirIter.h"

#include "MStatusDisplay.h"

#include "MDataSet.h"
#include "MJSpectrum.h"

using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890123456
    gLog << "========================================================" << endl;
    gLog << "                  Sponde - MARS V" << MARSVER             << endl;
    gLog << "               MARS -- SPectrum ON DEmand"                << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "========================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    //                1         2         3         4         5         6         7         8
    //       12345678901234567890123456789012345678901234567890123456789012345678901234567890
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " sponde [options] inputfile.root mcdataset.txt [outputfile.root]" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   inputfile.root:           Ganymed output [and result] file" << endl;
    gLog << "   mcdataset.txt:            Dataset describing the Monte Carlos to be used" << endl;
    gLog << "                             For more details see MDataSet." << endl;
    gLog << "   outputfile.root:          Optional file in which the result is stored" << endl << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << " Operation Mode:" << endl;
    gLog << "   --force-theta             Force execution even with non-fitting theta" << endl;
    gLog << "                             distributions." << endl;
    gLog << "   --force-runtime           Force usage of runtime instead of effective" << endl;
    gLog << "                             observation time from fit in star." << endl;
    gLog << "   --force-ontimefit         Force usage of effective on-time from a new fit to" << endl;
    gLog << "                             the time-stamps (overwrites --force-runtime)." << endl;
    gLog << endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
    gLog << "   --debug-env=3             Debug setting resources from resource file and command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << "   --rc=Name:Option          Set or overwrite a resource of the resource file." << endl;
    gLog << "                             (Note, that this option can be used multiple times." << endl;
    gLog << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
    gLog << "   --print-ds                Print Dataset information" << endl;
    gLog << "   --print-files             Print Files taken from Sequences ('+' found, '-' missing)" << endl;
    gLog << "   --config=sponde.rc        Resource file [default=sponde.rc]" << endl;
    gLog << "   --ind=path                Path to mc/star files [default=datacenter path]"  << endl;
    gLog << "   --ins=path                Path to sequence files [default=datacenter path]"   << endl;
    gLog << "   --dataset=number          Choose a dataset from a collection of datasets"     << endl;
    gLog << "                             in your file (for more details see MDataSet)"       << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
    gLog << "Background:" << endl;
    gLog << " Sponde is a natural satellite of Jupiter. It was discovered by a team"  << endl;
    gLog << " of astronomers from the University of Hawaii led by Scott S. Sheppard," << endl;
    gLog << " et al in 2001, and given the temporary designation S/2001 J 5. Sponde"  << endl;
    gLog << " is about  2 kilometers in diameter,  and orbits Jupiter at an average"  << endl;
    gLog << " of 23,487,000 kilometers. It is also designated as Jupiter XXXVI."      << endl;
    gLog << " It is named after one of the Horae (Hours),  which presided  over the"  << endl;
    gLog << " seventh hour (libations poured after lunch).  The Hours, godesses  of"  << endl;
    gLog << " the time of day but also of the seasons,  were daughters of  Zeus and"  << endl;
    gLog << " Themis." << endl;
    gLog << " It belongs to the Pasipha‰ group, irregular retrograde moons orbiting" << endl;
    gLog << " Jupiter  at distances ranging between  22.8  and  24.1 Gm,  and  with" << endl;
    gLog << " inclinations ranging between 144.5ø and 158.3ø." << endl << endl;
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

    const TString kConfig       =  arg.GetStringAndRemove("--config=", "sponde.rc");

    const Bool_t  kPrintSeq     =  arg.HasOnlyAndRemove("--print-ds");
    const Bool_t  kPrintFiles   =  arg.HasOnlyAndRemove("--print-files");
    const Int_t   kNumDataset    = arg.GetIntAndRemove("--dataset=", -1);
    const TString kPathDataFiles = arg.GetStringAndRemove("--ind=",  "");
    const TString kPathSequences = arg.GetStringAndRemove("--ins=",  "");
    const Bool_t  kDebugMem     =  arg.HasOnlyAndRemove("--debug-mem");
    Int_t  kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kQuit          =  arg.HasOnlyAndRemove("-q");
    const Bool_t  kBatch         =  arg.HasOnlyAndRemove("-b");
    const Bool_t  kOverwrite     =  arg.HasOnlyAndRemove("-f");

    const Bool_t  kForceTheta    =  arg.HasOnlyAndRemove("--force-theta");
    const Bool_t  kForceRunTime  =  arg.HasOnlyAndRemove("--force-runtime");
    const Bool_t  kForceOnTimeFit=  arg.HasOnlyAndRemove("--force-ontimefit");

    //
    // check for the right usage of the program (number of arguments)
    //
    if (arg.GetNumArguments()<2 || arg.GetNumArguments()>3)
    {
        gLog << warn << "WARNING - Wrong number of arguments..." << endl;
        Usage();
        return 2;
    }

    //
    // Now we access/read the resource file. This will remove all
    // --rc= from the list of arguments.
    //
    MEnv env(kConfig, "sponde.rc");
    if (!env.IsValid())
    {
        gLog << err << "ERROR - Reading resource file " << kConfig << "." << endl;
        return 0xfe;
    }

    // And move the resource options from the command line to the MEnv
    if (!env.TakeEnv(arg, kDebugEnv>2))
        return 0xfd;

    //
    // check for the right usage of the program (number of options)
    //
    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - Unknown command line options..." << endl;
        arg.Print("options");
        gLog << endl;
        return 2;
    }

    //
    // Setup sequence file and check for its existence
    //
    TString kInfile  = arg.GetArgumentStr(0);
    TString kDataset = arg.GetArgumentStr(1);
    TString kOutfile = arg.GetArgumentStr(2);

    if (kOutfile.IsNull() && kBatch)
    {
        gLog << err << "Writing no outputfile but running in batch mode is nonsense." << endl;
        return 2;
    }

    gSystem->ExpandPathName(kInfile);
    gSystem->ExpandPathName(kDataset);
    gSystem->ExpandPathName(kOutfile);

    if (gSystem->AccessPathName(kInfile, kFileExists))
    {
        gLog << err << "Sorry, ganymed root-file '" << kInfile << "' doesn't exist." << endl;
        return 2;
    }
    if (gSystem->AccessPathName(kDataset, kFileExists))
    {
        gLog << err << "Sorry, dataset file '" << kDataset << "' doesn't exist." << endl;
        return 2;
    }

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Setup sequence and check its validity
    //
    MDataSet seq(kDataset, (UInt_t)kNumDataset, kPathSequences, kPathDataFiles);
    if (kPrintSeq || kPrintFiles)
    {
        gLog << all;
        gLog.Separator(kDataset);
        seq.Print(kPrintFiles?"files":"");
        gLog << endl;
    }
    if (!seq.IsValid())
    {
        gLog << err << "Dataset read but not valid (maybe analysis number not set)!" << endl << endl;
        return 2;
    }

    if (!seq.IsMonteCarlo())
        gLog << warn << "Dataset file seems not to be a Monte Carlo dataset." << endl << endl;


    //
    // Initialize root
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("sponde", &argc, argv);
    if ((!gROOT->IsBatch() && !gClient) || (gROOT->IsBatch() && !kBatch))
    {
        gLog << err << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    //
    // Update frequency by default = 1Hz
    //
    MStatusDisplay *d = new MStatusDisplay;

    // From now on each 'Exit' means: Terminate the application
    d->SetBit(MStatusDisplay::kExitLoopOnExit);
    d->SetTitle(Form("-- Sponde: %s --", kDataset.Data()));

    //
    // Calculate spectrum (block for debug-mem)
    //
    {
        MJSpectrum job(Form("Spectrum - %s", gSystem->BaseName(kInfile)));
        job.SetEnv(&env);
        job.SetEnvDebug(kDebugEnv);
        job.SetDisplay(d);;
        job.SetOverwrite(kOverwrite);
        job.SetPathOut(kOutfile);
        job.SetPathIn(kInfile);
        job.SetCommandLine(MArgs::GetCommandLine(argc, argv));

        job.ForceTheta(kForceTheta);
        job.ForceRunTime(kForceRunTime);
        job.ForceOnTimeFit(kForceOnTimeFit);

        if (!job.Process(seq))
        {
            gLog << err << "Calculation of spectrum failed." << endl << endl;
            return 2;
        }
        if (kDebugEnv>0 || gLog.GetDebugLevel()>=2)
            env.PrintUntouched();

        if (!job.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }
    }

    if (kBatch || kQuit)
        delete d;
    else
    {
        // From now on each 'Close' means: Terminate the application
        d->SetBit(MStatusDisplay::kExitLoopOnClose);

        // Wait until the user decides to exit the application
        app.Run(kFALSE);
    }

    if (TObject::GetObjectStat())
    {
        TObject::SetObjectStat(kFALSE);
        gObjectTable->Print();
    }

    return 0;
}
