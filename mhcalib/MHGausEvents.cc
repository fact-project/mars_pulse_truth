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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHGausEvents
//
//  A base class for events which are believed to follow a Gaussian distribution 
//  with time, e.g. calibration events, observables containing white noise, ...
//
//  MHGausEvents derives from MH, thus all features of MH can be used by a class 
//  deriving from MHGausEvents, especially the filling functions. 
//
//  The central objects are: 
//
//  1) The TH1F fHGausHist: 
//     ====================
//   
//     It is created with a default name and title and resides in directory NULL.
//     - Any class deriving from MHGausEvents needs to apply a binning to fHGausHist
//       (e.g. by setting the variables fNbins, fFirst, fLast and calling the function 
//       InitBins() or by directly calling fHGausHist.SetBins(..) )
//     - The histogram is filled with the functions FillHist() or FillHistAndArray(). 
//     - The histogram can be fitted with the function FitGaus(). This involves stripping 
//       of all zeros at the lower and upper end of the histogram and re-binning to 
//       a new number of bins, specified in the variable fBinsAfterStripping.      
//     - The fit result's probability is compared to a reference probability fProbLimit
//       The NDF is compared to fNDFLimit and a check is made whether results are NaNs. 
//       Accordingly, a flag IsGausFitOK() is set.
//     - One can repeat the fit within a given amount of sigmas from the previous mean 
//       (specified by the variables fPickupLimit and fBlackoutLimit) with the function RepeatFit()
// 
//  2) The TArrayF fEvents:
//     ==========================
// 
//     It is created with 0 entries and not expanded unless FillArray() or FillHistAndArray()
//     are called.
//     - A first call to FillArray() or FillHistAndArray() initializes fEvents by default 
//       to 512 entries. 
//     - Any later call to FillArray() or FillHistAndArray() fills up the array. 
//       Reaching the limit, the array is expanded by a factor 2.
//     - The array can be fourier-transformed into the array fPowerSpectrum. 
//       Note that any FFT accepts only number of events which are a power of 2. 
//       Thus, fEvents is cut to the next power of 2 smaller than its actual number of entries. 
//       Be aware that you might lose information at the end of your analysis. 
//     - Calling the function CreateFourierSpectrum() creates the array fPowerSpectrum 
//       and its projection fHPowerProbability which in turn is fit to an exponential. 
//     - The fit result's probability is compared to a referenc probability fProbLimit 
//       and accordingly the flag IsExpFitOK() is set.
//     - The flag IsFourierSpectrumOK() is set accordingly to IsExpFitOK(). 
//       Later, a closer check will be installed. 
//     - You can display all arrays by calls to: CreateGraphEvents() and 
//       CreateGraphPowerSpectrum() and successive calls to the corresponding Getters.
//
// To see an example, have a look at: Draw()
//
//////////////////////////////////////////////////////////////////////////////
#include "MHGausEvents.h"

#include <TMath.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TRandom.h>

#include "MFFT.h"
#include "MString.h"
#include "MArrayF.h"

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHGausEvents);

using namespace std;

const Int_t    MHGausEvents::fgNDFLimit             = 2;
const Float_t  MHGausEvents::fgProbLimit            = 0.001;
const Int_t    MHGausEvents::fgPowerProbabilityBins = 30;
// --------------------------------------------------------------------------
//
// Default Constructor. 
// Sets: 
// - the default Probability Bins for fPowerProbabilityBins (fgPowerProbabilityBins)
// - the default Probability Limit for fProbLimit  (fgProbLimit)
// - the default NDF Limit for fNDFLimit           (fgNDFLimit)
// - the default number of bins after stripping for fBinsAfterStipping (fgBinsAfterStipping)
// - the default name of the fHGausHist            ("HGausHist")
// - the default title of the fHGausHist           ("Histogram of Events with Gaussian Distribution")
// - the default directory of the fHGausHist       (NULL)
// - the default for fNbins (100)
// - the default for fFirst (0.)
// - the default for fLast  (100.)
//
// Initializes:
// - fEvents to 0 entries
// - fHGausHist()
// - all other pointers to NULL
// - all variables to 0.
// - all flags to kFALSE
//
MHGausEvents::MHGausEvents(const char *name, const char *title)
    : fEventFrequency(0), fFlags(0),
      fHPowerProbability(NULL), 
      fPowerSpectrum(NULL),
      fFExpFit(NULL),
      fGraphEvents(NULL), fGraphPowerSpectrum(NULL),
      fFirst(0.), fLast(100.), fNbins(100), fFGausFit(NULL)
{ 

  fName  = name  ? name  : "MHGausEvents";
  fTitle = title ? title : "Events with expected Gaussian distributions";

  Clear();
  
  SetBinsAfterStripping();
  SetNDFLimit();
  SetPowerProbabilityBins();
  SetProbLimit();

  fHGausHist.SetName("HGausHist");
  fHGausHist.SetTitle("Histogram of Events with Gaussian Distribution");
  // important, other ROOT will not draw the axes:
  fHGausHist.UseCurrentStyle();
  fHGausHist.SetDirectory(NULL);
  TAxis *yaxe = fHGausHist.GetYaxis();
  yaxe->CenterTitle();
}



