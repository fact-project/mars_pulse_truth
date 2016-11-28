#ifndef MARS_MHFindSignificance
#define MARS_MHFindSignificance

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class TF1;
class TH1;
class TCanvas;

class MHFindSignificance : public MH
{
private:

    TH1  *fHistOrig;  // original plot of |alpha| (0.0 to 90.0 degrees)
    TH1  *fHist;      // copy of fHistOrig or rebinned histogram

    TH1D    *fSigVsAlpha;

    Double_t fAlphamin;  // requested lower limit of fit range
    Double_t fAlphammm;  // center of fit range
    Double_t fAlphamax;  // requested lower limit of fit range

    Double_t fAlphami;  // actual lower limit of fit range 
    Double_t fAlphamm;  // actual center of fit range 
    Double_t fAlphama;  // actual upper limit of fit range

    Double_t fAlphasig; // requested signal range
    Double_t fAlphasi;  // actual signal range

    Double_t fAlphalow; // requested lower edge of signal range
    Double_t fAlphalo;  // actual lower edge of signal range

    Double_t fAlphahig; // requested upper edge of background range
    Double_t fAlphahi;  // actual upper edge of background range
 
    // number of events in signal region
    Double_t fNon;     // total number of events in signal region
    Double_t fNbg;     // number of background events in signal region
    Double_t fNex;     // number of excess events in signal region

    Double_t fdNon;
    Double_t fdNbg;
    Double_t fdNex;

    // number of events in background region
    Double_t fNbgtot;  // total number of events in background region
    Double_t fNbgtotFitted;  // fitted total no. of events in background region
    Double_t fdNbgtotFitted; // fitted error of this number

    // effective number of background events
    Double_t fNoff;
    Double_t fGamma;   // Nbg = Non - gamma * Noff

    Double_t fSigLiMa; // significance of gamma signal according to Li & Ma

    const static Double_t fEps = 1.e-4;  // tolerance for floating point comparisons

    Bool_t fDraw;          // if true : draw plots
    Bool_t fFitGauss;      // if true : do the (polynomial+Gauss fit)
    Bool_t fRebin;         // if true : allow rebinning of the alpha plot    
    Bool_t fReduceDegree;  // if true : allow reducing of the order of the polynomial

    Bool_t fConstantBackg; // if set true if background fit is not possible
                           // due to low statistics

    TCanvas  *fCanvas;

    Double_t fNexGauss;    // total number of excess events 
                           // (from fitted Gauss function)
    Double_t fdNexGauss;   // error of the total number of excess events
 
    Double_t fSigmaGauss;  // sigma of fitted Gauss function
    Double_t fdSigmaGauss; // error of this sigma

    //--------------------
    TF1      *fPoly;   // polynomial function
    Int_t    fFitBad;  // if != 0 fit failed
    Int_t    fDegree;  // degree of polynomial to be fitted to the background
    Int_t    fNdf;     // number of degrees of freedom of polynomial fit
    Double_t fChisq;   // chi squared of polynomial fit
    Double_t fProb;    // chi squared probability ofg polynomial fit 

    TArrayD fValues;
    TArrayD fErrors;

    const static Int_t    fNdim = 6;
    Double_t fEmat[fNdim][fNdim];
    Double_t fEma [fNdim][fNdim];
    Double_t fCorr[fNdim][fNdim];

    Int_t  fMbins;     // number of bins in the fit range
    Int_t  fMlow;      // number of bins in the fit range with too few entries
    Int_t  fNzero;     // number of bins in the fit range with zero entry
    Int_t  fIstat;

    //--------------------

    //--------------------
    TF1      *fGPoly;   // (Gauss+polynomial) function
    TF1      *fGBackg;  // polynomial part of (Gauss+polynomial) function
    Int_t    fGFitBad;  // if != 0 fit failed
    Int_t    fGDegree;  // degree of polynomial to be fitted to the background
    Int_t    fGNdf;     // number of degrees of freedom of polynomial fit
    Double_t fGChisq;   // chi squared of polynomial fit
    Double_t fGProb;    // chi squared probability ofg polynomial fit 

    TArrayD fGValues;
    TArrayD fGErrors;

    const static Int_t    fGNdim = 9;
    Double_t fGEmat[fGNdim][fGNdim];
    Double_t fGEma[fGNdim][fGNdim];
    Double_t fGCorr[fGNdim][fGNdim];

    Int_t  fGMbins;     // number of bins in the fit range
    Int_t  fGNzero;     // numnber of bins in the fit range with zero entry
    Int_t  fGIstat;

    //--------------------

    static const TString gsDefName;  //! Default Name
    static const TString gsDefTitle; //! Default Title

    Bool_t DetExcess(); 
    Bool_t FitPolynomial();
    Bool_t FitGaussPoly();
    Bool_t RebinHistogram(Double_t x0, Int_t nrebin);

public:
    MHFindSignificance(const char *name=NULL, const char *title=NULL);
    ~MHFindSignificance();

    Bool_t FindSigma(TH1 *fhist,  Double_t alphamin, Double_t alphamax, 
                     Int_t degree, Double_t alphasig, 
                     Bool_t drawpoly, Bool_t fitgauss, Bool_t print);

    Bool_t SigmaLiMa(Double_t non, Double_t noff, Double_t gamma,
                     Double_t *siglima);

    Bool_t SigmaVsAlpha(TH1 *fhist,   Double_t alphamin, Double_t alphamax, 
                        Int_t degree, Bool_t print);

    Double_t GetSignificance()   { return fSigLiMa; }

    Bool_t DrawFit(Option_t *opt=NULL);

    Int_t    GetDegree()     const { return fDegree;  }
    Double_t GetProb()       const { return fProb;    }
    Int_t    GetNdf()        const { return fNdf;     }
    Double_t GetGamma()      const { return fGamma;   }
    Double_t GetNon()        const { return fNon;     }
    Double_t GetNex()        const { return fNex;     }
    Double_t GetNbg()        const { return fNbg;     }
    Double_t GetSigLiMa()    const { return fSigLiMa; }
    Int_t    GetMbins()      const { return fMbins;   }
    Double_t GetAlphasi()    const { return fAlphasi; }

    TH1* GetHist() const { return fHist;}
    Double_t GetdNon() const { return fNon;}
    Double_t GetdNbg() const {return fNbg; } 
    Double_t GetdNex()        const { return fdNex;     }
    Double_t GetNoff() const {return fNoff;}
    Double_t GetChisq() const {return fChisq;}
    Double_t GetNbgtotFitted() const {return fNbgtotFitted;}
    Double_t GetdNbgtotFitted() const {return fNbgtotFitted;}
    Double_t GetNbgtot() const {return fNbgtot;}
    Double_t GetSigmaGauss() const {return fSigmaGauss;}
    Double_t GetdSigmaGauss() const {return fdSigmaGauss;}
    Double_t GetNexGauss() const {return fNexGauss;}
    Double_t GetdNexGauss() const {return fdNexGauss;}
    TF1*    GetGBackg()     const { return fGBackg;  }
    TF1*    GetGPoly()      const { return fGPoly;   }
    TF1*    GetPoly()       const { return fPoly;    }


    void SetRebin(Bool_t b=kTRUE);
    void SetReduceDegree(Bool_t b=kTRUE);

    void PrintPoly(Option_t *opt=NULL);
    void PrintPolyGauss(Option_t *opt=NULL);

    ClassDef(MHFindSignificance, 1) // Determine significance from alpha plot
};

#endif




















