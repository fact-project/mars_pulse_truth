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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Harald Kornmayer 1/2001
!   Author(s): Abelardo Moralejo 2/2003
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   Explanations on the rate calculation can be found in
//   chapter 7 of the following diploma thesis:
//   http://www.pd.infn.it/magic/tesi2.ps.gz (in Italian)
//
////////////////////////////////////////////////////////////////////////////

#include "MHMcRate.h" 

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHMcRate);

using namespace std;

void MHMcRate::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MMcTriggerRate";
    fTitle = title ? title : "Task to calc the collection area ";

    fPartId=0;               // Type of particle

    fEnergyMax=0.0;          // Maximum Energy (TeV)
    fEnergyMin=1000000.0;    // Minimum Energy (TeV)

    fSolidAngle = -1.;       // Solid angle within which incident directions
                             // are distributed
    fThetaMax=0.0;           // Maximum theta angle of run
    fThetaMin=370.0;         // Minimum theta angle of run
    fPhiMax=0.0;             // Maximum phi angle of run
    fPhiMin=370.0;           // Minimum phi angle of run

    fImpactMax=0.0;          // Maximum impact parameter
    fImpactMin=100000.0;     // Minimum impact parameter

    fBackTrig=-1.0;          // Number of triggers from background
    fBackSim=-1.0;           // Number of simulated showers for the background

    fTriggerRate= -1.0;      // Trigger rate in Hz
    fTriggerRateError= -1.0; // Estimated error for the trigger rate in Hz

    fMultiplicity = -1;      // Multiplicity of the trigger condition
    fMeanThreshold = -1.;    // Discr. threshold of the trigger condition
    fTriggerCondNum = 0;     // Trigger condition number within camera file

}

// --------------------------------------------------------------------------
//
//  default constructor
//  fills all member data with initial values
//
MHMcRate::MHMcRate(const char *name, const char *title)
{
    Init(name, title);

    fSpecIndex=0.0;         // dn/dE = k * e^{- fSpecIndex}
    fFlux0=-1.0;            // dn/dE = fFlux0 * E^{-a}

    fShowerRate= -1.0;      // Showers rate in Hz
    fShowerRateError=0.0;   // Estimated error of shower rate in Hz
}

// --------------------------------------------------------------------------
//
//  overloaded constructor I
//  fills all member data with initial values and sets the rate of
//  incident showers to ShowRate
//
MHMcRate::MHMcRate(Float_t showrate,
                   const char *name, const char *title)
{
    Init(name, title);

    fSpecIndex=0.0;                  // dn/dE = k * e^{- fSpecIndex}
    fFlux0=-1.0;                     // dn/dE = fFlux0 * E^{-a}

    fShowerRate= showrate;           // Showers rate in Hz
    fShowerRateError=sqrt(showrate); // Estimated error of shower rate in Hz
}

// --------------------------------------------------------------------------
//
//  overloaded constructor I
//  fills all member data with initial values and sets the
//  spectral index and the initial flux to SpecIndex and Flux0
//
MHMcRate::MHMcRate(Float_t specindex, Float_t flux0,
                   const char *name, const char *title)
{
    Init(name, title);

    fSpecIndex=specindex;   // dn/dE = k * e^{- fSpecIndex}
    fFlux0=flux0;           // dn/dE = fFlux0 * E^{-a}

    fShowerRate= -1.0;
    fShowerRateError=0.0;
}

// --------------------------------------------------------------------------
//
//  set the particle that produces the showers in the athmosphere
//
void MHMcRate:: SetParticle(UShort_t part)
{
    fPartId=part;
}

// --------------------------------------------------------------------------
//
//  Set the information about trigger due only to the Night Sky Background:
//
void MHMcRate::SetBackground (Float_t showers, Float_t triggers)
{
    fBackTrig=showers;      // Number of triggers from background
    fBackSim=triggers;      // Number of simulated showers for the background
}

