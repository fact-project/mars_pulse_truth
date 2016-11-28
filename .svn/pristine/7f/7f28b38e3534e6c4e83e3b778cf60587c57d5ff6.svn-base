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
!   Author(s): Wolfgang Wittek,  July 2003      <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHFindSignificance
//
// determines the significance of a gamma signal in an |alpha| plot
// 
//
// Input : TH1 histogram of |alpha| : with 0 < |alpha| < 90 degrees
//         alphamin, alphamax :     defining the background region
//         alphasig           :     defining the signal region for which
//                                  the significance is calculated
//         degree : the degree of the polynomial to be fitted to the background
//                  ( a0 + a1*x + a2*x**2 + a3*x**3 + ...) 
//
// Output : 
//
//   - polynomial which describes the background in the background region
//   - the number of events in the signal region (Non)
//     the number of background events in the signal region (Nbg)
//   - the number of excess events in the signal region (Nex = Non - Nbg)
//   - thew effective number of background events (Noff), and gamma :
//     Nbg = gamma * Noff
//   - the significance of the gamma signal according to Li & Ma               
//
//
// call member function 'FindSigma' 
//      to fit the background and to determine the significance  
//
// call the member function 'SigmaVsAlpha'
//      to determine the significance as a function of alphasig
//
/////////////////////////////////////////////////////////////////////////////
#include "MHFindSignificance.h"

#include <fstream>
#include <math.h>

#include <TArrayD.h>
#include <TArrayI.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFitter.h>
#include <TMinuit.h>
#include <TPaveText.h>
#include <TStyle.h>

#include "MLog.h"
#include "MLogManip.h"
#include "MMinuitInterface.h"


ClassImp(MHFindSignificance);

using namespace std;

const TString MHFindSignificance::gsDefName  = "MHFindSignificance";
const TString MHFindSignificance::gsDefTitle = "Find Significance in alpha plot";



// --------------------------------------------------------------------------
//
// fcnpoly
//
// calculates the chi2 for the fit of the polynomial function 'poly' 
// to the histogram 'fhist'
//
// it is called by CallMinuit() (which is called in FitPolynomial()) 
//
// bins of fhist with huge errors are ignored in the calculation of the chi2
// (the huge errors were set in 'FitPolynomial()')
//

static void fcnpoly(Int_t &npar, Double_t *gin, Double_t &f, 
                    Double_t *par, Int_t iflag)
{
    TH1 *fhist = (TH1*)gMinuit->GetObjectFit();
    TF1 *fpoly = fhist->GetFunction("Poly");    


    //-------------------------------------------

    Double_t chi2 = 0.0;

    Int_t nbins = fhist->GetNbinsX();
    Int_t mbins = 0;
    for (Int_t i=1; i<=nbins; i++)
    {
      Double_t content = fhist->GetBinContent(i);
      Double_t error   = fhist->GetBinError(i);
      Double_t center  = fhist->GetBinCenter(i);

      //-----------------------------
      // ignore unwanted points
      if (error > 1.e19)
        continue;

      if (content <= 0.0)
      {
        gLog << "fcnpoly : bin with zero content; i, content, error = "
             << i << ",  " << content << ",  " << error << endl;
        continue;
      }        

      if (error <= 0.0)
      {
        gLog << "fcnpoly : bin with zero error; i, content, error = "
             << i << ",  " << content << ",  " << error << endl;
        continue;
      }        

      //-----------------------------
      mbins++;

      Double_t fu;
      fu = fpoly->EvalPar(&center, par);

      // the fitted function must not be negative
      if (fu <= 0.0)
      {
        chi2 = 1.e10;
        break;
      }

      Double_t temp = (content - fu) / error;
      chi2 += temp*temp;
    }

    //-------------------------------------------

    f = chi2;

    //-------------------------------------------
    // final calculations
    //if (iflag == 3)
    //{
    //}    

    //-------------------------------------------------------------
}


// --------------------------------------------------------------------------
//
// fcnpolygauss
//
// calculates the chi2 for the fit of the (polynomial+Gauss) function 
// 'PolyGauss' to the histogram 'fhist'
//
// it is called by CallMinuit() (which is called in FitGaussPoly()) 
//
// bins of fhist with huge errors are ignored in the calculation of the chi2
// (the huge errors were set in 'FitGaussPoly()')
//

static void fcnpolygauss(Int_t &npar, Double_t *gin, Double_t &f, 
                         Double_t *par, Int_t iflag)
{
    TH1 *fhist = (TH1*)gMinuit->GetObjectFit();
    TF1 *fpolygauss = fhist->GetFunction("PolyGauss");    


    //-------------------------------------------

    Double_t chi2 = 0.0;

    Int_t nbins = fhist->GetNbinsX();
    Int_t mbins = 0;
    for (Int_t i=1; i<=nbins; i++)
    {
      Double_t content = fhist->GetBinContent(i);
      Double_t error   = fhist->GetBinError(i);
      Double_t center  = fhist->GetBinCenter(i);

      //-----------------------------
      // ignore unwanted points
      if (error > 1.e19)
        continue;

      if (content <= 0.0)
      {
        gLog << "fcnpolygauss : bin with zero content; i, content, error = "
             << i << ",  " << content << ",  " << error << endl;
        continue;
      }        

      if (error <= 0.0)
      {
        gLog << "fcnpolygauss : bin with zero error; i, content, error = "
             << i << ",  " << content << ",  " << error << endl;
        continue;
      }        

      //-----------------------------
      mbins++;

      Double_t fu;
      fu = fpolygauss->EvalPar(&center, par);

      // the fitted function must not be negative
      if (fu <= 0.0)
      {
        chi2 = 1.e10;
        break;
      }

      Double_t temp = (content - fu) / error;
      chi2 += temp*temp;
    }

    //-------------------------------------------

    f = chi2;

    //-------------------------------------------
    // final calculations
    //if (iflag == 3)
    //{
    //}    

    //-------------------------------------------------------------
}



// --------------------------------------------------------------------------
//
//  Constructor
//
MHFindSignificance::MHFindSignificance(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fSigVsAlpha = NULL;

    fPoly   = NULL;
    fGPoly  = NULL;
    fGBackg = NULL;

    fHist     = NULL;
    fHistOrig = NULL;

    // allow rebinning of the alpha plot
    fRebin = kTRUE;

    // allow reducing the degree of the polynomial
    fReduceDegree = kTRUE;

    fCanvas = NULL;
}

// --------------------------------------------------------------------------
//
//  Destructor. 
//
// =====>  it is not clear why one obtains sometimes a segmentation violation
//         when the destructor is active     <=======================
//
// therefore the 'return'statement
//

MHFindSignificance::~MHFindSignificance()
{
  return;

  *fLog << "destructor of MHFindSignificance is called" << endl;
 
    //delete fHist;

    delete fSigVsAlpha;
    delete fPoly;
    delete fGPoly;
    delete fGBackg;
    //delete fCanvas;
}

// --------------------------------------------------------------------------
//
//  Set flag fRebin 
//
//  if flag is kTRUE rebinning of the alpha plot is allowed
//
//
void MHFindSignificance::SetRebin(Bool_t b)
{
  fRebin = b;

  *fLog << "MHFindSignificance::SetRebin; flag fRebin set to " 
        << (b? "kTRUE" : "kFALSE") << endl;
}

// --------------------------------------------------------------------------
//
//  Set flag fReduceDegree 
//
//  if flag is kTRUE reducing of the degree of the polynomial is allowed
//
//
void MHFindSignificance::SetReduceDegree(Bool_t b)
{
  fReduceDegree = b;

  *fLog << "MHFindSignificance::SetReduceDegree; flag fReduceDegree set to " 
        << (b? "kTRUE" : "kFALSE") << endl;
}

