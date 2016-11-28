/* ======================================================================== *\
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
!   Author(s): Thomas Bretz, 1/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJCut
//
/////////////////////////////////////////////////////////////////////////////
#include "MJCut.h"

// Root
#include <TEnv.h>
#include <TFile.h>
#include <TClass.h>

// Environment
#include "MLog.h"
#include "MLogManip.h"

// Eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// Display
#include "MStatusDisplay.h"

// Tasks
#include "MReadReports.h"
#include "MReadMarsFile.h"
#include "MPrint.h"
#include "MContinue.h"
#include "MTaskEnv.h"
#include "MPointingDevCalc.h"
#include "MParameterCalc.h"
#include "MSrcPosRndm.h"
#include "MSrcPosCalc.h"
#include "MSrcPosCorrect.h"
#include "MHillasCalc.h"
#include "MFillH.h"
#include "MWriteRootFile.h"

// Filter
#include "MFDataPhrase.h"

// Fit signal environment
#include "../mhflux/MAlphaFitter.h"
#include "../mhflux/MHAlpha.h"

// Containers
#include "MH3.h"
#include "MBinning.h"
#include "MDataSet.h"
#include "MParameters.h"
#include "MPointingPos.h"
#include "MObservatory.h"
#include "MHSrcPosCam.h"

ClassImp(MJCut);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.  Set defaults for fStoreSummary, fStoreresult,
// fWriteOnly, fFullDisplay to kFALSE and initialize
// /*fEstimateEnergy and*/ fCalcHadronness with NULL.
//
MJCut::MJCut(const char *name, const char *title)
    : fStoreSummary(kFALSE), fStoreResult(kTRUE), fWriteOnly(kFALSE),
    fFullDisplay(kTRUE), fRndmSrcPos(kFALSE), fNumOffSourcePos(3),
    fNameHist("MHThetaSq"),
    fCalcHadronness(0), fCalcDisp(0), fEstimateEnergy(0)
{
    fName  = name  ? name  : "MJCut";
    fTitle = title ? title : "Standard program to perform g/h-separation cuts";
}

// --------------------------------------------------------------------------
//
// Destructor. Delete fEstimateEnergy and fCalcHadronness if != NULL
//
MJCut::~MJCut()
{
    if (fEstimateEnergy)
        delete fEstimateEnergy;
    if (fCalcHadronness)
        delete fCalcHadronness;
    if (fCalcDisp)
        delete fCalcDisp;
}

// --------------------------------------------------------------------------
//
// Set the name of the summary file (events after cut0)
// If you give a name the storage of this file is enabled implicitly.
// If you give no filename the storage is neither enabled nor disabled,
// but the storage file name is reset.
// If no filename is set the default filename is used.
// You can explicitly enable or disable the storage using EnableStoreOf*()
// The default argument is no filename.
//
void MJCut::SetNameSummaryFile(const char *name)
{
    fNameSummary=name;
    if (!fNameSummary.IsNull())
        EnableStorageOfSummary();
}

// --------------------------------------------------------------------------
//
// Set the name of the summary file (events after cut3)
// If you give a name the storage of this file is enabled implicitly.
// If you give no filename the storage is neither enabled nor disabled,
// but the storage file name is reset.
// If no filename is set the default filename is used.
// You can explicitly enable or disable the storage using EnableStoreOf*()
// The default argument is no filename.
//
void MJCut::SetNameResultFile(const char *name)
{
    fNameResult=name;
    if (!fNameResult.IsNull())
        EnableStorageOfResult();
}

// --------------------------------------------------------------------------
//
// Setup a task estimating the energy. The given task is cloned.
//
/*
void MJCut::SetEnergyEstimator(const MTask *task)
{
    if (fEstimateEnergy)
        delete fEstimateEnergy;
    fEstimateEnergy = task ? (MTask*)task->Clone() : 0;
}
*/

// --------------------------------------------------------------------------
//
// Setup a task calculating the hadronness. The given task is cloned.
//
void MJCut::SetHadronnessCalculator(const MTask *task)
{
    if (fCalcHadronness)
        delete fCalcHadronness;
    fCalcHadronness = task ? (MTask*)task->Clone() : 0;
}

// --------------------------------------------------------------------------
//
// Setup a task calculating disp. The given task is cloned.
//
void MJCut::SetDispCalculator(const MTask *task)
{
    if (fCalcDisp)
        delete fCalcDisp;
    fCalcDisp = task ? (MTask*)task->Clone() : 0;
}

// --------------------------------------------------------------------------
//
// Setup a task estimating the eneryg. The given task is cloned.
//
void MJCut::SetEnergyEstimator(const MTask *task)
{
    if (fEstimateEnergy)
        delete fEstimateEnergy;
    fEstimateEnergy = task ? (MTask*)task->Clone() : 0;
}

