#ifndef MARS_MGeomCorsikaCT
#define MARS_MGeomCorsikaCT

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MGeomCorsikaCT : public MParContainer
{
private:

    Float_t fCTx;  // X position of the CT center
    Float_t fCTy;  // Y position of the CT center
    Float_t fCTz;  // Z above high position of the CT center
    Float_t fCTtheta; // 
    Float_t fCTphi;   //
    Float_t fCTdiam;  // Diameter where the Cph are stored at MMCs level
    Float_t fCTfocal; // Focal of the CT

public:

    MGeomCorsikaCT(const char *name=NULL, const char *title=NULL);

    void Print(Option_t *opt=NULL) const;

    void Fill(Float_t ctx, Float_t cty, Float_t ctz,
	      Float_t cttheta, Float_t ctphi,
	      Float_t ctdiam, Float_t ctfocal);

    Float_t GetCTx() { return fCTx; }
    Float_t GetCTy() { return fCTy; }
    Float_t GetCTz() { return fCTz; }

    ClassDef(MGeomCorsikaCT, 1)  // class containing information about CTelescope
};

#endif


