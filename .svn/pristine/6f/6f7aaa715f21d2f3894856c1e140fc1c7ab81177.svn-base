#ifndef MARS_MHHadronness
#define MARS_MHHadronness

#ifndef MARS_MH
#include "MH.h"
#endif

class TH1D;
class TGraph;
class MParList;
class MMcEvt;
class MParameterD;
class MHMatrix;

class MHHadronness : public MH
{
private:
    const MMcEvt      *fMcEvt;      //!
    const MParameterD *fHadronness; //!
    MHMatrix *fMatrix;              //!
    Int_t fMap;                     //!

    TH1D* fPhness;    //-> Hadrons Hadronness
    TH1D* fGhness;    //-> Gammas  Hadronness
    TH1D* fIntPhness; //-> Hadrons Acceptance
    TH1D* fIntGhness; //-> Gammas  Acceptance

    TGraph *fQfac;    //-> Quality factor
    TGraph *fGraph;   //-> gamma acceptance vs. hadron acceptance

    void Paint(Option_t *opt="");
    void CalcGraph(Double_t sumg, Double_t sump);

public:
    MHHadronness(Int_t nbins=100, const char *name=NULL, const char *title=NULL);
    ~MHHadronness();

    Double_t GetGammaAcceptance(Double_t acchad) const;
    Double_t GetHadronAcceptance(Double_t accgam) const;
    Double_t GetHadronness(Double_t acchad) const;

    TH1D *Getghness() const  { return fGhness; }
    TH1D *Getphness() const  { return fPhness; }
    TH1D *Getighness() const { return fIntGhness; }
    TH1D *Getiphness() const { return fIntPhness; }
    //TH2D *GetHist() const    { return fHist; }

    Float_t GetQ05() const;

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void InitMapping(MHMatrix *mat);
    void StopMapping();

    void Print(Option_t *option="") const;

    void Draw(Option_t *opt="");

    ClassDef(MHHadronness, 1) // Gamma/Hadron Separation Quality Histograms
};

#endif
