#ifndef MARS_MGeomCamFact
#define MARS_MGeomCamFact

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamFact : public MGeomCam
{
private:
    void CreateGeometry(const MGeomCam &hex);

public:
    // 223
    MGeomCamFact(Double_t rad, Double_t diameter=16, Double_t dist=4.92, const char *name=NULL);
    MGeomCamFact(Int_t  rings, Double_t diameter=16, Double_t dist=4.92, const char *name=NULL);
    MGeomCamFact(const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamFact, 1) // Geometry class for the Fact camera
};

#endif
