#ifndef MARS_MJStar
#define MARS_MJStar

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MJStar : public MJob
{
private:
    Bool_t fMuonAnalysis;

    Bool_t WriteResult();
    Bool_t CheckEnvLocal();

public:
    MJStar(const char *name=NULL, const char *title=NULL);

    // Setup
    void DisableMuonAnalysis(Bool_t b=kTRUE) { fMuonAnalysis = !b; }

    // Process
    Bool_t Process();

    ClassDef(MJStar, 0) // Tool to create a pedestal file (MPedestalCam)
};

#endif
