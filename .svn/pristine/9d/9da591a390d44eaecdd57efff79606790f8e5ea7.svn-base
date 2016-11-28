#ifndef MARS_MSrcPosFromFile
#define MARS_MSrcPosFromFile

#ifndef MARS_MSrcPlace
#include "MSrcPlace.h"
#endif

#ifndef ROOT_TMap
#include <TExMap.h>
#endif

class MRawRunHeader;
class MSrcPosCam;

class MSrcPosFromFile : public MSrcPlace
{
 private:

    MRawRunHeader *fRawRunHeader;

    Int_t  fNumRuns;
    UInt_t  fFirstRun;
    UInt_t  fLastRun;

    Int_t      *fRunList;
    MSrcPosCam *fRunSrcPos;
    MSrcPosCam *fLastValidSrcPosCam;
    TExMap     *fRunMap;   // list of run numbers positions

    TString fSourcePositionFilePath;

    virtual Int_t ReadSourcePositionsFile(UShort_t readmode);
    virtual Int_t ComputeNewSrcPosition();
    
    virtual Int_t PreProcess(MParList *plist);
    
public:    
    enum ReadMode_t {kCount=0,kRead};

    MSrcPosFromFile(TString cardpath=0, OnOffMode_t mode=kOn, const char *name=NULL, const char *title=NULL);
    ~MSrcPosFromFile();

    void SetInputFileName(TString fname) {fSourcePositionFilePath=fname;}

    ClassDef(MSrcPosFromFile, 0) // task to calculate the position of the source as a function of the run number 
};

#endif