// --------------------------------------------------------------------------
//
// return fOutputPath+"/ganymed%08d.root", num
//
TString MJCut::GetOutputFile(UInt_t num) const
{
    TString p(fPathOut);
    p += "/";
    p += fNameOutput.IsNull() ? Form("ganymed%08d.root", num) : fNameOutput.Data();
    gSystem->ExpandPathName(p);
    return p;
}

/*
Bool_t MJCut::ReadTasks(const char *fname, MTask* &env1, MTask* &env2) const
{
    //    const TString fname = Form("%s/calib%08d.root", fPathIn.Data(), fSequence.GetSequence());

    *fLog << inf << "Reading from file: " << fname << endl;

    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << dbginf << "ERROR - Could not open file " << fname << endl;
        return kFALSE;
    }

    TObject *o = file.Get("EstimateEnergy");
    if (o && !o->InheritsFrom(MTask::Class()))
    {
        *fLog << err << dbginf << "ERROR - EstimateEnergy read from " << fname << " doesn't inherit from MTask!" << endl;
        return kFALSE;
    }
    env1 = o ? (MTask*)o->Clone() : FNULL;

    o = file.Get("CalcHadronness");
    if (o && !o->InheritsFrom(MTask::Class()))
    {
        *fLog << err << dbginf << "ERROR - CalcHadronness read from " << fname << " doesn't inherit from MTask!" << endl;
        return kFALSE;
    }
    env2 = o ? (MTask*)o->Clone() : NULL;

    return kTRUE;
}
*/

// --------------------------------------------------------------------------
//
// Write the tasks in cont to the file corresponding to analysis number num,
// see GetOutputFile()
//
Bool_t MJCut::WriteTasks(UInt_t num, TObjArray &cont) const
{
    if (fPathOut.IsNull())
    {
        *fLog << inf << "No output path specified via SetPathOut - no output written." << endl;
        return kTRUE;
    }

    const TString oname(GetOutputFile(num));

    *fLog << inf << "Writing to file: " << oname << endl;

    TFile *file = 0;
    if (fNameResult.IsNull() && fStoreResult)
    {
        // If no special name fNameResult is given but the result should be stored,
        // search for a file named oname (fNameOutput or ganymed%08d.root)
        file = (TFile*)gROOT->GetListOfFiles()->FindObject(oname);
        if (!file)
        {
            *fLog << err << "ERROR - File " << oname << " expected to be open, but isn't..." << endl;
            return kFALSE;
        }

        file->cd();
    }
    else
    {
        // If special name fNameResult is given or no storage requested,
        // open a new file named oname (fNameOutput or ganymed%08d.root)
        file = TFile::Open(oname, fOverwrite?"RECREATE":"NEW", "File created by MJCut", 9);

        if (!file)
        {
            *fLog << err << "ERROR - Couldn't open new file " << oname << " for writing..." << endl;
            return kFALSE;
        }
    }

    // Write container to output file
    const Bool_t rc = WriteContainer(cont);

    // If file was newly opened, close it
    if (!(fNameResult.IsNull() && fStoreResult))
        delete file;

    // Return result of writing
    return rc;
}

// --------------------------------------------------------------------------
//
// Write the result plots and other results to the file corresponding to
// analysis number num, see GetOutputFile()
//
Bool_t MJCut::WriteResult(const MParList &plist, const MDataSet &set) const
{
    TObjArray arr;

    // Save all MBinnings
    TIter Next(plist);
    TObject *o=0;
    while ((o=Next()))
        if (o->InheritsFrom(MBinning::Class()))
            arr.Add(o);

    // Save also the result, not only the setup
    const MHAlpha *halpha = (MHAlpha*)plist.FindObject("Hist", "MHAlpha");
    if (halpha)
        arr.Add((TObject*)(&halpha->GetAlphaFitter()));

    // Save also the dataset, environment and display
    arr.Add(const_cast<MDataSet*>(&set));
    arr.Add(const_cast<TEnv*>(GetEnv()));
    if (fDisplay)
        arr.Add(fDisplay);

    TNamed cmdline("CommandLine", fCommandLine.Data());
    arr.Add(&cmdline);

    const Int_t num = set.GetNumAnalysis();
    const TString fname(fNameOutput.IsNull() ? Form("ganymed%08d.root", num) : fNameOutput.Data());

    // If requested, write to already open output file
    if (fNameResult.IsNull() && fStoreResult)
    {
        *fLog << inf2 << "Searching for open file " << fname << "... " << flush;

        TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(fname);
        if (file)
        {
            *fLog << "found." << endl;
            file->cd();
            return WriteContainer(arr);
        }
        *fLog << "not found." << endl;
    }

    return WriteContainer(arr, fname, "UPDATE");
}

