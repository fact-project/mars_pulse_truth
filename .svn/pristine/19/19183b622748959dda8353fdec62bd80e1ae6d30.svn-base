/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz  3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMath
//
// Mars - Math package (eg Significances, etc)
//
/////////////////////////////////////////////////////////////////////////////
#include "MMath.h"

#include <stdlib.h> // atof (Ubuntu 8.10)

#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_TComplex
#include <TComplex.h>
#endif

#ifndef ROOT_TRandom
#include <TRandom.h>  // gRandom in RndmExp
#endif

#include "MString.h"

//NamespaceImp(MMath);

// --------------------------------------------------------------------------
//
// Calculate Significance as
// significance = (s-b)/sqrt(s+k*k*b) mit k=s/b
//
// s: total number of events in signal region
// b: number of background events in signal region
// 
Double_t MMath::Significance(Double_t s, Double_t b)
{
    const Double_t k = b==0 ? 0 : s/b;
    const Double_t f = s+k*k*b;

    return f==0 ? 0 : (s-b)/TMath::Sqrt(f);
}

// --------------------------------------------------------------------------
//
// Symmetrized significance - this is somehow analog to
// SignificanceLiMaSigned
//
// Returns Significance(s,b) if s>b otherwise -Significance(b, s);
// 
Double_t MMath::SignificanceSym(Double_t s, Double_t b)
{
    return s>b ? Significance(s, b) : -Significance(b, s);
}

// --------------------------------------------------------------------------
//
//  calculates the significance according to Li & Ma
//  ApJ 272 (1983) 317, Formula 17
//
//  s                    // s: number of on events
//  b                    // b: number of off events
//  alpha = t_on/t_off;  // t: observation time
//
//  The significance has the same (positive!) value for s>b and b>s.
//
//  Returns -1 if s<0 or b<0 or alpha<0 or the argument of sqrt<0
//
// Here is some eMail written by Daniel Mazin about the meaning of the arguments:
//
//  > Ok. Here is my understanding:
//  > According to Li&Ma paper (correctly cited in MMath.cc) alpha is the
//  > scaling factor. The mathematics behind the formula 17 (and/or 9) implies
//  > exactly this. If you scale OFF to ON first (using time or using any other
//  > method), then you cannot use formula 17 (9) anymore. You can just try
//  > the formula before scaling (alpha!=1) and after scaling (alpha=1), you
//  > will see the result will be different.
//
//  > Here are less mathematical arguments:
//
//  >  1) the better background determination you have (smaller alpha) the more
//  > significant is your excess, thus your analysis is more sensitive. If you
//  > normalize OFF to ON first, you loose this sensitivity.
//
//  >  2) the normalization OFF to ON has an error, which naturally depends on
//  > the OFF and ON. This error is propagating to the significance of your
//  > excess if you use the Li&Ma formula 17 correctly. But if you normalize
//  > first and use then alpha=1, the error gets lost completely, you loose
//  > somehow the criteria of goodness of the normalization.
//
Double_t MMath::SignificanceLiMa(Double_t s, Double_t b, Double_t alpha)
{
    const Double_t sum = s+b;

    if (s<0 || b<0 || alpha<=0)
        return -1;

    const Double_t l = s==0 ? 0 : s*TMath::Log(s/sum*(alpha+1)/alpha);
    const Double_t m = b==0 ? 0 : b*TMath::Log(b/sum*(alpha+1)      );

    return l+m<0 ? -1 : TMath::Sqrt((l+m)*2);
}

/*
Double_t MMath::SignificanceLiMaErr(Double_t s, Double_t b, Double_t alpha)
{
    Double_t S = SignificanceLiMa(s, b, alpha);
    if (S<0)
        return -1;

    const Double_t sum = s+b;


    Double_t l = TMath::Log(s/sum*(alpha+1)/alpha)/TMath::Sqrt(2*S);
    Double_t m = TMath::Log(s/sum*(alpha+1)/alpha)/TMath::Sqrt(2*S);


    const Double_t sum = s+b;

    if (s<0 || b<0 || alpha<=0)
        return -1;

    const Double_t l = s==0 ? 0 : s*TMath::Log(s/sum*(alpha+1)/alpha);
    const Double_t m = b==0 ? 0 : b*TMath::Log(b/sum*(alpha+1)      );

    return l+m<0 ? -1 : TMath::Sqrt((l+m)*2);
}
*/

