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
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MPulseShape
//
// This container describes the pulse shape of a pulse.
//
//////////////////////////////////////////////////////////////////////////////
#include "MPulseShape.h"

#include <TF1.h>
#include <TH1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MSpline3.h"

ClassImp(MPulseShape);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MPulseShape::MPulseShape(const char* name, const char *title)
: fSpline(0), fFunction("exp(-(x/2)^2/2)"), fNpx(25), fXmin(-8), fXmax(8)
{
    fName  = name  ? name  : "MPulseShape";
    fTitle = title ? title : "";

    SetFunction(fFunction, fNpx, fXmin, fXmax);
}

// --------------------------------------------------------------------------
//
//  Call Clear()
//
MPulseShape::~MPulseShape()
{
    Clear();
}

// --------------------------------------------------------------------------
//
//  Delete fSpline if set and set it to 0
//
void MPulseShape::Clear(Option_t *)
{
    if (fSpline)
        delete fSpline;
    fSpline=0;
}

// --------------------------------------------------------------------------
//
//  Return the width of the range in which the spline is defined.
//
Float_t MPulseShape::GetPulseWidth() const
{
    return fSpline ? fSpline->GetXmax()-fSpline->GetXmin() : 0;
}

// --------------------------------------------------------------------------
//
//  Return the lower edge of the range in which the spline is defined.
//
Float_t MPulseShape::GetXmin() const
{
    return fSpline ? fSpline->GetXmin() : 0;
}

// --------------------------------------------------------------------------
//
//  Return the upper edge of the range in which the spline is defined.
//
Float_t MPulseShape::GetXmax() const
{
    return fSpline ? fSpline->GetXmax() : 0;
}

// --------------------------------------------------------------------------
//
//  Read the intended pulse shape from a file and initialize the spline
// accordingly
//
Bool_t MPulseShape::ReadFile(const char *fname)
{
    if (fname)
        fFileName = fname;

    *fLog << inf << "Reading pulse shape from " << fFileName << endl;

    const TGraph g(fFileName);
    if (g.GetN()==0)
    {
        *fLog << err << "ERROR - No data points from " << fFileName << "." << endl;
        return kFALSE;
    }

    // option: b1/e1 b2/e2   (first second derivative?)
    // option: valbeg/valend (first second derivative?)

    Clear();
    fSpline = new MSpline3(g);//, fRunHeader->GetFreqSampling()/1000.);
    fSpline->SetTitle("MPulseShape");

    // FIXME: Make a boundary condition e1b1,0,0 (First der, at Xmin and Xmax==0)
    // FIXME: Force the spline to be 0 at Xmin and Xmax?

    return kTRUE;
}

void MPulseShape::SetFunction(const TF1 &f)
{
    // FIXME: Use TF1 directly? (In most cases this seems to be slower)

    // option: b1/e1 b2/e2   (first second derivative?)
    // option: valbeg/valend (first second derivative?)

    // if (f.GetNpar()==0)
    // No SUPPORT

    Clear();
    fSpline = new MSpline3(f);//, fRunHeader->GetFreqSampling()/1000.);
    fSpline->SetTitle("MPulseShape");

    // FIXME: Make a boundary condition e1b1,0,0 (First der, at Xmin and Xmax==0)
    // FIXME: Force the spline to be 0 at Xmin and Xmax?

    fFunction = f.GetTitle();
}

Bool_t MPulseShape::SetFunction(const char *func, Int_t n, Double_t xmin, Double_t xmax)
{
    // FIXME: Use TF1 directly? (In most cases this seems to be slower)
    TF1 f("f", func, xmin, xmax);
    f.SetNpx(n);

    SetFunction(f);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// FileName: pulse-shape.txt
// Function.Name: gaus
// Function.Npx:    50
// Function.Xmin:  -5
// Function.Xmax:   5
//
Int_t MPulseShape::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        SetFileName(GetEnvValue(env, prefix, "FileName", fFileName));
        if (!fFileName.IsNull())
            if (ReadFile(fFileName))
                return kERROR;
    }

    if (IsEnvDefined(env, prefix, "Function.Name", print))
    {
        rc = kTRUE;

        if (IsEnvDefined(env, prefix, "Function.Npx", print))
            fNpx = GetEnvValue(env, prefix, "Function.Npx", fNpx);
        if (IsEnvDefined(env, prefix, "Function.Xmin", print))
            fXmin = GetEnvValue(env, prefix, "Function.Xmin", fXmin);
        if (IsEnvDefined(env, prefix, "Function.Xmax", print))
            fXmax = GetEnvValue(env, prefix, "Function.Xmax", fXmax);

        SetFunction(GetEnvValue(env, prefix, "Function.Name", fFunction), fNpx, fXmin, fXmax);
    }

    return rc;
}

void MPulseShape::Paint(Option_t *)
{
    fSpline->SetMarkerStyle(kFullDotMedium);

    if (!fSpline->GetHistogram())
        fSpline->Paint();

    TH1 *h = fSpline->GetHistogram();
    if (!h)
        return;

    h->SetXTitle("t [ns]");
    h->SetYTitle("a.u.");

    fSpline->Paint("PC");
}
