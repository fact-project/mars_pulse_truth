#ifndef MARS_MH
#define MARS_MH

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TH1;
class TH1I;
class TH1D;
class TH2;
class TH3;
class TAxis;
class TArrayF;
class TArrayD;
class MArrayF;
class MArrayD;
class TCanvas;
class TProfile;

class MBinning;
class MParList;
#include <TH2.h>
#include <TH3.h>
class MH : public MParContainer
{
private:
    Byte_t fSerialNumber;   // Serial number (eg of telecope)
    UInt_t fNumExecutions;  // Number of calls to Fill function

    static MBinning GetBinningForLabels(TAxis &x, const MBinning *bins);
    static void RestoreBinningForLabels(TAxis &x);

public:
    MH(const char *name=NULL, const char *title=NULL);

    virtual void SetSerialNumber(Byte_t num) { fSerialNumber = num; }
    Byte_t  GetSerialNumber() const { return fSerialNumber; }
    TString AddSerialNumber(const char *str) const { TString s(str); if (fSerialNumber==0) return s; s += ";"; s += fSerialNumber; return s; }
    TString AddSerialNumber(const TString &str) const { return AddSerialNumber((const char*)str); }

    UInt_t GetNumExecutions() const { return fNumExecutions; }
    void SetNumExecutions(UInt_t n) { fNumExecutions=n; }

    void PrintSkipped(UInt_t n, const char *str);

    Bool_t OverwritesDraw(TClass *cls=NULL) const;

    virtual Bool_t SetupFill(const MParList *) { return kTRUE; }
    virtual Bool_t ReInit(MParList *) { return kTRUE; }
    virtual Int_t  Fill(const MParContainer *par, const Stat_t weight=1);
    virtual Bool_t Finalize() { return kTRUE; }

    virtual TString GetDataMember() const { return ""; }

    virtual TH1 *GetHistByName(const TString name) const;

    static TCanvas *MakeDefCanvas(TString name="", const char *title="",
                                  UInt_t w=625, UInt_t h=440,
                                  Bool_t usescreenfactor=kTRUE);
    static TCanvas *MakeDefCanvas(const TObject *obj,
                                  UInt_t w=625, UInt_t h=440,
                                  Bool_t usescreenfactor=kFALSE);

    static void RemoveFromPad(const char *name);

    // Functions to applay new binnings to histograms
    static void SetBinning(TH1 &h, const MBinning &binsx);
    static void SetBinning(TH2 &h, const MBinning &binsx, const MBinning &binsy);
    static void SetBinning(TH3 &h, const MBinning &binsx, const MBinning &binsy, const MBinning &binsz);

    static void SetBinning(TH1 &h, const TArrayD &binsx);
    static void SetBinning(TH2 &h, const TArrayD &binsx, const TArrayD &binsy);
    static void SetBinning(TH3 &h, const TArrayD &binsx, const TArrayD &binsy, const TArrayD &binsz);

    static void SetBinning(TH1 &h, const TAxis &binsx);
    static void SetBinning(TH2 &h, const TAxis &binsx, const TAxis &binsy);
    static void SetBinning(TH3 &h, const TAxis &binsx, const TAxis &binsy, const TAxis &binsz);

    static void CopyBinning(const TH1 &x, TH1 &h);

    static Bool_t ApplyBinning(const MParList &plist, const TString x, TH1 &h);
    static Bool_t ApplyBinning(const MParList &plist, const TString x, TString y, TH2 &h);
    static Bool_t ApplyBinning(const MParList &plist, const TString x, TString y, TString z, TH3 &h);

    // Warpper functions not to break macros (new functions take references)
    static void SetBinning(TH1 *h, const MBinning *binsx) { SetBinning(*h, *binsx); }
    static void SetBinning(TH2 *h, const MBinning *binsx, const MBinning *binsy) { SetBinning(*h, *binsx, *binsy); }
    static void SetBinning(TH3 *h, const MBinning *binsx, const MBinning *binsy, const MBinning *binsz) { SetBinning(*h, *binsx, *binsy, *binsz); }

    static void SetBinning(TH1 *h, const TArrayD &binsx) { SetBinning(*h, binsx); }
    static void SetBinning(TH2 *h, const TArrayD &binsx, const TArrayD &binsy) { SetBinning(*h, binsx, binsy); }
    static void SetBinning(TH3 *h, const TArrayD &binsx, const TArrayD &binsy, const TArrayD &binsz)  { SetBinning(*h, binsx, binsy, binsz); }

