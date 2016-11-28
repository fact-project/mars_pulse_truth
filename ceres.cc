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

#include "MJSimulation.h"


using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "====================================================" << endl;
    gLog << "                Ceres - MARS V" << MARSVER           << endl;
    gLog << " MARS - Camera Electronics and REflector Simulation" << endl;
    gLog << "    Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "====================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   ceres [options] [inputfiles|sequence.txt]" << endl << endl;
    gLog << "     inputfiles              MMCS (CORSIKA) binary (cherenkov) files, wildcards allowed." << endl;
    gLog << "     sequence.txt            A sequence file." << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
    gLog << "   --debug-env=3             Debug setting resources from resource file and" << endl;
    gLog << "                              command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << "   --rc=Name:option          Set or overwrite a resource of the resource file." << endl;
    gLog << "                             (Note, that this option can be used multiple times)" << endl;
    gLog << endl;
    gLog << " Output options:" << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
    gLog << "   -ff                       Force reading of file even if problems occur" << endl;
    gLog << "   --out=path                Path to write the all results to [def=local path]" << endl;
    gLog << "   --ind=path                Input path of Corsika files if sequence used" << endl;
    gLog << "                              [def=standard path in datacenter]" << endl;
    gLog << "   --outf=filename           Output filename. Combines all input files into" << endl;
    gLog << "                              one output file. (Note that the output file will" << endl;
    gLog << "                              contain only the headers corresponding to the" << endl;
    gLog << "                              first input file)" << endl;
    gLog << "   --dev-null                Suppress output of files (for test purpose)" << endl;
    gLog << "   --print-seq               Print Sequence information [sequence only]" << endl;
    gLog << "   --print-files             Print Files taken from Sequence" << endl;
    gLog << "   --print-found             Print Files found from Sequence" << endl;
    gLog << "   --config=ceres.rc         Resource file [default=reflector.rc]" << endl;
    gLog << endl;
    gLog << "   --mode=pedestal           Execution mode. Produce either pedestals," << endl;
    gLog << "   --mode=calibration        calibration data (no input files required) or" << endl;
    gLog << "   --mode=data               process data files [default]" << endl << endl;
    gLog << "   --run-number=#            Optionally set the run number of the run to simulate" << endl;
    gLog << "   --fits                    Write FITS output files instead of root files." << endl << endl;
    gLog << endl;
//    gLog << "     -f:  force reading of runheader" << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
    gLog << "Background:" << endl;
    gLog << " Ceres,  formal designation 1 Ceres, is the smallest identified dwarf planet in" << endl;
    gLog << " the Solar System  and the only one in the asteroid belt.  It was discovered on" << endl;
    gLog << " January 1,  1801,  by Giuseppe Piazzi,  and  is named after  the Roman goddess" << endl;
    gLog << " Ceres, the goddess of growing plants, the harvest, and of motherly love." << endl;
    gLog << " With a diameter  of about 950km,  Ceres is by far the largest and most massive" << endl;
    gLog << " body  in the asteroid belt,  and  contains  a third of  the belt's total mass." << endl;
    gLog << " Recent observations  have revealed that it is spherical,  unlike the irregular" << endl;
    gLog << " shapes of smaller bodies with lower gravity.  The surface of Ceres is probably" << endl;
    gLog << " made of a mixture of water ice  and various hydrated minerals  like carbonates" << endl;
    gLog << " and clays. Ceres appears to be differentiated into a rocky core and ice mantle." << endl;
    gLog << " It may harbour an ocean of liquid water underneath its surface, which makes it" << endl;
    gLog << " a potential target in the search for extraterrestrial life." << endl;
    gLog << " Ceres' apparent magnitude  ranges from  6.7 to 9.3,  hence at its brightest is" << endl;
    gLog << " still too dim to be seen with the naked eye. On Sept. 27, 2007,  NASA launched" << endl;
    gLog << " the Dawn space probe to explore Vesta and Ceres." << endl << endl;
    gLog << "Example:" << endl;
    gLog << " ceres -f --out=outpath/ cer000001 cer000002" << endl;
    gLog << endl;
}

static void PrintFiles(const MArgs &arg, Bool_t allopt)
{
    const char *prep = allopt ? "Found" : "Scheduled";

    gLog << all;
    gLog.Separator(Form("%s Data Files", prep));
    for (int i=0; i<arg.GetNumArguments(); i++)
        if (!allopt || gSystem->AccessPathName(arg.GetArgumentStr(i), kFileExists)==0)
            gLog << arg.GetArgumentStr(i) << endl;
    gLog << endl;
}


