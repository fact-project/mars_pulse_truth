#ifndef MARS_MHFlux
#define MARS_MHFlux

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

class TH2D;

class MHGamma;
class MHEffOnTime;
class MHThetabarTheta;

// base class MH is used because it defines "MakeDefCanvas"
// if base class MH is used one has to define the member function Fill
//                          because in MH Fill is set to zero
class MHFlux : public MH 
{
private:

    TString fVarname;
    TString fUnit;

    TH2D fHOrig;       // original distribution of E-est 
    TH2D fHUnfold;     // unfolded distribution of E-unfold
    TH2D fHFlux;       // absolute differential photon flux versus E-unfold
    // all these plots for different bins of the variable (Theta or time)

    void CalcEffCol(const TAxis &axex, TH2D &aeff, Int_t n, Double_t aeffbar[], Double_t daeffbar[]);
    Double_t ParabInterpolCos(const TAxis &axe, const TH2D *aeff, Int_t j, Int_t k3, Double_t val) const;
    void FindBins(const TAxis &axe, const Double_t bar, Int_t &k3, Int_t &k0) const;
    void CalcAbsGammaFlux(const TH1D &teff, const TH2D &fHAeff);
    Double_t ParabInterpolLog(const TAxis &axe, Int_t j,
                              Double_t y[], Double_t Ebar) const;

public:
    MHFlux(const TH2D &h2d, const TString varname, const TString unit);
    MHFlux(const MHGamma &h2d, const TString varname, const TString unit);

    void Unfold();
    void CalcFlux(const TH1D *teff, const TProfile *thetabar,
                  const TH2D *aeff);

    void CalcFlux(const MHEffOnTime &teff,
                  const MHThetabarTheta &thetabar,
                  const TH2D *aeff);

    void Draw(Option_t *option="");

    void DrawFluxProjectionX(Option_t *opt="") const;
    void DrawOrigProjectionX(Option_t *opt="") const;

    const TH2D *GetHOrig()       { return &fHOrig; }
    const TH2D *GetHUnfold()     { return &fHUnfold; }
    const TH2D *GetHFlux()       { return &fHFlux; }

    static Bool_t Parab(double x1, double x2, double x3,
                        double y1, double y2, double y3,
                        double *a, double *b, double *c);

    static Double_t Parab(double x1, double x2, double x3,
                          double y1, double y2, double y3,
                          double val);

    ClassDef(MHFlux, 0) //2D-plots (original, unfolded, flux)
};

#endif












