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
!   Author(s): Markus Gaug 02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHCalibrationChargeBlindPix
//
//  Histogram class for the charge calibration of the Blind Pixel.
//  Stores and fits the charges and stores the averaged assumed pedestal and 
//  single-phe FADC slice entries. Charges are taken from MExtractedSignalBlindPix.
//  Performs the Single Photo-electron fit to extract the Poisson mean and its errors.
//
//  Different fits can be chosen with the function SetFitFunc().
//
//  The fit result is accepted under the condition that:
//  1) the Probability is greater than fProbLimit (default 0.001 == 99.7%)
//  2) at least fNumSinglePheLimit events are found in the single Photo-electron peak
//
//  The single FADC slice entries are averaged and stored in fASinglePheFADCSlices, if 
//  their sum exceeds fSinglePheCut, otherwise in fAPedestalFADCSlices.
//
//  Used numbers are the following:
//
//  Electronic conversion factor:
//   Assume, we have N_e electrons at the anode, 
//   thus a charge of N_e*e (e = electron charge) Coulomb.
//
//   This charge is AC coupled and runs into a R_pre = 50 Ohm resistency. 
//   The corresponding current is amplified by a gain factor G_pre = 400 
//   (the precision of this value still has to be checked !!!) and again AC coupled to 
//   the output. 
//   The corresponding signal goes through the whole transmission and 
//   amplification chain and is digitized in the FADCs. 
//   The conversion Signal Area to FADC counts (Conv_trans) has been measured 
//   by David and Oscar to be approx. 3.9 pVs^-1
//
//   Thus: Conversion FADC counts to Number of Electrons at Anode: 
//         FADC counts = (1/Conv_tran) * G_pre * R_pre *  e * N_e = 8 * 10^-4 N_e. 
//
//   Also: FADC counts = 8*10^-4 * GAIN * N_phe
//
//   In the blind pixel, there is an additional pre-amplifier with an amplification of 
//   about 10. Therefore, we have for the blind pixel:
//
//
//   FADC counts (Blind Pixel) = 8*10^-3 * GAIN * N_phe
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationChargeBlindPix.h"

#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TGraph.h>

#include <TVector.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2D.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtPixelIter.h"

ClassImp(MHCalibrationChargeBlindPix);

using namespace std;

const Double_t MHCalibrationChargeBlindPix::gkElectronicAmp      = 0.008;
const Double_t MHCalibrationChargeBlindPix::gkElectronicAmpErr   = 0.002;
const Float_t  MHCalibrationChargeBlindPix::gkSignalInitializer  = -9999.;
const Float_t  MHCalibrationChargeBlindPix::fgNumSinglePheLimit  =  50.;
// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - the default number for fNumSinglePheLimit (fgNumSinglePheLimit)
// - the default number of bins after stripping (30)
//
// Initializes:
// - all pointers to NULL
// - fASinglePheFADCSlices(0);
// - fAPedestalFADCSlices(0);
//
// Calls:
// - Clear()
//
MHCalibrationChargeBlindPix::MHCalibrationChargeBlindPix(const char *name, const char *title)
    :  fSinglePheFit(NULL), 
       fFitLegend(NULL),
       fHSinglePheFADCSlices(NULL), fHPedestalFADCSlices(NULL)
{

    fName  = name  ? name  : "MHCalibrationChargeBlindPix";
    fTitle = title ? title : "Statistics of the FADC sums of Blind Pixel calibration events";

    //fASinglePheFADCSlices.ResizeTo(1);
    //fAPedestalFADCSlices.ResizeTo(1);

    SetNumSinglePheLimit();
    SetProbLimit(0.001);
    SetBinsAfterStripping(0);

    Clear();
}

// --------------------------------------------------------------------------
//
// Default Destructor. 
//
// Deletes (if Pointer is not NULL):
// 
// - fFitLegend 
// - fHSinglePheFADCSlices
// - fHPedestalFADCSlices    
// - fSinglePheFit
// 
MHCalibrationChargeBlindPix::~MHCalibrationChargeBlindPix()
{

  //
  // The next two lines are important for the case that 
  // the class has been stored to a file and is read again. 
  // In this case, the next two lines prevent a segm. violation
  // in the destructor
  //
  gROOT->GetListOfFunctions()->Remove(fSinglePheFit);

  if (fSinglePheFit)
      delete fSinglePheFit;
  
  if (fFitLegend)
    delete fFitLegend;

  if (fHSinglePheFADCSlices)
    delete fHSinglePheFADCSlices;

  if (fHPedestalFADCSlices)
    delete fHPedestalFADCSlices;

}

// --------------------------------------------------------------------------
//
// Sets:
// - all variables to 0., except the fit result variables to gkSignalInitializer
// - all flags to kFALSE
// - all pointers to NULL
// - the default fit function (kEPoisson5)
//
// Deletes: 
// - all pointers unequal NULL
//
// Calls:
// - MHCalibrationChargePix::Clear()
//
void MHCalibrationChargeBlindPix::Clear(Option_t *o)
{

  fLambda    = gkSignalInitializer;
  fMu0       = gkSignalInitializer;
  fMu1       = gkSignalInitializer;
  fSigma0    = gkSignalInitializer;
  fSigma1    = gkSignalInitializer;
  fLambdaErr = gkSignalInitializer;
  fMu0Err    = gkSignalInitializer;
  fMu1Err    = gkSignalInitializer;
  fSigma0Err = gkSignalInitializer;
  fSigma1Err = gkSignalInitializer;

  fLambdaCheck    = gkSignalInitializer;
  fLambdaCheckErr = gkSignalInitializer;
  
  fFitFunc = kEPoisson5;

  fNumSinglePhes    = 0;
  fNumPedestals     = 0;

  fChisquare        = 0.;
  fNDF              = 0 ;
  fProb             = 0.;

  SetSinglePheFitOK ( kFALSE );
  SetPedestalFitOK  ( kFALSE );

  if (fFitLegend)
  {
    delete fFitLegend;
    fFitLegend = NULL;
  }

  if (fSinglePheFit)
  {
    delete fSinglePheFit;
    fSinglePheFit = NULL;
  }

  if (fHSinglePheFADCSlices)
  {
    delete fHSinglePheFADCSlices;
    fHSinglePheFADCSlices = NULL;
  }

  if (fHPedestalFADCSlices)
  {
    delete fHPedestalFADCSlices;
    fHPedestalFADCSlices = NULL;
  }

  MHCalibrationPix::Clear();
  return;
}

