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
!   Author(s): Hendrik Bartko, 09/2004 <mailto:hbartko@mppmu.mpg.de> 
!   Author(s): Thomas Bretz, 08/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtralgoDigitalFilter
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtralgoDigitalFilter.h"

#include <TMath.h>
#include <TRandom.h>

using namespace std;

Float_t MExtralgoDigitalFilter::ExtractNoise() const
{
    const Int_t pos  = gRandom->Integer(fNum-fWindowSize+1);
    const Int_t frac = gRandom->Integer(fWeightsPerBin);

    return Eval(fWeightsAmp, pos, frac-fWeightsPerBin/2);
}

// -----------------------------------------------------------------------------
//
// Calculates the chi2 of the fit, once the weights have been iterated.
// Argument: time, obtained after a call to EvalDigitalFilterHiGain
//
Float_t MExtralgoDigitalFilter::GetChisq(const Int_t maxp, const Int_t frac, const Float_t sum) const
{
    /*
    TMatrix g (windowh,1);
    TMatrix gt(windowh,1);
    TMatrix y (windowh,1);

    const Float_t etau   = fFineAdjustHi*sumhi;
    const Float_t t_fine = TMath::Abs(fFineAdjustHi)< 1./fBinningResolutionHiGain ? -fFineAdjustHi : 0.;

    //   if (t_fine==0.)
    //     return -1.;

    if (fDebug)
        gLog << all << " fMaxPHi: " << fMaxPHi << " fIterPHi " << fIterPHi << " t_fine: " << t_fine << endl;

    //
    // Slide with a window of size windowh over the sample
    // and calculate the arrays by interpolating the pulse shape using the
    // fine-tuned time information.
    //
    for (Int_t sample=0; sample &lt; windowh; sample++)
    {
        const Int_t idx = fArrBinningResHalfHiGain[sample] + fIterPHi;
        const Int_t ids = fMaxPHi + sample;

        y [sample][0]     = fHiGainSignalDF[ids];
        g [sample][0]     = t_fine >= 0
            ? (fPulseShapeHiGain[idx]    + t_fine*(fPulseShapeHiGain[idx+1]   -fPulseShapeHiGain[idx])  )*sumhi
            : (fPulseShapeHiGain[idx]    + t_fine*(fPulseShapeHiGain[idx]     -fPulseShapeHiGain[idx-1]))*sumhi;
        gt[sample][0]     = t_fine >= 0
            ? (fPulseShapeDotHiGain[idx] + t_fine*(fPulseShapeDotHiGain[idx+1]-fPulseShapeDotHiGain[idx])   )*etau
            : (fPulseShapeDotHiGain[idx] + t_fine*(fPulseShapeDotHiGain[idx]  -fPulseShapeDotHiGain[idx-1]) )*etau;
    }

    TMatrix second = y - g - gt;
    TMatrix first(TMatrix::kTransposed,second);
    TMatrix chisq = first * ((*fBHiInv)*second);

    return chisq[0][0]/(windowh-2);
    */
/*

    TMatrix S(fWindowSize, 1); // Signal (start==start of window)
    for (int i=0; i<fWindowSize; i++)
        S[i][0] = fVal[i+maxp];

    TMatrix g(fWindowSize, 1);
    //TMatrix gT(fWindowSize, 1);

    for (int i=0; i<fWindowSize; i++)
    {
        Int_t idx = fWeightsPerBin*i + frac;

        // FIXME: Maybe we could do an interpolation on time-fineadj?
        //Float_t slope  = fPulseShapeHiGain[idx+1]   -fPulseShapeHiGain[idx];
        //Float_t slopet = fPulseShapeDotHiGain[idx+1]-fPulseShapeDotHiGain[idx];

        g[i][0] = fPulseShapeHiGain[idx]   *sumhi;
        //gT[i][0] = fPulseShapeHiGainDot[idx]*tau;
    }

    TMatrix Ainv; // Autocorrelation Matrix (inverted)

    TMatrix m = S - g;// - gT;
    TMatrix mT(TMatrix::kTransposed, m);

    TMatrix chisq = mT * (Ainv*m);

    return chisq[0][0]/(fWindowSize-2);
  */

    Double_t sumc = 0;

    TMatrix d(fWindowSize, 1); // Signal (start==start of window)
    for (int i=0; i<fWindowSize; i++)
    {
        d[i][0] = fVal[i+maxp]/sum - fPulseShape[fWeightsPerBin*i + frac];
        sumc += d[i][0]*d[i][0];
    }

/*
    TMatrix Ainv; // Autocorrelation Matrix (inverted)

    TMatrix dT(TMatrix::kTransposed, d);

    TMatrix chisq = dT * (*fAinv*d);
  */
    return sumc;//chisq[0][0]/(fWindowSize-2);
}

