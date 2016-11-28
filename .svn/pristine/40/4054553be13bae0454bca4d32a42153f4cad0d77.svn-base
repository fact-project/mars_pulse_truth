/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analyzing Data of imaging Cerenkov telescopes.
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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzbrug.de>
!   Author(s): Markus Gaug 09/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2002-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtralgoSpline
//
//   Fast Spline extractor using a cubic spline algorithm, adapted from 
//   Numerical Recipes in C++, 2nd edition, pp. 116-119.
//   
//   The coefficients "ya" are here denoted as "fVal" corresponding to
//   the FADC value subtracted by the clock-noise corrected pedestal.
//
//   The coefficients "y2a" get immediately divided 6. and are called here 
//   fDer2 although they are now not exactly the second derivative
//   coefficients any more.
// 
//   The calculation of the cubic-spline interpolated value "y" on a point 
//   "x" along the FADC-slices axis becomes: EvalAt(x)
//
//   The coefficients fDer2 are calculated with the simplified
//   algorithm in InitDerivatives.
//
//   This algorithm takes advantage of the fact that the x-values are all
//   separated by exactly 1 which simplifies the Numerical Recipes algorithm.
//   (Note that the variables fDer are not real first derivative coefficients.)
//
//   Note, this spline is not optimized to be evaluated many many times, but
//   it is optimized to be initialized very fast with new values again and
//   again.
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtralgoSpline.h"

#include <iostream>
#include <TRandom.h>

#include "../mbase/MMath.h"
#include "../mbase/MArrayF.h"

using namespace std;

// --------------------------------------------------------------------------
//
// Calculate the first and second derivative for the splie.
//
// The coefficients are calculated such that
//   1) fVal[i] = Eval(i, 0)
//   2) Eval(i-1, 1)==Eval(i, 0)
//
// In other words: The values with the index i describe the spline
// between fVal[i] and fVal[i+1]
//
void MExtralgoSpline::InitDerivatives() const
{
    if (fNum<2)
        return;

    // Look up table for coefficients
    static MArrayF lut;

    // If the lut is not yet large enough: resize and reclaculate
    if (fNum>(Int_t)lut.GetSize())
    {
        lut.Set(fNum);

        lut[0] = 0.;
        for (Int_t i=1; i<fNum-1; i++)
            lut[i] = -1.0/(lut[i-1] + 4);
    }

    // Calculate the coefficients used to get reproduce the first and
    // second derivative.
    fDer1[0] = 0.;
    for (Int_t i=1; i<fNum-1; i++)
    {
        const Float_t d1 = fVal[i+1] - 2*fVal[i] + fVal[i-1];
        fDer1[i] = (fDer1[i-1]-d1)*lut[i];
    }

    fDer2[fNum-1] = 0.;
    for (Int_t k=fNum-2; k>=0; k--)
        fDer2[k] = lut[k]*fDer2[k+1] + fDer1[k];
}

// --------------------------------------------------------------------------
//
// Return the two results x1 and x2 of f'(x)=0 for the third order
// polynomial (spline) in the interval i. Return the number of results.
// (0 if the fist derivative does not have a null-point)
//
Int_t MExtralgoSpline::EvalDerivEq0(const Int_t i, Double_t &x1, Double_t &x2) const
{
    const Double_t difder = fDer2[i+1]-fDer2[i];
    const Double_t difval = fVal[i+1] -fVal[i];

    return MMath::SolvePol2(3*difder, 6*fDer2[i], difval-2*fDer2[i]-fDer2[i+1], x1, x2);
}

// --------------------------------------------------------------------------
//
// Solve the polynomial
//
//    y = a*x^3 + b*x^2 + c*x + d'
//    0 = a*x^3 + b*x^2 + c*x + d' - y
//
// to find y in the i-th bin. Return the result as x1, x2, x3 and the return
// code from MMath::SolvPol3.
//
Int_t MExtralgoSpline::SolvePol3(Int_t i, Double_t y, Double_t &x1, Double_t &x2, Double_t &x3) const
{
    // y = a*x^3 + b*x^2 + c*x + d'
    // 0 = a*x^3 + b*x^2 + c*x + d' - y

    // Calculate coefficients
    const Double_t a = fDer2[i+1]-fDer2[i];
    const Double_t b = 3*fDer2[i];
    const Double_t c = fVal[i+1]-fVal[i] -2*fDer2[i]-fDer2[i+1];
    const Double_t d = fVal[i] - y;

    // If the first derivative is nowhere==0 and it is increasing
    // in one point, and the value we search is outside of the
    // y-interval... it cannot be there
    // if (c>0 && (d>0 || fVal[i+1]<y) && b*b<3*c*a)
    //     return -2;

    return MMath::SolvePol3(a, b, c, d, x1, x2, x3);
}

// --------------------------------------------------------------------------
//
// Returns the highest x value in [min;max[ at which the spline in
// the bin i is equal to y
//
// min and max must be in the range [0;1]
//
// The default for min is 0, the default for max is 1
// The default for y is 0
//
Double_t MExtralgoSpline::FindYdn(Int_t i, Double_t y, Double_t min, Double_t max) const
{
    Double_t x1, x2, x3;
    const Int_t rc = SolvePol3(i, y, x1, x2, x3);

    Double_t x = -1;

    if (rc>0 && x1>=min && x1<max && x1>x)
        x = x1;
    if (rc>1 && x2>=min && x2<max && x2>x)
        x = x2;
    if (rc>2 && x3>=min && x3<max && x3>x)
        x = x3;

    return x<0 ? -2 : x+i;
}

