#ifndef MARS_MTFillMatrix
#define MARS_MTFillMatrix

#ifndef MARS_MParList
#include "MParList.h"
#endif

#ifndef MARS_MH3
#include "MH3.h"
#endif

class MRead;
class MTask;
class MHMatrix;
class MFilter;

class MTFillMatrix : public MParContainer
{
private:
    MH3      *fReference;
    MRead    *fReader;

    MHMatrix *fDestMatrix1;
    MHMatrix *fDestMatrix2;

    Int_t     fNumDestEvents1;
    Int_t     fNumDestEvents2;

    Int_t     fNumMaxEvents;

    MTask    *fWriteFile1;
    MTask    *fWriteFile2;

    TList     fPreCuts;
    TList     fPreTasks;
    TList     fPostTasks;

    void Init(const char *name, const char *title);

    void Add(const TList &src, const TClass *cls, TList &dest);

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    Bool_t CheckResult(MHMatrix *m, Int_t num) const;
    Bool_t WriteMatrix(MHMatrix *m, const TString &fname, Int_t i) const;

public:
    MTFillMatrix(const char *name=0, const char *title=0);
    MTFillMatrix(const MH3 *ref, const char *name=0, const char *title=0);
    ~MTFillMatrix();

    void SetDestMatrix1(MHMatrix *matrix, UInt_t num=0)
    {
        fDestMatrix1 = matrix;
        if (num>0)
            SetNumDestEvents1(num);
    }
    void SetWriteFile1(MTask *write, UInt_t num=0)
    {
        fWriteFile1 = write;
        if (num>0)
            SetNumDestEvents1(num);
    }
    void SetDestMatrix2(MHMatrix *matrix, UInt_t num=0)
    {
        fDestMatrix2 = matrix;
        if (num>0)
            SetNumDestEvents2(num);
    }
    void SetWriteFile2(MTask *write, UInt_t num=0)
    {
        fWriteFile2 = write;
        if (num>0)
            SetNumDestEvents2(num);
    }
    void SetNumDestEvents1(UInt_t num) { fNumDestEvents1 = num; }
    void SetNumDestEvents2(UInt_t num) { fNumDestEvents2 = num; }
    void SetMaxEvents(UInt_t num)      { fNumMaxEvents = num; }

    void SetReader(MRead *task) { fReader = task; }

    void AddPreCut(const char *rule);
    void AddPreCut(MFilter *f);
    void AddPreCuts(const TList &list);

    void ClearPreCuts()   { fPreCuts.Clear(); }
    void ClearPreTasks()  { fPreTasks.Clear(); }
    void ClearPostTasks() { fPostTasks.Clear(); }

    void AddPreTask(MTask *t);
    void AddPreTasks(const TList &list);

    void AddPostTask(MTask *t);
    void AddPostTasks(const TList &list);

    Bool_t Process(const MParList &plist=MParList());

    Bool_t WriteMatrix1(const TString &fname) const { return WriteMatrix(fDestMatrix1, fname, 1); }
    Bool_t WriteMatrix2(const TString &fname) const { return WriteMatrix(fDestMatrix2, fname, 2); }
    Bool_t WriteMatrices(const TString &fname) const;

    ClassDef(MTFillMatrix, 0) // Tool to fill matrices (eg. trainings- and test-matrices)
};

#endif
