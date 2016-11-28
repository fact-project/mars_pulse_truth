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
// Computes the Effective areas and coefficients for unfolding for a given
// spectrum that can be parametrized by a function
//
//////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <math.h>

#include "MEffAreaAndCoeffCalc.h"

#include "TF1.h"
#include "MHillas.h"
#include "MMcEvt.hxx"
#include "TH2F.h"
#include "TFile.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MEffAreaAndCoeffCalc);

using namespace std;

const Int_t ntbins=1;                      // default number of theta bins
const Double_t tbin[ntbins+1] = {0,90};    // default theta bins bounds
const Int_t nebins = 10;                   // default number of energy bins
const Float_t emin = 10.;                  // default minimum energy value
const Float_t emax = 10000.;               // default maximum energy value
const Int_t nsubbins = 20;                 // default number of subbins per bin
const Char_t* deff = "4.e9*pow(x,-2.6+1)"; // default spectrum function

// -------------------------------------------------------------------------
//
// Constructor
//
MEffAreaAndCoeffCalc::MEffAreaAndCoeffCalc()
  : fSpec(NULL),  fEmin(emin), fEmax(emax), fEbins(nebins), fEsubbins(nsubbins), 
    fWeight(NULL), fCoeff(NULL), fEffA(NULL)//, fFile(NULL)
{
  // set the default function
  SetFunction(deff);  

  // create the TChains 
  fCini = new TChain("OriginalMC");
  fCcut = new TChain("Events");

  // define some useful aliases
  fCini->SetAlias("logenergy","log10(MMcEvtBasic.fEnergy)");
  fCini->SetAlias("theta","MMcEvtBasic.fTelescopeTheta*180./3.14159");

  fCcut->SetAlias("logenergy","log10(MMcEvt.fEnergy)");
  fCcut->SetAlias("theta","MMcEvt.fTelescopeTheta*180./3.14159");

  fCcut->SetBranchAddress("MHillas.",&fHillas);
  fCcut->SetBranchAddress("MMcEvt.",&fMcEvt);

  // initial value of the zenith angle binning
  SetThetaBinning(ntbins,tbin);
  
  // borra
  //  fFile = new TFile("coeftest.root","RECREATE");
}

// -------------------------------------------------------------------------
//
// Destructor
//
MEffAreaAndCoeffCalc::~MEffAreaAndCoeffCalc()
{
  if(fSpec)
    delete fSpec;

  if(fTbin)
    delete [] fTbin;

  if(fWeight)
    delete [] fWeight;

  if(fCoeff)
    delete fCoeff;

  if(fEffA)
    delete fEffA;


  delete fCini;
  delete fCcut;
}

// -------------------------------------------------------------------------
//
// Set the binning in zenith angle
//
void MEffAreaAndCoeffCalc::SetThetaBinning(Int_t n, const Double_t* binlist)
{
  fNTbins=n;
  if(fTbin) delete [] fTbin;
  fTbin = new Double_t[n+1];
  for(Int_t i=0;i<n+1;i++)
    fTbin[i] = binlist[i];
}

// -------------------------------------------------------------------------
//
// Set the function by expression and minimum and maximum energies
//
void MEffAreaAndCoeffCalc::SetFunction(const Char_t* chfunc, Float_t emin, Float_t emax)
{
  if(fSpec)
    delete fSpec;
  if(emin<=0 || emax<=0 || emax<emin)
    {
      emin = fEmin;
      emax = fEmax;
    }
  else
    {
      fEmin = emin;
      fEmax = emax;
    }
  fSpec = new TF1("fspec",chfunc,emin,emax);
}

// -------------------------------------------------------------------------
//
// Set the function by function pointer
//
void MEffAreaAndCoeffCalc::SetFunction(TF1* newf)
{
  if(fSpec)
    delete fSpec;
  fSpec = newf;
}

// -------------------------------------------------------------------------
//
// fill the histogram containing the original sample energy spectrum
//
void MEffAreaAndCoeffCalc::FillOriginalSpectrum()
{  
}

