#ifndef MARS_MGeomCamMagic
#define MARS_MGeomCamMagic

#ifndef MARS_MGeomCamDwarf
#include "MGeomCamDwarf.h"
#endif

class MGeomCamMagic : public MGeomCam
{
private:
    void CreateCam();
    void CreateNN();

public:
    MGeomCamMagic(const char *name=NULL);
    MGeomCamMagic(Double_t dist, const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamMagic, 1)		// Geometry class for the Magic camera
};

class MGeomCamMagicII : public MGeomCamDwarf
{
private:
    void Rotate();

public:
    MGeomCamMagicII(const char *name=NULL);
    MGeomCamMagicII(Double_t dist, const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamMagicII, 1)		// Geometry class for the Magic camera
};

#endif

