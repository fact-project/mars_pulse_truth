#ifndef MARS_MGeomCamECO1000
#define MARS_MGeomCamECO1000

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamECO1000 : public MGeomCam
{
private:
    void CreateCam();
    void CreateNN();

public:
    MGeomCamECO1000(const char *name=NULL);

    ClassDef(MGeomCamECO1000, 1) // Geometry class for the ECO1000 camera
};

#endif

