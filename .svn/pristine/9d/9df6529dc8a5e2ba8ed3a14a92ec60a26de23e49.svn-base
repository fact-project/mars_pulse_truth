#ifndef MARS_MSimMMCS
#define MARS_MSimMMCS

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MMcEvt;
class MMcEvtBasic;
class MPointingPos;
class MCorsikaEvtHeader;
class MCorsikaRunHeader;
class MMcRunHeader;

class MSimMMCS : public MTask
{
private:
    MMcEvtBasic       *fMcEvtBasic;   //!
    MMcEvt            *fMcEvt;        //!
    MPointingPos      *fPointingTel;  //! telescope poiting position in local (telescope) coordinate system
    MCorsikaEvtHeader *fEvtHeader;    //!
    MCorsikaRunHeader *fRunHeader;    //!
    MMcRunHeader      *fMcRunHeader;  //!

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimMMCS(const char* name=0, const char *title=0);

    ClassDef(MSimMMCS, 0) // Task to copy the Mars CheObs MC headers to the old Mars style
};

#endif
