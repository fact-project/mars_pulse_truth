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
!   Author(s): Thomas Bretz, 7/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJMerpp
//
// Merging and preprocessing
//
/////////////////////////////////////////////////////////////////////////////
#include "MJMerpp.h"

#include <TFile.h>
#include <TTree.h>

#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MFDataPhrase.h"

#include "MRawRunHeader.h"

#include "MRawFileRead.h"
#include "MRawFitsRead.h"
#include "MReportFileReadCC.h"
#include "MWriteRootFile.h"

ClassImp(MJMerpp);

using namespace std;

MJMerpp::MJMerpp(const char *name, const char *title)
{
    fName  = name  ? name  : "MJMerpp";
    fTitle = title ? title : "Standard program for merging subsystem data";
}

void MJMerpp::AddTree(MReportFileReadCC &read, const TString &rep, const TString &only) const
{
    if (!only.IsNull() && rep!=only)
        return;

    read.AddToList(Form("MReport%s", rep.Data()));
}

void MJMerpp::SetupCurrents(MReportFileReadCC &read, MWriteRootFile *write) const
{
    read.SetHasNoHeader();
    read.AddToList("MReportCurrents");

    if (!write)
        return;

    write->AddContainer("MTimeCurrents",      "Currents");
    write->AddContainer("MCameraDC",          "Currents");
    write->AddContainer("MReportCurrents",    "Currents");
}

MFilter *MJMerpp::SetupReports(MReportFileReadCC &read, MWriteRootFile *write) const
{
    read.SetTelescope(fTelescope);

    read.SetRunNumber(fHeaderRun);
    read.SetFileNumber(fHeaderFile);

    AddTree(read, "CC",         fOnly);
    AddTree(read, "Rec",        fOnly);
    AddTree(read, "Drive",      fOnly);
    AddTree(read, "Camera",     fOnly);
    AddTree(read, "Trigger",    fOnly);
    AddTree(read, "Starguider", fOnly);
    AddTree(read, "Pyrometer",  fOnly);
    // AddTree(read, "DAQ",  fOnly);

    if (fReportRun!=(UInt_t)-1)
        read.AddToList("MReportRun");

    if (!write)
        return 0;

    const Bool_t required = fOnly.IsNull();
    write->AddContainer("MReportCamera",      "Camera",     required);
    write->AddContainer("MTimeCamera",        "Camera",     required);
    write->AddContainer("MCameraAUX",         "Camera",     required);
    write->AddContainer("MCameraCalibration", "Camera",     required);
    write->AddContainer("MCameraCooling",     "Camera",     required);
    write->AddContainer("MCameraActiveLoad",  "Camera",     required);
    write->AddContainer("MCameraHV",          "Camera",     required);
    write->AddContainer("MCameraDC",          "Camera",     required);
    write->AddContainer("MCameraLV",          "Camera",     required);
    write->AddContainer("MCameraLids",        "Camera",     required);
    write->AddContainer("MReportTrigger",     "Trigger",    required);
    write->AddContainer("MTimeTrigger",       "Trigger",    required);
    write->AddContainer("MTriggerBit",        "Trigger",    required);
    write->AddContainer("MTriggerIPR",        "Trigger",    required);
    write->AddContainer("MTriggerCell",       "Trigger",    required);
    write->AddContainer("MTriggerPrescFact",  "Trigger",    required);
    write->AddContainer("MTriggerLiveTime",   "Trigger",    required);
    write->AddContainer("MReportDrive",       "Drive",      required);
    write->AddContainer("MTimeDrive",         "Drive",      required);
    write->AddContainer("MCameraTH",          "Rec",        required);
    write->AddContainer("MCameraTD",          "Rec",        required);
    write->AddContainer("MCameraRecTemp",     "Rec",        required);
    write->AddContainer("MReportRec",         "Rec",        required);
    write->AddContainer("MTimeRec",           "Rec",        required);
    write->AddContainer("MReportCC",          "CC",         required);
    write->AddContainer("MTimeCC",            "CC",         required);
    write->AddContainer("MReportStarguider",  "Starguider", required);
    write->AddContainer("MTimeStarguider",    "Starguider", required);
    write->AddContainer("MReportPyrometer",   "Pyrometer",  required);
    write->AddContainer("MTimePyrometer",     "Pyrometer",  required);
    // write->AddContainer("MReportDAQ",         "DAQ");
    // write->AddContainer("MTimeDAQ",           "DAQ");

    if (fReportRun==(UInt_t)-1)
        return 0;

    const TString f1 = fReportRun>0   ? Form("MReportRun.fRunNumber==%ud",  fReportRun)  : "";
    const TString f2 = fReportFile>=0 ? Form("MReportRun.fFileNumber==%ud", fReportFile) : "";

    const TString f = Form(fReportRun>0 && fReportFile>=0 ? "%s && %s" : "%s%s",
                           f1.Data(), f2.Data());

    MFilter *filter = new MFDataPhrase(f.Data());
    write->SetFilter(filter);
    return filter;
}

