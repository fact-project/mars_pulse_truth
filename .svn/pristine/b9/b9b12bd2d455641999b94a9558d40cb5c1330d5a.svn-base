#ifndef MARS_MRflSinglePhoton
#define MARS_MRflSinglePhoton

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TH1;
class TH2;
class TH3;

class MRflSinglePhoton : public MParContainer
{
private:
    Float_t fX;      // [mm] camera impact point
    Float_t fY;      // [mm] camera impact point
    Float_t fCosU;   // [1] camera direction cosine
    Float_t fCosV;   // [1] camera direction cosine
    Float_t fTime;   // [ns] arrival time
    Float_t fHeight; // [cm] production height
    Float_t fPhi;    // [rad] camera inc. angle (rad)

public:
    MRflSinglePhoton() { }
    MRflSinglePhoton(const MRflSinglePhoton &ph)
        : fX(ph.fX), fY(ph.fY), fCosU(ph.fCosU), fCosV(ph.fCosV),
        fTime(ph.fTime), fHeight(ph.fHeight), fPhi(ph.fPhi)
    {
    }

    void SetXY(Float_t x, Float_t y) { fX = x; fY=y; }
    void SetCosUV(Float_t u, Float_t v) { fCosU = u; fCosV=v; }
    void SetTime(Float_t t) { fTime = t; }
    void SetHeight(Float_t h) { fHeight = h; }
    void SetInclinationAngle(Float_t phi) { fPhi = phi; }

    Float_t GetX() const { return fX; }
    Float_t GetY() const { return fY; }

    void FillRad(TH1 &hist, Float_t scale=1) const;
    void FillRad(TH2 &hist, Double_t x, Float_t scale=1) const;
    void Fill(TH2 &hist, Float_t scale=1) const;
    void Fill(TH3 &hist, Double_t z, Float_t scale=1) const;

    void Print(Option_t *o="") const;

    ClassDef(MRflSinglePhoton, 0) // Single Photon of a event from the reflector program

};

#endif
