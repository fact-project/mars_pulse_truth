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
!   Author(s) : T. Bretz  02/2002 <mailto:tbretz@astro.uni-wuerzburg.de>   
!               W. Wittek 09/2002 <mailto:wittek@mppmu.mpg.de>             
!               R. Wagner, 11/2004 <mailto:rwagner@mppmu.mpg.de>           
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MUnfold
//
/////////////////////////////////////////////////////////////////////////////   
#include "MUnfold.h"

#include <TMath.h>
#include <TRandom3.h>
#include <TVector.h>
#include <TMatrixD.h>
#include <TMatrix.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TF1.h>
#include <TMinuit.h>
#include <TCanvas.h>
#include <TMarker.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MUnfold);
using namespace std;

// -----------------------------------------------------------------------
//
// fcnSmooth     (used by SmoothMigrationMatrix)
//
// is called by MINUIT
// for given values of the parameters it calculates the function 
//                                               to be minimized
//
static void fcnSmooth(Int_t &npar, Double_t *gin, Double_t &f, 
               Double_t *par, Int_t iflag)
{
    MUnfold &gUnfold = *(MUnfold*)gMinuit->GetObjectFit();

    Double_t a0 = par[0];
    Double_t a1 = par[1];
    Double_t a2 = par[2];

    Double_t b0 = par[3];
    Double_t b1 = par[4];
    Double_t b2 = par[5];

    // loop over bins of log10(E-true)
    Double_t chi2 = 0.0;
    Int_t npoints = 0;
    Double_t func[20];

    for (UInt_t j=0; j<gUnfold.fNb; j++)
    {
        Double_t yj   = ((double)j) + 0.5;
        Double_t mean = a0 + a1*yj + a2*yj*yj + yj;
        Double_t RMS  = b0 + b1*yj + b2*yj*yj;

        if (RMS <= 0.0)
        {
            chi2 = 1.e20;
            break;
        }

        // loop over bins of log10(E-est)

        //.......................................
        Double_t function;
        Double_t sum=0.0;
        for (UInt_t i=0; i<gUnfold.fNa; i++)
        {
            Double_t xlow = (double)i;
            Double_t xup  = xlow + 1.0;
            Double_t xl  = (xlow- mean) / RMS;
            Double_t xu  = (xup - mean) / RMS;
            function = (TMath::Freq(xu) - TMath::Freq(xl));

            //*fLog << "i, xl, xu, function = " <<  i <<  ",  "  << xl << ",  "
            //     << xu  << ",  " << function << endl;

            if (function < 1.e-10)
                function = 0.0;

            func[i] = function;
            sum += function;
        }

        //      *fLog << "mean, RMS = "  << mean << ",  " << RMS
        //     << ",   j , sum of function = " << j << ",  " << sum << endl;

        //.......................................

        for (UInt_t i=0; i<gUnfold.fNa; i++)
        {
            if (sum != 0.0)
                func[i] /= sum;

            gUnfold.fMigSmoo(i,j) = func[i];
            gUnfold.fMigChi2(i,j) = 0.0;

            // if relative error is greater than 30 % ignore the point

            if (gUnfold.fMigOrig(i,j)     != 0 &&
                gUnfold.fMigOrigerr2(i,j) != 0 &&
                func[i] != 0  )
            {
                if (gUnfold.fMigOrigerr2(i,j)/
                    (gUnfold.fMigOrig(i,j)*gUnfold.fMigOrig(i,j)) <= 0.09)
                {
                    gUnfold.fMigChi2(i,j) =   ( gUnfold.fMigOrig(i,j) - func[i] )
                        * ( gUnfold.fMigOrig(i,j) - func[i] )
                        /   gUnfold.fMigOrigerr2(i,j);
                    chi2 += gUnfold.fMigChi2(i,j);
                    npoints += 1;
                }
            }
        }
        //.......................................

    }
    f = chi2;

    //*fLog << "fcnSmooth : f = " << f << endl;

    //--------------------------------------------------------------------
    // final calculations
    if (iflag == 3)
    {
        Int_t     NDF = npoints - npar;
        Double_t prob = TMath::Prob(chi2, NDF);

        gLog << "fcnSmooth : npoints, chi2, NDF, prob = " << npoints << ",  ";
        gLog << chi2 << ",  " << NDF << ",  " << prob << endl;
        gLog << "=======================================" << endl;
    }
}


// -----------------------------------------------------------------------
//
// fcnTikhonov2     (used by Tikhonov2)
//
// is called by MINUIT
// for given values of the parameters it calculates the function F
// the free parameters are the first (fNb-1) elements
//                     of the normalized unfolded distribution
//
static void fcnTikhonov2(Int_t &npar, Double_t *gin, Double_t &f,
                  Double_t *par, Int_t iflag)
{
    MUnfold &gUnfold = *(MUnfold*)gMinuit->GetObjectFit();

    // (npar+1) is the number of bins of the unfolded distribuition (fNb)
    //  npar    is the number of free parameters                    (fNb-1)

    UInt_t npar1 = npar + 1;

    UInt_t fNa = gUnfold.fNa;
    UInt_t fNb = gUnfold.fNb;
    if (npar1 != fNb)
    {
        gLog << "fcnTikhonov2 : inconsistency in number of parameters; npar, fNb = ";
        gLog << npar << ",  " << fNb << endl;
        //return;
    }
    npar1 = fNb;

    TMatrixD p(npar1, 1);
    TMatrixD &fVb = gUnfold.fVb;

    // p is the normalized unfolded distribution
    // sum(p(i,0)) from i=0 to npar is equal to 1
    Double_t sum = 0.0;
    for (Int_t i=0; i<npar; i++)
    {
        p(i,0) = par[i];
        sum += par[i];
    }
    p(npar,0) = 1.0 - sum;


    // all p(i,0) have to be greater than zero
    for (UInt_t i=0; i<npar1; i++)
        if (p(i,0) <= 0.0)
        {
            f = 1.e20;
            return;
        }

    //.......................
    // take least squares result for the normaliztion
    TMatrixD alpha(gUnfold.fMigrat, TMatrixD::kMult, p);

    //TMatrixD v4   (gUnfold.fVa, TMatrixD::kTransposeMult,
    //                                 gUnfold.fVacovInv);
    //TMatrixD norma(v4,  TMatrixD::kMult, gUnfold.fVa);

    TMatrixD v5   (alpha, TMatrixD::kTransposeMult, gUnfold.fVacovInv);
    TMatrixD normb(v5,    TMatrixD::kMult, alpha);

    TMatrixD normc(v5,    TMatrixD::kMult, gUnfold.fVa);

    Double_t norm  = normc(0,0)/normb(0,0);

    //.......................

    // b is the unnormalized unfolded distribution
    // sum(b(i,0)) from i=0 to npar is equal to norm
    //                       (the total number of events)
    for (UInt_t i=0; i<npar1; i++)
        fVb(i,0) = p(i,0) * norm;

    TMatrixD Gb(gUnfold.fMigrat, TMatrixD::kMult, fVb);
    TMatrixD v3(fNa, 1);
    v3 = gUnfold.fVa;
    v3 -= Gb;

    TMatrixD v1(1,fNa);
    for (UInt_t i=0; i<fNa; i++)
    {
        v1(0,i) = 0;
        for (UInt_t j=0; j<fNa; j++)
            v1(0,i) += v3(j,0) * gUnfold.fVacovInv(j,i) ;
    }

    for (UInt_t i = 0; i<fNa; i++)
        gUnfold.Chi2(i,0) = v1(0,i) * v3(i,0);

    Double_t chisqsum = 0;
    for (Int_t i=0; i<gUnfold.Chi2.GetNrows(); i++)
      chisqsum += gUnfold.Chi2(i, 0);
    gUnfold.Chisq = chisqsum;
    
    //-----------------------------------------------------
    // calculate 2nd derivative squared
    // regularization term (second derivative squared)
    gUnfold.SecDeriv = 0;
    for (UInt_t j=1; j<(fNb-1); j++)
     {
         const Double_t temp =
             + 2.0*(fVb(j+1,0)-fVb(j,0)) / (fVb(j+1,0)+fVb(j,0))
             - 2.0*(fVb(j,0)-fVb(j-1,0)) / (fVb(j,0)+fVb(j-1,0));

         gUnfold.SecDeriv += temp*temp;
     }

    gUnfold.ZerDeriv = 0;
    for (UInt_t j=0; j<fNb; j++)
        gUnfold.ZerDeriv += fVb(j,0) * fVb(j,0);

    f = gUnfold.Chisq/2 * gUnfold.fW + gUnfold.SecDeriv;

    //*fLog << "F=" << f      << " \tSecDeriv=" << gUnfold.SecDeriv
    //     << " \tchi2="
    //	  << gUnfold.Chisq << " \tfW=" << gUnfold.fW << endl;

    //--------------------------------------------------------------------
    // final calculations
    if (iflag == 3)
    {
        //..............................................
        // calculate external error matrix of the fitted parameters 'val'
        // extend it with the covariances for y=1-sum(val)
        Double_t emat[20][20];
        Int_t    ndim = 20;
        gMinuit->mnemat(&emat[0][0], ndim);

        Double_t covv = 0;
        for (UInt_t i=0; i<(gUnfold.fNb-1); i++)
        {
            Double_t cov = 0;
            for (UInt_t k=0; k<(gUnfold.fNb-1); k++)
                cov += emat[i][k];

            emat[i][gUnfold.fNb-1] = -cov;
            emat[gUnfold.fNb-1][i] = -cov;

            covv += cov;
        }
        emat[gUnfold.fNb-1][gUnfold.fNb-1] = covv;

        for (UInt_t i=0; i<gUnfold.fNb; i++)
            for (UInt_t k=0; k<gUnfold.fNb; k++)
                gUnfold.fVbcov(i,k) = emat[i][k] *norm*norm;

        //-----------------------------------------------------
        //..............................................
        // put unfolded distribution into fResult
        //     fResult(i,0)   value in bin i
        //     fResult(i,1)   error of value in bin i

        gUnfold.fResult.ResizeTo(gUnfold.fNb, 5);

        Double_t sum = 0;
        for (UInt_t i=0; i<(gUnfold.fNb-1); i++)
        {
            Double_t val;
            Double_t err;
            if (!gMinuit->GetParameter(i, val, err))
            {
                gLog << "Error getting parameter #" << i << endl;
                return;
            }

            Double_t eplus;
            Double_t eminus;
            Double_t eparab;
            Double_t gcc;
            gMinuit->mnerrs(i, eplus, eminus, eparab, gcc);

            gUnfold.fVb(i, 0)     = val    * norm;

            gUnfold.fResult(i, 0) = val    * norm;
            gUnfold.fResult(i, 1) = eparab * norm;
            gUnfold.fResult(i, 2) = eplus  * norm;
            gUnfold.fResult(i, 3) = eminus * norm;
            gUnfold.fResult(i, 4) = gcc;
            sum += val;
        }
        gUnfold.fVb(gUnfold.fNb-1, 0)     = (1.0-sum) * norm;

        gUnfold.fResult(gUnfold.fNb-1, 0) = (1.0-sum) * norm;
        gUnfold.fResult(gUnfold.fNb-1, 1) =
            sqrt(gUnfold.fVbcov(gUnfold.fNb-1,gUnfold.fNb-1));
        gUnfold.fResult(gUnfold.fNb-1, 2) = 0;
        gUnfold.fResult(gUnfold.fNb-1, 3) = 0;
        gUnfold.fResult(gUnfold.fNb-1, 4) = 1;
        //..............................................

        //-----------------------------------------------------
        // calculate 0th derivative squared
        gUnfold.ZerDeriv = 0;
        for (UInt_t j=0; j<fNb; j++)
            gUnfold.ZerDeriv += fVb(j,0) * fVb(j,0);

        //-----------------------------------------------------
        // calculate the entropy

        gUnfold.Entropy = 0;
        for (UInt_t j=0; j<gUnfold.fNb; j++)
            if (p(j,0) > 0)
                gUnfold.Entropy += p(j,0) * log( p(j,0) );


        //-----------------------------------------------------
        // calculate SpurSigma
        gUnfold.SpurSigma = 0.0;
        for (UInt_t m=0; m<fNb; m++)
            gUnfold.SpurSigma += gUnfold.fVbcov(m,m);
        // *fLog << "SpurSigma =" << SpurSigma << endl;

        //-----------------------------------------------------
        gUnfold.SpurAR  = 0;
        gUnfold.DiffAR2 = 0;

        //-----------------------------------------------------
        gUnfold.fNdf   = gUnfold.fNa;
        gUnfold.fChisq   = gUnfold.Chisq;

        for (UInt_t i=0; i<fNa; i++)
	{
          gUnfold.fChi2(i,0) = gUnfold.Chi2(i,0);
	}


        UInt_t iNdf = (UInt_t) (gUnfold.fNdf+0.5);

        //*fLog << "fcnTikhonov2 : fW, chisq (from fcnF) = "
        //     << gUnfold.fW << ",  " << gUnfold.fChisq << endl;

        gUnfold.fProb = iNdf>0 ? TMath::Prob(gUnfold.fChisq, iNdf) : 0;
    }
}