// --------------------------------------------------------------------------
//
//  FindSigma
//
//  calls FitPolynomial     to fit the background in the background region
//  calls DetExcess         to determine the number of excess events
//                          using an extrapolation of the polynomial
//                          into the signal region
//  calls SigmaLiMa         to determine the significance of the gamma signal
//                          in the range |alpha| < alphasig
//  calls FitGaussPoly      to fit a (polynomial+Gauss) function in the
//                          whole |alpha| region
//
//
Bool_t MHFindSignificance::FindSigma(TH1 *fhist,  Double_t alphamin,
       Double_t alphamax, Int_t degree, Double_t alphasig, 
       Bool_t drawpoly,   Bool_t fitgauss, Bool_t print)
{
  //*fLog << "MHFindSignificance::FindSigma;" << endl;

  fHistOrig = fhist;

  fHist = (TH1*)fHistOrig->Clone();
  fHist->SetName(fhist->GetName());
  if ( !fHist )
  {
    *fLog << "MHFindSignificance::FindSigma; Clone of histogram could not be generated" 
          << endl;
    return kFALSE;
  }
  
  fHist->Sumw2();
  //fHist->SetNameTitle("Alpha", "alpha plot");
  fHist->SetXTitle("|alpha|  [\\circ]");
  fHist->SetYTitle("Counts");
  fHist->UseCurrentStyle();

  fAlphamin = alphamin;
  fAlphamax = alphamax;
  fAlphammm = (alphamin+alphamax)/2.0;
  fDegree   = degree;
  fAlphasig = alphasig;

  fDraw     = drawpoly;
  fFitGauss = fitgauss;


  //--------------------------------------------
  // fit a polynomial in the background region

  //*fLog << "MHFindSignificance::FindSigma;  calling FitPolynomial()" << endl;
  if ( !FitPolynomial() )
  {
    *fLog << "MHFindSignificance::FindSigma; FitPolynomial failed"
	  << endl;  
    return kFALSE;
  }


  //--------------------------------------------
  // calculate the number of excess events in the signal region

  //*fLog << "MHFindSignificance::FindSigma;  calling DetExcess()" << endl;
  if ( !DetExcess() )
  {
    *fLog << "MHFindSignificance::FindSigma; DetExcess failed"
	  << endl;  
    return kFALSE;
  }


  //--------------------------------------------
  // calculate the significance of the excess

  //*fLog << "MHFindSignificance::FindSigma;  calling SigmaLiMa()" << endl;
  Double_t siglima = 0.0;
  if ( !SigmaLiMa(fNon, fNoff, fGamma, &siglima) )
  {
    *fLog << "MHFindSignificance::FindSigma; SigmaLiMa failed"
	  << endl;  
    return kFALSE;
  }
  fSigLiMa = siglima;

  //--------------------------------------------
  // calculate the error of the number of excess events

  fdNex = fNex / fSigLiMa;


  //--------------------------------------------

  //*fLog << "MHFindSignificance::FindSigma;  calling PrintPoly()" << endl;
  if (print)
    PrintPoly();


  //--------------------------------------------
  // fit a (polynomial + Gauss) function

  if (fFitGauss)
  {
    //--------------------------------------------------
    // delete objects from this fit
    // in order to have independent starting conditions for the next fit

      delete gMinuit;
      gMinuit = NULL;
    //--------------------------------------------------

      //*fLog << "MHFindSignificance::FindSigma;  calling FitGaussPoly()" 
      //      << endl;
    if ( !FitGaussPoly() )
    {
      *fLog << "MHFindSignificance::FindSigma; FitGaussPoly failed"  
      	    << endl;  
      return kFALSE;
    }

    if (print)
    {
      //*fLog << "MHFindSignificance::FindSigma;  calling PrintPolyGauss()" 
      //      << endl;
      PrintPolyGauss();
    }
  }

  //--------------------------------------------------
  // draw the histogram if requested

  if (fDraw)
  {
    //*fLog << "MHFindSignificance::FindSigma;  calling DrawFit()" << endl;
    if ( !DrawFit() )
    {
      *fLog << "MHFindSignificance::FindSigma; DrawFit failed"  
      	    << endl;  
      return kFALSE;
    }
  }


  //--------------------------------------------------
  // delete objects from this fit
  // in order to have independent starting conditions for the next fit

    delete gMinuit;
    gMinuit = NULL;
  //--------------------------------------------------

  return kTRUE;
}

// --------------------------------------------------------------------------
//
//  SigmaVsAlpha  (like FindSigma. However, alphasig is scanned and
//                 the significance is plotted versus alphasig)
//
//  calls FitPolynomial     to fit the background in the background region
//
//  scan alphasig; for a given alphasig :
//       calls DetExcess    to determine the number of excess events
//       calls SigmaLiMa    to determine the significance of the gamma signal
//                          in the range fAlphalow < |alpha| < alphasig
//

Bool_t MHFindSignificance::SigmaVsAlpha(TH1 *fhist,  Double_t alphamin,
                    Double_t alphamax, Int_t degree, Bool_t print)
{
  //*fLog << "MHFindSignificance::SigmaVsAlpha;" << endl;

  fHistOrig = fhist;

  fHist = (TH1*)fHistOrig->Clone();
  fHist->SetName(fhist->GetName());
  fHist->Sumw2();
  //fHist->SetNameTitle("alpha", "alpha plot");
  fHist->SetXTitle("|alpha|  [\\circ]");
  fHist->SetYTitle("Counts");
  fHist->UseCurrentStyle();

  fAlphamin = alphamin;
  fAlphamax = alphamax;
  fAlphammm = (alphamin+alphamax)/2.0;
  fDegree   = degree;


  //--------------------------------------------
  // fit a polynomial in the background region

  //*fLog << "MHFindSignificance::SigmaVsAlpha  calling FitPolynomial()" 
  //      << endl;
  if ( !FitPolynomial() )  
    {
      *fLog << "MHFindSignificance::SigmaVsAlpha;  FitPolynomial() failed" 
            << endl;
      return kFALSE;
    }


  //--------------------------------------------
  // loop over different signal regions

  Int_t    nsteps    =  15;

  fSigVsAlpha = new TH1D("SigVsAlpha","Sigma vs Alpha", nsteps, 0.0, alphamin);
  fSigVsAlpha->SetXTitle("upper edge of signal region in |alpha|  [\\circ]");
  fSigVsAlpha->SetYTitle("Significance of gamma signal");

  for (Int_t i=1; i<=nsteps; i++)
  {
    fAlphasig = fSigVsAlpha->GetBinCenter(i);

    if ( !DetExcess() )
    {
      *fLog << "MHFindSignificance::SigmaVsAlpha;  DetExcess() failed" << endl;
      continue;
    }

    Double_t siglima = 0.0;
    if ( !SigmaLiMa(fNon, fNoff, fGamma, &siglima) )
    {
      *fLog << "MHFindSignificance::SigmaVsAlpha;  SigmaLiMa() failed" << endl;
      continue;
    }

    fdNex = fNex / siglima;
    fSigVsAlpha->SetBinContent(i, siglima);

    if (print)
      PrintPoly();
  }

  //--------------------------------------------
  // plot significance versus alphasig
 
  TCanvas *ccc = new TCanvas("SigVsAlpha", "Sigma vs Alpha", 600, 600);
 
  gROOT->SetSelectedPad(NULL);
  gStyle->SetPadLeftMargin(0.05);

  ccc->cd();
  fSigVsAlpha->DrawCopy();

  ccc->Modified();
  ccc->Update();

  return kTRUE;
}

