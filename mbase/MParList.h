#ifndef MARS_MParList
#define MARS_MParList

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MParList                                                                //
//                                                                         //
// List of parameter containers (MParContainer)                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TClass;
class TOrdCollection;

class MLog;
class MTask;
class MTaskList;

class MParList : public MParContainer
{
private:
    TOrdCollection *fContainer;	 // Collection of Parameter and Data Containers
    TOrdCollection *fAutodelete; //! All what this list contains is deleted in the destructor

    static TString GetClassName(const char *classname);
    static TString GetObjectName(const char *classname, const char *objname);

    enum { kIsOwner = BIT(14) };

    void StreamPrimitive(std::ostream &out) const;

    void Init(const char *name, const char *title);

public:
    enum { kDoNotReset = BIT(17), kIsProcessing = BIT(18) };

    MParList(const char *name=NULL, const char *title=NULL);
    MParList(const MParList &ts, const char *name=NULL, const char *title=NULL);

    virtual ~MParList();

    Bool_t AddToList(MParContainer *obj, MParContainer *where = NULL);
    void   AddToList(TObjArray *list);

    Bool_t Replace(MParContainer *obj);
    void   Remove(MParContainer *obj);

    void SetLogStream(MLog *log);
    void SetDisplay(MStatusDisplay *d);

    TObject   *FindObject(const char *name) const;
    TObject   *FindObject(const TObject *obj) const;

    TObject   *FindObject(const char *name, const char *classname) const;
    TObject   *FindObject(const TObject *obj, const char *classname) const;

    MTask     *FindTask(const char *name, const char *tlist="MTaskList") const;
    MTaskList *FindTaskListWithTask(const char *name) const;
    MTaskList *FindTaskListWithTask(const MTask *task) const;

    MParContainer *FindCreateObj(const char *classname, const char *objname=NULL);

    TObjArray FindObjectList(const char *name, UInt_t first, const UInt_t last) const;
    TObjArray FindObjectList(const char *name, const UInt_t num) const
    {
        return FindObjectList(name, 0, num);
    }

    TObjArray FindCreateObjList(const char *cname, UInt_t first, const UInt_t last, const char *oname=NULL);
    TObjArray FindCreateObjList(const char *cname, const UInt_t num, const char *oname=NULL)
    {
        return FindCreateObjList(cname, 0, num, oname);
    }

    static TObjArray CreateObjList(const char *cname, UInt_t first, const UInt_t last, const char *oname=NULL);
    static TObjArray CreateObjList(const char *cname, const UInt_t num, const char *oname=NULL)
    {
        return CreateObjList(cname, 0, num, oname);
    }

    void RecursiveRemove(TObject *obj);

    void Reset();
    void SetReadyToSave(Bool_t flag=kTRUE);

    void SetOwner(Bool_t enable=kTRUE);
    Bool_t IsOwner() const { return TestBit(kIsOwner); }

    void Print(Option_t *t = NULL) const;

    void GetNames(TObjArray &arr) const;
    void SetNames(TObjArray &arr);

    void SavePrimitive(std::ostream &out, Option_t *o="");
    void SavePrimitive(std::ofstream &out, Option_t *o="");

    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);
    Bool_t WriteEnv(TEnv &env, TString prefix, Bool_t print=kFALSE) const;

    operator TIterator*() const;

    ClassDef(MParList, 1) // list of parameter containers (MParContainer)
};

#endif
