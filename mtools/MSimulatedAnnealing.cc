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
!   Author(s): Markus Gaug 10/2002  <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MSimulatedAnnealing                                                     //
//                                                                          //
//  class to perform a Simulated Annealing minimization on an n-dimensional //
//  simplex of a function 'FunctionToMinimize(TArrayF &)' in multi-         //
//  dimensional parameter space.                                            //
//  (The code is adapted from Numerical Recipies in C++, 2nd ed.,           //
//  pp. 457-459)                                                            //
//                                                                          //
//  Classes can inherit from MSimulatedAnnealing                            //
//  and use the function:                                                   //
//                                                                          //
//  RunSimulatedAnnealing();                                                //
//                                                                          //
//  They HAVE TO initialize the following input arguments                   //
//  (with ndim being the parameter dimension (max. 20)):                    //
//                                                                          //
//  1) a TMatrix p(ndim+1,ndim)                                             //
//     holding the start simplex                                            //
//  2) a TArrayF y(ndim+1)                                                  //
//     whose components must be pre-initialized to the values of            //
//      FunctionToMinimize evaluated at the fNdim+1 vertices (rows) of p    //
//  3) a TArrayF p0(ndim)                                                   //
//     whose components contain the lower simplex borders                   //
//  4) a TArrayF p1(ndim)                                                   //
//     whose components contain the upper simplex borders                   //
//    (The simplex will not get reflected out of these borders !!!)         //
//                                                                          //
//  These arrays have to be initialized with a call to:                     //
//  Initialize(TMatrix \&, TArrayF \&, TArrayF \&, TArrayF \&)              //
//                                                                          //
//  5) a virtual function FunctionToMinimize(TArrayF &)                     //
//     acting on a TArrayF(ndim) array of parameter values                  //
//                                                                          //
//  Additionally, a global start temperature can be chosen with:            //
//                                                                          //
//  SetStartTemperature(Float_t temp)                                       //
//  (default is: 10)                                                        //
//                                                                          //
//  A total number of total moves (watch out for the CPU time!!!) with:     //
//                                                                          //
//  SetNumberOfMoves(Float_t totalMoves)                                    //
//  (default is: 200)                                                       //
//                                                                          //
//  The temperature is reduced after evaluation step like:                  //
//      CurrentTemperature = StartTemperature*(1-currentMove/totalMoves)    //
//  where currentMove is the cumulative number of moves so far              //
//                                                                          //
//  WARNING: The start temperature and number of moves has to be optimized  //
//           for each individual problem.                                   //
//           It is not straightforward using the defaults!                  //
//           In case, you omit this important step,                         //
//           you will get local minima without even noticing it!!           //
//                                                                          //
//  You may define the following variables:                                 //
//                                                                          //
//  1) A global convergence criterium fTol                                  //
//     which determines an early return for:                                //
//                                                                          //
//     max(FunctionToMinimize(p))-min(FunctionToMinimize(p))                //
//     -----------------------------------------------------  \< fTol       //
//     max(FunctionToMinimize(p))+min(FunctionToMinimize(p))                //
//                                                                          //
//     ModifyTolerance(Float_t)                                             //
//                                                                          //
//  2) A verbose level for prints to *fLog                                  //
//                                                                          //
//     SetVerbosityLevel(Verbosity_t)                                       //
//                                                                          //
//  3) A bit if you want to have stored                                     //
//     the full simplex after every call to Amebsa:                         //
//                                                                          //
//     SetFullStorage()                                                     //
//                                                                          //
//  4) The random number generator                                          //
//     e.g. if you want to test the stability of the output                 //
//                                                                          //
//     SetRandom(TRandom *rand)                                             //
//                                                                          //
//                                                                          //
//  Output containers:                                                      //
//                                                                          //
//  MHSimulatedAnnealing                                                    //
//                                                                          //
//  Use:                                                                    //
//    GetResult()->Draw(Option_t *o)                                        //
//  or                                                                      //
//    GetResult()->DrawClone(Option_t *o)                                   //
//                                                                          //
//  to retrieve the output histograms                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include "MSimulatedAnnealing.h"

#include <fstream>
#include <iostream>

#include <TMath.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MHSimulatedAnnealing.h"

const Float_t MSimulatedAnnealing::gsYtryStr = 10000000;  
const Float_t MSimulatedAnnealing::gsYtryCon = 20000000;  
const Int_t   MSimulatedAnnealing::gsMaxDim  = 20;
const Int_t   MSimulatedAnnealing::gsMaxStep = 50;

ClassImp(MSimulatedAnnealing);

using namespace std;

