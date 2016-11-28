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
#include "MJPedestal.h"
#include "MJCalibration.h"
#include "MJCalibrateSignal.h"
#include "MJCalibTest.h"

using namespace std;

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "========================================================" << endl;
    gLog << "                 Callisto - MARS V" << MARSVER            << endl;
    gLog << "    MARS -- CALibrate LIght Signals and Time Offsets"     << endl;
    gLog << "      Compiled with ROOT v" << ROOT_RELEASE << " on <" << __DATE__ << ">" << endl;
    gLog << "========================================================" << endl;
    gLog << endl;
}

static void Usage()
{
    //                1         2         3         4         5         6         7         8
    //       12345678901234567890123456789012345678901234567890123456789012345678901234567890
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " callisto [-c] [-y] [options] sequence.txt|[tel:]number" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   sequence.txt:             ASCII file defining a sequence of runs" << endl;
    gLog << "   number:                   The sequence number (using file in the datacenter)" << endl;
    gLog << "                             For more details see MSequence" << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << " Operation Modes:" << endl;
    gLog << "   -c                        Calculate the calibration constants" << endl;
    gLog << "   -y                        Extract and calibrate signal" << endl << endl;
    gLog << " Data Type (exclusive):" << endl;
    gLog << "   -mc                       Input root-files are monte carlo files" << endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
    gLog << "   --debug-env=3             Debug setting resources from resource file and" << endl;
    gLog << "                             command line" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << "   --rc=Name:option          Set or overwrite a resource of the resource file." << endl;
    gLog << "                             (Note, that this option can be used multiple times." << endl;
    gLog << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
    gLog << "   --ind=path                Path where to search for the data files" << endl;
    gLog << "                             [default=standard path in datacenter]" << endl;
    gLog << "   --iny=path                Path where to search for the calibration files" << endl;
    gLog << "                             [default=local path or output path of Mode-C]" << endl;
    gLog << "   --outc=path               Path to write Mode-C result to   [def=local path]" << endl;
    gLog << "   --outy=path               Path to write Mode-Y result to   [def=local path]" << endl;
    gLog << "   --out=path                Path to write the all results to [def=local path]" << endl;
    gLog << "                             (overwrites --outc and --outy)" << endl;
    gLog << "   --path=path               Path to write the all results to [def=local path]" << endl;
    gLog << "                             (overwrites --iny, --outc and --outy)" << endl;
    gLog << "   --dev-null                Suppress output of Y-files (for test purpose)" << endl;
    gLog << "   --print-seq               Print Sequence information" << endl;
    gLog << "   --print-files             Print Files taken from Sequence" << endl;
    gLog << "   --print-found             Print Files found from Sequence" << endl;
//    gLog << "   --use-test                Apply calibration constants to same calibration" << endl;
//    gLog << "                             file (for testing, calibration mode only)" << endl;
    gLog << "   --movie                   Write a movie in addition to Mode-Y (this might " << endl;
    gLog << "                             stop the eventloop before all evts are processed)" << endl;
    gLog << "   --test                    Use this to calibrate the callibration files" << endl;
    gLog << "                             instead of the data files (for test purposes)" << endl;
//    gLog << "   --moon                    Force using pedestal fits instead of calculated RMS" << endl;
    gLog << "   --config=callisto.rc      Resource file [default=callisto.rc]" << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
    gLog << " Setup of the two jobs run by callisto (MJPedestal and MJCalibration)" << endl;
    gLog << " can be done with the resource file. See MJPedestal and MJCalibration" << endl;
    gLog << " especially CheckEnv() for more details.   Command line options might" << endl;
    gLog << " be overwritten by the resource file." << endl << endl;
    gLog << " If running in Mode-C and Mode-Y --iny= is obsolete,  instead --outc=" << endl;
    gLog << " is used." << endl << endl;
    gLog << " Using  --iny=,  --outc=,  --outy=,  --out=  or --path= automatically" << endl;
    gLog << " enables the corresponding modes.  In this case  -c/-y are obsolete." << endl << endl;
    gLog << "Description:" << endl;
    gLog << " callisto will calculate pedestals  from  pedestal-files defined in a" << endl;
    gLog << " sequence-file.  This pedestals are used to calculate the calibration" << endl;
    gLog << " constants. These constants are stored in a so called calibration-file" << endl;
    gLog << " together with some datacheck plots  which can be viewed using either" << endl;
    gLog << " showplot or MStatusDisplay in the interpreter.  A description  of  a" << endl;
    gLog << " sequence-file can be found in the class reference of MSequence." << endl << endl;
    gLog << "Background:" << endl;
    gLog << " Callisto is a large,  icy,  dark-colored,  low-density outer moon of" << endl;
    gLog << " Jupiter that is scarred with  impact craters  and  ejecta.  It has a" << endl;
    gLog << " diameter of about 4800km, the second-largest moon of Jupiter;  it is" << endl;
    gLog << " roughly the size of  Mercury.  Callisto has the largest-known impact" << endl;
    gLog << " crater in the Solar System, Valhalla, which has a bright patch 600km" << endl;
    gLog << " across  and  rings that go out  to almost  3000km.  Callisto  orbits" << endl;
    gLog << " Jupiter at a mean distance of 1,883,000km. Its mass is 1.1e23 kg. It" << endl;
    gLog << " takes Callisto  16.7 days to orbit  Jupiter  in a synchronous orbit." << endl;
    gLog << " Jupiter's moon Callisto was discovered independently by  Galileo and" << endl;
    gLog << " S.Marius in 1610." << endl << endl;
    gLog << "Example:" << endl;
    gLog << " callisto -f --outc=mycal/ --outy=mysignal/ --log sequence02345.txt" << endl;
    gLog << endl;
}

