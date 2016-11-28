#ifndef MARS_MEvtLoop
#define MARS_MEvtLoop

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MEvtLoop                                                                //
//                                                                         //
// Class to execute the tasks in a tasklist                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MTask;
class MParList;
class MTaskList;
class TGProgressBar;
#ifdef __MARS__
class MProgressBar;
#endif

class MEvtLoop : public MParContainer
{
private:
    MParList  *fParList;
    MTaskList *fTaskList;      //!

    TGProgressBar *fProgress;  //!

    ULong_t fNumEvents;        //!

    enum { kIsOwner = BIT(14), kPrivateDisplay = BIT(15) };

    Bool_t HasDuplicateNames(const TString txt) const;
    Bool_t HasDuplicateNames(TObjArray &arr, const TString txt) const;

    void StreamPrimitive(std::ostream &out) const;

    Bool_t ProcessGuiEvents(Int_t num, Int_t rc);

public:
    enum Statistics_t {
        kNoStatistics   = 0,
        kStdStatistics  = 1,
        kFullStatistics = 2
    };

    MEvtLoop(const char *name="Evtloop");
    virtual ~MEvtLoop();

    void       SetParList(MParList *p);
    MParList  *GetParList() const  { return fParList; }
    MTaskList *GetTaskList() const { return fTaskList; }

    MTask *FindTask(const char *name) const;
    MTask *FindTask(const MTask *obj) const;

    MStatusDisplay *GetDisplay() { return fDisplay; }
    void SetDisplay(MStatusDisplay *d);
    void SetPrivateDisplay(Bool_t b=kTRUE) { b ? SetBit(kPrivateDisplay) : ResetBit(kPrivateDisplay); } // Prevent status display from being cascaded in PreProcess

    void SetOwner(Bool_t enable=kTRUE);

    void SetProgressBar(TGProgressBar *bar);
#ifdef __MARS__
    void SetProgressBar(MProgressBar *bar);
#endif

    Bool_t PreProcess();
    Int_t  Process(UInt_t maxcnt);
    Bool_t PostProcess() const;

    Bool_t Eventloop(UInt_t maxcnt=0, Statistics_t printstat=kStdStatistics);

    void MakeMacro(const char *filename="evtloop.C");

    void SavePrimitive(std::ostream &out, Option_t *o="");
    void SavePrimitive(std::ofstream &out, Option_t *o="");

    Int_t Read(const char *name="Evtloop");
    Int_t Write(const char *name="Evtloop", Int_t option=0, Int_t bufsize=0) const;
    Int_t Write(const char *name="Evtloop", Int_t option=0, Int_t bufsize=0)
    {
        return const_cast<MEvtLoop*>(this)->Write(name, option, bufsize);
    }

    void Print(Option_t *opt="") const;

    Int_t  ReadEnv(const TEnv &env, TString prefix="", Bool_t print=kFALSE);
    Bool_t WriteEnv(TEnv &env, TString prefix="", Bool_t print=kFALSE) const;

    Bool_t ReadEnv(const char *config, Bool_t print=kFALSE);

    void RecursiveRemove(TObject *obj);

    static UInt_t GetMemoryUsage();

    ClassDef(MEvtLoop, 1) // Class to execute the tasks in a tasklist
};

#endif
