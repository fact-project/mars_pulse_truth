#ifndef MARS_MTaskList
#define MARS_MTaskList

///////////////////////////////////////////////////////////////////////
//                                                                   //
// MTaskList                                                         //
//                                                                   //
// Collection of tasks to be processed in the eventloop              //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MLog;
class MParList;
class MInputStreamID;

class MTaskList : public MTask
{
private:
    static const TString gsDefName;  // default name
    static const TString gsDefTitle; // default title

    TList    *fTasks;        // Container for the ordered list of different tasks
    TList     fTasksProcess; //! Task which overload the Process function
    MParList *fParList;      //! The parameter list given in PreProcess

    UInt_t fNumPasses;
    UInt_t fNumPass; //!

    enum { kIsOwner = BIT(14) };

    void   Remove(MTask *task);
    void   StreamPrimitive(std::ostream &out) const;
    Bool_t CheckAddToList(MTask *task, /*const char *tType,*/ const MTask *where=NULL) const;
    Int_t  ProcessTaskList();

public:
    MTaskList(const char *name=NULL, const char *title=NULL);
    MTaskList(const MTaskList &ts);

    ~MTaskList();

    void SetLogStream(MLog *log);
    void SetDisplay(MStatusDisplay *d);
    void SetAccelerator(Byte_t acc=kAccStandard);
    void SetNumPasses(UInt_t cnt=0) { fNumPasses=cnt; }

    UInt_t GetNumPass() const { return fNumPass; }
    UInt_t GetNumPasses() const { return fNumPasses; }

    Bool_t AddToListBefore(MTask *task, const MTask *where, const char *tType="All");
    Bool_t AddToListAfter(MTask *task, const MTask *where, const char *tType="All");
    Bool_t AddToList(MTask *task, const char *tType="All");
    Bool_t AddToList(const TList &list, const char *tType="All");
    Bool_t AddToListAfter(const TList &list, const MTask *where, const char *tType="All");
    Bool_t AddToListBefore(const TList &list, const MTask *where, const char *tType="All");

    void SetSerialNumber(Byte_t num);

    Bool_t Replace(MTask *obj);
    Bool_t RemoveFromList(MTask *task);
    Bool_t RemoveFromList(const TList &list);

    TObject *FindObject(const char *name) const;
    TObject *FindObject(const TObject *obj) const;

    MTask *FindTask(const char *name) const
    {
        return (MTask*)FindObject(name);
    }
    MTask *FindTask(const MTask *obj) const
    {
        return (MTask*)FindObject(obj);
    }
    MTaskList *FindTaskList(const char *task);
    MTaskList *FindTaskList(const MTask *task);

    Bool_t ReInit(MParList *pList=NULL);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    void Print(Option_t *opt = "") const;
    void PrintStatistics(const Int_t lvl=0, Bool_t title=kFALSE, Double_t time=-1) const;
    void SetOwner(Bool_t enable=kTRUE);

    const TList *GetList() const { return fTasks; }

    void GetNames(TObjArray &arr) const;
    void SetNames(TObjArray &arr);

    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);
    Bool_t WriteEnv(TEnv &env, TString prefix, Bool_t print=kFALSE) const;

    void   RecursiveRemove(TObject *obj);

    operator TIterator*() const;

    ClassDef(MTaskList, 1) //collection of tasks to be performed in the eventloop
};

#endif
