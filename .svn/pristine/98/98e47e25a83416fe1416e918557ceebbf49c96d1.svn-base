#ifndef MARS_MSpline3
#define MARS_MSpline3

#ifndef ROOT_TSpline
#include <TSpline.h>
#endif

class MArrayD;

class MSpline3 : public TSpline3
{
private:
    TGraph  *ConvertSpline(const TSpline &s, Float_t freq) const;
    TGraph  *ConvertGraph(const TGraph &s, Float_t freq) const;
    MArrayD &ConvertFunc(const TF1 &f, Float_t freq) const;

    Double_t IntegralBin(Int_t i, Double_t x) const;
    Double_t IntegralBin(Int_t i) const;

public:
    MSpline3() { }

    // None equidistant binning (evaluation a bit slower in case of many bins)
    MSpline3(const TGraph &g,
             const char *opt=0, Double_t valbeg=0, Double_t valend=0)
       : TSpline3("MSpline3", &g, opt, valbeg, valend)
    {
    }

    MSpline3(const TGraph &g, Double_t freq,
             const char *opt=0, Double_t valbeg=0, Double_t valend=0)
       : TSpline3("MSpline3", ConvertGraph(g, freq), opt, valbeg, valend)
    {
    }

    MSpline3(const TSpline &s, Double_t freq,
             const char *opt=0, Double_t valbeg=0, Double_t valend=0)
       : TSpline3("MSpline3", ConvertSpline(s, freq), opt, valbeg, valend)
    {
    }

    MSpline3(const Double_t x[], const Double_t y[], Int_t n, const char *opt=0,
             Double_t valbeg=0, Double_t valend=0)
       : TSpline3("MSpline3", const_cast<Double_t*>(x), const_cast<Double_t*>(y), n, opt, valbeg, valend)
    {
    }

    // Equidistant binning (evaluation a bit faster in case of many bins)

    // FIXME: In principle TF1 can be evaluated histogram like which should be faster
    MSpline3(const TF1 &f, const char *opt=0,Double_t valbeg=0, Double_t valend=0);
    MSpline3(const TF1 &f, Double_t freq, const char *opt=0,Double_t valbeg=0, Double_t valend=0);

    Double_t GetXmin() const { return fXmin; }     // Minimum value of abscissa
    Double_t GetXmax() const { return fXmax; }     // Maximum value of abscissa

    Int_t GetNp() const { return fNp; }

    TH1 *GetHistogram() const { return (TH1*)fHistogram; }

    Double_t Integral(Double_t a, Double_t b) const;
    Double_t Integral() const;

    Double_t IntegralSolidAngle() const;

    ClassDef(MSpline3, 1) // An extension of the TSpline3
};

#endif
