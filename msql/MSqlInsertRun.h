#ifndef MARS_MRawRunInsertSql
#define MARS_MRawRunInsertSql

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MSQLServer;

class MRawRunHeader;

class MSqlInsertRun : public MTask
{
private:
    MSQLServer *fSqlServer;

    Bool_t fIsUpdate;

    Int_t   GetIndex(MRawRunHeader *h);
    Int_t   GetKey(const char *table, const char *where);
    TString GetKeyStr(const char *table, const char *where, Bool_t &rc);
    TString GetEntry(const char *table, const char *col, const char *where);

    Bool_t  IsRunExisting(MRawRunHeader *h, Bool_t &ok);
    Bool_t  IsFileExisting(MRawRunHeader *h, Bool_t &ok);

    TString MagicNumber(MRawRunHeader *h, Bool_t &ok);
    TString RunType(MRawRunHeader *h, Bool_t &ok);
    TString Source(MRawRunHeader *h, Bool_t &ok);
    TString Project(MRawRunHeader *h, Bool_t &ok);
    TString RawFilePath(const char *path, Bool_t &ok);

    Bool_t InsertRun(MRawRunHeader *h, Bool_t update);
    Bool_t InsertFile(MRawRunHeader *h, MParList *pList, Bool_t update);

    Int_t  PreProcess(MParList *pList);
    Int_t  PostProcess();

    Bool_t PrintError(const char *txt, const char *q) const;

public:
    MSqlInsertRun(const char *db, const char *user, const char *pw);
    MSqlInsertRun(const char *url);
    ~MSqlInsertRun();

    void SetUpdate(Bool_t u=kTRUE) { fIsUpdate=u; }

    ClassDef(MSqlInsertRun, 0) // Task to insert run into database
};

#endif