// --------------------------------------------------------------------------
//
// Default Destructor. 
//
// Deletes (if Pointer is not NULL):
// 
// - fHPowerProbability
// - fPowerSpectrum     
// - fGraphEvents
// - fGraphPowerSpectrum
// 
// - fFGausFit 
// - fFExpFit
//
MHGausEvents::~MHGausEvents()
{

  //
  // The next two lines are important for the case that 
  // the class has been stored to a file and is read again. 
  // In this case, the next two lines prevent a segm. violation
  // in the destructor
  //
  gROOT->GetListOfFunctions()->Remove(fFGausFit);
  gROOT->GetListOfFunctions()->Remove(fFExpFit);
  
  // delete fits
  if (fFGausFit)
      delete fFGausFit; 
  
  if (fFExpFit)
      delete fFExpFit;

  // delete histograms
  if (fHPowerProbability)
    delete fHPowerProbability;

  // delete arrays
  if (fPowerSpectrum)  
    delete fPowerSpectrum;     

  // delete graphs
  if (fGraphEvents)
    delete fGraphEvents;

  if (fGraphPowerSpectrum)
    delete fGraphPowerSpectrum;
}
      
// --------------------------------------------------------------------------
//
// Default Clear(), can be overloaded.
//
// Sets:
// - all other pointers to NULL
// - all variables to 0. and keep fEventFrequency
// - all flags to kFALSE
// 
// Deletes (if not NULL):
// - all pointers
//
void MHGausEvents::Clear(Option_t *o)
{

  SetGausFitOK        ( kFALSE );
  SetExpFitOK         ( kFALSE );
  SetFourierSpectrumOK( kFALSE );
  SetExcluded         ( kFALSE );

  fMean              = 0.;
  fSigma             = 0.;
  fMeanErr           = 0.;
  fSigmaErr          = 0.;
  fProb              = 0.;

  fCurrentSize       = 0;

  if (fHPowerProbability)
    {
      delete fHPowerProbability;
      fHPowerProbability = NULL;
    }

  // delete fits
  if (fFGausFit)
    {
      delete fFGausFit; 
      fFGausFit = NULL;
    }
  
  if (fFExpFit)
    {
      delete fFExpFit;
      fFExpFit = NULL;
    }
  
  // delete arrays
  if (fPowerSpectrum)  
    {
      delete fPowerSpectrum;     
      fPowerSpectrum = NULL;
    }

  // delete graphs
  if (fGraphEvents)
    {
      delete fGraphEvents;
      fGraphEvents = NULL;
    }

  if (fGraphPowerSpectrum)
    {
      delete fGraphPowerSpectrum;
      fGraphPowerSpectrum = NULL;
    }
}

