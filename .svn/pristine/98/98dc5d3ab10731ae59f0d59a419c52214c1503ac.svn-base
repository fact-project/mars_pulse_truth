#ifndef MARS_MMath
#define MARS_MMath

#ifndef	_MATH_H
#include <math.h>  // Needed for darwin
#endif

#ifndef ROOT_TMath
#include <TMath.h> // TMath is included here for convinience
#endif

class TVector2;
class TVector3;
class TArrayD;

namespace MMath
{
    Double_t GaussProb(Double_t x, Double_t sigma=1, Double_t mean=0);
    Double_t GaussProb2D(Double_t x, Double_t sigma=1);

    template <class Size, class Element> void ReSortImp(Size n, Element *a, Bool_t down=kFALSE);
    void ReSort(Long64_t n, Short_t  *a, Bool_t down=kFALSE);
    void ReSort(Long64_t n, Int_t    *a, Bool_t down=kFALSE);
    void ReSort(Long64_t n, Float_t  *a, Bool_t down=kFALSE);
    void ReSort(Long64_t n, Double_t *a, Bool_t down=kFALSE);

    template <class Size, class Element> Double_t MedianDevImp(Size n, const Element *a) { Double_t med; return MedianDevImp(n, a, med); }
    template <class Size, class Element> Double_t MedianDevImp(Size n, const Element *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Short_t  *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Int_t    *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Float_t  *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Double_t *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Long_t   *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Long64_t *a, Double_t &med);
    Double_t  MedianDev(Long64_t n, const Short_t  *a);
    Double_t  MedianDev(Long64_t n, const Int_t    *a);
    Double_t  MedianDev(Long64_t n, const Float_t  *a);
    Double_t  MedianDev(Long64_t n, const Double_t *a);
    Double_t  MedianDev(Long64_t n, const Long_t   *a);
    Double_t  MedianDev(Long64_t n, const Long64_t *a);

    Double_t Significance(Double_t s, Double_t b);
    Double_t SignificanceSym(Double_t s, Double_t b);
    Double_t SignificanceLiMa(Double_t s, Double_t b, Double_t alpha=1);
    Double_t SignificanceLiMaSigned(Double_t s, Double_t b, Double_t alpha=1);
    Double_t SignificanceExc(Double_t s, Double_t b, Double_t alpha=1);
    Double_t ErrorExc(Double_t s, Double_t b, Double_t alpha=1);

    void ReducePrecision(Float_t &val);

    TVector3 GetParab(const TVector3 &x, const TVector3 &y);
    Double_t InterpolParabLin(const TVector3 &vx, const TVector3 &vy, Double_t x);
    Double_t InterpolParabLin(const TVector3 &vy, Double_t x);
    Double_t InterpolParabLog(const TVector3 &vx, const TVector3 &vy, Double_t x);
    Double_t InterpolParabCos(const TVector3 &vx, const TVector3 &vy, Double_t x);

    TVector2 GetIntersectionPoint(const TVector2 &x1, const TVector2 &y1, const TVector2 &x2, const TVector2 &y2, Bool_t &rc);
    TVector2 GetIntersectionPoint(const TVector2 &x1, const TVector2 &y1, const TVector2 &x2, const TVector2 &y2);

    inline Int_t SolvePol1(Double_t c, Double_t d, Double_t &x1)
    {
        if (c==0)
            return 0;

        x1 = -d/c;
        return 1;
    }
    Int_t SolvePol2(Double_t c, Double_t d, Double_t &x1, Double_t &x2);
    inline Int_t SolvePol2(Double_t b, Double_t c, Double_t d, Double_t &x1, Double_t &x2)
    {
        return b==0 ? SolvePol1(c, d, x1) : SolvePol2(c/b, d/b, x1, x2);
    }
    Int_t SolvePol3(Double_t b, Double_t c, Double_t d, Double_t &x1, Double_t &x2, Double_t &x3);
    inline Int_t SolvePol3(Double_t a, Double_t b, Double_t c, Double_t d, Double_t &x1, Double_t &x2, Double_t &x3)
    {
        return a==0 ? SolvePol2(b, c, d, x1, x2) : SolvePol3(b/a, c/a, d/a, x1, x2, x3);
    }

    TArrayD LeastSqFitExpW1(Int_t n, Double_t *x, Double_t *y);
    TArrayD LeastSqFitExp(Int_t n, Double_t *x, Double_t *y);
    TArrayD LeastSqFitLog(Int_t n, Double_t *x, Double_t *y);
    TArrayD LeastSqFitPowerLaw(Int_t n, Double_t *x, Double_t *y);

    inline Int_t ModF(Double_t dbl, Double_t &frac) { Double_t rc; frac = ::modf(dbl, &rc); return TMath::Nint(rc); }

    inline Double_t Sqrt3(Double_t x) { return ::cbrt(x); }

    inline Double_t Sgn(Double_t d) { return d<0 ? -1 : 1; }

    void Format(Double_t &v, Double_t &e);

    Double_t RndmExp(Double_t tau);
}

#endif
