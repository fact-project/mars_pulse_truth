#ifndef MARS_MAstroSky2Local
#define MARS_MAstroSky2Local

#ifndef ROOT_TRotation
#include <TRotation.h>
#endif

class MTime;
class MObservatory;

class MAstroSky2Local : public TRotation
{
private:
    void Init(Double_t gmst, const MObservatory &obs);

public:
    MAstroSky2Local(Double_t gmst,  const MObservatory &obs);
    MAstroSky2Local(const MTime &t, const MObservatory &obs);

    Double_t RotationAngle(Double_t ra, Double_t dec, Double_t dzd=0, Double_t daz=0) const;

    ClassDef(MAstroSky2Local, 1) // Rotation Matrix to convert sky coordinates to ideal local coordinates
};

#endif