// -------------------------------------------------------------------
//
// Create the fourier spectrum using the class MFFT.
// The result is projected into a histogram and fitted by an exponential
//
void MHGausEvents::CreateFourierSpectrum()
{

  if (fFExpFit)
    return;

  if (fEvents.GetSize() < 8)
    {
      *fLog << warn << "Cannot create Fourier spectrum in: " << fName 
            << ". Number of events smaller than 8 " << endl;
      return;
    }
  
  //
  // The number of entries HAS to be a potence of 2, 
  // so we can only cut out from the last potence of 2 to the rest. 
  // Another possibility would be to fill everything with 
  // zeros, but that gives a low frequency peak, which we would 
  // have to cut out later again. 
  //
  // So, we have to live with the possibility that at the end 
  // of the calibration run, something has happened without noticing 
  // it...
  //
  
  // This cuts only the non-used zero's, but MFFT will later cut the rest
  fEvents.StripZeros();

  if (fEvents.GetSize() < 8)
    {
      /*
      *fLog << warn << "Cannot create Fourier spectrum. " << endl;
      *fLog << warn << "Number of events (after stripping of zeros) is smaller than 8 " 
            << "in pixel: " << fPixId << endl;
      */
      return;
    }

  MFFT fourier;

  fPowerSpectrum     = fourier.PowerSpectrumDensity(&fEvents);
  fHPowerProbability = ProjectArray(*fPowerSpectrum, fPowerProbabilityBins,
                                    MString::Format("PowerProb%s", GetName()),
                                    "Probability of Power occurrance");
  fHPowerProbability->SetXTitle("P(f)");
  fHPowerProbability->SetYTitle("Counts");
  fHPowerProbability->GetYaxis()->CenterTitle();
  fHPowerProbability->SetDirectory(NULL);
  fHPowerProbability->SetBit(kCanDelete);  
  //
  // First guesses for the fit (should be as close to reality as possible, 
  //
  const Double_t xmax = fHPowerProbability->GetXaxis()->GetXmax();

  fFExpFit = new TF1("","exp([0]-[1]*x)",0.,xmax);
  fFExpFit->SetName("FExpFit");
  gROOT->GetListOfFunctions()->Remove(fFExpFit);


  const Double_t slope_guess  = (TMath::Log(fHPowerProbability->GetEntries())+1.)/xmax;
  const Double_t offset_guess = slope_guess*xmax;

  // 
  // For the fits, we have to take special care since ROOT 
  // has stored the function pointer in a global list which 
  // lead to removing the object twice. We have to take out 
  // the following functions of the global list of functions 
  // as well:
  //
  gROOT->GetListOfFunctions()->Remove(fFExpFit);
  fFExpFit->SetParameters(offset_guess, slope_guess);
  fFExpFit->SetParNames("Offset","Slope");
  fFExpFit->SetParLimits(0,offset_guess/2.,2.*offset_guess);
  fFExpFit->SetParLimits(1,slope_guess/1.5,1.5*slope_guess);
  fFExpFit->SetRange(0.,xmax);

  fHPowerProbability->Fit(fFExpFit,"RQL0");
  
  if (GetExpProb() > fProbLimit)
    SetExpFitOK(kTRUE);
  
  //
  // For the moment, this is the only check, later we can add more...
  //
  SetFourierSpectrumOK(IsExpFitOK());

  return;
}

// ----------------------------------------------------------------------------------
//
// Create a graph to display the array fEvents
// If the variable fEventFrequency is set, the x-axis is transformed into real time.
//
void MHGausEvents::CreateGraphEvents()
{

  fEvents.StripZeros();

  const Int_t n = fEvents.GetSize();
  if (n==0)
    return;

  const Float_t freq = fEventFrequency ? fEventFrequency : 1;

  MArrayF xaxis(n);
  for (Int_t i=0; i<n; i++)
      xaxis[i] = (Float_t)i/freq;

  fGraphEvents = new TGraph(n, xaxis.GetArray(), fEvents.GetArray());
  fGraphEvents->SetTitle("Evolution of Events with time");
  fGraphEvents->GetXaxis()->SetTitle(fEventFrequency ? "Time [s]" : "Event Nr.");
  fGraphEvents->GetYaxis()->SetTitle(fHGausHist.GetXaxis()->GetTitle());
  fGraphEvents->GetYaxis()->CenterTitle();
}