// --------------------------------------------------------------------------
//
// MJCut allows to setup several option by a resource file:
//   MJCut.WriteSummary: yes, no
//   MJCut.SummaryFile:  filename
//   MJCut.WriteResult:  yes, no
//   MJCut.ResultFile:   filename
//   MJCut.HistName:     MHAlpha
//
Bool_t MJCut::CheckEnvLocal()
{
    const TString f0(GetEnv("SummaryFile", ""));
    const TString f1(GetEnv("ResultFile",  ""));
    if (!f0.IsNull())
        SetNameSummaryFile(f0);
    if (!f1.IsNull())
        SetNameResultFile(f1);

    EnableStorageOfSummary(GetEnv("SummaryFile", fStoreSummary));
    EnableStorageOfResult(GetEnv("ResultFile", fStoreResult));
    EnableFullDisplay(GetEnv("FullDisplay", fFullDisplay));
    EnableRandomSrcPos(GetEnv("RandomSourcePosition", fRndmSrcPos));

    fNumOffSourcePos = GetEnv("NumOffSourcePositions", (Int_t)fNumOffSourcePos);
    //EnableSubstraction(GetEnv("HistogramSubstraction", fSubstraction));

    SetNameHist(GetEnv("NameHist", fNameHist));
    SetNameHistFS(GetEnv("NameHistFS", fNameHistFS));

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Setup write to write:
//     container         tree       optional?
//  --------------     ----------  -----------
//   "MHillas"      to  "Events"
//   "MHillasSrc"   to  "Events"
//   "Hadronness"   to  "Events"       yes
//   "MEnergyEst"   to  "Events"       yes
//   "DataType"     to  "Events"
//
void MJCut::SetupWriter(MWriteRootFile *write, const char *name) const
{
    if (!write)
        return;

    write->SetName(name);
    write->AddContainer("MHillas",        "Events");
    write->AddContainer("MHillasSrc",     "Events");
    write->AddContainer("MHillasExt",     "Events");
    write->AddContainer("MPointingPos",   "Events");
    write->AddContainer("MHillasSrcAnti", "Events", kFALSE);
    write->AddContainer("MImagePar",      "Events", kFALSE);
    write->AddContainer("MNewImagePar",   "Events", kFALSE);
    write->AddContainer("MNewImagePar2",  "Events", kFALSE);
    write->AddContainer("Hadronness",     "Events", kFALSE);
    write->AddContainer("MSrcPosCam",     "Events", kFALSE);
    write->AddContainer("MSrcPosAnti",    "Events", kFALSE);
    write->AddContainer("ThetaSquared",   "Events", kFALSE);
    write->AddContainer("OpticalAxis",    "Events", kFALSE);
    write->AddContainer("Disp",           "Events", kFALSE);
    write->AddContainer("Ghostbuster",    "Events", kFALSE);
    write->AddContainer("MEnergyEst",     "Events", kFALSE);
    write->AddContainer("MTime",          "Events", kFALSE);
    write->AddContainer("MMcEvt",         "Events", kFALSE);
    write->AddContainer("DataType",       "Events");
    write->AddContainer("FileId",         "Events");
    write->AddContainer("EvtNumber",      "Events");
    //    write->AddContainer("MMuonSearchPar", "Events", kFALSE);
    //    write->AddContainer("MMuonCalibPar",  "Events", kFALSE);
}

// --------------------------------------------------------------------------
//
// Create a new instance of an object with name name of class
// type fNameHist in parlist. It must derive from MHAlpha.
// Call ForceUsingSize for it and return its pointer.
// If something fails NULL is returned.
//
MHAlpha *MJCut::CreateNewHist(MParList &plist, const char *name) const
{
    TClass *cls = gROOT->GetClass(fNameHist);
    if (!cls)
    {
        *fLog << err << "Class " << fNameHist << " not found in dictionary... abort." << endl;
        return NULL;
    }
    if (!cls->InheritsFrom(MHAlpha::Class()))
    {
        *fLog << err << "Class " << fNameHist << " doesn't inherit from MHAlpha... abort." << endl;
        return NULL;
    }

    const TString objname(Form("Hist%s", name));
    MHAlpha *h = (MHAlpha*)plist.FindCreateObj(fNameHist, objname);
    if (!h)
        return NULL;

    h->ForceUsingSize();

    return h;
}

// --------------------------------------------------------------------------
//
// Create a new instance of an object with name name of class
// type fNameHistFS in parlist. It must derive from MHFalseSource
// If something fails NULL is returned.
//
MH *MJCut::CreateNewHistFS(MParList &plist, const char *name) const
{
    const TString cname(fNameHistFS.IsNull()?"MHFalseSource":fNameHistFS.Data());

    TClass *cls = gROOT->GetClass(cname);
    if (!cls)
    {
        *fLog << err << "Class " << cname << " not found in dictionary... abort." << endl;
        return NULL;
    }
    if (!cls->InheritsFrom("MHFalseSource"))
    {
        *fLog << err << "Class " << cname << " doesn't inherit from MHFalseSource... abort." << endl;
        return NULL;
    }

    const TString objname(Form("FS%s", name));
    return (MH*)plist.FindCreateObj(cname, objname);
}

Bool_t MJCut::FillSrcPosCam(const MDataSet &set, MPointingPos &source, MHSrcPosCam &hsrcpos)
{
    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Filling MHSrcPosCam " << set.GetBaseName() << endl;
    *fLog << endl;

    // --------------------------------------------------------------------------------

    // Setup Parlist
    MParList plist;
    plist.AddToList(this); // take care of fDisplay!

    // Setup Tasklist
    MTaskList tlist;
    plist.AddToList(&tlist);

    // La Palma Magic1, Possible source position
    MObservatory obs;
    plist.AddToList(&obs);
    plist.AddToList(&source);

    // Initialize default binnings
    // MBinning bins1(18,  0,     90,    "BinningSrcPosCam", "lin");
    // plist.AddToList(&bins1);

    // ------------- Loop Off Data --------------------
    MReadReports read;

    read.EnableAutoScheme();
    read.AddToBranchList("MTimeEffectiveOnTime.*");
    read.AddToBranchList("MEffectiveOnTime.*");

    read.AddTree("Events", "MTime.", MReadReports::kMaster);
    read.AddTree("Drive",            MReadReports::kRequired);
    read.AddTree("Starguider",       MReadReports::kRequired);
    read.AddTree("EffectiveOnTime");

    if (!set.AddFilesOn(read))
        return kFALSE;

    MFillH fill(&hsrcpos, "MSrcPosCam", "FillSrcPosCam");
    fill.SetNameTab("SrcPos");

    // How to get source position from off- and on-data?
    MSrcPosCorrect scor;
    MSrcPosCalc    scalc;
    scalc.SetMode(MSrcPosCalc::kDefault);

    MPointingDevCalc devcalc;

    tlist.AddToList(&read);
    tlist.AddToList(&devcalc, "Starguider");
    tlist.AddToList(&scalc,   "Events");
    tlist.AddToList(&scor,    "Events");
    tlist.AddToList(&fill,    "Events");

    // by setting it here it is distributed to all consecutive tasks
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Processing of on-sequences to fill SrcPosCam failed." << endl;
        return kFALSE;
    }

    if (!evtloop.GetDisplay())
    {
        *fLog << err << GetDescriptor() << ": Execution stopped by user." << endl;
        return kFALSE;
    }

    *fLog << all << GetDescriptor() << ": Done." << endl;
    *fLog << endl << endl;

    return kTRUE;
}

