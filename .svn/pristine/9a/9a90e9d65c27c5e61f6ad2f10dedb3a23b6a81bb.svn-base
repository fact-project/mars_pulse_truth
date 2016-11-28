#ifndef MARS_MSrcPosRndm
#define MARS_MSrcPosRndm

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MSrcPosCam;
class MHSrcPosCam;

class MSrcPosRndm : public MTask
{
private:
    MSrcPosCam   *fSrcPos;     //!
    MSrcPosCam   *fSrcPosAnti; //!
    MHSrcPosCam  *fHist;       //!
    MGeomCam     *fGeom;       //! Conversion factor from mm to deg

    Double_t fDistOfSource; // [deg] Distance of the source from the camera center

    Int_t  PreProcess(MParList *plist);
    Int_t  Process();

public:
    MSrcPosRndm() : fSrcPos(0), fSrcPosAnti(0), fHist(0), fGeom(0), fDistOfSource(-1)
    {
        fName  = "MSrcPosRndm";
        fTitle = "Overwrite the source position with a random one from MHSrcPosCam";
    }

    void SetDistOfSource(Double_t dist=-1) { fDistOfSource=dist; }

    ClassDef(MSrcPosRndm, 0) //Overwrite the source position with a random one from MHSrcPosCam
};

#endif
