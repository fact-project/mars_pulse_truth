#ifndef MARS_MImagePar
#define MARS_MImagePar

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MHillas;
class MGeomCam;
class MSignalCam;

class MImagePar : public MParContainer
{
private:
    Short_t fNumIslands;     // Number of islands found
    Short_t fNumSinglePixels;

    Float_t fSizeSinglePixels;
    Float_t fSizeSubIslands;
    Float_t fSizeMainIsland;

    Short_t fNumSatPixelsHG; // number of pixels with saturating hi-gains
    Short_t fNumSatPixelsLG; // number of pixels with saturating lo-gains

public:
    MImagePar(const char *name=NULL, const char *title=NULL);

    void Reset();

    Short_t GetNumIslands() const { return fNumIslands; }

    Short_t GetNumSatPixelsHG() const   { return fNumSatPixelsHG; }
    Short_t GetNumSatPixelsLG() const   { return fNumSatPixelsLG; }
    Short_t GetNumSinglePixels() const  { return fNumSinglePixels; }
    Float_t GetSizeSinglePixels() const { return fSizeSinglePixels; }
    Float_t GetSizeSubIslands() const   { return fSizeSubIslands; }
    Float_t GetSizeMainIsland() const   { return fSizeMainIsland; }

    void Print(Option_t *opt=NULL) const;

    void Calc(const MSignalCam &evt);

    ClassDef(MImagePar, 2) // Container to hold (geometry and island independant) image parameters
};

#endif