// --------------------------------------------------------------------------
//
//  FitPolynomial
//
//  - create a clone 'fHist' of the |alpha| distribution 'fHistOrig'
//  - fit a polynomial of degree 'fDegree' to the alpha distribution 
//    'fHist' in the region alphamin < |alpha| < alphamax
//
//  in pathological cases the histogram is rebinned before fitting
//     (this is done only if fRebin is kTRUE)
//
//  if the highest coefficient of the polynomial is compatible with zero
//     the fit is repeated with a polynomial of lower degree
//     (this is done only if fReduceDegree is kTRUE)
//
//
Bool_t MHFindSignificance::FitPolynomial()
{
  //--------------------------------------------------
  // check the histogram :
  //       - calculate initial values of the parameters
  //       - check for bins with zero entries
  //       - set minimum errors
  //       - save the original errors
  //       - set errors huge outside the fit range
  //         (in 'fcnpoly' points with huge errors will be ignored)


  Double_t dummy = 1.e20;

  Double_t mean;
  Double_t rms;
  Double_t nclose;
  Double_t nfar;
  Double_t a2init = 0.0;
  TArrayD  saveError;

  Int_t nbins;
  Int_t nrebin = 1;

  //----------------   start while loop for rebinning   -----------------
  while(1)
  {

  fNzero   = 0;
  fMbins   = 0;
  fMlow    = 0;
  fNbgtot  = 0.0;

  fAlphami =  10000.0;
  fAlphamm =  10000.0;
  fAlphama = -10000.0;

  mean   = 0.0;
  rms    = 0.0;
  nclose = 0.0;
  nfar   = 0.0;

  nbins = fHist->GetNbinsX();
  saveError.Set(nbins);

  for (Int_t i=1; i<=nbins; i++)
  {
    saveError[i-1] = fHist->GetBinError(i);

    // bin should be completely contained in the fit range
    // (fAlphamin, fAlphamax)
    Double_t  xlo = fHist->GetBinLowEdge(i);
    Double_t  xup = fHist->GetBinLowEdge(i+1);

    if ( xlo >= fAlphamin-fEps  &&  xlo <= fAlphamax+fEps  &&
	 xup >= fAlphamin-fEps  &&  xup <= fAlphamax+fEps     )
    {
      fMbins++;

      if ( xlo < fAlphami )
        fAlphami = xlo;

      if ( xup > fAlphama )
        fAlphama = xup;

      Double_t content = fHist->GetBinContent(i);
      fNbgtot += content;

      mean += content;
      rms  += content*content;

      // count events in low-alpha and high-alpha region
      if ( xlo >= fAlphammm-fEps  &&  xup >= fAlphammm-fEps)
      {
        nfar   += content;
        if ( xlo < fAlphamm )
          fAlphamm = xlo;
        if ( xup < fAlphamm )
          fAlphamm = xup; 
      }
      else
      {
        nclose += content;
        if ( xlo > fAlphamm )
          fAlphamm = xlo;
        if ( xup > fAlphamm )
          fAlphamm = xup; 
      }

      // count bins with zero entry
      if (content <= 0.0)
        fNzero++;
     
      // set minimum error
      if (content < 9.0)
      {
        fMlow += 1;
        fHist->SetBinError(i, 3.0);
      }

      //*fLog << "Take : i, content, error = " << i << ",  " 
      //      << fHist->GetBinContent(i) << ",  "
      //      << fHist->GetBinError(i)   << endl;

      continue;
    }    
    // bin is not completely contained in the fit range : set error huge

    fHist->SetBinError(i, dummy);

    //*fLog << "Omit : i, content, error = " << i << ",  " 
    //      << fHist->GetBinContent(i) << ",  " << fHist->GetBinError(i) 
    //      << endl;

  }

  // mean of entries/bin in the fit range
  if (fMbins > 0)
  {
    mean /= ((Double_t) fMbins);
    rms  /= ((Double_t) fMbins);
  }

  rms = sqrt( rms - mean*mean );

  // if there are no events in the background region
  //    there is no reason for rebinning
  //    and this is the condition for assuming a constant background (= 0)
  if (mean <= 0.0)
    break;

  Double_t helpmi = fAlphami*fAlphami*fAlphami;
  Double_t helpmm = fAlphamm*fAlphamm*fAlphamm;
  Double_t helpma = fAlphama*fAlphama*fAlphama;
  Double_t help   =   (helpma-helpmm) * (fAlphamm-fAlphami)    
	            - (helpmm-helpmi) * (fAlphama-fAlphamm);
  if (help != 0.0)
    a2init =  ( (fAlphamm-fAlphami)*nfar - (fAlphama-fAlphamm)*nclose )
                * 1.5 * fHist->GetBinWidth(1) / help;
  else
    a2init = 0.0;


  //--------------------------------------------
  // rebin the histogram
  //   - if a bin has no entries 
  //   - or if there are too many bins with too few entries
  //   - or if the new bin width would exceed half the size of the 
  //     signal region

  if ( !fRebin  ||
       ( fNzero <= 0 && (Double_t)fMlow<0.05*(Double_t)fMbins )  || 
       (Double_t)(nrebin+1)/(Double_t)nrebin * fHist->GetBinWidth(1) 
                                                           > fAlphasig/2.0 )
  {
    //*fLog << "before break" << endl;
    break;
  }

  nrebin += 1;
  TString histname = fHist->GetName();
  delete fHist;
  fHist = NULL;

  *fLog << "MHFindSignificance::FitPolynomial; rebin the |alpha| plot, grouping "
        << nrebin << " bins together" << endl;

  // TH1::Rebin doesn't work properly
  //fHist = fHistOrig->Rebin(nrebin, "Rebinned");
  // use private routine RebinHistogram()
  fHist = new TH1F;
  fHist->Sumw2();
  fHist->SetNameTitle(histname, histname);
  fHist->UseCurrentStyle();

  // do rebinning such that x0 remains a lower bin edge
  Double_t x0 = 0.0;
  if ( !RebinHistogram(x0, nrebin) )
  {
    *fLog << "MHFindSignificance::FitPolynomial; RebinHistgram() failed" 
          << endl;
    return kFALSE;
  }

  fHist->SetXTitle("|alpha|  [\\circ]");
  fHist->SetYTitle("Counts");

  }
  //----------------   end of while loop for rebinning   -----------------


  // if there are still too many bins with too few entries don't fit
  // and assume a constant background

  fConstantBackg = kFALSE;
  if ( fNzero > 0  ||  (Double_t)fMlow>0.05*(Double_t)fMbins ) 
  {
    *fLog << "MHFindSignificance::FitPolynomial; polynomial fit not possible,  fNzero, fMlow, fMbins = "
          << fNzero << ",  " << fMlow << ",  " << fMbins << endl;
    *fLog << "                    assume a constant background" << endl;

    fConstantBackg = kTRUE;
    fDegree        = 0;

    TString funcname = "Poly";
    Double_t xmin =   0.0;
    Double_t xmax =  90.0;

    TString formula = "[0]";

    fPoly = new TF1(funcname, formula, xmin, xmax);
    TList *funclist = fHist->GetListOfFunctions();
    funclist->Add(fPoly);

    //--------------------
    Int_t nparfree = 1;
    fChisq         = 0.0; 
    fNdf           = fMbins - nparfree;
    fProb          = 0.0;
    fIstat         = 0;

    fValues.Set(1);
    fErrors.Set(1);

    Double_t val, err;
    val = mean;
    err = sqrt( mean / (Double_t)fMbins );

    fPoly->SetParameter(0, val);
    fPoly->SetParError (0, err);

    fValues[0] = val;
    fErrors[0] = err; 

    fEma[0][0]  = err*err;
    fCorr[0][0] = 1.0;
    //--------------------

    //--------------------------------------------------
    // reset the errors of the points in the histogram
    for (Int_t i=1; i<=nbins; i++)
    {
      fHist->SetBinError(i, saveError[i-1]);
    }


    return kTRUE;
  }


  //===========   start loop for reducing the degree   ==================
  //              of the polynomial
  while (1)
  {
      //--------------------------------------------------
      // prepare fit of a polynomial :   (a0 + a1*x + a2*x**2 + a3*x**3 + ...)

      TString funcname = "Poly";
      Double_t xmin =   0.0;
      Double_t xmax =  90.0;

      TString formula = "[0]";
      TString bra1     = "+[";
      TString bra2     =    "]";
      TString xpower   = "*x";
      TString newpower = "*x";
      for (Int_t i=1; i<=fDegree; i++)
      {
          formula += bra1;
          formula += i;
          formula += bra2;
          formula += xpower;

          xpower += newpower;
      }

      //*fLog << "FitPolynomial : formula = " << formula << endl;

      fPoly = new TF1(funcname, formula, xmin, xmax);
      TList *funclist = fHist->GetListOfFunctions();
      funclist->Add(fPoly);

      //------------------------
      // attention : the dimensions must agree with those in CallMinuit()
      const UInt_t npar = fDegree+1;

      TString parname[npar];
      TArrayD vinit(npar);
      TArrayD  step(npar);
      TArrayD limlo(npar);
      TArrayD limup(npar);
      TArrayI   fix(npar);

      vinit[0] =   mean;
      vinit[2] = a2init;

      for (UInt_t j=0; j<npar; j++)
      {
          parname[j]  = "p";
          parname[j] += j+1;

          step[j] = vinit[j] != 0.0 ? TMath::Abs(vinit[j]) / 10.0 : 0.000001;
      }

      // limit the first coefficient of the polynomial to positive values
      // because the background must not be negative
      limup[0] = fHist->GetEntries();

      // use the subsequernt loop if you want to apply the
      // constraint : uneven derivatives (at alpha=0) = zero
      for (UInt_t j=1; j<npar; j+=2)
      {
          vinit[j] = 0;
          step[j]  = 0;
          fix[j]   = 1;
      }

      //*fLog << "FitPolynomial : before CallMinuit()" << endl;

      MMinuitInterface inter;
      const Bool_t rc = inter.CallMinuit(fcnpoly, parname, vinit, step,
                                         limlo, limup, fix, fHist, "Migrad",
                                         kFALSE);

      //*fLog << "FitPolynomial : after CallMinuit()" << endl;

      if (rc != 0)
      {
          //  *fLog << "MHFindSignificance::FitPolynomial; polynomial fit failed"
          //        << endl;
          //  return kFALSE;
      }


      //-------------------
      // get status of minimization
      Double_t fmin   = 0;
      Double_t fedm   = 0;
      Double_t errdef = 0;
      Int_t    npari  = 0;
      Int_t    nparx  = 0;

      if (gMinuit)
          gMinuit->mnstat(fmin, fedm, errdef, npari, nparx, fIstat);

      *fLog << "MHFindSignificance::FitPolynomial; fmin, fedm, errdef, npari, nparx, fIstat = "
            << fmin << ",  " << fedm << ",  " << errdef << ",  " << npari
            << ",  " << nparx << ",  " << fIstat << endl;


      //-------------------
      // store the results

      Int_t nparfree = gMinuit!=NULL ? gMinuit->GetNumFreePars() : 0;
      fChisq         = fmin;
      fNdf           = fMbins - nparfree;
      fProb          = TMath::Prob(fChisq, fNdf);


      // get fitted parameter values and errors
      fValues.Set(npar);
      fErrors.Set(npar);

      for (Int_t j=0; j<=fDegree; j++)
      {
          Double_t val, err;
          if (gMinuit)
              gMinuit->GetParameter(j, val, err);

          fPoly->SetParameter(j, val);
          fPoly->SetParError(j, err);

          fValues[j] = val;
          fErrors[j] = err;
      }


      //--------------------------------------------------
      // if the highest coefficient (j0) of the polynomial
      // is consistent with zero reduce the degree of the polynomial

      Int_t j0 = 0;
      for (Int_t j=fDegree; j>1; j--)
      {
          // ignore fixed parameters
          if (fErrors[j] == 0)
              continue;

          // this is the highest coefficient
          j0 = j;
          break;
      }

      if (!fReduceDegree || j0==0 || TMath::Abs(fValues[j0]) > fErrors[j0])
          break;

      // reduce the degree of the polynomial
      *fLog << "MHFindSignificance::FitPolynomial; reduce the degree of the polynomial from "
          << fDegree << " to " << (j0-2) << endl;
      fDegree = j0 - 2;

      funclist->Remove(fPoly);
      //if (fPoly)
      delete fPoly;
      fPoly = NULL;

      // delete the Minuit object in order to have independent starting
      // conditions for the next minimization
      //if (gMinuit)
      delete gMinuit;
      gMinuit = NULL;
  }
  //===========   end of loop for reducing the degree   ==================
  //              of the polynomial


  //--------------------------------------------------
  // get the error matrix of the fitted parameters


  if (fIstat >= 1)
  {
      // error matrix was calculated
      if (gMinuit)
          gMinuit->mnemat(&fEmat[0][0], fNdim);

      // copy covariance matrix into a matrix which includes also the fixed
      // parameters
      TString  name;
      Double_t bnd1, bnd2, val, err;
      Int_t    jvarbl;
      Int_t    kvarbl;
      for (Int_t j=0; j<=fDegree; j++)
      {
          if (gMinuit)
              gMinuit->mnpout(j, name, val, err, bnd1, bnd2, jvarbl);

          for (Int_t k=0; k<=fDegree; k++)
          {
              if (gMinuit)
                  gMinuit->mnpout(k, name, val, err, bnd1, bnd2, kvarbl);

              fEma[j][k] = jvarbl==0 || kvarbl==0 ? 0 : fEmat[jvarbl-1][kvarbl-1];
          }
      }
  }
  else
  {
      // error matrix was not calculated, construct it
      *fLog << "MHFindSignificance::FitPolynomial; error matrix not defined"
          << endl;
      for (Int_t j=0; j<=fDegree; j++)
      {
          for (Int_t k=0; k<=fDegree; k++)
              fEma[j][k] = 0;

          fEma[j][j] = fErrors[j]*fErrors[j];
      }
  }


  //--------------------------------------------------
  // calculate correlation matrix
  for (Int_t j=0; j<=fDegree; j++)
      for (Int_t k=0; k<=fDegree; k++)
      {
          const Double_t sq = fEma[j][j]*fEma[k][k];
          fCorr[j][k] = sq==0 ? 0 : fEma[j][k] / TMath::Sqrt(fEma[j][j]*fEma[k][k]);
      }


  //--------------------------------------------------
  // reset the errors of the points in the histogram
  for (Int_t i=1; i<=nbins; i++)
      fHist->SetBinError(i, saveError[i-1]);


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// ReBinHistogram
//
// rebin the histogram 'fHistOrig' by grouping 'nrebin' bins together 
// put the result into the histogram 'fHist'
// the rebinning is made such that 'x0' remains a lower bound of a bin
//

Bool_t MHFindSignificance::RebinHistogram(Double_t x0, Int_t nrebin)
{
  //-----------------------------------------
  // search bin i0 which has x0 as lower edge

  Int_t i0 = -1;
  Int_t nbold = fHistOrig->GetNbinsX();
  for (Int_t i=1; i<=nbold; i++)
  {
      if (TMath::Abs(fHistOrig->GetBinLowEdge(i) - x0) < 1.e-4 )
      {
          i0 = i;
          break;
      }
  }

  if (i0 == -1)
  {
    i0 = 1;
    *fLog << "MHFindsignificance::Rebin; no bin found with " << x0
          << " as lower edge,  start rebinning with bin 1" << endl;
  }

  Int_t istart = i0 - nrebin * ( (i0-1)/nrebin );

  //-----------------------------------------
  // get new bin edges

  const Int_t    nbnew = (nbold-istart+1) / nrebin;
  const Double_t xmin  = fHistOrig->GetBinLowEdge(istart);
  const Double_t xmax  = xmin + (Double_t)nbnew * nrebin * fHistOrig->GetBinWidth(1);
  fHist->SetBins(nbnew, xmin, xmax);

  *fLog << "MHFindSignificance::ReBin; x0, i0, nbold, nbnew, xmin, xmax = "
        << x0 << ",  " << i0 << ",  " << nbold << ",  " << nbnew << ",  "
        << xmin << ",  " << xmax << endl;

  //-----------------------------------------
  // get new bin entries

  for (Int_t i=1; i<=nbnew; i++)
  {
      Int_t j = nrebin*(i-1) + istart;

      Double_t content = 0;
      Double_t error2  = 0;
      for (Int_t k=0; k<nrebin; k++)
      {
          content += fHistOrig->GetBinContent(j+k);
          error2  += fHistOrig->GetBinError(j+k) * fHistOrig->GetBinError(j+k);
      }
      fHist->SetBinContent(i, content);
      fHist->SetBinError  (i, sqrt(error2));
  }
  fHist->SetEntries( fHistOrig->GetEntries() );

  return kTRUE;
}

// --------------------------------------------------------------------------
//
//  FitGaussPoly
//
//  fits a (Gauss + polynomial function) to the alpha distribution 'fhist' 
//
//
Bool_t MHFindSignificance::FitGaussPoly()
{
  *fLog << "Entry FitGaussPoly" << endl;

  //--------------------------------------------------
  // check the histogram :
  //       - calculate initial values of the parameters
  //       - check for bins with zero entries
  //       - set minimum errors
  //       - save the original errors
  //       - set errors huge outside the fit range
  //         (in 'fcnpoly' points with huge errors will be ignored)


  Double_t dummy = 1.e20;

  fGNzero   = 0;
  fGMbins   = 0;

  //------------------------------------------
  // if a constant background has been assumed (due to low statistics)
  // fit only in the signal region
  if ( !fConstantBackg )
  {
    fAlphalow = 0.0;
    fAlphahig = fAlphamax;
  }
  else
  {
    fAlphalow = 0.0;
    fAlphahig = 2.0*fAlphasig>25.0 ? 25.0 : 2.0*fAlphasig;
  }
  //------------------------------------------


  fAlphalo =  10000.0;
  fAlphahi = -10000.0;


  Int_t nbins = fHist->GetNbinsX();
  TArrayD saveError(nbins);

  for (Int_t i=1; i<=nbins; i++)
  {
    saveError[i-1] = fHist->GetBinError(i);

    // bin should be completely contained in the fit range
    // (fAlphalow, fAlphahig)
    Double_t  xlo = fHist->GetBinLowEdge(i);
    Double_t  xup = fHist->GetBinLowEdge(i+1);

    if ( xlo >= fAlphalow-fEps  &&  xlo <= fAlphahig+fEps  &&
	 xup >= fAlphalow-fEps  &&  xup <= fAlphahig+fEps     )
    {
      fGMbins++;

      if ( xlo < fAlphalo )
        fAlphalo = xlo;

      if ( xup > fAlphahi )
        fAlphahi = xup;

      Double_t content = fHist->GetBinContent(i);


      // count bins with zero entry
      if (content <= 0.0)
        fGNzero++;
     
      // set minimum error
      if (content < 9.0)
        fHist->SetBinError(i, 3.0);

      //*fLog << "Take : i, content, error = " << i << ",  " 
      //      << fHist->GetBinContent(i) << ",  "
      //      << fHist->GetBinError(i)   << endl;

      continue;
    }    
    // bin is not completely contained in the fit range : set error huge

    fHist->SetBinError(i, dummy);

    //*fLog << "Omit : i, content, error = " << i << ",  " 
    //      << fHist->GetBinContent(i) << ",  " << fHist->GetBinError(i) 
    //      << endl;

  }


  // if a bin has no entries don't fit
  if (fGNzero > 0)
  {
    *fLog << "MHFindSignificance::FitGaussPoly; out of " << fGMbins 
          << " bins there are " << fGNzero
          << " bins with zero entry" << endl;

    fGPoly = NULL;
    return kFALSE;
  }


  //--------------------------------------------------
  // prepare fit of a (polynomial+Gauss) :   
  // (a0 + a1*x + a2*x**2 + a3*x**3 + ...) + A*exp( -0.5*((x-x0)/sigma)**2 )

  TString funcname = "PolyGauss";
  Double_t xmin =   0.0;
  Double_t xmax =  90.0;

  TString xpower   = "*x";
  TString newpower = "*x";

  TString formulaBackg = "[0]";
  for (Int_t i=1; i<=fDegree; i++)
      formulaBackg += Form("+[%d]*x^%d", i, i);

  const TString formulaGauss = 
        Form("[%d]/[%d]*exp(-0.5*((x-[%d])/[%d])^2)",
             fDegree+1, fDegree+3, fDegree+2, fDegree+3);

  TString formula = formulaBackg;
  formula += "+";
  formula += formulaGauss;

  *fLog << "FitGaussPoly : formulaBackg = " << formulaBackg << endl;
  *fLog << "FitGaussPoly : formulaGauss = " << formulaGauss << endl;
  *fLog << "FitGaussPoly : formula = " << formula << endl;

  fGPoly = new TF1(funcname, formula, xmin, xmax);
  TList *funclist = fHist->GetListOfFunctions();
  funclist->Add(fGPoly);

  fGBackg = new TF1("Backg", formulaBackg, xmin, xmax);
  funclist->Add(fGBackg);

  //------------------------
  // attention : the dimensions must agree with those in CallMinuit()
  Int_t npar = fDegree+1 + 3;

  TString parname[npar];
  TArrayD vinit(npar);
  TArrayD  step(npar); 
  TArrayD limlo(npar); 
  TArrayD limup(npar); 
  TArrayI   fix(npar);


  // take as initial values for the polynomial 
  // the result from the polynomial fit
  for (Int_t j=0; j<=fDegree; j++)
    vinit[j] = fPoly->GetParameter(j);

  Double_t sigma = 8;
  vinit[fDegree+1] = 2.0 * fNex * fHist->GetBinWidth(1) / TMath::Sqrt(TMath::Pi()*2);
  vinit[fDegree+2] = 0;
  vinit[fDegree+3] = sigma;

  *fLog << "FitGaussPoly : starting value for Gauss-amplitude = " 
        << vinit[fDegree+1] << endl;

  for (Int_t j=0; j<npar; j++)
  {
      parname[j]  = "p";
      parname[j] += j+1;

      step[j] = vinit[j]!=0 ? TMath::Abs(vinit[j]) / 10.0 : 0.000001;
  }

  // limit the first coefficient of the polynomial to positive values
  // because the background must not be negative
  limup[0] = fHist->GetEntries()*10;

  // limit the sigma of the Gauss function
  limup[fDegree+3] = 20;


  // use the subsequernt loop if you want to apply the
  // constraint : uneven derivatives (at alpha=0) = zero
  for (Int_t j=1; j<=fDegree; j+=2)
  {
      vinit[j] = 0;
      step[j]  = 0;
      fix[j]   = 1;
  }

  // fix position of Gauss function
  vinit[fDegree+2] = 0;
  step[fDegree+2]  = 0;
  fix[fDegree+2]   = 1;
   
  // if a constant background has been assumed (due to low statistics)
  // fix the background
  if (fConstantBackg)
  {
      step[0] = 0;
      fix[0]  = 1;
  }

  MMinuitInterface inter;
  const Bool_t rc = inter.CallMinuit(fcnpolygauss, parname, vinit, step,
                                     limlo, limup, fix, fHist, "Migrad",
                                     kFALSE);

  if (rc != 0)
  {
  //  *fLog << "MHFindSignificance::FitGaussPoly; (polynomial+Gauss) fit failed"
  //        << endl;
  //  return kFALSE;
  }


  //-------------------
  // get status of the minimization
  Double_t fmin;
  Double_t fedm;
  Double_t errdef;
  Int_t    npari;
  Int_t    nparx;

  if (gMinuit)
    gMinuit->mnstat(fmin, fedm, errdef, npari, nparx, fGIstat);

  *fLog << "MHFindSignificance::FitGaussPoly; fmin, fedm, errdef, npari, nparx, fGIstat = "
        << fmin << ",  " << fedm << ",  " << errdef << ",  " << npari
        << ",  " << nparx << ",  " << fGIstat << endl;


  //-------------------
  // store the results

  Int_t nparfree  = gMinuit!=NULL ? gMinuit->GetNumFreePars() : 0;
  fGChisq         = fmin; 
  fGNdf           = fGMbins - nparfree;
  fGProb          = TMath::Prob(fGChisq, fGNdf);


  // get fitted parameter values and errors
  fGValues.Set(npar);
  fGErrors.Set(npar);

  for (Int_t j=0; j<npar; j++)
  {
    Double_t val, err;
    if (gMinuit)
      gMinuit->GetParameter(j, val, err);

    fGPoly->SetParameter(j, val);
    fGPoly->SetParError(j, err);

    fGValues[j] = val;
    fGErrors[j] = err; 

    if (j <=fDegree)
    {
      fGBackg->SetParameter(j, val);
      fGBackg->SetParError(j, err);
    }
  }

  fSigmaGauss  = fGValues[fDegree+3];
  fdSigmaGauss = fGErrors[fDegree+3];
  // fitted total number of excess events 
  fNexGauss = fGValues[fDegree+1] * TMath::Sqrt(TMath::Pi()*2) /
                                         (fHist->GetBinWidth(1)*2 );
  fdNexGauss = fNexGauss * fGErrors[fDegree+1]/fGValues[fDegree+1];

  //--------------------------------------------------
  // get the error matrix of the fitted parameters


  if (fGIstat >= 1)
  {
    // error matrix was calculated
    if (gMinuit)
      gMinuit->mnemat(&fGEmat[0][0], fGNdim);

    // copy covariance matrix into a matrix which includes also the fixed
    // parameters
    TString  name;
    Double_t bnd1, bnd2, val, err;
    Int_t    jvarbl;
    Int_t    kvarbl;
    for (Int_t j=0; j<npar; j++)
    {
        if (gMinuit)
            gMinuit->mnpout(j, name, val, err, bnd1, bnd2, jvarbl);

        for (Int_t k=0; k<npar; k++)
        {
            if (gMinuit)
                gMinuit->mnpout(k, name, val, err, bnd1, bnd2, kvarbl);

            fGEma[j][k] = jvarbl==0 || kvarbl==0 ? 0 : fGEmat[jvarbl-1][kvarbl-1];
        }
    }
  }
  else
  {
    // error matrix was not calculated, construct it
    *fLog << "MHFindSignificance::FitPolynomial; error matrix not defined" 
          << endl;
    for (Int_t j=0; j<npar; j++)
    {
        for (Int_t k=0; k<npar; k++)
            fGEma[j][k] = 0;

        fGEma[j][j] = fGErrors[j]*fGErrors[j];
    }
  }


  //--------------------------------------------------
  // calculate correlation matrix
  for (Int_t j=0; j<npar; j++)
  {
    for (Int_t k=0; k<npar; k++)
    {
        const Double_t sq = fGEma[j][j]*fGEma[k][k];
        fGCorr[j][k] = sq==0 ? 0 : fGEma[j][k] / sqrt( fGEma[j][j]*fGEma[k][k] );
    }
  }


  //--------------------------------------------------
  // reset the errors of the points in the histogram
  for (Int_t i=1; i<=nbins; i++)
    fHist->SetBinError(i, saveError[i-1]);

  return kTRUE;

}

// --------------------------------------------------------------------------
//
//  DetExcess
//
//  using the result of the polynomial fit (fValues), DetExcess determines
//
//  - the total number of events in the signal region (fNon)
//  - the number of backgound events in the signal region (fNbg)
//  - the number of excess events (fNex)
//  - the effective number of background events (fNoff), and fGamma :
//    fNbg = fGamma * fNoff;  fdNbg = fGamma * sqrt(fNoff);
//
//  It assumed that the polynomial is defined as
//               a0 + a1*x + a2*x**2 + a3*x**3 + ..
//
//  and that the alpha distribution has the range 0 < alpha < 90 degrees
//

Bool_t MHFindSignificance::DetExcess()
{
  //*fLog << "MHFindSignificance::DetExcess;" << endl;

  //--------------------------------------------
  // calculate the total number of events (fNon) in the signal region

  fNon  = 0.0;
  fdNon = 0.0;

  Double_t alphaup = -1000.0; 
  Double_t binwidth = fHist->GetBinWidth(1);

  Int_t nbins = fHist->GetNbinsX();
  for (Int_t i=1; i<=nbins; i++)
  {
    Double_t  xlo = fHist->GetBinLowEdge(i);
    Double_t  xup = fHist->GetBinLowEdge(i+1);
 
    // bin must be completely contained in the signal region
    if ( xlo <= (fAlphasig+fEps)  &&  xup <= (fAlphasig+fEps)    )
    {
      Double_t width = fabs(xup-xlo);
      if (fabs(width-binwidth) > fEps)
      {
        *fLog << "MHFindSignificance::DetExcess; alpha plot has variable binning, which is not allowed" 
          << endl;
        return kFALSE;
      }

      if (xup > alphaup)
        alphaup = xup;

      fNon  += fHist->GetBinContent(i);
      fdNon += fHist->GetBinError(i) * fHist->GetBinError(i);
    }
  }
  fdNon = sqrt(fdNon);

  // the actual signal range is :
  if (alphaup == -1000.0)
    return kFALSE;

  fAlphasi = alphaup;

  //*fLog << "fAlphasi, fNon, fdNon, binwidth, fDegree = " << fAlphasi << ",  "
  //      << fNon << ",  " << fdNon << ",  " << binwidth << ",  "
  //      << fDegree << endl;

  //--------------------------------------------
  // calculate the number of background events (fNbg) in the signal region
  // and its error (fdNbg) 

  Double_t fac = 1.0/binwidth;

  fNbg         = 0.0;
  Double_t altothejplus1 = fAlphasi;
  for (Int_t j=0; j<=fDegree; j++)
  {
    fNbg += fValues[j] * altothejplus1 / ((Double_t)(j+1));
    altothejplus1 *= fAlphasi;
  }
  fNbg *= fac;

  // derivative of Nbg 
  Double_t facj;
  Double_t fack;

  Double_t sum = 0.0;
  altothejplus1 = fAlphasi;
  for (Int_t j=0; j<=fDegree; j++)
  {
    facj = altothejplus1 / ((Double_t)(j+1));

    Double_t altothekplus1 = fAlphasi;    
    for (Int_t k=0; k<=fDegree; k++)
    {
      fack = altothekplus1 / ((Double_t)(k+1));

      sum   += facj * fack * fEma[j][k];
      altothekplus1 *= fAlphasi;
    }
    altothejplus1 *= fAlphasi;
  }
  sum  *= fac*fac;

  if (sum < 0.0)
  {
    *fLog << "MHFindsignificance::DetExcess; error squared is negative" 
          << endl;
    return kFALSE;
  }

  fdNbg = sqrt(sum);


  //--------------------------------------------
  // AS A CHECK :
  // calculate the number of background events (fNbgtotFitted) in the 
  // background region, and its error (fdNbgtotFitted) 
  // expect fdnbg to be approximately equal to sqrt(fNbgtotFitted)

  Double_t fNmi = 0.0;
  altothejplus1 = fAlphami;
  for (Int_t j=0; j<=fDegree; j++)
  {
    fNmi += fValues[j] * altothejplus1 / ((Double_t)(j+1));
    altothejplus1 *= fAlphami;
  }
  fNmi *= fac;

  Double_t fNma = 0.0;
  altothejplus1 = fAlphama;
  for (Int_t j=0; j<=fDegree; j++)
  {
    fNma += fValues[j] * altothejplus1 / ((Double_t)(j+1));
    altothejplus1 *= fAlphama;
  }
  fNma *= fac;

  fNbgtotFitted  = fNma - fNmi;

  //----------------------

  sum = 0.0;
  Double_t altothejma = fAlphama;
  Double_t altothejmi = fAlphami;
  for (Int_t j=0; j<=fDegree; j++)
  {
    facj = (altothejma-altothejmi) / ((Double_t)(j+1));

    Double_t altothekma = fAlphama;    
    Double_t altothekmi = fAlphami;    
    for (Int_t k=0; k<=fDegree; k++)
    {
      fack = (altothekma-altothekmi) / ((Double_t)(k+1));

      sum   += facj * fack * fEma[j][k];
      altothekma *= fAlphama;
      altothekmi *= fAlphami;
    }
    altothejma *= fAlphama;
    altothejmi *= fAlphami;
  }
  sum  *= fac*fac;

  fdNbgtotFitted = sqrt(sum);
  if ( fabs(fdNbgtotFitted - sqrt(fNbgtotFitted)) > 0.2 * sqrt(fNbgtotFitted) )
  {
    *fLog << "MHFindSignificance::DetExcess; error of calculated number of background events (in the background region) does not agree with the expectation :"
          << endl;
    *fLog << "                    fNbgtotFitted, fdNbgtotFitted = " 
          << fNbgtotFitted << ",  " << fdNbgtotFitted
          << ",  expected : " << sqrt(fNbgtotFitted) << endl;         
  } 


  //--------------------------------------------
  // calculate the number of excess events in the signal region

  fNex = fNon - fNbg;

  //--------------------------------------------
  // calculate the effective number of background events (fNoff) , and fGamma :
  // fNbg = fGamma * fNoff;   dfNbg = fGamma * sqrt(fNoff);

  if (fNbg < 0.0)
  {
    *fLog << "MHFindSignificamce::DetExcess; number of background events is negative,  fNbg, fdNbg = "
          << fNbg  << ",  " << fdNbg << endl;

    fGamma = 1.0;
    fNoff  = 0.0;
    return kFALSE;
  }

  if (fNbg > 0.0)
  {
    fGamma = fdNbg*fdNbg / fNbg;
    fNoff  =  fNbg*fNbg  / (fdNbg*fdNbg);
  }
  else
  {
    fGamma = 1.0;
    fNoff  = 0.0;
  }

  //*fLog << "Exit DetExcess()" << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
//  SigmaLiMa
//
//  calculates the significance according to Li & Ma
//  ApJ 272 (1983) 317
//
Bool_t MHFindSignificance::SigmaLiMa(Double_t non,   Double_t noff, 
                                     Double_t gamma, Double_t *siglima)
{
  if (gamma <= 0.0  ||  non <= 0.0  ||  noff <= 0.0)
  {
    *siglima = 0.0;
    return kFALSE;
  }

  Double_t help1 = non  * log( (1.0+gamma)*non  / (gamma*(non+noff)) );
  Double_t help2 = noff * log( (1.0+gamma)*noff / (       non+noff ) );
  *siglima = sqrt( 2.0 * (help1+help2) );

  Double_t nex = non - gamma*noff;
  if (nex < 0.0)
    *siglima = - *siglima;

  //*fLog << "MHFindSignificance::SigmaLiMa; non, noff, gamma, *siglima = "
  //      << non << ",  " << noff << ",  " << gamma << ",  " << *siglima << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MHFindSignificance::DrawFit(const Option_t *opt)
{
    if (fHist == NULL)
      *fLog << "MHFindSignificance::DrawFit; fHist = NULL" << endl;


    //TCanvas *fCanvas = new TCanvas("Alpha", "Alpha plot", 600, 600);
//      fCanvas = new TCanvas(fHist->GetName(), "Alpha plot", 600, 600);

    TVirtualPad *c = gPad ? gPad : MakeDefCanvas(this);

    //gStyle->SetOptFit(1011);

    gROOT->SetSelectedPad(NULL);    
    gStyle->SetPadLeftMargin(0.1);

//      fCanvas->cd();
    c->cd();


    if (fHist)
    {
      fHist->DrawCopy();
    }

    TF1 *fpoly = fHist->GetFunction("Poly");    
    if (fpoly == NULL)
      *fLog << "MHFindSignificance::DrawFit; fpoly = NULL" << endl;

    if (fpoly)
    {
      // 2, 1 is red and solid
      fpoly->SetLineColor(2);
      fpoly->SetLineStyle(1);
      fpoly->SetLineWidth(2);
      fpoly->DrawCopy("same");
    }

    if (fFitGauss)
    {
      TF1 *fpolygauss = fHist->GetFunction("PolyGauss");    
      if (fpolygauss == NULL)
        *fLog << "MHFindSignificance::DrawFit; fpolygauss = NULL" << endl;

      if (fpolygauss)
      {
        // 4, 1 is blue and solid
        fpolygauss->SetLineColor(4);
        fpolygauss->SetLineStyle(1);
        fpolygauss->SetLineWidth(4);
        fpolygauss->DrawCopy("same");
      }

      TF1 *fbackg = fHist->GetFunction("Backg");    
      if (fbackg == NULL)
        *fLog << "MHFindSignificance::DrawFit; fbackg = NULL" << endl;

      if (fbackg)
      {
        // 6, 4 is pink and dotted
        fbackg->SetLineColor(4);
        fbackg->SetLineStyle(4);
        fbackg->SetLineWidth(4);
        fbackg->DrawCopy("same");
      }
    }


    //-------------------------------
    // print results onto the figure
    TPaveText *pt = new TPaveText(0.30, 0.35, 0.70, 0.90, "NDC");
    char tx[100];

    sprintf(tx, "Results of polynomial fit (order %2d) :", fDegree);
    TText *t1 = pt->AddText(tx);
    t1->SetTextSize(0.03);
    t1->SetTextColor(2);

    sprintf(tx, "   (%6.2f< |alpha| <%6.2f [\\circ])", fAlphami, fAlphama);
    pt->AddText(tx);

    sprintf(tx, "   chi2 = %8.2f,  Ndof = %4d,  Prob = %6.2f", 
            fChisq, fNdf, fProb);
    pt->AddText(tx);

    sprintf(tx, "   Nbgtot(fit) = %8.1f #pm %8.1f", 
            fNbgtotFitted, fdNbgtotFitted);
    pt->AddText(tx);

    sprintf(tx, "   Nbgtot(meas) = %8.1f", fNbgtot);
    pt->AddText(tx);


    //sprintf(tx, "     ");
    //pt->AddText(tx);

    //--------------
    sprintf(tx, "Results for |alpha|< %6.2f [\\circ] :", fAlphasi);
    TText *t6 = pt->AddText(tx);
    t6->SetTextSize(0.03);
    t6->SetTextColor(8);

    sprintf(tx, "   Non = %8.1f #pm %8.1f", fNon, fdNon);
    pt->AddText(tx);

    sprintf(tx, "   Nex = %8.1f #pm %8.1f", fNex, fdNex);
    pt->AddText(tx);

    sprintf(tx, "   Nbg = %8.1f #pm %8.1f,    gamma = %6.1f", 
            fNbg, fdNbg, fGamma);
    pt->AddText(tx);

    Double_t ratio = fNbg>0.0 ? fNex/fNbg : 0.0;
    sprintf(tx, "   Significance = %6.2f,    Nex/Nbg = %6.2f", 
            fSigLiMa, ratio);
    pt->AddText(tx);

    //sprintf(tx, "     ");
    //pt->AddText(tx);

    //--------------
    if (fFitGauss)
    {
      sprintf(tx, "Results of (polynomial+Gauss) fit  :");
      TText *t7 = pt->AddText(tx);
      t7->SetTextSize(0.03);
      t7->SetTextColor(4);

      sprintf(tx, "   chi2 = %8.2f,  Ndof = %4d,  Prob = %6.2f", 
              fGChisq, fGNdf, fGProb);
      pt->AddText(tx);

      sprintf(tx, "   Sigma of Gauss = %8.1f #pm %8.1f  [\\circ]", 
              fSigmaGauss, fdSigmaGauss);
      pt->AddText(tx);

      sprintf(tx, "   total no.of excess events = %8.1f #pm %8.1f", 
              fNexGauss, fdNexGauss);
      pt->AddText(tx);
    }
    //--------------

    pt->SetFillStyle(0);
    pt->SetBorderSize(0);
    pt->SetTextAlign(12);

    pt->Draw();

//      fCanvas->Modified();
//      fCanvas->Update();
    c->Modified();
    c->Update();

    return kTRUE;
}



// --------------------------------------------------------------------------
//
// Print the results of the polynomial fit to the alpha distribution
// 
//
void MHFindSignificance::PrintPoly(Option_t *o) 
{
  *fLog << "---------------------------" << endl;
  *fLog << "MHFindSignificance::PrintPoly :" << endl; 

  *fLog << "fAlphami, fAlphama, fDegree, fAlphasi = "
        << fAlphami << ",  " << fAlphama << ",  " << fDegree << ",  " 
        << fAlphasi << endl;

  *fLog << "fMbins, fNzero, fIstat = " << fMbins << ",  "
        << fNzero << ",  " << fIstat << endl;

  *fLog << "fChisq, fNdf, fProb = " << fChisq << ",  " 
        << fNdf << ",  " << fProb << endl; 

  *fLog << "fNon, fNbg, fdNbg, fNbgtot, fNbgtotFitted, fdNbgtotFitted = "
        << fNon << ",  " << fNbg << ",  " << fdNbg << ",  " << fNbgtot 
        << ",  " << fNbgtotFitted << ",  " << fdNbgtotFitted << endl;

  Double_t sigtoback = fNbg>0.0 ? fNex/fNbg : 0.0;
  *fLog << "fNex, fdNex, fGamma, fNoff, fSigLiMa, sigtoback = "
        << fNex << ",  " << fdNex << ",  " << fGamma << ",  " << fNoff 
        << ",  " << fSigLiMa << ",  " << sigtoback << endl;

  //------------------------------------
  // get errors

  /*
  Double_t eplus; 
  Double_t eminus; 
  Double_t eparab; 
  Double_t gcc;
  Double_t errdiag;


  if ( !fConstantBackg )
  {
    *fLog << "parameter value     error     eplus     eminus    eparab   errdiag   gcc"
          << endl; 

    for (Int_t j=0; j<=fDegree; j++)
    {
      if (gMinuit)
        gMinuit->mnerrs(j, eplus, eminus, eparab, gcc);
      errdiag = sqrt(fEma[j][j]);
      *fLog << j << "  " << fValues[j] << "  "   << fErrors[j] << "  "
            << eplus     << "  "       << eminus << "  " << eparab     << "  " 
            <<  errdiag  << "  "       << gcc    << endl;
    }
  }  
  else
  {
    *fLog << "parameter value     error     errdiag "
          << endl; 

    for (Int_t j=0; j<=fDegree; j++)
    {
      errdiag = sqrt(fEma[j][j]);
      *fLog << j << "  " << fValues[j] << "  "   << fErrors[j] << "  "
            <<  errdiag  << endl;
    }
  }  
  */

  //----------------------------------------
  /*
  *fLog << "Covariance matrix :" << endl;
  for (Int_t j=0; j<=fDegree; j++)
  {
    *fLog << "j = " << j << " :   ";
    for (Int_t k=0; k<=fDegree; k++)
    {
      *fLog << fEma[j][k] << "   ";
    }
    *fLog << endl;
  }

  *fLog << "Correlation matrix :" << endl;
  for (Int_t j=0; j<=fDegree; j++)
  {
    *fLog << "j = " << j << " :   ";
    for (Int_t k=0; k<=fDegree; k++)
    {
      *fLog << fCorr[j][k] << "   ";
    }
    *fLog << endl;
  }
  */

  *fLog << "---------------------------" << endl;
}

// --------------------------------------------------------------------------
//
// Print the results of the (polynomial+Gauss) fit to the alpha distribution
// 
//
void MHFindSignificance::PrintPolyGauss(Option_t *o) 
{
  *fLog << "---------------------------" << endl;
  *fLog << "MHFindSignificance::PrintPolyGauss :" << endl; 

  *fLog << "fAlphalo, fAlphahi = "
        << fAlphalo << ",  " << fAlphahi << endl;

  *fLog << "fGMbins, fGNzero, fGIstat = " << fGMbins << ",  "
        << fGNzero << ",  " << fGIstat << endl;

  *fLog << "fGChisq, fGNdf, fGProb = " << fGChisq << ",  " 
        << fGNdf << ",  " << fGProb << endl; 


  //------------------------------------
  // get errors

  Double_t eplus; 
  Double_t eminus; 
  Double_t eparab; 
  Double_t gcc;
  Double_t errdiag;

  *fLog << "parameter value     error     eplus     eminus    eparab   errdiag   gcc"
        << endl; 
  for (Int_t j=0; j<=(fDegree+3); j++)
  {
    if (gMinuit)
      gMinuit->mnerrs(j, eplus, eminus, eparab, gcc);
    errdiag = sqrt(fGEma[j][j]);
    *fLog << j << "  " << fGValues[j] << "  "   << fGErrors[j] << "  "
          << eplus     << "  "       << eminus << "  " << eparab     << "  " 
          <<  errdiag  << "  "       << gcc    << endl;
  }

  
  *fLog << "Covariance matrix :" << endl;
  for (Int_t j=0; j<=(fDegree+3); j++)
  {
    *fLog << "j = " << j << " :   ";
    for (Int_t k=0; k<=(fDegree+3); k++)
    {
      *fLog << fGEma[j][k] << "   ";
    }
    *fLog << endl;
  }

  *fLog << "Correlation matrix :" << endl;
  for (Int_t j=0; j<=(fDegree+3); j++)
  {
    *fLog << "j = " << j << " :   ";
    for (Int_t k=0; k<=(fDegree+3); k++)
    {
      *fLog << fGCorr[j][k] << "   ";
    }
    *fLog << endl;
  }

  *fLog << "---------------------------" << endl;
}

//============================================================================

