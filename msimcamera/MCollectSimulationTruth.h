#ifndef MARS_MCollectSimulationTruth
#define MARS_MCollectSimulationTruth

#ifndef MARS_MTask
#include "MTask.h"
#endif

#include "MArrayF.h"
#include "MMatrix.h"

class MParList;
class MPhotonEvent;
class MParameterD;
class MPhotonStatistics;
class MRawRunHeader;
class MRawEvtData;
class MAnalogChannels;

class MCollectSimulationTruth : public MTask
{
private:
    MParameterD      *fPulsePos;   //! Intended pulse position
    MParameterD      *fTrigger;    //! Position of trigger in the analog signal
    MPhotonEvent     *fEvt;
    MPhotonStatistics *fStat;
    MRawRunHeader   *fRunHeader;        //! The run header storing infos about the digitization
    MRawEvtData      *fData;       //! Digitized FADC signal
    MAnalogChannels  *fCamera;     //! Analog channes to be read out

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MCollectSimulationTruth(const char *name=NULL, const char *title=NULL);

    ClassDef(MCollectSimulationTruth, 0) // Task to simulate the analog readout (FADCs)
};

#endif
