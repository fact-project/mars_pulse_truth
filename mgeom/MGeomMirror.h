#ifndef MARS_MGeomMirror
#define MARS_MGeomMirror

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class TRotation;
class MGeomMirror : public MParContainer
{
private:
    Int_t   fMirrorId;     // the Mirror Id

    Float_t fFocalDist;    // [cm] focal distance of that mirror
    Float_t fSX;           // [cm] curvilinear coordinate of mirror's center in X
    Float_t fSY;           // [cm] curvilinear coordinate of mirror's center in Y
    Float_t fX;            // [cm] x coordinate of the center of the mirror
    Float_t fY;            // [cm] y coordinate of the center of the mirror
    Float_t fZ;            // [cm] z coordinate of the center of the mirror
    Float_t fThetaN;       // polar theta angle of the direction where the mirror points to
    Float_t fPhiN;         // polar phi angle of the direction where the mirror points to
    Float_t fXN;           // xn coordinate of the normal vector in the center
    Float_t fYN;           // yn coordinate of the normal vector in the center
    Float_t fZN;           // zn coordinate of the normal vector in the center
                           // Note: fXN^2*fYN^2*fZN^2 = 1
    Float_t fDeviationX;   // [cm] deviation in x of the spot of a single mirror on the camera plane
    Float_t fDeviationY;   // [cm] deviation in y of the spot of a single mirror on the camera plane

    TArrayF fWavelength;   // List of wavelength
    TArrayF fReflectivity; // Mirror reflectivity

    TRotation *fReflector; //! Store this for acceleration

public:
    MGeomMirror(Int_t mir=-1, const char *name=NULL, const char *title=NULL);

    Int_t GetMirrorId() const         { return fMirrorId;   }

    void  SetMirrorContent(Int_t mir, Float_t focal, Float_t curv_x,
                           Float_t curv_y, Float_t lin_x, Float_t lin_y,
                           Float_t lin_z, Float_t theta, Float_t phi,
                           Float_t x_n, Float_t y_n, Float_t z_n);
    void  SetMirrorDeviations(Float_t dev_x, Float_t dev_y);

    void  SetReflectivity(const TArrayF &wav, const TArrayF &ref);

    void  SetArraySize(Int_t dim) { fWavelength.Set(dim); fReflectivity.Set(dim); }

    TVector3 GetMirrorCenter() const { return TVector3(fX/100, fY/100, fZ/100); }
    TVector3 GetMirrorNorm()   const { return TVector3(fXN, fYN, fZN); }

    TVector3 GetReflection(const TVector3 &star, Double_t dist=-1);

    ClassDef(MGeomMirror, 2)  // geometry class describing one mirror
};

#endif


