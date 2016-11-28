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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MLog
//
// This is what we call the logging-system.
//
// It is derived from the C++ streaming classes and can handle our
// logging. The log output can be redirected to stdout, stderr, any other
// stream or a root window.
//
// There is a global log-instance which you can use like cout, id is gLog.
// A log-instance of your choice (gLog by default) is destributed to all
// Task which are used in an eventloop, so that you can redirect the output
// of one eventloop to where you want..
//
// The MLog stream has the advantage, that it can be used like the common
// C++ streams (for example cout). It can redirect the stream to different
// outputs (console, file, GUI) if necessary at the same time.
//
// It supports different debug levels. The debug level of the current
// stream contents is set by SetDebugLevel, the output level of the
// current stream can be set by SetOutputLevel.
//
// The header file MLogManip.h contains so called manipulators (like flush
// or setw from iomanip.h) which can manipulate these levels from within
// stream, for example:
//    gLog << debug(3) << "Hallo World " << endl;
// sets the debug level of the following stream to 3
//
// edev(), ddev() can be used to enable/disable an output device from
// within the stream. The enumerations are defined in MLog::_flags
//
// Commonly used abbreviations are also defined:
//    dbginf  Prints source file name and line number. Used for output
//            which people may like to look up in the code
//    all     Is streamed to the output in any case. Used for outputs
//            which are requested by the user (eg TObject::Print)
//    err     Should be used for fatal errors which stops the current
//            processing, eg:
//              gLog << err << "ERROR: TObject::Copy - Stopped" << endl;
//    warn    Warning means an error occured, but it is not clear whether
//            this results further procesing or not.
//    inf     Informs the user about what's going on. Mostly usefull for
//            debugging, but in general not necessary at all.
//    dbg     Use this for your private purpose to mark something as debug
//            output. This is _not_ ment to be persistent!
//
// If your console is capable of ANSI colors the stream is displayed
// in several colors:
//    all:    default
//    err:    red
//    warn:   yellow/brown
//    inf:    green
//    dbg:    blue (and all other levels)
//
// If you have a dark background on your console you might want to set
// an environment variable, eg:
//    export MARSDEFINES=-DHAVE_DARKBACKGROUND
// and recompile MLog.
//
// If your console can display it also 'underline' can be used. This
// underlines a text till the next 'endl', eg:
//    gLog << underline << "This is important!" << endl;
//
// To switch off ANSI support call: SetNoColors()
//
// gLog is a global stream defined like cout or cerr
//
//////////////////////////////////////////////////////////////////////////////
#include "MLog.h"

#include <stdlib.h>     // mkstemp

#include <fstream>
#include <iomanip>

#include <TROOT.h>      // gROOT->GetListOfCleanups()
#include <TSystem.h>

#ifdef _REENTRANT
#include <TMutex.h>
#endif
#include <TGTextView.h>

#include <TEnv.h>       // gEnv (ErrorHandler)
#include <TError.h>     // TError (SetErrorHandler)

#include "MArgs.h"
#include "MTime.h"
#include "MString.h"
#include "MParContainer.h"

#include "MLogHtml.h"
#include "MLogManip.h"  // inf,warn,err (MLog::ErrorHandler)

ClassImp(MLog);

using namespace std;

#undef DEBUG
//#define DEBUG


// root 3.02:
// check for TObjectWarning, TObject::Info, gErrorIgnoreLevel

const char MLog::kESC = '\033'; // (char)27
const char *const MLog::kEsc       = "\033[";
const char *const MLog::kReset     = "\033[0m";
const char *const MLog::kRed       = "\033[31m";
const char *const MLog::kGreen     = "\033[32m";
#ifdef HAVE_DARKBACKGROUND
const char *const MLog::kYellow    = "\033[33m\033[1m";
#else
const char *const MLog::kYellow    = "\033[33m";
#endif
const char *const MLog::kBlue      = "\033[34m";
const char *const MLog::kUnderline = "\033[4m";
const char *const MLog::kBlink     = "\033[5m";
const char *const MLog::kBright    = "\033[1m";
const char *const MLog::kDark      = "\033[2m";

//
// This is the definition of the global log facility
//
MLog gLog;

// --------------------------------------------------------------------------
//
// this strange usage of an unbufferd buffer is a workaround
// to make it work on Alpha and Linux!
//
void MLog::Init()
{
    //
    // Creat drawing semaphore
    //
#ifdef _REENTRANT
    fMuxGui    = new TMutex;
    fMuxStream = new TMutex;
#endif

    fPlugins = new TList;
    gROOT->GetListOfCleanups()->Add(fPlugins);
    fPlugins->SetBit(kMustCleanup);

    setp(&fBuffer, &fBuffer+1);
    *this << '\0';
}