void MJMerpp::SetupRaw(MRawFileRead &read, MWriteRootFile *write) const
{
//    read.SetInterleave(fInterleave);
//    read.SetForceMode(fForceProcessing);

    if (!write)
        return;

    write->AddContainer("MRawRunHeader",  "RunHeaders");
    write->AddContainer("MTime",          "Events");
    write->AddContainer("MRawEvtHeader",  "Events");
    write->AddContainer("MRawEvtData",    "Events");
    write->AddContainer("MRawEvtData2",   "Events");
    write->AddContainer("MRawCrateArray", "Events");
}

Bool_t MJMerpp::GetTimeFromFile()
{
    if (!fAutoStartTime && !fAutoStopTime)
        return kTRUE;

    TFile f(fPathOut, "READ");
    if (f.IsZombie())
    {
        *fLog << err << "ERROR - File " << fPathOut << " could not be opened." << endl;
        return kFALSE;
    }

    TTree *t = (TTree*)f.Get("RunHeaders");
    if (t->GetEntries()!=1)
    {
        *fLog << err << "ERROR - File " << fPathOut << " contains no or more than one entry in RunHeaders... Times unchanged." << endl;
        return kFALSE;
    }

    MRawRunHeader *h = 0;
    t->SetBranchAddress("MRawRunHeader.", &h);
    t->GetEntry(0);
    if (!h)
    {
        *fLog << err << "ERROR - File " << fPathOut << " did not contain RunHeaders.MRawRunHeader... Times unchanged." << endl;
        return kFALSE;
    }

    if (fAutoStartTime)
        fTimeStart = h->GetRunStart();
    if (fAutoStopTime)
        fTimeStop = h->GetRunEnd();

    return kTRUE;
}

Bool_t MJMerpp::CheckFilePermissions()
{
    //
    // check whether the given files are OK.
    //
    if (gSystem->AccessPathName(fPathIn, kFileExists))
    {
        *fLog << err << "ERROR - Input file '" << fPathIn << "' not accessible." << endl;
        return kFALSE;
    }

    if (HasNullOut())
        return kTRUE;

    const Bool_t fileexist = !gSystem->AccessPathName(fPathOut, kFileExists);
    const Bool_t writeperm = !gSystem->AccessPathName(fPathOut, kWritePermission);

    if (fileexist && !writeperm)
    {
        *fLog << err << "ERROR - No write permission for '" << fPathOut << "'." << endl;
        return kFALSE;
    }

    if (fileexist && !fUpdate && !fOverwrite)
    {
        *fLog << err << "ERROR - File '" << fPathOut << "' already existing." << endl;
        return kFALSE;
    }

    if (!fileexist && fUpdate)
    {
        *fLog << err << "ERROR - File '" << fPathOut << "' doesn't yet exist... no update possible." << endl;
        return kFALSE;
    }

    return kTRUE;
}