Int_t MExtralgoDigitalFilter::AlignExtractionWindow(Int_t &maxp, Int_t &frac, const Double_t ampsum)
{
    // Align extraction window to signal position

    const Double_t timesum = Eval(fWeightsTime, maxp, frac);

    // Because fWeightsPerBin/2 doesn't correspond to the center
    // of a bin the time-values extracted are slightly positive.
    // They are roughly between -0.45 and 0.55
    const Double_t binoffset = TMath::Even(fWeightsPerBin) ? 0.5 : 0;

    // This is the time offset from the extraction position
    Double_t tmoffset = (frac+binoffset)/fWeightsPerBin + timesum/ampsum;

    // Convert the residual fraction of one slice into an
    // offset position in the extraction weights
    const Int_t integ = TMath::FloorNint(tmoffset+0.5);

    /*
    if (integ>0)
        tmoffset=0.49-0.05;
    if (integ<0)
        tmoffset=-0.49-0.05;
    integ=0;
    */

    // move the extractor by an offset number of slices
    // determined by the extracted time
    maxp -= integ;

    frac  = TMath::FloorNint((tmoffset-integ)*fWeightsPerBin);

    // Align maxp into available range (TO BE CHECKED)
    AlignIntoLimits(maxp, frac);

    return integ;
}

void MExtralgoDigitalFilter::Extract(Int_t maxpos)
{
    fSignal    =  0; // default is: no pulse found
    fTime      = -2; // default is: out if range (--> random)
    fSignalDev =  0; // default is: valid
    fTimeDev   =  0; // default is: valid

    // FIXME: How to handle saturation?

    Double_t maxamp = -FLT_MAX;
    Int_t    maxp   = -1;

    //
    // Calculate the sum of the first fWindowSize slices
    //
    // For the case of an even number of weights/bin there is
    // no central bin.So we create an artificial central bin.
    for (Int_t i=0; i<fNum-fWindowSize+1; i++)
    {
        const Double_t sumamp = Eval(fWeightsAmp, i);
        if (sumamp>maxamp)
        {
            maxamp = sumamp;
            maxp   = i;
        }
    }

    /*
     // This could be for a fast but less accurate extraction....
     maxamp = Eval(fWeightsAmp, maxpos-fWindowSize/2);
     maxp   = maxpos-fWindowSize/2;
     */

    // The number of available slices were smaller than the
    // extraction window size of the extractor
    if (maxp<0)
    {
        fSignalDev = -1;  // means: is invalid
        fTimeDev   = -1;  // means: is invalid
        return;
    }

    // For some reason (by chance or because all slices contain 0)
    // maxamp is 0. This means the signal is zero and no arrival
    // time can be extracted (but both informations are valid)
    if (maxamp==0)
        return;

    Int_t frac = 0;
    const Int_t shift = AlignExtractionWindow(maxp, frac, maxamp);

    // For safety we do another iteration if we have
    // shifted the extraction window
    if (TMath::Abs(shift)>0)
        AlignExtractionWindow(maxp, frac);

    // Now we have found the "final" position: extract time and charge
    const Double_t sumamp = Eval(fWeightsAmp, maxp, frac);

    fSignal = sumamp;
    if (sumamp == 0)
        return;

    const Double_t sumtime = Eval(fWeightsTime, maxp, frac);

    // This is used to align the weights to bins between
    // -0.5/fWeightsPerBin and 0.5/fWeightsPerBin instead of
    // 0 and 1./fWeightsPerBin
    const Double_t binoffset = TMath::Even(fWeightsPerBin) ? 0.5 : 0;

    fTime = maxp /*- 0.5*/ -  Double_t(frac+binoffset)/fWeightsPerBin;

    // To let the lowest value which can be safely extracted be>0:
    // Take also a possible offset from timefineadjust into account
    //  Sould it be: fTime += fWindowSize/2; ???

    // HERE we should add the distance from the beginning of the
    // extraction window to the leading edge!
    fTime += 0.5 + 0.5/fWeightsPerBin;
    // Define in each extractor a lowest and highest extracted value!

    const Float_t timefineadjust = sumtime/sumamp;

    //if (TMath::Abs(timefineadjust) < 0.2)
        fTime -= timefineadjust;
}


