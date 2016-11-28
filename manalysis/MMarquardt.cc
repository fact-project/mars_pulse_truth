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
!   Author(s): Wolfgang Wittek 10/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MMarquardt                                                              //
//                                                                         //
// Marquardt method of solving nonlinear least-squares problems            //
//                                                                         //
// (see Numerical recipes (2nd ed.), W.H.Press et al., p.688 ff)           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MMarquardt.h"

#include <math.h>            // fabs 

#include <TVectorD.h>
#include <TMatrixD.h>

#include <TStopwatch.h>

#include "MLog.h"
#include "MLogManip.h"
#include "MParContainer.h"

ClassImp(MMarquardt);

using namespace std;


// --------------------------------------------------------------------------
//
// Default constructor.
//
MMarquardt::MMarquardt(const char *name, const char *title)
{
    fName  = name  ? name  : "MMarquardt";
    fTitle = title ? title : "Marquardt minimization";
}

// -----------------------------------------------------------------------
//
// Set - the number of parameters
//     - the maximum number of steps allowed in the minimization and
//     - the change in chi2 signaling convergence

void MMarquardt::SetNpar(Int_t numpar, Int_t numstepmax, Double_t loopchi2)
{
  fNpar       = numpar;
  fNumStepMax = numstepmax;
  fLoopChi2   = loopchi2;

  fdParam.ResizeTo(fNpar);

  fParam.ResizeTo(fNpar);
  fGrad.ResizeTo(fNpar);
  fCovar.ResizeTo(fNpar, fNpar);  

  fmyParam.ResizeTo(fNpar);
  fmyGrad.ResizeTo(fNpar);
  fmyCovar.ResizeTo(fNpar, fNpar);  

  fIxc.ResizeTo(fNpar);
  fIxr.ResizeTo(fNpar);
  fIp.ResizeTo(fNpar);
}


// -----------------------------------------------------------------------
//
// do the minimization
//
// fcn    is the function which calculates for a given set of parameters
//        - the function L to be minimized
//        - beta_k  = -1/2 * dL/da_k          (a kind of gradient of L)
//        - alfa_kl =  1/2 * dL/(da_k da_l)   (a kind of 2nd derivative of L)
//                              
// Vinit  contains the starting values of the parameters
//

Int_t MMarquardt::Loop( 
          Bool_t (*fcn)(TVectorD &, TMatrixD &, TVectorD &, Double_t &),
          TVectorD &Vinit)
{
  fFunc = fcn; 

  // set the initial parameter values
  for (Int_t i=0; i<fNpar; i++)
    fParam(i) = Vinit(i);

  //-------------------------------------------
  // first call of the function func
  Bool_t rcfirst = FirstCall();
  if (!rcfirst)
  {
    *fLog << "MMarquardt::Loop; first call of function failed " << endl;
    return -1;
  }

  Double_t oldChi2  = fChi2;
  Double_t fdChi2   = 1.e10;
  Int_t    fNumStep = 0;

  //-------------------------------------------
  // do the next step in the minimization
  Bool_t rcnext;
  do
  {
    fNumStep++;

    rcnext = NextStep();
    if (!rcnext) break;

    fdChi2 = fabs(oldChi2-fChi2);
    oldChi2 = fChi2;
  } while (fdChi2 > fLoopChi2  &&  fNumStep < fNumStepMax);

  //-------------------------------------------
  // do the final calculations
  if (!rcnext)
  {
    *fLog << "MMarquardt::Loop; function call failed in step " << fNumStep
          << endl;
    return -2;
  }

  if (fdChi2 > fLoopChi2)
  {
    *fLog << "MMarquardt::Loop; minimization has not converged, fChi2, fdChi2 = "
          << fChi2 << ",  " << fdChi2 << endl;
    return -3;
  }

  *fLog << "MMarquardt::Loop; minimization has converged, fChi2, fdChi2, fNumStep = "
        << fChi2 << ",  " << fdChi2 << ",  " << fNumStep << endl;


  Bool_t rccov = CovMatrix();
  if (!rccov)
  {
    *fLog << "MMarquardt::Loop; calculation of covariance matrix failed " 
          << endl;
    return 1;
  }

  //-------------------------------------------
  // results

  *fLog << "MMarquardt::Loop; Results of fit : fChi2, fNumStep, fdChi2 =" 
        << fChi2 << ",  " << fNumStep << ",  " << fdChi2 << endl;

  for (Int_t i=0; i<fNpar; i++)
    fdParam(i) = sqrt(fCovar(i,i));

  *fLog << "MMarquardt::Loop;   i, Param(i), dParam(i)" << endl;
  for (Int_t i=0; i<fNpar; i++)
  {
    *fLog << i << "   " << fParam(i) << "   " << fdParam(i) << endl;
  }

  *fLog << "MMarquardt::Loop; Covariance matrix" << endl;
  for (Int_t i=0; i<fNpar; i++)
  {
    *fLog << i;
    for (Int_t j=0; j<fNpar; j++)
    {
      *fLog << fCovar(i,j) << "   ";
    }
    *fLog << endl;
  }

  return 0;
}


// -----------------------------------------------------------------------
//
// do 1st step of the minimization
//