// ---------------------------------------------------------------------------
//
//  Default Constructor
//  Initializes random number generator and default variables
//
MSimulatedAnnealing::MSimulatedAnnealing()
    : fResult(NULL), fTolerance(0.0001),
      fNdim(0), fNumberOfMoves(200),
      fStartTemperature(10), fFullStorage(kFALSE),
      fInit(kFALSE), 
      fP(gsMaxDim, gsMaxDim), fP0(gsMaxDim),
      fP1(gsMaxDim), fY(gsMaxDim), fYb(gsMaxDim), fYconv(gsMaxDim),
      fPb(gsMaxDim), fPconv(gsMaxDim),
      fBorder(kEStrictBorder), fVerbose(kEDefault)
{

    // random number generator
    fRandom = gRandom;
}

// --------------------------------------------------------------------------
//
//  Destructor. 
//
MSimulatedAnnealing::~MSimulatedAnnealing()
{
  if (fResult) 
    delete fResult;
}

// ---------------------------------------------------------------------------
//
//  Initialization needs the following four members:
//
//  1) a TMatrix p(ndim+1,ndim)
//     holding the start simplex 
//  2) a TVector y(ndim+1)
//     whose components must be pre-initialized to the values of 
//     FunctionToMinimize evaluated at the fNdim+1 vertices (rows) of fP
//  3) a TVector p0(ndim)
//     whose components contain the lower simplex borders 
//  4) a TVector p1(ndim)
//     whose components contain the upper simplex borders
//    (The simplex will not get reflected out of these borders !!!)
//
//  It is possible to perform an initialization and 
//  a subsequent RunMinimization several times. 
//  Each time, a new class MHSimulatedAnnealing will get created
//  (and destroyed). 
//
Bool_t MSimulatedAnnealing::Initialize(const TMatrix &p,  const TVector &y, 
                                       const TVector &p0, const TVector &p1)
{

    fNdim = p.GetNcols();
    fMpts = p.GetNrows();

    //
    // many necessary checks ...
    //
    if (fMpts > gsMaxDim) 
    {
       gLog << err << "Dimension of Matrix fP is too big ... aborting." << endl;
        return kFALSE;
    }
    if (fNdim+1 != fMpts) 
    {
        gLog << err << "Matrix fP does not have the right dimensions ... aborting." << endl;
        return kFALSE;
    }
    if (y.GetNrows() != fMpts) 
    {
        gLog << err << "Array fY has not the right dimension ... aborting." << endl;
        return kFALSE;
    }
    if (p0.GetNrows() != fNdim) 
    {
        gLog << err << "Array fP0 has not the right dimension ... aborting." << endl;
        return kFALSE;
    }
    if (p1.GetNrows() != fNdim) 
    {
        gLog << err << "Array fP1 has not the right dimension ... aborting." << endl;
        return kFALSE;
    }

    //
    // In order to allow multiple use of the class
    // without need to construct the class every time new
    // delete the old fResult and create a new one in RunMinimization
    //
    if (fResult)
       delete fResult;

    fY.ResizeTo(fMpts);
    
    fPsum.ResizeTo(fNdim);
    fPconv.ResizeTo(fNdim);
    
    fP0.ResizeTo(fNdim);
    fP1.ResizeTo(fNdim);
    fPb.ResizeTo(fNdim);
    
    fP.ResizeTo(fMpts,fNdim);

    fY  = y;
    fP  = p;
    fP0 = p0;
    fP1 = p1;
    fPconv.Zero();

    fInit = kTRUE;
    fYconv = 0.;

    return kTRUE;
}