// --------------------------------------------------------------------------
//
// default constructor which initializes the streamer and sets the device
// which is used for the output (i)
//
MLog::MLog(int i) : ostream(this), fPPtr(fBase), fEPtr(fBase+fgBufferSize), fOutputLevel(0), fDebugLevel((unsigned)-1), fDevice(i), fIsNull(kFALSE), fOut(NULL), fOutAllocated(kFALSE), fGui(NULL), fNumLines(0)
{
    Init();
}

// --------------------------------------------------------------------------
//
// default constructor which initializes the streamer and sets the given
// ofstream as the default output device
//
MLog::MLog(ofstream &sout) : ostream(this), fPPtr(fBase), fEPtr(fBase+fgBufferSize), fOutputLevel(0), fDebugLevel((unsigned)-1), fDevice(eFile), fIsNull(kFALSE), fOut(&sout), fOutAllocated(kFALSE), fGui(NULL), fNumLines(0)
{
    Init();
}

// --------------------------------------------------------------------------
//
// default constructor which initializes the streamer and sets the given
// TGTextView as the default output device
//
MLog::MLog(TGTextView &sout) : ostream(this), fPPtr(fBase), fEPtr(fBase+fgBufferSize), fOutputLevel(0), fDebugLevel((unsigned)-1), fDevice(eGui), fOut(NULL), fOutAllocated(kFALSE), fGui(&sout), fNumLines(0)
{
    Init();
}

// --------------------------------------------------------------------------
//
// default constructor which initializes the streamer and opens a file with
// the given name. Dependend on the flag the file is set as output device
// or not.
//
MLog::MLog(const char *fname, int flag) : ostream(this), fPPtr(fBase), fEPtr(fBase+fgBufferSize), fOutputLevel(0), fDebugLevel((unsigned)-1), fDevice(eFile), fIsNull(kFALSE), fGui(NULL), fNumLines(0)
{
    Init();

    AllocateFile(fname);
    CheckFlag(eFile, flag);
}

// --------------------------------------------------------------------------
//
//  Destructor, destroying the gui mutex.
//
MLog::~MLog()
{
    DeallocateFile();

#ifdef DEBUG
    TIter Next(fPlugins);
    TObject *o=0;
    while ((o=Next()))
    {
        cout << "Delete: " << o->GetName() << std::flush;
        cout << " [" << o->ClassName() << "]" << endl;
        delete o;
    }

    cout << "Delete: fPlugins " << fPlugins << "..." << std::flush;
#endif

    delete fPlugins;
#ifdef DEBUG
    cout << "done." << endl;
#endif

#ifdef _REENTRANT
    delete fMuxStream;
    delete fMuxGui;
#endif
}

// --------------------------------------------------------------------------
//
// copyt constructor
//
/*
MLog::MLog(MLog const& log)
{
//    fOutputLevel = log.fOutputLevel;
//    fDebugLevel  = log.fDebugLevel;
//    fDevice      = log.fDevice;
}
*/

void MLog::Underline()
{
    if (fIsNull)
        return;

    SetBit(kIsUnderlined);

    fPlugins->R__FOR_EACH(MLogPlugin, Underline)();

    if (TestBit(eNoColors))
        return;

    if (fDevice&eStdout)
        cout << kUnderline;

    if (fDevice&eStderr)
        cerr << kUnderline;
}

void MLog::Output(ostream &sout, int len)
{
    if (!TestBit(eNoColors))
        switch (fOutputLevel)
        {
            // do not output reset. Otherwise we reset underline in 0-mode
            // case 1: out << MLog::kReset; break; // all
        case 0:  break; // all = background color
        case 1:  sout << MLog::kRed;     break;  // err
        case 2:  sout << MLog::kYellow;  break;  // warn
        case 3:                                  // inf
        case 4:                                  // inf2
        case 5:  sout << MLog::kGreen;   break;  // inf3
        default: sout << MLog::kBlue;    break;  // all others (dbg)
        }

    if (len>0)
    {
        // Check for EOL
        const Int_t endline = fBase[len-1]=='\n' ? 1 : 0;
        // output text to screen (without trailing '\n')
        sout << TString(fBase, len-endline);
        // reset colors if working with colors
        if (!TestBit(eNoColors))
            sout << kReset;
        // output EOL of check found EOL
        if (endline)
        {
            sout << '\n';
            // Check whether text was underlined
            if (TestBit(kIsUnderlined) && TestBit(eNoColors))
            {
                sout << setw(len-1) << setfill('-') << "" << "\n";
                ResetBit(kIsUnderlined);
            }
        }
    }
    sout.flush();
}

