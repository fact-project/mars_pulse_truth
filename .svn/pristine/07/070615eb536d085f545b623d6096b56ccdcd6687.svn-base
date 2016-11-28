#ifndef MARS_MNewImageParCalc
#define MARS_MNewImageParCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MHillas;
class MNewImagePar;
class MSrcPosCam;
class MGeomCam;
class MCerPhotEvt;

class MNewImageParCalc : public MTask
{
private:
    MGeomCam    *fGeomCam;
    MCerPhotEvt *fCerPhotEvt;

    MHillas      *fHillas;       //! Pointer to the source independent hillas parameters
    MSrcPosCam   *fSrcPos;       //! Pointer to the source position
    MNewImagePar *fNewImagePar;  //! Pointer to the output container for the new image parameters

    TString fSrcName;
    TString fNewParName;
    TString fHillasInput;

    //Int_t       fErrors;

    Int_t PreProcess(MParList *plist);
    Int_t Process();
    //Bool_t PostProcess();

public:
    MNewImageParCalc(const char *src="MSrcPosCam", const char *newpar="MNewImagePar",
                     const char *name=NULL,        const char *title=NULL);

    void SetInput(TString hilname) { fHillasInput = hilname; }

    ClassDef(MNewImageParCalc, 0) // task to calculate new image parameters
};

#endif




