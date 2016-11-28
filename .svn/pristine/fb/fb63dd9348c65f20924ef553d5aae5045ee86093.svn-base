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
! * in supporting documentation. It is provided "as is" without expressed
! * or implied warranty.
! *
!
!   Author(s) : R. Wagner, 02/2004 <mailto:rwagner@mppmu.mpg.de>           
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MUnfoldSpectrum
//
//  Unfolds a gamma spectrum using the algorithms given in the MUnfold class
//
/////////////////////////////////////////////////////////////////////////////   
#include "MUnfoldSpectrum.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "MLog.h"
#include "MLogManip.h"
#include "MUnfold.h"

ClassImp(MUnfoldSpectrum);
using namespace std;

MUnfoldSpectrum::MUnfoldSpectrum()
  : fUnfoldingMethod(2), fPrior(0)
{
      
}

void MUnfoldSpectrum::Calc()
{
  // Unfold # Excess events vs. Energy and Theta
  
  //   TH2D* tobeunfolded = hex->GetHist();
  //   TH2D* unfolded = new TH2D(*tobeunfolded);  
  //   TH3D* migration = migm->GetHist();
  
  //const Int_t energyBins = fToBeUnfolded->GetXaxis()->GetNbins();
  //const Int_t thetaBins =  fToBeUnfolded->GetYaxis()->GetNbins();
  //const TAxis* axisEnergy = fToBeUnfolded->GetXaxis();
  //const TAxis* axisTheta  = fToBeUnfolded->GetYaxis();
  //cout << "Distribution to be unfolded has " << energyBins 
  //     << ", " << thetaBins << " bins" <<endl;
  
  TAxis &taxis  = *fToBeUnfolded->GetYaxis();
  Int_t numybins = taxis.GetNbins();
  
  cout << "Processing a total number of  " << numybins << " bins. " <<endl;
  cout << "-------------------" << endl << endl;
  
  for (Int_t m=1; m<=numybins; m++) {
    TString bintitle = "Bin ";
    bintitle += m;
    bintitle += ": ";
    
    cout << "Processing " << bintitle << endl;
    
    // -----------------------------------------
    // ha : distribution to be unfolded
    
    TH1D &ha = *fToBeUnfolded->ProjectionX("", m, m, "e");
    TString title = bintitle;
    title += "E-est distr. to be unfolded";
    ha.SetNameTitle("ha", title);
    TAxis &aaxis = *ha.GetXaxis();
    Int_t na = aaxis.GetNbins();
    Double_t alow = aaxis.GetBinLowEdge(1);
    Double_t aup  = aaxis.GetBinLowEdge(na+1);
	
    cout << ha.GetName() << ": " << ha.GetTitle() << endl;
    cout << "-----------------------------------------------------" << endl;
    for (Int_t i=1; i<=ha.GetNbinsX(); i++)
      cout << ha.GetBinContent(i) << " \t";
    cout << endl << endl;
	
    // -----------------------------------------
    // covariance matrix of the distribution ha
      
    title = bintitle;
    title +=  "Error matrix of distribution ha";
    TH2D hacov("hacov", title, na, alow, aup, na, alow, aup);
          
    Double_t errmin = 3.0;
    for (Int_t i=1; i<=na; i++) {
      for (Int_t j=1; j<=na; j++) 
	hacov.SetBinContent(i, j, 0.0);	
      const Double_t content = ha.GetBinContent(i);
      const Double_t error2  = (ha.GetBinError(i))*(ha.GetBinError(i));
      if (content <= errmin  &&  error2 < errmin) 
	hacov.SetBinContent(i, i, errmin);
      else
	hacov.SetBinContent(i, i, error2);
    }
    
    //PrintTH2Content(hacov);
      
    // -----------------------------------------
    // migration matrix :
    //           x corresponds to measured quantity
    //           y corresponds to true quantity
    
    // The projection is made for the selected bins only.
    // To select a bin range along an axis, use TAxis::SetRange, eg
    // h3.GetYaxis()->SetRange(23,56);
    
    //        taxis->SetRange(m,m);
    TH2D &hmig = *(TH2D*)fMigrationMatrix->Project3D("yxe");
    title = bintitle;
    title += "Migration Matrix";
    hmig.SetNameTitle("Migrat", title);
    
    TAxis &aaxismig = *hmig.GetXaxis();
    Int_t namig = aaxismig.GetNbins();
    
    if (na != namig) {
      cout << "doUnfolding : binnings are incompatible; na, namig = "
	   << na << ",  " << namig << endl;
      return;
    }
    
    TAxis &baxismig = *hmig.GetYaxis();
    Int_t nbmig = baxismig.GetNbins();
    Double_t blow = baxismig.GetBinLowEdge(1);
    Double_t bup  = baxismig.GetBinLowEdge(nbmig+1);
          
    // -----------------------------------------
    // dummy ideal distribution
    
    Int_t nb = nbmig;
      
    title = bintitle;
    title += "Dummy Ideal distribution";
    TH1D hb0("dummyhb0", title, nb, blow, bup);
    //MH::SetBinning(&hb0, &baxismig); 
    hb0.Sumw2();
    
    for (Int_t k=1; k<=nb; k++) {
      hb0.SetBinContent(k, 1.0/nb);
      hb0.SetBinError  (k, 0.1/nb);
    }
	
    //PrintTH1Content(hb0);
        
    // -----------------------------------------
    // unfolded distribution
	
    title = bintitle;
    title += "Unfolded distribution";
    TH1D hb("hb", title, nb, blow, bup);
	                    
    // ha      is the distribution to be unfolded
    // hacov   is the covariance matrix of the distribution ha
    // hmig    is the migration matrix;
    //         it is used in the unfolding unless it is overwritten
    //         by SmoothMigrationMatrix by the smoothed migration matrix
    // hmigor  is the migration matrix to be smoothed;
    //         the smoothed migration matrix will be used in the unfolding
    // hpr     the prior distribution
    //         it is only used if SetPriorInput(*hpr) is called   
    // hb      unfolded distribution

    // create an MUnfold object;
    MUnfold unfold(ha, hacov, hmig);
    unfold.bintitle = bintitle;

    // smooth the migration matrix;
    //        the smoothed migration matrix will be used in the unfolding
    //        hmig is the original (unsmoothed) migration matrix
    unfold.SmoothMigrationMatrix(hmig);

    // define prior distribution (has always to be defined) 
    // the alternatives are 
    //   SetPriorConstant():   isotropic distribution
    //   SetPriorPower(gamma): dN/dE = E^{-gamma}
    //   SetPriorInput(*hpr):  the distribution *hpr is used 
    //   SetPriorRebin(*ha):   use rebinned histogram ha 
    Bool_t errorprior=kTRUE;
    switch (fPrior)
    {
    case 1:
        unfold.SetPriorConstant();
        break;
    case 2:
        errorprior = unfold.SetPriorPower(fPriorPowerGamma);
        break;
    case 3:
        if (!fPriorInputHist)
        {
            cout << "Error: No hpr!" << endl;
            return;
        }
        errorprior = unfold.SetPriorInput(*fPriorInputHist);
        break;
    case 4:
        errorprior = unfold.SetPriorRebin(*fPriorRebinHist);
        break;
    }
    if (!errorprior)
    {
        cout << "MUnfoldSpectrum::SetPrior... : failed.  fPrior = " ;
        cout << fPrior << endl;
        return;
    }

    // calculate the matrix G = M * M(transposed)
    //           M being the migration matrix
    unfold.CalculateG();

    switch (fUnfoldingMethod)
    {
    case 1: // Schmelling:
    // minimize the function Z by Gauss-Newton iteration;
    // the parameters to be fitted are gamma(i) = lambda(i)/w;
        cout << "Unfolding algorithm : Schmelling" << endl;
        if (!unfold.Schmelling(hb0)) cout << "MUnfoldSpectrum::Schmelling : failed." << endl;
        break;

    case 2: // Tikhonov2 :
    // regularization term is sum of (2nd deriv.)**2 ;
    // minimization by using MINUIT;
    // the parameters to be fitted are
    // the bin contents of the unfolded distribution
        cout << "Unfolding algorithm :   Tikhonov" << endl;
        if (!unfold.Tikhonov2(hb0)) cout << "MUnfoldSpectrum::Tikhonov2 : failed." << endl;
        break;

    case 3: // Bertero: minimization by iteration
        cout << "Unfolding algorithm :    Bertero" << endl;
        if (!unfold.Bertero(hb0)) cout << "MUnfoldSpectrum::Bertero : failed." << endl;
        break;
    }    
    unfold.PrintResults();
    unfold.DrawPlots();

    // get unfolded distribution
    TMatrixD &Vb    = unfold.GetVb();
    TMatrixD &Vbcov = unfold.GetVbcov();
    UInt_t fNb = unfold.fNb;
    for (UInt_t a=0; a<fNb; a++)
    {
      hb.SetBinContent(a+1, Vb(a,0));
      hb.SetBinError(a+1, sqrt(Vbcov(a, a)) );
    }
    
    for (Int_t k=1; k<=nb; k++) {
      Double_t content = hb.GetBinContent(k);
      Double_t error   = hb.GetBinError(k);
      
      fUnfolded->SetBinContent(k, m, content);
      fUnfolded->SetBinError(k, m, error);
      
      //hex->FillBinContent(k, m, content, error);
    }    	
  }
}


// --------------------------------------------------------------------------
//
// Default destructor.
//
MUnfoldSpectrum::~MUnfoldSpectrum()
{
}


// -----------------------------------------------------------------------
//
// Define prior distribution to be a constant
//
void MUnfoldSpectrum::SetPriorConstant()
{
  fPrior=1;  
}

void MUnfoldSpectrum::SetPriorRebin(TH1D *ha)
{
  fPriorRebinHist=ha;
  fPrior=2;
}
 
void MUnfoldSpectrum::SetPriorInput(TH1D *hpr)
{
  fPriorInputHist=hpr;
  fPrior=3;
}

void MUnfoldSpectrum::SetPriorPower(Double_t gamma)
{
  fPriorPowerGamma=gamma;
  fPrior=4;
}