    static void SetBinning(TH1 *h, const TAxis *binsx) { SetBinning(*h, *binsx); }
    static void SetBinning(TH2 *h, const TAxis *binsx, const TAxis *binsy) { SetBinning(*h, *binsx, *binsy); }
    static void SetBinning(TH3 *h, const TAxis *binsx, const TAxis *binsy, const TAxis *binsz) { SetBinning(*h, *binsx, *binsy, *binsz); }

    static Bool_t ApplyBinning(const MParList &plist, TString x, TH1 *h) { return ApplyBinning(plist, x, *h); }
    static Bool_t ApplyBinning(const MParList &plist, TString x, TString y, TH2 *h) { return ApplyBinning(plist, x, y, *h); }
    static Bool_t ApplyBinning(const MParList &plist, TString x, TString y, TString z, TH3 *h) { return ApplyBinning(plist, x, y, z, *h); }

    // Other helpers for histogram treatment
    static void SetBinomialErrors(TH1 &hres, const TH1 &h1, const TH1 &h2, Double_t c1=1, Double_t c2=1);

    static void RemoveFirstBin(TH1 &h);

    static void    ScaleArray(TArrayD &bins, Double_t f);
    static TArrayD ScaleAxis(TAxis &axe, Double_t f);
    static void    ScaleAxis(TH1 &bins, Double_t fx=1, Double_t fy=1, Double_t fz=1);

    static Double_t GetBinCenterLog(const TAxis &axe, Int_t nbin);

    static void DrawSameCopy(const TH1 &hist1, const TH1 &hist2, const TString title);
    static void DrawSame(TH1 &hist1, TH1 &hist2, const TString title, Bool_t same=kFALSE);

    TObject *Clone(const char *name="") const;

    TObject *DrawClone(Option_t *opt, Int_t w, Int_t h) const;
    TObject *DrawClone(Option_t *opt="") const
    {
        return MH::DrawClone(opt, 625, 440);
    }

    static TVirtualPad *GetNewPad(TString &opt);

    static void FindGoodLimits(Int_t nbins, Int_t &newbins, Double_t &xmin, Double_t &xmax, Bool_t isInteger);
    static Double_t GetMinimumGT(const TH1 &h, Double_t gt=0);
    static Int_t StripZeros(TH1 &h, Int_t nbins);

    static TH1I* ProjectArray(const TArrayF &array, Int_t nbins=30,
                              const char* name="ProjectArray", const char* title="Projected Array");
    static TH1I* ProjectArray(const TArrayD &array, Int_t nbins=30,
                              const char* name="ProjectArray", const char* title="Projected Array");
    static TH1I* ProjectArray(const MArrayF &array, Int_t nbins=30,
                              const char* name="ProjectArray", const char* title="Projected Array");
    static TH1I* ProjectArray(const MArrayD &array, Int_t nbins=30,
                              const char* name="ProjectArray", const char* title="Projected Array");
    
    static void GetRangeUser(const TH1 &hist, Axis_t &lo, Axis_t &hi);
    static void GetRangeUserX(const TH1 &hist, Axis_t &lo, Axis_t &hi);
    static void GetRangeUserY(const TH1 &hist, Axis_t &lo, Axis_t &hi);
    static void GetRange(const TH1 &hist, Int_t &lo, Int_t &hi);
    static void GetRangeX(const TH1 &hist, Int_t &lo, Int_t &hi);
    static void GetRangeY(const TH1 &hist, Int_t &lo, Int_t &hi);

    static TObject *FindObjectInPad(const char *name, TVirtualPad *pad=NULL);

    static void SetPalette(TString paletteName="pretty", Int_t ncol=50);
    static void SetPadRange(Float_t max, Float_t aspect=1);
    static void SetPadRange(Float_t x0, Float_t y0, Float_t x1, Float_t y1);

    static char *GetObjectInfoH(Int_t px, Int_t py, const TH1 &h);
    static char *GetObjectInfoP(Int_t px, Int_t py, const TProfile &p);
    static char *GetObjectInfo(Int_t px, Int_t py, const TObject &o);
    char *GetObjectInfo(Int_t px, Int_t py) const { return MParContainer::GetObjectInfo(px, py); }

    ClassDef(MH, 2) //A base class for Mars histograms
};

#endif

