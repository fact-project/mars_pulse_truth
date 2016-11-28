#ifndef MARS_MGeomCamMagicXT
#define MARS_MGeomCamMagicXT

#ifndef MARS_MGeomCamDwarf
#include "MGeomCamDwarf.h"
#endif

class MGeomCamMagicXT : public MGeomCamDwarf
{
public:
    MGeomCamMagicXT(const char *name=NULL);

    ClassDef(MGeomCamMagicXT, 1) // Geometry class for the 'extended' Magic camera
};

#endif

