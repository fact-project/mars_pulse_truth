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
!   Author(s): Markus Gaug 01/2004  <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2001-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  Fast Fourier Transforms                                                 //
//                                                                          //
//  (Most of the code is adapted from Numerical Recipies in C++, 2nd ed.,   //
//  pp. 509-563)                                                            //
//                                                                          //
//  Usage:                                                                  //
//                                                                          //
//  1) Functions RealFunctionFFT:  (FOURIER TRANSFORM)                      //
//     * Take as argument arrays of real numbers,                           // 
//       in some cases the dimension of the array has to be given separately//
//     * Return a COMPLEX array with the following meaning:                 //
//       array[0]: The value of F(0) (has only real component)              //
//       array[1]: The value of F(N/2) (has only real component)            //
//       array[2i]: The real part of F(i)                                   //
//       array[2i+1]: The imaginary part of F(i)                            //
//     * Note that F(N-i)* = F(i), therefore only the positive frequency    //
//       half is stored.                                                    //
//     * The dimension MUST be an integer power of 2,                       //
//       otherwise, the array will be shortened!!                           //
//                                                                          //
//  2) Functions RealFunctionIFFT:  (INVERSER FOURIER TRANSFORM)            // 
//     * Take as argument a COMPLEX array                                   //
//       of Fourier-transformed REAL numbers                                //
//       with the following meaning:                                        //
//       array[0]: The value of F(0) (has only real component)              //
//       array[1]: The value of F(N/2) (has only real component)            //
//       array[2i]: The real part of F(i)                                   //
//       array[2i+1]: The imaginary part of F(i)                            //
//     * Returns the original complex array of dimension 2N-1               //                                
//                                                                          //
//  3) Functions PowerSpectrumDensity:                                      //
//     * Return a histogram with the spectral density, i.e.                 //
//       P(k) = 1/(N*N) * |F(k)|*|F(k)|                                     //
//     * The histogram is ranged between 0 and 1./(2*binwidth)              //
//     * The number of bins equals N/2+1                                    //
//     * Note that histograms with unequal binwidth can not yet be treated! //
//     * If the PSD does NOT CONVERGE to 0 at the maximum bin,              //
//       you HAVE TO sample your data finer!                                //
//
// Fourier-Transformation:
// =======================

// (taken from http://www.parasitaere-kapazitaeten.net/Pd/ft.htm)
//
//  The Fourier-Transformation is a mathematical function that breaks
// down a signal (like sound) into its frequency-spectrum as a set of
// sinusoidal components, converting it from the Time Domain to the
// Frequency Domain.
// 
//  In the Time Domain the signal x[ ] consists of N samples, labeled
// from 0 to N-1. In the Frequency Domain the RFFT produces two signals
// (signalvectors), treated as complex numbers representing the Real Part:
// Re X[ ] and the Imaginary Part: Im X[ ]. They are seen as the Cosine-
// und Sine-Components of the base frequencies. Each of these two signals
// contains one more sample than the half of the original signal: N/2 + 1
// samples. (this results from the fact, that the sine-components of the
// first frequency (0) and the last (nyquist, N/2) are always 0). With the
// complex Fourier-Transformation N complexe values are transformed to N
// new complex values. For both it applies to: the Frequency Domain
// contains exactly the same information as the Time-Domain.
// 
//  A Real FFT over 64 samples produces values for 33 cosine- and 33
// sine-wave-amplitudes with the frequencies 0, 1, 2, 3, ..., 30, 31, 32.
// The first value (frequency 0) is the DC (direct current), the other
// values have to be seen in practice as factors of a
// fundamental-frequency which can be calculated by dividing samplerate by
// windowsize. The highest frequency is the nyquist-frequency
// (samplerate/2).
// 
//////////////////////////////////////////////////////////////////////////////

#include "MFFT.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayD.h"
#include "MArrayF.h"
#include "MArrayI.h"

