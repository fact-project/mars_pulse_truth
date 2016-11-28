#ifndef MARS_MGeomCamDwarf
#define MARS_MGeomCamDwarf

#ifndef MARS_MGeomCam
#include "MGeomCam.h"
#endif

class MGeomCamDwarf : public MGeomCam
{
private:
    static Int_t CalcNumPix(Double_t rad);
    static Int_t CalcNumPix(Int_t rings);

    void CreateCam(Double_t diameter, Double_t rad);
    void CreateCam(Double_t diameter, Int_t rings);

public:
    // 223
    MGeomCamDwarf(Double_t rad, Double_t diameter=16, Double_t dist=4.92, const char *name=NULL);
    MGeomCamDwarf(Int_t  rings, Double_t diameter=16, Double_t dist=4.92, const char *name=NULL);
    MGeomCamDwarf(const char *name=NULL);

    Bool_t HitFrame(MQuaternion p, const MQuaternion &u) const;

    ClassDef(MGeomCamDwarf, 1) // Geometry class for the Dwarf camera
};

#endif