// --------------------------------------------------------------------------
//
// Calculates MMath::SignificanceLiMa(s, b, alpha). Returns 0 if the
// calculation has failed. Otherwise the Li/Ma significance which was
// calculated. If s<b a negative value is returned.
//
Double_t MMath::SignificanceLiMaSigned(Double_t s, Double_t b, Double_t alpha)
{
    const Double_t sig = SignificanceLiMa(s, b, alpha);
    if (sig<=0)
        return 0;

    return TMath::Sign(sig, s-alpha*b);
}

// --------------------------------------------------------------------------
//
// Return Li/Ma (5) for the error of the excess, under the assumption that
// the existance of a signal is already known. (basically signal/error
// calculated by error propagation)
//
Double_t MMath::SignificanceExc(Double_t s, Double_t b, Double_t alpha)
{
    const Double_t error = ErrorExc(s, b, alpha);
    if (error==0)
        return 0;

    const Double_t Ns = s - alpha*b;

    return Ns/error;
}

// --------------------------------------------------------------------------
//
// Calculate the error of s-alpha*b by error propagation
//
Double_t MMath::ErrorExc(Double_t s, Double_t b, Double_t alpha)
{
    const Double_t sN = s + alpha*alpha*b;
    return sN<0 ? 0 : TMath::Sqrt(sN);
}

// --------------------------------------------------------------------------
//
// Returns: 2/(sigma*sqrt(2))*integral[0,x](exp(-(x-mu)^2/(2*sigma^2)))
//
Double_t MMath::GaussProb(Double_t x, Double_t sigma, Double_t mean)
{
    if (x<mean)
        return 0;

    static const Double_t sqrt2 = TMath::Sqrt(2.);

    const Double_t rc = TMath::Erf((x-mean)/(sigma*sqrt2));

    if (rc<0)
        return 0;
    if (rc>1)
        return 1;

    return rc;
}

