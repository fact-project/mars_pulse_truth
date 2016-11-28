/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: sinope.cc,v 1.14 2008-10-13 14:53:24 tbretz Exp $
! --------------------------------------------------------------------------
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz
!   Author(s): Daniela Dorner
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */
#include <errno.h>
#include <fstream>

#include <TROOT.h>
#include <TClass.h>
#include <TApplication.h>
#include <TObjectTable.h>

#include <TH1.h>
#include <TLine.h>
#include <TString.h>
#include <TVirtualPad.h>

#include "MRawRunHeader.h"
#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"
#include "MRawFileRead.h"
#include "MArrayD.h"
#include "MFTriggerPattern.h"
#include "MTriggerPatternDecode.h"
#include "MContinue.h"
#include "MTaskInteractive.h"
#include "MLog.h"
#include "MLogManip.h"
#include "MArgs.h"
#include "MSequence.h"
#include "MStatusDisplay.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"
#include "MRawFileRead.h"
#include "MDirIter.h"

using namespace std;

MRawEvtData   data;
MRawRunHeader header;

MArrayD artime;
MArrayD height(256);
Int_t   entries=0;
Int_t   events =0;

Int_t PreProcess(MParList *plist)
{
    artime.Set(header.GetNumSamplesHiGain() + header.GetNumSamplesLoGain());
    return kTRUE;
}

Int_t Process()
{
    events++;

    // This is the workaround to put hi- and lo-gains together
    const Int_t nhigain = header.GetNumSamplesHiGain();
    const Int_t nlogain = header.GetNumSamplesLoGain();

    const Int_t n = nhigain+nlogain;

    // Real Process
    MRawEvtPixelIter pixel(&data);

    Byte_t slices[n];

    while (pixel.Next())
    {
        // This is the fast workaround to put hi- and lo-gains together
        memcpy(slices,         pixel.GetHiGainSamples(), nhigain);
        memcpy(slices+nhigain, pixel.GetLoGainSamples(), nlogain);

        Byte_t *max = slices;
        Byte_t *min = slices;

        for (Byte_t *b=slices+1; b<slices+n; b++)
        {
            if (*b>=*max)
                max = b;
            if (*b<=*min)
                min = b;
        }

        const Int_t smax = max-slices;
        const Int_t diff = *max-*min;

        if (diff<50 || diff>235) // no-signal
            continue;

        height[diff]++;
        artime[smax]++;
        entries++;
    }
    return kTRUE;
}

Int_t GetFWHM(const TH1D &h, Int_t &min, Int_t &max)
{
    const Double_t hmax = h.GetMaximum()/2;
    const Int_t    bin  = h.GetMaximumBin();

    for (min=bin; min>1; min--)
        if (h.GetBinContent(min)<hmax)
            break;

    for (max=bin; max<h.GetNbinsX(); max++)
        if (h.GetBinContent(max)<hmax)
            break;

    return max-min;
}

TString kOutpath="";
TString kOutfile="";
MStatusDisplay *d=0;

Int_t PostProcess()
{
    if (entries==0)
    {
        gLog << warn << "No entries processed..." << endl;

        ofstream fout(Form("%stxt", kOutpath.Data()));
        if (!fout)
        {
            gLog << err << "Cannot open file: " << strerror(errno) << endl;
            return kERROR;
        }

        fout << "Events:         " << events << endl;
        fout << endl;

        return kTRUE;
    }

    TH1D h1("Arrival", "Arrival Time distribution for signals", artime.GetSize(), -0.5, artime.GetSize()-0.5);
    TH1D h2("Height",  "Pulse height distribution",             height.GetSize(), -0.5, height.GetSize()-0.5);
    h1.SetXTitle("Arrival Time [slice]");
    h2.SetXTitle("Pulse Height [cts]");
    h1.SetDirectory(0);
    h2.SetDirectory(0);
    h1.SetEntries(entries);
    h2.SetEntries(entries);
    memcpy(h1.GetArray()+1, artime.GetArray(), artime.GetSize()*sizeof(Double_t));
    memcpy(h2.GetArray()+1, height.GetArray(), height.GetSize()*sizeof(Double_t));

    TLine l;
    l.SetLineColor(kGreen);
    l.SetLineWidth(2);

    Int_t min, max;

    //MStatusDisplay *d = new MStatusDisplay;
    d->AddTab("Time");
    h1.DrawCopy();
    l.DrawLine(h1.GetMaximumBin()-1, 0, h1.GetMaximumBin()-1, h1.GetMaximum());
    const Int_t fwhm1  = GetFWHM(h1, min, max);
    const Bool_t asym1 = TMath::Abs((h1.GetMaximumBin()-min)-(max-h1.GetMaximumBin()))>fwhm1/2;;
    l.DrawLine(min-1, h1.GetMaximum()/2, max-1, h1.GetMaximum()/2);
    gPad->Update();

    d->AddTab("Pulse");
    h2.DrawCopy();
    l.DrawLine(h2.GetMaximumBin()-1, 0, h2.GetMaximumBin()-1, h2.GetMaximum());
    const Int_t fwhm2  = GetFWHM(h2, min, max);
    const Bool_t asym2 = TMath::Abs((h2.GetMaximumBin()-min)-(max-h2.GetMaximumBin()))>fwhm2/2;;
    l.DrawLine(min-1, h2.GetMaximum()/2, max-1, h2.GetMaximum()/2);
    gPad->Update();

    d->SaveAsRoot(Form("%sroot", kOutpath.Data()));

    ofstream fout(Form("%stxt", kOutpath.Data()));
    if (!fout)
    {
        gLog << err << "Cannot open file: " << strerror(errno) << endl;
        return kERROR;
    }

    fout << "Events:         " << events << endl;
    fout << "HasSignal:      " << (fwhm1>10?"no":"yes") << endl;
    fout << "HasPedestal:    " << (fwhm1<20?"no":"yes") << endl;
    fout << endl;
    fout << "PositionSignal: " << h1.GetMaximumBin()-1 << endl;
    fout << "PositionFWHM:   " << fwhm1 << endl;
    fout << "PositionAsym:   " << (asym1?"yes":"no") << endl;
    fout << endl;
    fout << "HeightSignal:   " << h2.GetMaximumBin()-1 << endl;
    fout << "HeightFWHM:     " << fwhm2 << endl;
    fout << "HeightAsym:     " << (asym2?"yes":"no") << endl;
    fout << endl;

    return kTRUE;
}