// ----------------------------------------------------------------------------------
//
// Create a graph to display the array fPowerSpectrum
// If the variable fEventFrequency is set, the x-axis is transformed into real frequency.
//
void MHGausEvents::CreateGraphPowerSpectrum()
{

  fPowerSpectrum->StripZeros();

  const Int_t n = fPowerSpectrum->GetSize();

  const Float_t freq = fEventFrequency ? fEventFrequency : 1;

  MArrayF xaxis(n);
  for (Int_t i=0; i<n; i++)
      xaxis[i] = 0.5*(Float_t)i*freq/n;

  fGraphPowerSpectrum = new TGraph(n, xaxis.GetArray(), fPowerSpectrum->GetArray());
  fGraphPowerSpectrum->SetTitle("Power Spectrum Density");
  fGraphPowerSpectrum->GetXaxis()->SetTitle(fEventFrequency ? "Frequency [Hz]" : "Frequency");
  fGraphPowerSpectrum->GetYaxis()->SetTitle("P(f)");
  fGraphPowerSpectrum->GetYaxis()->CenterTitle();

}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// The following options can be chosen:
//
// "EVENTS": displays a TGraph of the array fEvents
// "FOURIER": display a TGraph of the fourier transform of fEvents 
//            displays the projection of the fourier transform with the fit
//
// The following picture shows a typical outcome of call to Draw("fourierevents"): 
// - The first plot shows the distribution of the values with the Gauss fit
//   (which did not succeed, in this case, for obvious reasons)
// - The second plot shows the TGraph with the events vs. time
// - The third plot shows the fourier transform and a small peak at about 85 Hz.
// - The fourth plot shows the projection of the fourier components and an exponential 
//   fit, with the result that the observed deviation is still statistical with a 
//   probability of 0.5%. 
//
//Begin_Html
/*
<img src="images/MHGausEventsDraw.gif">
*/
//End_Html
//
void MHGausEvents::Draw(const Option_t *opt)
{

  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this,600, 600);

  TString option(opt);
  option.ToLower();
  
  Int_t win   = 1;
  Int_t nofit = 0;

  if (option.Contains("events"))
    win += 1;
  if (option.Contains("fourier"))
    win += 2;
  
  if (IsEmpty())
    win--;

  if (option.Contains("nofit"))
    {
      option.ReplaceAll("nofit","");
      nofit++;
    }
  
  pad->SetBorderMode(0);
  if (win > 1)
    pad->Divide(1,win);

  Int_t cwin = 1;

  gPad->SetTicks();

  if (!IsEmpty())
    {
      pad->cd(cwin++);

      if (!IsOnlyOverflow() && !IsOnlyUnderflow())
        gPad->SetLogy();

      fHGausHist.Draw(option);

      if (!nofit)
        if (fFGausFit)
          {
            fFGausFit->SetLineColor(IsGausFitOK() ? kGreen : kRed);
            fFGausFit->Draw("same");
          }
    }
  
  if (option.Contains("events"))
    {
      pad->cd(cwin++);
      DrawEvents();
    }
  if (option.Contains("fourier"))      
    {
      pad->cd(cwin++);
      DrawPowerSpectrum();
      pad->cd(cwin);
      DrawPowerProjection();
    }
}

// -----------------------------------------------------------------------------
// 
// DrawEvents:
//
// Will draw the graph with the option "A", unless the option:
// "SAME" has been chosen 
//
void MHGausEvents::DrawEvents(Option_t *opt)
{
  
  if (!fGraphEvents)
    CreateGraphEvents();

  if (!fGraphEvents)
    return;

  fGraphEvents->SetBit(kCanDelete);
  fGraphEvents->SetTitle("Events with time");

  TString option(opt);
  option.ToLower();

  if (option.Contains("same"))
    {
      option.ReplaceAll("same","");      
      fGraphEvents->Draw(option+"L");
    }
  else
    fGraphEvents->Draw(option+"AL");  
}


