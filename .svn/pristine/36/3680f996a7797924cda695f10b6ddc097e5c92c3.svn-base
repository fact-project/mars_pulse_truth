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
!   Author(s): Javier Rico     02/2005 <mailto:jrico@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHMcUnfoldCoeff
// Container that stores the coefficients to convert from estimated to real
// (unfolded) energy. 
// It is filled by MMcUnfoldCoeffCalc
// Events are weighted from any initial spectrum to a tentative spectrum
// that must be set through MMcUnfoldCoeffCalc::SetSpectrum method.
//
//////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <math.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TArrayD.h"

#include "MHMcUnfoldCoeff.h"

#include "MBinning.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHMcUnfoldCoeff);

using namespace std;


// -------------------------------------------------------------------------
//
// Constructor: Define and configure the different histograms to be filled
//
MHMcUnfoldCoeff::MHMcUnfoldCoeff(const char *name, const char *title) :
  fNsubbins(20), fEmin(1.), fEmax(100000.), fNumMin(10), fFineBinning(NULL)
{
  fName  = name  ? name  : "MHMcUnfoldCoeff";
  fTitle = title ? title : "Unfolding Coefficients vs. Theta vs. Energy";

  // define histos
  fHistAll    = new TH1D(); 
  fHistWeight = new TH1D(); 
  fHistMcE    = new TH2D();
  fHistEstE   = new TH2D();
  fHistCoeff  = new TH2D();

  // set histo names
  fHistAll->SetName("AllEvents");
  fHistWeight->SetName("Weights");
  fHistMcE->SetName("MCEnergy");
  fHistEstE->SetName("EstimatedEnergy");
  fHistCoeff->SetName("UnfoldCoeff");

  // set histo titles
  fHistAll->SetTitle("MC energy for all generated events");
  fHistWeight->SetTitle("Weights");
  fHistMcE->SetTitle("MC energy for survivors");
  fHistEstE->SetTitle("Estimate energy for survivors");
  fHistCoeff->SetTitle(fTitle);

  // histos directory
  fHistAll->SetDirectory(NULL);
  fHistWeight->SetDirectory(NULL);
  fHistMcE->SetDirectory(NULL);
  fHistEstE->SetDirectory(NULL);
  fHistCoeff->SetDirectory(NULL);

  // histos style
  fHistAll->UseCurrentStyle();
  fHistWeight->UseCurrentStyle();
  fHistMcE->UseCurrentStyle();
  fHistEstE->UseCurrentStyle();
  fHistCoeff->UseCurrentStyle();

  // labels
  fHistAll->SetXTitle("E [GeV]");
  fHistWeight->SetXTitle("E [GeV]");
  fHistMcE->SetXTitle("E_{MC} [GeV]");
  fHistEstE->SetXTitle("E_{est} [GeV]");
  fHistCoeff->SetXTitle("E_{est} [GeV]");

  fHistWeight->SetYTitle("weight");
  fHistMcE->SetYTitle("\\theta [\\circ]");
  fHistEstE->SetYTitle("\\theta [\\circ]");
  fHistCoeff->SetYTitle("\\theta [\\circ]");

  fHistMcE->SetZTitle("weighted entries");
  fHistEstE->SetZTitle("weighted entries");
  fHistCoeff->SetZTitle("Coefficient");
}

// -------------------------------------------------------------------------
//
// Destructor: Delete all the histograms
//
MHMcUnfoldCoeff::~MHMcUnfoldCoeff()
{
  delete fHistAll;
  delete fHistWeight;
  delete fHistMcE;
  delete fHistEstE;
  delete fHistCoeff;

  if(fFineBinning) 
    delete fFineBinning;
}