void MLog::AddGuiLine(const TString &line)
{
    // add a new TString* to the array of gui lines
    TString **newstr = new TString*[fNumLines+1];
    memcpy(newstr, fGuiLines, fNumLines*sizeof(TString*));
    if (fNumLines>0)
        delete [] fGuiLines;
    fGuiLines = newstr;

    // add Gui line as last line of array
    fGuiLines[fNumLines++] = new TString(line);
}

// --------------------------------------------------------------------------
//
// This is the function which writes the stream physically to a device.
// If you want to add a new device this must be done here.
//
void MLog::WriteBuffer()
{
    //
    // restart writing to the buffer at its first char
    //
    const int len = fPPtr - fBase;

    fPPtr = fBase;

    if (fIsNull)
        return;

    if (fDevice&eStdout)
        Output(cout, len);

    if (fDevice&eStderr)
        Output(cerr, len);

    if (fDevice&eFile && fOut)
        fOut->write(fBase, len);

    fPlugins->R__FOR_EACH(MLogPlugin, SetColor)(fOutputLevel);
    fPlugins->R__FOR_EACH(MLogPlugin, WriteBuffer)(fBase, len);

    if (fDevice&eGui && fGui)
    {
        // check whether the current text was flushed or endl'ed
        const Int_t endline = fBase[len-1]=='\n' ? 1 : 0;

        // for the gui remove trailing characters ('\n' or '\0')
        fBase[len-endline]='\0';

        // add new text to line storage
        fGuiLine += fBase;

        if (endline)
        {
            AddGuiLine(fGuiLine);
            fGuiLine = "";

            // Check whether text should be underlined
            if (endline && TestBit(kIsUnderlined))
            {
                AddGuiLine("");
                fGuiLines[fNumLines-1]->Append('-', fGuiLines[fNumLines-2]->Length());
                ResetBit(kIsUnderlined);
            }
        }
    }
}

void MLog::UpdateGui()
{
    if (fNumLines==0)
        return;

    // lock mutex
    if (!LockUpdate("UpdateGui"))
    {
        Warning("UpdateGui", "Execution skipped");
        return;
    }

    TGText &txt=*fGui->GetText();

    // copy lines to TGListBox
    for (int i=0; i<fNumLines; i++)
    {
        // Replace all tabs by 7 white spaces
        fGuiLines[i]->ReplaceAll("\t", "       ");
        txt.InsText(TGLongPosition(0, txt.RowCount()), *fGuiLines[i]);
        delete fGuiLines[i];
    }
    delete [] fGuiLines;

    fNumLines=0;

    // cut text box top 1000 lines
    //    while (txt.RowCount()>1000)
    //        txt.DelLine(1);

    // show last entry
    fGui->Layout();
    fGui->SetVsbPosition(txt.RowCount()-1);

    // tell a main loop, that list box contents have changed
    fGui->SetBit(kHasChanged);

    // release mutex
    UnLockUpdate("UpdateGui");
}

bool MLog::LockUpdate(const char *msg)
{
#ifdef _REENTRANT
    if (fMuxGui->Lock()==13)
    {
        Info("LockUpdate", "%s - mutex is already locked by this thread\n", msg);
        return false;
    }
#endif
    return true;
}

bool MLog::UnLockUpdate(const char *msg)
{
#ifdef _REENTRANT
    if (fMuxGui->UnLock()==13)
    {
        Info("UnLockUpdate", "%s - tried to unlock mutex locked by other thread\n", msg);
        return false;
    }
#endif
    return true;
}

bool MLog::Lock(const char *msg)
{
#ifdef _REENTRANT
    if (fMuxStream->Lock()==13)
    {
        Error("Lock", "%s - mutex is already locked by this thread\n", msg);
        return false;
    }
//    while (fMuxStream->Lock()==13)
//        usleep(1);
//    {
//        Error("Lock", "%s - mutex is already locked by this thread\n", msg);
//        return false;
//    }
#endif
    return true;
}

bool MLog::UnLock(const char *msg)
{
#ifdef _REENTRANT
    if (fMuxStream->UnLock()==13)
    {
        Error("UnLock", "%s - tried to unlock mutex locked by other thread\n", msg);
        return false;
    }
#endif
    return true;
}

