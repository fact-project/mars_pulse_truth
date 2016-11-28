#include <TTree.h>
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
#include "MJCut.h"

using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "========================================================" << endl;
    gLog << "                  Ganymed - MARS V" << MARSVER            << endl;
    gLog << "   MARS -- Gammas Are Now Your Most Exciting Discovery"   << endl;
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
    gLog << " ganymed [options] dataset.txt|number" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   dataset.txt:              File defining a collection of datasets/sequences"   << endl;
    gLog << "                             (for more details see MSequence/MDataSet)"          << endl;
    gLog << "   number:                   The dataset number (using file in the datacenter)"  << endl;
    gLog << "                             For more details see MDataSet."                     << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)"         << endl;
    gLog << endl;
    gLog << " Operation Mode:" << endl;
    gLog << "   -mc                       Monte Carlo dataset (no times)"                     << endl;
    gLog << "   --wobble                  Force wobble mode (overwrites dataset)"             << endl;
    gLog << "   --no-wobble               Force normal mode (overwrites dataset)"             << endl;
    gLog << endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    gLog << "   --debug-env=0             Disable debugging setting resources <default>"      << endl;
    gLog << "   --debug-env[=1]           Display untouched resources after execution"        << endl;
    gLog << "   --debug-env=2             Display untouched resources after eventloop setup"  << endl;
    gLog << "   --debug-env=3             Debug setting resources from resource file and"     << endl;
    gLog << "                             command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << "   --rc=Name:option          Set or overwrite a resource of the resource file."  << endl;
    gLog << "                             (Note, this option can be used multiple times)"     << endl;
    gLog << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
    gLog << "   --n=number                Analysis number (required if not in dataset file)"  << endl;
    gLog << "   --dataset=number          Choose a dataset from a collection of datasets"     << endl;
    gLog << "                             in your file (for more details see MDataSet)"       << endl;
    gLog << "   --out=path                Path to write all output to [def=local path]"       << endl;
    gLog << "   --ind=path                Path to data/star files [default=datacenter path]"  << endl;
    gLog << "   --ins=path                Path to sequence files [default=datacenter path]"   << endl;
    gLog << "   --outf=filename           Filename for output file (eg. status display)"      << endl;
    gLog << "   --sum[=filename]          Enable writing of summary file (events after cut0)" << endl;
//    gLog << "   --res[=filename]          Enable writing of result file (surviving events)" << endl;
    gLog << "   --skip-res                Disable writing of result events"                   << endl;
    gLog << "   --write-only              Only write output files. No histograms filled."     << endl;
    gLog << "   --print-ds                Print dataset as interpreted from the dataset file" << endl;
    gLog << "   --print-files             Print files from sequences, '+' found, '-' missing" << endl;
//    gLog << "   --full-display            Show as many plots as possible" << endl;
    gLog << "   --config=ganymed.rc       Resource file [default=ganymed.rc]"                 << endl;
    gLog << endl;
    gLog << "   --max-file-size=number    Sets the maximum size before root opens a new file" << endl;
    gLog << "                             automatically. The number is given in kB (1000b)."  << endl;
    gLog << "                             Use this option with caution it can result in"      << endl;
    gLog << "                             extremely large files (especially summary files),"  << endl;
    gLog << "                             use (e.g. lower size) cuts instead to decrease"     << endl;
    gLog << "                             the file size. Root's default is 1.9GB."            << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number"           << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
    gLog << "Background:" << endl;
    gLog << " Ganymed is the largest moon of Jupiter, a large, icy, outer moon that" << endl;
    gLog << " is scarred  with impact craters  and  many parallel faults.  It has a" << endl;
    gLog << " diameter of about  5268km  and orbits  Jupiter  at a mean distance of" << endl;
    gLog << " 1,070,000km.  It has a magnetic field  and probably has a molten iron" << endl;
    gLog << " core.  It takes  Ganymed  7.15 days to  orbit  Jupiter.  Its mass  is" << endl;
    gLog << " 1.5e23kg. It was independently discovered by  Galileo and S.Marius in" << endl;
    gLog << " 1610.  Ganymed is  the largest moon  in the solar system;  it is also" << endl;
    gLog << " larger than the planets Mercury and Pluto." << endl << endl;
}

