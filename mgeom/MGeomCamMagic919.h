#ifndef MARS_MGeomCamMagic919
#define MARS_MGeomCamMagic919

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamMagic919 : public MGeomCam
{
private:
    void CreateCam();
    void CreateNN();

public:
    MGeomCamMagic919(const char *name=NULL);

    ClassDef(MGeomCamMagic919, 1)		// Geometry class for Magic camera with only small pixels
};

#endif

