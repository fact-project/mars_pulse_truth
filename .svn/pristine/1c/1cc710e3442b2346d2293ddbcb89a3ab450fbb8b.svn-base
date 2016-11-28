#include <TObjectTable.h>
#include <TClass.h>

#include "MArgs.h"
#include "MArray.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MJMerpp.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This is an easy implementation of the Merging process                    //
// (as compilable prog)                                                     //
//                                                                          //
// at the moment it reads a binary file ("rawtest.bin") which was written   //
// in the DAQ raw format.                                                   //
//                                                                          //
// The data are stored in root container objects (classes derived from      //
// TObject like MRawRunHeader)                                              //
//                                                                          //
// This containers are written to a root file ("rawtest.root")              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "                MERPP - MARS V" << MARSVER          << endl;
    gLog << "     MARS - Merging and Preprocessing Program"      << endl;
    gLog << "   Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    //                1         2         3         4         5         6         7         8
    //       12345678901234567890123456789012345678901234567890123456789012345678901234567890
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " merpp [options] [tel:]seqnumber  [outpath [inpath]]" << endl;
    gLog << " merpp [options] [tel:]seqnumber  outpath sumfile.txt" << endl;
    gLog << " merpp [options] seqfile.txt      [outpath [inpath]]" << endl;
    gLog << " merpp [options] seqfile.txt      outpath sumfile.txt" << endl;
//    gLog << " merpp [options] mysql           [outpath [inpath]]" << endl;
//    gLog << " merpp [options] mysql           outpath sumfile.txt" << endl;
    gLog << " merpp [options] infile.raw[.gz]  [outfile.root]" << endl;
    gLog << " merpp [options] infile.fits[.gz] [outfile.root]" << endl;
    gLog << " merpp [options] infile.rep       [outfile.root]" << endl;
    gLog << " merpp [options] infile.txt       outfile.root" << endl;
    gLog << endl;
    gLog << " Arguments:" << endl;
    gLog << "   [tel:]seqnumber           Telescope and sequence number to mbe merpped." << endl;
    gLog << "   inpath                    The input  path where the CC files   are." << endl;
    gLog << "   outpath                   The output path where the outputfile are or stored." << endl;
    gLog << "   sumfile.txt               A central control report summary file (--summary can be omitted)." << endl;
    gLog << "   infile.raw[.gz]           Magic DAQ binary file." << endl;
    gLog << "   infile.rep                Magic Central Control report file." << endl;
    gLog << "   infile.txt                Magic DC currents file." << endl;
    gLog << "   outfile.root              Merpped root file." << endl;
//    gLog << "   mysql                     mysql://user:password@host/database/tel:sequence" << endl;
    gLog << endl;
    gLog << " Options:" << endl;
    gLog.Usage();
//    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
//    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
//    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
//    gLog << "   --debug-env=3             Debug setting resources from resource file and command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << endl;
    gLog << " File Options:" << endl;
    gLog << "   -c#                       Compression level #=1..9 [default=2]" << endl;
    gLog << "   -f                        Force overwrite of an existing file" << endl;
    gLog << "   -u, --update              Update an existing file." << endl;
    gLog << endl;
    gLog << " Raw Data Options:" << endl;
    gLog << "   -ff                       Force merpp to ignore broken events and don't stop" << endl;
    gLog << "   --interleave=#            Process only each i-th event [default=1]" << endl;
    gLog << endl;
    gLog << " Report/Currents File Options:" << endl;
    gLog << "   --auto-time-start         Take time automatically from MRawRunHeader" << endl;
    gLog << "                                (overwrites --start=, update only)" << endl;
    gLog << "   --auto-time-stop          Take time automatically from MRawRunHeader" << endl;
    gLog << "                                (overwrites --stop=, update only)" << endl;
    gLog << "   --auto-time               Abbrev. for --auto-time-start and auto-time-stop" << endl;
    gLog << "   --start=\"time\"          Start event time (format see MTime::SetSqlDateTime, update only)" << endl;
    gLog << "   --stop=\"time\"           Stop  event time (format see MTime::SetSqlDateTime, update only)" << endl;
    gLog << endl;
    gLog << " Report Options:" << endl;
    gLog << "   --rep-run=#               Only data corresponding to this run number as" << endl;
    gLog << "                                taken from the RUN-REPORT is extracted" << endl;
    gLog << "   --rep-file=#              Only data corresponding to this file number as" << endl;
    gLog << "                                taken from the RUN-REPORT is extracted" << endl;
    gLog << "   --header-run=#            Allow only run-control .rep-files with this" << endl;
    gLog << "                                run number in there header" << endl;
    gLog << "   --header-file=#           Allow only run-control .rep-files with this" << endl;
    gLog << "                                file number in there header" << endl;
    gLog << "   --telescope=#             Allow only run-control .rep-files with this" << endl;
    gLog << "                                telescope number in there header" << endl;
    gLog << "   --sumfile                 Check for an all night summary file" << endl;
    gLog << "                                (from .rep header)" << endl;
    gLog << "   --allfiles                Don't check file type <default>" << endl << endl;
    gLog << "   --only=Name               Read only reports described by MReportName. See the" << endl;
    gLog << "                                mreport-directory for available classes." << endl;
    gLog << "   --dev-null                Suppress output of files (for test purpose)" << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl;
    gLog << endl;
    gLog << " Compatibility (deprecated):" << endl;
    gLog << "   --run=#                   See --rep-run (overwritten by --rep-run)" << endl;
    gLog << "   --runfile=#               See --header-run (overwritten by --header-run)" << endl << endl;
    gLog << " REMARK: - At the moment you can process a .raw _or_ a .rep file, only!" << endl;
    gLog << "         - 'date/time' has the format 'yyyy-mm-dd/hh:mm:ss.mmm'" << endl << endl;
}

