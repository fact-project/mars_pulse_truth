#ifndef MARS_MHReflector
#define MARS_MHReflector

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TH3
#include <TH3.h>
#endif

class MRflEvtData;
class MRflEvtHeader;

class MHReflector : public MH
{
private:
    MRflEvtData   *fData;   //!
    MRflEvtHeader *fHeader; //!

    Double_t fMm2Deg;

    TH3D fHistXY;
    TH2D fHistRad;
    TH2D fHistSize;

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t weight=1);
    Bool_t Finalize();

public:
    MHReflector(const char *name=0, const char *title=0);

    TH2D *GetHistRad() { return &fHistRad; }
    TH3D *GetHistXY()  { return &fHistXY; }

    void Draw(Option_t *o="");
    void Paint(Option_t *o="");

    ClassDef(MHReflector, 1)
};

#endif