// ---------------------------------------------------------------------------
//
//  RunMinimization:
//
//  Runs only eafter a call to Initialize(const TMatrix \&, const TVector \&,
//                                        const TVector \&, const TVector \&)
//  
//  Temperature and number of moves should have been set
//  (default: StartTemperature = 10, NumberOfMoves = 200
//
//  
//  It is possible to perform an initialization and 
//  a subsequent RunMinimization several times. 
//  Each time, a new class MHSimulatedAnnealing will get created
//  (and destroyed). 
Bool_t MSimulatedAnnealing::RunMinimization()
{
    if (!fInit)
    {
        gLog << err << "No succesful initialization performed yet... aborting." << endl;
        return kFALSE;
    }

    Int_t    iter        = 0;
    UShort_t iret        = 0;
    UShort_t currentMove = 0;
    Real_t   currentTemp = fStartTemperature;

    fResult = new MHSimulatedAnnealing(fNumberOfMoves,fNdim);
    if (fFullStorage) 
      fResult->InitFullSimplex();

    while(1)
    {
        if (iter > 0) 
        {
            gLog << "Convergence at move: " << currentMove ;
            gLog << " and temperature: " << currentTemp << endl;
            break;
        }
        
        if (currentTemp > 0.) 
        {
          //
          // Reduce the temperature 
          //
          // FIXME: Maybe it is necessary to also incorporate other 
          //        ways to reduce the temperature (T0*(1-k/K)**alpha)
          // 
          currentTemp = fStartTemperature*(1.-(float)currentMove++/fNumberOfMoves);
          iter = 1;
        } 
        else 
        {
            // Make sure that now, the program will return only on convergence !
            // The program returns to here only after gsMaxStep moves
            // If we have not reached convergence until then, we assume that an infinite 
            // loop has occurred and quit.
            if (iret != 0) 
            {
                gLog << warn << "No Convergence at the end ! " << endl;
                fY.Zero();

                break;
            }
            iter = 150;
            iret++;
            currentMove++;
        }
        
        if (fVerbose==2) {
            gLog << dbginf << " current..." << endl;
            gLog << " - move:        " << currentMove << endl;
            gLog << " - temperature: " << currentTemp << endl;
            gLog << " - best function evaluation: " << fYb << endl;
        }

        iter = Amebsa(iter, currentTemp);

        // Store the current best values in the histograms
        fResult->StoreBestValueEver(fPb,fYb,currentMove);

        // Store the complete simplex if we have full storage
        if (fFullStorage) 
          fResult->StoreFullSimplex(fP,currentMove);
    }

    //
    // Now, the matrizes and vectors have all the same value, 
    // Need to initialize again to allow a new Minimization
    //
    fInit = kFALSE;

    return kTRUE;
}

// ---------------------------------------------------------------------------
//
//  Amebsa
//
//  This is the (adjusted) amebsa function from 
//  Numerical Recipies (pp. 457-458)
//             
//  The routine makes iter function evaluations at an annealing
//  temperature fCurrentTemp, then returns. If iter is returned 
//  with a poisitive value, then early convergence has occurred. 
//
Int_t MSimulatedAnnealing::Amebsa(Int_t iter, const Real_t temp)
{
    GetPsum();
  
    while (1) 
    {
        UShort_t ihi = 0; // Simplex point with highest function evaluation
        UShort_t ilo = 1; // Simplex point with lowest  function evaluation

        // Function eval. at ilo (with random fluctuations)
        Real_t ylo = fY(0) + gRandom->Exp(temp); 

        // Function eval. at ihi (with random fluctuations)
        Real_t yhi = fY(1) + gRandom->Exp(temp); 

        // The function evaluation at next highest point
        Real_t ynhi = ylo; 

        if (ylo > yhi)
        {
            // Determine which point is the highest (worst),
            // next-highest and lowest (best)
            ynhi = yhi;
            yhi  = ylo;
            ylo  = ynhi;
        }
    
        // By looping over the points in the simplex 
        for (UShort_t i=2;i<fMpts;i++) 
        {
            const Real_t yt = fY(i) + gRandom->Exp(temp);
      
            if (yt <= ylo)
            {
                ilo = i;
                ylo = yt;
            }

            if (yt > yhi)
            {
                ynhi = yhi;
                ihi  = i;
                yhi  = yt;
            }
            else 
                if (yt > ynhi) 
                    ynhi = yt;
        }

        // Now, fY(ilo) is smallest and fY(ihi) is at biggest value 
        if (iter < 0)
        {
            // Enough looping with this temperature, go to decrease it
            // First put best point and value in slot 0
            
            Real_t dum = fY(0);
            fY(0)      = fY(ilo);
            fY(ilo)    = dum;

            for (UShort_t n=0;n<fNdim;n++)
            {
                dum       = fP(0,n);
                fP(0,n)   = fP(ilo,n);
                fP(ilo,n) = dum;
            }
          
            break;
        }
        
        // Compute the fractional range from highest to lowest and
        // return if satisfactory
        Real_t tol = fabs(yhi) + fabs(ylo);
        if (tol != 0)
            tol = 2.0*fabs(yhi-ylo)/tol;
    
        if (tol<fTolerance)
        {
            // Put best point and value in fPconv
            fYconv = fY(ilo);
            for (UShort_t n=0; n<fNdim; n++)
                fPconv(n) = fP(ilo, n);  

            break;
        }
        iter -= 2;

        // Begin new Iteration. First extrapolate by a factor of -1 through
        // the face of the simplex across from the high point, i.e. reflect
        // the simplex from the high point
        Real_t ytry = Amotsa(-1.0, ihi, yhi,temp);
    
        if (ytry <= ylo)
        {
            // cout << " !!!!!!!!!!!!!! E X P A N D  !!!!!!!!!!!!!!" << endl;
            // Gives a result better than the best point, so try an additional
            // extrapolation by a factor of 2
            ytry = Amotsa(2.0, ihi, yhi,temp);
            continue;
        }

        if (ytry < ynhi)
        {
            iter++;
            continue;
        }

        // cout << " !!!!!!!!!!!! R E F L E C T  !!!!!!!!!!!!!!!!!!!!" << endl;
        // The reflected point is worse than the second-highest, so look for an
        // intermediate lower point, for (a one-dimensional contraction */
        const Real_t fYsave = yhi;
        ytry = Amotsa(0.5, ihi, yhi,temp);

        if (ytry < fYsave)
            continue;

        // cout << " !!!!!!!!!!!! R E F L E C T  !!!!!!!!!!!!!!!!!!!!" << endl;
        // The reflected point is worse than the second-highest, so look for an
        // intermediate lower point, for (a one-dimensional contraction */
        const Real_t ysave = yhi;
        ytry = Amotsa(0.5, ihi, yhi,temp);
      
        if (ytry < ysave)
            continue;

        // cout << " !!!!!!!!!!!! C O N T R A C T  !!!!!!!!!!!!!!!!!!" << endl;
        // Cannot seem to get rid of that point, better contract around the
        // lowest (best) point
        for (UShort_t i=0; i<fMpts; i++)
        {
            if (i != ilo)
            {
                for (UShort_t j=0;j<fNdim;j++)
                {
                    fPsum(j) = 0.5*(fP(i, j) + fP(ilo, j));

                    // Create new cutvalues
                    fP(i, j) = fPsum(j);
                }
                fY(i) = FunctionToMinimize(fPsum);
            }
        }

        iter -= fNdim;
        GetPsum();
    }
    return iter;
}

