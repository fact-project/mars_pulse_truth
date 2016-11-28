#ifndef MARS_MHMcTriggerLvl2
#define MARS_MHMcTriggerLvl2

#ifndef MARS_MH
#include "MH.h"
#endif

class TH2D;
class TH1F;
class TF1;
class MMcTriggerLvl2;

class MHMcTriggerLvl2 : public MH
{
private:

    TH1F *fHistLutPseudoSize;        // Histogram of fLutPseudoSize
    TH1F *fHistLutPseudoSizeNorm;    // Histogram of fLutPseudoSize normalized on integral of distribution
    TH1F *fHistPseudoSize;           // Histogram of fPseudoSize
    TH1F *fHistPseudoSizeNorm;       // Histogram of fPseudoSize normalized on integral of distribution
    TH1F *fHistCellPseudoSize;           // Histogram of fCellPseudoSize
    TH1F *fHistCellPseudoSizeNorm;       // Histogram of fCellPseudoSize normalized on integral of distribution
    TH1F *fHistSizeBiggerCell;       // Histogram of fSizeBiggerCell
    TH1F *fHistSizeBiggerCellNorm;   // Histogram of fSizeBiggerCell normalized on integral of distribution
    TH2D *fHistPseudoSizeEnergy;     // 2D-Histogram of fPseudoSize vs. Energy
    TH2D *fHistLutPseudoSizeEnergy;  // 2D-Histogram of fLutPseudoSize vs. Energy
    TH2D *fHistCellPseudoSizeEnergy;  // 2D-Histogram of fCellPseudoSize vs. Energy
    TH2D *fHistSizeBiggerCellEnergy; // 2D-Histogram of fSizeBiggerCell vs. Energy
    TF1* fFNorm;                     // Function used to normalize histograms

    static Int_t fColorLps;
    static Int_t fColorSbc;
    static Int_t fColorPs;
    static Int_t fColorCps;
    static Int_t fColorPsE;
    static Int_t fColorLPsE;
    static Int_t fColorCPsE;
    static Int_t fColorSBCE;
    

    TObject *DrawHist(TH1 &hist, TH1 &histNorm, const TString &canvasname, Int_t &colore) const;
    TObject *Draw2DHist(TH1 &hist, const TString &canvasname, Int_t &col) const;
    
public:
    MHMcTriggerLvl2(const char *name=NULL, const char *title=NULL);
    ~MHMcTriggerLvl2();

    Bool_t Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;

    TH1F *GetHistLutPseudoSize() const { return fHistLutPseudoSize; }
    TH1F *GetHistLutPseudoSizeNorm() const { return fHistLutPseudoSizeNorm; }
    TH1F *GetHistPseudoSize()  const { return fHistPseudoSize; }
    TH1F *GetHistPseudoSizeNorm()  const { return fHistPseudoSizeNorm; }
    TH1F *GetHistCellPseudoSize()  const { return fHistCellPseudoSize; }
    TH1F *GetHistCellPseudoSizeNorm()  const { return fHistCellPseudoSizeNorm; }
    TH1F *GetHistSizeBiggerCell()  const { return fHistSizeBiggerCell; }
    TH1F *GetHistSizeBiggerCellNorm()  const { return fHistSizeBiggerCellNorm; }
    TH2D *GetHistLutPseudoSizeEnergy() const { return fHistLutPseudoSizeEnergy; }
    TH2D *GetHistPseudoSizeEnergy() const { return fHistPseudoSizeEnergy; }
    TH2D *GetHistSizeBiggerCellEnergy() const { return fHistSizeBiggerCellEnergy; }
    

    void Draw(Option_t *opt=NULL);
    TObject *DrawClone(Option_t *opt=NULL) const;

    TH1 *NormalizeHist(TH1 &histNorm, TH1 *hist) const;

    ClassDef(MHMcTriggerLvl2, 1) // Container which holds histograms for the Trigger Level2 image parameters
};


#endif
