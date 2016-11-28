#ifndef MARS_MRanForestGrow
#define MARS_MRanForestGrow

#ifndef MARS_MRead
#include "MRead.h"
#endif

class MHMatrix;
class MParList;
class MRanForest;

class MRanForestGrow : public MRead
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    //     Int_t fNumTrees;
    //     Int_t fNumTry;
    //     Int_t fNdSize;

    MRanForest *fRanForest;
    MHMatrix   *fMatrix;   //! matrix with events

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    UInt_t  GetEntries()            { return 100; }//fNumTrees; }
    TString GetFileName() const     { return "MRanForestGrow"; }
    TString GetFullFileName() const { return "MRanForestGrow"; }

public:
    MRanForestGrow(const char *name=NULL, const char *title=NULL);

    MRanForest *GetForest() const { return fRanForest; }

    //     void SetNumTrees(Int_t n=-1) { fNumTrees=n>0?n:100; }
    //     void SetNumTry(Int_t   n=-1) { fNumTry  =n>0?n:  3; }
    //     void SetNdSize(Int_t   n=-1) { fNdSize  =n>0?n:  1; }

    ClassDef(MRanForestGrow, 0) // Task to grow a random forest
};

#endif