Int_t MJMerpp::Process()
{
    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "In:  " << fPathIn << endl;
    if (!HasNullOut())
        *fLog << (fUpdate?"Upd":"Out") << ": " << fPathOut << endl;;
    *fLog << endl;

    if (!CheckFilePermissions())
        return 1;

    // FIXME: Add a check whether the file has already been merpped!!!

    const Bool_t isreport = fPathIn.EndsWith(".rep");
    const Bool_t isdc     = fPathIn.EndsWith(".txt");

    //
    // Evaluate possible start-/stop-time
    //
    if (fUpdate && (isreport || isdc))
        if (!GetTimeFromFile())
            return 2;

    if (fTimeStart)
        *fLog << inf << "Start Time: " << fTimeStart << endl;
    if (fTimeStop)
        *fLog << inf << "Stop  Time: " << fTimeStop << endl;

    //
    // create a (empty) list of parameters which can be used by the tasks
    // and an (empty) list of tasks which should be executed
    //
    MParList plist;

    MTaskList tasks;
    tasks.SetOwner();
    plist.AddToList(&tasks);

    //
    // create the tasks which should be executed and add them to the list
    // in the case you don't need parameter containers, all of them can
    // be created by MRawFileRead::PreProcess
    //
    const TString option(fUpdate?"UPDATE":(fOverwrite?"RECREATE":"NEW"));

    MTask          *read   = 0;
    MFilter        *filter = 0;
    MWriteRootFile *write  = HasNullOut() ? 0 : new MWriteRootFile(fPathOut, option, "Magic root-file", fCompression);

    if (isreport || isdc)
    {
        MReportFileReadCC *r = new MReportFileReadCC(fPathIn);
        r->SetTimeStart(fTimeStart);
        r->SetTimeStop(fTimeStop);

        if (isdc)
            SetupCurrents(*r, write);
        else
            filter = SetupReports(*r, write);

        read = r;
    }
    else
    {
        MRawFileRead *r = MRawFitsRead::IsFits(fPathIn) ? new MRawFitsRead(fPathIn) : new MRawFileRead(fPathIn);
        SetupRaw(*r, write);
        read = r;
    }

    tasks.AddToList(read);
    if (filter)
        tasks.AddToList(filter);

    if (write)
        tasks.AddToList(write);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetLogStream(fLog);
    //evtloop.SetDisplay(fDisplay);
    //if (!SetupEnv(evtloop))
    //    return kFALSE;

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return 3;
    }

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;;

    return 0;
}

Int_t MJMerpp::ProcessSeq(TString fname)
{
    if (!MSequence::InflateSeq(fname))
        return 4;

    MSequence seq(fname);
    if (!seq.IsValid())
        return 5;

    const UInt_t num = seq.GetNumEntries(MSequence::kDat);

    const TString reppath = fPathIn;
    const TString calpath = fPathOut;

    // PreCheck: We don't wantto start merpp if we
    // know that an error will occur
    for (UInt_t i=0; i<num; i++)
    {
        const TString name1 = fHeaderRun==0 ? reppath : seq.GetFileName(i, MSequence::kReportDat, reppath);
        const TString name2 = seq.GetFileName(i, MSequence::kCalibrated, calpath);

        // Full qualified name could not be determined or file is not
        // accessible. For excluded files "0" is returned.
        if (name1.IsNull() || name2.IsNull())
            return 6;
    }

    // Now we can safely start processing
    TString rc;
    for (UInt_t i=0; i<num; i++)
    {
        UInt_t run, file;

        // Excluded
        if (seq.GetFile(i, MSequence::kDat, run, file)==0)
            continue;

        // fHeaderRun==0 means: All day summary file
        const TString name1 = fHeaderRun==0 ? reppath : seq.GetFileName(i, MSequence::kReportDat, reppath);
        const TString name2 = seq.GetFileName(i, MSequence::kCalibrated, calpath);

        // FIXME: check runcallisto
        if (fHeaderRun==0)                 // extract valid range from summary file
            SetConstrainRunRep(run, file); // (this is not guranteed to work :-( )
        else                               // check for the correct header
            SetConstrainHeader(seq.GetTelescope(), run, file);

        SetTime(MTime(), MTime()); // Raise error if set?
        SetPathIn(name1);
        SetPathOut(name2);

        const Int_t rc = Process();
        if (rc==0)
            continue;

        return rc;
    }
    return 0;
}
