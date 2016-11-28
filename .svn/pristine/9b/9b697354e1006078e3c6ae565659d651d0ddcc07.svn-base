#ifndef MARS_MSimReadout
#define MARS_MSimReadout

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MParameterD;
class MRawEvtData;
class MRawRunHeader;
class MRawEvtHeader;
class MAnalogChannels;

class MSimReadout : public MTask
{
private:
    MRawRunHeader    *fRunHeader;  //! Digitization window and frequency
    MRawEvtHeader    *fEvtHeader;  //! Event header which is filled with the event number
    MAnalogChannels  *fCamera;     //! Analog channes to be read out
    MParameterD      *fPulsePos;   //! Intended pulse position
    MParameterD      *fTrigger;    //! Position of trigger in the analog signal

    MRawEvtData      *fData;       //! Digitized FADC signal

    Double_t fConversionFactor;    // Conversion factor (arbitrary) from analog signal to FADC counts

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Bool_t ReInit(MParList *pList);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

public:
    MSimReadout(const char *name=NULL, const char *title=NULL);

    ClassDef(MSimReadout, 0) // Task to simulate the analog readout (FADCs)
};

#endif