// -----------------------------------------------------------------------------
// 
// DrawPowerSpectrum
//
// Will draw the fourier spectrum of the events sequence with the option "A", unless the option:
// "SAME" has been chosen 
//
void MHGausEvents::DrawPowerSpectrum(Option_t *option)
{

  TString opt(option);
  
  if (!fPowerSpectrum)
    CreateFourierSpectrum();

  if (fPowerSpectrum)
    {
      if (!fGraphPowerSpectrum)
        CreateGraphPowerSpectrum();

      if (!fGraphPowerSpectrum)
        return;
      
      if (opt.Contains("same"))
        {
          opt.ReplaceAll("same","");      
          fGraphPowerSpectrum->Draw(opt+"L");
        }
      else
        {
          fGraphPowerSpectrum->Draw(opt+"AL");  
          fGraphPowerSpectrum->SetBit(kCanDelete);
        }
    }
}

// -----------------------------------------------------------------------------
// 
// DrawPowerProjection
//
// Will draw the projection of the fourier spectrum onto the power probability axis
// with the possible options of TH1D
//
void MHGausEvents::DrawPowerProjection(Option_t *option)
{
  
  TString opt(option);

  if (!fHPowerProbability)
    CreateFourierSpectrum();

  if (fHPowerProbability && fHPowerProbability->GetEntries() > 0)
    {
      gPad->SetLogy();
      fHPowerProbability->Draw(opt.Data());
      if (fFExpFit)
        {
          fFExpFit->SetLineColor(IsExpFitOK() ? kGreen : kRed);
          fFExpFit->Draw("same");
        }
    }
}


// --------------------------------------------------------------------------
//
// Fill fEvents with f
// If size of fEvents is 0, initializes it to 512
// If size of fEvents is smaller than fCurrentSize, double the size
// Increase fCurrentSize by 1
//
void MHGausEvents::FillArray(const Float_t f)
{

  if (fEvents.GetSize() == 0)
    fEvents.Set(512);

  if (fCurrentSize >= fEvents.GetSize())
    fEvents.Set(fEvents.GetSize()*2);
  
  fEvents.AddAt(f,fCurrentSize++);
}


// --------------------------------------------------------------------------
//
// Fills fHGausHist with f
// Returns kFALSE, if overflow or underflow occurred, else kTRUE
//
Bool_t MHGausEvents::FillHist(const Float_t f)
{
  return fHGausHist.Fill(f) > -1;
}

// --------------------------------------------------------------------------
//
// Executes:
// - FillArray()
// - FillHist()
//
Bool_t MHGausEvents::FillHistAndArray(const Float_t f)
{

  FillArray(f);
  return FillHist(f);
}

