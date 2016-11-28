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
!   Author(s): Thomas Bretz  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSpline3
//
// This is a extension of TSpline3. In addition to TSpline3 it allows access
// to Xmin, Xman and Np. The construction is a bit simplified because no
// title hase to be given (it can be given later by SetTitle anyway)
// and is provides constructors which allow to scale the x-values by
// pre-defined multiplier (e.g. frequency) to create the spline.
//
//////////////////////////////////////////////////////////////////////////////
#include "MSpline3.h"

#include <TF1.h>
#include <TMath.h>

#include "MArrayD.h"

ClassImp(MSpline3);

using namespace std;

// --------------------------------------------------------------------------
//
//  Constructor.
//
MSpline3::MSpline3(const TF1 &f, const char *opt, Double_t valbeg, Double_t valend)
    : TSpline3("MSpline3", f.GetXmin(), f.GetXmax(), &f, f.GetNpx(), opt, valbeg, valend)
{
}

MSpline3::MSpline3(const TF1 &f, Double_t freq, const char *opt,Double_t valbeg, Double_t valend)
    : TSpline3("MSpline3", f.GetXmin()*freq, f.GetXmax()*freq, ConvertFunc(f, freq).GetArray(), f.GetNpx(), opt, valbeg, valend)
{
}

// --------------------------------------------------------------------------
//
//  This is a helper to convert the x-values by multiplying with freq
// before initializing the spline
//
TGraph *MSpline3::ConvertSpline(const TSpline &s, Float_t freq) const
{
    const UInt_t npx = s.GetNpx();

    // WARNING: This is a stupid workaround because the TSpline3-
    // constructor takes a pointer as input! It is not thread-safe!
    static TGraph g;
    g.Set(npx);

    for (UInt_t i=0; i<npx; i++)
    {
        Double_t x, y;
        s.GetKnot(i, x, y);
        g.SetPoint(i, x*freq, y);
    }

    return &g;
}

// --------------------------------------------------------------------------
//
//  This is a helper to convert the x-values by multiplying with freq
// before initializing the spline
//
TGraph *MSpline3::ConvertGraph(const TGraph &s, Float_t freq) const
{
    const UInt_t npx = s.GetN();

    // WARNING: This is a stupid workaround because the TSpline3-
    // constructor takes a pointer as input! It is not thread-safe!
    static TGraph g;
    g.Set(npx);

    for (UInt_t i=0; i<npx; i++)
    {
        Double_t x, y;
        s.GetPoint(i, x, y);
        g.SetPoint(i, x*freq, y);
    }

    return &g;
}

// --------------------------------------------------------------------------
//
//  This is a helper to convert the x-values by multiplying with freq
// before initializing the spline. The conversion from the function to
// a discrete binning is done similar to the constructor of TSpline
//
MArrayD &MSpline3::ConvertFunc(const TF1 &f, Float_t freq) const
{
    const UInt_t npx = f.GetNpx();

    // WARNING: This is a stupid workaround because the TSpline3-
    // constructor takes a pointer as input! It is not thread-safe!
    static MArrayD g;
    g.Set(npx);

    const Double_t step = (f.GetXmax()-f.GetXmin())/(npx-1);

    for (UInt_t i=0; i<npx; ++i)
    {
        const Double_t x = f.GetXmin() + i*step;
        g[i] = f.Eval(x);
    }

    return g;
}

