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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MEvtLoop
//
// This class is the core of each event processing.
// First you must set the parameter list to use. The parameter list
// must contain the task list (MTaskList) to use. The name of the task
// list can be specified if you call Eventloop. The standard name is
// "MTaskList". The name you specify must match the name of the MTaskList
// object.
//
// If you call Eventloop() first all PreProcess functions - with the
// parameter list as an argument - of the tasks in the task list are
// executed. If one of them returns kFALSE then the execution is stopped.
// If the preprocessing was ok, The Process function of the tasks are
// executed as long as one function returns kSTOP. Only the tasks which
// are marked as "All" or with a string which matches the MInputStreamID
// of MTaskList are executed. If one tasks returns kCONTINUE the pending
// tasks in the list are skipped and the execution in continued with
// the first one in the list.
// Afterwards the PostProcess functions are executed.
//
// If you want to display the progress in a gui you can use SetProgressBar
// and a TGProgressBar or a MProgressBar. If you set a MStatusDisplay
// using SetDisplay, the Progress bar from this display is used.
//
// You can create a macro from a completely setup eventloop by:
//   evtloop.MakeMacro("mymacro.C");
//
// You will always need to check the macro, it will not run, but it
// should have al important information.
//
//
// You can also write all this information to a root file:
//   TFile file("myfile.root");
//   evtloop.Write("MyEvtloopKey");
//
// You can afterwards read the information from an open file by:
//   evtloop.Read("MyEvtloopKey");
//
// To lookup the information write it to a file using MakeMacro
//
//////////////////////////////////////////////////////////////////////////////
#include "MEvtLoop.h"

#include <time.h>           // time_t
#include <fstream>          // ofstream, SavePrimitive

#include <TEnv.h>           // TEnv
#include <TRint.h>          // gApplication, TRint::Class()
#include <TTime.h>          // TTime
#include <TFile.h>          // gFile
#include <TThread.h>        // TThread::Self()
#include <TDatime.h>        // TDatime
#include <TSystem.h>        // gSystem
#include <TStopwatch.h>
#include <TGProgressBar.h>  

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"
#include "MParList.h"
#include "MTaskList.h"
#ifdef __MARS__
#include "MRead.h"           // for setting progress bar
#include "MProgressBar.h"    // MProgressBar::GetBar
#include "MStatusDisplay.h"  // MStatusDisplay::GetBar
#endif

ClassImp(MEvtLoop);

using namespace std;

// --------------------------------------------------------------------------
//
// default constructor
//
MEvtLoop::MEvtLoop(const char *name) : fParList(NULL), fTaskList(NULL), fProgress(NULL)
{
    fName = name;

    gROOT->GetListOfCleanups()->Add(this); // To remove fDisplay
    SetBit(kMustCleanup);

    *fLog << inf << endl << underline << "Instantiated MEvtLoop (" << name << "), using ROOT v" << ROOT_RELEASE << endl;
}

// --------------------------------------------------------------------------
//
// destructor
//
MEvtLoop::~MEvtLoop()
{
    if (TestBit(kIsOwner) && fParList)
        delete fParList;
}

void MEvtLoop::SetParList(MParList *p)
{
    if (!p)
        return;

    p->SetBit(kMustCleanup);
    fParList = p;
}

// --------------------------------------------------------------------------
//
// If the evntloop knows its tasklist search for the task there,
// otherwise return NULL.
//
MTask *MEvtLoop::FindTask(const char *name) const
{
    return fTaskList ? fTaskList->FindTask(name) : NULL;
}

// --------------------------------------------------------------------------
//
// If the evntloop knows its tasklist search for the task there,
// otherwise return NULL.
//
MTask *MEvtLoop::FindTask(const MTask *obj) const
{
    return fTaskList ? fTaskList->FindTask(obj) : NULL;
}