// -------------------------------------------------------------------
//
// Fit fGausHist with a Gaussian after stripping zeros from both ends 
// and rebinned to the number of bins specified in fBinsAfterStripping
//
// The fit results are retrieved and stored in class-own variables.  
//
// A flag IsGausFitOK() is set according to whether the fit probability 
// is smaller or bigger than fProbLimit, whether the NDF is bigger than 
// fNDFLimit and whether results are NaNs.
//
Bool_t MHGausEvents::FitGaus(Option_t *option, const Double_t xmin, const Double_t xmax)
{

  if (IsGausFitOK())
    return kTRUE;

  //
  // First, strip the zeros from the edges which contain only zeros and rebin 
  // to fBinsAfterStripping bins. If fBinsAfterStripping is 0, reduce bins only by 
  // a factor 10.
  //
  // (ATTENTION: The Chisquare method is more sensitive, 
  // the _less_ bins, you have!)
  //
  StripZeros(fHGausHist,
             fBinsAfterStripping ? fBinsAfterStripping 
             : (fNbins > 1000 ? fNbins/10 : 0));
  
  TAxis *axe = fHGausHist.GetXaxis();
  //
  // Get the fitting ranges
  //
  Axis_t rmin = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetFirst()) : xmin;
  Axis_t rmax = ((xmin==0.) && (xmax==0.)) ? fHGausHist.GetBinCenter(axe->GetLast())  : xmax;

  //
  // First guesses for the fit (should be as close to reality as possible, 
  //
  const Stat_t   entries     = fHGausHist.Integral(axe->FindBin(rmin),axe->FindBin(rmax),"width");
  const Double_t mu_guess    = fHGausHist.GetBinCenter(fHGausHist.GetMaximumBin());
  const Double_t sigma_guess = fHGausHist.GetRMS();
  const Double_t area_guess  = entries/TMath::Sqrt(TMath::TwoPi())/sigma_guess;

  fFGausFit = new TF1("GausFit","gaus",rmin,rmax);

  if (!fFGausFit) 
    {
    *fLog << warn << dbginf << "WARNING: Could not create fit function for Gauss fit " 
          << "in: " << fName << endl;
    return kFALSE;
    }
  
  // 
  // For the fits, we have to take special care since ROOT 
  // has stored the function pointer in a global list which 
  // lead to removing the object twice. We have to take out 
  // the following functions of the global list of functions 
  // as well:
  //
  gROOT->GetListOfFunctions()->Remove(fFGausFit);

  fFGausFit->SetParameters(area_guess,mu_guess,sigma_guess);
  fFGausFit->SetParNames("Area","#mu","#sigma");
  fFGausFit->SetParLimits(0,0.,area_guess*25.);
  fFGausFit->SetParLimits(1,rmin,rmax);
  fFGausFit->SetParLimits(2,0.,rmax-rmin);
  fFGausFit->SetRange(rmin,rmax);

  fHGausHist.Fit(fFGausFit,option);

  fMean     = fFGausFit->GetParameter(1);
  fSigma    = fFGausFit->GetParameter(2);
  fMeanErr  = fFGausFit->GetParError(1);
  fSigmaErr = fFGausFit->GetParError(2);
  fProb     = fFGausFit->GetProb();

  //
  // The fit result is accepted under condition:
  // 1) The results are not NaN's  (not a number)
  // 2) The results are all finite
  // 3) The NDF is not smaller than fNDFLimit (default: fgNDFLimit)
  // 4) The Probability is greater than fProbLimit (default: fgProbLimit)
  //
  // !Finitite means either infinite or not-a-number
  if (   !TMath::Finite(fMean)
      || !TMath::Finite(fMeanErr)
      || !TMath::Finite(fProb)
      || !TMath::Finite(fSigma)
      || !TMath::Finite(fSigmaErr)
      || fFGausFit->GetNDF() < fNDFLimit 
      || fProb < fProbLimit )
    return kFALSE;

  SetGausFitOK(kTRUE);
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Default InitBins, can be overloaded.
//
// Executes:
// - fHGausHist.SetBins(fNbins,fFirst,fLast)
//
void MHGausEvents::InitBins()
{
  //  const TAttAxis att(fHGausHist.GetXaxis());
  fHGausHist.SetBins(fNbins,fFirst,fLast);
  //  att.Copy(fHGausHist.GetXaxis());
}

// -----------------------------------------------------------------------------------
// 
// A default print
//
void MHGausEvents::Print(const Option_t *o) const 
{
  
  *fLog << all                                                        << endl;
  *fLog << all << "Results of the Gauss Fit in: " << fName            << endl;
  *fLog << all << "Mean: "             << GetMean()                   << endl;
  *fLog << all << "Sigma: "            << GetSigma()                  << endl;
  *fLog << all << "Chisquare: "        << GetChiSquare()              << endl;
  *fLog << all << "DoF: "              << GetNdf()                    << endl;
  *fLog << all << "Probability: "      << GetProb()                   << endl;
  *fLog << all                                                        << endl;
  
}


// --------------------------------------------------------------------------
//
// Default Reset(), can be overloaded.
//
// Executes:
// - Clear()
// - fHGausHist.Reset()
// - fEvents.Set(0)
// - InitBins()
//
void MHGausEvents::Reset()
{

  Clear();
  fHGausHist.Reset();
  fEvents.Set(0);
  InitBins();
}

// ----------------------------------------------------------------------------
//
// Simulates Gaussian events and fills them  into the histogram and the array
// In order to do a fourier analysis, call CreateFourierSpectrum()
//
void  MHGausEvents::SimulateGausEvents(const Float_t mean, const Float_t sigma, const Int_t nevts)
{

  if (!IsEmpty())
    *fLog << warn << "The histogram is already filled, will superimpose simulated events on it..." << endl;
  
  for (Int_t i=0;i<nevts;i++) {
    const Double_t ran = gRandom->Gaus(mean,sigma);
    FillHistAndArray(ran);
  }
  
}
