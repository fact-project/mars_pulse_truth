/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  4/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MParSpline
//
//  Parameter container to store a MParSpline
//
//////////////////////////////////////////////////////////////////////////////
#include "MParSpline.h"

#include <TF1.h>  // MParSpline

#include "MLog.h"
#include "MLogManip.h"

#include "MSpline3.h"

ClassImp(MParSpline);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MParSpline::MParSpline(const char *name, const char *title) : fSpline(0)
{
    fName  = name  ? name  : "MParSpline";
    fTitle = title ? title : "Parameter container to store a MSpline3";
}

// --------------------------------------------------------------------------
//
// Return Xmin of the spline. 0 if fSpline==NULL
//
Double_t MParSpline::GetXmin() const
{
    return fSpline ? fSpline->GetXmin() : 0;
}

// --------------------------------------------------------------------------
//
// Return Xmax of the spline. 0 if fSpline==NULL
//
Double_t MParSpline::GetXmax() const
{
    return fSpline ? fSpline->GetXmax() : 0;
}

// --------------------------------------------------------------------------
//
//  Return the width of the range in which the spline is defined.
//
Double_t MParSpline::GetWidth() const
{
    return fSpline ? fSpline->GetXmax()-fSpline->GetXmin() : 0;
}

// --------------------------------------------------------------------------
//
// Return the result of the spline at val.
// The user is reposible to make sure that fSpline is valid (!=NULL)
//
Double_t MParSpline::Eval(Double_t val) const
{
    return fSpline->Eval(val);
}

// --------------------------------------------------------------------------
//
// Delete fSpline and set to 0.
//
void MParSpline::Clear(Option_t *)
{
    if (fSpline)
        delete fSpline;
    fSpline=0;
}

// --------------------------------------------------------------------------
//
// Read a TGraph from a file and return a newly allocated MSpline3.
//
MSpline3 *MParSpline::ReadSpline(const char *fname) const
{
    *fLog << inf << "Reading spline from " << fname << endl;

    TGraph g(fname);
    if (g.GetN()==0)
    {
        *fLog << err << "ERROR - No data points from " << fname << "." << endl;
        return 0;
    }

    // option: b1/e1 b2/e2   (first second derivative?)
    // option: valbeg/valend (first second derivative?)

    MSpline3 *s = new MSpline3(g);
    s->SetTitle(fname);

    return s;
}

// --------------------------------------------------------------------------
//
// Initializes a spline from min to max with n points with 1.
//
/*
void MParSpline::InitUnity(UInt_t n, Float_t min, Float_t max)
{
    // Delete the existing spline
    Clear();

    // We need at least two points (the edges)
    if (n<2)
        return;

    // Initialize an array with the x-values
    const MBinning bins(n-1, min, max);

    // Initialize an array with all one
    MArrayD y(n);
    y.Reset(1);

    // Set the spline
    fSpline = new MSpline3(bins.GetEdges(), y.GetArray(), n);
}
*/

// --------------------------------------------------------------------------
//
// Takes the existing fSpline and multiplies it with the given spline.
// As x-points the values from fSpline are used.
//
void MParSpline::Multiply(const TSpline3 &spline)
{
    if (!fSpline)
    {
        Clear();
        // WARNING WARNING WARNING: This is a very dangerous cast!
        fSpline = (MSpline3*)spline.Clone();
        return;
    }

    // Initialize a TGraph with the number of knots from a TSpline
    TGraph g(fSpline->GetNp());

    // Loop over all knots
    for (int i=0; i<fSpline->GetNp(); i++)
    {
        // Get th i-th knot
        Double_t x, y;
        fSpline->GetKnot(i, x, y);

        // Multiply y by the value from the given spline
        y *= spline.Eval(x);

        // push the point "back"
        g.SetPoint(i, x, y);
    }

    // Clear the spline and initialize a new one from the updated TGraph
    Clear();
    fSpline = new MSpline3(g);
}