static bool HasExtension(const TString &name)
{
    return
        name.EndsWith(".rep")  || name.EndsWith(".txt")    ||
        name.EndsWith(".raw")  || name.EndsWith(".raw.gz") ||
        name.EndsWith(".root") ||
        name.EndsWith(".fits") || name.EndsWith(".fits.gz");
}

int main(const int argc, char **argv)
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

    arg.RemoveRootArgs();

    const Bool_t  kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");
    const Int_t   kComprlvl      = arg.GetIntAndRemove("-c", 2);
    const Bool_t  kInterleave    = arg.GetIntAndRemove("--interleave=", 1);
    const Bool_t  kOverwrite     = arg.HasOnlyAndRemove("-f");
    const Bool_t  kForceProc     = arg.HasOnlyAndRemove("-ff");
    const Int_t   run            = arg.GetIntAndRemove("--run=", -1);
    const Int_t   kRunNumber     = arg.GetIntAndRemove("--rep-run=", run);
    const Int_t   kFileNumber    = arg.GetIntAndRemove("--rep-file=", -1);
    const Bool_t  kAutoTime      = arg.HasOnlyAndRemove("--auto-time");
    const Bool_t  kAutoTimeStart = arg.HasOnlyAndRemove("--auto-time-start") || kAutoTime;
    const Bool_t  kAutoTimeStop  = arg.HasOnlyAndRemove("--auto-time-stop")  || kAutoTime;
    const Int_t   runfile        = arg.GetIntAndRemove("--runfile=", -1);
          Int_t   kHeaderRun     = arg.GetIntAndRemove("--header-run=", runfile);
    const Int_t   kHeaderFile    = arg.GetIntAndRemove("--header-file=", -1);
    const Int_t   kTelescope     = arg.GetIntAndRemove("--telescope=", -1);
          Bool_t  kUpdate        = arg.HasOnlyAndRemove("--update") || arg.HasOnlyAndRemove("-u");
    const TString kOnly          = arg.GetStringAndRemove("--only", "");
    const Bool_t  kNullOut    = arg.HasOnlyAndRemove("--dev-null");

//    Int_t  kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
//    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    MTime kTimeStart(arg.GetStringAndRemove("--start="));
    MTime kTimeStop(arg.GetStringAndRemove("--stop="));

    if (arg.HasOnlyAndRemove("--sumfile"))
        kHeaderRun  = 0;

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
    if (arg.GetNumArguments()<1 || arg.GetNumArguments()>3)
    {
        Usage();
        return 2;
    }

    //
    // This is to make argv[i] more readable insidethe code
    //
    const Int_t narg = arg.GetNumArguments();

    const TString arg0 = arg.GetArgumentStr(0);
    const TString arg1 = arg.GetArgumentStr(1);
    const TString arg2 = arg.GetArgumentStr(2);

    const Bool_t isseq =
        (narg==1 && arg0.EndsWith(".txt")) ||
        (narg==2 && !HasExtension(arg1))   ||
        (narg==3);

    TString sequence, kNamein, kNameout;
    if (isseq)
    {
        sequence = arg0;
        kNameout = arg1;
        kNamein  = arg2;

        if (arg2.EndsWith(".txt"))  // set --summary
        {
            gLog << inf << "Summary file option switched on automatically due to file extension." << endl;
            kHeaderRun = 0;
        }

        kUpdate = kTRUE;
    }
    else
    {
        kNamein  = arg0;
        kNameout = arg1.IsNull() ? arg0(0, arg0.Last('.')) : arg1(0, arg1.Last('.'));

        if (!kNameout.EndsWith(".root"))
            kNameout += ".root";
    }

    //
    // Initialize Non-GUI (batch) mode
    //
    gROOT->SetBatch();

    //
    // Ignore TObject Streamer (bits, uniqueid) for MArray and MParContainer
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    {
        MJMerpp merpp(Form("MJMerpp %s", gSystem->BaseName(arg0.Data())));
        merpp.SetOverwrite(kOverwrite);
        merpp.SetCompression(kComprlvl);
        merpp.SetUpdate(kUpdate);
        merpp.SetInterleave(kInterleave);
        merpp.SetForceProcessing(kForceProc);
        merpp.SetConstrainHeader(kTelescope, kHeaderRun, kHeaderFile);
        merpp.SetConstrainRunRep(kRunNumber, kFileNumber);
        merpp.SetOnly(kOnly);
        merpp.SetTime(kTimeStart, kTimeStop);
        merpp.SetAutoTime(kAutoTimeStart, kAutoTimeStop);
        merpp.SetNullOut(kNullOut);
        //merpp.SetEnvDebug(kDebugEnv);
        //merpp.SetEnv(&env);
        //merpp.SetDisplay(d);;

        merpp.SetPathIn(kNamein);
        merpp.SetPathOut(kNameout);

        const Int_t rc = sequence.IsNull() ? merpp.Process() : merpp.ProcessSeq(sequence);
        if (rc>0)
        {
            gLog << err << "Merpp failed." << endl << endl;
            return rc;
        }

        //if (kDebugEnv>0)
        //    env.PrintUntouched();
    }

    if (TObject::GetObjectStat())
    {
        TObject::SetObjectStat(kFALSE);
        gObjectTable->Print();
    }

    return 0;
}
