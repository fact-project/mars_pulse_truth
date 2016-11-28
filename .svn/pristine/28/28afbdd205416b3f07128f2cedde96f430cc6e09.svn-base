#ifndef MARS_MSrcPosFromModel
#define MARS_MSrcPosFromModel

//#ifndef ROOT_TVector2
//#include <TVector2.h>
//#endif

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MPointingPos;
class MSrcPosCam;
class MGeomCam;
class MPointing;
class MRawRunHeader;
class MTime;
class MObservatory;

class MSrcPosFromModel : public MTask
{
private:
    MPointingPos  *fPointPos;   //! Poiting position as recorded by the drive system
    MSrcPosCam    *fSrcPos;     //! Source position in the camera
    MGeomCam      *fGeom;       //! Camera geometry
    MRawRunHeader *fRun;        //! Run Header storing the run-number
    MTime         *fTime;
    MObservatory  *fObservatory;

    MPointing    *fPoint0401;   //! Pointing Model used since 1/2004
    MPointing    *fPoint0405;   //! Pointing Model used since 5/2004

    Int_t PreProcess(MParList *pList);
    Int_t Process();
/*
    TVector2 CalcXYinCamera(const ZdAz &pos0, const ZdAz &pos) const;
    TVector2 CalcXYinCamera(const MVector3 &pos0, const MVector3 &pos) const;
    */

public:
    MSrcPosFromModel(const char *name=NULL, const char *title=NULL);
    ~MSrcPosFromModel();

    ClassDef(MSrcPosFromModel, 0) // Calculates the source position from a corrected pointing model
};

#endif