Int_t MJCut::Process(const MDataSet &set)
{
    if (!set.IsValid())
    {
        *fLog << err << "ERROR - DataSet invalid!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(GetOutputFile(set.GetNumAnalysis())))
        return kFALSE;

    if (!CheckEnv())
        return kFALSE;

    // --------------------------------------------------------------------------------

    if (!set.IsWobbleMode() && fNumOffSourcePos!=1)
    {
        *fLog << inf << "No wobble mode, but NumOffSoucePos!=1 (" << fNumOffSourcePos << ")... reset to 1." << endl;
        fNumOffSourcePos = 1;
    }

    // Possible source position (eg. Wobble Mode)
    MPointingPos source("MSourcePos");
    if (set.HasSource())
    {
        if (!set.GetSourcePos(source))
            return -1;
        *fLog << all;
        source.Print("RaDec");
    }
    else
        *fLog << all << "No source position applied..." << endl;

    // ADD A CHECK FOR THE SOURCE POSITION by COMPARISON WITH RA/DEC

    MParList plist;

    MHSrcPosCam hsrcpos(set.IsWobbleMode()?0.4:0);
    if (!set.IsWobbleMode() && source.IsInitialized() && fRndmSrcPos)
    {
        if (!FillSrcPosCam(set, source, hsrcpos))
            return -2;
        plist.AddToList(&hsrcpos);
    }

    // --------------------------------------------------------------------------------

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Perform cuts for data set " << set.GetBaseName() << endl;
    *fLog << endl;

    // --------------------------------------------------------------------------------

    // Setup Parlist
    plist.AddToList(this); // take care of fDisplay!

    MParameterI par("DataType");
    plist.AddToList(&par);

    // Setup Tasklist
    MTaskList tlist;
    plist.AddToList(&tlist);

    // La Palma Magic1
    MObservatory obs;
    plist.AddToList(&obs);

    if (source.IsInitialized())
        plist.AddToList(&source);

    // Initialize default binnings
    MBinning bins1( 18,  0,     90,    "BinningAlpha",  "lin");
    MBinning bins2( 15, 10,     1e6 ,  "BinningSize",   "log");
    MBinning bins3( 67, -0.005, 0.665, "BinningTheta",  "asin");
    //MBinning binsT(150,  0,     150,   "BinningDeltaT", "lin");
    MBinning bins4("BinningFalseSource");
    MBinning bins5("BinningWidth");
    MBinning bins6("BinningLength");
    MBinning bins7("BinningDist");
    MBinning bins8("BinningSlope");
    MBinning bins9("BinningM3Long");
    MBinning bins0("BinningM3Trans");
    MBinning binsa("BinningAsym");
    MBinning binsb("BinningConc1");
    plist.AddToList(&bins1);
    plist.AddToList(&bins2);
    plist.AddToList(&bins3);
    plist.AddToList(&bins4);
    plist.AddToList(&bins5);
    plist.AddToList(&bins6);
    plist.AddToList(&bins7);
    plist.AddToList(&bins8);
    plist.AddToList(&bins9);
    plist.AddToList(&bins0);
    plist.AddToList(&binsa);
    plist.AddToList(&binsb);
    //plist.AddToList(&binsT);

    // --------------------------------------------------------------------------------

    MParameterD scale;
    scale.SetVal(1./fNumOffSourcePos);

    // Setup fitter and histograms
    MAlphaFitter fit;
    plist.AddToList(&fit);
    if (set.IsWobbleMode())
        fit.SetScaleUser(1./fNumOffSourcePos); // includes fit.SetScaleMode(MAlphaFitter::kUserScale);

    MHAlpha *halphaoff = CreateNewHist(plist, "Off");
    MFillH falpha(halphaoff, "", "FillHist");
    MH *hfsoff = CreateNewHistFS(plist, "Off");
    MFillH ffs(hfsoff, "MHillas", "FillFS");

    // FIXME: If fPathIn read cuts and energy estimator from file!
    MContinue contq("", "CutQ");
    MContinue cont0("", "Cut0");
    MContinue cont1("", "Cut1");
    MContinue cont2("", "Cut2");
    MContinue cont3("", "Cut3");
    contq.SetAllowEmpty();
    cont0.SetAllowEmpty();
    cont1.SetAllowEmpty();
    cont2.SetAllowEmpty();
    cont3.SetAllowEmpty();

    // Filter for VsSize
    MFDataPhrase ftheta(0, "CutT");

    // ------------- Loop Off Data --------------------
    MReadReports readoffdata;
    readoffdata.AddTree("Events", "MTime.", MReadReports::kMaster);
    readoffdata.AddTree("Drive",            MReadReports::kRequired);
    readoffdata.AddTree("Starguider",       MReadReports::kRequired);
    readoffdata.AddTree("EffectiveOnTime");

    MReadMarsFile readoffmc("Events");
    readoffmc.DisableAutoScheme();

    MRead &readoff = set.IsMonteCarlo() ? (MRead&)readoffmc : (MRead&)readoffdata;
    const Bool_t setrc = set.IsWobbleMode() ? set.AddFilesOn(readoff) : set.AddFilesOff(readoff);
    if (!setrc && (set.HasOffSequences() || set.IsWobbleMode()))
    {
        *fLog << err << "MDataSet::AddFiles" << (set.IsWobbleMode()?"On":"Off") << " failed." << endl;
        return kFALSE;
    }

    const TString path(Form("%s/", fPathOut.Data()));
    TString fname0(path);
    TString fname1(path);
    fname0 += fNameSummary.IsNull() ? (TString) Form("ganymed%08d-summary.root", set.GetNumAnalysis()) : fNameSummary;
    fname1 += fNameResult.IsNull()  ? (TString) Form("ganymed%08d.root",         set.GetNumAnalysis()) : fNameResult;

    MWriteRootFile dummy0(fPathOut.IsNull()||!CanStoreSummary()?0:fname0.Data(), fOverwrite?"RECREATE":"NEW");
    MWriteRootFile dummy1(fPathOut.IsNull()||!CanStoreResult() ?0:fname1.Data(), fOverwrite?"RECREATE":"NEW");

    MWriteRootFile *write0 = CanStoreSummary() ? &dummy0 : 0;
    MWriteRootFile *write1 = CanStoreResult()  ? &dummy1 : 0;
    SetupWriter(write0, "WriteAfterCut0");
    SetupWriter(write1, "WriteAfterCut3");

    MTaskEnv taskenv0("CalcDisp");
    taskenv0.SetDefault(fCalcDisp);

    MTaskEnv taskenv1("CalcHadronness");
    taskenv1.SetDefault(fCalcHadronness);

    MTaskEnv taskenv2("EstimateEnergy");
    taskenv2.SetDefault(fEstimateEnergy);

    MParameterCalc setevtnum("MRawEvtHeader.fDAQEvtNumber", "SetEvtNumber");
    setevtnum.SetNameParameter("EvtNumber");

    MParameterCalc setrunnum("MRawRunHeader.GetFileID", "SetFileId");
    setrunnum.SetNameParameter("FileId");

    MFillH fill1a("MHHillasOffPre  [MHHillas]",      "MHillas",      "FillHillasPre");
    MFillH fill2a("MHHillasOffPost [MHHillas]",      "MHillas",      "FillHillasPost");
    MFillH fill3a("MHVsSizeOffPost [MHVsSize]",      "MHillasSrc",   "FillVsSizePost");
    MFillH fill3c("MHVsSizeOffTheta [MHVsSize]",     "MHillasSrc",   "FillVsSizeTheta");
    MFillH fill4a("MHHilExtOffPost [MHHillasExt]",   "MHillasSrc",   "FillHilExtPost");
    MFillH fill5a("MHHilSrcOffPost [MHHillasSrc]",   "MHillasSrc",   "FillHilSrcPost");
    MFillH fill6a("MHImgParOffPost [MHImagePar]",    "MImagePar",    "FillImgParPost");
    MFillH fill7a("MHNewParOffPost [MHNewImagePar]", "MNewImagePar", "FillNewParPost");
    //MFillH fill9a("MHEffOffTime    [MHEffectiveOnTime]", "MTime",        "FillEffOnTime");
    fill1a.SetNameTab("PreCut");
    fill2a.SetNameTab("PostCut");
    fill3a.SetNameTab("VsSize");
    fill3c.SetNameTab("CutT");
    fill4a.SetNameTab("HilExt");
    fill5a.SetNameTab("HilSrc");
    fill6a.SetNameTab("ImgPar");
    fill7a.SetNameTab("NewPar");
    //fill9a.SetNameTab("EffOffT");

    fill3c.SetFilter(&ftheta);

    //MFDataMember fbin("Bin.fVal", '>', 0);
    //fill9a.SetFilter(&fbin);

    MPrint print2("MEffectiveOnTime");
    print2.EnableSkip();

    MTaskList tlist2;
    if (set.IsWobbleMode())
    {
        tlist2.SetNumPasses(fNumOffSourcePos);
        fill2a.SetWeight(&scale);
        fill3a.SetWeight(&scale);
        fill3c.SetWeight(&scale);
        fill4a.SetWeight(&scale);
        fill5a.SetWeight(&scale);
        fill6a.SetWeight(&scale);
        fill7a.SetWeight(&scale);
    }

    // How to get source position from off- and on-data?
    MSrcPosCalc scalc;
    scalc.SetMode(set.IsWobbleMode()?MSrcPosCalc::kWobble:MSrcPosCalc::kOffData); /********************/
    scalc.SetCallback(&tlist2);

    MSrcPosCorrect scor;

    MHillasCalc hcalc;
    MHillasCalc hcalc2("MHillasCalcAnti");
    hcalc.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalc2.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalc2.SetNameHillasSrc("MHillasSrcAnti");
    hcalc2.SetNameSrcPosCam("MSrcPosAnti");

    MSrcPosRndm srcrndm;

    MH3 hvs("MPointingPos.fZd");
    hvs.SetName("ThetaOff;Theta");
    hvs.SetTitle("Effective On-Time vs. Zenith Angle;\\Theta [\\circ];T_{on} [s]");

    MFillH fillvs(&hvs, "", "FillOnTime");
    if (!set.IsMonteCarlo())
        fillvs.SetWeight("MEffectiveOnTime");
    fillvs.SetNameTab("OnTime");

    // It is not really necessary to re-calculate the image parameters
    // for the the on-source for MCs, but it is done for symmetry reasons
    if (set.IsMonteCarlo())
        tlist2.AddToList(&fillvs);

    tlist2.AddToList(&scalc);
    tlist2.AddToList(&scor);
    tlist2.AddToList(&srcrndm);
    tlist2.AddToList(&hcalc);
    if (set.IsWobbleMode())
        tlist2.AddToList(&hcalc2);
    tlist2.AddToList(&cont0);  
    tlist2.AddToList(&taskenv0);
    tlist2.AddToList(&taskenv1);
    tlist2.AddToList(&setrunnum);
    tlist2.AddToList(&setevtnum);
    if (write0)
        tlist2.AddToList(write0);
    tlist2.AddToList(&cont1);
    if (!fWriteOnly && (!set.IsWobbleMode() || !fNameHistFS.IsNull()))
        tlist2.AddToList(&ffs);
    tlist2.AddToList(&cont2);
    if (!fWriteOnly)
    {
        tlist2.AddToList(&fill2a);
        if (fFullDisplay)
        {
            tlist2.AddToList(&ftheta);
            tlist2.AddToList(&fill3a);
            tlist2.AddToList(&fill3c);
            tlist2.AddToList(&fill4a);
            tlist2.AddToList(&fill5a);
            tlist2.AddToList(&fill6a);
            tlist2.AddToList(&fill7a);
        }
    }
    if (!fWriteOnly)
    {
        tlist2.AddToList(&falpha);
        /* if (!fIsMonteCarlo)
        {
            tlist2.AddToList(&fbin);
            tlist2.AddToList(&fill9a);
        } */
    }
    tlist2.AddToList(&cont3);
    tlist2.AddToList(&taskenv2);

    if (write1)
        tlist2.AddToList(write1);

    MPointingDevCalc devcalc;

    tlist.AddToList(&readoff);
    if (!set.IsMonteCarlo())
        tlist.AddToList(&fillvs, "EffectiveOnTime");
    if (gLog.GetDebugLevel()>4)
        tlist.AddToList(&print2, "EffectiveOnTime");
    tlist.AddToList(&devcalc, "Starguider");
    tlist.AddToList(&contq,   "Events");
    if (!fWriteOnly)
        tlist.AddToList(&fill1a, "Events");
    tlist.AddToList(&tlist2,  "Events");

    // by setting it here it is distributed to all consecutive tasks
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);
 
    par.SetVal(0);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    TObjArray cont;
    cont.Add(&contq);
    cont.Add(&cont0);
    cont.Add(&cont1);
    cont.Add(&cont2);
    cont.Add(&cont3);
    //if (taskenv1.GetTask())
    //    cont.Add(taskenv1.GetTask());
    if (taskenv0.GetTask())
        cont.Add(taskenv0.GetTask());
    if (taskenv1.GetTask())
        cont.Add(taskenv1.GetTask());
    if (taskenv2.GetTask())
        cont.Add(taskenv2.GetTask());

    if (!WriteTasks(set.GetNumAnalysis(), cont))
        return kFALSE;

    if (set.HasOffSequences() || set.IsWobbleMode())
    {
        // Execute first analysis
        if (!evtloop.Eventloop(fMaxEvents))
        {
            *fLog << err << GetDescriptor() << ": Processing of off-sequences failed." << endl;
            return -3;
        }

        if (!evtloop.GetDisplay())
        {
            *fLog << err << GetDescriptor() << ": Execution stopped by user." << endl;
            return kFALSE;
        }

        //plist.FindObject("MTimeEffectiveOnTime")->Clear();
    }
    else
    {
        // This is the simplest way to remove the two object from the parlist
        delete halphaoff;
        delete hfsoff;
    }

    // ------------- Loop On Data --------------------
    MReadReports readondata;
    readondata.AddTree("Events", "MTime.", MReadReports::kMaster);
    readondata.AddTree("Drive",            MReadReports::kRequired);
    readondata.AddTree("Starguider",       MReadReports::kRequired);
    readondata.AddTree("EffectiveOnTime");

    MReadMarsFile readonmc("Events");
    readonmc.DisableAutoScheme();

    MRead &readon = set.IsMonteCarlo() ? (MRead&)readonmc : (MRead&)readondata;
    if (!set.AddFilesOn(readon))
        return kFALSE;

    scalc.SetMode(MSrcPosCalc::kDefault);
    scalc.SetNumRandomOffPositions(fNumOffSourcePos);

    MFillH fill1b("MHHillasOnPre  [MHHillas]",      "MHillas",      "FillHillasPre");
    MFillH fill2b("MHHillasOnPost [MHHillas]",      "MHillas",      "FillHillasPost");
    MFillH fill3b("MHVsSizeOnPost [MHVsSize]",      "MHillasSrc",   "FillVsSizePost");
    MFillH fill3d("MHVsSizeOnTheta [MHVsSize]",     "MHillasSrc",   "FillVsSizeTheta");
    MFillH fill4b("MHHilExtOnPost [MHHillasExt]",   "MHillasSrc",   "FillHilExtPost");
    MFillH fill5b("MHHilSrcOnPost [MHHillasSrc]",   "MHillasSrc",   "FillHilSrcPost");
    MFillH fill6b("MHImgParOnPost [MHImagePar]",    "MImagePar",    "FillImgParPost");
    MFillH fill7b("MHNewParOnPost [MHNewImagePar]", "MNewImagePar", "FillNewParPost");
    //MFillH fill9b("MHEffOnTime    [MHEffectiveOnTime]", "MTime",        "FillEffOnTime");
    fill1b.SetNameTab("PreCut");
    fill2b.SetNameTab("PostCut");
    fill3b.SetNameTab("VsSize");
    fill3d.SetNameTab("CutT");
    fill4b.SetNameTab("HilExt");
    fill5b.SetNameTab("HilSrc");
    fill6b.SetNameTab("ImgPar");
    fill7b.SetNameTab("NewPar");
    //fill9b.SetNameTab("EffOnT");
    fill1b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill2b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill3b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill3d.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill4b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill5b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill6b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    fill7b.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");
    //fill9b.SetFilter(&fbin);

    fill3d.SetFilter(&ftheta);

    /*
     MHVsTime hvs("MEffectiveOnTime.fVal");
     hvs.SetTitle("Effective On-Time vs. Time;;T_{on}");
     MFillH fillvs(&hvs, "MTimeEffectiveOnTime", "FillOnTime");
     fillvs.SetNameTab("OnTime");
     */

    /*
    MParameterD weight;
    weight.SetVal(-1);
    fill2a.SetWeight(&weight);
    fill3a.SetWeight(&weight);
    fill4a.SetWeight(&weight);
    fill5a.SetWeight(&weight);
    fill6a.SetWeight(&weight);
    fill7a.SetWeight(&weight);
    if (fSubstraction)
    {
        fill2a.SetNameTab("PostCut-");
        fill3a.SetNameTab("VsSize-");
        fill4a.SetNameTab("HilExt-");
        fill5a.SetNameTab("HilSrc-");
        fill6a.SetNameTab("ImgPar-");
        fill7a.SetNameTab("NewPar-");
    }
    */
    MHAlpha *halphaon=CreateNewHist(plist);
    MFillH falpha2(halphaon, "", "FillHist");
    MH *hfs=CreateNewHistFS(plist);
    MFillH ffs2(hfs, "MHillas", "FillFS");
    MFillH fillphi("MHPhi", "", "FillPhi");
    fillphi.SetDrawOption("anticut");

    tlist2.SetNumPasses();

    tlist.Replace(&readon);
    if (fRndmSrcPos && !set.IsWobbleMode())
        tlist2.RemoveFromList(&srcrndm);

    MFillH fillsrc(&hsrcpos, "MSrcPosCam", "FillSrcPosCam");
    fillsrc.SetNameTab("SrcPos");

    if (set.IsWobbleMode()/* && !set.IsMonteCarlo()*/)
        tlist2.AddToListBefore(&fillsrc, &hcalc);

    MH3 hvs2("MPointingPos.fZd");
    hvs2.SetName("Theta");
    hvs2.SetTitle("Effective On-Time vs. Zenith Angle;\\Theta [\\circ];T_{on} [s]");

    MFillH fillvs2(&hvs2, "", "FillOnTime");
    if (!set.IsMonteCarlo())
        fillvs2.SetWeight("MEffectiveOnTime");
    fillvs2.SetNameTab("OnTime");
    fillvs2.SetDrawOption(set.HasOffSequences()||set.IsWobbleMode()?"same":"");

    if (!fWriteOnly)
    {
        tlist.Replace(&fill1b);

        tlist2.Replace(&fill2b);
        if (fFullDisplay)
        {
            tlist2.Replace(&fill3b);
            tlist2.Replace(&fill3d);
            tlist2.Replace(&fill4b);
            tlist2.Replace(&fill5b);
            tlist2.Replace(&fill6b);
            tlist2.Replace(&fill7b);
        }
        tlist2.Replace(&falpha2);
        //if (!fIsMonteCarlo)
        //    tlist2.Replace(&fill9b);
        if (!set.IsWobbleMode() || !fNameHist.IsNull())
            tlist2.Replace(&ffs2);
        if (set.IsWobbleMode())
        {
            tlist2.AddToListAfter(&fillphi, &falpha2);
            if (!fNameHist.IsNull())
                tlist2.RemoveFromList(&ffs);
        }

        if (!set.IsMonteCarlo())
            tlist.Replace(&fillvs2);
        else
            tlist2.Replace(&fillvs2);
    }

    // by setting it here it is distributed to all consecutive tasks
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    par.SetVal(1);

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Processing of on-sequences failed." << endl;
        return -4;
    }

    // FIXME: Perform fit and plot energy dependant alpha plots
    // and fit result to new tabs!
    if (!WriteResult(plist, set))
        return kFALSE;

    *fLog << all << GetDescriptor() << ": Done." << endl;
    *fLog << endl << endl;

    return kTRUE;
}