// --------------------------------------------------------------------------
//
// Set the histograms binning. The coarse binning has to be provided
// from outside (in the parameter list) whereas the fine binning for
// energy is computed from the former.
// The subbinning is set logarithmicaly (linearly) for a logarithmic
// (linear) coarse binning. 
// It is extended down to fEmin and up to fEmax
// The number of subbins per coarse bins may be changed using
// SetNsubbins() method
// The maximum and minimum energies of the fine binning histograms
// may be changed with the SetEmin() and SetEmax() methods
//
void MHMcUnfoldCoeff::SetCoarseBinnings(const MBinning &binsEnergy, 
					const MBinning &binsTheta)
{
  // set histo binning (for coarse bin histos)
  MH::SetBinning(fHistMcE,&binsEnergy,&binsTheta);
  MH::SetBinning(fHistEstE,&binsEnergy,&binsTheta);
  MH::SetBinning(fHistCoeff,&binsEnergy,&binsTheta);

  // good errors
  fHistMcE->Sumw2();
  fHistEstE->Sumw2();

  // define fine binning from coarse one
  const Int_t nbins = binsEnergy.GetNumBins();
  const TArrayD edges = binsEnergy.GetEdgesD();
  const Bool_t isLinear = binsEnergy.IsLinear();

  // compute bins needed to extend down to fEmin and up to fEmax
  Double_t dedown, deup;
  Int_t binsdown,binsup;
  MBinning dbin;
  MBinning ubin;
  if(isLinear)
    {
      dedown   = (edges[1]-edges[0])/fNsubbins;
      deup     = (edges[nbins]-edges[nbins-1])/fNsubbins;      
      binsdown = (fEmin<edges[0])     ? Int_t((edges[0]-fEmin)/dedown) : 0;
      binsup   = (fEmax>edges[nbins]) ? Int_t((fEmax-edges[nbins])/deup) : 0;
      if(binsdown) dbin.SetEdges(binsdown,fEmin,edges[0]);
      if(binsup)   ubin.SetEdges(binsup,edges[nbins],fEmax);
    }
  else
    {
      dedown   = (TMath::Log10(edges[1])-TMath::Log10(edges[0]))/fNsubbins;
      deup     = (TMath::Log10(edges[nbins])-TMath::Log10(edges[nbins-1]))/fNsubbins;
      binsdown = (fEmin<edges[0])    ? Int_t((TMath::Log10(edges[0])-TMath::Log10(fEmin))/dedown) : 0;
      binsup   = (fEmax>edges[nbins])? Int_t((TMath::Log10(fEmax)-TMath::Log10(edges[nbins]))/deup) : 0;
      if(binsdown) dbin.SetEdgesLog(binsdown,fEmin,edges[0]);
      if(binsup)   ubin.SetEdgesLog(binsup,edges[nbins],fEmax);
    }
  
  // fill the subins' edges
  TArrayD fineedges(binsdown+nbins*fNsubbins+binsup+1);

  for(Int_t i=0;i<binsdown;i++)
    fineedges[i] = dbin.GetEdgesD().At(i);

  for(Int_t i=0;i<nbins;i++)
    {
      MBinning coarb;
      if(isLinear)
	coarb.SetEdges(fNsubbins,edges[i],edges[i+1]);
      else
	coarb.SetEdgesLog(fNsubbins,edges[i],edges[i+1]);
      
      for(Int_t j=0;j<fNsubbins;j++)
	fineedges[binsdown+i*fNsubbins+j] = coarb.GetEdgesD().At(j);
    }  

  for(Int_t i=0;i<binsup;i++)
    fineedges[binsdown+nbins*fNsubbins+i] = ubin.GetEdgesD().At(i);
  
  fineedges[binsdown+nbins*fNsubbins+binsup] = binsup ? ubin.GetEdgesD().At(binsup): edges[nbins];

  // define fine binning object
  fFineBinning = new MBinning;
  fFineBinning->SetEdges(fineedges);

  // apply fine binning to histograms that need it
  fFineBinning->Apply(*fHistAll);
  fFineBinning->Apply(*fHistWeight);  
}