#include <TH1.h>
#include <TH2.h>
#include <TMatrixD.h>
#include <TArrayF.h>
#include <iostream>
#include <TSpline.h>
#include <TProfile.h>

Int_t MExtralgoDigitalFilter::CalculateWeights(TH1 &shape, const TH2 &autocorr, TArrayF &weightsamp, TArrayF &weightstime, Int_t wpb)
{
    const Int_t weightsperbin = wpb<=0?shape.GetNbinsX()/autocorr.GetNbinsX():wpb;

    if (wpb<=0 && weightsperbin*autocorr.GetNbinsX()!=shape.GetNbinsX())
    {
        cout << "ERROR - Number of bins mismatch..." << endl;
        cout << "        Shape: " << shape.GetNbinsX() << endl;
        cout << "        ACorr: " << autocorr.GetNbinsX() << endl;
        return -1;
    }

    const TAxis &axe = *shape.GetXaxis();

    const Int_t first = axe.GetFirst()/weightsperbin;
    const Int_t last  = axe.GetLast() /weightsperbin;

    const Int_t width = last-first;

    cout << "Range:  " << first << " <= bin < " << last << endl;
    cout << "Window: " << width << endl;
    cout << "W/Bin:  " << weightsperbin << endl;

    // ---------------------------------------------

    const Float_t sum = shape.Integral(first*weightsperbin, last*weightsperbin-1)/weightsperbin;
    shape.Scale(1./sum);

    cout << "Sum:    " << sum << endl;

//    TGraph gr(&shape);
//    TSpline5 val("Signal", &gr);

    // FIXME: DELETE!!!
    TH1 &derivative = *static_cast<TH1*>(shape.Clone());
    derivative.SetDirectory(0);
    derivative.Reset();

    for (int i=0; i<derivative.GetNbinsX(); i++)
    {
//       const Float_t x = derivative.GetBinCenter(i+1);
//       derivative.SetBinContent(i+1, val.Derivative(x));

        const Float_t binm = shape.GetBinContent(i+1-1);
        const Float_t binp = shape.GetBinContent(i+1+1);

        const Float_t der = (binp-binm)/2./shape.GetBinWidth(1);

        derivative.SetBinContent(i+1, der);

        if (derivative.InheritsFrom(TProfile::Class()))
            static_cast<TProfile&>(derivative).SetBinEntries(i+1,1);
    }

    // ---------------------------------------------

    TMatrixD B(width, width);
    for (Int_t i=0; i<width; i++)
        for (Int_t j=0; j<width; j++)
            B[i][j]=autocorr.GetBinContent(i+1/*first*/, j+1/*first*/);

    const TMatrixD Binv(TMatrixD::kInverted, B);

    // ---------------------------------------------

    weightsamp.Set(width*weightsperbin);
    weightstime.Set(width*weightsperbin);

    for (Int_t i=0; i<weightsperbin; i++)
    {
        TMatrixD g(width, 1);
        TMatrixD d(width, 1);

        for (Int_t bin=0; bin<width; bin++)
        {
            const Int_t idx = weightsperbin*(bin+first) + i;

            g[bin][0]=shape.GetBinContent(idx+1);
            d[bin][0]=derivative.GetBinContent(idx+1);
        }

        const TMatrixD gT(TMatrixD::kTransposed, g);
        const TMatrixD dT(TMatrixD::kTransposed, d);

        const TMatrixD denom  = (gT*(Binv*g))*(dT*(Binv*d)) - (dT*(Binv*g))*(dT*(Binv*g));

        if (denom[0][0]==0)
        {
            cout << "ERROR - Division by zero: denom[0][0]==0 for i=" << i << "." << endl;
            return -1;
        }

        const TMatrixD w_amp  = (dT*(Binv*d))*(gT*Binv) - (gT*(Binv*d))*(dT*Binv);
        const TMatrixD w_time = (gT*(Binv*g))*(dT*Binv) - (gT*(Binv*d))*(gT*Binv);

        for (Int_t bin=0; bin<width; bin++)
        {
            const Int_t idx = weightsperbin*bin + i;

            weightsamp[idx]  = w_amp [0][bin]/denom[0][0];
            weightstime[idx] = w_time[0][bin]/denom[0][0];
        }
    }

    return first*weightsperbin;
}

