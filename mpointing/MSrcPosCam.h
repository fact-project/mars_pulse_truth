#ifndef MARS_MSrcPosCam
#define MARS_MSrcPosCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#include <TMath.h>

class fits;
class TVector2;

class MSrcPosCam : public MParContainer
{
private:
    Float_t fX; // [mm] x position of source in camera
    Float_t fY; // [mm] y position of source in camera

public:
    MSrcPosCam(const char *name=NULL, const char *title=NULL);
    MSrcPosCam(const MSrcPosCam &p);

    void Clear(Option_t *)           { fX = 0; fY = 0; }

    void SetX(Float_t x)             { fX = x; }
    void SetY(Float_t y)             { fY = y; }
    void SetXY(Float_t x, Float_t y) { fX = x; fY = y; }
    void SetXY(const TVector2 &v);

    void Add(const TVector2 &v);

    Float_t GetDist() const;

    Float_t GetX() const             { return fX; }
    Float_t GetY() const             { return fY; }
    TVector2 GetXY() const;

    void Paint(Option_t *opt);
    void Print(Option_t *opt=NULL) const;

    void StreamPrimitive(std::ostream &out) const;

    //void AsciiRead(ifstream &fin);
    //void AsciiWrite(ofstream &fout) const;

    Bool_t SetupFits(fits &fin);

    ClassDef(MSrcPosCam, 1) // container to store source position in the camera plain
};

#endif