// -------------------------------------------------------------------------
//
// compute the weights for a particular input spectrum
//
void MEffAreaAndCoeffCalc::ComputeWeights()
{  
  // OJO!! maybe this should be hard-coded somewhere else
  const Float_t abslogmin=0.;
  const Float_t abslogmax=5.;

  const Float_t logemin = TMath::Log10(fEmin);
  const Float_t logemax = TMath::Log10(fEmax);
  const Float_t de    = (logemax-logemin)/fEbins; // bin size (in log)
  const Float_t desub = de/fEsubbins; // subbin size (in log)
  const Int_t esbins   = fEbins*fEsubbins; // total number of subbins

  // compute the binning for weights histogram
  const Int_t nmindist = (logemin>abslogmin)? Int_t((logemin-abslogmin)/desub) : 0;
  const Int_t nmaxdist = (logemax<abslogmax)? Int_t((abslogmax-logemax)/desub) : 0;

  fLogEWmin = logemin-desub*nmindist;
  fLogEWmax = logemax+desub*nmaxdist;
  fEWbins   = nmindist+esbins+nmaxdist;
  
  // reset the weights array
  if(fWeight)
    delete [] fWeight;
  fWeight = new Double_t[fEWbins];
    

  TH1F* horigs = new TH1F("horigs","Original energy spectrum",fEWbins,fLogEWmin,fLogEWmax);
  fCini->Draw("logenergy>>horigs","","goff");
  // borra
  //  horigs->Write();

  // borra
  //  TH1F hw("hw","hw",fEWbins,fLogEWmin,fLogEWmax);
  for(Int_t i=0;i<fEWbins;i++)
    {
      const Float_t denom = horigs->GetBinContent(i+1);               // number of events in initial spectrum
      const Float_t ew    = TMath::Power(10,fLogEWmin+(i+0.5)*desub); // real energy
      const Float_t numer = fSpec->Eval(ew);                          // number of events for the required spectrum
      if(denom>10)
	fWeight[i]=numer/denom;
      else
	{
	  //	  cout << "MEffAreaAndCoeffCalc::ComputeWeights Warning: no statistic to compute weight for energy " << ew << ", setting it to -1 "  << endl;
	  fWeight[i]=-1;
	}
      // borra
      //      hw.SetBinContent(i+1,fWeight[i]);
    }
  // borra
  //  hw.Write();

  delete horigs;
}

// --------------------------------------------------------------
//
// compute the coefficients used for the (iterative) unfolding 
//
void MEffAreaAndCoeffCalc::ComputeCoefficients()
{ 
  if(!fWeight)
    {
      cout << "MEffAreaAndCoeffCalc::ComputeCoefficients Warning: No weights computed! nothing done" << endl;
      return;
    }

  const Float_t logemin = TMath::Log10(fEmin);
  const Float_t logemax = TMath::Log10(fEmax);
  const Float_t de = (logemax-logemin)/fEbins; // bin size (in log)
  const Float_t desub = de/fEsubbins; // subbin size (in log)
  const Int_t nentries = Int_t(fCcut->GetEntries());
 
  // declare needed histos
  TH2F* hest = new TH2F("hest","Estimated energy",fEbins,logemin,logemax,fNTbins,fTbin);
  TH2F* hmc  = new TH2F("hmc","MC energy",fEbins,logemin,logemax,fNTbins,fTbin);

  // borra
  //  TH1F* hest1  = new TH1F("hest1","Estimated energy",fEbins,logemin,logemax);
  //  TH1F* hmc1   = new TH1F("hmc1","MC energy",fEbins,logemin,logemax);
  //  TH1F* coef1  = new TH1F("coef1","coefficients",fEbins,logemin,logemax);

  // reset the coefficients
  if(fCoeff)
    delete fCoeff;
  fCoeff = new TH2F("fcoeff","Coefficients for unfolding",fEbins,logemin,logemax,fNTbins,fTbin);

  // fill the histograms of estimated and measured energy for weighted events
  for(Int_t i=0;i<nentries;i++)
    {
      fCcut->GetEntry(i);  

      // mc and estimated energy
      // OJO!! Estimated energy will be taken directly from some input container
      Float_t emc   = fMcEvt->GetEnergy();
      Float_t estim = fHillas->GetSize()/0.18/15.;

      if((emc<fEmin && estim<fEmin) || (emc>fEmax && estim>fEmax) ||
	 (emc<fEmin && estim>fEmax) || (emc>fEmax && estim<fEmin))
	continue;

      Float_t theta = fMcEvt->GetTheta()*180./3.14159; // zenith angle (in degrees)

      // compute the bin where the weight is taken from
      Int_t wbin = Int_t((TMath::Log10(emc)-fLogEWmin)/desub);

      // fill the histograms
      if(wbin<fEWbins)
	{
	  if(fWeight[wbin]>0)
	    {
	      hest->Fill(TMath::Log10(estim),theta,fWeight[wbin]); 
	      hmc->Fill(TMath::Log10(emc),theta,fWeight[wbin]);
	      // borra
// 	      if(theta<fTbin[1])
// 		{
// 		  hest1->Fill(TMath::Log10(estim),fWeight[wbin]); 
// 		  hmc1->Fill(TMath::Log10(emc),fWeight[wbin]);
// 		}
	    }
	  else
	    cout << "MEffAreaAndCoeffCalc::ComputeCoefficients Warning: event " << i << " with energy " << emc << " has no computed weight (lack of MC statistics), skipping" << endl;
	}
      else
      	cout << "MEffAreaAndCoeffCalc::ComputeCoefficients Warning: event " << i << " with energy " << emc << " has energy out of bounds, skipping" << endl;
    }

  // divide the previous histos to get the coefficients for unfolding
  for(Int_t j=0;j<fNTbins;j++)
    for(Int_t i=0;i<fEbins;i++)
      {
	const Float_t num = hmc->GetBinContent(i+1,j+1);
	const Float_t den = hest->GetBinContent(i+1,j+1);
	//borra
// 	const Float_t num1 = hmc1->GetBinContent(i+1);
// 	const Float_t den1 = hest1->GetBinContent(i+1);
	if(den)
	  {
	    fCoeff->SetBinContent(i+1,j+1,num/den);
	    //borra
// 	    if(j==0 && den1)
// 	      coef1->SetBinContent(i+1,num1/den1);
	  }
	else
	  {
	    cout << "MEffAreaAndCoeffCalc::ComputeCoefficients Warning: energy bin " << i << ", thetabin " << j << " has no survivors, setting coefficient to 0" << endl;
	    fCoeff->SetBinContent(i+1,j+1,0.);
	  }
      }


  //borra
//   hmc1->Write();
//   hest1->Write();
//   coef1->Write();

  delete hmc;
  delete hest;
}

