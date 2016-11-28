#ifndef MARS_MHn
#define MARS_MHn

#ifndef MARS_MH3
#include "MH3.h"
#endif

class MHn : public MH
{
public:
    enum Layout_t { kSimple, kComplex };

    enum { kDoNotReset = BIT(20) };

protected:
    MH3    *fHist[6];        // Possible six histograms
    TString fDrawOption[6];  // Possible corresponding draw options

    Layout_t fLayout;        // Specifier for the layout in the canvas

    Int_t fNum;              // Number of initialized histograms

    void InitHist();

    Bool_t InitName(Int_t n,  const char *name);
    Bool_t InitTitle(Int_t n, const char *title);
    Bool_t SetDrawOption(Int_t n, const char *opt);

public:
    MHn(const char *name=NULL, const char *title=NULL);
    ~MHn();

    // Setter
    void SetLayout(Layout_t t) { fLayout = t; }
    void SetDrawOption(const char *opt) { SetDrawOption(fNum-1, opt); }

    Bool_t AddHist(const char *memberx);
    Bool_t AddHist(const char *memberx, const char *membery, MH3::Type_t type=MH3::kHistogram);
    Bool_t AddHist(const char *memberx, const char *membery, const char *memberz, MH3::Type_t type=MH3::kHistogram);
    Bool_t AddHist(const char *memberx, const char *membery, const char *memberz, const char *weight);

    void InitName(const char *n)  { InitName(fNum-1, n); }
    void InitTitle(const char *t) { InitTitle(fNum-1, t); }

    // General interfact to MH3
    void SetScale(Double_t x, Double_t y=1, Double_t z=2) const;
    void SetLog(Bool_t x=kTRUE, Bool_t y=kTRUE, Bool_t z=kTRUE) const;
    void SetAutoRange(Bool_t x=kTRUE, Bool_t y=kTRUE, Bool_t z=kTRUE) const;
    void SetBinnings(MBinning *x=0, MBinning *y=0, MBinning *z=0) const;
    void Sumw2() const;

    // Interface to labels of MH3
    void InitLabels(MH3::Labels_t labels) const;

    void DefaultLabelX(const char *name=0);
    void DefaultLabelY(const char *name=0);
    void DefaultLabelZ(const char *name=0);

    void DefineLabelX(Int_t label, const char *name);
    void DefineLabelY(Int_t label, const char *name);
    void DefineLabelZ(Int_t label, const char *name);

    void DefineLabelsX(const TString &labels);
    void DefineLabelsY(const TString &labels);
    void DefineLabelsZ(const TString &labels);

    // Set additonal weights for MH3 
    void SetWeight(const char *phrase);

    // Set a conversion function for displaying the histogram
    void SetConversion(const char *func="");

    // MH
    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    // TObject
    //void SetColors() const;
    void Draw(Option_t *opt=NULL);
    //void Paint(Option_t *opt="");

    void RecursiveRemove(TObject *obj);

    ClassDef(MHn, 1) // Generalized histogram class for up to six histograms
};

#endif