// --------------------------------------------------------------------------
//
// Initializes a TSpline3 with n knots x and y. Call Multiply for it.
//
void MParSpline::Multiply(UInt_t n, const Double_t *x, const Double_t *y)
{
    const TSpline3 spline("Spline", const_cast<Double_t*>(x), const_cast<Double_t*>(y), n);
    Multiply(spline);
}

// --------------------------------------------------------------------------
//
// Read a Spline from a file using ReadSpline.
// Call Multiply for it.
//
void MParSpline::Multiply(const char *fname)
{
    const MSpline3 *spline = ReadSpline(fname);
    if (!spline)
        return;

    Multiply(*spline);

    delete spline;
}

// --------------------------------------------------------------------------
//
// Read a spline from a file and set fSpline accfordingly.
//
Bool_t MParSpline::ReadFile(const char *fname)
{
    MSpline3 *spline = ReadSpline(fname);
    if (!spline)
        return kFALSE;


    // option: b1/e1 b2/e2   (first second derivative?)
    // option: valbeg/valend (first second derivative?)

    Clear();
    fSpline = spline;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the spline to a function. For details see MSpline3
//
void MParSpline::SetFunction(const TF1 &f)
{
    // FIXME: Use TF1 directly? (In most cases this seems to be slower)

    // option: b1/e1 b2/e2   (first second derivative?)
    // option: valbeg/valend (first second derivative?)

    // if (f.GetNpar()==0)
    // No SUPPORT

    Clear();
    fSpline = new MSpline3(f);//, fRunHeader->GetFreqSampling()/1000.);
    fSpline->SetTitle(f.GetTitle());

    // FIXME: Make a boundary condition e1b1,0,0 (First der, at Xmin and Xmax==0)
    // FIXME: Force the spline to be 0 at Xmin and Xmax?
}

Bool_t MParSpline::SetFunction(const char *func, Int_t n, Double_t xmin, Double_t xmax)
{
    // FIXME: Use TF1 directly? (In most cases this seems to be slower)
    TF1 f("f", func, xmin, xmax);
    f.SetNpx(n);

    SetFunction(f);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// FileName: reflectivity.txt
// Function.Name: exp(-(x/2)^2/2)
// Function.Npx:   25
// Function.Xmin:  -8
// Function.Xmax:   8
//
Int_t MParSpline::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        if (!ReadFile(GetEnvValue(env, prefix, "FileName", "")))
            return kERROR;
    }

    if (IsEnvDefined(env, prefix, "Function.Name", print))
    {
        rc = kTRUE;

        Int_t   npx  = 25;
        Float_t xmin = -8;
        Float_t xmax =  8;
        TString func = "exp(-(x/2)^2/2)";

        if (IsEnvDefined(env, prefix, "Function.Npx", print))
            npx = GetEnvValue(env, prefix, "Function.Npx", npx);
        if (IsEnvDefined(env, prefix, "Function.Xmin", print))
            xmin = GetEnvValue(env, prefix, "Function.Xmin", xmin);
        if (IsEnvDefined(env, prefix, "Function.Xmax", print))
            xmax = GetEnvValue(env, prefix, "Function.Xmax", xmax);

        SetFunction(GetEnvValue(env, prefix, "Function.Name", func), npx, xmin, xmax);
    }
    return rc;
}

void MParSpline::Paint(Option_t *)
{
    fSpline->SetMarkerStyle(kFullDotMedium);

    if (!fSpline->GetHistogram())
        fSpline->Paint();

    TH1 *h = fSpline->GetHistogram();
    if (!h)
        return;

    //h->SetXTitle("t [ns]");
    //h->SetYTitle("a.u.");

    fSpline->Paint("PC");
}

void MParSpline::RecursiveRemove(TObject *obj)
{
    if (obj==fSpline)
        fSpline=0;

    MParContainer::RecursiveRemove(obj);
}
