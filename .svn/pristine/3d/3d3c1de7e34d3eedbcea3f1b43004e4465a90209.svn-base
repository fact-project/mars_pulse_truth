#ifndef MARS_MSimBundlePhotons
#define MARS_MSimBundlePhotons

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MLut
#include "MLut.h"
#endif

class MParList;
class MPhotonEvent;
class MPhotonStatistics;

class MSimBundlePhotons: public MTask
{
private:
    MPhotonEvent      *fEvt;     //! Event storing the photons
    MPhotonStatistics *fStat;    //! Event statistics needed for crosschecks

    TString fFileName;           // File to from which to read the lut
    MLut    fLut;                // Look-up table

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MSimBundlePhotons(const char *name=NULL, const char *title=NULL);

    void SetFileName(const char *name) { fFileName = name; }

    ClassDef(MSimBundlePhotons, 0) // Task to bundle (re-index) photons according to a look-up table
};

#endif