TH1 *MUnfold::DrawMatrixClone(const TMatrixD &m, Option_t *opt="")
{
    const Int_t nrows = m.GetNrows();
    const Int_t ncols = m.GetNcols();

    TMatrix m2(nrows, ncols);
    for (int i=0; i<nrows; i++)
        for (int j=0; j<ncols; j++)
            m2(i, j) = m(i, j);

    TH2F *hist = new TH2F(m2);
    hist->SetBit(kCanDelete);
    hist->Draw(opt);
    hist->SetDirectory(NULL);

    return hist;

}


TH1 *MUnfold::DrawMatrixColClone(const TMatrixD &m, Option_t *opt="", Int_t col=0)
{
    const Int_t nrows = m.GetNrows();

    TVector vec(nrows);
    for (int i=0; i<nrows; i++)
        vec(i) = m(i, col);

    TH1F *hist = new TH1F("TVector","",nrows,0,nrows);
    for (int i=0; i<nrows; i++)
      hist->SetBinContent(i+1, vec(i));
    
    hist->SetBit(kCanDelete);
    hist->Draw(opt);
    hist->SetDirectory(NULL);

    return hist;
}


void MUnfold::PrintTH3Content(const TH3 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
    {
      for (Int_t j=1; j<=hist.GetNbinsY(); j++)
        for (Int_t k=1; k<=hist.GetNbinsZ(); k++)
            *fLog << hist.GetBinContent(i,j,k) << " \t";
      *fLog << endl << endl;
    }
}

void MUnfold::PrintTH3Error(const TH3 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << " <error>" << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
    {
      for (Int_t j=1; j<=hist.GetNbinsY(); j++)
        for (Int_t k=1; k<=hist.GetNbinsZ(); k++)
            *fLog << hist.GetBinError(i, j, k) << " \t";
      *fLog << endl << endl;
    }
}

void MUnfold::PrintTH2Content(const TH2 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
        for (Int_t j=1; j<=hist.GetNbinsY(); j++)
            *fLog << hist.GetBinContent(i,j) << " \t";
        *fLog << endl << endl;
}

void MUnfold::PrintTH2Error(const TH2 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << " <error>" << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
        for (Int_t j=1; j<=hist.GetNbinsY(); j++)
            *fLog << hist.GetBinError(i, j) << " \t";
        *fLog << endl << endl;
}

void MUnfold::PrintTH1Content(const TH1 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
        *fLog << hist.GetBinContent(i) << " \t";
    *fLog << endl << endl;
}

void MUnfold::PrintTH1Error(const TH1 &hist)
{
    *fLog << hist.GetName() << ": " << hist.GetTitle() << " <error>" << endl;
    *fLog << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=hist.GetNbinsX(); i++)
        *fLog << hist.GetBinError(i) << " \t";
    *fLog << endl << endl;
}

void MUnfold::CopyCol(TMatrixD &m, const TH1 &h, Int_t col=0)
{
    const Int_t n = m.GetNrows();

    for (Int_t i=0; i<n; i++)
        m(i, col) = h.GetBinContent(i+1);
}

void MUnfold::CopyCol(TH1 &h, const TMatrixD &m, Int_t col=0)
{
    const Int_t n = m.GetNrows();

    for (Int_t i=0; i<n; i++)
        h.SetBinContent(i+1, m(i, col));
}

void MUnfold::CopyH2M(TMatrixD &m, const TH2 &h)
{
    const Int_t nx = m.GetNrows();
    const Int_t ny = m.GetNcols();

    for (Int_t i=0; i<nx; i++)
        for (Int_t j=0; j<ny; j++)
            m(i, j) = h.GetBinContent(i+1, j+1);
}

void MUnfold::CopySqr(TMatrixD &m, const TH1 &h)
{
    const Int_t nx = m.GetNrows();
    const Int_t ny = m.GetNcols();

    for (Int_t i=0; i<nx; i++)
        for (Int_t j=0; j<ny; j++)
        {
            const Double_t bin =  h.GetBinContent(i+1, j+1);
            m(i, j) = bin*bin;
        }
}

Double_t MUnfold::GetMatrixSumRow(const TMatrixD &m, Int_t row)
{
    const Int_t n = m.GetNcols();

    Double_t sum = 0;
    for (Int_t i=0; i<n; i++)
        sum += m(row, i);

    return sum;
}

Double_t MUnfold::GetMatrixSumDiag(const TMatrixD &m)
{
    const Int_t n = m.GetNcols();

    Double_t sum = 0;
    for (Int_t i=0; i<n; i++)
        sum += m(i, i);

    return sum;
}

Double_t MUnfold::GetMatrixSumCol(const TMatrixD &m, Int_t col=0)
{
    const Int_t n = m.GetNrows();

    Double_t sum = 0;
    for (Int_t i=0; i<n; i++)
        sum += m(i, col);

    return sum;
}

Double_t MUnfold::GetMatrixSum(const TMatrixD &m)
{
    const Int_t n = m.GetNrows();

    Double_t sum = 0;
    for (Int_t i=0; i<n; i++)
        sum += GetMatrixSumRow(m, i);

    return sum;
}


Double_t MUnfold::CalcSpurSigma(TMatrixD &T, Double_t norm)
{
  Double_t spursigma = 0;
  
  for (UInt_t a=0; a<fNb; a++)
    {
      for (UInt_t b=0; b<fNb; b++)
	{
	  fVbcov(a,b) = 0;
	  
	  for (UInt_t c=0; c<fNa; c++)
	    for (UInt_t d=0; d<fNa; d++)
	      fVbcov(a,b) += T(a,d)*fVacov(d,c)*T(b,c);
	  
	  fVbcov(a,b) *= norm*norm;
	}
      spursigma += fVbcov(a,a);
    }
  
  return spursigma;
}


