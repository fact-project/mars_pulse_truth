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
#include "MCubicCoeff.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCubicCoeff);

using namespace std;

//----------------------------------------------------------------------------
//
// Constructor (The spline is: fA(x-fX)3+fB(x-fX)2+fC(x-fX)+fY
// where x is the independent variable, 2 and 3 are exponents
//
MCubicCoeff::MCubicCoeff(Double_t x, Double_t xNext, Double_t y, Double_t yNext, 
			 Double_t a, Double_t b, Double_t c) : 
    fX(x), fXNext(xNext), fA(a), fB(b), fC(c), fY(y), fYNext(yNext)
{
    fH = fXNext - fX;
    if (EvalMinMax())
        return;

    gLog << warn << "Failed to eval interval Minimum and Maximum, returning zeros" << endl;
    fMin = 0;
    fMax = 0;
}

//----------------------------------------------------------------------------
//
// Evaluate the spline at a given point
//

Double_t MCubicCoeff::Eval(Double_t x)
{
    const Double_t dx = x - fX;
    return fY + dx*(fC + dx*(fB + dx*fA));
}

//----------------------------------------------------------------------------
//
// Find min and max using derivatives. The min and max could be at the begin
// or at the end of the interval or somewhere inside the interval (in this case
// a comparison between the first derivative and zero is made)
// The first derivative coefficients are obviously: 3*fA, 2*fB, fC
//
Bool_t MCubicCoeff::EvalMinMax()
{
    fMin = fY;
    fMax = fY;

    fAbMin = fX;
    fAbMax = fX;

    if (fYNext < fMin)
    {
	fMin   = fYNext;
	fAbMin = fXNext;
    }
    if (fYNext > fMax)
    {
	fMax   = fYNext;
	fAbMax = fXNext;
    }

    const Double_t delta = fB*fB*4 - fA*fC*12;
    if (delta >= 0 && fA != 0)
    {
        const Double_t sqrtDelta = TMath::Sqrt(delta);

        const Double_t xPlus  = (-fB*2 + sqrtDelta)/(fA*6);
        const Double_t xMinus = (-fB*2 - sqrtDelta)/(fA*6);

        if (xPlus >= 0 && xPlus <= fH)
        {
            const Double_t tempMinMax = Eval(fX+xPlus);
            if (tempMinMax < fMin)
            {
                fMin = tempMinMax;
                fAbMin = fX + xPlus;
            }
            if (tempMinMax > fMax)
            {
                fMax = tempMinMax;
                fAbMax = fX + xPlus;
            }
        }
        if (xMinus >= 0 && xMinus <= fH)
        {
            const Double_t tempMinMax = Eval(fX+xMinus);
            if (tempMinMax < fMin)
            {
                fMin = tempMinMax;
                fAbMin = fX + xMinus;
            }
            if (tempMinMax > fMax)
            {
                fMax = tempMinMax;
                fAbMax = fX + xMinus;
            }
        }
        return kTRUE;
    }

    /* if fA is zero then we have only one possible solution */
    if (fA == 0 && fB != 0)
    {
        const Double_t xSolo = -fC/(fB*2);

        if (xSolo < 0 || xSolo > fH)
            return kTRUE;

        const Double_t tempMinMax = Eval(fX+xSolo);
        if (tempMinMax < fMin)
        {
            fMin = tempMinMax;
            fAbMin = fX + xSolo;
        }
        if (tempMinMax > fMax)
        {
            fMax = tempMinMax;
            fAbMax = fX + xSolo;
        }
        return kTRUE;
    }

    return kTRUE;
}
//-------------------------------------------------------------------------
//
// Given y finds x using the cubic (cardan) formula.
//
// we consider the following form: x3 + ax2 + bx + c = 0 where
//   a = fB/fA, b = fC/fA, c = (fY - y)/fA  
//
// There could be three or one real solutions
//
Short_t MCubicCoeff::FindCardanRoot(Double_t y, Double_t *x)
{
    const Double_t a = fB/fA;
    const Double_t b = fC/fA;
    const Double_t c = (fY - y)/fA;

    const Double_t q = (a*a - b*3)/9;
    const Double_t r = (a*a*a*2 - a*b*9 + c*27)/54;

    const Double_t aOver3 = a/3;
    const Double_t r2 = r*r;
    const Double_t q3 = q*q*q;
    
    if (r2 < q3) //3 real sol
    {
	const Double_t sqrtQ = TMath::Sqrt(q);
	const Double_t min2SqQ = -sqrtQ*2;
	const Double_t theta = TMath::ACos(r/(sqrtQ*sqrtQ*sqrtQ));

        x[0] = min2SqQ * TMath::Cos(theta/3) - aOver3;
	x[1] = min2SqQ * TMath::Cos((theta+TMath::TwoPi())/3) - aOver3;
	x[2] = min2SqQ * TMath::Cos((theta-TMath::TwoPi())/3) - aOver3;

        for (Int_t i = 0; i < 3; i++)
	    if (x[i] >= 0 && x[i] <= fH)
	    {
                x[i] += fX;
                return i;
	    }
        return -1;
    }

    const Double_t s = r==0 ? 0 : -TMath::Sign(TMath::Power(TMath::Abs(r) + TMath::Sqrt(r2 - q3), 1./3), r);

    x[0] = s==0 ? - aOver3 : (s + q/s) - aOver3;

    if (x[0] < 0 || x[0] > fH)
        return -1;

    x[0] += fX;
    return 0;
}

//------------------------------------------------------------------------------------
//
// return true if x is in this interval
//

Bool_t MCubicCoeff :: IsIn(Double_t x)
{
    return x >= fX && x <= fXNext;
}
