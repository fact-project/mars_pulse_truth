#ifndef MARS_MSQLMagic
#define MARS_MSQLMagic

#ifndef MARS_MSQLServer
#include "MSQLServer.h"
#endif

class MSQLMagic : public MSQLServer
{
    Bool_t fIsDummy;

public:
    MSQLMagic(TSQLServer *serv, const char *dbname=0, const char *tname=0, const char *col=0) :
        MSQLServer(serv, dbname, tname, col), fIsDummy(kFALSE)
    {
    }

    MSQLMagic(const char *connection, const char *user, const char *password) :
        MSQLServer(connection, user, password), fIsDummy(kFALSE)
    {
    }

    MSQLMagic(const char *link) : MSQLServer(link), fIsDummy(kFALSE)
    {
    }

    MSQLMagic(TEnv &env, const char *prefix=0) :
        MSQLServer(env, prefix), fIsDummy(kFALSE)
    {
    }

    MSQLMagic() : MSQLServer(), fIsDummy(kFALSE)
    {
    }


    void SetIsDummy(Bool_t dummy=kTRUE) { fIsDummy=dummy; }
    Bool_t IsDummy() const { return fIsDummy; }

    TString QueryValOf(TString col, const char *ext, const char *key);
    TString QueryValOfKey(TString col, const char *key);
    TString QueryNameOfKey(TString col, const char *key);
    Int_t   QueryKeyOfName(const char *col, const char *name, Bool_t insert=kTRUE);
    Int_t   QueryKeyOfVal(const char *col, const char *val);
    Int_t   QueryKeyOf(const char *col, const char *ext, const char *val);
    Bool_t  ExistStr(const char *column, const char *table, const char *test, const char *where=0);
    Bool_t  ExistRow(const char *table, const char *where);

    Int_t Insert(const char *table, const char *vars, const char *where=0);
    Int_t Update(const char *table, const char *vars, const char *where=0);
    Int_t Delete(const char *table, const char *where);
    Int_t InsertUpdate(const char *table, const char *col, const char *val, const char *vars);
    Int_t InsertUpdate(const char *table, const char *vars, const char *where);

    void Delete(const Option_t *o) { TObject::Delete(o); }

    ClassDef(MSQLMagic, 0) // An enhancement of MSQLServer featuring our database
};

#endif
