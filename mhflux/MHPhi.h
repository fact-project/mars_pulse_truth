#ifndef MARS_MHPhi
#define MARS_MHPhi

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MHillas;
class MSrcPosCam;
class MParameterD;

class MHPhi : public MH
{
private:
    TH1D fHPhi;             // Phi plot of the signal w.r.t. source
    TH1D fHPhiOff;          // Phi plot of the signal w.r.t. source+180deg
    TH1D fHTemplate;        // Template how the background should look in the ideal case

    TH1D fHInhom;           // Phi plot off the signal w.r.t. source (out of the ring with the signal)
    TH1D fHInhomOff;        // Phi plot off the signal w.r.t. source+180deg (out of the ring with the signal)

    MHillas     *fHillas;   //! Pointer to input container "MHillas"
    MSrcPosCam  *fSrcPos;   //! Pointer to input container "MSrcPosCam"
    MParameterD *fDisp;     //! Pointer to input container "Disp"

    Double_t fConvMm2Deg;   //! Conversion factor from camera geometry

    Int_t   fNumBinsSignal; // Number of bins for signal region
    Float_t fThetaCut;      // Theta cut
    Float_t fDistSrc;       // Nominal distance of source from center in wobble

    Bool_t  fUseAntiPhiCut; // Whether to use a anti-phi cut or not

    // Paint
    void PaintUpdate() const;
    void PaintText(const TH1D &res) const;

    // MH
    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

public:
    MHPhi(const char *name=NULL, const char *title=NULL);

    // Setter
    void SetNumBinsSignal(UInt_t n)       { fNumBinsSignal=TMath::Max(n, 1U); }
    void SetUseAntiPhiCut(Bool_t b=kTRUE) { fUseAntiPhiCut=b; }

    // TObject
    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    ClassDef(MHPhi, 2) // Histogram for ring-method
};

#endif