static void PrintFiles(const MSequence &seq, const TString &kInpathD, Bool_t showall)
{
    const char *prep = showall ? "Found" : "Scheduled";

    MDirIter Next1, Next2, Next3;
    seq.GetRuns(Next1, MSequence::kRawPed, kInpathD);
    seq.GetRuns(Next2, MSequence::kRawCal, kInpathD);
    seq.GetRuns(Next3, MSequence::kRawDat, kInpathD);

    gLog << all;
    gLog.Separator(Form("%s Pedestal Files", prep));
    Next1.Print(showall?"all":"");
    gLog << endl;
    gLog.Separator(Form("%s Calibration Files", prep));
    Next2.Print(showall?"all":"");
    gLog << endl;
    gLog.Separator(Form("%s Data Files", prep));
    Next3.Print(showall?"all":"");
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

    const TString kConfig     = arg.GetStringAndRemove("--config=", "callisto.rc");

    const Bool_t  kPrintSeq   = arg.HasOnlyAndRemove("--print-seq");
    const Bool_t  kPrintFiles = arg.HasOnlyAndRemove("--print-files");
    const Bool_t  kPrintFound = arg.HasOnlyAndRemove("--print-found");
//    const Bool_t  kUseTest    = arg.HasOnlyAndRemove("--use-test");
    const Bool_t  kMovie      = arg.HasOnlyAndRemove("--movie");
    const Bool_t  kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");
    const Bool_t  kNullOut    = arg.HasOnlyAndRemove("--dev-null");
    const Bool_t  kTestMode   = arg.HasOnlyAndRemove("--test");
//          Bool_t  kMoon       = arg.HasOnlyAndRemove("--moon");
    Int_t kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kQuit       = arg.HasOnlyAndRemove("-q");
    const Bool_t  kBatch      = arg.HasOnlyAndRemove("-b");
    const Bool_t  kOverwrite  = arg.HasOnlyAndRemove("-f");
    //const Bool_t  kForceExec  = arg.HasOnlyAndRemove("-ff");

    const TString kInpathD    = arg.GetStringAndRemove("--ind=",  "");
          TString kInpathY    = arg.GetStringAndRemove("--iny=",  "");
          TString kOutpathY   = arg.GetStringAndRemove("--outy=", "");
          TString kOutpathC   = arg.GetStringAndRemove("--outc=", "");
    const TString kOutpath    = arg.GetStringAndRemove("--out=",  "");
    const TString kPath       = arg.GetStringAndRemove("--path=", "");

          Bool_t  kModeC      = arg.HasOnlyAndRemove("-c");
          Bool_t  kModeY      = arg.HasOnlyAndRemove("-y");
    const Bool_t  kIsMC       = arg.HasOnlyAndRemove("-mc");

    if (!kInpathY.IsNull() || !kOutpathY.IsNull() || !kOutpath.IsNull() || !kPath.IsNull())
        kModeY = kTRUE;
    if (!kOutpathC.IsNull() || !kOutpath.IsNull() || !kPath.IsNull())
        kModeC = kTRUE;

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
    MEnv env(kConfig, "callisto.rc");
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

    if (!kModeC && !kModeY /*&& !kUseTest*/)
    {
        gLog << err << "Neither calibration (-c), signal extraction (-y) or test-mode (--use-test)" << endl;
        gLog <<        "specified! You can also give the operation mode by the options defining the" << endl;
        gLog <<        "output path --outc, --outy or --out." << endl;
        Usage();
        return 2;
    }
    if (!kModeY && kMovie)
    {
        gLog << err << "Signal extraction mode (-y) not switched on but movie requested." << endl;
        Usage();
        return 2;
    }

    if ((kModeC/* || kUseTest*/) && kOutpathC.IsNull())
        kOutpathC = ".";
    if (kModeY)
    {
        if (kInpathY.IsNull())
            kInpathY = ".";
        if (kOutpathY.IsNull())
            kOutpathY = ".";
    }
    if (!kOutpath.IsNull())
    {
        kOutpathC = kOutpath;
        kOutpathY = kOutpath;
    }
    if (!kPath.IsNull())
    {
        kOutpathC = kOutpath;
        kOutpathY = kOutpath;
        kInpathY  = kOutpath;
    }

    if (kModeC && kModeY)
        kInpathY = kOutpathC;

    //
    // Setup sequence file and check for its existance
    //
    TString kSequence = arg.GetArgumentStr(0);

    if (!MSequence::InflateSeq(kSequence, kIsMC))
        return 3;

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Setup sequence and check its validity
    //
    MSequence seq(kSequence, kInpathD);
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
        return 5;
    }

    //
    // Process print options
    //
    if (kPrintFiles)
        PrintFiles(seq, kInpathD, kFALSE);
    if (kPrintFound)
        PrintFiles(seq, kInpathD, kTRUE);

