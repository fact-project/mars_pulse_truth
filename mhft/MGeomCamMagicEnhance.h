#ifndef MARS_MGeomCamMagicEnhance
#define MARS_MGeomCamMagicEnhance

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MArrayD;
class MSignalCam;

class MGeomCamMagicEnhance : public MTask
{
private:
    static const char *fgNameSignalCamIn;
    static const char *fgNameSignalCamOut;

    TString fNameSignalCamIn;
    TString fNameSignalCamOut;

    MSignalCam *fEvtIn;
    MSignalCam *fEvtOut;

    MArrayD Convert() const;

    Int_t PreProcess(MParList *p);
    Int_t Process();

    void CleanFreqSpace(MArrayD &re, MArrayD &im);

public:
    MGeomCamMagicEnhance(const char *name=0, const char *title=0);

    void SetNameSignalCamIn(const char *n) { fNameSignalCamIn = n; }
    void SetNameSignalCamOut(const char *n) { fNameSignalCamOut = n; }

    ClassDef(MGeomCamMagicEnhance, 0) //Task to convert MSignalCam from MGeomCamMagic to MGeomCamMagicXT
};
    
#endif
