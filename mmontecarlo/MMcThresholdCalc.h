#ifndef MARS_MMcThresholdCalc
#define MARS_MMcThresholdCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MMcThresholdCalc                                                        //
//                                                                         //
// Compute the energy threshold from Monte Carlo data                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MMcEvt;
class MMcTrig;
class MHMcEnergy;

class MMcThresholdCalc : public MTask
{
private:
    UInt_t fFirst;
    UInt_t fLast;

    UInt_t fNum;

    const MMcEvt  *fMcEvt;       // Container with Monte Carlo information

    TObjArray    *fMcTrig;       // Container with Monte Carlo trigger information
    TObjArray    *fEnergy;       // Container where we save the energy (threshold)

    const static Float_t fSqrt2; // sqrt(2)

    MMcTrig    *GetTrig   (UInt_t i) { return (MMcTrig*)   (*fMcTrig)[i]; }
    MHMcEnergy *GetHEnergy(UInt_t i) { return (MHMcEnergy*)(*fEnergy)[i]; }

    Int_t PreProcess(MParList* pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MMcThresholdCalc(const Int_t dim = 0,
                     const char* name = NULL, const char* title = NULL);
    ~MMcThresholdCalc();

    ClassDef(MMcThresholdCalc, 0) // Task to compute the energy threshold
};

#endif