// --------------------------------------------------------------------------
//
// Returns: 1 - exp((x/sigma)^2 / 2)
//
Double_t MMath::GaussProb2D(Double_t x, Double_t sigma)
{
    const Double_t xs = x/sigma;
    return 1 - TMath::Exp(-xs*xs/2);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
template <class Size, class Element>
Double_t MMath::MedianDevImp(Size n, const Element *a, Double_t &med)
{
    static const Double_t prob = 0.682689477208650697; //MMath::GaussProb(1.0);

    // Sanity check
    if (n <= 0 || !a)
        return 0;

    // Get median of distribution
    med = TMath::Median(n, a);

    // Create the abs(a[i]-med) distribution
    Double_t arr[n];
    for (int i=0; i<n; i++)
        arr[i] = (Double_t)TMath::Abs(Double_t(a[i])-med);

    //return TMath::Median(n, arr)/0.67449896936; //MMath::GaussProb(x)=0.5

    // Define where to divide (floor because the highest possible is n-1)
    const Size div = TMath::FloorNint(Double_t(n)*prob);

    // Calculate result
    Double_t dev = TMath::KOrdStat(n, arr, div);
    if (n%2 == 0)
    {
        dev += TMath::KOrdStat(n, arr, div-1);
        dev /= 2;
    }

    return dev;
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Short_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Int_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Float_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Double_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Long_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

// ------------------------------------------------------------------------
//
// Return the "median" (at 68.3%) value of the distribution of
// abs(a[i]-Median)
//
Double_t MMath::MedianDev(Long64_t n, const Long64_t *a, Double_t &med)
{
    return MedianDevImp(n, a, med);
}

Double_t MMath::MedianDev(Long64_t n, const Short_t  *a) { Double_t med; return MedianDevImp(n, a, med); }
Double_t MMath::MedianDev(Long64_t n, const Int_t    *a) { Double_t med; return MedianDevImp(n, a, med); }
Double_t MMath::MedianDev(Long64_t n, const Float_t  *a) { Double_t med; return MedianDevImp(n, a, med); }
Double_t MMath::MedianDev(Long64_t n, const Double_t *a) { Double_t med; return MedianDevImp(n, a, med); }
Double_t MMath::MedianDev(Long64_t n, const Long_t   *a) { Double_t med; return MedianDevImp(n, a, med); }
Double_t MMath::MedianDev(Long64_t n, const Long64_t *a) { Double_t med; return MedianDevImp(n, a, med); }

// ------------------------------------------------------------------------
//
// Re-sort an array. Intsead of returning an index (like TMath::Sort)
// the array contents are sorted.
//
template <class Size, class Element> void MMath::ReSortImp(Size n, Element *a, Bool_t down)
{
    Element *cpy = new Element[n];
    Size    *pos = new Size[n];

    memcpy(cpy, a, n*sizeof(Element));

    TMath::Sort(n, a, pos, down);

    Size *idx = pos;

    for (Element *ptr=a; ptr<a+n; ptr++)
        *ptr = cpy[*idx++];

    delete [] cpy;
    delete [] pos;
}

void MMath::ReSort(Long64_t n, Short_t  *a, Bool_t down) { ReSortImp(n, a, down); }
void MMath::ReSort(Long64_t n, Int_t    *a, Bool_t down) { ReSortImp(n, a, down); }
void MMath::ReSort(Long64_t n, Float_t  *a, Bool_t down) { ReSortImp(n, a, down); }
void MMath::ReSort(Long64_t n, Double_t *a, Bool_t down) { ReSortImp(n, a, down); }

// --------------------------------------------------------------------------
//
// This function reduces the precision to roughly 0.5% of a Float_t by
// changing its bit-pattern (Be carefull, in rare cases this function must
// be adapted to different machines!). This is usefull to enforce better
// compression by eg. gzip.
//
void MMath::ReducePrecision(Float_t &val)
{
    UInt_t &f = (UInt_t&)val;

    f += 0x00004000;
    f &= 0xffff8000;
}

// -------------------------------------------------------------------------
//
// Quadratic interpolation
//
// calculate the parameters of a parabula such that
//    y(i) = a + b*x(i) + c*x(i)^2
//
// If the determinant==0 an empty TVector3 is returned.
//
TVector3 MMath::GetParab(const TVector3 &x, const TVector3 &y)
{
    const Double_t x1 = x(0);
    const Double_t x2 = x(1);
    const Double_t x3 = x(2);

    const Double_t y1 = y(0);
    const Double_t y2 = y(1);
    const Double_t y3 = y(2);

    const double det =
        + x2*x3*x3 + x1*x2*x2 + x3*x1*x1
        - x2*x1*x1 - x3*x2*x2 - x1*x3*x3;


    if (det==0)
        return TVector3();

    const double det1 = 1.0/det;

    const double ai11 = x2*x3*x3 - x3*x2*x2;
    const double ai12 = x3*x1*x1 - x1*x3*x3;
    const double ai13 = x1*x2*x2 - x2*x1*x1;

    const double ai21 = x2*x2 - x3*x3;
    const double ai22 = x3*x3 - x1*x1;
    const double ai23 = x1*x1 - x2*x2;

    const double ai31 = x3 - x2;
    const double ai32 = x1 - x3;
    const double ai33 = x2 - x1;

    return TVector3((ai11*y1 + ai12*y2 + ai13*y3) * det1,
                    (ai21*y1 + ai22*y2 + ai23*y3) * det1,
                    (ai31*y1 + ai32*y2 + ai33*y3) * det1);
}

// --------------------------------------------------------------------------
//
//  Interpolate the points with x-coordinates vx and y-coordinates vy
// by a parabola (second order polynomial) and return the value at x.
//
Double_t MMath::InterpolParabLin(const TVector3 &vx, const TVector3 &vy, Double_t x)
{
    const TVector3 c = GetParab(vx, vy);
    return c(0) + c(1)*x + c(2)*x*x;
}

// --------------------------------------------------------------------------
//
//  Interpolate the points with x-coordinates vx=(-1,0,1) and
// y-coordinates vy by a parabola (second order polynomial) and return
// the value at x.
//
Double_t MMath::InterpolParabLin(const TVector3 &vy, Double_t x)
{
    const TVector3 c(vy(1), (vy(2)-vy(0))/2, vy(0)/2 - vy(1) + vy(2)/2);
    return c(0) + c(1)*x + c(2)*x*x;
}

Double_t MMath::InterpolParabLog(const TVector3 &vx, const TVector3 &vy, Double_t x)
{
    const Double_t l0 = TMath::Log10(vx(0));
    const Double_t l1 = TMath::Log10(vx(1));
    const Double_t l2 = TMath::Log10(vx(2));

    const TVector3 vx0(l0, l1, l2);
    return InterpolParabLin(vx0, vy, TMath::Log10(x));
}

Double_t MMath::InterpolParabCos(const TVector3 &vx, const TVector3 &vy, Double_t x)
{
    const Double_t l0 = TMath::Cos(vx(0));
    const Double_t l1 = TMath::Cos(vx(1));
    const Double_t l2 = TMath::Cos(vx(2));

    const TVector3 vx0(l0, l1, l2);
    return InterpolParabLin(vx0, vy, TMath::Cos(x));
}

// --------------------------------------------------------------------------
//
// Analytically calculated result of a least square fit of:
//    y = A*e^(B*x)
// Equal weights
//
// It returns TArrayD(2) = { A, B };
//
// see: http://mathworld.wolfram.com/LeastSquaresFittingExponential.html
//
TArrayD MMath::LeastSqFitExpW1(Int_t n, Double_t *x, Double_t *y)
{
    Double_t sumxsqy  = 0;
    Double_t sumylny  = 0;
    Double_t sumxy    = 0;
    Double_t sumy     = 0;
    Double_t sumxylny = 0;
    for (int i=0; i<n; i++)
    {
        sumylny  += y[i]*TMath::Log(y[i]);
        sumxy    += x[i]*y[i];
        sumxsqy  += x[i]*x[i]*y[i];
        sumxylny += x[i]*y[i]*TMath::Log(y[i]);
        sumy     += y[i];
    }

    const Double_t dev = sumy*sumxsqy - sumxy*sumxy;

    const Double_t a = (sumxsqy*sumylny - sumxy*sumxylny)/dev;
    const Double_t b = (sumy*sumxylny - sumxy*sumylny)/dev;

    TArrayD rc(2);
    rc[0] = TMath::Exp(a);
    rc[1] = b;
    return rc;
}

// --------------------------------------------------------------------------
//
// Analytically calculated result of a least square fit of:
//    y = A*e^(B*x)
// Greater weights to smaller values
//
// It returns TArrayD(2) = { A, B };
//
// see: http://mathworld.wolfram.com/LeastSquaresFittingExponential.html
//
TArrayD MMath::LeastSqFitExp(Int_t n, Double_t *x, Double_t *y)
{
    // -------- Greater weights to smaller values ---------
    Double_t sumlny  = 0;
    Double_t sumxlny = 0;
    Double_t sumxsq  = 0;
    Double_t sumx    = 0;
    for (int i=0; i<n; i++)
    {
        sumlny  += TMath::Log(y[i]);
        sumxlny += x[i]*TMath::Log(y[i]);

        sumxsq  += x[i]*x[i];
        sumx    += x[i];
    }

    const Double_t dev = n*sumxsq-sumx*sumx;

    const Double_t a = (sumlny*sumxsq - sumx*sumxlny)/dev;
    const Double_t b = (n*sumxlny - sumx*sumlny)/dev;

    TArrayD rc(2);
    rc[0] = TMath::Exp(a);
    rc[1] = b;
    return rc;
}

// --------------------------------------------------------------------------
//
// Analytically calculated result of a least square fit of:
//    y = A+B*ln(x)
//
// It returns TArrayD(2) = { A, B };
//
// see: http://mathworld.wolfram.com/LeastSquaresFittingLogarithmic.html
//
TArrayD MMath::LeastSqFitLog(Int_t n, Double_t *x, Double_t *y)
{
    Double_t sumylnx  = 0;
    Double_t sumy     = 0;
    Double_t sumlnx   = 0;
    Double_t sumlnxsq = 0;
    for (int i=0; i<n; i++)
    {
        sumylnx  += y[i]*TMath::Log(x[i]);
        sumy     += y[i];
        sumlnx   += TMath::Log(x[i]);
        sumlnxsq += TMath::Log(x[i])*TMath::Log(x[i]);
    }

    const Double_t b = (n*sumylnx-sumy*sumlnx)/(n*sumlnxsq-sumlnx*sumlnx);
    const Double_t a = (sumy-b*sumlnx)/n;

    TArrayD rc(2);
    rc[0] = a;
    rc[1] = b;
    return rc;
}

// --------------------------------------------------------------------------
//
// Analytically calculated result of a least square fit of:
//    y = A*x^B
//
// It returns TArrayD(2) = { A, B };
//
// see: http://mathworld.wolfram.com/LeastSquaresFittingPowerLaw.html
//
TArrayD MMath::LeastSqFitPowerLaw(Int_t n, Double_t *x, Double_t *y)
{
    Double_t sumlnxlny  = 0;
    Double_t sumlnx   = 0;
    Double_t sumlny    = 0;
    Double_t sumlnxsq   = 0;
    for (int i=0; i<n; i++)
    {
        sumlnxlny  += TMath::Log(x[i])*TMath::Log(y[i]);
        sumlnx     += TMath::Log(x[i]);
        sumlny     += TMath::Log(y[i]);
        sumlnxsq   += TMath::Log(x[i])*TMath::Log(x[i]);
    }

    const Double_t b = (n*sumlnxlny-sumlnx*sumlny)/(n*sumlnxsq-sumlnx*sumlnx);
    const Double_t a = (sumlny-b*sumlnx)/n;

    TArrayD rc(2);
    rc[0] = TMath::Exp(a);
    rc[1] = b;
    return rc;
}

// --------------------------------------------------------------------------
//
// Calculate the intersection of two lines defined by (x1;y1) and (x2;x2)
// Returns the intersection point.
//
// It is assumed that the lines intersect. If there is no intersection
// TVector2() is returned (which is not destinguishable from
// TVector2(0,0) if the intersection is at the coordinate source)
//
// Formula from: http://mathworld.wolfram.com/Line-LineIntersection.html
//
TVector2 MMath::GetIntersectionPoint(const TVector2 &x1, const TVector2 &y1, const TVector2 &x2, const TVector2 &y2, Bool_t &rc)
{
    TMatrix d(2,2);
    d[0][0] = x1.X()-y1.X();
    d[0][1] = x2.X()-y2.X();
    d[1][0] = x1.Y()-y1.Y();
    d[1][1] = x2.Y()-y2.Y();

    const Double_t denom = d.Determinant();
    if (denom==0)
    {
        rc = false;
        return TVector2();
    }

    rc = true;

    TMatrix l1(2,2);
    TMatrix l2(2,2);

    l1[0][0] = x1.X();
    l1[0][1] = y1.X();
    l2[0][0] = x2.X();
    l2[0][1] = y2.X();

    l1[1][0] = x1.Y();
    l1[1][1] = y1.Y();
    l2[1][0] = x2.Y();
    l2[1][1] = y2.Y();

    TMatrix a(2,2);
    a[0][0] = l1.Determinant();
    a[0][1] = l2.Determinant();
    a[1][0] = x1.X()-y1.X();
    a[1][1] = x2.X()-y2.X();

    const Double_t X = a.Determinant()/denom;

    a[1][0] = x1.Y()-y1.Y();
    a[1][1] = x2.Y()-y2.Y();

    const Double_t Y = a.Determinant()/denom;

    return TVector2(X, Y);
}

TVector2 MMath::GetIntersectionPoint(const TVector2 &x1, const TVector2 &y1, const TVector2 &x2, const TVector2 &y2)
{
    Bool_t rc;
    return GetIntersectionPoint(x1, y1, x2, y2, rc);
}

// --------------------------------------------------------------------------
//
// Solves: x^2 + ax + b = 0;
// Return number of solutions returned as x1, x2
//
Int_t MMath::SolvePol2(Double_t a, Double_t b, Double_t &x1, Double_t &x2)
{
    const Double_t r = a*a - 4*b;
    if (r<0)
        return 0;

    if (r==0)
    {
        x1 = x2 = -a/2;
        return 1;
    }

    const Double_t s = TMath::Sqrt(r);

    x1 = (-a+s)/2;
    x2 = (-a-s)/2;

    return 2;
}

// --------------------------------------------------------------------------
//
// This is a helper function making the execution of SolverPol3 a bit faster
//
static inline Double_t ReMul(const TComplex &c1, const TComplex &th)
{
    const TComplex c2 = TComplex::Cos(th/3.);
    return c1.Re() * c2.Re() - c1.Im() * c2.Im();
}

// --------------------------------------------------------------------------
//
// Solves: x^3 + ax^2 + bx + c = 0;
// Return number of the real solutions, returned as z1, z2, z3
//
// Algorithm adapted from http://home.att.net/~srschmitt/cubizen.heml
// Which is based on the solution given in
//    http://mathworld.wolfram.com/CubicEquation.html
//
// -------------------------------------------------------------------------
//
// Exact solutions of cubic polynomial equations
// by Stephen R. Schmitt Algorithm
//
// An exact solution of the cubic polynomial equation:
//
//   x^3 + a*x^2 + b*x + c = 0
//
// was first published by Gerolamo Cardano (1501-1576) in his treatise,
// Ars Magna. He did not discoverer of the solution; a professor of
// mathematics at the University of Bologna named Scipione del Ferro (ca.
// 1465-1526) is credited as the first to find an exact solution. In the
// years since, several improvements to the original solution have been
// discovered. Zeno source code
//
// http://home.att.net/~srschmitt/cubizen.html
//
// % compute real or complex roots of cubic polynomial
// function cubic( var z1, z2, z3 : real, a, b, c : real ) : real
// 
//     var Q, R, D, S, T : real
//     var im, th : real
// 
//     Q := (3*b - a^2)/9
//     R := (9*b*a - 27*c - 2*a^3)/54
//     D := R^2 -Q^3                           % polynomial discriminant
// 
//     if (D >= 0) then                        % complex or duplicate roots
// 
//         S := sgn(R + sqrt(D))*abs(R + sqrt(D))^(1/3)
//         T := sgn(R - sqrt(D))*abs(R - sqrt(D))^(1/3)
// 
//         z1 := -a/3 + (S + T)               % real root
//         z2 := -a/3 - (S + T)/2             % real part of complex root
//         z3 := -a/3 - (S + T)/2             % real part of complex root
//         im := abs(sqrt(3)*(S - T)/2)       % complex part of root pair
// 
//     else                                    % distinct real roots
// 
//         th := arccos(R/sqrt( -Q^3))
//         
//         z1 := 2*sqrt(-Q)*cos(th/3) - a/3
//         z2 := 2*sqrt(-Q)*cos((th + 2*pi)/3) - a/3
//         z3 := 2*sqrt(-Q)*cos((th + 4*pi)/3) - a/3
//         im := 0
// 
//     end if
// 
//     return im                               % imaginary part
// 
// end function
//
// see also http://en.wikipedia.org/wiki/Cubic_equation
//
Int_t MMath::SolvePol3(Double_t a, Double_t b, Double_t c,
                       Double_t &x1, Double_t &x2, Double_t &x3)
{
    //    Double_t coeff[4] = { 1, a, b, c };
    //    return TMath::RootsCubic(coeff, x1, x2, x3) ? 1 : 3;

    const Double_t Q = (a*a - 3*b)/9;
    const Double_t R = (9*b*a - 27*c - 2*a*a*a)/54;
    const Double_t D = R*R - Q*Q*Q;             // polynomial discriminant

    // ----- The single-real / duplicate-roots solution -----

    // D<0:  three real roots
    // D>0:  one real root
    // D==0: maximum two real roots (two identical roots)

    // R==0: only one unique root
    // R!=0: two roots

    if (D==0)
    {
        const Double_t r = MMath::Sqrt3(R);

        x1 = r - a/3.;               // real root
        if (R==0)
            return 1;

        x2 = 2*r - a/3.;               // real root
        return 2;
    }

    if (D>0)                                    // complex or duplicate roots
    {
        const Double_t sqrtd = TMath::Sqrt(D);

        const Double_t A = TMath::Sign(1., R)*MMath::Sqrt3(TMath::Abs(R)+sqrtd);

        // The case A==0 cannot happen. This would imply D==0
        // if (A==0)
        // {
        //     x1 = -a/3;
        //     return 1;
        // }

        x1 = (A+Q/A)-a/3;

        //const Double_t S = MMath::Sqrt3(R + sqrtd);
        //const Double_t T = MMath::Sqrt3(R - sqrtd);
        //x1 = (S+T) - a/3.;               // real root

        return 1;

        //z2 = (S + T)/2 - a/3.;            // real part of complex root
        //z3 = (S + T)/2 - a/3.;            // real part of complex root
        //im = fabs(sqrt(3)*(S - T)/2)      // complex part of root pair
    }

    // ----- The general solution with three roots ---

    if (Q==0)
        return 0;

    if (Q>0) // This is here for speed reasons
    {
        const Double_t sqrtq = TMath::Sqrt(Q);
        const Double_t rq    = R/TMath::Abs(Q);

        const Double_t t = TMath::ACos(rq/sqrtq)/3;

        static const Double_t sqrt3 = TMath::Sqrt(3.);

        const Double_t sn = TMath::Sin(t)*sqrt3;
        const Double_t cs = TMath::Cos(t);

        x1 = 2*sqrtq *       cs  - a/3;
        x2 =  -sqrtq * (sn + cs) - a/3;
        x3 =   sqrtq * (sn - cs) - a/3;

        /* --- Easier to understand but slower ---
        const Double_t th1 = TMath::ACos(rq/sqrtq);
        const Double_t th2 = th1 + TMath::TwoPi();
        const Double_t th3 = th2 + TMath::TwoPi();

        x1 = 2.*sqrtq * TMath::Cos(th1/3.) - a/3.;
        x2 = 2.*sqrtq * TMath::Cos(th2/3.) - a/3.;
        x3 = 2.*sqrtq * TMath::Cos(th3/3.) - a/3.;
        */
        return 3;
    }

    const TComplex sqrtq = TComplex::Sqrt(Q);
    const Double_t rq    = R/TMath::Abs(Q);

    const TComplex th1 = TComplex::ACos(rq/sqrtq);
    const TComplex th2 = th1 + TMath::TwoPi();
    const TComplex th3 = th2 + TMath::TwoPi();

    // For ReMul, see bove
    x1 = ReMul(2.*sqrtq, th1) - a/3.;
    x2 = ReMul(2.*sqrtq, th2) - a/3.;
    x3 = ReMul(2.*sqrtq, th3) - a/3.;

    return 3;
}

// --------------------------------------------------------------------------
//
// Format a value and its error corresponding to the rules (note
// this won't work if the error is more then eight orders smaller than
// the value)
//
void MMath::Format(Double_t &v, Double_t &e)
{
    // Valid digits
    Int_t i = TMath::FloorNint(TMath::Log10(v))-TMath::FloorNint(TMath::Log10(e));

    // Check if error starts with 1 or 2. In this case use one
    // more valid digit
    TString error = MString::Format("%.0e", e);
    if (error[0]=='1' || error[0]=='2')
    {
        i++;
        error = MString::Format("%.1e", e);
    }

    const TString fmt = MString::Format("%%.%de", i);

    v = MString::Format(fmt.Data(), v).Atof();
    e = error.Atof();
}

Double_t MMath::RndmExp(Double_t tau)
{
    // returns an exponential deviate.
    //
    //          exp( -t/tau )

    const Double_t x = gRandom->Rndm(); // uniform on ] 0, 1 ]

    return -tau * TMath::Log(x);       // convert to exponential distribution
}
