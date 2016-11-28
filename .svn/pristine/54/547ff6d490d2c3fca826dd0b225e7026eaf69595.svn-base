#ifndef MARS_MHMuonPar
#define MARS_MHMuonPar

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef ROOT_TProfile
#include <TProfile.h>
#endif

class MMuonSearchPar;
class MMuonCalibPar;
class MGeomCam;

class MHMuonPar : public MH
{
private:
    static const Float_t  fgIntegralLoLim; // lower limit of integral
    static const Float_t  fgIntegralUpLim; // upper limit of integral

    MMuonSearchPar *fMuonSearchPar; //!
    MMuonCalibPar  *fMuonCalibPar;  //!
    MGeomCam       *fGeom;          //! Conversion mm to deg

    TH1F     fHistRadius;     // Radius
    TH1F     fHistArcWidth;   // ArcWidth

    TProfile fHistBroad;      // ArcWidth/Radius Vs Radius
    TProfile fHistSize;       // MuonSize Vs Radius

    //Double_t Integral(const TProfile &p, Int_t a, Int_t b) const;
    Double_t Integral(const TProfile &p, Float_t a=fgIntegralLoLim, Float_t b=fgIntegralUpLim) const;

public:
    MHMuonPar(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    const TH1F&     GetHistRadius() const    { return fHistRadius; }
    const TH1F&     GetHistArcWidth() const  { return fHistArcWidth; }
    const TProfile& GetHistBroad() const     { return fHistBroad; }
    const TProfile& GetHistSize() const      { return fHistSize; }

    Double_t GetMeanSize() const  { return Integral(fHistSize);     }
    Double_t GetMeanWidth() const { return Integral(fHistBroad);    }
    Stat_t   GetEntries() const   { return fHistBroad.GetEntries(); }

    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    ClassDef(MHMuonPar, 2)
};

#endif


