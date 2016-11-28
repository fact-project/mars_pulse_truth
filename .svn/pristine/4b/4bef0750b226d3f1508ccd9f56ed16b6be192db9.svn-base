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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// Modified 4/7/2002 Abelardo Moralejo: now the dimension of fTrigger is
//  set dinamically, to allow an arbitrary large number of trigger
//  conditions to be processed.
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcTriggerRateCalc.h"

#include <math.h>

#include <TCanvas.h>
#include <TGraphErrors.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcEvt.hxx"
#include "MMcTrig.hxx"
#include "MMcRunHeader.hxx"
#include "MMcTrigHeader.hxx"
#include "MMcCorsikaRunHeader.h"

#include "MH.h"
#include "MHMcRate.h"

ClassImp(MMcTriggerRateCalc);

using namespace std;

void MMcTriggerRateCalc::Init(int dim, float *trigbg, float simbg,
                              const char *name, const char *title)
{
    fName  = name  ? name  : "MMcTriggerRateCalc";
    fTitle = title ? title : "Task to calc the trigger rate ";

    fMcTrig = NULL;
    fMcRate = NULL;

    fTrigNSB = trigbg;
    fSimNSB = simbg;

    fPartId = -1; 

    fShowers = 0;
    fAnalShow = 0;

    fFirst = dim>0 ?   1 : -dim;
    fLast  = dim>0 ? dim : -dim;

    fNum = fLast-fFirst+1;
    fTrigger = new float[fNum];

    for (UInt_t i=0;i<fNum;i++)
      fTrigger[i]=0;

    AddToBranchList("MMcEvt.fPartId");
    AddToBranchList("MMcEvt.fImpact");
    AddToBranchList("MMcEvt.fEnergy");
    AddToBranchList("MMcEvt.fPhi");
    AddToBranchList("MMcEvt.fTheta");
    AddToBranchList("MMcEvt.fPhotElfromShower");
    AddToBranchList("MMcTrig", "fNumFirstLevel", fFirst, fLast);

}

// ReInit: read run header to get some info later:

Bool_t MMcTriggerRateCalc::ReInit(MParList *pList)
{
    fMcRunHeader = (MMcRunHeader*) pList->FindObject("MMcRunHeader");
    if (!fMcRunHeader)
    {
        *fLog << err << dbginf << "Error - MMcRunHeader not found... exit." << endl;
        return kFALSE;
    }

    fMcCorRunHeader = (MMcCorsikaRunHeader*) pList->FindObject("MMcCorsikaRunHeader");
    if (!fMcCorRunHeader)
    {
        *fLog << err << dbginf << "Error - MMcCorsikaRunHeader not found... exit." << endl;
        return kFALSE;
    }

    fShowers += fMcRunHeader->GetNumSimulatedShowers();

    if (fMcRunHeader->GetAllEvtsTriggered())
      {
          *fLog << warn;
          *fLog << "WARNING - the input data file contains only the" << endl;
          *fLog << "events that triggered. I will assume the standard value" << endl;
          *fLog << "for maximum impact parameter (400 m)" <<endl;


          if (fTrigNSB[0] > 0)
          {
              *fLog << warn;
              *fLog << "WARNING - NSB rate can be overestimated by up to 5%." << endl;
              *fLog << "For a precise estimate of the total rate including NSB" << endl;
              *fLog << "accidental triggers I need a file containing all event headers." << endl;
          }
          else
          {
            *fLog << warn << "WARNING - calculating only shower rate (no NSB accidental triggers)" << endl;
          }
      }

    *fLog << endl << warn <<
      "WARNING: I will assume the standard maximum off axis angle" << endl <<
      "(5 degrees) for the original showers" << endl;


    for (UInt_t i=0; i<fNum; i++)
      {
	if (fMcRunHeader->GetAllEvtsTriggered())
	  {
	    GetRate(i)->SetImpactMin(0.);
	    GetRate(i)->SetImpactMax(40000.);   // in cm
	  }
	GetRate(i)->SetSolidAngle(2.390941702e-2);  // sr

	//
	// Set limits of energy range, coverting from GeV to TeV:
	//
	GetRate(i)->SetEnergyMin(fMcCorRunHeader->GetELowLim()/1000.);
	GetRate(i)->SetEnergyMax(fMcCorRunHeader->GetEUppLim()/1000.);

        TString th("MMcTrigHeader");
	if (GetRate(i)->GetTriggerCondNum() > 0)
	  {
	    th += ";";
	    th += GetRate(i)->GetTriggerCondNum();
	  }

	MMcTrigHeader* trighead = (MMcTrigHeader*) pList->FindObject(th);
	GetRate(i)->SetMeanThreshold(trighead->GetMeanThreshold());
	GetRate(i)->SetMultiplicity(trighead->GetMultiplicity());

      }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  overloaded constructor I
//
//      dim:     fDimension
//      *trigbg: number of NSB triggers for
//               a given trigger condition.
//      simbg:   Number of simulated events for the NSB
//      rate:    rate of incident showers
//
MMcTriggerRateCalc::MMcTriggerRateCalc(float rate, int dim,
                                       float *trigbg, float simbg,
                                       const char *name, const char *title)
{
    Init(dim, trigbg, simbg, name, title);
}


// --------------------------------------------------------------------------
//
//  overloaded constructor II
//
//      dim:     fDimension
//      *trigbg: number of NSB triggers for
//               a given trigger condition.
//      simbg:   Number of simulated events for the NSB
//
MMcTriggerRateCalc::MMcTriggerRateCalc(int dim, float *trigbg,float simbg,
                                       const char *name, const char *title)
{
    Init(dim, trigbg, simbg, name, title);
}

MMcTriggerRateCalc::~MMcTriggerRateCalc()
{
    if (fMcTrig)
        delete fMcTrig;

    if (fMcRate)
        delete fMcRate;
}


// --------------------------------------------------------------------------
//
//  The PreProcess connects the raw data with this task. It checks if the 
//  input containers exist, if not a kFalse flag is returned. It also checks
//  if the output contaniers exist, if not they are created.
//  This task can read either Montecarlo files with multiple trigger
//  options, either Montecarlo files with a single trigger option.
//
Int_t MMcTriggerRateCalc::PreProcess (MParList *pList)
{
    // connect the raw data with this task

    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    UInt_t num;

    fMcTrig = new TObjArray(pList->FindObjectList("MMcTrig", fFirst, fLast));
    num = fMcTrig->GetEntriesFast();
    if (num != fNum)
    {
        *fLog << err << dbginf << fNum << " MMcTrig objects requested, ";
        *fLog << num << " are available... aborting." << endl;
        return kFALSE;
    }

    fMcRate = new TObjArray(pList->FindObjectList("MHMcRate", fFirst, fLast));
    num = fMcRate->GetEntriesFast();
    if (num != fNum)
    {
        *fLog << err << dbginf << fNum << " MHMcRate objects requested, ";
        *fLog << num << " are available... aborting." << endl;
        return kFALSE;
    }

    for (UInt_t i=0;i<fNum;i++)
      {
        MHMcRate &rate = *GetRate(i);

	if (fTrigNSB)
	  rate.SetBackground(fTrigNSB[i], fSimNSB);
	else
	  rate.SetBackground(0., fSimNSB);
      }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  The Process-function counts the number of simulated showers, the
//  number of analised showers and the number of triggers. It also updates
//  the limits for theta, phi, energy and impact parameter in the
//  MHMcRate container.
//
Int_t MMcTriggerRateCalc::Process()
{
    //
    //  Counting analysed showers (except in the case in which the file 
    //  contains only events that triggered, since then we do not know
    //  how many showers were analysed).
    //

    if ( ! fMcRunHeader->GetAllEvtsTriggered() &&
	 fMcEvt->GetPhotElfromShower() )
      fAnalShow++;

    //
    // Set PartId and check it is the same for all events
    //
    if (fPartId < 0)
      fPartId = fMcEvt->GetPartId();
    else if (fPartId != fMcEvt->GetPartId())
      {
	*fLog << err << dbginf << "Incident particle type seems to change";
	*fLog << "from " << fPartId << " to " << fMcEvt->GetPartId() << endl;
	*fLog << "in input data files... aborting." << endl;
	return kFALSE;
      }

    //
    //  Getting angles, energy and impact parameter to set boundaries
    //
    const Float_t theta = fMcEvt->GetTheta();
    const Float_t phi   = fMcEvt->GetPhi();
    const Float_t param = fMcEvt->GetImpact();
    const Float_t ener  = fMcEvt->GetEnergy()/1000.0;

    //
    //  Counting number of triggers
    //
    for (UInt_t i=0; i<fNum; i++)
    {
	if (GetTrig(i)->GetFirstLevel())
	    fTrigger[i] ++; 

	if ( ! fMcRunHeader->GetAllEvtsTriggered())
	  GetRate(i)->UpdateBoundaries(ener, theta, phi, param);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  The PostProcess-function calculates and shows the trigger rate
//
Int_t MMcTriggerRateCalc::PostProcess()
{
    for (UInt_t i=0; i<fNum; i++)
    {
        MHMcRate &rate = *GetRate(i);

        rate.SetParticle(fPartId);
        switch (fPartId)
        {
        case MMcEvt::kPROTON:
	  if ((Int_t)floor(-100*fMcCorRunHeader->GetSlopeSpec()+0.5) != 275)
	    {
                *fLog << err << "Spectrum slope as read from input file (";
                *fLog << fMcCorRunHeader->GetSlopeSpec() << ") does not match the expected ";
                *fLog << "one (-2.75) for protons" << endl << "... aborting." << endl;
                return kFALSE;
            }
	  rate.SetFlux(0.1091, 2.75);
	  break;
        case MMcEvt::kHELIUM:
	  if ((Int_t)floor(-100*fMcCorRunHeader->GetSlopeSpec()+0.5) != 262)
	    {
                *fLog << err << "Spectrum slope as read from input file (";
                *fLog << fMcCorRunHeader->GetSlopeSpec() << ") does not match the expected ";
                *fLog << "one (-2.62) for Helium" << endl << "... aborting." << endl;
                return kFALSE;
	    }
	  rate.SetFlux(0.0660, 2.62);
	  break;
        default:
	  *fLog << err << "Unknown incident flux parameters for ";
	  *fLog << fPartId<< " particle Id ... aborting." << endl;
	  return kFALSE;
	}
    }

    //
    // Computing trigger rate
    //
    for (UInt_t i=0; i<fNum; i++)
        GetRate(i)->CalcRate(fTrigger[i], fAnalShow, fShowers);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draw rate as a funtion of discriminator threshold.
//
void MMcTriggerRateCalc::Draw(Option_t *)
{
    /*
     Commented out, because this is creating a memory leak!
     The histograms are neither deleted anywhere, nor it is made
     sure, that the histograms are not overwritten.
     Also the comment for the function doesn't match the rules.
     BTW: please replace all arrays by Root lists (TArray*, TList, etc)
  TCanvas *c = MH::MakeDefCanvas("Rate");

  Float_t xmin = GetRate(0)->GetMeanThreshold()-0.55;
  Float_t xmax = GetRate(fNum-1)->GetMeanThreshold()+0.55;
  Int_t  nbins = (Int_t)((xmax-xmin)*10);

  fHist[1] = new TH1F("Rate2","Trigger rate, mult. 2", nbins, xmin, xmax);
  fHist[2] = new TH1F("Rate3","Trigger rate, mult. 3", nbins, xmin, xmax);
  fHist[3] = new TH1F("Rate4","Trigger rate, mult. 4", nbins, xmin, xmax);
  fHist[4] = new TH1F("Rate5","Trigger rate, mult. 5", nbins, xmin, xmax);

  for (UInt_t i=0; i<fNum; i++)
    {
      Short_t mult = GetRate(i)->GetMultiplicity();

      fHist[mult-1]->SetBinContent(fHist[mult-1]->FindBin(GetRate(i)->GetMeanThreshold()), GetRate(i)->GetTriggerRate());

      fHist[mult-1]->SetBinError(fHist[mult-1]->FindBin(GetRate(i)->GetMeanThreshold()), GetRate(i)->GetTriggerRateError());
    }

  for (Int_t i = 1; i <=4; i++)
    {
      fHist[i]->SetLineWidth(2);
      fHist[i]->SetMarkerStyle(20);
      fHist[i]->SetMarkerSize(.5);
    }

  c->DrawFrame (xmin, 0.5*GetRate(fNum-1)->GetTriggerRate(), xmax, 1.2*GetRate(0)->GetTriggerRate(), "Trigger rate for multiplicity = 3, 4, 5");

  c->SetLogy();
  c->SetGridy();
  c->SetGridx();

  fHist[2]->SetLineColor(1);
  fHist[2]->SetMarkerColor(1);
  fHist[2]->SetMinimum(0.5*GetRate(fNum-1)->GetTriggerRate());
  fHist[2]->GetXaxis()->SetTitle("Discr. threshold (mV)");
  fHist[2]->GetYaxis()->SetTitle("Trigger rate (Hz)");
  fHist[2]->GetYaxis()->SetTitleOffset(1.2);
  fHist[2]->Draw("axis");
  fHist[2]->Draw("same");

  fHist[3]->SetLineColor(3);
  fHist[3]->SetMarkerColor(3);
  fHist[3]->Draw("same");

  fHist[4]->SetLineColor(4);
  fHist[4]->Draw("same");
  fHist[4]->SetMarkerColor(4);
     */
}