Bool_t MMarquardt::FirstCall()
{
  fLambda = 0.001;
  Bool_t rc = (*fFunc)(fParam, fCovar, fGrad, fChi2);
  if (!rc) return kFALSE;

  fCHIq = fChi2;
  for (Int_t j=0; j<fNpar; j++)
    fmyParam(j) = fParam(j);    

  return kTRUE;
}


// -----------------------------------------------------------------------
//
// do one step of the minimization
//

Bool_t MMarquardt::NextStep()
{
  for (Int_t j=0; j<fNpar; j++)
  {
    for (Int_t k=0; k<fNpar; k++)
      fmyCovar(j,k) = fCovar(j,k);

    fmyCovar(j,j) *= (1.0 + fLambda);
    fmyGrad(j) = fGrad(j);
  }

  Bool_t rgj = GaussJordan(fNpar, fmyCovar, fmyGrad);
  if(!rgj) return kFALSE;

  for (Int_t j=0; j<fNpar; j++)
    fmyParam(j) = fParam(j) + fmyGrad(j);

  Bool_t rc = (*fFunc)(fmyParam, fmyCovar, fmyGrad, fChi2);
  if(!rc) return kFALSE;

  if (fChi2 < fCHIq)
  {
    fLambda *= 0.1;
    fCHIq = fChi2;

    for (Int_t j=0; j<fNpar; j++)
    {
      for (Int_t k=0; k<fNpar; k++)
        fCovar(j,k) = fmyCovar(j,k);

      fGrad(j)  = fmyGrad(j);
      fParam(j) = fmyParam(j);
    }
  }
    else
      fLambda *= 10.0;


  return kTRUE;
}

// -----------------------------------------------------------------------
//
// calculate error matrix of fitted parameters
//

Bool_t MMarquardt::CovMatrix()
{
  Bool_t rc = (*fFunc)(fParam, fCovar, fGrad, fChi2);
  if(!rc) return kFALSE;

  for (Int_t j=0; j<fNpar; j++)
  {
    for (Int_t k=0; k<fNpar; k++)
      fmyCovar(j,k) = fCovar(j,k);

    fmyCovar(j,j) *= (1.0 + fLambda);
    fmyGrad(j) = fGrad(j);
  }

  Bool_t rgj = GaussJordan(fNpar, fmyCovar, fmyGrad);
  if(!rgj) return kFALSE;

  for (Int_t j=0; j<fNpar; j++)
  {
    for (Int_t k=0; k<fNpar; k++)
      fCovar(j,k) = fmyCovar(j,k);
  }

  return kTRUE;
}

// -----------------------------------------------------------------------
//
// solve normal equations 
//
//       sum(covar_kl * x_l) = beta_k        (k=0,... (n-1)) 
//
// by the Gauss-Jordan method
// (see Numerical recipes (2nd ed.), W.H.Press et al., p.39) 
//
// on return :  covar contains the inverse of the input matrix covar
//              beta  contains the result for x
//
// return value =  kTRUE   means OK
//                 kFALSE  means singular matrix
//
                 
Bool_t MMarquardt::GaussJordan(Int_t &n, TMatrixD &covar, TVectorD &beta)
{
  Int_t      i, j, k, l, ll;
  Int_t      ic = 0;
  Int_t      ir = 0;
  Double_t   h, d, p;

  for (j=0; j<n; j++)
    fIp(j) = 0;

  for (i=0; i<n; i++)
  {
    h = 0.0;
    for (j=0; j<n; j++)
    {
      if (fIp(j) != 1)
      {
        for (k=0; k<n; k++)
	{
          if (fIp(k) == 0)
	  {
            if (fabs(covar(j,k)) >= h)
	    {
              h = fabs(covar(j,k));
              ir = j;
              ic = k;
	    }
	  }
          else
	  {
            if (fIp(k) > 1) return kFALSE; 
	  }
	}
      }
    }

    fIp(ic)++;
    if (ir != ic)
    {
      for (l=0; l<n; l++)
      {
        d = covar(ir,l);
        covar(ir,l) = covar(ic,l);
        covar(ic,l) = d;
      }
      d = beta(ir);
      beta(ir) = beta(ic);
      beta(ic) = d;
    }

    fIxr(i) = ir;
    fIxc(i) = ic;
    if (covar(ic,ic) == 0.0) return kFALSE;
    p = 1.0 / covar(ic,ic);
    covar(ic,ic) = 1.0;

    for (l=0; l<n; l++)
      covar(ic,l) *= p;

    beta(ic) *= p;

    for (ll=0; ll<n; ll++)
    {
      if (ll!= ic)
      {
        d = covar(ll,ic);
        covar(ll,ic) = 0.0;

        for (l=0; l<n; l++)
          covar(ll,l) -= covar(ic,l) * d;

        beta(ll) -= beta(ic) * d;
      }
    }
  }

  for (l=n-1; l>=0; l--)
  {
    if (fIxr(l) != fIxc(l))
    {
      for (k=0; k<n; k++)
      {
        d = covar(k,fIxr(l));
        covar(k,fIxr(l)) = covar(k,fIxc(l));
        covar(k,fIxc(l)) = d;
      }
    }
  }

  return kTRUE;
}
//=========================================================================