// -------------------------------------------------------------------------
//
// Compute the weights for a particular tentative spectrum.
// For each energy (fine) bin we compute it as the value of the input function
// divided by the number of entries in the actual energy histogram.
// First and last bin are not filled since they could be biased
//
void MHMcUnfoldCoeff::ComputeWeights(TF1* spectrum)
{  
  Bool_t first=kTRUE;
  Int_t  lastb=0;
  for(Int_t i=0;i<fHistAll->GetNbinsX();i++)
    {
      const Float_t denom = fHistAll->GetBinContent(i+1); // number of events in initial spectrum
      const Float_t ew    = fHistAll->GetBinCenter(i+1);  // energy associated to the bin
      const Float_t numer = spectrum->Eval(ew);           // number of events for the required spectrum

      if(denom>fNumMin)
	{
	  // do not fill it if it is the first one
	  if(first)
	    {
	      fHistWeight->SetBinContent(i+1,-1);
	      first=kFALSE;
	    }
	  else
	    {
	      fHistWeight->SetBinContent(i+1,numer/denom);	
	      lastb=i+1;
	    }
	}
      else
	fHistWeight->SetBinContent(i+1,-1);
    }

  //remove last filled bin
  if(lastb)
    fHistWeight->SetBinContent(lastb,-1);
}

// --------------------------------------------------------------
//
// Compute the coefficients used for the (iterative) unfolding
// The coefficients are the ratio between the contents of the
// mc energy and estimate energy histograms (filled with weighted
// events
// Errors are computed as if estimated and MC energy histos where
// uncorrelated
//
void MHMcUnfoldCoeff::ComputeCoefficients()
{ 
  for(Int_t j=0;j<fHistEstE->GetNbinsY();j++)
    for(Int_t i=0;i<fHistEstE->GetNbinsX();i++)
      {
	const Float_t num = fHistMcE->GetBinContent(i+1,j+1);	
	const Float_t den = fHistEstE->GetBinContent(i+1,j+1);
		
	if(den>0)
	  {	    
	    const Float_t cf  = num/den;
	    fHistCoeff->SetBinContent(i+1,j+1,cf);

	    if(num>0)
	      {
		const Float_t ernum = fHistMcE->GetBinError(i+1,j+1);
		const Float_t erden = fHistEstE->GetBinError(i+1,j+1);
		const Float_t ercf  = cf*(TMath::Sqrt(ernum/num*ernum/num+erden/den*erden/den));
		fHistCoeff->SetBinError(i+1,j+1,ercf);
	      }
	    else
	      fHistCoeff->SetBinError(i+1,j+1,0);		  
	  }
	else
	  {
	    *fLog << warn << "MHMcUnfoldCoeff::ComputeCoefficients Warning: energy bin " << i << ", thetabin " << j << " has no survivors, setting coefficient to 0" << endl;
	    fHistCoeff->SetBinContent(i+1,j+1,0.);
	    fHistCoeff->SetBinError(i+1,j+1,0.);
	  }
      }
}

// --------------------------------------------------------------------------
//
// Fill data into the histogram which contains all showers
//
void MHMcUnfoldCoeff::FillAll(Double_t energy)
{
  fHistAll->Fill(energy);
}

// --------------------------------------------------------------------------
//
// Fill data into the histograms which contain survivors
// each event is introduced with a weight depending on its (MC) energy
//
void MHMcUnfoldCoeff::FillSel(Double_t mcenergy,Double_t estenergy,Double_t theta)
{
  // bin of the corresponding weight
  const Int_t wbin = fFineBinning->FindHiEdge(mcenergy);

  if(wbin>0)
    {
      // weight
      const Double_t weight = fHistWeight->GetBinContent(wbin);
      
      if(weight>0)
	{
	  fHistMcE->Fill(mcenergy,theta,weight);
	  fHistEstE->Fill(estenergy,theta,weight);
	}
      //      else
	//	*fLog << warn << "MHMcUnfoldCoeff::FillSel Warning: event with energy " << mcenergy << " has no computed weight (lack of MC statistics), skipping" << endl;
    }
  // the event has an energy out of the considered range
  else
    *fLog << warn << "MHMcUnfoldCoeff::FillSel Warning: event with energy " << mcenergy << " has energy out of bounds, skipping" << endl;   
}

// --------------------------------------------------------------------------
//
// Draw
//
void MHMcUnfoldCoeff::Draw(Option_t* option)
{
  TCanvas *c1 = new TCanvas();  
  c1->SetLogx();
  c1->SetLogy();
  c1->SetGridx();
  c1->SetGridy();

  fHistCoeff->Draw("");
}

