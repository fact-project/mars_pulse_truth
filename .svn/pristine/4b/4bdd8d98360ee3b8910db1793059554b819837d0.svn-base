#ifndef MARS_MGeomCamFACT
#define MARS_MGeomCamFACT

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamFACT : public MGeomCam
{
private:
    enum { kDirCenter, kDirNE, kDirNW, kDirW, kDirSW, kDirSE, kDirE };

    static Double_t CalcXY(Int_t dir, Int_t ring, Int_t i, Double_t &x, Double_t &y);

    void CreateCam();

public:
    MGeomCamFACT(const char *name=NULL);
    MGeomCamFACT(Double_t dist, const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamFACT, 1) // Geometry class for the final FACT camera
};

#endif
