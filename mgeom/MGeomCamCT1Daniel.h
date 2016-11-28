#ifndef MARS_MGeomCamCT1Daniel
#define MARS_MGeomCamCT1Daniel

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamCT1Daniel : public MGeomCam
{
private:

    void CreateCam();
    void CreateNN();

public:

    MGeomCamCT1Daniel(const char *name=NULL);

    ClassDef(MGeomCamCT1Daniel, 1)  // Geometry class for the CT1 camera (daniel's version)
};

#endif