// --------------------------------------------------------------------------
//
// This is called to flush the buffer of the streaming devices
//
int MLog::sync()
{
    if (!LockUpdate("sync"))
        usleep(1);
    WriteBuffer();
    UnLockUpdate("sync");

    if (fDevice&eStdout)
    {
        if (!fIsNull && !TestBit(eNoColors))
            cout << kReset;
        cout.flush();
    }

    if (fDevice&eStderr)
        cerr.flush();

    if (fDevice&eFile && fOut)
        fOut->flush();

    return 0;
}

// --------------------------------------------------------------------------
//
// This function comes from streambuf and should
// output the buffer to the device (flush, endl)
// or handle a buffer overflow (too many chars)
// If a real overflow happens i contains the next
// chars which doesn't fit into the buffer anymore.
// If the buffer is not really filled i is EOF(-1).
//
int MLog::overflow(int i) // i=EOF means not a real overflow
{
    //
    // no output if
    //
    if (fOutputLevel <= fDebugLevel)
    {
        if (!LockUpdate("overflow"))
            usleep(1);

        *fPPtr++ = (char)i;

        if (fPPtr == fEPtr)
            WriteBuffer();

        UnLockUpdate("overflow");
    }

    return 0;
}

// --------------------------------------------------------------------------
//
// Print usage information setup in Setup()
//
void MLog::Usage()
{
    //                 1         2         3         4         5         6         7         8
    //        12345678901234567890123456789012345678901234567890123456789012345678901234567890
    *this << "   -v#                       Verbosity level # [default=2]" << endl;
    *this << "   -a, --no-colors           Do not use Ansii color codes" << endl;
    *this << "   --log[=file]              Write log-out to ascii-file [default: prgname.log]" << endl;
    *this << "   --html[=file]             Write log-out to html-file [default: prgname.html]" << endl;
    *this << "   --debug[=n]               Enable root debugging [default: gDebug=1]" << endl;
    *this << "   --null                    Null output (supresses all output)" << endl;
}

