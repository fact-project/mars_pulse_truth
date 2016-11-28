#ifndef MARS_MNewImagePar2
#define MARS_MNewImagePar2

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MGeomCam;
class MSignalCam;

class MNewImagePar2 : public MParContainer
{
private:
    Float_t fBorderLinePixel;   //
    Float_t fBorderLineCenter;  //

public:
    MNewImagePar2(const char *name=NULL, const char *title=NULL);

    void Reset();

    Float_t GetBorderLinePixel() const  { return fBorderLinePixel; }
    Float_t GetBorderLineCenter() const { return fBorderLineCenter; }

    void Print(Option_t *opt=NULL) const;
    void Print(const MGeomCam &geom) const;

    void Calc(const MGeomCam &geom, const MSignalCam &evt, Int_t island=-1);

    ClassDef(MNewImagePar2, 1) // Container to hold new image parameters 2
};

#endif
