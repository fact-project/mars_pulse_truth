/* ======================================================================== *\
!  $Name: not supported by cvs2svn $:$Id: MLog.h,v 1.38 2008-02-24 11:37:48 tbretz Exp $
\* ======================================================================== */
#ifndef MARS_MLog
#define MARS_MLog

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TSystem
#include <TSystem.h>
#endif

#include <iostream>  // base classes for MLog

class MArgs;

class TEnv;
class TMutex;
class TGTextView;

class MLogPlugin;

class MLog : public std::streambuf, public std::ostream, public TObject
{
public:
    typedef enum _flags {
        eStdout   = 0x001,
        eStderr   = 0x002,
        eFile     = 0x004,
        eGui      = 0x008,
        eNoColors = 0x400  //BIT(15)
    } Flags_t;

    enum ELogBits {
        kHasChanged = BIT(14)  // if gui has changed
    };

    enum ELogType {
        kDefault,
        kColor,
        kBlackWhite
    };

private:
    enum {
        kIsUnderlined = BIT(15)//, kIsAutoIntro = BIT(16)
    };
    static const char kESC;
    static const char *const kEsc;
    static const char *const kReset;
    static const char *const kRed;
    static const char *const kGreen;
    static const char *const kYellow;
    static const char *const kBlue;
    static const char *const kUnderline;
    static const char *const kBlink;
    static const char *const kBright;
    static const char *const kDark;

    static const int fgBufferSize = 160;  // two standard lines

    char        fBuffer;      //!
    char        fBase[fgBufferSize+1]; //! Buffer to store the data in
    char       *fPPtr;        //! Pointer to present position in buffer
    const char *fEPtr;        //! Pointer to end of buffer

    UInt_t fOutputLevel;      //! Present output level of the stream
    UInt_t fDebugLevel;       //! Present global debug level
    UInt_t fDevice;           //! Flag to indicate the present streams

    Bool_t fIsNull;           //! Switch output completely off

    std::ofstream *fOut;      //! possible file output stream
    Bool_t      fOutAllocated; //! flag if fout is created by MLogging
    TGTextView *fGui;          //! Text View output

    Bool_t     fIsDirectGui;  //! Pipe text directly to the GUI (for single threaded environments)
    TString  **fGuiLines;     //! Lines to pipe to gui
    Int_t      fNumLines;     //!
    TString    fGuiLine;      //!

#ifdef _REENTRANT
    TMutex *fMuxGui;          //! Mutex locking access of TGListBox
    TMutex *fMuxStream;       //! Mutex locking access to streaming
#endif

    TList *fPlugins;

    void Init();

    void WriteBuffer();
    int sync();
    int overflow(int i); // i=EOF means not a real overflow

    void AllocateFile(const char *f);
    void DeallocateFile();
    void ReallocateFile(const char *f);
    void CheckFlag(Flags_t chk, int flag);
    void Output(std::ostream &out, int len);
    void AddGuiLine(const TString& line);

    // User defined error handling (see TError.h)
    static bool ErrorHandlerIgnore(Int_t level);
    static void ErrorHandlerPrint(Int_t level, const char *location, const char *msg);
    static void ErrorHandlerCol(Int_t level, Bool_t abort, const char *location, const char *msg);
    static void ErrorHandlerAll(Int_t level, Bool_t abort, const char *location, const char *msg);

public:

    MLog(int i=eStdout);
    MLog(std::ofstream &out);
    MLog(TGTextView &out);
    MLog(const char *fname, int flag=-1);

    MLog(MLog const& log) : std::ios(), std::streambuf(), std::ostream((std::streambuf*)&log), TObject()
    {
        fOutputLevel = log.fOutputLevel;
        fDebugLevel  = log.fDebugLevel;
        fDevice      = log.fDevice;
    }
    ~MLog();

    static TString Intro();
    static void RedirectErrorHandler(ELogType typ=kColor);

    bool LockUpdate(const char *msg);
    bool UnLockUpdate(const char *msg);

