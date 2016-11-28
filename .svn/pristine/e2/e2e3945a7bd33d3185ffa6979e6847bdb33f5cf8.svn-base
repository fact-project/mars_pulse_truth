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
!   Author(s): Sebastian Raducci 01/2004  <mailto:raducci@fisica.uniud.it>
!
!   Copyright: MAGIC Software Development, 2001-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  Cubic Spline Interpolation
//
//////////////////////////////////////////////////////////////////////////////
#include "MCubicSpline.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MCubicCoeff.h"

ClassImp(MCubicSpline);

using namespace std;

//---------------------------------------------------------------------------
//
// Contructor 
//
//
MCubicSpline::MCubicSpline(const Byte_t *y, const Byte_t *x, Bool_t areAllEq,
			   Int_t n, Double_t begSD, Double_t endSD)
{
    Init(y,x,areAllEq,n,begSD,endSD);
}

//---------------------------------------------------------------------------
//
// Constructor for FADC slice (only the FADC counts are needed)
//
//
MCubicSpline::MCubicSpline(const Byte_t *y)
{
    const Byte_t x[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E};
    Init(y,x,kTRUE,15,0.0,0.0);
}

//---------------------------------------------------------------------------
//
// Constructors common part
//
//
void MCubicSpline::Init(const Byte_t *y, const Byte_t *x, Bool_t areAllEq,
			   Int_t n, Double_t begSD, Double_t endSD)

{
    Double_t *temp = new Double_t[n];
    Double_t *ysd  = new Double_t[n];

    fCoeff = new TObjArray(n-1,0);

    ysd[0]  =begSD;
    temp[0] =begSD;
    ysd[n-1]=endSD;
    
    Double_t h = x[1]-x[0];

    if (areAllEq)
    {
	for(Int_t i = 1; i < n-1; i++)
	{
	    const Double_t p = ysd[i-1]/2+2;

            ysd[i]  = -0.5/p;
	    temp[i] = (y[i+1] - y[i]*2 + y[i-1])/h;
	    temp[i] = (temp[i]*6/h-temp[i-1]/2)/p;
        }
    }
    else
    {
	for(Int_t i = 1; i < n-1; i++)
	{
            const Double_t sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);

            const Double_t p = sig*ysd[i-1]+2;

	    ysd[i]  = (sig-1.0)/p;
	    temp[i] = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
	    temp[i] = (temp[i]*6/(x[i+1]-x[i-1])-sig*temp[i-1])/p;
	}
    }

    for(Int_t i = n-2; i > 0; i--)
        ysd[i] = ysd[i]*ysd[i+1] + temp[i];

    for(Int_t i = 0; i < n-1; i++)
    {
        if (!areAllEq)
            h = x[i+1]-x[i];

        MCubicCoeff *c = new MCubicCoeff(x[i], x[i+1], y[i], y[i+1], (ysd[i+1]-ysd[i])/(h*6),
                                         ysd[i]/2, (y[i+1]-y[i])/h-(h*(ysd[i+1]+ysd[i]*2))/6);
        fCoeff->AddAt(c, i);
    }

    delete [] temp;
    delete [] ysd;
}

MCubicSpline::~MCubicSpline()
{
    fCoeff->Delete();
    delete fCoeff;
}

//---------------------------------------------------------------------------
//
// Evaluate the spline at a given point
//
Double_t MCubicSpline :: Eval(Double_t x)
{
    const Int_t n = fCoeff->GetSize();
    for (Int_t i = 0; i < n; i++)
    {
        MCubicCoeff *c = (MCubicCoeff*)fCoeff->UncheckedAt(i);
	if (c->IsIn(x))
            return c->Eval(x);
    }

    gLog << warn << "Cannot evaluate Spline at " << x << "; returning 0";

    return 0;
}

//----------------------------------------------------------------------------
//
// Search for max
//
Double_t MCubicSpline :: EvalMax()
{
    Double_t max = -FLT_MAX;

    TIter Next(fCoeff);
    MCubicCoeff *c;
    while ((c=(MCubicCoeff*)Next()))
        max = TMath::Max(max, c->GetMax());

    return max;
}

//----------------------------------------------------------------------------
//
// Search for min
//
Double_t MCubicSpline :: EvalMin()
{
    Double_t min = FLT_MAX;

    TIter Next(fCoeff);
    MCubicCoeff *c;
    while ((c=(MCubicCoeff*)Next()))
        min = TMath::Min(min, c->GetMin());

    return min;
}

//----------------------------------------------------------------------------
//
// Search for abscissa of the max
//
Double_t MCubicSpline :: EvalAbMax()
{
    Double_t max = -FLT_MAX;

    TIter Next(fCoeff);

    MCubicCoeff *c;
    MCubicCoeff *cmax=0;

    while ((c=(MCubicCoeff*)Next()))
    {
        const Double_t temp = c->GetMax();
        if (temp <= max)
            continue;

        max = temp;
        cmax = c;
    }

    return cmax ? cmax->GetAbMax() : -FLT_MAX;
}

//----------------------------------------------------------------------------
//
// Search for abscissa of the min
//
Double_t MCubicSpline :: EvalAbMin()
{
    Double_t min = FLT_MAX;

    TIter Next(fCoeff);

    MCubicCoeff *c;
    MCubicCoeff *cmin=0;

    while ((c=(MCubicCoeff*)Next()))
    {
        const Double_t temp = c->GetMin();
        if (temp >= min)
            continue;

        min = temp;
        cmin = c;
    }

    return cmin ? cmin->GetAbMin() : FLT_MAX;
}

//----------------------------------------------------------------------------
//
// Finds the abscissa where the spline reaches y starting from x0 going in
// direction direction
// You have to give as input a starting point and a direction ("l" or "r")
//
Double_t MCubicSpline :: FindVal(Double_t y, Double_t x0, Char_t direction = 'l')
{
    Double_t roots[3] = { 0, 0, 0 };

    const Int_t n = fCoeff->GetSize()-1;

    for (Int_t i = 0; i < n; i++)
    {
        if (!((MCubicCoeff*)fCoeff->At(i))->IsIn(x0))
            continue;

        switch (direction)
        {
        case 'l':
            for (Int_t j = i; j >= 0; j--)
            {
                const Int_t whichRoot = ((MCubicCoeff*)fCoeff->At(j))->FindCardanRoot(y, roots);
                if (whichRoot >= 0 )
                    return roots[whichRoot];
            }
            break;

        case 'r':
            for (Int_t j = i; j < n; j++)
            {
                const Int_t whichRoot = ((MCubicCoeff*)fCoeff->At(j))->FindCardanRoot(y, roots);
                if (whichRoot >= 0)
                    return roots[whichRoot];
            }
            break;
        }
    }

    //gLog << warn << "Nothing found calling MCubicSpline :: FindVal(), returning 0" << endl;

    return 0;
}
