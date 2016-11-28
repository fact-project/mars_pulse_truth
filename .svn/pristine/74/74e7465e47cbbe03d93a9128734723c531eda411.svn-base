#ifndef MARS_MAddGainFluctuation
#define MARS_MAddGainFluctuation

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MCerPhotCalc                                                            //
//                                                                         //
// Integrates the desired ADC time slices of one pixel and substracts the  //
// pedestal (offset) value                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef ROOT_TH1F
#include <TH1F.h>
#endif

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MCerPhotEvt;
class MGainFluctuationCam;
class MGeomCam;
class TH1F;

class MAddGainFluctuation : public MTask
{
    const MGeomCam     *fGeomCam;
    MCerPhotEvt    *fCerPhotEvt; // Cerenkov Photon Event used for calculation
    MGainFluctuationCam    *fGainFlucCam; // Gain Fluctuation
    TH1F *fGainsDistribution;   // Distribution of Gain fluctuatins in the Camera
    Int_t fAddFluctuations;     // If 0 skip Process

    void Fill();

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    Bool_t ReInit(MParList *pList);

public:
    MAddGainFluctuation(const char *name=NULL, const char *title=NULL);

    void SetHist(int bins, float fb, float lb, float *values);
    void FillHist(int flag, float sigma);

    ClassDef(MAddGainFluctuation, 0)   // Task to add Gain fluctuations
};
 

#endif
