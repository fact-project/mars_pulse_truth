#ifndef MARS_MConcentration
#define MARS_MConcentration

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MHillas;
class MGeomCam;
class MSignalCam;

class MConcentration : public MParContainer
{
private:
    Float_t fConc[9];       // [ratio] Num photons in i+1 pixels over size of event 

public:
    MConcentration(const char *name=NULL, const char *title=NULL);

    void Reset();

    Int_t Calc(const MGeomCam &geom, const MSignalCam &pix, const MHillas &hil);

    void Print(Option_t *opt=NULL) const;

    Float_t GetConc1() const { return fConc[0]; }
    Float_t GetConc2() const { return fConc[1]; }
    Float_t GetConc3() const { return fConc[2]; }
    Float_t GetConc4() const { return fConc[3]; }
    Float_t GetConc5() const { return fConc[4]; }
    Float_t GetConc6() const { return fConc[5]; }
    Float_t GetConc7() const { return fConc[6]; }
    Float_t GetConc8() const { return fConc[7]; }
    Float_t GetConc9() const { return fConc[8]; }

    ClassDef(MConcentration, 1) // Storage Container for Concentration Parameter
};

#endif
