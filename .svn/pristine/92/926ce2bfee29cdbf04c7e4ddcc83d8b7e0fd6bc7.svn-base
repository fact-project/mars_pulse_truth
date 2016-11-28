#ifndef MARS_MSQLServer
#define MARS_MSQLServer

#ifndef ROOT_TList
#include <TList.h>
#endif

class TEnv;
class TArrayI;

class TSQLServer;
class TSQLResult;

class MTreeSQL;

class MSQLServer : public TObject
{
private:
    TSQLServer *fServ;
    TList fTrees;

    TString fDataBase;
    TString fTable;
    TString fColumn;

    TList   fList;

    enum Type_t { kIsZombie, kIsServer, kIsDataBase, kIsTable, kIsColumn };

    Type_t  fType;

    enum { kIsOwner=BIT(14) };

    Bool_t IsFolder() const { return kTRUE; }

    Bool_t PrintError(const char *txt, const char *q) const;

    TString GetFields() const;

    void BrowseDataBase(TBrowser *b);
    void BrowseTable(TBrowser *b);
    void BrowseColumn(TBrowser *b);
    void BrowseServer(TBrowser *b);

    void Browse(TBrowser *b)
    {
        if (!b)
            return;

        switch (fType)
        {
        case kIsServer:   BrowseServer(b);   break;
        case kIsDataBase: BrowseDataBase(b); break;
        case kIsTable:    BrowseTable(b);    break;
        case kIsColumn:   BrowseColumn(b);   break;
        default:
            break;
        }
    }

    const char *GetNameTable()    const { return Form("%s/%s",    (const char*)fDataBase, (const char*)fTable); }
    const char *GetNameColumn()   const { return Form("%s/%s/%s", (const char*)fDataBase, (const char*)fTable, (const char*)fColumn); }

    Bool_t Split(TString &url, TString &user, TString &pasw) const;

    void Init(const char *connection, const char *user, const char *password);
    void InitEnv(TEnv &env, const char *prefix=0);

public:
    MSQLServer(TSQLServer *serv, const char *dbname=0, const char *tname=0, const char *col=0)
        : fServ(serv), fDataBase(dbname), fTable(tname), fColumn(col), fType(kIsZombie)
    {
        fList.SetOwner();

        fType = kIsColumn;

        if (fColumn.IsNull())
            fType = kIsTable;

        if (fTable.IsNull() && fColumn.IsNull())
            fType = kIsDataBase;

        if (fDataBase.IsNull() && fTable.IsNull() && fColumn.IsNull())
            fType = kIsZombie;

        if (!serv)
            fType = kIsZombie;
    }

    MSQLServer(const char *connection, const char *user, const char *password);
    MSQLServer(const char *link);
    MSQLServer(TEnv &env, const char *prefix=0);
    MSQLServer(MSQLServer &serv);
    MSQLServer();
    ~MSQLServer();

    static void PrintLine(const TArrayI &max);
    static void PrintTable(TSQLResult &res);

    const char *GetName() const;
    const char *GetNameDataBase() const { return fDataBase; }

    void Print(Option_t *o) const;
    void Print() const { Print(""); } //*MENU*
    void PrintQuery(const char *query) const; //*MENU*
    void ShowColumns() const; //*MENU*
    void ShowStatus() const; //*MENU*
    void ShowTableIndex() const; //*MENU*
    void ShowTableCreate() const; //*MENU*
    void ShowVariables() const { PrintQuery("SHOW VARIABLES"); } //*MENU*
    void ShowProcesses() const { PrintQuery("SHOW PROCESSLIST"); } //*MENU*

    void Close(Option_t *option="");
    TSQLResult *Query(const char *sql);
    Bool_t      Exec(const char* sql);
    Int_t       SelectDataBase(const char *dbname);
    TSQLResult *GetDataBases(const char *wild = 0);
    TSQLResult *GetTables(const char *wild = 0, const char *dbname = 0);
    TSQLResult *GetColumns(const char *table, const char *wild = 0, const char *dbname = 0);
    Int_t       CreateDataBase(const char *dbname);
    Int_t       DropDataBase(const char *dbname);
    Int_t       Reload();
    Int_t       Shutdown();
    const char *ServerInfo();
    Bool_t      IsConnected() const;

    TString     GetEntry(const char *where, const char *col=0, const char *table=0) const;
    TString     GetPrimaryKeys(const char *table);
    TString     GetJoins(const char *table, const TString text);

    void RecursiveRemove(TObject *obj);

    MTreeSQL   *GetTree(TString table, TString addon="");

    ClassDef(MSQLServer, 0) // An enhancement of TSQLServer
};

class MSQLColumn : public MSQLServer
{
private:
    Bool_t IsFolder() const { return kFALSE; }

public:
    MSQLColumn(TSQLServer *serv, const char *dbname=0, const char *tname=0, const char *col=0)
        : MSQLServer(serv, dbname, tname, col)
    {
    }
    ClassDef(MSQLColumn, 0) // A workarount to let MSQLServer return kFALSE for IsFolder
};

#endif