MUnfold::MUnfold(TH1D &ha, TH2D &hacov, TH2D &hmig)
        : fVEps(hmig.GetYaxis()->GetNbins(),1), fVEps0(fVEps, 0)
    {
        // ha      is the distribution to be unfolded
        // hacov   is the covariance matrix of ha
        // hmig    is the migration matrix;
        //         this matrix will be used in the unfolding
        //         unless SmoothMigrationMatrix(*hmigrat) is called;
        //         in the latter case hmigrat is smoothed
        //         and the smoothed matrix is used in the unfolding

        // Eigen values of the matrix G, which are smaller than EpsLambda
        // will be considered as being zero
        EpsLambda = 1.e-10;
        fW = 0.0;

        fNa  = hmig.GetXaxis()->GetNbins();
        const Double_t alow = hmig.GetXaxis()->GetXmin();
        const Double_t aup  = hmig.GetXaxis()->GetXmax();

        fNb  = hmig.GetYaxis()->GetNbins();
        const Double_t blow = hmig.GetYaxis()->GetXmin();
        const Double_t bup  = hmig.GetYaxis()->GetXmax();


        UInt_t Na = ha.GetNbinsX();
        if (fNa != Na)
        {
            *fLog << "MUnfold::MUnfold : dimensions do not match,  fNa = ";
            *fLog << fNa << ",   Na = " << Na << endl;
        }

        *fLog << "MUnfold::MUnfold :" << endl;
        *fLog << "==================" << endl;
        *fLog << "   fNa = " << fNa << ",   fNb = " << fNb << endl;

        // ------------------------

        fVa.ResizeTo(fNa, 1);
        CopyCol(fVa, ha, 0);

        *fLog << "   fVa = ";

        for (UInt_t i=0; i<fNa; i++)
            *fLog << fVa(i,0) << " \t";
        *fLog << endl;

        Double_t vaevents = GetMatrixSumCol(fVa, 0);
        *fLog << "   Total number of events in fVa = " << vaevents << endl;

        // ------------------------

        fChi2.ResizeTo(fNa,1);
        Chi2.ResizeTo(fNa,1);

        // ------------------------

        fVacov.ResizeTo(fNa, fNa);
        fSpurVacov = 0;

        CopyH2M(fVacov, hacov);

        fVapoints = 0;
        for (UInt_t i=0; i<fNa; i++)
            if (fVa(i,0)>0 && fVacov(i,i)<fVa(i,0)*fVa(i,0))
                fVapoints++;

        fSpurVacov = GetMatrixSumDiag(fVacov);

        //*fLog << "MUnfold::MUnfold :   fVacov = " << endl;
        //*fLog << "==============================" << endl;
        //fVacov.Print();

        *fLog << "   Number of significant points in fVa = ";
        *fLog << fVapoints << endl;

        *fLog << "   Spur of fVacov = ";
        *fLog << fSpurVacov << endl;

        // ------------------------

        fVacovInv.ResizeTo(fNa, fNa);
        fVacovInv = fVacov;
        fVacovInv.InvertPosDef();

        //*fLog << "MUnfold::MUnfold :   fVacovInv = " << endl;
        //*fLog << "==================================" << endl;
        //fVacovInv.Print();

        // ------------------------
        // fMigrat is the migration matrix to be used in the unfolding;
        // fMigrat may be overwritten by SmoothMigrationMatrix

        fMigrat.ResizeTo(fNa, fNb); // row, col

        CopyH2M(fMigrat, hmig);


        // ------------------------

        fMigraterr2.ResizeTo(fNa, fNb); // row, col
        CopySqr(fMigraterr2, hmig);

        // normaxlize

        for (UInt_t j=0; j<fNb; j++)
        {
            const Double_t sum = GetMatrixSumCol(fMigrat, j);

            if (sum==0)
                continue;

            TMatrixDColumn col1(fMigrat, j);
            col1 *= 1./sum;

            TMatrixDColumn col2(fMigraterr2, j);
            col2 *= 1./(sum*sum);
        }

        //*fLog << "MUnfold::MUnfold :   fMigrat = " << endl;
        //*fLog << "===============================" << endl;
        //fMigrat.Print();

        //*fLog << "MUnfold::MUnfold :   fMigraterr2 = " << endl;
        //*fLog << "===================================" << endl;
        //fMigraterr2.Print();

        // ------------------------
        G.ResizeTo(fNa, fNa);
        EigenValue.ResizeTo(fNa);
        Eigen.ResizeTo(fNa, fNa);

        fMigOrig.ResizeTo(fNa, fNb); 
        fMigOrigerr2.ResizeTo(fNa, fNb); 

        fMigSmoo.ResizeTo    (fNa, fNb); 
        fMigSmooerr2.ResizeTo(fNa, fNb); 
        fMigChi2.ResizeTo    (fNa, fNb); 

        // ------------------------

        fVEps0 = 1./fNb;

        //*fLog << "MUnfold::MUnfold :   Default prior distribution fVEps = " << endl;
        //*fLog << "========================================================" << endl;
        //fVEps.Print();

        // ------------------------

        fVb.ResizeTo(fNb,1);
        fVbcov.ResizeTo(fNb,fNb);

        // ----------------------------------------------------
        // number and range of weights to be scanned
        Nix  = 30;
        xmin = 1.e-5;
        xmax = 1.e5;
        dlogx = (log10(xmax)-log10(xmin)) / Nix;

        SpSig.ResizeTo (Nix);
        SpAR.ResizeTo  (Nix);
        chisq.ResizeTo (Nix);
        SecDer.ResizeTo(Nix);
        ZerDer.ResizeTo(Nix);
        Entrop.ResizeTo(Nix);
        DAR2.ResizeTo  (Nix);
        Dsqbar.ResizeTo(Nix);

        //------------------------------------
        // plots as a function of the iteration  number

        hBchisq = new TH1D("Bchisq", "chisq",
                           Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBSpAR  = new TH1D("BSpAR", "SpurAR",
                           Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDSpAR  = new TH1D("BDSpAR", "Delta(SpurAR)",
                            Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBSpSig = new TH1D("BSpSig", "SpurSigma/SpurC",
                           Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDSpSig = new TH1D("BDSpSig", "Delta(SpurSigma/SpurC)",
                            Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBSecDeriv = new TH1D("BSecDeriv", "Second Derivative squared",
                              Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDSecDeriv = new TH1D("BDSecDeriv", "Delta(Second Derivative squared)",
                               Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBZerDeriv = new TH1D("BZerDeriv", "Zero Derivative squared",
                              Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDZerDeriv = new TH1D("BDZerDeriv", "Delta(Zero Derivative squared)",
                               Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBEntropy = new TH1D("BEntrop", "Entropy",
                             Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDEntropy = new TH1D("BDEntrop", "Delta(Entropy)",
                              Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBDAR2 = new TH1D("BDAR2", "norm(AR-AR+)",
                          Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        hBD2bar = new TH1D("BD2bar", "(b_unfolded-b_ideal)**2",
                           Nix, log10(xmin)-dlogx/2.0, log10(xmax)-dlogx/2.0 );

        //-------------------------------------
        // original migration matrix
        fhmig = new TH2D("fMigrat", "Migration matrix",
                         fNa, alow, aup, fNb, blow, bup);
        fhmig->Sumw2();

        //-------------------------------------
        // smoothed migration matrix
        shmig = new TH2D("sMigrat", "Smoothed migration matrix",
                         fNa, alow, aup, fNb, blow, bup);
        shmig->Sumw2();

        //-------------------------------------
        // chi2 contributions for smoothing of migration matrix
        shmigChi2 = new TH2D("sMigratChi2", "Chi2 contr. for smoothing",
                             fNa, alow, aup, fNb, blow, bup);

        //-------------------------------------
        // eigen values of matrix G = M * M(transposed)
        hEigen = new TH1D("Eigen", "Eigen values of M*MT",
                          fNa, 0.5, fNa+0.5);

        //------------------------------------
        // Ideal distribution
        
        fhb0 = new TH1D("fhb0", "Ideal distribution", fNb, blow, bup);
        fhb0->Sumw2();
        

        //------------------------------------
        // Distribution to be unfolded
        fha = new TH1D("fha", "Distribution to be unfolded", fNa, alow, aup);
        fha->Sumw2();

        //------------------------------------
        // Prior distribution
        hprior = new TH1D("Prior", "Prior distribution", fNb, blow, bup);

        //-----------------------------S-------
        // Unfolded distribution
        hb = new TH1D("DataSp", "Unfolded distribution", fNb, blow, bup);
        hb->Sumw2();

    }


// --------------------------------------------------------------------------
//
// Default destructor.
//
    MUnfold::~MUnfold()
    {
//       if (hBchisq) delete hBchisq;
//       if (hBSpAR) delete hBSpAR;
//       if (hBDSpAR) delete hBDSpAR;
//       if (hBSpSig) delete hBSpSig;
//       if (hBDSpSig) delete hBDSpSig;
//       if (hBSecDeriv) delete hBSecDeriv;
//       if (hBSecDeriv) delete hBDSecDeriv;
//       if (hBZerDeriv) delete hBZerDeriv;
//       if (hBDZerDeriv) delete hBDZerDeriv;
//       if (hBEntropy) delete hBEntropy;
//       if (hBDEntropy) delete hBDEntropy;
//       if (hBDAR2) delete hBDAR2;
//       if (hBD2bar) delete hBD2bar;
//       if (fhmig) delete fhmig;
//       if (shmig) delete shmig;
//       if (shmigChi2) delete shmigChi2;
//       if (hEigen) delete hEigen;
//       if (fhb0) delete fhb0;
//       if (fha) delete fha;
//       if (hprior) delete hprior;
//       if (hb) delete hb;
    }

    // -----------------------------------------------------------------------
    //
    // Define prior distribution to be a constant
    //
    void MUnfold::SetPriorConstant()
    {
        fVEps0 = 1./fNb;

        CopyCol(*hprior, fVEps);

        //*fLog << "SetPriorConstant : Prior distribution fVEps = " << endl;
        //*fLog << "==============================================" << endl;
        //fVEps.Print();
    }

    // -----------------------------------------------------------------------
    //
    // Take prior distribution from the histogram 'ha'
    // which may have a different binning than 'hprior'
    //
    Bool_t MUnfold::SetPriorRebin(TH1D &ha)
    {

        // ------------------------------------------------------------------
        //
        // fill the contents of histogram 'ha' into the histogram 'hprior';
        // the histograms need not have the same binning;
        // if the binnings are different, the bin contents of histogram 'ha'
        //    are distributed properly (linearly) over the bins of 'hprior'
        //

        const Int_t    na   = ha.GetNbinsX();
        const Double_t alow = ha.GetBinLowEdge(1);
        const Double_t aup  = ha.GetBinLowEdge(na+1);

        const Int_t    nb   = hprior->GetNbinsX();
        const Double_t blow = hprior->GetBinLowEdge(1);
        const Double_t bup  = hprior->GetBinLowEdge(nb+1);

        // check whether there is an overlap
        //       between the x ranges of the 2 histograms
        if (alow>bup || aup<blow)
        {
            *fLog << "Rebinning not possible because there is no overlap of the x ranges of the two histograms" << endl;
            return kFALSE;
        }

        // there is an overlap
        //********************
        Double_t sum = 0;
        for (Int_t j=1; j<=nb; j++)
        {
            const Double_t yl = hprior->GetBinLowEdge(j);
            const Double_t yh = hprior->GetBinLowEdge(j+1);

            // search bins of histogram ha which contribute
            // to bin j of histogram hb
            //----------------
            Int_t il=0;
            Int_t ih=0;
            for (Int_t i=2; i<=na+1; i++)
            {
                const Double_t xl = ha.GetBinLowEdge(i);
                if (xl>yl)
                {
                    il = i-1;

                    //.................................
                    ih = 0;
                    for (Int_t k=(il+1); k<=(na+1); k++)
                    {
                        const Double_t xh = ha.GetBinLowEdge(k);
                        if (xh >= yh)
                        {
                            ih = k-1;
                            break;
                        }
                    }
                    //.................................
                    if (ih == 0)
                        ih = na;
                    break;
                }
            }
            //----------------
            if (il == 0)
            {
                *fLog << "Something is wrong " << endl;
                *fLog << "          na, alow, aup = " << na << ",  " << alow
                    << ",  " << aup << endl;
                *fLog << "          nb, blow, bup = " << nb << ",  " << blow
                    << ",  " << bup << endl;
                return kFALSE;
            }

            Double_t content=0;
            // sum up the contribution to bin j
            for (Int_t i=il; i<=ih; i++)
            {
                const Double_t xl = ha.GetBinLowEdge(i);
                const Double_t xh = ha.GetBinLowEdge(i+1);
                const Double_t bina = xh-xl;

                if (xl<yl  &&  xh<yh)
                    content += ha.GetBinContent(i) * (xh-yl) / bina;
                else
                    if (xl<yl  &&  xh>=yh)
                        content += ha.GetBinContent(i) * (yh-yl) / bina;
                    else
                        if (xl>=yl  &&  xh<yh)
                            content += ha.GetBinContent(i);
                        else if (xl>=yl  &&  xh>=yh)
                            content += ha.GetBinContent(i) * (yh-xl) / bina;
            }
            hprior->SetBinContent(j, content);
            sum += content;
        }

        // normalize histogram hb
        if (sum==0)
        {
            *fLog << "histogram hb is empty; sum of weights in ha = ";
            *fLog << ha.GetSumOfWeights() << endl;
            return kFALSE;
        }

        for (Int_t j=1; j<=nb; j++)
        {
            const Double_t content = hprior->GetBinContent(j)/sum;
            hprior->SetBinContent(j, content);
            fVEps0(j-1) = content;
        }

        //*fLog << "SetPriorRebin : Prior distribution fVEps = " << endl;
        //*fLog << "===========================================" << endl;
        //fVEps.Print();

        return kTRUE;
    }


    // -----------------------------------------------------------------------
    //
    // Set prior distribution to a given distribution 'hpr'
    //
    Bool_t MUnfold::SetPriorInput(TH1D &hpr)
    {
        CopyCol(fVEps, hpr);

        const Double_t sum = GetMatrixSumCol(fVEps, 0);

        if (sum<=0)
        {
            *fLog << "MUnfold::SetPriorInput: invalid prior distribution" << endl;
            return kFALSE;
        }

        // normalize prior distribution
        fVEps0 *= 1./sum;

        CopyCol(*hprior, fVEps);

        //*fLog << "SetPriorInput : Prior distribution fVEps = " << endl;
        //*fLog << "===========================================" << endl;
        //fVEps.Print();

        return kTRUE;
    }

    // -----------------------------------------------------------------------
    //
    // Define prior distribution to be a power law
    // use input distribution 'hprior' only
    //           for defining the histogram parameters
    //
    Bool_t MUnfold::SetPriorPower(Double_t gamma)
    {
        // generate distribution according to a power law
        //                        dN/dE = E^{-gamma}
        //  or with y = lo10(E),  E = 10^y :
        //                        dN/dy = ln10 * 10^{y*(1-gamma)}
        TH1D hpower(*hprior);

        const UInt_t   nbin = hprior->GetNbinsX();
        const Double_t xmin = hprior->GetBinLowEdge(1);
        const Double_t xmax = hprior->GetBinLowEdge(nbin+1);

        *fLog << "nbin, xmin, xmax = " << nbin << ",  ";
        *fLog << xmin << ",  " << xmax << endl;

        TF1* fpow = new TF1("fpow", "pow(10.0, x*(1.0-[0]))", xmin,xmax);
        fpow->SetParName  (0,"gamma");
        fpow->SetParameter(0, gamma );

        hpower.FillRandom("fpow", 100000);

        // fill prior distribution
        CopyCol(fVEps, hpower);

        const Double_t sum = GetMatrixSumCol(fVEps, 0);
        if (sum <= 0)
        {
            *fLog << "MUnfold::SetPriorPower : invalid prior distribution"  << endl;
            return kFALSE;
        }

        // normalize prior distribution
        fVEps0 *= 1./sum;
        CopyCol(*hprior, fVEps);

        //*fLog << "SetPriorPower : Prior distribution fVEps = " << endl;
        //*fLog << "===========================================" << endl;
        //fVEps.Print();

        return kTRUE;
    }


    // -----------------------------------------------------------------------
    //
    // Set the initial weight
    //
    Bool_t MUnfold::SetInitialWeight(Double_t &weight)
    {
        if (weight == 0.0)
        {
            TMatrixD v1(fVa, TMatrixD::kTransposeMult, fVacovInv);
            TMatrixD v2(v1, TMatrixD::kMult, fVa);
            weight = 1./sqrt(v2(0,0));
        }

        *fLog << "MUnfold::SetInitialWeight : Initial Weight = "
            << weight << endl;

        return kTRUE;
    }

    // -----------------------------------------------------------------------
    //
    // Print the unfolded distribution
    //
    void MUnfold::PrintResults()
    {
        *fLog << bintitle << endl;
        *fLog << "PrintResults : Unfolded distribution fResult " << endl;
        *fLog << "=============================================" << endl;
        //*fLog << "val, eparab, eplus, eminus, gcc = "  << endl;

        for (UInt_t i=0; i<fNb; i++)
        {
	  //    *fLog << fResult(i, 0) << " \t";
          //  *fLog << fResult(i, 1) << " \t";
          //  *fLog << fResult(i, 2) << " \t";
          //  *fLog << fResult(i, 3) << " \t";
          //  *fLog << fResult(i, 4) <<  endl;
        }
        *fLog << "Chisquared, NDF, chi2 probability, ixbest = "
            << fChisq << ",  "
            << fNdf << ",  " << fProb << ",  " << ixbest << endl;

    }


    // -----------------------------------------------------------------------
    //
    // Schmelling  : unfolding by minimizing the function Z
    //               by Gauss-Newton iteration
    //
    //               the weights are scanned between
    //               1.e-5*fWinitial and 1.e5*fWinitial
    //
    Bool_t MUnfold::Schmelling(TH1D &hb0)
    {
    
        //======================================================================
        // copy ideal distribution
        for (UInt_t i=1; i<=fNb; i++)
        {
            fhb0->SetBinContent(i, hb0.GetBinContent(i));
            fhb0->SetBinError  (i, hb0.GetBinError(i));
        }
    
        //-----------------------------------------------------------------------
        // Initialization
        // ==============

        Int_t numGiteration;
        Int_t MaxGiteration = 1000;

        TMatrixD alpha;
        alpha.ResizeTo(fNa, 1);


        //-----------------------------------------------------------------------
        // Newton iteration
        // ================

        Double_t dga2;
        Double_t dga2old;
        Double_t EpsG = 1.e-12;

        TMatrixD wZdp_inv(fNa, fNa);
        TMatrixD d(fNb, 1);
        TMatrixD p(fNb, 1);

        TMatrixD gamma (fNa, 1);
        TMatrixD dgamma(fNa, 1);

        Double_t fWinitial;
        fWinitial = 0.0;
        SetInitialWeight(fWinitial);
        // for my example this fWinitial was not good; therefore :
        fWinitial = 1.0;

        Int_t ix;
        Double_t xiter;

        //--------   start  scanning weights   --------------------------
        // if full == kFALSE   only quantities necessary for the Gauss-Newton
        //                     iteration are calculated in SchmellCore
        // if full == kTRUE    in addition the unfolded distribution,
        //                     its covariance matrix and quantities like
        //                     Chisq, SpurAR, etc. are computed in SchmellCore
        //Bool_t full;
        //full = kFALSE;
        Int_t full;

        *fLog << "Schmelling : start loop over weights" << endl;

        dga2 = 1.e20;
        for (ix=0; ix<Nix; ix++)
        {
            xiter = pow(10.0,log10(xmin)+ix*dlogx) * fWinitial;

            //----------   start Gauss-Newton iteration   ----------------------
            numGiteration = 0;

            // if there was no convergence and the starting gamma was != 0
            // redo unfolding for the same weight starting with gamma = 0
            //
            Int_t gamma0 = 0;
            while (1)
	    {
              if (dga2 > EpsG)
	      {
                gamma0 = 1;
                gamma.Zero();
	      }

              dga2 = 1.e20;

              while (1)
              {
                dga2old = dga2;

                full = 0;
                SchmellCore(full, xiter, gamma, dgamma, dga2);

                gamma += dgamma;

                //*fLog << "Schmelling : ix, numGiteration, dga2, dga2old = "
                //     << ix << ",  " << numGiteration << ",  "
                //     << dga2 << ",  " << dga2old << endl;

                numGiteration += 1;

                // convergence
                if (dga2 < EpsG)
                    break;

                // no convergence
                if (numGiteration > MaxGiteration)
                    break;

                // gamma doesn't seem to change any more
                if (fabs(dga2-dga2old) < EpsG/100.)
                    break;
              }
              //----------   end Gauss-Newton iteration   ------------------------
              if (dga2<EpsG || gamma0 != 0) break;
	    }

            // if Gauss-Newton iteration has not converged
            // go to next weight
            if (dga2 > EpsG)
            {
                *fLog << "Schmelling : Gauss-Newton iteration has not converged;"
                    << "   numGiteration = " << numGiteration << endl;
                *fLog << "             ix, dga2, dga2old = " << ix << ",  "
                    << dga2 << ",  " << dga2old << endl;
                continue;
            }

            //*fLog << "Schmelling : Gauss-Newton iteration has converged;" << endl;
            //*fLog << "==================================================" << endl;
            //*fLog << "             numGiteration = " << numGiteration << endl;
            //*fLog << "             ix, dga2 = " << ix << ",  " << dga2 << endl;

            // calculated quantities which will be useful for determining
            // the best weight (Chisq, SpurAR, ...)
            //full = kTRUE;
            full = 1;
            SchmellCore(full, xiter, gamma, dgamma, dga2);

            // calculate difference between ideal and unfolded distribution
            Double_t D2bar = 0.0;
            for (UInt_t i = 0; i<fNb; i++)
            {
                Double_t temp = fVb(i,0)-hb0.GetBinContent(i+1,0);
                D2bar += temp*temp;
            }

            SpAR(ix)  = SpurAR;
            SpSig(ix) = SpurSigma;
            chisq(ix) = Chisq;
            SecDer(ix) = SecDeriv;
            ZerDer(ix) = ZerDeriv;
            Entrop(ix) = Entropy;
            DAR2(ix)   = DiffAR2;
            Dsqbar(ix) = D2bar;

        }
        //----------   end of scanning weights   -------------------------------
        *fLog << "Schmelling : end of loop over weights" << endl;
        // plots ------------------------------
        for (ix=0; ix<Nix; ix++)
        {
            Double_t xbin = log10(xmin)+ix*dlogx;
            xiter = pow(10.0,xbin) * fWinitial;

            Int_t bin;
            bin = hBchisq->FindBin( xbin );
            hBchisq->SetBinContent(bin,chisq(ix));
            hBSpAR->SetBinContent(bin,SpAR(ix));
            hBSpSig->SetBinContent(bin,SpSig(ix)/fSpurVacov);
            hBSecDeriv->SetBinContent(bin,SecDer(ix));
            hBZerDeriv->SetBinContent(bin,ZerDer(ix));
            hBEntropy->SetBinContent(bin,Entrop(ix));
            hBDAR2->SetBinContent(bin,DAR2(ix));
            hBD2bar->SetBinContent(bin,Dsqbar(ix));

            if (ix > 0)
            {
                Double_t DSpAR = SpAR(ix) - SpAR(ix-1);
                hBDSpAR->SetBinContent(bin,DSpAR);

                Double_t diff = SpSig(ix) - SpSig(ix-1);
                Double_t DSpSig = diff;
                hBDSpSig->SetBinContent(bin, DSpSig/fSpurVacov);

                Double_t DEntrop = Entrop(ix) - Entrop(ix-1);
                hBDEntropy->SetBinContent(bin,DEntrop);

                Double_t DSecDer = SecDer(ix) - SecDer(ix-1);
                hBDSecDeriv->SetBinContent(bin,DSecDer);

                Double_t DZerDer = ZerDer(ix) - ZerDer(ix-1);
                hBDZerDeriv->SetBinContent(bin,DZerDer);
            }
        }

        // Select best weight
        SelectBestWeight();

        if (ixbest < 0.0)
        {
            *fLog << "Schmelling : no solution found; " << endl;
            return kFALSE;
        }

        // do the unfolding using the best weight
        //full = kTRUE;


        xiter = pow(10.0,log10(xmin)+ixbest*dlogx) * fWinitial;

        //----------   start Gauss-Newton iteration   ----------------------
        numGiteration = 0;
        gamma.Zero();
        dga2 = 1.e20;

        while (1)
        {
            full = 1;
            SchmellCore(full, xiter, gamma, dgamma, dga2);
            gamma += dgamma;

            //*fLog << "Schmelling : sum(dgamma^2) = " << dga2 << endl;

            numGiteration += 1;

            if (numGiteration > MaxGiteration)
                break;

            if (dga2 < EpsG)
                break;
        }
        //----------   end Gauss-Newton iteration   ------------------------


        //-----------------------------------------------------------------------
        // termination stage
        // =================

        *fLog << "Schmelling : best solution found; " << endl;
        *fLog << "==================================" << endl;
        *fLog << "             xiter, ixbest, numGiteration, Chisq = "
            << xiter << ",  " << ixbest << ",  "
            << numGiteration << ",  " << Chisq << endl;

        //------------------------------------
        //..............................................
        // put unfolded distribution into fResult
        //     fResult(i,0)   value in bin i
        //     fResult(i,1)   error of value in bin i

        fNdf = SpurAR;
        fChisq = Chisq;

        for (UInt_t i=0; i<fNa; i++)
	{
          fChi2(i,0) = Chi2(i,0);
	}

        UInt_t iNdf   = (UInt_t) (fNdf+0.5);
        fProb = iNdf>0 ? TMath::Prob(fChisq, iNdf) : 0;

        fResult.ResizeTo(fNb, 5);
        for (UInt_t i=0; i<fNb; i++)
        {
            fResult(i, 0) = fVb(i,0);
            fResult(i, 1) = sqrt(fVbcov(i,i));
            fResult(i, 2) = 0.0;
            fResult(i, 3) = 0.0;
            fResult(i, 4) = 1.0; 
	}

        //--------------------------------------------------------

        *fLog << "Schmelling : gamma = " << endl;
        for (UInt_t i=0; i<fNa; i++)
            *fLog << gamma(i,0) << " \t";
        *fLog << endl;

        return kTRUE;
    }




    // -----------------------------------------------------------------------
    //
    // SchmellCore     main part of Schmellings calculations
    //
    void MUnfold::SchmellCore(Int_t full, Double_t &xiter, TMatrixD &gamma,
                     TMatrixD &dgamma, Double_t &dga2)
    {
        Double_t norm;
        TMatrixD wZdp_inv(fNa, fNa);
        TMatrixD d(fNb, 1);
        TMatrixD p(fNb, 1);

        //--------------------------------------------------------
        //--      determine the probability vector p


        TMatrixD v3(gamma, TMatrixD::kTransposeMult, fMigrat);
        TMatrixD v4(TMatrixD::kTransposed, v3);
        d = v4;
        Double_t dmax  = -1.e10;
        for (UInt_t j=0; j<fNb; j++)
            if (d(j,0)>dmax)
                dmax = d(j,0);

        Double_t psum = 0.0;
        for (UInt_t j=0; j<fNb; j++)
        {
            d(j,0) -= dmax;
            p(j,0)  = fVEps0(j)*exp(xiter*d(j,0));
            psum += p(j,0);
        }

        p *= 1.0/psum;

        //--      get the vector alpha

        TMatrixD alpha(fMigrat, TMatrixD::kMult, p);

        //--      determine the current normalization

        TMatrixD v2   (alpha, TMatrixD::kTransposeMult, fVacovInv);
        TMatrixD normb(v2,    TMatrixD::kMult, alpha);

        TMatrixD normc(v2,    TMatrixD::kMult, fVa);

        norm  = normc(0,0)/normb(0,0);

        //--------------------------------------------------------
        //--      determine the scaled slope vector s and Hessian H

        TMatrixD Zp(fNa,1);
        for (UInt_t i=0; i<fNa; i++)
        {
            Zp(i,0) = norm*alpha(i,0) - fVa(i,0);
            for (UInt_t k=0; k<fNa; k++)
                Zp(i,0) += gamma(k,0)*fVacov(k,i);
        }


        TMatrixD Q  (fNa, fNa);
        TMatrixD wZdp(fNa, fNa);
        for (UInt_t i=0; i<fNa; i++)
        {
            for (UInt_t j=0; j<fNa; j++)
            {
                Q(i,j) = - alpha(i,0)*alpha(j,0);
                for (UInt_t k=0; k<fNb; k++)
                    Q(i,j) += fMigrat(i,k)*fMigrat(j,k)*p(k,0);
                wZdp(i,j) = xiter*norm*Q(i,j) + fVacov(i,j);
            }
        }

        //--      invert H and calculate the next Newton step

        Double_t determ = 1.0;
        wZdp_inv = wZdp;
        wZdp_inv.Invert(&determ);

        if(determ == 0.0)
        {
            *fLog << "SchmellCore: matrix inversion for H failed" << endl;
            return;
        }


        dga2 = 0.0;
        for (UInt_t i=0; i<fNa; i++)
        {
            dgamma(i,0) = 0.0;
            for (UInt_t j=0; j<fNa; j++)
                dgamma(i,0) -= wZdp_inv(i,j)*Zp(j,0);
            dga2 += dgamma(i,0)*dgamma(i,0);
        }

        if (full == 0)
            return;

        //--------------------------------------------------------
        //--      determine chi2 and dNdf (#measurements ignored)
        Double_t dNdf = 0;
        for (UInt_t i=0; i<fNa; i++)
        {
            Chi2(i,0) = 0;
            for (UInt_t j=0; j<fNa; j++)
            {
                Chi2(i,0) += fVacov(i,j) * gamma(i,0) * gamma(j,0);
                dNdf       += fVacov(i,j) * wZdp_inv(j,i);
            }
        }
        Chisq = GetMatrixSumCol(Chi2, 0);
        SpurAR = fNa - dNdf;

        //-----------------------------------------------------
        // calculate the norm |AR - AR+|**2

        TMatrixD AR(fNa, fNa);
        DiffAR2 = 0.0;
        for (UInt_t i=0; i<fNa; i++)
        {
            for (UInt_t j=0; j<fNa; j++)
            {
                AR(i,j) = 0.0;
                for (UInt_t k=0; k<fNa; k++)
                    AR(i,j) +=  fVacov(i,k) * wZdp_inv(k,j);
                DiffAR2 += AR(i,j) * AR(i,j);
            }
        }

        //--------------------------------------------------------
        //--      fill distribution b(*)
        fVb = p;
        fVb *= norm;

        //--      determine the covariance matrix of b (very expensive)

        TMatrixD T(fNb,fNa);
        for (UInt_t i=0; i<fNb; i++)
        {
            for (UInt_t j=0; j<fNa; j++)
            {
                T(i,j) = 0.0;
                for (UInt_t k=0; k<fNa; k++)
                    T(i,j) += xiter*wZdp_inv(k,j)*(fMigrat(k,i)-alpha(k,0))*p(i,0);
            }
        }

        SpurSigma = CalcSpurSigma(T, norm);

        //--------------------------------------------------------

        //-----------------------------------------------------
        // calculate the second derivative squared

        SecDeriv = 0;
        for (UInt_t j=1; j<(fNb-1); j++)
        {
            Double_t temp =
                + 2.0*(fVb(j+1,0)-fVb(j,0)) / (fVb(j+1,0)+fVb(j,0))
                - 2.0*(fVb(j,0)-fVb(j-1,0)) / (fVb(j,0)+fVb(j-1,0));
            SecDeriv += temp*temp;
        }

        ZerDeriv = 0;
        for (UInt_t j=0; j<fNb; j++)
            ZerDeriv += fVb(j,0) * fVb(j,0);

        //-----------------------------------------------------
        // calculate the entropy
        Entropy = 0;
        for (UInt_t j=0; j<fNb; j++)
            if (p(j,0) > 0.0)
                Entropy += p(j,0) * log( p(j,0) );

        //--------------------------------------------------------
    }


    // -----------------------------------------------------------------------
    //
    // Smooth migration matrix
    //              by fitting a function to the migration matrix
    //
    Bool_t MUnfold::SmoothMigrationMatrix(TH2D &hmigorig)
    {
        // copy histograms into matrices; the matrices will be used in fcnSmooth
        // ------------------------

      
      //*fLog << "MUnfold::SmoothMigrationMatrix : fNa, fNb = " << fNa << ",  " << fNb << endl;

      //*fLog << "MUnfold::SmoothMigrationMatrix:   fMigOrig = "  << endl;
      //*fLog << "========================================"  << endl;
        for (UInt_t i=0; i<fNa; i++)
        {
            for (UInt_t j=0; j<fNb; j++)
            {
                fMigOrig(i, j)     = hmigorig.GetBinContent(i+1, j+1);
	        //*fLog << fMigOrig(i, j) << " \t";
            }
            //*fLog << endl;
        }
      

        // ------------------------

      
        //*fLog << "MUnfold::SmoothMigrationMatrix :   fMigOrigerr2 = " << endl;
        //*fLog << "=============================================" << endl;
        for (UInt_t i=0; i<fNa; i++)
        {
            for (UInt_t j=0; j<fNb; j++)
            {
                fMigOrigerr2(i, j) =   hmigorig.GetBinError(i+1, j+1)
                    * hmigorig.GetBinError(i+1, j+1);

                //*fLog << fMigOrigerr2(i, j) << " \t";
            }
            //*fLog << endl;
        }
      

        // ------------------------
        // the number of free parameters (npar) is equal to 6:
        //     a0mean, a1mean, a2mean     
        //     <log10(Eest)>    = a0 + a1*log10(Etrue) + a2*SQR(log10(Etrue))
        //                                                     + log10(Etrue)  
        //     b0RMS,  b1RMS, b2RMS      
        //     RMS(log10(Eest)) = b0 + b1*log10(Etrue) + b2*SQR(log10(Etrue))
        // 
        UInt_t npar = 6;

        if (npar > 20)
        {
            *fLog << "MUnfold::SmoothMigrationMatrix : too many parameters,  npar = "
                << npar << endl;
            return kFALSE;
        }


        //..............................................
        // Find reasonable starting values for a0, a1 and b0, b1

        Double_t xbar   = 0.0;
        Double_t xxbar  = 0.0;

        Double_t ybarm  = 0.0;
        Double_t xybarm = 0.0;

        Double_t ybarR  = 0.0;
        Double_t xybarR = 0.0;

        Double_t Sum = 0.0;
        for (UInt_t j=0; j<fNb; j++)
        {
            Double_t x = (double)j + 0.5;

            Double_t meany = 0.0;
            Double_t RMSy  = 0.0;
            Double_t sum   = 0.0;
            for (UInt_t i=0; i<fNa; i++)
            {
                Double_t y = (double)i + 0.5;
                meany +=   y * fMigOrig(i, j);
                RMSy  += y*y * fMigOrig(i, j);
                sum   +=       fMigOrig(i, j);
            }
            if (sum > 0.0)
            {
                meany  = meany / sum;
                RMSy   =  RMSy / sum  - meany*meany;
                RMSy = sqrt(RMSy);

                Sum += sum;
                xbar  +=   x * sum;
                xxbar += x*x * sum;

                ybarm  +=   meany * sum;
                xybarm += x*meany * sum;

                ybarR  +=   RMSy  * sum;
                xybarR += x*RMSy  * sum;
            }
        }

        if (Sum > 0.0)
        {
            xbar   /= Sum;
            xxbar  /= Sum;

            ybarm  /= Sum;
            xybarm /= Sum;

            ybarR  /= Sum;
            xybarR /= Sum;
        }

        Double_t a1start = (xybarm - xbar*ybarm) / (xxbar - xbar*xbar);
        Double_t a0start = ybarm - a1start*xbar;
        a1start = a1start - 1.0;

        Double_t b1start = (xybarR - xbar*ybarR) / (xxbar - xbar*xbar);
        Double_t b0start = ybarR - b1start*xbar;

        *fLog << "MUnfold::SmoothMigrationMatrix : " << endl;
        *fLog << "============================" << endl;
        *fLog << "a0start, a1start = " << a0start << ",  " << a1start << endl;
        *fLog << "b0start, b1start = " << b0start << ",  " << b1start << endl;

        //..............................................
        // Set starting values and step sizes for parameters

        char name[20][100];
        Double_t vinit[20];
        Double_t  step[20];
        Double_t limlo[20];
        Double_t limup[20];
        Int_t    fix[20];

        sprintf(&name[0][0], "a0mean");
        vinit[0] = a0start;
        //vinit[0] = 1.0;
        step[0]  = 0.1;
        limlo[0] = 0.0;
        limup[0] = 0.0;
        fix[0]   = 0;

        sprintf(&name[1][0], "a1mean");
        vinit[1] = a1start;
        //vinit[1] = 0.0;
        step[1]  = 0.1;
        limlo[1] = 0.0;
        limup[1] = 0.0;
        fix[1]   = 0;

        sprintf(&name[2][0], "a2mean");
        vinit[2] = 0.0;
        step[2]  = 0.1;
        limlo[2] = 0.0;
        limup[2] = 0.0;
        fix[2]   = 0;

        sprintf(&name[3][0], "b0RMS");
        vinit[3] = b0start;
          //vinit[3] = 0.8;
        step[3]  = 0.1;
        limlo[3] = 1.e-20;
        limup[3] = 10.0;
        fix[3]   = 0;

        sprintf(&name[4][0], "b1RMS");
        vinit[4] = b1start;
        //vinit[4] = 0.0;
        step[4]  = 0.1;
        limlo[4] = 0.0;
        limup[4] = 0.0;
        fix[4]   = 0;

        sprintf(&name[5][0], "b2RMS");
        vinit[5] = 0.0;
        step[5]  = 0.1;
        limlo[5] = 0.0;
        limup[5] = 0.0;
        fix[5]   = 0;


        if ( CallMinuit(fcnSmooth, npar, name, vinit,
                        step, limlo, limup, fix) )
        {

            // ------------------------
            // fMigrat is the migration matrix to be used in the unfolding;
            // fMigrat, as set by the constructor, is overwritten
            //          by the smoothed migration matrix

            for (UInt_t i=0; i<fNa; i++)
                for (UInt_t j=0; j<fNb; j++)
                    fMigrat(i, j) = fMigSmoo(i, j);

            // ------------------------

            for (UInt_t i=0; i<fNa; i++)
                for (UInt_t j=0; j<fNb; j++)
                    fMigraterr2(i, j) = fMigSmooerr2(i, j);


            // normalize
            for (UInt_t j=0; j<fNb; j++)
            {
                Double_t sum = 0.0;
                for (UInt_t i=0; i<fNa; i++)
                    sum += fMigrat(i, j);

                //*fLog << "SmoothMigrationMatrix : normalization fMigrat; j, sum + "
                //     << j << ",  " << sum << endl;

                if (sum == 0.0)
                    continue;

                for (UInt_t i=0; i<fNa; i++)
                {
                    fMigrat(i, j)     /= sum;
                    fMigraterr2(i, j) /= (sum*sum);
                }
            }

            *fLog << "MUnfold::SmoothMigrationMatrix :   fMigrat = "  << endl;
            *fLog << "========================================"  << endl;
            for (UInt_t i=0; i<fNa; i++)
            {
                for (UInt_t j=0; j<fNb; j++)
                    *fLog << fMigrat(i, j) << " \t";
                *fLog << endl;
            }

	    /*
            *fLog << "MUnfold::SmoothMigrationMatrix :   fMigraterr2 = "  << endl;
            *fLog << "============================================"  << endl;
            for (UInt_t i=0; i<fNa; i++)
            {
                for (UInt_t j=0; j<fNb; j++)
                    *fLog << fMigraterr2(i, j) << " \t";
                *fLog << endl;
            }
	    */

            // ------------------------

            return kTRUE;
        }

        return kFALSE;
    }

    // -----------------------------------------------------------------------
    //
    // Prepare the call to MINUIT for the minimization of the function
    //         f = chi2*w/2 + reg, where reg is the regularization term
    //         reg is the sum the squared 2nd derivatives
    //                        of the unfolded distribution
    //
    // the corresponding fcn routine is 'fcnTikhonov2'
    //
    Bool_t MUnfold::Tikhonov2(TH1D &hb0)
    {
        // the number of free parameters (npar) is equal to
        // the number of bins (fNb) of the unfolded distribution minus 1,
        // because of the constraint that the total number of events
        // is fixed
        UInt_t npar = fNb-1;

        if (npar > 20)
        {
            *fLog << "MUnfold::Tikhonov2 : too many parameters,  npar = "
                << npar << ",  fNb = " << fNb << endl;
            return kFALSE;
        }

        // copy ideal distribution
        
        for (UInt_t i=1; i<=fNb; i++)
        {
            fhb0->SetBinContent(i, hb0.GetBinContent(i));
            fhb0->SetBinError  (i, hb0.GetBinError(i));
        }
        

        //---   start w loop -----------------------------------

        *fLog << "Tikhonov2 : start loop over weights" << endl;

        Int_t ix;
        Double_t xiter;

        for (ix=0; ix<Nix; ix++)
        {
            fW = pow(10.0,log10(xmin)+ix*dlogx);

            //..............................................
            // Set starting values and step sizes for parameters

            char name[20][100];
            Double_t vinit[20];
            Double_t  step[20];
            Double_t limlo[20];
            Double_t limup[20];
            Int_t    fix[20];

            for (UInt_t i=0; i<npar; i++)
            {
                sprintf(&name[i][0], "p%d", i+1);
                vinit[i] = fVEps0(i);
                step[i]  = fVEps0(i)/10;

                // lower and upper limits  (limlo=limup=0: no limits)
                //limlo[i] = 1.e-20;
                limlo[i] = -1.0;
                limup[i] = 1.0;
                fix[i]   = 0;
            }

            // calculate solution for the weight fW
            // flag non-convergence by chisq(ix) = 0.0
            chisq(ix) = 0.0;
            if ( CallMinuit(fcnTikhonov2, npar, name, vinit,
                            step, limlo, limup, fix) )
            {
                // calculate difference between ideal and unfolded distribution
                Double_t D2bar = 0.0;
                for (UInt_t i = 0; i<fNb; i++)
                {
                    Double_t temp = fVb(i,0)-hb0.GetBinContent(i+1,0);
                    D2bar += temp*temp;
                }

                SpAR(ix)   = SpurAR;
                SpSig(ix)  = SpurSigma;
                chisq(ix)  = Chisq;
                SecDer(ix) = SecDeriv;
                ZerDer(ix) = ZerDeriv;
                Entrop(ix) = Entropy;
                DAR2(ix)   = DiffAR2;
                Dsqbar(ix) = D2bar;
            }
        }
        *fLog << "Tikhonov2 : end of loop over weights" << endl;

        // plots ------------------------------
        for (ix=0; ix<Nix; ix++)
        {
            // test whether minimization has converged
            if (chisq(ix) != 0.0)
            {
                xiter = pow(10.0,log10(xmin)+ix*dlogx);

                Int_t bin;
                bin = hBchisq->FindBin( log10(xiter) );
                hBchisq->SetBinContent(bin,chisq(ix));

                //hBSpAR->SetBinContent(bin,SpAR(ix));
                hBSpAR->SetBinContent(bin,0.0);

                hBSpSig->SetBinContent(bin,SpSig(ix)/fSpurVacov);
                hBSecDeriv->SetBinContent(bin,SecDer(ix));
                hBZerDeriv->SetBinContent(bin,ZerDer(ix));
                hBEntropy->SetBinContent(bin,Entrop(ix));

                //hBDAR2->SetBinContent(bin,DAR2(ix));
                hBDAR2->SetBinContent(bin,0.0);

                hBD2bar->SetBinContent(bin,Dsqbar(ix));

                if (ix > 0)
                {
                    //Double_t DSpAR = SpAR(ix) - SpAR(ix-1);
                    //hBDSpAR->SetBinContent(bin,DSpAR);

                    Double_t diff = SpSig(ix) - SpSig(ix-1);
                    Double_t DSpSig = diff;
                    hBDSpSig->SetBinContent(bin, DSpSig/fSpurVacov);

                    Double_t DEntrop = Entrop(ix) - Entrop(ix-1);
                    hBDEntropy->SetBinContent(bin,DEntrop);

                    Double_t DSecDer = SecDer(ix) - SecDer(ix-1);
                    hBDSecDeriv->SetBinContent(bin,DSecDer);

                    Double_t DZerDer = ZerDer(ix) - ZerDer(ix-1);
                    hBDZerDeriv->SetBinContent(bin,DZerDer);
                }
            }
        }


        //---   end w loop -----------------------------------

        // Select best weight
        SelectBestWeight();

        *fLog << " Tikhonov2 : after SelectBestWeight" << endl;

        if (ixbest < 0.0)
        {
            *fLog << "Tikhonov2 : no result found; " << endl;
            return kFALSE;
        }

        *fLog << "Tikhonov2 : best result found; " << endl;
        *fLog << "===============================" << endl;
        *fLog << "           ixbest = " << ixbest << endl;


        // do a final unfolding using the best weight

        fW = pow(10.0,log10(xmin)+ixbest*dlogx);

        //..............................................
        // Set starting values and step sizes for parameters

        char name[20][100];
        Double_t vinit[20];
        Double_t  step[20];
        Double_t limlo[20];
        Double_t limup[20];
        Int_t    fix[20];

        for (UInt_t i=0; i<npar; i++)
        {
            sprintf(&name[i][0], "p%d", i+1);
            vinit[i] = fVEps0(i);
            step[i]  = fVEps0(i)/10;

            // lower and upper limits  (limlo=limup=0: no limits)
            //limlo[i] = 1.e-20;
            limlo[i] = -1.0;
            limup[i] = 1.0;
            fix[i]   = 0;
        }

        // calculate solution for the best weight
        CallMinuit(fcnTikhonov2, npar, name, vinit,
                   step, limlo, limup, fix);


        *fLog << "Tikhonov : Values for best weight " << endl;
        *fLog << "==================================" << endl;
        *fLog << "fW, ixbest, Chisq, SpurAR, SpurSigma, SecDeriv, ZerDeriv, Entrop, DiffAR2, D2bar = " << endl;
        *fLog << "      " << fW << ",  " << ixbest << ",  "
            << Chisq << ",  " << SpurAR << ",  "
            << SpurSigma << ",  " << SecDeriv << ",  " << ZerDeriv << ",  "
            << Entropy << ",  " << DiffAR2 << ",  "
            << Dsqbar(ixbest) << endl;

        return kTRUE;

    }


    // -----------------------------------------------------------------------
    //
    // Bertero :
    //
    // the unfolded distribution is calculated iteratively;
    // the number of iterations after which the iteration is stopped
    //            corresponds to the 'weight' in other methods
    // a small number of iterations corresponds to strong regularization
    // a high number to no regularization
    //
    // see : M.Bertero, INFN/TC-88/2 (1988)
    //       V.B.Anykeyev et al., NIM A303 (1991) 350
    //
    Bool_t MUnfold::Bertero(TH1D &hb0)
    {
        // copy ideal distribution
        
        for (UInt_t i=1; i<=fNb; i++)
        {
            fhb0->SetBinContent(i, hb0.GetBinContent(i));
            fhb0->SetBinError  (i, hb0.GetBinError(i));
        }
        

        TMatrixD bold(fNb, 1);
        bold.Zero();

        //----------------------------------------------------------

        Double_t db2 = 1.e20;


        TMatrixD aminusaest(fNa, 1);

        //-------   scan number of iterations   -----------------

        *fLog << "Bertero : start loop over number of iterations" << endl;

        Int_t ix;

        for (ix=0; ix<Nix; ix++)
        {
            Double_t xiter = pow(10.0,log10(xmin)+ix*dlogx);

            // calculate solution for the iteration number xiter
            BertCore(xiter);

            // calculate difference between ideal and unfolded distribution
            Double_t D2bar = 0.0;
            for (UInt_t i = 0; i<fNb; i++)
            {
                Double_t temp = fVb(i,0)-hb0.GetBinContent(i+1,0);
                D2bar += temp*temp;
            }

            SpAR(ix)   = SpurAR;
            SpSig(ix)  = SpurSigma;
            chisq(ix)  = Chisq;
            SecDer(ix) = SecDeriv;
            ZerDer(ix) = ZerDeriv;
            Entrop(ix) = Entropy;
            DAR2(ix)   = DiffAR2;
            Dsqbar(ix) = D2bar;

            db2 = 0.0;
            for (UInt_t i = 0; i<fNb; i++)
            {
                Double_t temp = fVb(i,0)-bold(i,0);
                db2 += temp*temp;
            }
            bold = fVb;

            //if (db2 < Epsdb2) break;

        }

        *fLog << "Bertero : end of loop over number of iterations" << endl;

        // plots ------------------------------
        for (ix=0; ix<Nix; ix++)
        {
            Double_t xiter = pow(10.0,log10(xmin)+ix*dlogx);

            Int_t bin;
            bin = hBchisq->FindBin( log10(xiter) );
            hBchisq->SetBinContent(bin,chisq(ix));
            hBSpAR->SetBinContent(bin,SpAR(ix));
            hBSpSig->SetBinContent(bin,SpSig(ix)/fSpurVacov);
            hBSecDeriv->SetBinContent(bin,SecDer(ix));
            hBZerDeriv->SetBinContent(bin,ZerDer(ix));
            hBEntropy->SetBinContent(bin,Entrop(ix));
            hBDAR2->SetBinContent(bin,DAR2(ix));
            hBD2bar->SetBinContent(bin,Dsqbar(ix));

            if (ix > 0)
            {
                Double_t DSpAR = SpAR(ix) - SpAR(ix-1);
                hBDSpAR->SetBinContent(bin,DSpAR);

                Double_t diff = SpSig(ix) - SpSig(ix-1);
                Double_t DSpSig = diff;
                hBDSpSig->SetBinContent(bin, DSpSig/fSpurVacov);

                Double_t DEntrop = Entrop(ix) - Entrop(ix-1);
                hBDEntropy->SetBinContent(bin,DEntrop);

                Double_t DSecDer = SecDer(ix) - SecDer(ix-1);
                hBDSecDeriv->SetBinContent(bin,DSecDer);

                Double_t DZerDer = ZerDer(ix) - ZerDer(ix-1);
                hBDZerDeriv->SetBinContent(bin,DZerDer);
            }
        }
        //-------   end of scan of number of iterations   -----------------

        // Select best weight
        SelectBestWeight();


        if (ixbest < 0.0)
        {
            *fLog << "Bertero : weight iteration has NOT converged; " << endl;
            return kFALSE;
        }

        *fLog << "Bertero : weight iteration has converged; " << endl;
        *fLog << "            ixbest = " << ixbest << endl;


        // do a final unfolding using the best weight

        // calculate solution for the iteration number xiter
        Double_t xiter = pow(10.0,log10(xmin)+ixbest*dlogx);
        BertCore(xiter);

        *fLog << "Bertero : Values for best weight " << endl;
        *fLog << "=================================" << endl;
        *fLog << "fW, ixbest, Chisq, SpurAR, SpurSigma, SecDeriv, ZerDeriv, Entrop, DiffAR2, D2bar = " << endl;
        *fLog << "      " << fW << ",  " << ixbest << ",  "
            << Chisq << ",  " << SpurAR << ",  "
            << SpurSigma << ",  " << SecDeriv << ",  " << ZerDeriv << ",  "
            << Entropy << ",  " << DiffAR2 << ",  "
            << Dsqbar(ixbest) << endl;

        //----------------

        fNdf   = SpurAR;
        fChisq = Chisq;

        for (UInt_t i=0; i<fNa; i++)
	{
          fChi2(i,0) = Chi2(i,0);
	}

        UInt_t iNdf   = (UInt_t) (fNdf+0.5);
        fProb = iNdf>0 ? TMath::Prob(fChisq, iNdf) : 0;


        fResult.ResizeTo(fNb, 5);
        for (UInt_t i=0; i<fNb; i++)
        {
            fResult(i, 0) = fVb(i,0);
            fResult(i, 1) = sqrt(fVbcov(i,i));
            fResult(i, 2) = 0.0;
            fResult(i, 3) = 0.0;
            fResult(i, 4) = 1.0;
        }

        return kTRUE;
    }

    // -----------------------------------------------------------------------
    //
    // main part of Bertero's calculations
    //
    Bool_t MUnfold::BertCore(Double_t &xiter)
    {
        // ignore eigen values which are smaller than EpsLambda
        TMatrixD G_inv(fNa, fNa);
        TMatrixD Gtil_inv(fNa, fNa);
        TMatrixD atil(fNb, fNa);
        TMatrixD aminusaest(fNa, 1);

        G_inv.Zero();
        Gtil_inv.Zero();
        SpurAR = 0.0;

        // -----   loop over eigen values   ------------------
        // calculate the approximate inverse of the matrix G
        //*fLog << "flaml = " << endl;

        UInt_t flagstart = 2;
        Double_t flaml=0;

        for (UInt_t l=0; l<fNa; l++)
        {
            if (EigenValue(l) < EpsLambda)
                continue;

            switch (flagstart)
            {
            case 1 :
                // This is the expression for f(lambda) if the initial C^(0)
                // is chosen to be zero
                flaml = 1.0 - pow(1.0-tau*EigenValue(l),xiter);
                break;

            case 2 :
                // This is the expression for f(lambda) if the initial C^(0)
                // is chosen to be equal to the measured distribution
                flaml =           1.0 - pow(1.0-tau*EigenValue(l),xiter)
                    + EigenValue(l) * pow(1.0-tau*EigenValue(l),xiter);
                break;
            }

            //  *fLog << flaml << ",  ";

            for (UInt_t m=0; m<fNa; m++)
            {
                for (UInt_t n=0; n<fNa; n++)
                {
                    G_inv(m,n)    += 1.0  /EigenValue(l) * Eigen(m,l)*Eigen(n,l);
                    Gtil_inv(m,n) += flaml/EigenValue(l) * Eigen(m,l)*Eigen(n,l);
                }
            }
            SpurAR += flaml;
        }
        //*fLog << endl;


        //*fLog << "Gtil_inv =" << endl;
        //for (Int_t m=0; m<fNa; m++)
        //{
        //  for (Int_t n=0; n<fNa; n++)
        //  {
        //    *fLog << Gtil_inv(m,n) << ",  ";
        //  }
        //  *fLog << endl;
        //}

        //-----------------------------------------------------
        // calculate the unfolded distribution b
        TMatrixD v2(fMigrat, TMatrixD::kTransposeMult, Gtil_inv);
        atil = v2;
        TMatrixD v4(atil, TMatrixD::kMult, fVa);
        fVb = v4;

        //-----------------------------------------------------
        // calculate AR and AR+
        TMatrixD AR(v2, TMatrixD::kMult, fMigrat);

        TMatrixD v3(fMigrat, TMatrixD::kTransposeMult, G_inv);
        TMatrixD ARplus(v3, TMatrixD::kMult, fMigrat);


        //-----------------------------------------------------
        // calculate the norm |AR - AR+|**2

        DiffAR2 = 0.0;
        for (UInt_t j=0; j<fNb; j++)
        {
            for (UInt_t k=0; k<fNb; k++)
            {
                Double_t tempo = AR(j,k) - ARplus(j,k);
                DiffAR2       += tempo*tempo;
            }
        }

        //-----------------------------------------------------
        // calculate the second derivative squared

        SecDeriv = 0;
        for (UInt_t j=1; j<(fNb-1); j++)
        {
            // temp = ( 2.0*fVb(j,0)-fVb(j-1,0)-fVb(j+1,0) ) / ( 2.0*fVb(j,0) );
            Double_t temp =    2.0*(fVb(j+1,0)-fVb(j,0)) / (fVb(j+1,0)+fVb(j,0))
                - 2.0*(fVb(j,0)-fVb(j-1,0)) / (fVb(j,0)+fVb(j-1,0));
            SecDeriv += temp*temp;
        }

        ZerDeriv = 0;
        for (UInt_t j=0; j<fNb; j++)
            ZerDeriv += fVb(j,0) * fVb(j,0);

        //-----------------------------------------------------
        // calculate the entropy

        Double_t sumb = 0.0;
        for (UInt_t j=0; j<fNb; j++)
            sumb += fVb(j,0);

        TMatrixD p(fNb,1);
        p = fVb;
        if (sumb > 0.0)
            p *= 1.0/sumb;

        Entropy = 0;
        for (UInt_t j=0; j<fNb; j++)
            if (p(j,0) > 0.0)
                Entropy += p(j,0) * log( p(j,0) );

        //-----------------------------------------------------

        TMatrixD Gb(fMigrat, TMatrixD::kMult, fVb);
        aminusaest = fVa;
        aminusaest -= Gb;
    
        TMatrixD v1(1,fNa);
        for (UInt_t i=0; i<fNa; i++)
        {
            v1(0,i) = 0.0;
            for (UInt_t j=0; j<fNa; j++)
                v1(0,i) += aminusaest(j,0) * fVacovInv(j,i) ;
        }

        //-----------------------------------------------------
        // calculate error matrix fVbcov of unfolded distribution
        SpurSigma = CalcSpurSigma(atil);

        //-----------------------------------------------------
        // calculate the chi squared
        for (UInt_t i = 0; i<fNa; i++)
            Chi2(i,0) = v1(0,i) * aminusaest(i,0);

        Chisq = GetMatrixSumCol(Chi2,0);
        return kTRUE;
    }


    // -----------------------------------------------------------------------
    //
    // Calculate the matrix G = M * M(transposed)
    //           and its eigen values and eigen vectors
    //
    Bool_t MUnfold::CalculateG()
    {
        // Calculate matrix G = M*M(transposed)     (M = migration matrix)
        //           the matrix Eigen of the eigen vectors of G
        //           the vector EigenValues of the eigen values of G
        //           the parameter tau = 1/lambda_max
        //
        TMatrixD v5(TMatrixD::kTransposed, fMigrat);
        //TMatrixD G(fMigrat, TMatrixD::kMult, v5);
        G.Mult(fMigrat, v5);

        Eigen = G.EigenVectors(EigenValue);

        RankG = 0.0;
        for (UInt_t l=0; l<fNa; l++)
        {
            if (EigenValue(l) < EpsLambda) continue;
            RankG += 1.0;
        }

        tau = 1.0 / EigenValue(0);

        //  *fLog << "eigen values : " << endl;
        //  for (Int_t i=0; i<fNa; i++)
        //  {
        //    *fLog << EigenValue(i) << ",  ";
        //  }
        //  *fLog << endl;

        //*fLog << "eigen vectors : " << endl;
        //for (Int_t i=0; i<fNa; i++)
        //{
        //  *fLog << "               vector " << i << endl;
        //  for (Int_t j=0; j<fNa; j++)
        //  {
        //    *fLog << Eigen(j,i) << ",  ";
        //  }
        //  *fLog << endl;
        //}
        //*fLog << endl;

        //*fLog << "G =" << endl;
        //for (Int_t m=0; m<fNa; m++)
        //{
        //  for (Int_t n=0; n<fNa; n++)
        //  {
        //    *fLog << G(m,n) << ",  ";
        //  }
        //  *fLog << endl;
        //}

        return kTRUE;
    }

    // -----------------------------------------------------------------------
    //
    // Select the best weight
    //
    Bool_t MUnfold::SelectBestWeight()
    {
        //-------------------------------
        // select 'best' weight according to some criterion

        Int_t ix;

        Double_t DiffSpSigmax = -1.e10;
        Int_t    ixDiffSpSigmax = -1;

        Double_t DiffSigpointsmin = 1.e10;
        Int_t    ixDiffSigpointsmin = -1;

        Double_t DiffRankGmin = 1.e10;
        Int_t    ixDiffRankGmin = -1;

        Double_t D2barmin = 1.e10;
        Int_t    ixD2barmin = -1;

        Double_t DiffSpSig1min = 1.e10;
        Int_t    ixDiffSpSig1min = -1;


        Int_t ixmax = -1;

        // first loop over all weights :
        //       find smallest chi2
        Double_t chisqmin = 1.e20;
        for (ix=0; ix<Nix; ix++)
        {
            // consider only weights for which
            //  - unfolding was successful
            if (chisq(ix) != 0.0)
            {
                if (chisq(ix) < chisqmin)
                    chisqmin = chisq(ix);
            }
        }
        Double_t chisq0 = chisqmin > fVapoints ? chisqmin : fVapoints/2.0;

        // second loop over all weights :
        //        consider only weights for which chisq(ix) < chisq0
        ixbest = -1;
        for (ix=0; ix<Nix; ix++)
        {
            if (chisq(ix) != 0.0 && chisq(ix) < 2.0*chisq0)
            {
                // ixmax = highest weight with successful unfolding
                //         (Least squares solution)
                ixmax = ix;

                SpurSigma = SpSig(ix);
                SpurAR    = SpAR(ix);
                Chisq    = chisq(ix);
                D2bar     = Dsqbar(ix);

                //----------------------------------
                // search weight where SpurSigma changes most
                //                               (as a function of the weight)
                if (ix > 0  &&  chisq(ix-1) != 0.0)
                {
                    Double_t diff = SpSig(ix) - SpSig(ix-1);
                    if (diff > DiffSpSigmax)
                    {
                        DiffSpSigmax   = diff;
                        ixDiffSpSigmax = ix;
                    }
                }

                //----------------------------------
                // search weight where Chisq is close
                //        to the number of significant measurements
                Double_t DiffSigpoints = fabs(Chisq-fVapoints);

                if (DiffSigpoints < DiffSigpointsmin)
                {
                    DiffSigpointsmin   = DiffSigpoints;
                    ixDiffSigpointsmin = ix;
                }

                //----------------------------------
                // search weight where Chisq is close
                //        to the rank of matrix G
                Double_t DiffRankG = fabs(Chisq-RankG);

                if (DiffRankG < DiffRankGmin)
                {
                    DiffRankGmin   = DiffRankG;
                    ixDiffRankGmin = ix;
                }

                //----------------------------------
                // search weight where SpurSigma is close to 1.0
                Double_t DiffSpSig1 = fabs(SpurSigma/fSpurVacov-1.0);

                if (DiffSpSig1 < DiffSpSig1min)
                {
                    DiffSpSig1min   = DiffSpSig1;
                    ixDiffSpSig1min = ix;
                }

                //----------------------------------
                // search weight where D2bar is minimal

                if (D2bar < D2barmin)
                {
                    D2barmin   = D2bar;
                    ixD2barmin = ix;
                }

                //----------------------------------
            }
        }


        // choose solution where increase of SpurSigma is biggest
        //if ( DiffSpSigmax > 0.0)
        //  ixbest = ixDiffSpSigmax;
        //else
        //  ixbest = ixDiffSigpointsmin;

        // choose Least Squares Solution
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        // ixbest = ixmax;
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

        // choose weight where chi2 is close to the number of significant
        // measurements
        // ixbest = ixDiffSigpointsmin;

        // choose weight where chi2 is close to the rank of matrix G
        // ixbest = ixDiffRankGmin;

        // choose weight where chi2 is close to the rank of matrix G
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
           ixbest = ixDiffSpSig1min;
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

        *fLog << "SelectBestWeight : ixDiffSpSigmax, DiffSpSigmax = "
            << ixDiffSpSigmax << ",  " << DiffSpSigmax << endl;
        *fLog << "================== ixDiffSigpointsmin, DiffSigpointsmin = "
            << ixDiffSigpointsmin << ",  " << DiffSigpointsmin << endl;

        *fLog << "                   ixDiffRankGmin, DiffRankGmin = "
            << ixDiffRankGmin << ",  " << DiffRankGmin << endl;

        *fLog << "                   ixDiffSpSig1min, DiffSpSig1min = "
            << ixDiffSpSig1min << ",  " << DiffSpSig1min << endl;

        *fLog << "                   ixD2barmin, D2barmin = "
            << ixD2barmin << ",  " << D2barmin << endl;
        *fLog << "                   ixmax  = " << ixmax  << endl;
        *fLog << "                   ixbest = " << ixbest << endl;


        return kTRUE;
    }

    // -----------------------------------------------------------------------
    //
    // Draw the plots
    //
    Bool_t MUnfold::DrawPlots()
    {

        // in the plots, mark the weight which has been selected
        Double_t xbin = log10(xmin)+ixbest*dlogx;

        TMarker *m = new TMarker();
        m->SetMarkerSize(1);
        m->SetMarkerStyle(20);

        //-------------------------------------
        // draw the iteration plots
        TString ctitle = bintitle;
        ctitle += "Plots versus weight";
        TCanvas *c = new TCanvas("iter", ctitle, 900, 600);
        c->Divide(3,2);

        c->cd(1);
        hBchisq->Draw();
        gPad->SetLogy();
        hBchisq->SetXTitle("log10(iteration number)");
        hBchisq->SetYTitle("chisq");
        m->DrawMarker(xbin, log10(chisq(ixbest)));

        c->cd(2);
        hBD2bar->Draw();
        gPad->SetLogy();
        hBD2bar->SetXTitle("log10(iteration number)");
        hBD2bar->SetYTitle("(b_unfolded-b_ideal)**2");
        m->DrawMarker(xbin, log10(Dsqbar(ixbest)));

        /*
         c->cd(3);
         hBDAR2->Draw();
         gPad->SetLogy();
         strgx = "log10(iteration number)";
         strgy = "norm(AR-AR+)";
         hBDAR2->SetXTitle(strgx);
         hBDAR2->SetYTitle(strgy);
         m->DrawMarker(xbin, log10(DAR2(ixbest)));
         */

        c->cd(3);
        hBSecDeriv->Draw();
        hBSecDeriv->SetXTitle("log10(iteration number)");
        hBSecDeriv->SetYTitle("Second Derivative squared");
        m->DrawMarker(xbin, SecDer(ixbest));

        /*
         c->cd(8);
         hBDSecDeriv->Draw();
         strgx = "log10(iteration number)";
         strgy = "Delta(Second Derivative squared)";
         hBDSecDeriv->SetXTitle(strgx);
         hBDSecDeriv->SetYTitle(strgy);
         */

        /*
         c->cd(4);
         hBZerDeriv->Draw();
         strgx = "log10(iteration number)";
         strgy = "Zero Derivative squared";
         hBZerDeriv->SetXTitle(strgx);
         hBZerDeriv->SetYTitle(strgy);
         m->DrawMarker(xbin, ZerDer(ixbest));
         */

        /*
         c->cd(5);
         hBDZerDeriv->Draw();
         strgx = "log10(iteration number)";
         strgy = "Delta(Zero Derivative squared)";
         hBDZerDeriv->SetXTitle(strgx);
         hBDZerDeriv->SetYTitle(strgy);
         */

        c->cd(4);
        hBSpAR->Draw();
        hBSpAR->SetXTitle("log10(iteration number)");
        hBSpAR->SetYTitle("SpurAR");
        m->DrawMarker(xbin, SpAR(ixbest));


        /*
         c->cd(11);
         hBDSpAR->Draw();
         strgx = "log10(iteration number)";
         strgy = "Delta(SpurAR)";
         hBDSpAR->SetXTitle(strgx);
         hBDSpAR->SetYTitle(strgy);
         */

        c->cd(5);
        hBSpSig->Draw();
        hBSpSig->SetXTitle("log10(iteration number)");
        hBSpSig->SetYTitle("SpurSig/SpurC");
        m->DrawMarker(xbin, SpSig(ixbest)/fSpurVacov);

        /*
         c->cd(14);
         hBDSpSig->Draw();
         strgx = "log10(iteration number)";
         strgy = "Delta(SpurSig/SpurC)";
         hBDSpSig->SetXTitle(strgx);
         hBDSpSig->SetYTitle(strgy);
         */

        c->cd(6);
        hBEntropy->Draw();
        hBEntropy->SetXTitle("log10(iteration number)");
        hBEntropy->SetYTitle("Entropy");
        m->DrawMarker(xbin, Entrop(ixbest));

        /*
         c->cd(17);
         hBDEntropy->Draw();
         strgx = "log10(iteration number)";
         strgy = "Delta(Entropy)";
         hBDEntropy->SetXTitle(strgx);
         hBDEntropy->SetYTitle(strgy);
         */

        //-------------------------------------

        for (UInt_t i=0; i<fNa; i++)
        {
            fha->SetBinContent(i+1, fVa(i, 0)         );
            fha->SetBinError  (i+1, sqrt(fVacov(i, i)));

            for (UInt_t j=0; j<fNb; j++)
            {
                fhmig->SetBinContent(i+1, j+1, fMigOrig(i, j)           );
                fhmig->SetBinError  (i+1, j+1, sqrt(fMigOrigerr2(i, j)) );

                shmig->SetBinContent(i+1, j+1, fMigrat(i, j)           );
                shmig->SetBinError  (i+1, j+1, sqrt(fMigraterr2(i, j)) );
                shmigChi2->SetBinContent(i+1, j+1, fMigChi2(i, j)   );
            }
        }

        //PrintTH2Content(*shmig);
        //PrintTH2Content(*shmigChi2);

        //-------------------------------------
        CopyCol(*hprior, fVEps);
        CopyCol(*hb,     fVb);
        for (UInt_t i=0; i<fNb; i++)
            hb->SetBinError(i+1, sqrt(fVbcov(i, i)));

        PrintTH1Content(*hb);
        PrintTH1Error(*hb);

        //..............................................
        for (UInt_t i=0; i<fNa; i++)
            hEigen->SetBinContent(i+1, EigenValue(i));

        //..............................................
        // draw the plots
        TString cctitle = bintitle;
        cctitle +=  "Unfolding input";
        TCanvas *cc = new TCanvas("input", cctitle, 900, 600);
        cc->Divide(3, 2);

        // distribution to be unfolded
        cc->cd(1);
        fha->Draw();
        gPad->SetLogy();
        fha->SetXTitle("log10(E_{est}/GeV)");
        fha->SetYTitle("Counts");

        // superimpose unfolded distribution
        hb->Draw("*HSAME");
	hb->SetMarkerColor(75);
	hb->SetLineColor(75);

        // prior distribution
        cc->cd(2);
        hprior->Draw();
        gPad->SetLogy();
        hprior->SetXTitle("log10(E_{true}/GeV)");
        hprior->SetYTitle("Counts");

        // migration matrix
        cc->cd(3);
        fhmig->Draw("box");
        fhmig->SetXTitle("log10(E_{est}/GeV)");
        fhmig->SetYTitle("log10(E_{true}/GeV)");

        // smoothed migration matrix
        cc->cd(4);
        shmig->Draw("box");
        shmig->SetXTitle("log10(E_{est}/GeV)");
        shmig->SetYTitle("log10(E_{true}/GeV)");

        // chi2 contributions for smoothing
        cc->cd(5);
        shmigChi2->Draw("box");
        shmigChi2->SetXTitle("log10(E_{est}/GeV)");
        shmigChi2->SetYTitle("log10(E_{true}/GeV)");

        // Eigenvalues of matrix M*M(transposed)
        cc->cd(6);
        hEigen->Draw();
        hEigen->SetXTitle("l");
        hEigen->SetYTitle("Eigen values Lambda_l of M*M(transposed)");


       //..............................................
        // draw the results
        TString crtitle = bintitle;
        crtitle +=  "Unfolding results";
        TCanvas *cr = new TCanvas("results", crtitle, 600, 600);
        cr->Divide(2, 2);

        // unfolded distribution
        cr->cd(1);
        hb->Draw();
        gPad->SetLogy();
        hb->SetXTitle("log10(E-true/GeV)");
        hb->SetYTitle("Counts");

	
        // covariance matrix of unfolded distribution
        cr->cd(2);
        TH1 *hbcov=DrawMatrixClone(fVbcov, "lego");
        hbcov->SetBins(fNb, hb->GetBinLowEdge(1), hb->GetBinLowEdge(fNb+1),
                       fNb, hb->GetBinLowEdge(1), hb->GetBinLowEdge(fNb+1));

        hbcov->SetName("hbcov");
        hbcov->SetTitle("Error matrix of distribution hb");
        hbcov->SetXTitle("log10(E_{true}/GeV)");
        hbcov->SetYTitle("log10(E_{true}/GeV)");
       
	
        // chi2 contributions
        cr->cd(3);
        TH1 *hchi2=DrawMatrixColClone(fChi2);
        hchi2->SetBins(fNa, fha->GetBinLowEdge(1), fha->GetBinLowEdge(fNa+1));

        hchi2->SetName("Chi2");
        hchi2->SetTitle("chi2 contributions");
        hchi2->SetXTitle("log10(E_{est}/GeV)");
        hchi2->SetYTitle("Chisquared");
	
	
        // ideal distribution
        
        cr->cd(4);
        fhb0->Draw();
        gPad->SetLogy();
        fhb0->SetXTitle("log10(E_{true}/GeV)");
        fhb0->SetYTitle("Counts");
        

        // superimpose unfolded distribution
        hb->Draw("*Hsame");
	

        return kTRUE;
    }


    // -----------------------------------------------------------------------
    //
    // Interface to MINUIT
    //
    //
    Bool_t MUnfold::CallMinuit(
                      void (*fcnx)(Int_t &, Double_t *, Double_t &, Double_t *, Int_t),
                      UInt_t npar, char name[20][100],
                      Double_t vinit[20], Double_t step[20],
                      Double_t limlo[20], Double_t limup[20], Int_t fix[20])
    {
        //
        // Be carefull: This is not thread safe
        //
        UInt_t maxpar = 100;

        if (npar > maxpar)
        {
            *fLog << "MUnfold::CallMinuit : too many parameters,  npar = " << fNb
                << ",   maxpar = " << maxpar << endl;
            return kFALSE;
        }

        //..............................................
        // Set the maximum number of parameters
        TMinuit minuit(maxpar);


        //..............................................
        // Set the print level
        // -1   no output except SHOW comands
        //  0   minimum output
        //  1   normal output (default)
        //  2   additional ouput giving intermediate results
        //  3   maximum output, showing progress of minimizations
        //
        Int_t printLevel = -1;
        minuit.SetPrintLevel(printLevel);

        //..............................................       
        // Printout for warnings
        //    SET WAR      print warnings
        //    SET NOW      suppress warnings
        Int_t errWarn;
        Double_t tmpwar = 0;
        minuit.mnexcm("SET NOW", &tmpwar, 0, errWarn);

        //..............................................
        // Set the address of the minimization function
        minuit.SetFCN(fcnx);

        //..............................................
        // Set starting values and step sizes for parameters
        for (UInt_t i=0; i<npar; i++)
        {
            if (minuit.DefineParameter(i, &name[i][0], vinit[i], step[i],
                                       limlo[i], limup[i]))
            {
                *fLog << "MUnfold::CallMinuit: Error in defining parameter "
                    << name << endl;
                return kFALSE;
            }
        }

        //..............................................
        //Int_t NumPars = minuit.GetNumPars();
        //*fLog << "MUnfold::CallMinuit :  number of free parameters = "
        //     << NumPars << endl;

        //..............................................
        // Minimization
        minuit.SetObjectFit(this);

        //..............................................
        // Error definition :
        //
        //    for2 chisquare function :
        //      up = 1.0   means calculate 1-standard deviation error
        //         = 4.0   means calculate 2-standard deviation error
        //
        //    for log(likelihood) function :
        //      up = 0.5   means calculate 1-standard deviation error
        //         = 2.0   means calculate 2-standard deviation error
        Double_t up = 1.0;
        minuit.SetErrorDef(up);



        // Int_t errMigrad;
        // Double_t tmp = 0;
        // minuit.mnexcm("MIGRAD", &tmp, 0, errMigrad);


        //..............................................
        // fix a parameter
        for (UInt_t i=0; i<npar; i++)
        {
            if (fix[i] > 0)
            {
                Int_t parNo = i;
                minuit.FixParameter(parNo);
            }
        }

        //..............................................
        // Set maximum number of iterations (default = 500)
        Int_t maxiter = 100000;
        minuit.SetMaxIterations(maxiter);

        //..............................................
        // minimization by the method of Migrad
        // Int_t errMigrad;
        // Double_t tmp = 0;
        // minuit.mnexcm("MIGRAD", &tmp, 0, errMigrad);

        //..............................................       
        // same minimization as by Migrad
        // but switches to the SIMPLEX method if MIGRAD fails to converge
        Int_t errMinimize;
        Double_t tmp = 0;
        minuit.mnexcm("MINIMIZE", &tmp, 0, errMinimize);

        //..............................................       
        // check quality of minimization
        // istat = 0   covariance matrix not calculated
        //         1   diagonal approximation only (not accurate)
        //         2   full matrix, but forced positive-definite
        //         3   full accurate covariance matrix 
        //             (indication of normal convergence)
        Double_t fmin, fedm, errdef;
        Int_t    npari, nparx, istat;
        minuit.mnstat(fmin, fedm, errdef, npari, nparx, istat);

        if (errMinimize || istat < 3)
        {
            *fLog << "MUnfold::CallMinuit : Minimization failed" << endl;
            *fLog << "       fmin = " << fmin   << ",   fedm = "  << fedm
                << ",   errdef = "  << errdef << ",   istat = " << istat
                << endl;
            return kFALSE;
        }

        //..............................................
        // Minos error analysis
        // minuit.mnmnos();

        //..............................................       
        // Print current status of minimization
        // if nkode = 0    only function value
        //            1    parameter values, errors, limits
        //            2    values, errors, step sizes, internal values
        //            3    values, errors, step sizes, 1st derivatives
        //            4    values, paraboloc errors, MINOS errors
  
        //Int_t nkode = 4;
        //minuit.mnprin(nkode, fmin);

        //..............................................       
        // call fcn with IFLAG = 3 (final calculation : calculate p(chi2))
        // iflag = 1   initial calculations only
        //         2   calculate 1st derivatives and function
        //         3   calculate function only
        //         4   calculate function + final calculations
        const char *command = "CALL";
        Double_t iflag = 3;
        Int_t errfcn3;
        minuit.mnexcm(command, &iflag, 1, errfcn3); 

        return kTRUE;
    }









