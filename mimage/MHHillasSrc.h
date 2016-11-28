#ifndef MARS_MHHillasSrc
#define MARS_MHHillasSrc

#ifndef MARS_MH
#include "MH.h"
#endif

class TH1F;
class MGeomCam;
class MHillas;

class MHHillasSrc : public MH
{
private:
    MGeomCam *fGeom;  //! conversion mm to deg

    TH1F *fAlpha;     //->
    TH1F *fDist;      //->
    TH1F *fCosDA;     //->

    TH1F *fDCA;       //->
    TH1F *fDCADelta;  //->

public:
    MHHillasSrc(const char *name=NULL, const char *title=NULL);
    ~MHHillasSrc();

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    TH1F *GetHistAlpha()         { return fAlpha; }
    TH1F *GetHistDist()          { return fDist; }
    TH1F *GetHistCosDeltaAlpha() { return fCosDA; }
    TH1F *GetHistDCA()           { return fDCA; }
    TH1F *GetHistDCADelta()      { return fDCADelta; }

    void Draw(Option_t *opt=NULL);
    void Paint(Option_t *opt);


    ClassDef(MHHillasSrc, 2) // Container which holds histograms for the source dependant parameters
};

#endif
