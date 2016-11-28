#ifndef MARS_MGeomCamCT1
#define MARS_MGeomCamCT1

#ifndef MARS_MGeomCamDwarf
#include "MGeomCamDwarf.h"
#endif

class MGeomCamCT1 : public MGeomCamDwarf
{
public:
    MGeomCamCT1(const char *name=NULL) : MGeomCamDwarf(6, 21, 4.88, name)
    {
        SetTitle("Geometry information of HEGRA CT1 Camera");
    }

    ClassDef(MGeomCamCT1, 2) // Geometry class for the CT1 camera
};

#endif