/*
// --------------------------------------------------------------------------
//
// Our own clone function is necessary since root 3.01/06 or Mars 0.4
// I don't know the reason. 
//
// Creates new MHCalibrationCam
//
TObject *MHCalibrationChargeBlindPix::Clone(const char *) const
{

    MHCalibrationChargeBlindPix *pix = new MHCalibrationChargeBlindPix();
    this->Copy(*pix);

    this->fHGausHist.Copy(pix->fHGausHist);
    this->fSinglePheFit->Copy(*(pix->fSinglePheFit));
    this->fHSinglePheFADCSlices->Copy(*(pix->fHSinglePheFADCSlices));
    this->fHPedestalFADCSlices->Copy(*(pix->fHPedestalFADCSlices));
    

    return pix;
}
*/

// --------------------------------------------------------------------------
//
// Set bit kSinglePheFitOK from outside
//
void MHCalibrationChargeBlindPix::SetSinglePheFitOK (const Bool_t b ) 
{
    b ? SETBIT(fFlags,kSinglePheFitOK) : CLRBIT(fFlags,kSinglePheFitOK);
}

// --------------------------------------------------------------------------
//
// Set bit kPedestalFitOK from outside
//
void MHCalibrationChargeBlindPix::SetPedestalFitOK(const Bool_t b)
{
    b ? SETBIT(fFlags,kPedestalFitOK) : CLRBIT(fFlags,kPedestalFitOK);
}

// --------------------------------------------------------------------------
//
// Ask for status of bit kSinglePheFitOK 
//
const Bool_t  MHCalibrationChargeBlindPix::IsSinglePheFitOK()     const 
{
    return TESTBIT(fFlags,kSinglePheFitOK);
}

// --------------------------------------------------------------------------
//
// Ask for status of bit kPedestalFitOK 
//
const Bool_t  MHCalibrationChargeBlindPix::IsPedestalFitOK()  const
{
    return TESTBIT(fFlags,kPedestalFitOK);
}

/*
// --------------------------------------------------------------------------
//
// Normalizes fASinglePheFADCSlices and fAPedestalFADCSlices
//
void MHCalibrationChargeBlindPix::FinalizeSinglePheSpectrum() 
{
  
  if (fNumSinglePhes > 1)
      for (Int_t i=0;i<fASinglePheFADCSlices.GetNrows();i++)
	  fASinglePheFADCSlices[i] = fASinglePheFADCSlices[i]/fNumSinglePhes;
  if (fNumPedestals > 1)
      for (Int_t i=0;i<fAPedestalFADCSlices.GetNrows();i++)
        fAPedestalFADCSlices[i]  = fAPedestalFADCSlices[i]/fNumPedestals;
}

// --------------------------------------------------------------------------
//
// Checks again for the size and fills fASinglePheFADCSlices with the FADC slice entries
// 
void MHCalibrationChargeBlindPix::FillSinglePheFADCSlices(const MRawEvtPixelIter &iter)
{

  const Int_t n = iter.GetNumHiGainSamples() + iter.GetNumLoGainSamples();

  if (fASinglePheFADCSlices.GetNrows() < n)
      fASinglePheFADCSlices.ResizeTo(n);
  
  Int_t i=0;
  
  Byte_t *start = iter.GetHiGainSamples();
  Byte_t *end   = start + iter.GetNumHiGainSamples();

  for (Byte_t *ptr = start; ptr < end; ptr++, i++)
      fASinglePheFADCSlices(i) = fASinglePheFADCSlices(i) + (Float_t)*ptr;

  start = iter.GetLoGainSamples();
  end   = start + iter.GetNumLoGainSamples();

  for (Byte_t *ptr = start; ptr < end; ptr++, i++)
      fASinglePheFADCSlices(i) = fASinglePheFADCSlices(i) + (Float_t)*ptr;

  fNumSinglePhes++;
}

// --------------------------------------------------------------------------
//
// Checks again for the size and fills fAPedestalFADCSlices with the FADC slice entries
// 
void MHCalibrationChargeBlindPix::FillPedestalFADCSlices(const MRawEvtPixelIter &iter)
{

  const Int_t n = iter.GetNumHiGainSamples() + iter.GetNumLoGainSamples();

  if (fAPedestalFADCSlices.GetNrows() < n)
      fAPedestalFADCSlices.ResizeTo(n);

  Int_t i = 0;
  Byte_t *start = iter.GetHiGainSamples();
  Byte_t *end   = start + iter.GetNumHiGainSamples();

  for (Byte_t *ptr = start; ptr < end; ptr++, i++)
      fAPedestalFADCSlices(i) = fAPedestalFADCSlices(i)+ (Float_t)*ptr;

  start = iter.GetLoGainSamples();
  end   = start + iter.GetNumLoGainSamples();

  for (Byte_t *ptr = start; ptr < end; ptr++, i++)
      fAPedestalFADCSlices(i) = fAPedestalFADCSlices(i)+ (Float_t)*ptr;

  fNumPedestals++;
}
*/