int main(int argc, char **argv)
{
    if (!MARS::CheckRootVer())
        return 0xff;

    // FIXME: Record command line to file!!!

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

    const TString kConfig       = arg.GetStringAndRemove("--config=", "ganymed.rc");

    const Bool_t  kPrintSeq     = arg.HasOnlyAndRemove("--print-ds");
    const Bool_t  kPrintFiles   = arg.HasOnlyAndRemove("--print-files");
    const Bool_t  kDebugMem     = arg.HasOnlyAndRemove("--debug-mem");
    const Bool_t  kWriteOnly    = arg.HasOnlyAndRemove("--write-only");
//    const Bool_t  kFullDisplay  = arg.HasOnlyAndRemove("--full-display");
    Int_t  kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kQuit          = arg.HasOnlyAndRemove("-q");
    const Bool_t  kBatch         = arg.HasOnlyAndRemove("-b");
    const Bool_t  kOverwrite     = arg.HasOnlyAndRemove("-f");
    //const Bool_t  kForceExec   = arg.HasOnlyAndRemove("-ff");

    const Bool_t  kIsMc          = arg.HasOnlyAndRemove("-mc");
    const Bool_t  kWobbleModeOn  = arg.HasOnlyAndRemove("--wobble");
    const Bool_t  kWobbleModeOff = arg.HasOnlyAndRemove("--no-wobble");

    const Int_t   kNumAnalysis   = arg.GetIntAndRemove("--n=", -1);
    const Int_t   kNumDataset    = arg.GetIntAndRemove("--dataset=", -1);
    const TString kOutpath       = arg.GetStringAndRemove("--out=",  ".");
    const TString kOutfile       = arg.GetStringAndRemove("--outf=",  "");
    const TString kPathDataFiles = arg.GetStringAndRemove("--ind=",  "");
    const TString kPathSequences = arg.GetStringAndRemove("--ins=",  "");
    const Bool_t  kWriteSummary  = arg.HasOnlyAndRemove("--sum");
    const TString kNameSummary   = arg.GetStringAndRemove("--sum=");
    const Bool_t  kSkipResult    = arg.HasOnlyAndRemove("--skip-res");

//    const Bool_t  kWriteResult   = arg.HasOnlyAndRemove("--res");
//    const TString kNameResult    = arg.GetStringAndRemove("--res=");

    TTree::SetMaxTreeSize((Long64_t)arg.GetIntAndRemove("--max-file-size=", TTree::GetMaxTreeSize()/1000)*1000);

    if (kWobbleModeOn && kWobbleModeOff)
    {
        gLog << err << "ERROR - Wobble mode options are exclusive." << endl;
        Usage();
        return 2;
    }

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
    MEnv env(kConfig, "ganymed.rc");
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
        gLog << warn << "WARNING - Unknown command-line options..." << endl;
        arg.Print("options");
        gLog << endl;
        return 2;
    }

    //
    // Setup sequence file and check for its existence
    //
    TString kSequences = arg.GetArgumentStr(0);

    //
    // Something special for datacenter access
    //
    if (kSequences.IsDigit())
    {
        const Int_t numseq = kSequences.Atoi();
        kSequences = Form("/magic/datasets/%05d/dataset%08d.txt", numseq/1000, numseq);
        gLog << inf << "inflated dataset file: " << kSequences << endl;
    }

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Setup sequence and check its validity
    //
    MDataSet seq(kSequences, (UInt_t)kNumDataset, kPathSequences, kPathDataFiles);
    if (!seq.IsValid())
    {
        gLog << err << "ERROR - Reading dataset file " << kSequences << "." << endl;
        return 0xfc;
    }
    if (!seq.IsMonteCarlo())
        seq.SetMonteCarlo(kIsMc);
    if (kWobbleModeOn || kWobbleModeOff)
        seq.SetWobbleMode(kWobbleModeOn);
    if (kNumAnalysis>=0)
        seq.SetNumAnalysis(kNumAnalysis);
    if (kPrintSeq || kPrintFiles)
    {
        gLog << all;
        gLog.Separator(kSequences);
        seq.Print(kPrintFiles?"files":"");
        gLog << endl;
    }
    if (!seq.IsValid())
    {
        gLog << err << "Dataset read but not valid (maybe analysis number not set)!" << endl << endl;
        return 2;
    }

    //
    // Initialize root
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("ganymed", &argc, argv);
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
    d->SetTitle(Form("-- Ganymed: %s --", kSequences.Data()));

    //
    // Run cut program. (block for debug-mem)
    //
    {
        MJCut job(Form("MJCut #%d", seq.GetNumAnalysis()));
        job.SetEnv(&env);
        job.SetEnvDebug(kDebugEnv);
        job.SetDisplay(d);;
        job.SetOverwrite(kOverwrite);
        job.SetPathOut(kOutpath);
        job.SetNameOutFile(kOutfile);
        job.SetNameSummaryFile(kNameSummary);
        job.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        //job.SetNameResultFile(kNameResult);
        job.EnableWriteOnly(kWriteOnly);
        //if (kFullDisplay)
        //    job.EnableFullDisplay(kFullDisplay);
        job.EnableStorageOfResult(!kSkipResult);
        if (kWriteSummary) // Don't change flag set in SetNameSummaryFile
            job.EnableStorageOfSummary();
        //if (kWriteResult)  // Don't change flag set in SetNameSummaryFile
        //    job.EnableStorageOfResult();

        const Int_t rc = job.Process(seq);
        if (rc<=0)
        {
            gLog << err << "Calculation of cuts failed." << endl << endl;

            //error coding for the automatic analysis (to be filled into the database)
            switch (rc)
            {
            case  0:        // MJCut failed
                return 3;
            case -1:        // Source not found
                return 4;
            case -2:        // FillRndSrcCam failed
                return 5;
            case -3:        // Processing off-data failed
                return 6;
            case -4:        // Processing on-data failed
                return 7;
            }
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