void MSimulatedAnnealing::GetPsum()
{
    for (Int_t n=0; n<fNdim; n++)
    {
        Real_t sum=0.0;
        for (Int_t m=0;m<fMpts;m++)
            sum += fP(m,n);

        fPsum(n) = sum;
    }
}


Real_t MSimulatedAnnealing::Amotsa(const Float_t fac, const UShort_t ihi, 
                                   Real_t &yhi, const Real_t temp)
{
  
    const Real_t fac1 = (1.-fac)/fNdim;
    const Real_t fac2 = fac1 - fac;

    Int_t borderflag = 0;
    TVector ptry(fNdim);
    TVector cols(fMpts);

    for (Int_t j=0; j<fNdim; j++)
    {
        ptry(j) = fPsum(j)*fac1 - fP(ihi, j)*fac2;

        // Check that the simplex does not go to infinite values,
        // in case of: reflect it
        const Real_t newcut = ptry(j);
  
        if (fP1(j) > fP0(j))
        {
            if (newcut > fP1(j))
            {
                ptry(j) = fP1(j);
                borderflag = 1;
            }
            else
                if (newcut < fP0(j))
                {
                    ptry(j) = fP0(j);
                    borderflag = 1;
                }
        }
    
        else
        {
            if (newcut < fP1(j))
            {
                ptry(j) = fP1(j);
                borderflag = 1;
            }
            else
                if (newcut > fP0(j))
                {
                    ptry(j) = fP0(j);
                    borderflag = 1;
                }
        }
    }

    Real_t faccompare = 0.5;
    Real_t ytry = 0;
  
    switch (borderflag)
    {
    case kENoBorder:
        ytry = FunctionToMinimize(fPsum);
        break;

    case kEStrictBorder:
        ytry = FunctionToMinimize(fPsum) + gsYtryStr;
        break;

    case kEContractBorder:
        ytry = fac == faccompare ? gsYtryCon : gsYtryStr;
        break;
    }

    if (ytry < fYb)
    {
        fPb = ptry;
        fYb = ytry;
    }

    const Real_t yflu = ytry + gRandom->Exp(temp);

    if (yflu >= yhi)
        return yflu;

    fY(ihi) = ytry;
    yhi     = yflu;
    
    for(Int_t j=0; j<fNdim; j++)
    {
        fPsum(j) += ptry(j)-fP(ihi, j);
        fP(ihi, j) = ptry(j);
    }

    return yflu;
}

// ---------------------------------------------------------------------------
//
//  Dummy FunctionToMinimize
//
//  A class inheriting from MSimulatedAnnealing NEEDS to contain a similiar
//  
//  virtual Float_t FunctionToMinimize(const TVector \&)
//  
//  The TVector contains the n parameters (dimensions) of the function
//
Float_t MSimulatedAnnealing::FunctionToMinimize(const TVector &arr) 
{
  return 0.0;
}
