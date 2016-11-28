#ifndef MARS_MRunIter
#define MARS_MRunIter

#ifndef MARS_MDirIter
#include "MDirIter.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MRunIter : public MDirIter
{
private:
    TString fPath;
    TArrayI fRuns;

    Bool_t fIsRawFile;
    Bool_t fIsStandardFile;

    void AddRunNumber(UInt_t run)
    {
        fRuns.Set(fRuns.GetSize()+1);
        fRuns[fRuns.GetSize()-1] = run;

        SortRuns();

    }

    void SortRuns();

public:
    MRunIter(const char *path=0) : fPath(path), fIsRawFile(0), fIsStandardFile(kFALSE) { }

    void SetRawFile(Bool_t filetype) { fIsRawFile = filetype; }
    void SetStandardFile(Bool_t filetype) { fIsStandardFile = filetype; }

    Int_t AddRun(UInt_t run, const char *path=0);
    Int_t AddRuns(const char* runrange, const char *path=0);
    Int_t AddRuns(UInt_t from, UInt_t to, const char *path=0)
    {
        Int_t n = 0;

        for (UInt_t i=from; i<=to; i++)
            AddRun(i, path);

        return n;
    }

    UInt_t GetNumRuns() const { return fRuns.GetSize(); }
    const TArrayI &GetRuns() const { return fRuns; }

    TString GetRunsAsString() const {
        TString txt;
        for (int i=0; i<fRuns.GetSize(); i++)
        {
            txt += "#";
            txt += fRuns[i];
            if (i!=fRuns.GetSize()-1)
                txt += " ";
        }
        return txt;
    }
    TString GetRunsAsFileName() const {
        TString txt;
        for (int i=0; i<fRuns.GetSize(); i++)
        {
            txt += fRuns[i];
            if (i!=fRuns.GetSize()-1)
                txt += "_";
        }
        return txt;
    }

    ClassDef(MRunIter, 1) // Iterator for runs
};

#endif