    bool Lock(const char *msg="");
    bool UnLock(const char *msg="");

    void EnableDirectGui()  { fIsDirectGui = kTRUE; }
    void DisableDirectGui() { fIsDirectGui = kFALSE; }
    void UpdateGui();

    void Underline();

    void SetDebugLevel(int i)           { fDebugLevel  =  i;   }
    int  GetDebugLevel() const          { return fDebugLevel;  }
    void SetOutputLevel(int i)          { fOutputLevel =  i;   }
    int  GetOutputLevel() const         { return fOutputLevel; }
    void SetOutputDevice(int i)         { fDevice      =  i;   }
    void EnableOutputDevice(Flags_t f)  { fDevice     |=  f;   }
    void DisableOutputDevice(Flags_t f) { fDevice     &= ~f;   }
    void operator=(std::ofstream &sout) { SetOutputFile(sout); }
    void operator=(TGTextView *sout)    { SetOutputGui(sout);  }
    //void SetAutoIntro(Bool_t b=kTRUE)   { b ? SetBit(kIsAutoIntro) : SetBit(kIsAutoIntro); }

    Bool_t IsNullOutput() const { return fIsNull; }
    Bool_t IsOutputDeviceEnabled(int i) const { return fDevice & i; }

    void SetOutputGui(TGTextView *sout, int flag=-1)
    {
        fGui = sout;
        CheckFlag(eGui, flag);
    }

    void SetOutputFile(std::ofstream &sout, int flag=-1)
    {
        //
        // Set new output file by a given stream. The new output
        // file is not deleted automatically. If no flag is specified
        // the state of the file-device stream is unchanged.
        // if the a flag is specified the state is changed
        // in correspondance to the flag
        //
        DeallocateFile();
        fOut = &sout;
        CheckFlag(eFile, flag);
    }

    void SetOutputFile(const char *f=NULL, int flag=-1)
    {
        //
        // Set new output file by name. The new output file must
        // not be deleted by the user. If no flag is specified
        // the state of the file-device stream is unchanged.
        // if the a flag is specified the state is changed
        // in correspondance to the flag
        //
        ReallocateFile(f);
        CheckFlag(eFile, flag);
    }

    std::ofstream &GetOutputFile()
    {
        //
        // Get the file output stream from MLogging
        // if no file output stream is existing yet it will be created.
        // For the creating a C-Temporary file name is used.
        // if the stream is created here the user must not delete it
        //
        // What a pitty, but it seems, that there is now way to ask
        // an ofstream for the corresponding file name. Elsewhise
        // I would implement a GetFileName-function, too.
        //
        if (!fOut)
            ReallocateFile(NULL);
        return *fOut;
    }

    // FIXME: Switch off colors when on....
    void SetNullOutput(Bool_t n=kTRUE) { fIsNull = n; }

    void SetNoColors(Bool_t flag=kTRUE) { flag ? SetBit(eNoColors) : ResetBit(eNoColors); }

    void Setup(MArgs &arg);
    void Usage();

    void ReadEnv(const TEnv &env, TString prefix="", Bool_t print=kFALSE);
    void WriteEnv(TEnv &env, TString prefix="", Bool_t print=kFALSE) const;

    MLog &Separator(TString str="", int outlvl=0)
    {
        if (!str.IsNull())
        {
            const Int_t l = (78-str.Length())/2-3;
            str.Prepend("={ ");
            str.Prepend('-', l<1?1:l);
            str.Append(" }=");
            str.Append('-', l<1?1:l);
        }
        if (str.Length()<78)
            str.Append('-', 78-str.Length());

        const int save = fOutputLevel;
        SetOutputLevel(outlvl);
        (*this) << str << std::endl;
        fOutputLevel = save;

        return *this;
    }

    void AddPlugin(MLogPlugin *plug);

    ClassDef(MLog, 0) // This is what we call 'The logging system'
};

//
// This is the definition of a global output stream, which by
// default pipes all output to the stdout
//
R__EXTERN MLog gLog;

#endif