//    if (seq.HasMoon())
//        kMoon = kTRUE;


    /*
    //
    // Check for existance of all files
    //
    MDirIter iter;
    const Int_t n0 = seq.SetupAllRuns(iter, kInpathD, "[DPC]");
    const Int_t n1 = seq.GetNumAllRuns();
    if (n0 != n1)
    {
        if (kForceExec)
            gLog << warn << "WARNING";
        else
            gLog << err << "ERROR";
        gLog << " - " << n1 << " files in sequence defined, but " << n0 << " found in ";
        gLog << (kInpathD.IsNull() ? "<defaultpath>" : kInpathD.Data()) << endl;
        if (!kForceExec)
            return 2;
    }

    if (kPrintOnly)
        return 0;
  */
    //
    // Initialize root
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    // FIXME: Remove all non-root arguments and options from argc, argv

    TApplication app("callisto", &argc, argv);
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
    d->SetTitle(Form("-- Callisto: %s --", kSequence.Data()));

    if (kModeC/* || kUseTest*/)
    {
        //
        // Calculate pedestal for pedestal-calculation and calibration
        //
        MJPedestal job1(Form("MJPedestalC1 #%d", seq.GetSequence()));
        job1.SetNoStorage();
        job1.SetSequence(seq);
        job1.SetEnv(&env);
        job1.SetEnvDebug(kDebugEnv);
        job1.SetDisplay(d);
        job1.SetOverwrite(kOverwrite);
//        job1.SetUseHists(kMoon);


        job1.SetExtractionFundamental();
        // job1.SetPathOut(kOutpathC); // not yet needed
        // job1.SetPathIn(kInpathC);   // DO NOT CALL IT. IT WOULD READ THE XEXTRACTOR FROM calib-file

        if (!job1.Process())
        {
            gLog << err << "Calculation of pedestal failed." << endl << endl;
            return 6;
        }

        if (!job1.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }

        //
        // Calculate pedestal and pedestal resolution
        //
        MJPedestal job2(Form("MJPedestalC2 #%d", seq.GetSequence()));
        job2.SetNoStorage();
        job2.SetSequence(seq);
        job2.SetEnv(&env);
        job2.SetEnvDebug(kDebugEnv);
        job2.SetDisplay(d);;
        job2.SetOverwrite(kOverwrite);
//        job2.SetUseHists(kMoon);
        job2.SetDeadPixelCheck();
        // job1.SetPathOut(kOutpathC); // not yet needed
        // job1.SetPathIn(kInpathC);   // DO NOT CALL IT. IT WOULD READ THE XEXTRACTOR FROM calib-file

        //job2.SetExtractorResolution();
        job2.SetExtractionWithExtractorRndm();
        job2.SetExtractor(job1.GetExtractor());
        job2.SetPedestals(job1.GetPedestalCam());
        job2.SetBadPixels(job1.GetBadPixels());

        // Please check the Changelog of 2005/04/20 about further deatils of the next comment
        //if (job1.GetExtractor().InheritsFrom("MExtractTimeAndCharge"))
        if (!job2.Process())
        {
            gLog << err << "Calculation of pedestal resolution failed." << endl << endl;
            return 7;
        }

        if (!job2.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }

        //
        // Do calibration
        //
        MJCalibration job3(Form("MJCalibration #%d", seq.GetSequence()));
        job3.SetSequence(seq);
        job3.SetEnv(&env);
        job3.SetEnvDebug(kDebugEnv);
        job3.SetDisplay(d);
        job3.SetOverwrite(kOverwrite);
        job3.SetPathOut(kOutpathC);
        job3.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        // job2.SetPathIn(kInpathC); // not yet needed

        job3.SetBadPixels(job2.GetBadPixels());
        job3.SetExtractor(job2.GetExtractor());
        job3.SetExtractorCam(job2.GetPedestalCam());

        if (!job3.Process(job1.GetPedestalCam()))
        {
            gLog << err << "Calculation of calibration failed." << endl << endl;
            return 8;
        }

        if (kDebugEnv>0)
            env.PrintUntouched();

        if (!job3.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }
/*
        if (kUseTest)
        {
            MJCalibTest job4(Form("MJCalibTest #%d", seq.GetSequence()));
            job4.SetBadPixels(job2.GetBadPixels());
            job4.SetSequence(seq);
            job4.SetEnv(&env);
            job4.SetEnvDebug(kDebugEnv);
            job4.SetDisplay(d);;
            job4.SetOverwrite(kOverwrite);
            job4.SetPathOut(kOutpathC);

            if (!job4.Process(job1.GetPedestalCam()))
            {
                gLog << err << "Calibration of calibration failed." << endl << endl;
                return 9;
            }

            if (kDebugEnv>0)
                env.PrintUntouched();

            if (!job4.GetDisplay())
            {
                gLog << warn << "Display closed by user... execution aborted." << endl << endl;
                return 1;
            }
        }*/
    }

    if (kModeY)
    {
        d->Reset();

        //
        // Calculate starting pedestal for data extraction
        //
        MJPedestal job1(Form("MJPedestalY1 #%d", seq.GetSequence()));
        job1.SetNoStorage();
        job1.SetSequence(seq);
        job1.SetEnv(&env);
        job1.SetEnvDebug(kDebugEnv);
        job1.SetDisplay(d);
        job1.SetNoDisplay();
        job1.SetOverwrite(kOverwrite);
        job1.SetPathIn(kInpathY);   // --> READ Extractor from calib-file
        //job1.SetPathOut(kOutpathY);   // not yet needed
        job1.SetUseData();
        job1.SetExtractionFundamental();

        if (!job1.Process())
        {
            gLog << err << "Calculation of fundamental pedestal failed." << endl << endl;
            return 10;
        }

        if (!job1.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }

        //
        // Calculate pedestal and pedestal resolution
        //
        MJPedestal job2(Form("MJPedestalY2 #%d", seq.GetSequence()));
        job2.SetNoStorage(kMovie);
        job2.SetSequence(seq);
        job2.SetEnv(&env);
        job2.SetEnvDebug(kDebugEnv);
        job2.SetDisplay(d);
        job2.SetNoDisplay();
        job2.SetOverwrite(kOverwrite);
        job2.SetPathIn(kInpathY);
        job2.SetPulsePosCheck(!seq.IsMonteCarlo());
        job2.SetPathOut(kOutpathY); // for updating the extractor

        job2.SetUseData();
        job2.SetExtractionWithExtractorRndm();
        job2.SetExtractor(job1.GetExtractor());
        job2.SetPedestals(job1.GetPedestalCam());
        job2.SetBadPixels(job1.GetBadPixels());
//        job2.SetUseHists(kMoon);

        // Please check the Changelog of 2005/04/20 about further deatils of the next comment
        //if (job1.GetExtractor().InheritsFrom("MExtractTimeAndCharge"))
        const Int_t rc = job2.Process();
        if (rc<=0)
        {
            // rc==0:  Error
            // rc==-1: too much to the left
            // rc==-2: too much to the right
            // rc==-3: too much to the right (beyond limits)

            gLog << err << "Calculation of pedestal from extractor (random) failed." << endl << endl;

            //error coding for the automatic analysis (to be filled into the database)
            switch (rc)
            {
            case 0:
                return 11;
            case -1:
                return 12;
            case -2:
                return 13;
            case -3:
                return 14;
            }
            return 2;
        }

        if (!job2.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }

        //
        // Calculate pedestal and pedestal resolution
        //
        MJPedestal job3(Form("MJPedestalY3 #%d", seq.GetSequence()));
        job3.SetNoStorage();
        job3.SetSequence(seq);
        job3.SetEnv(&env);
        job3.SetEnvDebug(kDebugEnv);
        job3.SetDisplay(d);
        job3.SetNoDisplay();
        job3.SetOverwrite(kOverwrite);
        job3.SetPathIn(kInpathY);
        // job1.SetPathOut(kOutpathC); // not yet needed
        // job1.SetPathIn(kInpathC);   // not yet needed

        job3.SetUseData();
        job3.SetExtractionWithExtractor();
        job3.SetExtractor(job2.GetExtractor());
        job3.SetPedestals(job1.GetPedestalCam());
        job3.SetBadPixels(job1.GetBadPixels());
//        job3.SetUseHists(kMoon);

        if (!job3.Process())
        {
            gLog << err << "Calculation of pedestal from extractor failed." << endl << endl;
            return 15;
        }

        if (!job3.GetDisplay())
        {
            gLog << warn << "Display closed by user... execution aborted." << endl << endl;
            return 1;
        }

        //
        // Extract signal and calibrate it
        //
        MJCalibrateSignal job4(Form("MJCalibrateSignal #%d", seq.GetSequence()));
        job4.SetSequence(seq);
        job4.SetDisplay(d);;
        job4.SetEnv(&env);
        job4.SetEnvDebug(kDebugEnv);
        job4.SetOverwrite(kOverwrite);
        job4.SetPathIn(kInpathY);
        job4.SetPathOut(kOutpathY);
        job4.SetMovieMode(kMovie);
        job4.SetNullOut(kNullOut);
        job4.SetCommandLine(MArgs::GetCommandLine(argc, argv));
        job4.SetTestMode(kTestMode);
        if (!seq.IsMonteCarlo())
            job4.SetExtractor(job2.GetExtractor());

        // Where to search for calibration files
        if (!job4.Process(job1.GetPedestalCam(), job3.GetPedestalCam(), job2.GetPedestalCam()))
            return 16;

        if (kDebugEnv>0)
            env.PrintUntouched();

        if (!job4.GetDisplay())
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

