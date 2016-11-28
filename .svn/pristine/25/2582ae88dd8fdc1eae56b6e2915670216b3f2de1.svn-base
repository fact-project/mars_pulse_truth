#ifndef MARS_MMcTriggerLvl2Calc
#define MARS_MMcTriggerLvl2Calc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;

class MMcEvt;
class MMcTrig;

class MMcTriggerLvl2;
class MHMcTriggerLvl2;

class MGeomCam;

class MMcTriggerLvl2Calc : public MTask
{
 private:

  MMcEvt  *fMcEvt;
  MMcTrig *fMcTrig;

  MGeomCam *fCam;

  MMcTriggerLvl2 *fMMcTriggerLvl2;
  MHMcTriggerLvl2 *fMHMcTriggerLvl2;

  Int_t PreProcess(MParList *pList);
  Int_t Process();
  Int_t PostProcess();

  Bool_t ReInit(MParList *pList);
  Bool_t IsMCRun(MParList *pList) const;

 public:
    MMcTriggerLvl2Calc(const char *name=NULL, const char *title=NULL);   

    ClassDef(MMcTriggerLvl2Calc, 0) // Task to calculate the MMcTriggerLvl2 Parameters
};

#endif 





