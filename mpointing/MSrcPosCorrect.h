#ifndef MARS_MSrcPosCorrect
#define MARS_MSrcPosCorrect

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class TVector2;
class MSrcPosCam;
class MReportStarguider;

class MSrcPosCorrect : public MTask
{
private:
    MSrcPosCam *fSrcPosCam;   //! Source position
    MSrcPosCam *fSrcPosAnti;  //! Anti source position
    MSrcPosCam *fAxis;        //! New "center of camera"

    MGeomCam   *fGeom;        //! Camera geometry

    UShort_t fRunType;        //! Run Type to decide where to get pointing position from
    UInt_t   fRunNumber;      //! Current run number

    Float_t fDx;              // [mm] Correction in x
    Float_t fDy;              // [mm] Correction in y

    // MTask
    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MSrcPosCorrect(const char *name=NULL, const char *title=NULL);

    ClassDef(MSrcPosCorrect, 0) // Corrects MSrcPosCam for missfocussing
};

#endif
