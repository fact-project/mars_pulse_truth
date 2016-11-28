#ifndef MARS_MPointingDev
#define MARS_MPointingDev

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

class MPointingDev : public MParContainer
{
private:
    Double_t fDevZd;  // [deg] Pointing offset zenith distance as calculated from starguider data
    Double_t fDevAz;  // [deg] Pointing offset azimuth as calculated from starguider dat

    Double_t fDevX;   // [deg] Pointing offset in x
    Double_t fDevY;   // [deg] Pointing offset in y

public:
    MPointingDev(const char *name=0, const char *title=0) : fDevZd(0), fDevAz(0), fDevX(0), fDevY(0)
    {
        fName  = name ? name   : "MPointingDev";
        fTitle = title ? title : "Container storing the telescope mispointing";
    }

    void SetDevZdAz(Double_t devzd, Double_t devaz) { fDevZd=devzd; fDevAz=devaz; }
    void SetDevXY(Double_t dx, Double_t dy) { fDevX=dx; fDevY=dy; }
    void SetDevXY(const TVector2 &d) { fDevX=d.X(); fDevY=d.Y(); }

    Double_t GetDevZd() const  { return fDevZd; }
    Double_t GetDevAz() const  { return fDevAz; }

    Double_t GetDevX() const  { return fDevX; }
    Double_t GetDevY() const  { return fDevY; }

    TVector2 GetDevXY() const { return TVector2(fDevX, fDevY); }

    Double_t GetDevZdRad() const;//  { return fDevZd*TMath::DegToRad(); }
    Double_t GetDevAzRad() const;//  { return fDevAz*TMath::DegToRad(); }

    ClassDef(MPointingDev, 1) //Container storing the telescope mispointing
};

#endif
