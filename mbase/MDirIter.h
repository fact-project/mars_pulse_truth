#ifndef MARS_MDirIter
#define MARS_MDirIter

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MDirIter : public TObject
{
private:
    TObjArray fList;
    TString   fFilter;

    TIter     fNext;        //!
    void     *fDirPtr;      //!
    TObject  *fCurrentPath; //!

    void   *Open();
    void    Close();
    Bool_t  CheckEntry(const TString n) const;
    Int_t   IsDir(const char *dir) const;
    Bool_t  MatchFilter(const TString &name, TString filter) const;
    TString ConcatFileName(const char *dir, const char *name) const;
    void    PrintEntry(const TObject &o) const;

public:
    MDirIter() : fNext(&fList), fDirPtr(NULL)
    {
        fList.SetOwner();
    }
    MDirIter(const MDirIter &dir) : TObject(), fNext(&fList), fDirPtr(NULL)
    {
        fList.SetOwner();

        TObject *o=NULL;
        TIter NextD(&dir.fList);
        while ((o=NextD()))
            AddDirectory(o->GetName(), o->GetTitle());
    }
    MDirIter(const char *dir, const char *filter="", Int_t rec=0) : fNext(&fList), fDirPtr(NULL)
    {
        fList.SetOwner();
        AddDirectory(dir, filter, rec);
    }
    ~MDirIter()
    {
        Close();
    }

    Int_t ReadList(const char *fname, const TString &path="");
    void Sort();

    Int_t AddDirectory(const char *dir, const char *filter="", Int_t recursive=0);
    Int_t AddFile(const char *name);
    void  Add(const MDirIter &iter);
    void  Reset();

    TString Next(Bool_t nocheck=kFALSE);
    TString operator()(Bool_t nocheck=kFALSE) { return Next(nocheck); }

    void SetFilter(const char *f="") { fFilter = f; }
    UInt_t GetNumEntries() const
    {
        UInt_t n = 0;
        MDirIter NextD(*this);
        while (!NextD().IsNull()) n++;
        return n;
    }

    void Print(const Option_t *o="") const;

    ClassDef(MDirIter, 1) // Iterator for files in several directories (with filters)
};

#endif