ClassImp(MFFT);

using namespace std;

// ---------------------------------------------------------------------------
//
//  Default Constructor
//  Initializes random number generator and default variables
//
MFFT::MFFT() : fDim(0)
{
}

// --------------------------------------------------------------------------
//
//  Destructor. 
//
MFFT::~MFFT()
{
}

void MFFT::TransformF(const Int_t isign, TArrayF &data)
{
  
  UInt_t   n,mmax,m,j,istep,i;
  Float_t wtemp,wr,wpr,wpi,wi,theta;
  Float_t tempr,tempi;
  
  Int_t nn = fDim/2;
  n = nn << 1;

  //
  // The bit-reversal section of the routine:
  // Exchange the two complex numbers
  //
  j=1;
  for (i=1;i<n;i+=2) {
    if (j > i) {
      Swap(data[j-1],data[i-1]);
      Swap(data[j],data[i]);
    }
    m=nn;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
  // 
  // Here begins the Danielson-Lanczos section of the routine
  //
  mmax=2;
  while (n > mmax) {         // Outer loop executed log_2(nn) times

    istep = mmax << 1;
    //
    // Initialize the trigonometric recurrence:
    //
    theta = isign*(6.28318530717959/mmax);

    wtemp = TMath::Sin(0.5*theta);
    wpr   = -2.0*wtemp*wtemp;
    wpi   = TMath::Sin(theta);

    wr=1.0;
    wi=0.0;

    for (m=1; m<mmax; m+=2) {
      for (i=m; i<=n; i+=istep) {
        // 
        // The Danielson-Lanczos formula:
        //
        j          = i+mmax;
        tempr      = wr*data[j-1] - wi*data[j];
        tempi      = wr*data[j]   + wi*data[j-1];
        data[j-1] = data[i-1]   - tempr;
        data[j]   = data[i]     - tempi;
        data[i-1] += tempr;
        data[i]   += tempi;
      }

      //
      // Trigonometric recurrence
      //
      wr = (wtemp=wr)*wpr - wi*wpi+wr;
      wi = wi*wpr         + wtemp*wpi+wi;

    }
    mmax=istep;
  }
}


void MFFT::TransformD(const Int_t isign, TArrayD &data)
{
  
  UInt_t   n,mmax,m,j,istep,i;
  Double_t wtemp,wr,wpr,wpi,wi,theta;
  Double_t tempr,tempi;
  
  Int_t nn = fDim/2;
  n = nn << 1;

  //
  // The bit-reversal section of the routine:
  // Exchange the two complex numbers
  //
  j=1;
  for (i=1;i<n;i+=2) {
    if (j > i) {
      Swap(data[j-1],data[i-1]);
      Swap(data[j],data[i]);
    }
    m=nn;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
  // 
  // Here begins the Danielson-Lanczos section of the routine
  //
  mmax=2;
  while (n > mmax) {         // Outer loop executed log_2(nn) times

    istep = mmax << 1;
    //
    // Initialize the trigonometric recurrence:
    //
    theta = isign*(6.28318530717959/mmax);

    wtemp = TMath::Sin(0.5*theta);
    wpr   = -2.0*wtemp*wtemp;
    wpi   = TMath::Sin(theta);

    wr=1.0;
    wi=0.0;

    for (m=1; m<mmax; m+=2) {
      for (i=m; i<=n; i+=istep) {
        // 
        // The Danielson-Lanczos formula:
        //
        j          = i+mmax;
        tempr      = wr*data[j-1] - wi*data[j];
        tempi      = wr*data[j]   + wi*data[j-1];
        data[j-1] = data[i-1]   - tempr;
        data[j]   = data[i]     - tempi;
        data[i-1] += tempr;
        data[i]   += tempi;
      }

      //
      // Trigonometric recurrence
      //
      wr = (wtemp=wr)*wpr - wi*wpi+wr;
      wi = wi*wpr         + wtemp*wpi+wi;

    }
    mmax=istep;
  }
}

//
// Calculates the Fourier transform of a set of n real-valued data points. 
// Replaces this data (which is stored in array data[1..n]) by the positive
// frequency half of its complex Fourier transform. The real-valued first 
// and last components of the complex transform are returned as elements 
// data[1] and data[2], respectively. n must be a power of 2. This routine
// also calculates the inverse transform of a complex data array if it is 
// the transform of real data. (Result in this case mus be multiplied by 
// 2/n.). From NUMERICAL RECIPES IN C++.
//
void MFFT::RealFTF(const Int_t isign)
{
  
  Int_t    i,i1,i2,i3,i4;
  Float_t  c1=0.5,c2,h1r,h1i,h2r,h2i;
  Float_t wr,wi,wpr,wpi,wtemp,theta;

  //
  // Initialize the recurrence
  //
  theta = TMath::Pi() / (Double_t)(fDim>>1);

  if(isign==1) // forward transform
    {
      c2    = -0.5;
      TransformF(1,fDataF);
    }
  else         // set up backward transform
    {
      c2    = 0.5;
      theta = -theta;
    }

  wtemp = TMath::Sin(0.5*theta);
  wpr   = -2.0*wtemp*wtemp;
  wpi   = TMath::Sin(theta);

  wr    = 1.0 + wpr;
  wi    = wpi;

  for(i=1;i<(fDim>>2);i++) // case i=0 done separately below
    {

      i2 = 1 + (i1 = i+i);
      i4 = 1 + (i3 = fDim-i1);

      //
      // The two separate transforms are separated out of the data
      //
      h1r  =  c1*(fDataF[i1]+fDataF[i3]);
      h1i  =  c1*(fDataF[i2]-fDataF[i4]);
      h2r  = -c2*(fDataF[i2]+fDataF[i4]);
      h2i  =  c2*(fDataF[i1]-fDataF[i3]);

      //
      // Here, they are recombined to from the true transform 
      // of the orginal real data
      //
      fDataF[i1] =  h1r + wr*h2r - wi*h2i;
      fDataF[i2] =  h1i + wr*h2i + wi*h2r;
      fDataF[i3] =  h1r - wr*h2r + wi*h2i;
      fDataF[i4] = -h1i + wr*h2i + wi*h2r;
      
      //
      // The recurrence
      //
      wr = (wtemp=wr)*wpr - wi*wpi + wr;
      wi =    wi*wpr   + wtemp*wpi + wi;
    }

  //
  // Squeeze the first and last data together to get them all 
  // within the original array
  //
  if(isign==1)
    {
      fDataF[0] = (h1r=fDataF[0]) + fDataF[1];
      fDataF[1] =     h1r  -      fDataF[1];
    }
  else
    {

      fDataF[0] = c1*((h1r=fDataF[0]) + fDataF[1]);
      fDataF[1] = c1*(h1r-fDataF[1]);

      //
      // The inverse transform for the case isign = -1
      //
      TransformF(-1,fDataF);  

      //
      // normalize correctly (not done in original NR's)
      //
      for(i=1;i<=fDim;i++)
        fDataF[i] *= (2./fDim);
    }
}
void MFFT::RealFTD(const Int_t isign)
{
  
  Int_t    i,i1,i2,i3,i4;
  Float_t  c1=0.5,c2,h1r,h1i,h2r,h2i;
  Double_t wr,wi,wpr,wpi,wtemp,theta;

  //
  // Initialize the recurrence
  //
  theta=3.141592653589793/(Double_t) (fDim>>1);

  if(isign==1) // forward transform
    {
      c2    = -0.5;
      TransformD(1,fDataD);
    }
  else         // set up backward transform
    {
      c2    = 0.5;
      theta = -theta;
    }

  wtemp = TMath::Sin(0.5*theta);
  wpr   = -2.0*wtemp*wtemp;
  wpi   = TMath::Sin(theta);

  wr    = 1.0 + wpr;
  wi    = wpi;

  for(i=1;i<(fDim>>2);i++) // case i=0 done separately below
    {

      i2 = 1 + (i1 = i+i);
      i4 = 1 + (i3 = fDim-i1);

      //
      // The two separate transforms are separated out of the data
      //
      h1r  =  c1*(fDataD[i1]+fDataD[i3]);
      h1i  =  c1*(fDataD[i2]-fDataD[i4]);
      h2r  = -c2*(fDataD[i2]+fDataD[i4]);
      h2i  =  c2*(fDataD[i1]-fDataD[i3]);

      //
      // Here, they are recombined to from the true transform 
      // of the orginal real data
      //
      fDataD[i1] =  h1r + wr*h2r - wi*h2i;
      fDataD[i2] =  h1i + wr*h2i + wi*h2r;
      fDataD[i3] =  h1r - wr*h2r + wi*h2i;
      fDataD[i4] = -h1i + wr*h2i + wi*h2r;
      
      //
      // The recurrence
      //
      wr = (wtemp=wr)*wpr - wi*wpi + wr;
      wi =    wi*wpr   + wtemp*wpi + wi;
    }

  //
  // Squeeze the first and last data together to get them all 
  // within the original array
  //
  if(isign==1)
    {
      fDataD[0] = (h1r=fDataD[0]) + fDataD[1];
      fDataD[1] =     h1r  -      fDataD[1];
    }
  else
    {

      fDataD[0] = c1*((h1r=fDataD[0]) + fDataD[1]);
      fDataD[1] = c1*(h1r-fDataD[1]);

      //
      // The inverse transform for the case isign = -1
      //
      TransformD(-1,fDataD);  
      
      //
      // normalize correctly (not done in original NR's)
      //
      for(i=1;i<=fDim;i++)
        fDataD[i] *= (2./fDim);
    }
}


//
// Fast Fourier Transform for float arrays
//
Float_t* MFFT::RealFunctionFFT(const Int_t n, const Float_t *data)
{

  fDim = n;
  CheckDim(n);

  fDataF.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = data[i];

  RealFTF(1);
  
  return fDataF.GetArray();

}

//
// Fast Inverse Fourier Transform for float arrays
//
Float_t* MFFT::RealFunctionIFFT(const Int_t n, const Float_t *data)
{

  fDim = n;
  CheckDim(fDim);
  
  fDataF.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = data[i];

  RealFTF(-1);
  
  return fDataF.GetArray();

}

//
// Fast Fourier Transform for double arrays
//
Double_t* MFFT::RealFunctionFFT(const Int_t n, const Double_t *data)
{

  fDim = n;
  CheckDim(n);

  fDataD.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = data[i];

  RealFTD(1);
  
  return fDataD.GetArray();

}

//
// Fast Inverse Fourier Transform for double arrays
//
Double_t* MFFT::RealFunctionIFFT(const Int_t n, const Double_t *data)
{

  fDim = n;
  CheckDim(fDim);
  
  fDataD.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = data[i];

  RealFTD(-1);
  
  return fDataD.GetArray();

}

//
// Fast Fourier Transform for TArrayF's
//
TArrayF* MFFT::RealFunctionFFT(const TArrayF *data)
{

  fDim = data->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = data->At(i);

  RealFTF(1);
  
  return new TArrayF(fDim,fDataF.GetArray());

}

//
// Inverse Fast Fourier Transform for TArrayF's
//
TArrayF* MFFT::RealFunctionIFFT(const TArrayF *data)
{

  fDim = data->GetSize();
  CheckDim(fDim);
  
  fDataF.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = data->At(i);

  RealFTF(-1);

  return new TArrayF(fDim,fDataF.GetArray());
}


//
// Fast Fourier Transform for TArrayD's
//
TArrayD* MFFT::RealFunctionFFT(const TArrayD *data)
{

  fDim = data->GetSize();
  CheckDim(fDim);

  fDataD.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = data->At(i);

  RealFTD(1);
  
  return new TArrayD(fDim,fDataD.GetArray());

}

//
// Inverse Fast Fourier Transform for TArrayD's
//
TArrayD* MFFT::RealFunctionIFFT(const TArrayD *data)
{

  fDim = data->GetSize();
  CheckDim(fDim);
  
  fDataD.Set(fDim);
  //
  // Clone the array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = data->At(i);

  RealFTD(-1);

  return new TArrayD(fDim,fDataD.GetArray());
}

//----------------------------------------------------------
//
// Power Spectrum Density Calculation
//
TH1D* MFFT::PowerSpectrumDensity(const TH1D *hist)
{

  TH1D *newhist = (TH1D*)CheckHist(hist,1);

  fDataD.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = hist->GetBinContent(i);

  RealFTD(1);

  Int_t dim2 = fDim*fDim;
  Double_t c02;
  Double_t ck2;
  Double_t cn2;
  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //    (stored in fData{0])
  //
  c02 = fDataD[0]*fDataD[0];
  newhist->Fill(c02/dim2);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)| + |C(N-k)|*|C(N-k)|)
  //
  for (Int_t k=2;k<fDim-2;k+=2)
    {

      Int_t ki  = k+1;
      ck2 = (fDataD[k]*fDataD[k] + fDataD[ki]*fDataD[ki]);
      newhist->Fill(ck2/dim2);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //    (stored in fData[1])
  //
  cn2 = (fDataD[1]*fDataD[1]);
  newhist->Fill(cn2/dim2);

  return newhist;
}

// -------------------------------------------------
//
// Power Spectrum Density calculation for TArrayF
//
TArrayF* MFFT::PowerSpectrumDensity(const TArrayF *array)
{

  fDim = array->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = array->At(i);

  RealFTF(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t c02;
  Float_t ck2;
  Float_t cn2;
  
  TArrayF *newarray = new TArrayF(dim05);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  c02 = (fDataF[0]*fDataF[0]);
  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataF[k2]*fDataF[k2] + fDataF[k2+1]*fDataF[k2+1]);
      newarray->AddAt(ck2/dim2,k);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  cn2 = (fDataF[1]*fDataF[1]);
  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}

// -------------------------------------------------
//
// Power Spectrum Density calculation for TArrayI
//
TArrayF* MFFT::PowerSpectrumDensity(const TArrayI *array)
{

  fDim = array->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = (Float_t)array->At(i);

  RealFTF(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t c02;
  Float_t ck2;
  Float_t cn2;
  
  TArrayF *newarray = new TArrayF(dim05);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  c02 = (fDataF[0]*fDataF[0]);
  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataF[k2]*fDataF[k2] + fDataF[k2+1]*fDataF[k2+1]);
      newarray->AddAt(ck2/dim2,k);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  cn2 = (fDataF[1]*fDataF[1]);
  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}


// -------------------------------------------------
//
// Power Spectrum Density calculation for TArrayD
//
TArrayD* MFFT::PowerSpectrumDensity(const TArrayD *array)
{
  
  fDim = array->GetSize();
  CheckDim(fDim);

  fDataD.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = array->At(i);

  RealFTD(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t c02;
  Float_t ck2;
  Float_t cn2;
  
  TArrayD *newarray = new TArrayD(dim05);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  c02 = (fDataD[0]*fDataD[0]);
  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataD[k2]*fDataD[k2] + fDataD[k2+1]*fDataD[k2+1]);
      newarray->AddAt(ck2/dim2,k);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  cn2 = (fDataD[1]*fDataD[1]);
  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}

// -------------------------------------------------
//
// Power Spectrum Density calculation for MArrayF
// The difference to the TArrayF versions is that 
// the resulting array has two entries less, namely 
// the first and last one are skipped!
//
MArrayF* MFFT::PowerSpectrumDensity(const MArrayF *array)
{

  fDim = array->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = array->At(i);

  RealFTF(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t ck2;
  
  MArrayF *newarray = new MArrayF(dim05-2);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  //  c02 = (fDataF[0]*fDataF[0]);
  //  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataF[k2]*fDataF[k2] + fDataF[k2+1]*fDataF[k2+1]);
      newarray->AddAt(ck2/dim2,k-1);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  //  cn2 = (fDataF[1]*fDataF[1]);
  //  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}

//-----------------------------------------------------
//
// Power Spectrum Density calculation for MArrayI
// The difference to the TArrayI versions is that 
// the resulting array has two entries less, namely 
// the first and last one are skipped!
//
MArrayF* MFFT::PowerSpectrumDensity(const MArrayI *array)
{

  fDim = array->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = (Float_t)array->At(i);

  RealFTF(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t ck2;
  
  MArrayF *newarray = new MArrayF(dim05-2);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  //  c02 = (fDataF[0]*fDataF[0]);
  //  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataF[k2]*fDataF[k2] + fDataF[k2+1]*fDataF[k2+1]);
      newarray->AddAt(ck2/dim2,k-1);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  //  cn2 = (fDataF[1]*fDataF[1]);
  //  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}

// -------------------------------------------------
//
// Power Spectrum Density calculation for MArrayD
// The difference to the TArrayI versions is that 
// the resulting array has two entries less, namely 
// the first and last one are skipped!
//
MArrayD* MFFT::PowerSpectrumDensity(const MArrayD *array)
{
  
  fDim = array->GetSize();
  CheckDim(fDim);

  fDataD.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataD[i] = array->At(i);

  RealFTD(1);

  const Int_t dim2  = fDim*fDim;
  const Int_t dim05 = fDim/2;
  Float_t ck2;
  
  MArrayD *newarray = new MArrayD(dim05-2);

  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  //  c02 = (fDataD[0]*fDataD[0]);
  //  newarray->AddAt(c02/dim2,0);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=1;k<dim05-1;k++)
    {
      const Int_t k2 = k+k;
      ck2 = (fDataD[k2]*fDataD[k2] + fDataD[k2+1]*fDataD[k2+1]);
      newarray->AddAt(ck2/dim2,k-1);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  //  cn2 = (fDataD[1]*fDataD[1]);
  //  newarray->AddAt(cn2,dim05-1);
  
  return newarray;
}

// -----------------------------------------------
//
// Power Spectrum Density calculation for TH1
//
TH1F* MFFT::PowerSpectrumDensity(const TH1 *hist)
{

  TH1F *newhist = (TH1F*)CheckHist(hist,0);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = hist->GetBinContent(i);

  RealFTF(1);

  Int_t dim2 = fDim*fDim;
  Float_t c02;
  Float_t ck2;
  Float_t cn2;
  //
  // Fill the new histogram: 
  //
  // 1) P(0) = 1/(N*N) |C(0)|*|C(0)|
  //
  c02 = (fDataF[0]*fDataF[0]);
  newhist->Fill(0.,c02/dim2);
  //
  // 2) P(k) = 1/(N*N) (|C(k)|*|C(k)|))
  //
  for (Int_t k=2;k<fDim;k+=2)
    {
      ck2 = (fDataF[k]*fDataF[k] + fDataF[k+1]*fDataF[k+1]);
      newhist->Fill(k/2.,ck2/dim2);
    }
  //
  // 3) P(N) = 1/(N*N) (|C(n/2)|*|C(n/2)|)
  //
  cn2 = (fDataF[1]*fDataF[1]);
  newhist->Fill(fDim/2.-1.,cn2/dim2);
  
  return newhist;
}


//
// Power Spectrum Density calculation for TH1I
//
TH1F* MFFT::PowerSpectrumDensity(const TH1F *hist)
{
  return PowerSpectrumDensity((TH1*)hist);
}

//
// Power Spectrum Density calculation for TH1I
//
TH1F* MFFT::PowerSpectrumDensity(const TH1I *hist)
{
  return PowerSpectrumDensity((TH1*)hist);
}


void MFFT::CheckDim(Int_t a)
{

  // If even number, return 0
  if (a==2)  return;

  // If odd number, return the closest power of 2
  if (a & 1) 
    { 
      Int_t b = 1; 
      while (b < fDim/2+1)
        b <<= 1; 

      fDim = b;
      //      gLog << warn << "Dimension of Data is not a multiple of 2, will take only first " 
      //           << fDim << " entries! " << endl;
      return; 
    }

  CheckDim(a>>1);
}

TH1* MFFT::CheckHist(const TH1 *hist, const Int_t flag)
{
  
  // number of entries
  fDim = hist->GetNbinsX();
  CheckDim(fDim);

  // Step width
  Double_t delta = hist->GetBinWidth(1);
  
  // Nyquist frequency
  Axis_t fcrit = 1./(2.*delta);
  Axis_t low = -0.5;
  Axis_t up  = fcrit;

  switch (flag)
    {
    case 0: 
      return new TH1F(Form("%s%s",hist->GetName()," PSD"),
                      Form("%s%s",hist->GetTitle()," - Power Spectrum Density"),
                      fDim/2,low,up);
      break;
    case 1:
      return new TH1D(Form("%s%s",hist->GetName()," PSD"),
                      Form("%s%s",hist->GetTitle()," - Power Spectrum Density"),
                      fDim/2,low,up);
      break;
    default:
      return new TH1F(Form("%s%s",hist->GetName()," PSD"),
                      Form("%s%s",hist->GetTitle()," - Power Spectrum Density"),
                      fDim/2,low,up);
      break;
    }
}

//
// Real function spectrum with data windowing
//
TArrayF* MFFT::RealFunctionSpectrum(const TArrayF *data) 
{
  
  fDim = data->GetSize();
  CheckDim(fDim);

  fDataF.Set(fDim);
  //
  // Copy the hist into an array
  //
  for (Int_t i=0;i<fDim;i++)
    fDataF[i] = data->At(i);

  fWindowF.Set(fDim);

  Int_t dim2 = fDim/2;

  TArrayF *power = new TArrayF(dim2);

  // 
  // Start program spctrm from NR's
  //
  Float_t w, facp, facm, sumw=0.;
  
  facm = dim2;
  facp = 1./dim2;
  
  for (Int_t j=0;j<dim2;j++)
    {
      Int_t j2 = j+j;
      w     = ApplyWindow(j,facm,facp);
      sumw += w*w;
      fWindowF[j2]   = fDataF[j]*w;
      fWindowF[j2+1] = fDataF[dim2+j]*w;
    }
  
  TransformF(1,fWindowF);
  
  power->AddAt(fWindowF[0]*fWindowF[0] + fWindowF[1]*fWindowF[1],0);

  //  power->AddAt(fWindowF[0]*fWindowF[0],0);
  //  power->AddAt(fWindowF[1]*fWindowF[1],dim2-1);  


  for (Int_t j=1;j<dim2;j++)
    //  for (Int_t j=1;j<dim2;j++)
    {
      Int_t j2 = j+j;
      Float_t buf = fWindowF[j2+1]     *fWindowF[j2+1] 
                  + fWindowF[j2  ]     *fWindowF[j2  ] 
                  + fWindowF[fDim-j2+1]*fWindowF[fDim-j2+1] 
                  + fWindowF[fDim-j2  ]*fWindowF[fDim-j2  ] ;
      power->AddAt(buf/sumw/(fDim+fDim),j);
    }
  
  return power;

}
