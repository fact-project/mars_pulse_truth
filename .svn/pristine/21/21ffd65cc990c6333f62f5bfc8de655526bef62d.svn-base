#ifndef MARS_MRawFileWrite
#define MARS_MRawFileWrite

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TFile;
class TTree;

class MTime;
class MParList;
class MRawRunHeader;
class MRawEvtHeader;
class MRawCrateArray;
class MRawEvtData;

class MRawFileWrite : public MTask
{
private:
    MTime          *fTime;
    MRawRunHeader  *fRawRunHeader;
    MRawEvtHeader  *fRawEvtHeader;
    MRawEvtData    *fRawEvtData1;
    MRawEvtData    *fRawEvtData2;
    MRawCrateArray *fRawCrateArray;

    TTree *fTData;                  //!
    TTree *fTPedestal;              //!
    TTree *fTCalibration;           //!

    TTree *fTRunHeader;             //!

    TFile *fOut;                    //!

    Bool_t fSplit;

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MRawFileWrite(const char *fname, const Option_t *opt="RECREATE",
                  const char *ftitle="Untitled", const Int_t comp=2,
                  const char *name=NULL, const char *title=NULL);
    ~MRawFileWrite();

    void EnableSplit(Bool_t b=kTRUE) { fSplit=b; }

    ClassDef(MRawFileWrite, 0)	// Task to write the raw data containers to a root file
};

#endif