// --------------------------------------------------------------------------
//
//  if you set the Eventloop as owner the destructor of the given parameter
//  list is calles by the destructor of MEvtLoop, otherwise not.
//
void MEvtLoop::SetOwner(Bool_t enable)
{
    enable ? SetBit(kIsOwner) : ResetBit(kIsOwner);
}

void MEvtLoop::SetProgressBar(TGProgressBar *bar)
{
    fProgress = bar;
    if (fProgress)
        fProgress->SetBit(kMustCleanup);
}

#ifdef __MARS__
// --------------------------------------------------------------------------
//
//  Specify an existing MProgressBar object. It will display the progress
//  graphically. This will make thing about 1-2% slower.
//
void MEvtLoop::SetProgressBar(MProgressBar *bar)
{
    SetProgressBar(bar->GetBar());
}
#endif

void MEvtLoop::SetDisplay(MStatusDisplay *d)
{
    MParContainer::SetDisplay(d);
    if (!d)
        fProgress=NULL;
    else
    {
        d->SetBit(kMustCleanup);

        // Get pointer to update Progress bar
        fProgress = fDisplay->GetBar();
    }

    if (fParList)
        fParList->SetDisplay(d);
}

// --------------------------------------------------------------------------
//
// The proprocessing part of the eventloop. Be careful, this is
// for developers or use in special jobs only!
//
Bool_t MEvtLoop::PreProcess()
{
    fTaskList = NULL;

    //
    // check if the needed parameter list is set.
    //
    if (!fParList)
    {
        *fLog << err << dbginf << "Parlist not initialized." << endl;
        return kFALSE;
    }

    //
    //  check for the existance of the specified task list
    //  the default name is "MTaskList"
    //
    fTaskList = (MTaskList*)fParList->FindObject("MTaskList");
    if (!fTaskList)
    {
        *fLog << err << dbginf << "Cannot find MTaskList in parameter list." << endl;
        return kFALSE;
    }

    if (fLog != &gLog)
        fParList->SetLogStream(fLog);

#ifdef __MARS__
    //
    // Check whether display is still existing
    //
    if (fDisplay)
    {
        // Lock display to prevent user from deleting it
        fDisplay->Lock();
        // Don't display context menus
        fDisplay->SetNoContextMenu();
        // Set window and icon name
        fDisplay->SetWindowName(fName);
        fDisplay->SetIconName(fName);
        // Start automatic update
        fDisplay->StartUpdate();
        // Cascade display through childs
        if (!TestBit(kPrivateDisplay))
            fParList->SetDisplay(fDisplay);
    }
#endif

    if (fProgress)
        fProgress->SetPosition(0);

    //
    //  execute the preprocess of all tasks
    //  connect the different tasks with the right containers in
    //  the parameter list
    //
    if (!fTaskList->PreProcess(fParList))
    {
        *fLog << err << "Error detected while PreProcessing." << endl;
        return kFALSE;
    }

    *fLog << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the memory currently used by this process (VmSize)
// which contains shared memory, data memory and private memory.
//
UInt_t MEvtLoop::GetMemoryUsage()
{
    const TString path = MString::Format("/proc/%d/status", gSystem->GetPid());
    if (gSystem->AccessPathName(path, kFileExists))
        return 0;

    return TEnv(path).GetValue("VmSize", 0);
}

Bool_t MEvtLoop::ProcessGuiEvents(Int_t num, Int_t looprc)
{
    if (gROOT->IsBatch())
        return kTRUE;

    //
    // Check status of display
    //
    Bool_t rc = kTRUE;

    if (fDisplay)
        switch (fDisplay->CheckStatus())
        {
        case MStatusDisplay::kLoopPause:
            fDisplay->SetStatusLine1(MString::Format("Paused after %d", num-1));
            while (fDisplay && fDisplay->CheckStatus()==MStatusDisplay::kLoopPause)
            {
                gSystem->ProcessEvents();
                gSystem->Sleep(1);
            }
            // FALLTHROUGH
        case MStatusDisplay::kLoopStep:
            if (fDisplay && fDisplay->CheckStatus()==MStatusDisplay::kLoopStep && looprc!=kCONTINUE)
            {
                //fDisplay->ClearStatus();
                fDisplay->SetPause();
            }
            // FALLTHROUGH
        case MStatusDisplay::kLoopNone:
            break;
        case MStatusDisplay::kLoopStop:
            rc = kFALSE;
            fDisplay->ClearStatus();
            break;
        //
        // If the display is not on the heap (means: not created
        // with the new operator) the object is deleted somewhere
        // else in the code. It is the responsibility of the
        // application which instantiated the object to make
        // sure that the correct action is taken. This can be
        // done by calling MStatusDisplay::CheckStatus()
        //
        // Because we are synchronous we can safely delete it here!
        //
        // Close means: Close the display but leave analysis running
        // Exit means: Close the display and stop analysis
        //
        case MStatusDisplay::kFileClose:
        case MStatusDisplay::kFileExit:
            rc = fDisplay->CheckStatus() == MStatusDisplay::kFileClose;

            if (fDisplay->IsOnHeap())
                delete fDisplay;

            //
            // This makes the display really disappear physically on
            // the screen in case of MStatusDisplay::kFileClose
            //
            gSystem->ProcessEvents();

            return rc;
        default:
            *fLog << warn << "MEvtloop: fDisplay->CheckStatus() has returned unknown status #" << fDisplay->CheckStatus() << "... cleared." << endl;
            fDisplay->ClearStatus();
            break;
        }

    //
    // Check System time (don't loose too much time by updating the GUI)
    //

    // FIXME: Not thread safe (if you have more than one eventloop running)
    static Int_t start = num;
    static TTime t1 = gSystem->Now();
    static TTime t2 = t1;

    //
    // No update < 20ms
    //
    const TTime t0 = gSystem->Now();
    if (t0-t1 < (TTime)20)
        return rc;
    t1 = t0;

    //
    // Update current speed each 1.5 second
    //
    if (fDisplay && t0-t2>(TTime)1500)
    {
        const Float_t speed = 1000.*(num-start)/(long int)(t0-t2);
        TString txt = "Processing...";
        if (speed>0)
        {
            txt += " (";
            txt += (Int_t)speed;
            txt += "Evts/s";
            if (fNumEvents>0 && (Int_t)fNumEvents-num>0)
            {
                txt += ", est: ";
                txt += (int)((fNumEvents-num)/speed/60)+1;
                txt += "min";
            }
            //txt += (int)fmod(entries/(1000.*(num-start)/(long int)(t0-t2)), 60);
            //txt += "s";
            txt += ")";
        }
        fDisplay->SetStatusLine1(txt);
        start = num;
        t2 = t0;
    }

    //
    // Set new progress bar position
    //
    if (fNumEvents>0 && fProgress)
    {
        const Double_t pos = (Double_t)num/fNumEvents;
        fProgress->SetPosition(pos);
//        if (gROOT->IsBatch())
//            *fLog << all << MString::Format("%.1f", pos) << "%..." << flush;
    }

    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
    {
        //
        // Handle GUI events (display changes)
        //
#if ROOT_VERSION_CODE < ROOT_VERSION(3,02,06)
        gSystem->ProcessEvents();
#else
        if (fDisplay)
            gSystem->ProcessEvents();
        else
            if (fProgress)
                gClient->ProcessEventsFor(fProgress);
#endif
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// The processing part of the eventloop. Be careful, this is
// for developers or use in special jobs only!
//
Int_t MEvtLoop::Process(UInt_t maxcnt)
{
    if (!fTaskList)
        return kFALSE;

    const UInt_t mem0 = GetMemoryUsage();

    //
    //   loop over all events and process all tasks for
    //   each event
    //
    *fLog << all <<"Eventloop running (";

    if (maxcnt==0)
        *fLog << "all";
    else
        *fLog << dec << maxcnt;

    *fLog << " events)..." << flush;

    UInt_t entries = kMaxUInt;
    fNumEvents = 0;

    if (fProgress && !gROOT->IsBatch())
    {
        fProgress->Reset();
        fProgress->SetRange(0, 1);

#ifdef __MARS__
        MRead *read = (MRead*)fTaskList->FindObject("MRead");
        if (read && read->GetEntries()>0)
            entries = read->GetEntries();
#endif

        if (maxcnt>0)
            fNumEvents = TMath::Min(maxcnt, entries);
        else
            if (entries!=kMaxUInt)
                fNumEvents = entries;
    }

    if (fDisplay)
    {
        fDisplay->SetStatusLine1("Processing...");
        fDisplay->SetStatusLine2("");
    }

    //
    // start a stopwatch
    //
    TStopwatch clock;
    clock.Start();

    //
    // This is the MAIN EVENTLOOP which processes the data
    // if maxcnt==0 the number of processed events is counted
    // else only maxcnt events are processed
    //
    UInt_t numcnts = 0;
    UInt_t dummy   = maxcnt;

    Int_t rc=kTRUE;
    if (maxcnt==0)
        // process first and increment if sucessfull
        while (1)
        {
            rc=fTaskList->CallProcess();
            if (rc!=kTRUE && rc!=kCONTINUE)
                break;

            numcnts++;
            if (!ProcessGuiEvents(++dummy, rc))
                break;
        }
    else
        // check for number and break if unsuccessfull
        while (dummy--)
        {
            rc=fTaskList->CallProcess();
            if (rc!=kTRUE && rc!=kCONTINUE)
                break;

            numcnts++;
            if (!ProcessGuiEvents(maxcnt - dummy, rc))
                break;
        }

    //
    // stop stop-watch, print results
    //
    clock.Stop();

    if (fProgress && !gROOT->IsBatch())
    {
        //fProgress->SetPosition(maxcnt>0 ? TMath::Min(maxcnt, entries) : entries);
        fProgress->SetPosition(1);

        // FIXME: This is a workaround, because TApplication::Run is not
        //        thread safe against ProcessEvents. We assume, that if
        //        we are not in the Main-Thread ProcessEvents() is
        //        called by the TApplication Event Loop...
        if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        {
#if ROOT_VERSION_CODE < ROOT_VERSION(3,02,06)
            gSystem->ProcessEvents();
#else
            gClient->ProcessEventsFor(fDisplay ? fDisplay->GetBar() : fProgress);
#endif
        }
    }

    *fLog << all << "Ready!" << endl << endl;

    *fLog << dec << endl << "CPU  - Time: ";
    *fLog << clock.CpuTime() << "s" << " for " << numcnts << " Events";
    if (numcnts>0)
        *fLog << " --> " << numcnts/clock.CpuTime() << " Events/s";
    *fLog << endl << "Real - Time: ";
    *fLog << clock.RealTime() << "s" << " for " << numcnts << " Events";
    if (numcnts>0)
        *fLog << " --> " << numcnts/clock.RealTime() << " Events/s";


    const UInt_t mem1 = GetMemoryUsage();
    if (mem1>mem0)
        *fLog << endl << "Mem  - Loss: " << mem1-mem0 << "kB" << endl;

    *fLog << endl << endl;

    return rc!=kERROR;
}

// --------------------------------------------------------------------------
//
//  The postprocessing part of the eventloop. Be careful, this is
// for developers or use in special jobs only!
//
Bool_t MEvtLoop::PostProcess() const
{
    //
    //  execute the post process of all tasks
    //
    return fTaskList ? fTaskList->PostProcess() : kTRUE;
}

// --------------------------------------------------------------------------
//
// See class description above. Returns kTRUE if PreProcessing,
// Processing and PostProcessing was successfull.
// kFALSE is retuned if something was not successfull, eg:
//   PreProcess or PostProcess returned kFALSE
//   process returned kERRR
//
// maxcnt==0 means: all events
// tlist is the name of the task-list to be used. Be carefull, this
// feature is not finally implemented - it will only work if no
// task will access the tasklist.
//
Bool_t MEvtLoop::Eventloop(UInt_t maxcnt, Statistics_t printstat)
{
    *fLog << inf << endl << underline << "Eventloop: " << fName << " started at " << TDatime().AsString() << endl;

    Bool_t rc = PreProcess();

    //
    // If all Tasks were PreProcesses successfully start Processing.
    //
    if (rc)
        rc = Process(maxcnt);

    //
    // Now postprocess all tasks. Only successfully preprocessed tasks
    // are postprocessed. If the Postprocessing of one task fails
    // return an error.
    //
    if (!PostProcess())
    {
        *fLog << err << "Error detected while PostProcessing." << endl;
        rc = kFALSE;
    }

    //
    // If Process has ever been called print statistics
    //
    if (fTaskList && fTaskList->GetNumExecutions()>0)
        switch (printstat)
        {
        case kNoStatistics:
            break;
        case kStdStatistics:
            fTaskList->PrintStatistics();
            break;
        case kFullStatistics:
            fTaskList->PrintStatistics(0, kTRUE);
            break;
        }

    if (!fDisplay)
        return rc;

    // Set status lines
    fDisplay->SetStatusLine1(fName);
    fDisplay->SetStatusLine2(rc ? "Done." : "Error!");
    // Stop automatic update
    fDisplay->StopUpdate();
    // Reallow context menus
    fDisplay->SetNoContextMenu(kFALSE);
    // Reallow user to exit window by File menu
    fDisplay->UnLock();

    //
    // If postprocessing of all preprocessed tasks was sucefully return rc.
    // This gives an error in case the preprocessing has failed already.
    // Otherwise the eventloop is considered: successfully.
    //
    return rc;
}

// --------------------------------------------------------------------------
//
//  After you setup (or read) an Evtloop you can use MakeMacro() to write
//  the eventloop setup as a macro. The default name is "evtloop.C". The
//  default extension is .C If the extension is not given, .C is added.
//  If the last character in the argument is a '+' the file is not closed.
//  This is usefull if you have an eventloop which runs three times and
//  you want to write one macro. If the first character is a '+' no
//  opening is written, eg:
//
//     MEvtLoop evtloop;
//     // some setup
//     evtloop.MakeMacro("mymacro+");
//     // replace the tasklist the first time
//     evtloop.MakeMacro("+mymacro+");
//     // replace the tasklist the second time
//     evtloop.MakeMacro("+mymacro");
//
void MEvtLoop::MakeMacro(const char *filename)
{
    TString name(filename);

    name = name.Strip(TString::kBoth);

    Bool_t open  = kTRUE;
    Bool_t close = kTRUE;
    if (name[0]=='+')
    {
        open = kFALSE;
        name.Remove(0, 1);
        name = name.Strip(TString::kBoth);
    }

    if (name[name.Length()-1]=='+')
    {
        close = kFALSE;
        name.Remove(name.Length()-1, 1);
        name = name.Strip(TString::kBoth);
    }

    if (!name.EndsWith(".C"))
        name += ".C";

    ofstream fout;

    if (!open)
    {
        fout.open(name, ios::app);
        fout << endl;
        fout << "   // ----------------------------------------------------------------------" << endl;
        fout << endl;
    }
    else
    {
        fout.open(name);

        time_t t = time(NULL);
        fout <<
            "/* ======================================================================== *\\" << endl <<
            "!" << endl <<
            "! *" << endl <<
            "! * This file is part of MARS, the MAGIC Analysis and Reconstruction" << endl <<
            "! * Software. It is distributed to you in the hope that it can be a useful" << endl <<
            "! * and timesaving tool in analysing Data of imaging Cerenkov telescopes." << endl <<
            "! * It is distributed WITHOUT ANY WARRANTY." << endl <<
            "! *" << endl <<
            "! * Permission to use, copy, modify and distribute this software and its" << endl <<
            "! * documentation for any purpose is hereby granted without fee," << endl <<
            "! * provided that the above copyright notice appear in all copies and" << endl <<
            "! * that both that copyright notice and this permission notice appear" << endl <<
            "! * in supporting documentation. It is provided \"as is\" without express" << endl <<
            "! * or implied warranty." << endl <<
            "! *" << endl <<
            "!" << endl <<
            "!" << endl <<
            "!   Author(s): Thomas Bretz et al. <mailto:tbretz@astro.uni-wuerzburg.de>" << endl <<
            "!" << endl <<
            "!   Copyright: MAGIC Software Development, 2000-2005" << endl <<
            "!" << endl <<
            "!" << endl <<
            "\\* ======================================================================== */" << endl << endl <<
            "// ------------------------------------------------------------------------" << endl <<
            "//" << endl <<
            "//     This macro was automatically created on" << endl<<
            "//             " << ctime(&t) <<
            "//        with the MEvtLoop::MakeMacro tool." << endl <<
            "//" << endl <<
            "// ------------------------------------------------------------------------" << endl << endl <<
            "void " << name(0, name.Length()-2) << "()" << endl <<
            "{" << endl;
    }

    SavePrimitive(fout, (TString)"" + (open?"open":"") + (close?"close":""));

    if (!close)
        return;

    fout << "}" << endl;

    *fLog << inf << "Macro '" << name << "' written." << endl;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MEvtLoop::StreamPrimitive(ostream &out) const
{
    out << "   MEvtLoop " << GetUniqueName();
    if (fName!="Evtloop")
        out << "(\"" << fName << "\")";
    out << ";" << endl;
}

// --------------------------------------------------------------------------
//
//
void MEvtLoop::SavePrimitive(ostream &out, Option_t *opt)
{
    TString options = opt;
    options.ToLower();

    if (HasDuplicateNames("MEvtLoop::SavePrimitive"))
    {
        out << "   // !" << endl;
        out << "   // ! WARNING - Your eventloop (MParList, MTaskList, ...) contains more than" << endl;
        out << "   // ! one object (MParContainer, MTask, ...) with the same name. The created macro" << endl;
        out << "   // ! may need manual intervention before it can be used." << endl;
        out << "   // !" << endl;
        out << endl;
    }

    if (!options.Contains("open"))
    {
        if (gListOfPrimitives)
        {
            *fLog << err << "MEvtLoop::SavePrimitive - Error: old file not closed." << endl;
            gListOfPrimitives->R__FOR_EACH(TObject, ResetBit)(BIT(15));
            delete gListOfPrimitives;
        }
        gListOfPrimitives = new TList;
    }

    if (fParList)
        fParList->SavePrimitive(out);

    MParContainer::SavePrimitive(out);

    if (fParList)
        out << "   " << GetUniqueName() << ".SetParList(&" << fParList->GetUniqueName() << ");" << endl;
    else
        out << "   // fParList empty..." << endl;
    out << "   if (!" << GetUniqueName() << ".Eventloop())" << endl;
    out << "      return;" << endl;

    if (!options.Contains("close"))
        return;

    gListOfPrimitives->R__FOR_EACH(TObject, ResetBit)(BIT(15));
    delete gListOfPrimitives;
    gListOfPrimitives = 0;
}

void MEvtLoop::SavePrimitive(ofstream &out, Option_t *o)
{
    SavePrimitive(static_cast<ostream&>(out), o);
}

// --------------------------------------------------------------------------
//
// Get a list of all conmtainer names which are somehow part of the
// eventloop. Chack for duplicate members and print a warning if
// duplicates are found. Return kTRUE if duplicates are found, otherwise
// kFALSE;
//
Bool_t MEvtLoop::HasDuplicateNames(TObjArray &arr, const TString txt) const
{
    arr.Sort();

    TIter Next(&arr);
    TObject *obj;
    TString name;
    Bool_t found = kFALSE;
    while ((obj=Next()))
    {
        if (name==obj->GetName())
        {
            if (!found)
            {
                *fLog << warn << endl;
                *fLog << " ! WARNING (" << txt << ")" << endl;
                *fLog << " ! Your eventloop (MParList, MTaskList, ...) contains more than" << endl;
                *fLog << " ! one object (MParContainer, MTask, ...) with the same name." << endl;
                *fLog << " ! Creating a macro from it using MEvtLoop::MakeMacro may create" << endl;
                *fLog << " ! a macro which needs manual intervention before it can be used." << endl;
                found = kTRUE;
            }
            *fLog << " ! Please rename: " << obj->GetName() << endl;
        }
        name = obj->GetName();
    }

    return found;
}

// --------------------------------------------------------------------------
//
// Get a list of all conmtainer names which are somehow part of the
// eventloop. Chack for duplicate members and print a warning if
// duplicates are found. Return kTRUE if duplicates are found, otherwise
// kFALSE;
//
Bool_t MEvtLoop::HasDuplicateNames(const TString txt) const
{
    if (!fParList)
        return kFALSE;

    TObjArray list;
    list.SetOwner();

    fParList->GetNames(list);

    return HasDuplicateNames(list, txt);
}

// --------------------------------------------------------------------------
//
// Reads a saved eventloop from a file. The default name is "Evtloop".
// Therefor an open file must exist (See TFile for more information)
//
//  eg:
//        TFile file("myfile.root", "READ");
//        MEvtLoop evtloop;
//        evtloop.Read();
//        evtloop.MakeMacro("mymacro");
//
Int_t MEvtLoop::Read(const char *name)
{
    if (!gFile)
    {
        *fLog << err << "MEvtloop::Read: No file found. Please create a TFile first." << endl;
        return 0;
    }

    if (!gFile->IsOpen())
    {
        *fLog << err << "MEvtloop::Read: File not open. Please open the TFile first." << endl;
        return 0;
    }

    Int_t n = 0;
    TObjArray list;

    n += TObject::Read(name);

    if (n==0)
    {
        *fLog << err << "MEvtloop::Read: No objects read." << endl;
        return 0;
    }

    n += list.Read((TString)name+"_names");

    fParList->SetNames(list);

    HasDuplicateNames(list, "MEvtLoop::Read");

    *fLog << inf << "Eventloop '" << name << "' read from file." << endl;

    return n;
}

// --------------------------------------------------------------------------
//
// If available print the contents of the parameter list.
//
void MEvtLoop::Print(Option_t *) const
{
    if (fParList)
        fParList->Print();
    else
        *fLog << all << "MEvtloop: No Parameter List available." << endl;
}

// --------------------------------------------------------------------------
//
// Writes a eventloop to a file. The default name is "Evtloop".
// Therefor an open file must exist (See TFile for more information)
//
//  eg:
//        TFile file("myfile.root", "RECREATE");
//        MEvtLoop evtloop;
//        evtloop.Write();
//        file.Close();
//
Int_t MEvtLoop::Write(const char *name, Int_t option, Int_t bufsize) const
{
    if (!gFile)
    {
        *fLog << err << "MEvtloop::Write: No file found. Please create a TFile first." << endl;
        return 0;
    }

    if (!gFile->IsOpen())
    {
        *fLog << err << "MEvtloop::Write: File not open. Please open the TFile first." << endl;
        return 0;
    }

    if (!gFile->IsWritable())
    {
        *fLog << err << "MEvtloop::Write: File not writable." << endl;
        return 0;
    }

    Int_t n = 0;

    TObjArray list;
    list.SetOwner();

    fParList->GetNames(list);

#if ROOT_VERSION_CODE < ROOT_VERSION(4,02,00)
    n += const_cast<MEvtLoop*>(this)->TObject::Write(name, option, bufsize);
#else
    n += TObject::Write(name, option, bufsize);
#endif

    if (n==0)
    {
        *fLog << err << "MEvtloop::Read: No objects written." << endl;
        return 0;
    }

    n += list.Write((TString)name+"_names", kSingleKey);

    HasDuplicateNames(list, "MEvtLoop::Write");

    *fLog << inf << "Eventloop written to file as " << name << "." << endl;

    return n;
}

// --------------------------------------------------------------------------
//
// Read the contents/setup of a parameter container/task from a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
//
// With the argument prefix you can overwrite the name of the MEvtLoop object
// as prefix - use with extreme care! The prifix argument must not end with
// a dot!
//
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Int_t MEvtLoop::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
//    if (!prefix.IsNull())
//        *fLog << warn << "WARNING - Second argument in MEvtLoop::ReadEnv has no meaning... ignored." << endl;

    if (prefix.IsNull())
        prefix = fName;
    prefix += ".";

    *fLog << inf << "Looking for resources with prefix " << prefix /*TEnv::fRcName << " from " << env.GetRcName()*/ << endl;

    fLog->ReadEnv(env, prefix, print);

    if (!fParList)
    {
        *fLog << warn << "WARNING - No parameter list to propagate resources to." << endl;
        return kTRUE;
    }

    if (fParList->ReadEnv(env, prefix, print)==kERROR)
    {
        *fLog << err << "ERROR - Reading resource file." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls 'ReadEnv' with a TEnv initialized with the given file name.
// If 'config=0' kTRUE is returned.
//
Bool_t MEvtLoop::ReadEnv(const char *config, Bool_t print)
{
    if (!config)
        return kTRUE;

    const Bool_t fileexist = !gSystem->AccessPathName(config, kFileExists);
    if (!fileexist)
    {
        *fLog << warn << "WARNING - resource file '" << config << "' not found... no resources applied." << endl;
        return kFALSE;
    }

    const TEnv env(config);
    return ReadEnv(env, "", print);
}

// --------------------------------------------------------------------------
//
// Write the contents/setup of a parameter container/task to a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Bool_t MEvtLoop::WriteEnv(TEnv &env, TString prefix, Bool_t print) const
{
    if (!prefix.IsNull())
        *fLog << warn << "WARNING - Second argument in MEvtLoop::WriteEnv has no meaning... ignored." << endl;

    prefix = fName;
    prefix += ".";

    *fLog << inf << "Writing resources with prefix " << prefix /*TEnv::fRcName << " to " << env.GetRcName()*/ << endl;

    fLog->WriteEnv(env, prefix, print);

    if (!fParList)
    {
        *fLog << warn << "WARNING - No parameter list to get resources from." << endl;
        return kTRUE;
    }


    if (fParList->WriteEnv(env, prefix, print)!=kTRUE)
    {
        *fLog << err << "ERROR - Writing resource file." << endl;
        return kFALSE;
    }

    return kTRUE;
}

void MEvtLoop::RecursiveRemove(TObject *obj)
{
    // If the tasklist was deleted... remove it
    if (obj==fTaskList)
        fTaskList = NULL;

    // If the parlist was deleted...invalidate par- and tasklist
    if (obj==fParList)
    {
        fParList=NULL;
        fTaskList=NULL;
    }

    // if the progress bar was deleted...remove it
    if (obj==fProgress)
        fProgress = NULL;

    // If it was something else check par- and tasklist
    if (fParList)
        fParList->RecursiveRemove(obj);
    else
        if (fTaskList) // Note that the tasklist is included in the parlist
            fTaskList->RecursiveRemove(obj);

    // Now check for fDisplay and fLog
    MParContainer::RecursiveRemove(obj);
}