static void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "            Sinope - MARS V" << MARSVER          << endl;
    gLog << "    MARS -- SImple Non Online Pulse Evaluation"       << endl;
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
    gLog << " sinope [options] --run={number} --date={yyyy-mm-dd}" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   --run={number}:           Run number of run to process" << endl;
    gLog << "   --date={yy-mm-dd}:        Night the run belongs to" << endl << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << " Options:" << endl;
    gLog.Usage();
    //    gLog << "   --debug-env=0             Disable debugging setting resources <default>" << endl;
    //    gLog << "   --debug-env[=1]           Display untouched resources after program execution" << endl;
    //    gLog << "   --debug-env=2             Display untouched resources after eventloop setup" << endl;
    //    gLog << "   --debug-env=3             Debug setting resources from resource file" << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl << endl;
    gLog << endl;
    gLog << "   -q                        Quit when job is finished" << endl;
    gLog << "   -f                        Force overwrite of existing files" << endl;
    gLog << "   -dat                      Run sinope only for data events in the run" << endl;
    gLog << "   -cal                      Run sinope only for calibration events in the run" << endl;
    gLog << "   --ind=path                Path where to search for the data file" << endl;
    gLog << "                             [default=standard path in datacenter]" << endl;
    gLog << "   --out=path                Path to write the all results to [def=local path]" << endl;
    gLog << "   --outf=filename           Filename of the outputfiles [def=sinope{runnumber}.*]" << endl;
    gLog << "   --num={number}            Number of events to process (default=1000)" << endl;
    gLog << "   --print-seq               Print Sequence information" << endl;
    gLog << "   --print-files             Print Files taken from Sequence" << endl;
    gLog << "   --print-found             Print Files found from Sequence" << endl;
    //    gLog << "   --config=callisto.rc      Resource file [default=callisto.rc]" << endl;
    gLog << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl << endl;
    gLog << "Background:" << endl;
    gLog << " Sinope is  Jupiter's sixteenth moon.  Sinope is 28km in diameter and" << endl;
    gLog << " and orbits 23,700,000km from Jupiter.  Sinope has a mass of  8e16kg." << endl;
    gLog << " It orbits Jupiter in 758days and is in a retrograde orbit  (orbiting" << endl;
    gLog << " opposite to the direction of  Jupiter).  Very little  is known about" << endl;
    gLog << " Sinope.  Sinope was discovered by S.Nicholson in 1914." << endl << endl;
    gLog << "Example:" << endl;
    gLog << " sinope -f --date=2004-05-06 --run=32456" << endl;
    gLog << endl;
}

