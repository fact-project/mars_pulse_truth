#ifndef MARS_MContinue
#define MARS_MContinue

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MContinue                                                               //
//                                                                         //
// Does nothing than return kCONTINUE in the Process-fucntion              //
// (use with filters)                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MLog;
class MFilter;
class MTaskList;
class MStatusDisplay;

class MContinue : public MTask
{
private:
    MTaskList *fTaskList;  //! pointer to the present tasklist

    Int_t fRc;             // Return code returned in Process()

    // MTask
    Int_t  PreProcess(MParList *list);
    Int_t  Process() { return fRc; }
    Int_t  PostProcess();

    // MContinue
    enum { kIsOwner = BIT(14), kFilterIsPrivate = BIT(15), kAllowEmpty = BIT(16) };

public:
    MContinue(const TString rule="", const char *name=NULL, const char *title=NULL);
    MContinue(MFilter *f, const char *name=NULL, const char *title=NULL);
    ~MContinue();

    // MContinue
    void SetAllowEmpty(Bool_t b=kTRUE) { b ? SetBit(kAllowEmpty) : ResetBit(kAllowEmpty); }
    Bool_t IsAllowEmpty() const { return TestBit(kAllowEmpty); }

    void SetInverted(Bool_t i=kTRUE);
    Bool_t IsInverted() const;

    void SetRc(Int_t rc) { fRc = rc; }

    // MParContainer
    void SetDisplay(MStatusDisplay *d);
    void SetLogStream(MLog *lg);
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // TObject
    void Print(Option_t *o="") const; //*MENU*

    ClassDef(MContinue, 2) //Task returning kCONTINUE (or any other return code)
};

#endif