// --------------------------------------------------------------------------
//
// Setup MLog and global debug output from command line arguments.
//
void MLog::Setup(MArgs &arg)
{
    // FXIME: This is not really at a place where it belongs to!
    gDebug = arg.HasOption("--debug=") ? arg.GetIntAndRemove("--debug=") : 0;
    if (gDebug==0 && arg.HasOnlyAndRemove("--debug"))
        gDebug=1;

    TString f1 = arg.GetStringAndRemove("--log=", "");
    if (f1.IsNull() && arg.HasOnlyAndRemove("--log"))
        f1 = MString::Format("%s.log", arg.GetName());
    if (!f1.IsNull())
    {
        SetOutputFile(f1);
        EnableOutputDevice(eFile);
    }

    TString f2 = arg.GetStringAndRemove("--html=", "");
    if (f2.IsNull() && arg.HasOnlyAndRemove("--html"))
        f2 = MString::Format("%s.html", arg.GetName());
    if (!f2.IsNull())
    {
        MLogHtml *html = new MLogHtml(f2);
        html->SetBit(kCanDelete);
        AddPlugin(html);
    }

    const Bool_t null = arg.HasOnlyAndRemove("--null");
    if (null)
        SetNullOutput();

    if (arg.HasOnlyAndRemove("--no-colors") || arg.HasOnlyAndRemove("-a"))
        SetNoColors();

    SetDebugLevel(arg.GetIntAndRemove("-v", 2));
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv:
//   MLog.VerbosityLevel: 0, 1, 2, 3, 4
//   MLog.DebugLevel: 0, 1, 2, 3, 4
//   MLog.NoColors
//
// Depending on your setup it might be correct to use something like:
//   Job1.MLog.VerbosityLevel: 1
//   Job1.DebugLevel: 2
//   Job1.MLog.NoColors
//
void MLog::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    MParContainer mlog("MLog");

    if (mlog.IsEnvDefined(env, prefix+"MLog", "VerbosityLevel", print))
        SetDebugLevel(mlog.GetEnvValue(env, prefix+"MLog", "VerbosityLevel", 2));
    else
        if (mlog.IsEnvDefined(env, "MLog", "VerbosityLevel", print))
            SetDebugLevel(mlog.GetEnvValue(env, "MLog", "VerbosityLevel", 2));

    if (mlog.IsEnvDefined(env, prefix+"MLog", "DebugLevel", print))
        gDebug = mlog.GetEnvValue(env, prefix+"MLog", "DebugLevel", 0);
    else
        if (mlog.IsEnvDefined(env, "MLog", "DebugLevel", print))
            gDebug = mlog.GetEnvValue(env, "MLog", "DebugLevel", 0);

    if (mlog.IsEnvDefined(env, prefix+"MLog", "NoColors", print))
        SetNoColors(mlog.GetEnvValue(env, prefix+"MLog", "NoColors", kFALSE));
    else
        if (mlog.IsEnvDefined(env, "MLog", "NoColors", print))
            SetNoColors(mlog.GetEnvValue(env, "MLog", "NoColors", kFALSE));
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv:
//   MLog.VerbosityLevel: 0, 1, 2, 3, 4
//   MLog.DebugLevel: 0, 1, 2, 3, 4
//   MLog.NoColors
//
// Depending on your setup it might be correct to use something like:
//   Job1.MLog.VerbosityLevel: 1
//   Job1.DebugLevel: 2
//   Job1.MLog.NoColors
//
void MLog::WriteEnv(TEnv &, TString prefix, Bool_t) const
{
    if (!prefix.IsNull())
        prefix += ".";
    prefix += "MLog";

    cout << "MLog::WriteEnv: not yet implemented!" << endl;
}

// --------------------------------------------------------------------------
//
// Create a new instance of an file output stream
// an set the corresponding flag
//
void MLog::AllocateFile(const char *fname)
{
    // gcc 3.2:
    const char *txt = "logXXXXXX";

    TString n(fname ? fname : txt);
    gSystem->ExpandPathName(n);

    fOut = new ofstream(n.Data());

    // switch off buffering
    fOut->rdbuf()->pubsetbuf(0,0);

    fOutAllocated = kTRUE;
}

// --------------------------------------------------------------------------
//
// if fout was allocated by this instance of MLooging
// delete it.
//
void MLog::DeallocateFile()
{
    if (fOutAllocated)
        delete fOut;
}

// --------------------------------------------------------------------------
//
// if necessary delete the old in stance of the file
// output stream and create a new one
//
void MLog::ReallocateFile(const char *fname)
{
    DeallocateFile();
    AllocateFile(fname);
}

// --------------------------------------------------------------------------
//
// This function checks if a device should get enabled or disabled.
//
void MLog::CheckFlag(Flags_t chk, int flag)
{
    if (flag==-1)
        return;

    flag ? EnableOutputDevice(chk) : DisableOutputDevice(chk);
}

// --------------------------------------------------------------------------
//
// Add a plugin to which the output should be redirected, eg. MLogHtml
// The user has to take care of its deletion. If the plugin is deleted
// (and the kMustCleanup bit was not reset accidentaly) the plugin
// is automatically removed from the list of active plugins.
//
// If MLog should take the ownership call plug->SetBit(kCanDelete);
//
void MLog::AddPlugin(MLogPlugin *plug)
{
    fPlugins->Add(plug);

    // Make sure that it is recursively deleted from all objects in ListOfCleanups
    plug->SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
// Returns "yyyy-mm-dd user@host gROOT->GetName()[pid]"
//
TString MLog::Intro()
{
    UserGroup_t *user = gSystem->GetUserInfo();

    TString rc;
    rc += MTime(-1).GetSqlDateTime();
    rc += " ";
    rc += user->fUser;
    rc += "@";
    rc += gSystem->HostName();
    rc += " ";
    rc += gROOT->GetName();
    rc += "[";
    rc += gSystem->GetPid();
    rc += "] ";

    delete user;

    return rc;
}

// --------------------------------------------------------------------------
//
// Check whether errors at this level should be ignored.
//
bool MLog::ErrorHandlerIgnore(Int_t level)
{
    // The default error handler function. It prints the message on stderr and
    // if abort is set it aborts the application.
   if (gErrorIgnoreLevel == kUnset) {
      R__LOCKGUARD2(gErrorMutex);

      gErrorIgnoreLevel = 0;
      if (gEnv) {
         TString lvl = gEnv->GetValue("Root.ErrorIgnoreLevel", "Info");
         if (!lvl.CompareTo("Info",TString::kIgnoreCase))
            gErrorIgnoreLevel = kInfo;
         else if (!lvl.CompareTo("Warning",TString::kIgnoreCase))
            gErrorIgnoreLevel = kWarning;
         else if (!lvl.CompareTo("Error",TString::kIgnoreCase))
            gErrorIgnoreLevel = kError;
         else if (!lvl.CompareTo("Break",TString::kIgnoreCase))
            gErrorIgnoreLevel = kBreak;
         else if (!lvl.CompareTo("SysError",TString::kIgnoreCase))
            gErrorIgnoreLevel = kSysError;
         else if (!lvl.CompareTo("Fatal",TString::kIgnoreCase))
            gErrorIgnoreLevel = kFatal;
      }
   }

   return level < gErrorIgnoreLevel;
}

// --------------------------------------------------------------------------
//
// Output the root error message to the log-stream.
//
void MLog::ErrorHandlerPrint(Int_t level, const char *location, const char *msg)
{
    R__LOCKGUARD2(gErrorMutex);

    if (level >= kError)
        gLog << "ROOT:Error";
    else
        if (level >= kSysError)
            gLog << "SysError";
        else
            if (level >= kBreak)
                gLog << "\n *** Break ***";
            else
                if (level >= kFatal)
                    gLog << "Fatal";
                else
                    if (level >= kWarning)
                        gLog << "ROOT:Warning";
                    else
                        if (level >= kInfo)
                            gLog << "ROOT:Info";

    if (level >= kBreak && level < kSysError)
        gLog << ": " << msg << std::endl;
    else
        if (location==0 || location[0]==0)
            gLog << ": " << msg << std::endl;
        else
            gLog << " in <" << location << ">: " << msg << std::endl;
}

// --------------------------------------------------------------------------
//
// A new error handler using gLog instead of stderr as output.
// It is mainly a copy of root's DefaultErrorHandler
// (see TError.h and TError.cxx)
//
void MLog::ErrorHandlerCol(Int_t level, Bool_t abort, const char *location, const char *msg)
{
    if (ErrorHandlerIgnore(level))
        return;

    // This is a really stupid hack/workaround to suppress these
    // annoying errors in case of a log-scale set too early
    if (level==kError && !strcmp(location, "THistPainter::PaintInit"))
        level=kInfo+2;

    // This is a really stupid hack/workaround to suppress these
    // annoying errors in case of 0 number of points
    if (level==kError && !strcmp(location, "TGraphPainter::PaintGraph"))
        level=kInfo+2;

    // This is a really stupid hack/workaround to suppress these
    // annoying errors in case of 0 number of points
    if (level==kWarning && !strcmp(location, "Fit"))
        level=kInfo+2;

    gLog << std::flush;

    const Int_t store = gLog.GetOutputLevel();

    if (level >= kInfo)
        gLog << inf;
    if (level==kInfo+1)
        gLog << inf2;
    if (level==kInfo+2)
        gLog << inf3;
    if (level >= kWarning)
        gLog << warn;
    if (level >= kError)
        gLog << err;

    ErrorHandlerPrint(level, location, msg);

    gLog << std::flush;

    gLog.SetOutputLevel(store);
    if (!abort)
        return;

    gLog << err << "aborting" << std::endl;
    if (gSystem) {
        gSystem->StackTrace();
        gLog.SetOutputLevel(store);
        gSystem->Abort();
    }
    else
    {
        gLog.SetOutputLevel(store);
        ::abort();
    }
}

// --------------------------------------------------------------------------
//
// A new error handler using gLog instead of stderr as output.
// It is mainly a copy of root's DefaultErrorHandler
// (see TError.h and TError.cxx)
//
void MLog::ErrorHandlerAll(Int_t level, Bool_t abort, const char *location, const char *msg)
{
    if (ErrorHandlerIgnore(level))
        return;

    gLog << std::flush << all;

    ErrorHandlerPrint(level, location, msg);

    gLog << std::flush;
    if (!abort)
        return;

    gLog << err << "aborting" << std::endl;
    if (gSystem) {
        gSystem->StackTrace();
        gSystem->Abort();
    } else
        ::abort();
}

// --------------------------------------------------------------------------
//
// Redirect the root ErrorHandler (see TError.h) output to gLog.
//
// The diffrent types are:
//  kColor:      Use gLog colors
//  kBlackWhite: Use all-qualifier (as in gLog << all << endl;)
//  kDefault:    Set back to root's default error handler
//               (redirect output to stderr)
//
void MLog::RedirectErrorHandler(ELogType typ)
{
    switch (typ)
    {
    case kColor:
        SetErrorHandler(MLog::ErrorHandlerCol);
        break;
    case kBlackWhite:
        SetErrorHandler(MLog::ErrorHandlerAll);
        break;
    case kDefault:
        SetErrorHandler(DefaultErrorHandler);
    }
}