static void PrintFiles(const MSequence &seq, const TString &kInpathD, Bool_t ball)
{
    const char *prep = ball ? "Found" : "Scheduled";

    MDirIter Next;
    seq.SetupAllRuns(Next, kInpathD, kTRUE);

    gLog << all;
    gLog.Separator(Form("%s Files", prep));
    Next.Print(ball?"all":"");
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

    //const TString kConfig     = arg.GetStringAndRemove("--config=", "callisto.rc");

    const Bool_t  kPrintSeq   = arg.HasOnlyAndRemove("--print-seq");
    const Bool_t  kPrintFiles = arg.HasOnlyAndRemove("--print-files");
    const Bool_t  kPrintFound = arg.HasOnlyAndRemove("--print-found");
    const Bool_t  kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");
    //Int_t kDebugEnv = arg.HasOnlyAndRemove("--debug-env") ? 1 : 0;
    //kDebugEnv = arg.GetIntAndRemove("--debug-env=", kDebugEnv);

    const Bool_t  kQuit       = arg.HasOnlyAndRemove("-q");
    const Bool_t  kBatch      = arg.HasOnlyAndRemove("-b");
    const Bool_t  kOverwrite  = arg.HasOnlyAndRemove("-f");
    const Bool_t  kData       = arg.HasOnlyAndRemove("-dat");
    const Bool_t  kCal        = arg.HasOnlyAndRemove("-cal");
    //const Bool_t  kForceExec  = arg.HasOnlyAndRemove("-ff");

    const TString kInpathD    = arg.GetStringAndRemove("--ind=",  "");
    /*const TString*/ kOutpath    = arg.GetStringAndRemove("--out=",  "");
    /*const TString*/ kOutfile    = arg.GetStringAndRemove("--outf=",  "");

    const Int_t   kNumEvents  = arg.GetIntAndRemove("--num=", header.GetNumEvents());
    const Int_t   kRunNumber  = arg.GetIntAndRemove("--run=",  -1);
    const TString kDate       = arg.GetStringAndRemove("--date=", "");

    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - Unknown commandline options..." << endl;
        arg.Print("options");
        gLog << endl;
        Usage();
        return 2;
    }

    if (kRunNumber<0)
    {
        gLog << warn << "ERROR - No '--run=' option given... required." << endl;
        gLog << endl;
        Usage();
        return 2;
    }
    if (kDate.IsNull())
    {
        gLog << warn << "ERROR - No '--date=' option given... required." << endl;
        gLog << endl;
        Usage();
        return 2;
    }

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()>0)
    {
        Usage();
        return 2;
    }

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Setup sequence and check its validity
    //
    MSequence seq;
    seq.SetNight(kDate);
    seq.AddRuns(kRunNumber);
    if (kPrintSeq)
    {
        gLog << all;
        gLog.Separator();
        seq.Print();
        gLog << endl;
    }
    if (!seq.IsValid())
    {
        gLog << err << "Sequence invalid!" << endl << endl;
        return 2;
    }

    //
    // Process print options
    //
    if (kPrintFiles)
        PrintFiles(seq, kInpathD, kFALSE);
    if (kPrintFound)
        PrintFiles(seq, kInpathD, kTRUE);

    if (kOutpath.IsNull())
    {
        kOutpath = seq.GetStandardPath();
        kOutpath += "rawfiles/";
    }
    if (!kOutpath.EndsWith("/"))
        kOutpath += "/";
    if (kOutfile.IsNull())
        kOutpath += Form("sinope%08d.", kRunNumber);
    else
        kOutpath += kOutfile+".";

    if (!kOverwrite)
    {
        TString file = Form("%sroot", kOutpath.Data());
        if (!gSystem->AccessPathName(file, kFileExists))
        {
            gLog << err << "Sorry, file '" << file << "' exists... use -f option.." << endl;
            return 2;
        }
        file = Form("%stxt", kOutpath.Data());
        if (!gSystem->AccessPathName(file, kFileExists))
        {
            gLog << err << "Sorry, file '" << file << "' exists... use -f option.." << endl;
            return 2;
        }
    }

    //
    // Initialize root
    //
    MArray::Class()->IgnoreTObjectStreamer();
    MParContainer::Class()->IgnoreTObjectStreamer();

    TApplication app("sinope", &argc, argv);
    if (!gROOT->IsBatch() && !gClient || gROOT->IsBatch() && !kBatch)
    {
        gLog << err << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    // ----------------------------------------------------------

    /*MStatusDisplay **/d = new MStatusDisplay;

    // From now on each 'Exit' means: Terminate the application
    d->SetBit(MStatusDisplay::kExitLoopOnExit);
    d->SetTitle(Form("Sinope #%d", kRunNumber));

    MDirIter iter;
    seq.SetupAllRuns(iter, 0, kTRUE);

    MRawFileRead read;
    read.AddFiles(iter);

    MTaskInteractive task;

    MTriggerPatternDecode decode;
    MFTriggerPattern      ftp;
    if (kCal || kData)
    {
        ftp.SetDefault(kTRUE);
        ftp.DenyCalibration();
        if (!kCal)
        {
            ftp.DenyPedestal();
            ftp.SetInverted();
        }
    }
    MContinue conttp(&ftp, "ContTrigPattern");

    task.SetPreProcess(PreProcess);
    task.SetProcess(Process);
    task.SetPostProcess(PostProcess);

    MTaskList tlist;
    tlist.AddToList(&read);
    if (kCal || kData)
    {
        tlist.AddToList(&decode);
        tlist.AddToList(&conttp);
    }
    tlist.AddToList(&task);

    MParList  plist;
    plist.AddToList(&tlist);

    plist.AddToList(&data);
    plist.AddToList(&header);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    if (!evtloop.Eventloop(kNumEvents))
        return 1;

    if (!evtloop.GetDisplay())
    {
        gLog << warn << "Display closed by user... execution aborted." << endl << endl;
        return 0;
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
