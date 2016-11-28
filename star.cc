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

#include "MSequence.h"
#include "MJStar.h"

using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5         6
    //       123456789012345678901234567890123456789012345678901234567890
    gLog << "========================================================" << endl;
    gLog << "                   Star - MARS V" << MARSVER              << endl;
    gLog << "      MARS -- STandard Analysis and Reconstruction"       << endl;
    gLog << "     Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "========================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    //                1         2         3         4         5         6         7         8
    //       12345678901234567890123456789012345678901234567890123456789012345678901234567890
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " star [options] sequence.txt|[tel:]number" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   sequence.txt:             Ascii file defining a sequence of runs" << endl;
    gLog << "   number:                   The sequence number (using file in the datacenter)" << endl;
    gLog << "                             For more details see MSequence" << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
    gLog << "   --debug-env=3             Debug setting resources from resource file and command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << "   --rc=Name:option          Set or overwrite a resource of the resource file." << endl;
    gLog << "                             (Note, that this option can be used multiple times." << endl;
    gLog << endl;
    gLog << " Input Options:" << endl;
    gLog << "   -mc                       You must use this for MC files (PRELIMINARY)" << endl << endl;
    gLog << " Output options:" << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
//    gLog << "   -ff                       Force execution if not all files found" << endl;
    gLog << "   --ind=path                Path where to search for the calibrated data (Y)" << endl;
    gLog << "                             [default=standard path in datacenter]" << endl;
    gLog << "   --out=path                Path to write the all results to [def=local path]" << endl;
    gLog << "   --dev-null                Suppress output of I-files (for test purpose)" << endl;
    gLog << "   --no-muons                Switch off Muon analysis (for fast tests)" << endl;
    gLog << "   --print-seq               Print Sequence information" << endl;
    gLog << "   --print-files             Print Files taken from Sequence" << endl;
    gLog << "   --print-found             Print Files found from Sequence" << endl;
    gLog << "   --config=star.rc          Resource file [default=star.rc]" << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
}

static void PrintFiles(const MSequence &seq, const TString &kInpathD, Bool_t allopt)
{
    const char *prep = allopt ? "Found" : "Scheduled";

    MDirIter Next;
    seq.GetRuns(Next, MSequence::kCalibrated, kInpathD);

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

    const TString kConfig     = arg.GetStringAndRemove("--config=", "star.rc");

    const Bool_t  kPrintSeq   = arg.HasOnlyAndRemove("--print-seq");
    const Bool_t  kPrintFiles = arg.HasOnlyAndRemove("--print-files");
    const Bool_t  kPrintFound = arg.HasOnlyAndRemove("--print-found");
    const Bool_t  kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");
    const Bool_t  kNullOut    = arg.HasOnlyAndRemove("--dev-null");
    Int_t  kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kQuit       = arg.HasOnlyAndRemove("-q");
    const Bool_t  kBatch      = arg.HasOnlyAndRemove("-b");
    const Bool_t  kOverwrite  = arg.HasOnlyAndRemove("-f");
    //const Bool_t  kForceExec  = arg.HasOnlyAndRemove("-ff");
    const Bool_t  kIsMC       = arg.HasOnlyAndRemove("-mc");
    const Bool_t  kNoMuons    = arg.HasOnlyAndRemove("--no-muons");

    const TString kInpath     = arg.GetStringAndRemove("--ind=", "");
    const TString kOutpath    = arg.GetStringAndRemove("--out=", ".");

    //
    // check for the right usage of the program (number of arguments)
    //
    if (arg.GetNumArguments()!=1)
    {
        gLog << warn << "WARNING - Wrong number of arguments..." << endl;
        Usage();
        return 2;
    }

    //
    // Now we access/read the resource file. This will remove all
    // --rc= from the list of arguments.
    //
    MEnv env(kConfig, "star.rc");
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
    TString kSequence = arg.GetArgumentStr(0);

    //
    // Something special for datacenter access
    //
    if (!MSequence::InflateSeq(kSequence, kIsMC))
        return 2;

    //
    // Setup sequence and check its validity
    //
    MSequence seq(kSequence, kInpath);
    if (!seq.IsMonteCarlo())
        seq.SetMonteCarlo(kIsMC);
    if (kPrintSeq)
    {
        gLog << all;
        gLog.Separator(kSequence);
        seq.Print();
        gLog << endl;
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

    //
    // Initialize root
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("star", &argc, argv);
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
    d->SetTitle(Form("-- Star: %s --", kSequence.Data()));

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Do star in a block (debug mem)
    //
    {
        MJStar job(Form("MJStar #%d", seq.GetSequence()));
        job.SetSequence(seq);
        job.SetEnv(&env);
        job.SetEnvDebug(kDebugEnv);
        job.SetDisplay(d);;
        job.SetOverwrite(kOverwrite);
        job.SetPathOut(kOutpath);
        job.SetNullOut(kNullOut);
        job.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        // job.SetPathIn(kInpath); // not yet needed
        if (kNoMuons)
            job.DisableMuonAnalysis();

        if (!job.Process())
        {
            gLog << err << "Calculation of image parameters failed." << endl << endl;
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
