#ifndef MARS_MExtractFACT
#define MARS_MExtractFACT

#ifndef MARS_MExtractTime
#include "MExtractTime.h"
#endif

class MExtractFACT : public MExtractTime
{
private:
  Int_t  PreProcess(MParList *pList);
  Bool_t ReInit(MParList *pList);
  Int_t  Process();
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
  MExtractFACT(const char *name=NULL, const char *title=NULL);
  
  virtual Bool_t InitArrays(Int_t) { return kTRUE; }

  ClassDef(MExtractFACT, 0)   // Time And Charge Extractor Base Class
};

#endif
