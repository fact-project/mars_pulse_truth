#ifndef MARS_MNewImagePar
#define MARS_MNewImagePar

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MHillas;
class MGeomCam;
class MSignalCam;

class MNewImagePar : public MParContainer
{
private:
    // FIXME: MOVE ALL PARAMETRS WHICH DOES'T DEPEND ON THE IMAGE
    //        CLEANING TO A NEW CLASS!
    Float_t fLeakage1;             // (photons in most outer ring of pixels) over fSize
    Float_t fLeakage2;             // (photons in the 2 outer rings of pixels) over fSize

    Float_t fConc;                 // [ratio] concentration ratio: sum of the two highest pixels / fSize
    Float_t fConc1;                // [ratio] concentration ratio: sum of the highest pixel / fSize
    Float_t fConcCOG;              // [ratio] concentration of the three pixels next to COG
    Float_t fConcCore;             // [ratio] concentration of signals inside or touching the ellipse

    Float_t fUsedArea;             // Area of pixels which survived the image cleaning
    Float_t fCoreArea;             // Area of core pixels
    Short_t fNumUsedPixels;        // Number of pixels which survived the image cleaning
    Short_t fNumCorePixels;        // number of core pixels

public:
    MNewImagePar(const char *name=NULL, const char *title=NULL);

    void Reset();

    Float_t GetLeakage1() const { return fLeakage1; }
    Float_t GetLeakage2() const { return fLeakage2; }

    Float_t GetConc() const     { return fConc;     }
    Float_t GetConc1() const    { return fConc1;    }
    Float_t GetConcCOG() const  { return fConcCOG;  }
    Float_t GetConcCore() const { return fConcCore; }

    Short_t GetNumUsedPixels() const { return fNumUsedPixels; }
    Short_t GetNumCorePixels() const { return fNumCorePixels; }

    Float_t GetUsedArea() const { return fUsedArea; }
    Float_t GetCoreArea() const { return fCoreArea; }

    void Print(Option_t *opt=NULL) const;
    void Print(const MGeomCam &geom) const;

    void Calc(const MGeomCam &geom, const MSignalCam &evt,
              const MHillas &hillas, Int_t island=-1);

    ClassDef(MNewImagePar, 6) // Container to hold new image parameters
};

#endif
