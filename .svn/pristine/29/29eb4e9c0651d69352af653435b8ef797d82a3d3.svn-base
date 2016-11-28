#ifndef MARS_MMcTriggerRateCalc
#define MARS_MMcTriggerRateCalc

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif
#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MMcEvt;
class MMcRunHeader;
class MMcCorsikaRunHeader;
class MMcTrig;
class MHMcRate;
class TH1F;

class MMcTriggerRateCalc : public MTask
{
private:
    MMcEvt    *fMcEvt;     //!

    TObjArray *fMcRate;
    TObjArray *fMcTrig;

    MMcRunHeader *fMcRunHeader; 
    MMcCorsikaRunHeader *fMcCorRunHeader; 

    UInt_t     fNum;       // decoded dimension
    UInt_t     fFirst;
    UInt_t     fLast;

    Float_t   *fTrigNSB;   // Number of triggers due to NSB alone
    Float_t    fSimNSB;    // Number of simulated NSB-only events

    Float_t   *fTrigger;   // Number of triggered showers
    Float_t    fShowers;   // Number of simulated showers
    Float_t    fAnalShow;  // Number of analysed showers

    Int_t      fPartId;    // Incident particle that generates showers

    TH1F      *fHist[5];

    void Init(int dim, float *trigbg,
              float simbg, const char *name, const char *title);

    MHMcRate *GetRate(UInt_t i) const { return (MHMcRate*)((*fMcRate)[i]); }
    MMcTrig  *GetTrig(UInt_t i) const { return (MMcTrig*)((*fMcTrig)[i]); }

    Bool_t ReInit(MParList *plist);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MMcTriggerRateCalc(int dim=0, float *trigbg=NULL, float simbg=100000,
                       const char *name=NULL, const char *title=NULL);

    MMcTriggerRateCalc(float rate, int dim, float *trigbg, float simbg,
                       const char *name=NULL, const char *title=NULL);

    ~MMcTriggerRateCalc();

    TH1F* GetHist(Int_t i) {return fHist[i];}

    void Draw(const Option_t *o="");

    ClassDef(MMcTriggerRateCalc, 0)	// Task to compute the trigger rate
};

#endif 
