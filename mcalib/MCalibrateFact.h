#ifndef MARS_MCalibrateFact
#define MARS_MCalibrateFact

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class MSignalCam;
class MBadPixelsCam;
class MExtractedSignalCam;

class MCalibrateFact : public MTask
{
private:
    MExtractedSignalCam *fIn;
    MSignalCam          *fOut;

    MBadPixelsCam       *fBadPixels;

    TString fNameExtractedSignalCam;
    TString fNameSignalCam;

    MArrayD fCalibConst;

    Double_t fScale;

    // MTask
    Int_t  PreProcess(MParList *list);
    Bool_t ReInit(MParList *list);
    Int_t  Process();
    //Int_t  PostProcess();

public:
    MCalibrateFact(const char *name=NULL, const char *title=NULL);
    ~MCalibrateFact();

    // MCalibrateFact
    void SetCalibConst(const MArrayD &arr) { fCalibConst=arr; }
    void SetScale(const Double_t &val) { fScale=val; }

    ClassDef(MCalibrateFact, 0) //
};

#endif