// --------------------------------------------------------------
//
// compute the coefficients used for the (iterative) unfolding 
//
void MEffAreaAndCoeffCalc::ComputeEffectiveAreas()
{
  if(!fWeight)
    {
      cout << "MEffAreaAndCoeffCalc::ComputeEffectiveAreas Warning: No weights computed! nothing done" << endl;
      return;
    }

  //OJO!! radius should be read from somewhere!
  const Float_t radius = 30000.; // generation radius (in cm)
  const Float_t Atot = 3.14159*radius*radius; //total area (in cm^2)
  const Float_t logemin = TMath::Log10(fEmin);
  const Float_t logemax = TMath::Log10(fEmax);
  const Int_t esbins = fEbins*fEsubbins; // total number of subbins
  const Float_t de = (logemax-logemin)/fEbins; // bin size (in log)
  const Float_t desub = de/fEsubbins; // subbin size (in log)

  // reset the effective areas
  if(fEffA)
    delete fEffA;
  fEffA = new TH2F("feffa","Effective area",fEbins,logemin,logemax,fNTbins,fTbin);
  //borra
//   TH1F eff("eff","Effective area",fEbins,logemin,logemax);
  
  // fill the spectrum of the survivors
  TH2F* hpass= new TH2F("hpass","Survivors",esbins,logemin,logemax,fNTbins,fTbin);
  TH2F* horig= new TH2F("horig","Original events",esbins,logemin,logemax,fNTbins,fTbin);

  fCcut->Draw("theta:logenergy>>hpass","","goff");
  fCini->Draw("theta:logenergy>>horig","","goff");
  
  // compute the effective areas
  for(Int_t j=0;j<fNTbins;j++)
    for(Int_t i=0;i<fEbins;i++)
      {
	Float_t effarea =0;
	Float_t wtot = 0;
	for(Int_t k=0;k<fEsubbins;k++)
	  {
	    Float_t numerator = hpass->GetBinContent(i*fEsubbins+k+1,j+1);
	    Float_t denominator = horig->GetBinContent(i*fEsubbins+k+1,j+1);
	    
	    if(denominator<=0)
	      cout << "MEffAreaAndCoeffCalc::ComputeEffectiveAreas Warning: energy subbin " << i*fEsubbins+k <<", theta bin " << j << " contains no events" << endl;
	    else
	      {
		const Float_t ew = TMath::Power(10,logemin+(i*fEsubbins+k+0.5)*desub);
		const Float_t ww = fSpec->Eval(ew);
		effarea+=Atot*numerator/denominator*ww;
		wtot   += ww;
	      }
	  }
	if(!wtot)
	  {
	    cout << "MEffAreaAndCoeffCalc::ComputeEffectiveAreas Warning: energy bin " << i <<", theta bin " << j << " contains no events setting effective area to 0" << endl;
	    fEffA->SetBinContent(i+1,j+1,0);
	  }	    
	else
	  {
	    fEffA->SetBinContent(i+1,j+1,effarea/wtot);	
	    // borra
// 	    if(j==0)
// 	      {
// 		//		cout << "*****" << i << ": " << effarea/wtot << endl;
// 		eff.SetBinContent(i+1,effarea/wtot);
// 	      }
	  }
      }

  // borra
//   eff.Write();
  
  delete hpass;
  delete horig;
}

// --------------------------------------------------------------
//
// Call the internal functions to compute all the factors
//
void MEffAreaAndCoeffCalc::ComputeAllFactors()
{
  ComputeWeights();
  ComputeCoefficients();
  ComputeEffectiveAreas();
}
