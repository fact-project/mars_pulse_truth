#ifndef MARS_MStereoPar
#define MARS_MStereoPar

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

class MHillas;
class MGeomCam;
class MPointingPos;

class MStereoPar : public MParContainer
{
private:

    Float_t fCoreX;   // Estimated core position on ground x
    Float_t fCoreY;   // Estimated core position on ground y

    Float_t fCoreX2;  // Estimated core position on ground assuming that
    Float_t fCoreY2;  // the source direction is paralel to the tel. axis.

    Float_t fSourceX; // Estimated source position on the camera
    Float_t fSourceY; // Units are degrees! 

    Float_t fTheta2;  // deg^2; Squared angular distance of estimated source position to cameracenter.

    Float_t fCT1Impact; // Estimated shower impact parameter from CT1
    Float_t fCT2Impact; // Estimated shower impact parameter from CT2

    Float_t fCT1Impact2; // Estimated shower impact parameter from CT1
                         // assuming that the source direction is paralel 
                         // to the telescope axis.

    Float_t fCT2Impact2; // Estimated shower impact parameter from CT2
                         // assuming that the source direction is paralel 
                         // to the telescope axis.

    TVector3 CamToDir(const MGeomCam &geom, const MPointingPos &pos, Float_t x, Float_t y) const;
    TVector3 CamToDir(const MGeomCam &geom, const MPointingPos &pos, const TVector2 &p) const;

    void     CalcCT(const MHillas &h, const MPointingPos &p, const MGeomCam &g, TVector2 &cv1, TVector2 &cv2) const;

    TVector2 VersorToCore(const TVector2 &v1, const TVector2 &v2, const TVector2 &p1, const TVector2 &p2) const;

    Double_t CalcImpact(const TVector2 &w, const TVector2 &v, const TVector2 &p) const;
    Double_t CalcImpact(const TVector3 &v, const TVector2 &p) const;
    Double_t CalcImpact(const TVector2 &core, const TVector2 &p, const MPointingPos &point) const;

public:
    MStereoPar(const char *name=NULL, const char *title=NULL);

    void Reset();

    Float_t GetCoreX() const { return fCoreX; }
    Float_t GetCoreY() const { return fCoreY; }
    Float_t GetSourceX() const { return fSourceX; }
    Float_t GetSourceY() const { return fSourceY; }
    Float_t GetTheta2() const { return fTheta2; }
    Float_t GetCT1Impact() const { return fCT1Impact; }
    Float_t GetCT2Impact() const { return fCT2Impact; }
    Float_t GetCT1Impact2() const { return fCT1Impact2; }
    Float_t GetCT2Impact2() const { return fCT2Impact2; }

    void Calc(const MHillas &h1, const MPointingPos &p1, const MGeomCam &g1, const Float_t ct1_x, const Float_t ct1_y,
              const MHillas &h2, const MPointingPos &p2, const MGeomCam &g2, const Float_t ct2_x, const Float_t ct2_y);

    ClassDef(MStereoPar, 1) // Container to hold new image parameters
};

#endif
