#ifndef MARS_MH3
#define MARS_MH3

#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef MARS_MObjLookup
#include "MObjLookup.h"
#endif

class TH1;
class MData;
class MBinning;
class TFormula;

class MH3 : public MH
{
public:
    enum Labels_t {
        kNoLabels  = 0,
        kLabelsX   = BIT(0),
        kLabelsY   = BIT(1),
        kLabelsZ   = BIT(2),
        kLabelsXY  = kLabelsX|kLabelsY,
        kLabelsXZ  = kLabelsX|kLabelsZ,
        kLabelsYZ  = kLabelsY|kLabelsZ,
        kLabelsXYZ = kLabelsX|kLabelsY|kLabelsZ,
    };

    enum Type_t {
        kHistogram     = 0,
        kProfile       = 1,
        kProfileSpread = kProfile | 2,
    };

    enum {
        kDoNotReset = BIT(20)
    };


private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    // Helper for constructor
    void        Init();

    // Helper for dealing with labeled histograms
    MObjLookup *GetLabels(char axe);
    void        InitLabels(TAxis &x) const;
    void        DeflateLabels() const;
    Labels_t    GetLabels() const;
    const char *GetLabel(Int_t axe, Double_t val) const;

    // Get type
    Type_t GetType() const;

    // Helper for conversion
    void Convert(TH1 &h) const;

    MObjLookup fLabels[3];       //! Lookup table to conflate and name labels

protected:
    // Could be const but root < 3.02/06 doesn't like this...
    Int_t       fDimension;      // Number of dimensions of histogram
    TH1        *fHist;           // Histogram to fill
    MData      *fData[4];        // Object from which the data is filled
    MData      *fWeight;         // Additional weight
    MBinning   *fBins[3];        // Binning set omitting the parlist access
    Double_t    fScale[4];       // Scale for the three axis (eg unit)
    Byte_t      fStyleBits;      // Set the range of a histogram automatically in Finalize

    TFormula   *fConversion;     // Conversion function for displaying the histogram

    void HandleLogAxis(TAxis &axe) const;

    void StreamPrimitive(std::ostream &out) const;


    enum {
        kIsLogx     = BIT(17),
        kIsLogy     = BIT(18),
        kIsLogz     = BIT(19)
    };

public:
    MH3(const Int_t dim=0, Type_t type=MH3::kHistogram);
    MH3(const TH1 &h1);
    MH3(const char *memberx, Type_t type=MH3::kHistogram);
    MH3(const char *memberx, const char *membery, Type_t type=MH3::kHistogram);
    MH3(const char *memberx, const char *membery, const char *memberz, Type_t type=MH3::kHistogram);
    MH3(const char *memberx, const char *membery, const char *memberz, const char *weight, Type_t type=MH3::kProfile);
    ~MH3();

    // Setter
    void SetScaleX(Double_t scale) { fScale[0] = scale; }
    void SetScaleY(Double_t scale) { fScale[1] = scale; }
    void SetScaleZ(Double_t scale) { fScale[2] = scale; }
    void SetScaleT(Double_t scale) { fScale[3] = scale; }
    void SetScale(Double_t x, Double_t y=1, Double_t z=1, Double_t t=1) { SetScaleX(x); SetScaleY(y); SetScaleZ(z); SetScaleT(t); }

    void SetLogx(Bool_t b=kTRUE) { b ? fHist->SetBit(kIsLogx) : fHist->ResetBit(kIsLogx); }
    void SetLogy(Bool_t b=kTRUE) { b ? fHist->SetBit(kIsLogy) : fHist->ResetBit(kIsLogy); }
    void SetLogz(Bool_t b=kTRUE) { b ? fHist->SetBit(kIsLogz) : fHist->ResetBit(kIsLogz); }
    void SetLog(Bool_t x=kTRUE, Bool_t y=kTRUE, Bool_t z=kTRUE) { SetLogx(x); SetLogy(y); SetLogz(z); }

    void SetAutoRangeX(Bool_t b=kTRUE) { b ? SETBIT(fStyleBits, 0) : CLRBIT(fStyleBits, 0); }
    void SetAutoRangeY(Bool_t b=kTRUE) { b ? SETBIT(fStyleBits, 1) : CLRBIT(fStyleBits, 1); }
    void SetAutoRangeZ(Bool_t b=kTRUE) { b ? SETBIT(fStyleBits, 2) : CLRBIT(fStyleBits, 2); }
    void SetAutoRange(Bool_t x=kTRUE, Bool_t y=kTRUE, Bool_t z=kTRUE) { SetAutoRangeX(x); SetAutoRangeY(y); SetAutoRangeZ(z); }

    void SetBinningX(MBinning *x) { fBins[0] = x; }
    void SetBinningY(MBinning *y) { fBins[1] = y; }
    void SetBinningZ(MBinning *z) { fBins[2] = z; }
    void SetBinnings(MBinning *x=0, MBinning *y=0, MBinning *z=0) { SetBinningX(x); SetBinningY(y); SetBinningZ(z); }

    void Sumw2() const { if (fHist) fHist->Sumw2(); }

    void InitLabels(Labels_t labels) const;

    void DefaultLabel(char axe, const char *name=0);
    void DefaultLabelX(const char *name=0) { DefaultLabel('x', name); }
    void DefaultLabelY(const char *name=0) { DefaultLabel('y', name); }
    void DefaultLabelZ(const char *name=0) { DefaultLabel('z', name); }

    void DefineLabel(char axe, Int_t label=0, const char *name=0);
    void DefineLabelX(Int_t label, const char *name) { DefineLabel('x', label, name); }
    void DefineLabelY(Int_t label, const char *name) { DefineLabel('y', label, name); }
    void DefineLabelZ(Int_t label, const char *name) { DefineLabel('z', label, name); }

    void DefineLabels(char axe, const TString &labels);
    void DefineLabelsX(const TString &labels) { DefineLabels('x', labels); }
    void DefineLabelsY(const TString &labels) { DefineLabels('y', labels); }
    void DefineLabelsZ(const TString &labels) { DefineLabels('z', labels); }

    void SetWeight(const char *phrase);

    Bool_t SetConversion(const char *func="");

    // Getter
    Int_t GetDimension() const { return TMath::Abs(fDimension); }
    Int_t GetNbins() const;
    Int_t FindFixBin(Double_t x, Double_t y=0, Double_t z=0) const;

    TH1 &GetHist() { return *fHist; }
    const TH1 &GetHist() const { return *fHist; }

    TString GetDataMember() const;
    TString GetRule(const Char_t axis='x') const;

    // MH
    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    TH1 *GetHistByName(const TString name="") const { return fHist; }
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    // MParContainer
    MParContainer *New() const;

    // TObject
    void SetName(const char *name);
    void SetTitle(const char *title);

    const char *GetTitle() const { return fHist ? fHist->GetTitle() : static_cast<const char *>(fTitle); }
    const TString &GetFullTitle() const { return fTitle; }

    void Draw(Option_t *opt=NULL);
    void Paint(Option_t *opt="");

    void RecursiveRemove(TObject *obj);

    ClassDef(MH3, 7) // Generalized 1/2/3D-histogram for Mars variables
};

#endif
