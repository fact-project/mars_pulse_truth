#ifndef MARS_MGeomCamSquare
#define MARS_MGeomCamSquare

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamSquare : public MGeomCam
{
private:
    void CreateCam(Short_t x, Short_t y, Double_t diameter);

public:
    MGeomCamSquare(Short_t x, Short_t y, Double_t diameter=7, Double_t dist=4.92, const char *name=NULL);
    MGeomCamSquare(const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamSquare, 1)	// Geometry class for a square pixel camera
};

#endif