// --------------------------------------------------------------------------
//
// Task to simulate single phe spectrum with the given parameters
// 
Bool_t MHCalibrationChargeBlindPix::SimulateSinglePhe(Double_t lambda, Double_t mu0, Double_t mu1, Double_t sigma0, Double_t sigma1) 
{

  gRandom->SetSeed();

  if (fHGausHist.GetIntegral() != 0)
    {
      *fLog << err << "Histogram " << fHGausHist.GetTitle() << " is already filled. " << endl;
      *fLog << err << "Create new class MHCalibrationBlindPixel for simulation! " << endl;
      return kFALSE;
    }

  if (!InitFit())
    return kFALSE;

  for (Int_t i=0;i<10000; i++) 
    fHGausHist.Fill(fSinglePheFit->GetRandom());
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// - Get the ranges from the stripped histogram
// - choose reasonable start values for the fit
// - initialize the fit function depending on fFitFunc
// - initialize parameter names and limits depending on fFitFunc
//
Bool_t MHCalibrationChargeBlindPix::InitFit()
{
  
  //
  // Get the fitting ranges
  //
  Axis_t rmin = fHGausHist.GetBinCenter(fHGausHist.GetXaxis()->GetFirst());
  Axis_t rmax = fHGausHist.GetBinCenter(fHGausHist.GetXaxis()->GetLast()); 

  if (rmin < 0.)
      rmin = 0.;

  //
  // First guesses for the fit (should be as close to reality as possible, 
  // otherwise the fit goes gaga because of high number of dimensions ...
  //
  const Stat_t   entries      = fHGausHist.Integral("width");
  const Double_t lambda_guess = 0.5;
  //const Double_t maximum_bin  = fHGausHist.GetBinCenter(fHGausHist.GetMaximumBin());
  const Double_t norm         = entries/TMath::Sqrt(TMath::TwoPi());

  //
  // Initialize the fit function
  //
  switch (fFitFunc)
    {
    case kEPoisson4:
      fSinglePheFit = new TF1("SinglePheFit",&PoissonKto4,rmin,rmax,6);
      rmin += 6.5;
      break;
    case kEPoisson5:
      fSinglePheFit = new TF1("SinglePheFit",&PoissonKto5,rmin,rmax,6);
      rmin = 0.;
      break;
    case kEPoisson6:
      fSinglePheFit = new TF1("SinglePheFit",&PoissonKto6,rmin,rmax,6);
      break;
    case kEPolya:
      fSinglePheFit = new TF1("SinglePheFit",&Polya,rmin,rmax,8);
      break;
    case kEMichele:
      fSinglePheFit = new TF1("SinglePheFit",&FitFuncMichele,rmin,rmax,9);
      break;
    default:
      *fLog << warn << "WARNING: Could not find Fit Function for Blind Pixel " << endl;
      return kFALSE;
      break;
    }

  if (!fSinglePheFit) 
  {
      *fLog << warn << dbginf << "WARNING: Could not create fit function for Single Phe fit" << endl;
      return kFALSE;
  }

  //
  // For the fits, we have to take special care since ROOT 
  // has stored the function pointer in a global list which 
  // lead to removing the object twice. We have to take out 
  // the following functions of the global list of functions 
  // as well:
  // 
  gROOT->GetListOfFunctions()->Remove(fSinglePheFit);
  
  const Double_t mu_0_guess = 13.5;
  const Double_t si_0_guess = 2.5;
  const Double_t mu_1_guess = 30.;
  const Double_t si_1_guess = si_0_guess + si_0_guess;
  // Michele
  const Double_t lambda_1cat_guess = 1.00;
  const Double_t lambda_1dyn_guess = lambda_1cat_guess/10.;
  const Double_t mu_1cat_guess = 50.;
  const Double_t mu_1dyn_guess = 17.;
  const Double_t si_1cat_guess = si_0_guess + si_0_guess;
  const Double_t si_1dyn_guess = si_0_guess + si_0_guess/2.;
  // Polya
  const Double_t excessPoisson_guess = 0.5;
  const Double_t delta1_guess     = 8.;
  const Double_t delta2_guess     = 5.;
  const Double_t electronicAmp_guess  = gkElectronicAmp;
  const Double_t electronicAmp_limit  = gkElectronicAmpErr;

  //
  // Initialize boundaries and start parameters
  //
  switch (fFitFunc)
    {
      
    case kEPoisson4:
	fSinglePheFit->SetParNames(  "#lambda",   "#mu_{0}",    "#mu_{1}", "#sigma_{0}",  "#sigma_{1}","Area");
        fSinglePheFit->SetParameters(lambda_guess,mu_0_guess,mu_1_guess,si_0_guess,si_1_guess,norm);
	fSinglePheFit->SetParLimits(0,0.,2.);
        fSinglePheFit->SetParLimits(1,10.,17.);
	fSinglePheFit->SetParLimits(2,17.,50.);
        fSinglePheFit->SetParLimits(3,1.,5.);
	fSinglePheFit->SetParLimits(4,5.,30.);
	fSinglePheFit->SetParLimits(5,norm-(0.5*norm),norm+(0.7*norm));
	break;
    case kEPoisson5:
    case kEPoisson6:
      fSinglePheFit->SetParNames("#lambda","#mu_{0}","#mu_{1}","#sigma_{0}","#sigma_{1}","Area");
      fSinglePheFit->SetParameters(lambda_guess,mu_0_guess,800.,si_0_guess,500.,norm);
      fSinglePheFit->SetParLimits(0,0.,2.);
      fSinglePheFit->SetParLimits(1,0.,100.);
      fSinglePheFit->SetParLimits(2,300.,1500.);
      fSinglePheFit->SetParLimits(3,30.,250.);
      fSinglePheFit->SetParLimits(4,100.,1000.);
      fSinglePheFit->SetParLimits(5,norm/1.5,norm*1.5);
      break;

    case kEPolya:
        fSinglePheFit->SetParameters(lambda_guess, excessPoisson_guess,
                                     delta1_guess,delta2_guess,
                                     electronicAmp_guess,
                                     10.,
                                     norm, 
                                     0.);
      fSinglePheFit->SetParNames("#lambda","b_{tot}",
                                 "#delta_{1}","#delta_{2}",
                                 "amp_{e}","#sigma_{0}",
                                 "Area", "#mu_{0}");
      fSinglePheFit->SetParLimits(0,0.,1.);
      fSinglePheFit->SetParLimits(1,0.,1.); 
      fSinglePheFit->SetParLimits(2,6.,12.);    
      fSinglePheFit->SetParLimits(3,3.,8.);    
      fSinglePheFit->SetParLimits(4,electronicAmp_guess-electronicAmp_limit,
                                    electronicAmp_guess+electronicAmp_limit);    
      fSinglePheFit->SetParLimits(5,0.,40.);
      fSinglePheFit->SetParLimits(6,norm-0.1,norm+0.1);
      fSinglePheFit->SetParLimits(7,-10.,10.);
      break;
    case kEMichele:
      fSinglePheFit->SetParNames("#lambda_{cat}","#lambda_{dyn}",
                                 "#mu_{0}","#mu_{1cat}","#mu_{1dyn}",
                                 "#sigma_{0}","#sigma_{1cat}","#sigma_{1dyn}",
                                 "Area");
      fSinglePheFit->SetParameters(lambda_1cat_guess, lambda_1dyn_guess, 
                                   mu_0_guess, mu_1cat_guess,mu_1dyn_guess,
                                   si_0_guess, si_1cat_guess,si_1dyn_guess,
                                   norm);
      fSinglePheFit->SetParLimits(0,0.01,2.0);
      fSinglePheFit->SetParLimits(1,0.,0.5); 
      fSinglePheFit->SetParLimits(2,10.,16.);    
      fSinglePheFit->SetParLimits(3,25.,50.);    
      fSinglePheFit->SetParLimits(4,16.,18.5);    
      fSinglePheFit->SetParLimits(5,1.,5.);    
      fSinglePheFit->SetParLimits(6,10.,50.);    
      fSinglePheFit->SetParLimits(7,5.,10.);    
      fSinglePheFit->SetParLimits(8,norm/2.,norm*2.5);
      break;

    default:
      *fLog << warn << "WARNING: Could not find Fit Function for Blind Pixel " << endl;
      return kFALSE;
      break;
    }

  fSinglePheFit->SetRange(rmin,rmax);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// - Retrieve the parameters depending on fFitFunc
// - Retrieve probability, Chisquare and NDF
//
void MHCalibrationChargeBlindPix::ExitFit()
{
  

  //
  // Finalize
  //
  switch (fFitFunc)
    {
      
    case kEPoisson4:
    case kEPoisson5:
    case kEPoisson6:
    case kEPoisson7:
      fLambda = fSinglePheFit->GetParameter(0);
      fMu0    = fSinglePheFit->GetParameter(1);
      fMu1    = fSinglePheFit->GetParameter(2);
      fSigma0 = fSinglePheFit->GetParameter(3);
      fSigma1 = fSinglePheFit->GetParameter(4);
      
      fLambdaErr = fSinglePheFit->GetParError(0);
      fMu0Err    = fSinglePheFit->GetParError(1);
      fMu1Err    = fSinglePheFit->GetParError(2);
      fSigma0Err = fSinglePheFit->GetParError(3);
      fSigma1Err = fSinglePheFit->GetParError(4);
      break;
    case kEPolya:
      fLambda =  fSinglePheFit->GetParameter(0);
      fMu0    =  fSinglePheFit->GetParameter(7);
      fMu1    = 0.;
      fSigma0 =  fSinglePheFit->GetParameter(5);
      fSigma1 = 0.;

      fLambdaErr = fSinglePheFit->GetParError(0);
      fMu0Err    = fSinglePheFit->GetParError(7);
      fMu1Err    = 0.;
      fSigma0Err = fSinglePheFit->GetParError(5);
      fSigma1Err = 0.;
    case kEMichele:
      fLambda =  fSinglePheFit->GetParameter(0);
      fMu0    =  fSinglePheFit->GetParameter(2);
      fMu1    =  fSinglePheFit->GetParameter(3);
      fSigma0 =  fSinglePheFit->GetParameter(5);
      fSigma1 =  fSinglePheFit->GetParameter(6);

      fLambdaErr = fSinglePheFit->GetParError(0);
      fMu0Err    = fSinglePheFit->GetParError(2);
      fMu1Err    = fSinglePheFit->GetParError(3);
      fSigma0Err = fSinglePheFit->GetParError(5);
      fSigma1Err = fSinglePheFit->GetParError(6);
      break;
    default:
      break;
    }

  fProb      = fSinglePheFit->GetProb();
  fChisquare = fSinglePheFit->GetChisquare();
  fNDF       = fSinglePheFit->GetNDF();

  *fLog << all << "Results of the Blind Pixel Fit: "              << endl;
  *fLog << all << "Chisquare:   " << fChisquare                   << endl;
  *fLog << all << "DoF:         " << fNDF                         << endl;
  *fLog << all << "Probability: " << fProb                        << endl;

}

// --------------------------------------------------------------------------
//
// - Executes InitFit()
// - Fits the fHGausHist with fSinglePheFit
// - Executes ExitFit()
//
// The fit result is accepted under condition:
// 1) The results are not nan's
// 2) The NDF is not smaller than fNDFLimit (5)
// 3) The Probability is greater than fProbLimit (default 0.001 == 99.9%)
// 4) at least fNumSinglePheLimit events are in the single Photo-electron peak
//
Bool_t MHCalibrationChargeBlindPix::FitSinglePhe(Option_t *opt) 
{

  if (!InitFit())
      return kFALSE;

  fHGausHist.Fit(fSinglePheFit,opt);

  ExitFit();

  //
  // The fit result is accepted under condition:
  // 1) The results are not nan's
  // 2) The NDF is not smaller than fNDFLimit (5)
  // 3) The Probability is greater than fProbLimit (default 0.001 == 99.9%)
  // 4) at least fNumSinglePheLimit events are in the single Photo-electron peak
  //
  // !Finitite means either infinite or not-a-number
  if (   !TMath::Finite(fLambda)
      || !TMath::Finite(fLambdaErr)
      || !TMath::Finite(fProb)
      || !TMath::Finite(fMu0)
      || !TMath::Finite(fMu0Err)
      || !TMath::Finite(fMu1)
      || !TMath::Finite(fMu1Err)
      || !TMath::Finite(fSigma0)
      || !TMath::Finite(fSigma0Err)
      || !TMath::Finite(fSigma1)
      || !TMath::Finite(fSigma1Err)
      || fNDF  < GetNDFLimit()
      || fProb < GetProbLimit() )
    return kFALSE;

  const Stat_t   entries      = fHGausHist.Integral("width");
  const Float_t  numSinglePhe = TMath::Exp(-1.0*fLambda)*fLambda*entries;
  
  if (numSinglePhe < fNumSinglePheLimit) 
    {
      *fLog << warn << "WARNING - Statistics is too low: Only " << numSinglePhe
            << " in the Single Photo-Electron peak " << endl;
      return kFALSE;
    } 
  else
    *fLog << all << numSinglePhe << " in Single Photo-Electron peak " << endl;
  
  SetSinglePheFitOK();
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// - Retrieves limits for the fit
// - Fits the fHGausHist with Gauss 
// - Retrieves the results to fLambdaCheck and fLambdaCheckErr
// - Sets a flag IsPedestalFitOK()
//
void MHCalibrationChargeBlindPix::FitPedestal  (Option_t *opt)
{

  // Perform the cross-check fitting only the pedestal:
  const Axis_t rmin    = 0.;
  //  const Axis_t rmax    = fHGausHist.GetBinCenter(fHGausHist.GetMaximumBin());
  const Axis_t rmax = fSinglePheCut;

  FitGaus(opt, rmin, rmax);

  const Stat_t   entries = fHGausHist.Integral("width");
  const Double_t pedarea = fFGausFit->Integral(0.,fSinglePheCut);

  fLambdaCheck     = TMath::Log(entries/pedarea);
  // estimate the error by the error of the obtained area from the Gauss-function:
  fLambdaCheckErr  = fFGausFit->GetParError(0)/fFGausFit->GetParameter(0);
  
  SetPedestalFitOK(IsGausFitOK());
  return;
}

 
// -------------------------------------------------------------------------
//
// Draw a legend with the fit results
//
void MHCalibrationChargeBlindPix::DrawLegend(Option_t *opt)
{

  TString option(opt);

  if (!fFitLegend)
  {
      fFitLegend = new TPaveText(0.05,0.05,0.95,0.95);
      fFitLegend->SetLabel(Form("%s%s", "Results of the single PhE Fit (",
				(fFitFunc ==  kEPoisson4) ? "Poisson(k=4))" : 
				(fFitFunc ==  kEPoisson5) ? "Poisson(k=5))" : 
				(fFitFunc ==  kEPoisson6) ? "Poisson(k=6))" :
				(fFitFunc ==  kEPolya   ) ? "Polya(k=4))"   : 
				(fFitFunc ==  kEMichele ) ?  "Michele)"     
                                                                                       : " none )" ));
      fFitLegend->SetTextSize(0.05);
  }
  else
      fFitLegend->Clear();

  const TString line1 = 
      Form("Mean: #lambda = %2.2f #pm %2.2f",fLambda,fLambdaErr);
  TText *t1 = fFitLegend->AddText(line1.Data());
  t1->SetBit(kCanDelete);
      
  const TString line6 =
      Form("Mean #lambda_{check} = %2.2f #pm %2.2f",fLambdaCheck,fLambdaCheckErr);
  TText *t2 = fFitLegend->AddText(line6.Data());
  t2->SetBit(kCanDelete);

  if (option.Contains("datacheck"))
    {
      if (fLambda + 3.*fLambdaErr < fLambdaCheck - 3.*fLambdaCheckErr 
          || 
          fLambda - 3.*fLambdaErr > fLambdaCheck + 3.*fLambdaCheckErr )
        {
          TText *t = fFitLegend->AddText("#lambda and #lambda_{check} more than 3#sigma apart!");
          t->SetBit(kCanDelete);
        }
    }
  else
    {

      const TString line2 = 
        Form("Pedestal: #mu_{0} = %2.2f #pm %2.2f",fMu0,fMu0Err);
      TText *t3 = fFitLegend->AddText(line2.Data());
      t3->SetBit(kCanDelete);
      
      const TString line3 =
        Form("Width Pedestal: #sigma_{0} = %2.2f #pm %2.2f",fSigma0,fSigma0Err);
      TText *t4 = fFitLegend->AddText(line3.Data());
      t4->SetBit(kCanDelete);
      
      const TString line4 =
        Form("1^{st} Phe-peak: #mu_{1} = %2.2f #pm %2.2f",fMu1,fMu1Err);
      TText *t5 = fFitLegend->AddText(line4.Data());
      t5->SetBit(kCanDelete);
      
      const TString line5 =
        Form("Width 1^{st} Phe-peak: #sigma_{1} = %2.2f #pm %2.2f",fSigma1,fSigma1Err);
      TText *t6 = fFitLegend->AddText(line5.Data());
      t6->SetBit(kCanDelete);
    }
  
  const TString line7 =
    Form("#chi^{2} / N_{dof}: %4.2f / %3i",fChisquare,fNDF);
  TText *t7 = fFitLegend->AddText(line7.Data());
  t7->SetBit(kCanDelete);
  
  const TString line8 =
      Form("Probability: %6.4f ",fProb);
  TText *t8 = fFitLegend->AddText(line8.Data());
  t8->SetBit(kCanDelete);
  
  if (IsSinglePheFitOK())
  {
      TText *t = fFitLegend->AddText("Result of the Fit: OK");
      t->SetBit(kCanDelete);
  }
  else
  {
      TText *t = fFitLegend->AddText("Result of the Fit: NOT OK");
      t->SetBit(kCanDelete);
  }

  fFitLegend->SetFillColor(IsSinglePheFitOK() ? 80 : 2);
  fFitLegend->Draw();
  
  return;
}


// -------------------------------------------------------------------------
//
// Draw the histogram
//
// The following options can be chosen:
//
// "": displays the fHGausHist, the fits, the legend and fASinglePheFADCSlices and fAPedestalFADCSlices
// "all": executes additionally MHGausEvents::Draw(), with option "fourierevents"
// "datacheck" display the fHGausHist, the fits and the legend
//
void MHCalibrationChargeBlindPix::Draw(Option_t *opt) 
{

  TString option(opt);
  option.ToLower();
  
  Int_t win = 1;

  TVirtualPad *oldpad = gPad ? gPad : MH::MakeDefCanvas(this,900, 600);
  TVirtualPad *pad    = NULL;

  if (option.Contains("all"))
  {
      option.ReplaceAll("all","");
      oldpad->Divide(2,1);
      win = 2;
      oldpad->cd(1);
      TVirtualPad *newpad = gPad;
      pad = newpad;
      pad->Divide(2,2);
      pad->cd(1);
  }
  else if (option.Contains("datacheck"))
    {
      pad = oldpad;
      pad->Divide(1,2);
      pad->cd(1);
      fHGausHist.SetStats(0);
    }
  else
  {
      pad = oldpad;
      pad->Divide(2,2);
      pad->cd(1);
  }

  if (!IsEmpty() && !IsOnlyOverflow() && !IsOnlyUnderflow())
    gPad->SetLogy();

  gPad->SetTicks();

  fHGausHist.Draw(); 
  if (fFGausFit )
  {
      fFGausFit->SetLineColor(kBlue);
      fFGausFit->Draw("same");
      if (!option.Contains("datacheck"))
        {
          TLine *line = new TLine(fSinglePheCut, 0., fSinglePheCut, 10.);
          line->SetBit(kCanDelete);
          line->SetLineColor(kBlue);
          line->SetLineWidth(3);
          line->DrawLine(fSinglePheCut, 0., fSinglePheCut, 2.);
        }
  }
  
  if (fSinglePheFit)
  {    
    fSinglePheFit->SetFillStyle(0);
    fSinglePheFit->SetLineWidth(3);
    fSinglePheFit->SetLineColor(IsSinglePheFitOK() ? kGreen : kRed);          
    fSinglePheFit->Draw("same");
  }

  pad->cd(2);
  DrawLegend(option.Data());

  if (option.Contains("datacheck"))
    return;
/*
  pad->cd(3);
  if (fASinglePheFADCSlices.GetNrows()!=1)
    {
      if (fHSinglePheFADCSlices)
        delete fHSinglePheFADCSlices;

      fHSinglePheFADCSlices = new TH1F(fASinglePheFADCSlices);
      fHSinglePheFADCSlices->SetName("SinglePheFADCSlices");
      fHSinglePheFADCSlices->SetTitle(Form("%s%4.1f","Assumed Single Phe FADC Slices, Sum > ",fSinglePheCut));
      fHSinglePheFADCSlices->SetXTitle("FADC slice number");
      fHSinglePheFADCSlices->SetYTitle("FADC counts");
      const Int_t nbins = fHSinglePheFADCSlices->GetNbinsX();
      TH2D *nulls = new TH2D("Nulls",fHSinglePheFADCSlices->GetTitle(),nbins,0.,
                            fHSinglePheFADCSlices->GetXaxis()->GetBinCenter(nbins),
                            100,0.,50.);
      nulls->SetDirectory(NULL);
      nulls->SetBit(kCanDelete);
      nulls->GetXaxis()->SetTitle(fHSinglePheFADCSlices->GetXaxis()->GetTitle());
      nulls->GetYaxis()->SetTitle(fHSinglePheFADCSlices->GetYaxis()->GetTitle());  
      nulls->GetXaxis()->CenterTitle();
      nulls->GetYaxis()->CenterTitle();
      nulls->SetStats(0);
      nulls->Draw();
      fHSinglePheFADCSlices->Draw("same");
    }
  
  pad->cd(4);
  if (fAPedestalFADCSlices.GetNrows()!=1)
    {

      if (fHPedestalFADCSlices)
        delete fHPedestalFADCSlices;
      
      fHPedestalFADCSlices = new TH1F(fAPedestalFADCSlices);
      fHPedestalFADCSlices->SetName("PedestalFADCSlices");
      fHPedestalFADCSlices->SetTitle(Form("%s%4.1f","Pedestal FADC Slices, Sum < ",fSinglePheCut));
      fHPedestalFADCSlices->SetXTitle("FADC slice number");
      fHPedestalFADCSlices->SetYTitle("FADC counts");
      const Int_t nbins = fHPedestalFADCSlices->GetNbinsX();
      TH2D *nullp = new TH2D("Nullp",fHPedestalFADCSlices->GetTitle(),nbins,0.,
                            fHPedestalFADCSlices->GetXaxis()->GetBinCenter(nbins),
                            100,0.,50.);
      nullp->SetDirectory(NULL);
      nullp->SetBit(kCanDelete);
      nullp->GetXaxis()->SetTitle(fHPedestalFADCSlices->GetXaxis()->GetTitle());
      nullp->GetYaxis()->SetTitle(fHPedestalFADCSlices->GetYaxis()->GetTitle());  
      nullp->GetXaxis()->CenterTitle();
      nullp->GetYaxis()->CenterTitle();
      nullp->SetStats(0);
      nullp->Draw();
      fHPedestalFADCSlices->Draw("same");
    }
  */
  if (win < 2)
  return;

  oldpad->cd(2);
  MHCalibrationPix::Draw("fourierevents");
}

Double_t MHCalibrationChargeBlindPix::FitFuncMichele(Double_t *x, Double_t *par)
{
    Double_t lambda1cat = par[0];
    Double_t lambda1dyn = par[1];
    Double_t mu0        = par[2];
    Double_t mu1cat     = par[3];
    Double_t mu1dyn     = par[4];
    Double_t sigma0     = par[5];
    Double_t sigma1cat  = par[6];
    Double_t sigma1dyn  = par[7];

    Double_t sumcat = 0.;
    Double_t sumdyn = 0.;
    Double_t arg    = 0.;

    if (lambda1cat < lambda1dyn)
        return FLT_MAX;

    if (mu1cat    < mu0)
        return FLT_MAX;

    if (mu1dyn    < mu0)
        return FLT_MAX;

    if (mu1cat < mu1dyn)
        return FLT_MAX;

    if (sigma0 < 0.0001)
        return FLT_MAX;

    if (sigma1cat < sigma0)
        return FLT_MAX;

    if (sigma1dyn < sigma0)
        return FLT_MAX;

    Double_t mu2cat = (2.*mu1cat)-mu0;
    Double_t mu2dyn = (2.*mu1dyn)-mu0;
    Double_t mu3cat = (3.*mu1cat)-(2.*mu0);
    Double_t mu3dyn = (3.*mu1dyn)-(2.*mu0);

    Double_t sigma2cat = TMath::Sqrt((2.*sigma1cat*sigma1cat) - (sigma0*sigma0));
    Double_t sigma2dyn = TMath::Sqrt((2.*sigma1dyn*sigma1dyn) - (sigma0*sigma0));
    Double_t sigma3cat = TMath::Sqrt((3.*sigma1cat*sigma1cat) - (2.*sigma0*sigma0));
    Double_t sigma3dyn = TMath::Sqrt((3.*sigma1dyn*sigma1dyn) - (2.*sigma0*sigma0));

    Double_t lambda2cat = lambda1cat*lambda1cat;
    Double_t lambda2dyn = lambda1dyn*lambda1dyn;
    Double_t lambda3cat = lambda2cat*lambda1cat;
    Double_t lambda3dyn = lambda2dyn*lambda1dyn;

    // k=0:
    arg = (x[0] - mu0)/sigma0;
    sumcat = TMath::Exp(-0.5*arg*arg)/sigma0;
    sumdyn = sumcat;

    // k=1cat:
    arg = (x[0] - mu1cat)/sigma1cat;
    sumcat += lambda1cat*TMath::Exp(-0.5*arg*arg)/sigma1cat;
    // k=1dyn:
    arg = (x[0] - mu1dyn)/sigma1dyn;
    sumdyn += lambda1dyn*TMath::Exp(-0.5*arg*arg)/sigma1dyn;

    // k=2cat:
    arg = (x[0] - mu2cat)/sigma2cat;
    sumcat += 0.5*lambda2cat*TMath::Exp(-0.5*arg*arg)/sigma2cat;
    // k=2dyn:
    arg = (x[0] - mu2dyn)/sigma2dyn;
    sumdyn += 0.5*lambda2dyn*TMath::Exp(-0.5*arg*arg)/sigma2dyn;


    // k=3cat:
    arg = (x[0] - mu3cat)/sigma3cat;
    sumcat += 0.1666666667*lambda3cat*TMath::Exp(-0.5*arg*arg)/sigma3cat;
    // k=3dyn:
    arg = (x[0] - mu3dyn)/sigma3dyn;
    sumdyn += 0.1666666667*lambda3dyn*TMath::Exp(-0.5*arg*arg)/sigma3dyn;

    sumcat = TMath::Exp(-1.*lambda1cat)*sumcat;
    sumdyn = TMath::Exp(-1.*lambda1dyn)*sumdyn;

    return par[8]*(sumcat+sumdyn)/2.;
}

Double_t MHCalibrationChargeBlindPix::PoissonKto4(Double_t *x, Double_t *par)
{
    Double_t lambda = par[0];

    Double_t sum = 0.;
    Double_t arg = 0.;

    Double_t mu0 = par[1];
    Double_t mu1 = par[2];

    if (mu1 < mu0)
        return FLT_MAX;

    Double_t sigma0 = par[3];
    Double_t sigma1 = par[4];

    if (sigma0 < 0.0001)
        return FLT_MAX;

    if (sigma1 < sigma0)
        return FLT_MAX;

    Double_t mu2 = (2.*mu1)-mu0;
    Double_t mu3 = (3.*mu1)-(2.*mu0);
    Double_t mu4 = (4.*mu1)-(3.*mu0);

    Double_t sigma2 = TMath::Sqrt((2.*sigma1*sigma1) - (sigma0*sigma0));
    Double_t sigma3 = TMath::Sqrt((3.*sigma1*sigma1) - (2.*sigma0*sigma0));
    Double_t sigma4 = TMath::Sqrt((4.*sigma1*sigma1) - (3.*sigma0*sigma0));

    Double_t lambda2 = lambda*lambda;
    Double_t lambda3 = lambda2*lambda;
    Double_t lambda4 = lambda3*lambda;

    // k=0:
    arg = (x[0] - mu0)/sigma0;
    sum = TMath::Exp(-0.5*arg*arg)/sigma0;

    // k=1:
    arg = (x[0] - mu1)/sigma1;
    sum += lambda*TMath::Exp(-0.5*arg*arg)/sigma1;

    // k=2:
    arg = (x[0] - mu2)/sigma2;
    sum += 0.5*lambda2*TMath::Exp(-0.5*arg*arg)/sigma2;

    // k=3:
    arg = (x[0] - mu3)/sigma3;
    sum += 0.1666666667*lambda3*TMath::Exp(-0.5*arg*arg)/sigma3;

    // k=4:
    arg = (x[0] - mu4)/sigma4;
    sum += 0.041666666666667*lambda4*TMath::Exp(-0.5*arg*arg)/sigma4;

    return TMath::Exp(-1.*lambda)*par[5]*sum;
}

Double_t MHCalibrationChargeBlindPix::PoissonKto5(Double_t *x, Double_t *par)
{
    Double_t lambda = par[0];

    Double_t sum = 0.;
    Double_t arg = 0.;

    Double_t mu0 = par[1];
    Double_t mu1 = par[2];

    if (mu1 < mu0)
        return FLT_MAX;

    Double_t sigma0 = par[3];
    Double_t sigma1 = par[4];

    if (sigma0 < 0.0001)
        return FLT_MAX;

    if (sigma1 < sigma0)
        return FLT_MAX;


    Double_t mu2 = (2.*mu1)-mu0;
    Double_t mu3 = (3.*mu1)-(2.*mu0);
    Double_t mu4 = (4.*mu1)-(3.*mu0);
    Double_t mu5 = (5.*mu1)-(4.*mu0);

    Double_t sigma2 = TMath::Sqrt((2.*sigma1*sigma1) - (sigma0*sigma0));
    Double_t sigma3 = TMath::Sqrt((3.*sigma1*sigma1) - (2.*sigma0*sigma0));
    Double_t sigma4 = TMath::Sqrt((4.*sigma1*sigma1) - (3.*sigma0*sigma0));
    Double_t sigma5 = TMath::Sqrt((5.*sigma1*sigma1) - (4.*sigma0*sigma0));

    Double_t lambda2 = lambda*lambda;
    Double_t lambda3 = lambda2*lambda;
    Double_t lambda4 = lambda3*lambda;
    Double_t lambda5 = lambda4*lambda;

    // k=0:
    arg = (x[0] - mu0)/sigma0;
    sum = TMath::Exp(-0.5*arg*arg)/sigma0;

    // k=1:
    arg = (x[0] - mu1)/sigma1;
    sum += lambda*TMath::Exp(-0.5*arg*arg)/sigma1;

    // k=2:
    arg = (x[0] - mu2)/sigma2;
    sum += 0.5*lambda2*TMath::Exp(-0.5*arg*arg)/sigma2;

    // k=3:
    arg = (x[0] - mu3)/sigma3;
    sum += 0.1666666667*lambda3*TMath::Exp(-0.5*arg*arg)/sigma3;

    // k=4:
    arg = (x[0] - mu4)/sigma4;
    sum += 0.041666666666667*lambda4*TMath::Exp(-0.5*arg*arg)/sigma4;

    // k=5:
    arg = (x[0] - mu5)/sigma5;
    sum += 0.008333333333333*lambda5*TMath::Exp(-0.5*arg*arg)/sigma5;

    return TMath::Exp(-1.*lambda)*par[5]*sum;
}

Double_t MHCalibrationChargeBlindPix::PoissonKto6(Double_t *x, Double_t *par)
{
    Double_t lambda = par[0];

    Double_t sum = 0.;
    Double_t arg = 0.;

    Double_t mu0 = par[1];
    Double_t mu1 = par[2];

    if (mu1 < mu0)
        return FLT_MAX;

    Double_t sigma0 = par[3];
    Double_t sigma1 = par[4];

    if (sigma0 < 0.0001)
        return FLT_MAX;

    if (sigma1 < sigma0)
        return FLT_MAX;


    Double_t mu2 = (2.*mu1)-mu0;
    Double_t mu3 = (3.*mu1)-(2.*mu0);
    Double_t mu4 = (4.*mu1)-(3.*mu0);
    Double_t mu5 = (5.*mu1)-(4.*mu0);
    Double_t mu6 = (6.*mu1)-(5.*mu0);

    Double_t sigma2 = TMath::Sqrt((2.*sigma1*sigma1) - (sigma0*sigma0));
    Double_t sigma3 = TMath::Sqrt((3.*sigma1*sigma1) - (2.*sigma0*sigma0));
    Double_t sigma4 = TMath::Sqrt((4.*sigma1*sigma1) - (3.*sigma0*sigma0));
    Double_t sigma5 = TMath::Sqrt((5.*sigma1*sigma1) - (4.*sigma0*sigma0));
    Double_t sigma6 = TMath::Sqrt((6.*sigma1*sigma1) - (5.*sigma0*sigma0));

    Double_t lambda2 = lambda*lambda;
    Double_t lambda3 = lambda2*lambda;
    Double_t lambda4 = lambda3*lambda;
    Double_t lambda5 = lambda4*lambda;
    Double_t lambda6 = lambda5*lambda;

    // k=0:
    arg = (x[0] - mu0)/sigma0;
    sum = TMath::Exp(-0.5*arg*arg)/sigma0;

    // k=1:
    arg = (x[0] - mu1)/sigma1;
    sum += lambda*TMath::Exp(-0.5*arg*arg)/sigma1;

    // k=2:
    arg = (x[0] - mu2)/sigma2;
    sum += 0.5*lambda2*TMath::Exp(-0.5*arg*arg)/sigma2;

    // k=3:
    arg = (x[0] - mu3)/sigma3;
    sum += 0.1666666667*lambda3*TMath::Exp(-0.5*arg*arg)/sigma3;

    // k=4:
    arg = (x[0] - mu4)/sigma4;
    sum += 0.041666666666667*lambda4*TMath::Exp(-0.5*arg*arg)/sigma4;

    // k=5:
    arg = (x[0] - mu5)/sigma5;
    sum += 0.008333333333333*lambda5*TMath::Exp(-0.5*arg*arg)/sigma5;

    // k=6:
    arg = (x[0] - mu6)/sigma6;
    sum += 0.001388888888889*lambda6*TMath::Exp(-0.5*arg*arg)/sigma6;

    return TMath::Exp(-1.*lambda)*par[5]*sum;
}

Double_t MHCalibrationChargeBlindPix::Polya(Double_t *x, Double_t *par)
{
    const Double_t QEcat = 0.247;            // mean quantum efficiency
    const Double_t sqrt2 = 1.4142135623731;
    const Double_t sqrt3 = 1.7320508075689;
    const Double_t sqrt4 = 2.;

    const Double_t lambda = par[0];           // mean number of photons

    const Double_t excessPoisson = par[1];    // non-Poissonic noise contribution
    const Double_t delta1 = par[2];           // amplification first dynode
    const Double_t delta2 = par[3];           // amplification subsequent dynodes

    const Double_t electronicAmpl = par[4];   // electronic amplification and conversion to FADC charges

    const Double_t pmtAmpl = delta1*delta2*delta2*delta2*delta2*delta2;  // total PMT gain
    const Double_t A = 1. + excessPoisson - QEcat
        + 1./delta1
        + 1./delta1/delta2
        + 1./delta1/delta2/delta2;                                  // variance contributions from PMT and QE

    const Double_t totAmpl = QEcat*pmtAmpl*electronicAmpl;        // Total gain and conversion

    const Double_t mu0 = par[7];                                      // pedestal
    const Double_t mu1 = totAmpl;                                 // single phe position
    const Double_t mu2 = 2*totAmpl;                               // double phe position
    const Double_t mu3 = 3*totAmpl;                               // triple phe position
    const Double_t mu4 = 4*totAmpl;                               // quadruple phe position

    const Double_t sigma0 = par[5];
    const Double_t sigma1 = electronicAmpl*pmtAmpl*TMath::Sqrt(QEcat*A);
    const Double_t sigma2 = sqrt2*sigma1;
    const Double_t sigma3 = sqrt3*sigma1;
    const Double_t sigma4 = sqrt4*sigma1;

    const Double_t lambda2 = lambda*lambda;
    const Double_t lambda3 = lambda2*lambda;
    const Double_t lambda4 = lambda3*lambda;

    //-- calculate the area----
    Double_t arg = (x[0] - mu0)/sigma0;
    Double_t sum = TMath::Exp(-0.5*arg*arg)/sigma0;

    // k=1:
    arg = (x[0] - mu1)/sigma1;
    sum += lambda*TMath::Exp(-0.5*arg*arg)/sigma1;

    // k=2:
    arg = (x[0] - mu2)/sigma2;
    sum += 0.5*lambda2*TMath::Exp(-0.5*arg*arg)/sigma2;

    // k=3:
    arg = (x[0] - mu3)/sigma3;
    sum += 0.1666666667*lambda3*TMath::Exp(-0.5*arg*arg)/sigma3;

    // k=4:
    arg = (x[0] - mu4)/sigma4;
    sum += 0.041666666666667*lambda4*TMath::Exp(-0.5*arg*arg)/sigma4;

    return TMath::Exp(-1.*lambda)*par[6]*sum;
}