// --------------------------------------------------------------------------
//
// Return the integral in the splines bin i up to x.
//
// The TSpline3 in the Interval [fX[i], fX[i+1]] is defined as:
//
//      dx = x-fX[i]
//      y = fY + dx*fB + dx*dx*fC + dx*dx*dx*fD
//
// This yields the integral:
//
//   int(y) = dx*fY + 1/2*dx*dx*fB + 1/3*dx*dx*dx*fC + 1/4*dx*dx*dx*dx*fD
//          = dx*(fY + dx*(1/2*fB + dx*(1/3*fC + dx*(1/4*fD))))
//
// Which gives for the integral range [fX[i], fX[i]+w]:
//   int(fX[i]+w)-int(fX[i]) = w*(fY + w*(1/2*fB + w*(1/3*fC + w*(1/4*fD))))
//
// and for the integral range [fX[i]+w, fX[i+1]]:
//   int(fX[i+1])-int(fX[i]+w) = `
//     W*(fY + W*(1/2*fB + W*(1/3*fC + W*(1/4*fD)))) -
//     w*(fY + w*(1/2*fB + w*(1/3*fC + w*(1/4*fD))))
// with
//     W := fX[i+1]-fX[i]
//
Double_t MSpline3::IntegralBin(Int_t i, Double_t x) const
{
    Double_t x0, y, b, c, d;
    const_cast<MSpline3*>(this)->GetCoeff(i, x0, y, b, c, d);

    const Double_t w = x-x0;

    return w*(y + w*(b/2 + w*(c/3 + w*d/4)));
}

// --------------------------------------------------------------------------
//
// Return the integral of the spline's bin i.
//
Double_t MSpline3::IntegralBin(Int_t i) const
{
    Double_t x, y;

    GetKnot(i+1, x, y);

    return IntegralBin(i, x);
}

// --------------------------------------------------------------------------
//
// Return the integral from a to b
//
Double_t MSpline3::Integral(Double_t a, Double_t b) const
{
    const Int_t n = FindX(a);
    const Int_t m = FindX(b);

    Double_t sum = -IntegralBin(n, a);

    for (int i=n; i<=m-1; i++)
        sum += IntegralBin(i);

    sum += IntegralBin(m, b);

    return sum;
}

// --------------------------------------------------------------------------
//
// Return the integral between Xmin and Xmax
//
Double_t MSpline3::Integral() const
{
    Double_t sum = 0;

    for (int i=0; i<GetNp()-1; i++)
        sum += IntegralBin(i);

    return sum;
}

// --------------------------------------------------------------------------
//
// Return the integral between Xmin and Xmax of int( f(x)*sin(x) )
//
// The x-axis is assumed to be in degrees
//
Double_t MSpline3::IntegralSolidAngle() const
{
    const Int_t n = GetNp();

    MArrayD x(n);
    MArrayD y(n);

    for (int i=0; i<n; i++)
    {
        GetKnot(i, x[i], y[i]);

        x[i] *= TMath::DegToRad();
        y[i] *= TMath::Sin(x[i]);
    }

    return TMath::TwoPi()*MSpline3(x.GetArray(), y.GetArray(), n).Integral();
}


// FIXME: As soon as TSpline3 allows access to fPoly we can implement
//        a much faster evaluation of the spline, especially in
//        special conditions like in MAnalogSignal::AddPulse
//        This will be the case for root > 5.22/00

/*
Double_t MSpline3::EvalFast(Double_t x) const
{
    // Eval this spline at x
    const Int_t klow=FindFast(x);
    return fPoly[klow].Eval(x);
}

Int_t MSpline3::FindFast(Double_t x) const
{
    //
    // If out of boundaries, extrapolate
    // It may be badly wrong

    // if (x<=fXmin)
    //     return 0;
    //
    // if (x>=fXmax)
    //     return fNp-1;

    //
    // Equidistant knots, use histogramming
    if (fKstep)
        return TMath::Min(Int_t((x-fXmin)/fDelta),fNp-1);

    //
    // Non equidistant knots, binary search
    Int_t klow = 0;
    Int_t khig = fNp-1;

    Int_t khalf;
    while (khig-klow>1)
        if(x>fPoly[khalf=(klow+khig)/2].X())
            klow=khalf;
        else
            khig=khalf;

    // This could be removed, sanity check
    //if(!(fPoly[klow].X()<=x && x<=fPoly[klow+1].X()))
    //    Error("Eval",
    //          "Binary search failed x(%d) = %f < %f < x(%d) = %f\n",
    //          klow,fPoly[klow].X(),x,fPoly[klow+1].X());

    return klow;
}
*/
