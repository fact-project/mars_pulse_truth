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
!   Author(s): Javier Rico 2/2005 <mailto:jrico@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MMcUnfoldCoeffCalc
//  Task to compute the coefficients for energy unfolding (to be used 
//  typically in an iterative process).
//
//  Input containers:
//   MMcEvt
//   MMcEvtBasic
//   MEnergyEst
//   binsTheta  [MBinning]
//   binsEnergy [MBinning]
//
//  Output containers:
//   MHMcUnfoldCoeff
//
//  The binning for energy and theta are looked in the parameter list
//  The tentative spectrum MUST be provided using SetSpectrum, otherwise
//  it is used for default pow(energy,-1.6) [~Crab at 1 TeV]
//
//  This task is supposed to be in a two-looped macro/program: 
//  In the first one MMcEvtBasic is read in order to compute the 
//  weights to convert from original to tentative spectrum.
//  In the second one MMcEvt and MEnergyEst are read and fill the 
//  MC and estimated energy that are divided in the postprocess to
//  compute the coefficients
//
//////////////////////////////////////////////////////////////////////////////

#include "TF1.h"

#include "MMcUnfoldCoeffCalc.h"
#include "MHMcUnfoldCoeff.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"
#include "MMcEvtBasic.h"
#include "MEnergyEst.h"

ClassImp(MMcUnfoldCoeffCalc);

using namespace std;

// -------------------------------------------------------------------------
//
//  Constructor
//
MMcUnfoldCoeffCalc::MMcUnfoldCoeffCalc(const char* name, const char* title) :
  fSpectrum(NULL)
{
  fName  = name  ? name  : "MMcUnfoldCoeffCalc";
  fTitle = title ? title : "Task to calculate the unfolding coefficients";
} 

// -------------------------------------------------------------------------
//
// PreProcess. 
// Create MHMcUnfoldCoeff object if necessary. Search for other necessary 
// containers: if MMcEvt is found, it means we are in the loop over the Events 
// tree, and so we must fill the histogram MHMcUnfoldCoeff::fHistMcE and 
// MHMcUnfoldCoeff::fHistEstE (using MHMcUnfoldCoeff::FillSel()). 
// If MMcEvt is not found, it means that we are in the loop over the 
// "OriginalMC" tree, containing all the original Corsika events 
// produced, and hence we must fill the histogram  fHistAll through 
// MHMcUnfoldCoeff::FillAll()
//
Int_t MMcUnfoldCoeffCalc::PreProcess (MParList *pList)
{
  fUnfoldCoeff = (MHMcUnfoldCoeff*)pList->FindCreateObj("MHMcUnfoldCoeff");
  
  if (!fBinsTheta)
    {
      fBinsTheta = (MBinning*) pList->FindObject("binsTheta");
      if (!fBinsTheta)
	{
	  *fLog << err << "Coarse Theta binning not found... Aborting." 
		<< endl;
	  return kFALSE;
	}
    }

  if (!fBinsEnergy)
    {
      fBinsEnergy = (MBinning*) pList->FindObject("binsEnergy");
      if (!fBinsEnergy)
	{
	  *fLog << err << "Coarse Energy binning not found... Aborting." 
		<< endl;
	  return kFALSE;
	}
    }
  
  fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
  if (fMcEvt)
    {      
      *fLog << inf << "MMcEvt found... Filling MHMcUnfoldCoeff.fHistSel..." << endl;

      fEnergyEst =  (MEnergyEst*)pList->FindObject("MEnergyEst");
      if(!fEnergyEst)
	{
	  *fLog << err << "MEnergyEst not found... aborting..." << endl;
	  return kFALSE;
	}

      return kTRUE;
    }

  *fLog << inf << "MMcEvt not found... looking for MMcEvtBasic..." << endl;

  fMcEvtBasic = (MMcEvtBasic*) pList->FindObject("MMcEvtBasic");
    
  if (fMcEvtBasic)
    {
      fUnfoldCoeff->SetCoarseBinnings(*fBinsEnergy,*fBinsTheta);

      *fLog << inf << "MMcEvtBasic found... Filling MHMcUnfoldCoeff.fHistAll..." << endl;
      return kTRUE;
    }

  *fLog << err << "MMcEvtBasic not found. Aborting..." << endl;

  return kFALSE;
}

// -------------------------------------------------------------------------
//
// Depending on whether fMcEvt or fMcEvtBasic are available, fill 
// MHMcUnfoldCoeff::fHistAll, else, if fMcEvt is available, fill 
// MHMcUnfoldCoeff::fHistMcE and MHMcUnfoldCoeff::fHistEstE
//
Int_t MMcUnfoldCoeffCalc::Process()
{
  Double_t mcenergy  = fMcEvt ? fMcEvt->GetEnergy() : fMcEvtBasic->GetEnergy();
  Double_t estenergy = fEnergyEst ? fEnergyEst->GetEnergy() : 0;
  Double_t theta  = fMcEvt? fMcEvt->GetTelescopeTheta()*TMath::RadToDeg() : 0; // in deg
  
  if (fMcEvt)
    fUnfoldCoeff->FillSel(mcenergy, estenergy, theta);
  else
    fUnfoldCoeff->FillAll(mcenergy);
  
  return kTRUE;
}

// -------------------------------------------------------------------------
//
// Postprocess. 
// If both fHistMcE and fHistEstE are already filled, calculate
// the coefficients. 
// Else it means we still have to run one more loop, and then the
// weights are computed
//
Int_t MMcUnfoldCoeffCalc::PostProcess()
{
  if(((TH2D*)fUnfoldCoeff->GetHistMcE())->GetEntries() > 0  &&
     ((TH2D*)fUnfoldCoeff->GetHistEstE())->GetEntries() > 0)
    {
      fUnfoldCoeff->ComputeCoefficients();
      return kTRUE;
    }
 
  if(((TH1D*)fUnfoldCoeff->GetHistAll())->GetEntries() <= 0)
    {
      *fLog << err << "Postprocess reached with no histogram computed. Aborting" << endl;
      return kFALSE;
    }
  

  if(fSpectrum)
    fUnfoldCoeff->ComputeWeights(fSpectrum);
  else
    {
      // default spectrum in case no other one is provided
      TF1 spectrum("func","pow(x,-1.6)",2.,20000.);
      fUnfoldCoeff->ComputeWeights(&spectrum);
    }
  
  return kTRUE;
}