// --------------------------------------------------------------------------
//
// Returns the lowest x value in [min;max[ at which the spline in
// the bin i is equal to y
//
// min and max must be in the range [0;1]
//
// The default for min is 0, the default for max is 1
// The default for y is 0
//
Double_t MExtralgoSpline::FindYup(Int_t i, Double_t y, Double_t min, Double_t max) const
{
    Double_t x1, x2, x3;
    const Int_t rc = SolvePol3(i, y, x1, x2, x3);

    Double_t x = 2;

    if (rc>0 && x1>min && x1<=max && x1<x)
        x = x1;
    if (rc>1 && x2>min && x2<=max && x2<x)
        x = x2;
    if (rc>2 && x3>min && x3<=max && x3<x)
        x = x3;

    return x>1 ? -2 : x+i;
}

// --------------------------------------------------------------------------
//
// Search analytically downward for the value y of the spline, starting
// at x, until x==0. If y is not found or out of range -2 is returned.
//
Double_t MExtralgoSpline::SearchYdn(Double_t x, Double_t y) const
{
    if (x>=fNum-1)
        x = fNum-1.0001;

    Int_t i = TMath::FloorNint(x);
    if (i<0)
        return -3;

    Double_t rc = FindYdn(i, y, 0, x-i);
    while (--i>=0 && rc<0)
        rc = FindYdn(i, y);

    return rc;
}

// --------------------------------------------------------------------------
//
// Search analytically upwards for the value y of the spline, starting
// at x, until x==fNum-1. If y is not found or out of range -2 is returned.
//
Double_t MExtralgoSpline::SearchYup(Double_t x, Double_t y) const
{
    if (x<0)
        x = 0.0001;

    Int_t i = TMath::FloorNint(x);
    if (i>fNum-2)
        return -3;

    Double_t rc = FindYup(i, y, x-i, 1.);
    while (++i<fNum-1 && rc<0)
        rc = FindYup(i, y);

    return rc;
}

// --------------------------------------------------------------------------
//
// Do a range check an then calculate the integral from start-fRiseTime
// to start+fFallTime. An extrapolation of 0.5 slices is allowed.
//
Float_t MExtralgoSpline::CalcIntegral(Float_t pos) const
{
    // We allow extrapolation of 1/2 slice.
    const Float_t min = fRiseTime;        //-0.5+fRiseTime;
    const Float_t max = fNum-1-fFallTime; //fNum-0.5+fFallTime;

    if (pos<min)
        pos = min;
    if (pos>max)
        pos = max;

    return EvalInteg(pos-fRiseTime, pos+fFallTime);
}

Float_t MExtralgoSpline::CalcIntegral(Float_t beg, Float_t width) const
{
    Float_t end = beg + width;

    if (beg<0)
    {
        end -= beg;
        beg = 0;
    }

    if (end>fNum-1)
    {
        beg -= (end-fNum-1);
        end  = fNum-1;
    }

    return EvalInteg(beg, end);
}

MArrayF MExtralgoSpline::GetIntegral(bool norm) const
{
    MArrayF val(fNum);

    //val[0] = 0;

    Double_t integ = 0;
    for (int i=0; i<fNum-1; i++)
    {
        integ += EvalInteg(i);

        val[i+1] = integ;
    }

    if (norm)
        for (int i=0; i<fNum-1; i++)
            val[i+1] /= val[fNum-1];

    return val;
}

Float_t MExtralgoSpline::ExtractNoise()
{
    if (fNum<5)
        return 0;

    if (!(fExtractionType&kIntegral))
    {
        const Int_t   pos = gRandom->Integer(fNum-1);
        const Float_t nsx = gRandom->Uniform();
        return Eval(pos, nsx);
    }
    else
    {
        const Float_t pos = gRandom->Uniform(fNum-1-fRiseTime-fFallTime)+fRiseTime;
        return CalcIntegral(pos);
    }
}

void MExtralgoSpline::Extract(Int_t maxbin, Bool_t width)
{
    fSignal    =  0;
    fTime      =  0;
    fWidth     =  0;
    fSignalDev = -1;
    fTimeDev   = -1;
    fWidthDev  = -1;

    if (fNum<2)
        return;

    Float_t maxpos;
    // FIXME: Check the default if no maximum found!!!
    GetMaxAroundI(maxbin, maxpos, fHeight);

    const Float_t h = (fExtractionType&kTimeRel) ? fHeight*fHeightTm : fHeightTm;

    switch (fExtractionType)
    {
    case kAmplitude:
        fSignal    = fHeight;
        fTime      = maxpos;
        fSignalDev = 0;  // means: is valid
        fTimeDev   = 0;
        break;

    case kAmplitudeAbs:
    case kAmplitudeRel:
        fSignal    = fHeight;
        fTime      = SearchYdn(maxpos, h);
        fSignalDev = 0;  // means: is valid
        fTimeDev   = 0;
        break;

    case kIntegralAbs:
    case kIntegralRel:
        fSignal    = CalcIntegral(maxpos);
        fTime      = SearchYdn(maxpos, h);
        fSignalDev = 0;  // means: is valid
        fTimeDev   = 0;
        break;

    case kIntegralDyn:
        fTime      = SearchYdn(maxpos, h);
        fWidth     = SearchYup(maxpos, h)-fTime;
        fSignal    = CalcIntegral(fTime, fWidth);
        fTimeDev   = 0;
        fWidthDev  = 0;
        fSignalDev = 0;
        break;

    case kIntegralFixed:
        fTime      = SearchYdn(maxpos, h);
        fSignal    = CalcIntegral(fTime-fRiseTime, fRiseTime+fFallTime);
        fTimeDev   = 0;
        fSignalDev = 0;
        break;
    }
}
