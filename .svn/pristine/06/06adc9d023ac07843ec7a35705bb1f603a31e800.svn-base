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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer  1/2001
!   Author(s): Abelardo Moralejo 2/2005 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MMcCollectionAreaCalc
//
//////////////////////////////////////////////////////////////////////////////

#include "MMcCollectionAreaCalc.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"
#include "MMcEvtBasic.h"

#include "MMcRunHeader.hxx"
#include "MMcCorsikaRunHeader.h"

#include "MHMcCollectionArea.h"

ClassImp(MMcCollectionAreaCalc);

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
MMcCollectionAreaCalc::MMcCollectionAreaCalc(const char *name, const char *title): 
  fBinsTheta(0), fBinsEnergy(0), fSpectrum(0)
{
    fName  = name  ? name  : "MMcCollectionAreaCalc";
    fTitle = title ? title : "Task to calculate the collection area";
} 

////////////////////////////////////////////////////////////////////////////////
//
// PreProcess. 
// Create MHMcCollectionArea object if necessary. 
// Search in parameter list for MBinning objects with binning in theta and E.
// These contain the coarse binning to be used in the analysis. Then search 
// for other necessary input containers: 
// if MMcEvt is found, it means we are in the loop over the Events tree, 
// and so we must fill the histogram MHMcCollectionArea::fHistSel (using 
// MHMcCollectionArea::FillSel). If MMcEvt is not found, it means that we are in 
// the loop over the "OriginalMC" tree, containing all the original Corsika events 
// produced, and hence we must fill the histogram  fHistAll through 
// MHMcCollectionArea::FillAll.
//
Int_t MMcCollectionAreaCalc::PreProcess (MParList *pList)
{
  fCollArea = (MHMcCollectionArea*)pList->FindCreateObj("MHMcCollectionArea");

  // Look for the binnings of the histograms if they have not been already
  // found in a previous loop.

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

  fCollArea->SetCoarseBinnings(*fBinsEnergy, *fBinsTheta);


  // Look for the input containers

  fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
  if (fMcEvt)
    {
        *fLog << inf << "MMcEvt found... I will fill MHMcCollectionArea.fHistSel..." << endl;
	return kTRUE;
    }

  *fLog << inf << "MMcEvt not found... looking for MMcEvtBasic..." << endl;


  fMcEvtBasic = (MMcEvtBasic*) pList->FindObject("MMcEvtBasic");
    
  if (fMcEvtBasic)
    {
      *fLog << inf << "MMcEvtBasic found... I will fill MHMcCollectionArea.fHistAll..." << endl;
      return kTRUE;
    }

  *fLog << err << "MMcEvtBasic not found. Aborting..." << endl;

  return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// Process. Depending on whether fMcEvt or fMcEvtBasic are available, fill 
// MHMcCollectionArea::fHistAll, else, if fMcEvt is available, fill 
// MHMcCollectionArea::fHistSel
//
Int_t MMcCollectionAreaCalc::Process()
{
    Double_t energy = fMcEvt? fMcEvt->GetEnergy() : fMcEvtBasic->GetEnergy();
    Double_t impact = fMcEvt? fMcEvt->GetImpact()/100. : fMcEvtBasic->GetImpact()/100.; // in m
    Double_t theta  = fMcEvt? fMcEvt->GetTelescopeTheta()*TMath::RadToDeg() : 
      fMcEvtBasic->GetTelescopeTheta()*TMath::RadToDeg(); // in deg

    if (fMcEvt)
      fCollArea->FillSel(energy, impact, theta);
    else
      fCollArea->FillAll(energy, impact, theta);

    return kTRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Postprocess. If both fHistAll and fHistSel are already filled, calculate
// effective areas. Else it means we still have to run one more loop.
//
Int_t MMcCollectionAreaCalc::PostProcess()
{
  if ( ((TH2D*)fCollArea->GetHistAll())->GetEntries() > 0 &&
       ((TH2D*)fCollArea->GetHistSel())->GetEntries() > 0)
    {
      *fLog << inf << "Calculation Collection Area..." << endl;
      fCollArea->Calc(fSpectrum);
    }

    return kTRUE;
}