Int_t MExtralgoDigitalFilter::CalculateWeights2(TH1 &shape, const TH2 &autocorr, TArrayF &weightsamp, TArrayF &weightstime, Int_t wpb)
{
    const Int_t weightsperbin = wpb<=0?shape.GetNbinsX()/autocorr.GetNbinsX():wpb;

    if (wpb<=0 && weightsperbin*autocorr.GetNbinsX()!=shape.GetNbinsX())
    {
        cout << "ERROR - Number of bins mismatch..." << endl;
        cout << "        Shape: " << shape.GetNbinsX() << endl;
        cout << "        ACorr: " << autocorr.GetNbinsX() << endl;
        return -1;
    }

    const TAxis &axe = *shape.GetXaxis();

    const Int_t first = axe.GetFirst()/weightsperbin;
    const Int_t last  = axe.GetLast() /weightsperbin;

    const Int_t width = last-first;

    cout << "Range:  " << first << " <= bin < " << last << endl;
    cout << "Window: " << width << endl;
    cout << "W/Bin:  " << weightsperbin << endl;

    // ---------------------------------------------

    const Float_t sum = shape.Integral(first*weightsperbin, last*weightsperbin-1)/weightsperbin;
    shape.Scale(1./sum);

    TGraph gr(&shape);
    TSpline5 val("Signal", &gr);

    // FIXME: DELETE!!!
    TH1 &derivative = *static_cast<TH1*>(shape.Clone());
    derivative.SetDirectory(0);
    derivative.Reset();

    for (int i=0; i<derivative.GetNbinsX(); i++)
    {
        const Float_t x = derivative.GetBinCenter(i+1);
        derivative.SetBinContent(i+1, val.Derivative(x));

     /*
        const Float_t binm = shape.GetBinContent(i+1-1);
        const Float_t binp = shape.GetBinContent(i+1+1);

        const Float_t der = (binp-binm)/2./shape.GetBinWidth(1);

        derivative.SetBinContent(i+1, der);
      */
    }

    // ---------------------------------------------

    TMatrixD B(width, width);
    for (Int_t i=0; i<width; i++)
        for (Int_t j=0; j<width; j++)
            B[i][j]=autocorr.GetBinContent(i+first, j+first);
    B.Invert();

    // ---------------------------------------------

    weightsamp.Set(width*weightsperbin);
    weightstime.Set(width*weightsperbin);

    for (Int_t i=0; i<weightsperbin; i++)
    {
        TMatrixD g(width, 1);
        TMatrixD d(width, 1);

        for (Int_t bin=0; bin<width; bin++)
        {
            const Int_t idx = weightsperbin*(bin+first) + i;

            g[bin][0]=shape.GetBinContent(idx+1);
            d[bin][0]=derivative.GetBinContent(idx+1);
        }

        const TMatrixD gT(TMatrixD::kTransposed, g);
        const TMatrixD dT(TMatrixD::kTransposed, d);

        const TMatrixD denom  = (gT*(B*g))*(dT*(B*d)) - (dT*(B*g))*(dT*(B*g));

        if (denom[0][0]==0)
        {
            cout << "ERROR - Division by zero: denom[0][0]==0 for i=" << i << "." << endl;
            return -1;
        }

        const TMatrixD w_amp  = (dT*(B*d))*(gT*B) - (gT*(B*d))*(dT*B);
        const TMatrixD w_time = (gT*(B*g))*(dT*B) - (gT*(B*d))*(gT*B);

        for (Int_t bin=0; bin<width; bin++)
        {
            const Int_t idx = weightsperbin*bin + i;

            weightsamp[idx]  = w_amp [0][bin]/denom[0][0];
            weightstime[idx] = w_time[0][bin]/denom[0][0];
        }
    }
    return first*weightsperbin;
}
