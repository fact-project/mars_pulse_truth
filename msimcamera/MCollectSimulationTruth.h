#ifndef MARS_MCollectSimulationTruth
#define MARS_MCollectSimulationTruth

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MParameterD;
class MRawEvtData;
class MRawRunHeader;
class MRawEvtHeader;
class MAnalogChannels;

class MCollectSimulationTruth : public MTask
{
private:
    MParameterD      *fPulsePos;   //! Intended pulse position
    MParameterD      *fTrigger;    //! Position of trigger in the analog signal
    MPhotonEvent     *fEvt;
    MPhotonStatistics *fStat;

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MCollectSimulationTruth(const char *name=NULL, const char *title=NULL);

    ClassDef(MCollectSimulationTruth, 0) // Task to simulate the analog readout (FADCs)
};

#endif