// --------------------------------------------------------------------------
//
//  set the parameters to compute the incident rate 
//
void MHMcRate:: SetFlux(Float_t flux0, Float_t specindx)
{
    fFlux0=flux0;
    fSpecIndex=specindx;

}

// --------------------------------------------------------------------------
//
//  set the incident rate 
//
void MHMcRate:: SetIncidentRate(Float_t showerrate)
{
    fShowerRate=showerrate;
}

// --------------------------------------------------------------------------
//
//  update the limits for energy, theta, phi and impact parameter
//
void MHMcRate::UpdateBoundaries(Float_t energy, Float_t theta,
                                Float_t phi, Float_t impact)
{ 
    // It updates the limit values

    if (fThetaMax<theta) fThetaMax=theta;
    if (fThetaMin>theta) fThetaMin=theta;

    if (fPhiMax<phi) fPhiMax=phi;
    if (fPhiMin>phi) fPhiMin=phi;

    if (fImpactMax<impact) fImpactMax=impact;
    if (fImpactMin>impact) fImpactMin=impact;

    if (fEnergyMax<energy) fEnergyMax=energy;
    if (fEnergyMin>energy) fEnergyMin=energy;
} 

// --------------------------------------------------------------------------
//
//  compute the trigger rate and set the ReadyToSave bit
//
void MHMcRate::CalcRate(Float_t trig, Float_t anal, Float_t simu) 
{ 
    // It computes the trigger rate

    // First one computes the rate of incident showers.
    const Double_t specidx = 1.0-fSpecIndex;

    const Double_t epowmax = pow((double)fEnergyMax, specidx);
    const Double_t epowmin = pow((double)fEnergyMin, specidx);

    if (fShowerRate <= 0)
      fShowerRate = fFlux0/specidx*(epowmax-epowmin);

    if (fSolidAngle < 0.)
      fSolidAngle = (fPhiMax-fPhiMin)*(cos(fThetaMin)-cos(fThetaMax));

    if (fPartId!=1)
      fShowerRate *= fSolidAngle;

    fShowerRate *= TMath::Pi()*(fImpactMax/100.0*fImpactMax/100.0 - 
				fImpactMin/100.0*fImpactMin/100.0);

    fShowerRateError = sqrt(fShowerRate);

    // The simulated trigger time in the camera program is 160 ns:
    // 9/10/2002, AM: Fixed error below in calculation of "anal2"
    // ( added factor fShowerRate/simu )

    const Double_t anal2 = 1.0-fShowerRate*(anal/simu)*160.0e-9;
    const Double_t back2 = fBackSim*160.0e-9;

    // Then the trigger rate and its error is evaluated
    if(fBackTrig<0){
        fTriggerRateError = sqrt((trig*fShowerRate*fShowerRate/(simu*simu)) +
                                 (anal2*anal2*1/(fBackSim*back2*back2)));
        fBackTrig=0;
    }
    else
        fTriggerRateError = sqrt((trig*fShowerRate*fShowerRate/(simu*simu)) +
                                 (anal2*anal2*fBackTrig/(back2*back2)));

    fTriggerRate = trig*fShowerRate/simu + anal2*fBackTrig/back2;

    SetReadyToSave();
}

// --------------------------------------------------------------------------
//
//  print the trigger rate
//
void MHMcRate::Print(Option_t *) const
{
    *fLog << all << "Incident rate " << fShowerRate << " Hz " << endl;
    *fLog << "Multiplicity: " << fMultiplicity << ",  Discr. threshold: " << fMeanThreshold << endl;
    *fLog << "Trigger Rate " << fTriggerRate << " +- " << fTriggerRateError << " Hz" << endl;
}

// --------------------------------------------------------------------------
//
//  draw the trigger rate
//
void MHMcRate::Draw(Option_t *)
{
    *fLog << all << dbginf << " - MHMcRate::Draw: To be iplemented" << endl;
}

