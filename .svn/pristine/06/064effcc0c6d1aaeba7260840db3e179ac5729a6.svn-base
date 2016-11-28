#ifndef MARS_MHStarMap
#define MARS_MHStarMap

#ifndef MARS_MH
#include "MH.h"
#endif

class TH2F;
class MHillas;
class MSrcPosCam;
class MObservatory;
class MPointingPos;

class MHStarMap : public MH
{
private:
    MSrcPosCam   *fSrcPos;      //!
    MPointingPos *fPointPos;    //!
    MObservatory *fObservatory; //!

    TH2F *fStarMap;      //->

    Float_t fMm2Deg;

    Bool_t fUseMmScale;

    Float_t fCosLat;  //!
    Float_t fSinLat;  //!

    void PrepareDrawing() const;

    void Paint(Option_t *opt="");

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    Bool_t ReInit(MParList *pList);
    void GetRotationAngle(Double_t &sin, Double_t &cos);

public:
    MHStarMap(const char *name=NULL, const char *title=NULL);
    ~MHStarMap();

    void SetMmScale(Bool_t mmscale=kTRUE);
    void SetMm2Deg(Float_t mmdeg);

    TH1 *GetHistByName(const TString name) const { return (TH1*)fStarMap; }

    TH2F *GetHist() { return fStarMap; }

    void Draw(Option_t *opt=NULL);
    TObject *DrawClone(Option_t *opt=NULL) const;

    ClassDef(MHStarMap, 1) // Container to hold the Starmap
};

#endif
