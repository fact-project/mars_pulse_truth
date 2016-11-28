#ifndef MARS_MRanForestFill
#define MARS_MRanForestFill

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRanTree;
class MRanForest;
class MParList;
class MDataArray;

class MRanForestFill : public MTask
{
private:
    MRanTree *fRanTree;
    MRanForest *fRanForest;
    MDataArray *fData;
    Int_t fNumTrees;
    Int_t fNum;

    Int_t PreProcess(MParList *plist);
    Int_t Process();
    Int_t PostProcess();

public:
    MRanForestFill(const char *name=NULL, const char *title=NULL);
    ~MRanForestFill();

    void SetNumTrees(UShort_t n=100) { fNumTrees = n; }

    ClassDef(MRanForestFill, 0) // Task
};

#endif