static void PrintFiles(const MSequence &seq, const TString &kInpathD, Bool_t allopt)
{
    const char *prep = allopt ? "Found" : "Scheduled";

    MDirIter Next;
    seq.GetRuns(Next, MSequence::kCorsika, kInpathD);

    gLog << all;
    gLog.Separator(Form("%s Data Files", prep));
    Next.Print(allopt?"all":"");
    gLog << endl;
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

    const Bool_t  kBatch      = arg.HasOnlyAndRemove("-b");
    //const Int_t   kCompLvl   = arg.GetIntAndRemove("--comp=", 1);
    const Bool_t  kForce      = arg.HasOnlyAndRemove("-ff");
    const Bool_t  kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");
    const Bool_t  kNullOut    = arg.HasOnlyAndRemove("--dev-null");
    Int_t kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kPrintSeq   = arg.HasOnlyAndRemove("--print-seq");
    const Bool_t  kPrintFiles = arg.HasOnlyAndRemove("--print-files");
    const Bool_t  kPrintFound = arg.HasOnlyAndRemove("--print-found");

    const Bool_t  kQuit       = arg.HasOnlyAndRemove("-q");
    const Bool_t  kOverwrite  = arg.HasOnlyAndRemove("-f");

    const TString kConfig     = arg.GetStringAndRemove("--config=", "ceres.rc");
    const TString kInpath     = arg.GetStringAndRemove("--ind=", "");
    const TString kOutpath    = arg.GetStringAndRemove("--out=", ".");
    const TString kOutfile    = arg.GetStringAndRemove("--outf=", "");

    const Int_t   kRunNumber  = arg.GetIntAndRemove("--run-number=", -1);

    const TString kOpMode     = arg.GetStringAndRemove("--mode=", "data");
    const Bool_t  kFitsFile   = arg.HasOnlyAndRemove("--fits");

    Int_t opmode = -1;
    if (TString("data").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimulation::kModeData;
    if (TString("pointrun").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimulation::kModePointRun;
    if (TString("pedestal").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimulation::kModePed;
    if (TString("calibration").BeginsWith(kOpMode, TString::kIgnoreCase))
        opmode = MJSimulation::kModeCal;

    if (opmode<0)
    {
        gLog << err << "ERROR - Wrong mode specified..." << endl;
        Usage();
        return 2;
    }

    //
    // check for the right usage of the program (number of arguments)
    if (arg.GetNumArguments()<1 && opmode==MJSimulation::kModeData)
    {
        gLog << warn << "WARNING - Wrong number of arguments..." << endl;
        Usage();
        return 2;
    }

    //
    // for compatibility with the first version of ceres
    //
    if (arg.GetNumArguments()==1 && opmode==MJSimulation::kModeData)
    {
        if (arg.GetArgumentStr(0)=="pedestal")
        {
            opmode = MJSimulation::kModePed;
            arg.RemoveArgument(0);
        }
        if (arg.GetArgumentStr(0)=="calibration")
        {
            opmode = MJSimulation::kModeCal;
            arg.RemoveArgument(0);
        }
    }

    if (arg.GetNumArguments()>0 && opmode!=MJSimulation::kModeData)
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

    // And move the resource options from the command line to the MEnv
    if (!env.TakeEnv(arg, kDebugEnv>2))
        return 0xfd;

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
        if (kPrintSeq)
        {
            gLog << all;
            gLog.Separator(kSequence);
            seq.Print();
            gLog << endl;
        }
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
        if (kPrintFiles)
            PrintFiles(seq, kInpath, kFALSE);
        if (kPrintFound)
            PrintFiles(seq, kInpath, kTRUE);
    }
    else
    {
        if (kPrintFiles)
            PrintFiles(arg, kFALSE);
        if (kPrintFound)
            PrintFiles(arg, kTRUE);
    }

    //
    // Initialize root
    //
    TVector2::Class()->IgnoreTObjectStreamer();
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("ceres", &argc, argv);
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
    d->SetTitle(seq.IsValid() ? Form("-- Ceres: %s --", kSequence.Data()) : "-- Ceres --");

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Do star in a block (debug mem)
    //
    {
        MJSimulation job(seq.IsValid() ? Form("Ceres #%d", seq.GetSequence()) : "Ceres");
        //job.SetSequence(seq);
        job.SetEnv(&env);
        job.SetEnvDebug(kDebugEnv);
        job.SetDisplay(d);;
        job.SetOverwrite(kOverwrite);
        job.SetPathOut(kOutpath);
        job.SetFileOut(kOutfile);
        job.SetNullOut(kNullOut);
        job.SetForceMode(kForce);
        job.SetWriteFitsFile(kFitsFile);
        job.SetMode(opmode);
        job.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        job.SetRunNumber(kRunNumber);

        // job.SetPathIn(kInpath); // not yet needed

        if (!job.Process(arg, seq))
        {
            gLog << err << "Calculation of ceres failed." << endl << endl;
            return 2;
        }

        if (kDebugEnv>0)
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
