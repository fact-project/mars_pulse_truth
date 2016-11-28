#ifndef MARS_MHillas
#define MARS_MHillas

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TArrayF;
class TVector2;

class MGeomCam;
class MSignalCam;

class MHillas : public MParContainer
{
private:
    // for description see MHillas.cc
    Float_t fLength;        // [mm]        major axis of ellipse
    Float_t fWidth;         // [mm]        minor axis of ellipse
    Float_t fDelta;         // [rad]       angle of major axis with x-axis
    Float_t fSize;          // [#CerPhot]  sum of content of all pixels (number of Cherenkov photons)
    Float_t fMeanX;         // [mm]        x-coordinate of center of ellipse
    Float_t fMeanY;         // [mm]        y-coordinate of center of ellipse

    Float_t fSinDelta;      // [1] sin of Delta (to be used in derived classes)
    Float_t fCosDelta;      // [1] cos of Delta (to be used in derived classes)

public:
    MHillas(const char *name=NULL, const char *title=NULL);

    void Reset();

    Int_t Calc(const MGeomCam &geom, const MSignalCam &pix, Int_t island=-1);

    void Print(const MGeomCam &geom) const;
    void Print(Option_t *opt=NULL) const;
    void Paint(Option_t *opt=NULL);

    Float_t  GetLength() const { return fLength; }
    Float_t  GetWidth() const  { return fWidth; }
    Float_t  GetDelta() const  { return fDelta; }
    Float_t  GetSize() const   { return fSize; }
    Float_t  GetMeanX() const  { return fMeanX; }
    Float_t  GetMeanY() const  { return fMeanY; }
    Double_t GetDist0() const;

    TVector2 GetMean() const;
    TVector2 GetNormAxis() const;

    Double_t GetBorderLine() const;
    Double_t GetArea() const;

    Float_t GetCosDelta() const { return fCosDelta; }
    Float_t GetSinDelta() const { return fSinDelta; }

    void Set(const TArrayF &arr);

    ClassDef(MHillas, 3) // Storage Container for Hillas Parameter
};

#endif
