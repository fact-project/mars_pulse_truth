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
!   Author(s): Abelardo Moralejo, 02/2005 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMcEvtBasic
//
// This class contains the most basic MonteCarlo information
// with which an event has been generated
//
// Note: The azimuth fTelescopePhi angle in this and other MC classes 
// follow the convention in the Corsika program (see Corsika manual and
// TDAS 02-11). There, phi is the azimuth of the momentum vector of 
// particles, and is measured from the north direction, anticlockwise 
// (i.e, west is phi=90 degrees). When it refers to the telescope 
// orientation, it is the azimuth of a vector along the telescope axis, 
// going from the camera to the mirror. So, fTelescopeTheta=90, 
// fTelescopePhi = 0 means the telescope is pointing horizontally towards 
// South. 
//
//
// Version 1: 
//  New container to keep the very basic informations on the
//  original MC events produced by Corsika
//
// Version 2:
//  - added typedef for ParticleId_t from MMcEvt
//  - replaced MMcEvt::ParticleId_t by ParticleId_t
//
// Version 3:
//  - moved fPhi   from MMcEvt
//  - moved fTheta from MmcEvt
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcEvtBasic.h"

#include "fits.h"

#include "MString.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MMcEvtBasic);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Calls Clear()
//
MMcEvtBasic::MMcEvtBasic()
{
    fName  = "MMcEvtBasic";
    fTitle = "Basic event info from Monte Carlo";

    Clear();
}

// --------------------------------------------------------------------------
//
// Constructor. Use this to set all data members
//
// THIS FUNCTION IS FOR THE SIMULATION OLNY.
// DON'T USE THIS MEMBERFUNCTION IN THE ANALYSIS.
//
MMcEvtBasic::MMcEvtBasic(ParticleId_t usPId, Float_t fEner,
                         Float_t fImpa, Float_t fTPhii, Float_t fTThet)
   : fPartId(usPId), fEnergy(fEner), fImpact(fImpa),
     fTelescopePhi(fTPhii), fTelescopeTheta(fTThet), fTheta(0), fPhi(0)
{
    fName  = "MMcEvtBasic";
    fTitle = "Basic event info from Monte Carlo";
}

// --------------------------------------------------------------------------
//
// Copy operator. Copy all data members
//
void MMcEvtBasic::operator=(const MMcEvtBasic &evt)
{
    fPartId         = evt.fPartId;
    fEnergy         = evt.fEnergy;
    fImpact         = evt.fImpact;
    fTelescopePhi   = evt.fTelescopePhi;
    fTelescopeTheta = evt.fTelescopeTheta;
    fPhi            = evt.fPhi;
    fTheta          = evt.fTheta;
}

// --------------------------------------------------------------------------
//
//  Reset all values.
//
void MMcEvtBasic::Clear(Option_t *opt)
{
    fPartId         = kUNDEFINED;

    fEnergy         = -1;
    fImpact         = -1;

    fTelescopePhi   = 0;
    fTelescopeTheta = 0;

    fTheta          = 0;
    fPhi            = 0;
}

// --------------------------------------------------------------------------
//
// Fill all data members
//
void MMcEvtBasic::Fill(ParticleId_t usPId, Float_t fEner,
		       Float_t fImpa, Float_t fTPhii, Float_t fTThet)
{
    fPartId         = usPId;

    fEnergy         = fEner;
    fImpact         = fImpa;

    fTelescopePhi   = fTPhii;
    fTelescopeTheta = fTThet;
}

TString MMcEvtBasic::GetParticleName(Int_t id)
{
    switch (id)
    {
    case kUNDEFINED:  return "Undefined";
    case kGAMMA:      return "Gamma";
    case kPOSITRON:   return "Positron";
    case kELECTRON:   return "Electron";
    case kANTIMUON:   return "Anti-Muon";
    case kMUON:       return "Muon";
    case kPI0:        return "Pi-0";
    case kNEUTRON:    return "Neutron";
    case kPROTON:     return "Proton";
    case kHELIUM:     return "Helium";
    case kOXYGEN:     return "Oxygen";
    case kIRON:       return "Iron";
    case kArtificial: return "Artificial";
    case kNightSky:   return "NightSky";
    }

    return MString::Format("Id:%d", id);
}

TString MMcEvtBasic::GetParticleSymbol(Int_t id)
{
    switch (id)
    {
    case kUNDEFINED:return "N/A";
    case kGAMMA:    return "\\gamma";
    case kPOSITRON: return "e^{+}";
    case kELECTRON: return "e^{-}";
    case kANTIMUON: return "\\mu^{+}";
    case kMUON:     return "\\mu^{-}";
    case kPI0:      return "\\pi^{0}";
    case kNEUTRON:  return "n";
    case kPROTON:   return "p";
    case kHELIUM:   return "He";
    case kOXYGEN:   return "O";
    case kIRON:     return "Fe";
    case kNightSky: return "\\gamma_{NSB}";
    }

    return MString::Format("Id:%d", id);
}

TString MMcEvtBasic::GetEnergyStr(Float_t e)
{
    if (e>=1000)
        return MString::Format("%.1fTeV", e/1000);

    if (e>=10)
        return MString::Format("%dGeV", (Int_t)(e+.5));

    if (e>=1)
        return MString::Format("%.1fGeV", e);

    return MString::Format("%dMeV", (Int_t)(e*1000+.5));
}


// --------------------------------------------------------------------------
//
// Print the contents of the container.
//
//  if you specify an option only the requested data members are printed:
//  allowed options are: id, energy, impact
//
void MMcEvtBasic::Print(Option_t *opt) const
{
    //
    //  print out the data member on screen
    //
    TString str(opt);
    if (str.IsNull())
    {
        *fLog << all << endl;
        *fLog << "Monte Carlo output:" << endl;
        *fLog << " Particle Id:    " << GetParticleName() << endl;
        *fLog << " Energy:         " << fEnergy << "GeV" << endl;
        *fLog << " Impactparam.:   " << fImpact/100 << "m" << endl;
        *fLog << endl;
        return;
    }
    if (str.Contains("id", TString::kIgnoreCase))
        *fLog << "Particle: " << GetParticleName() << endl;
    if (str.Contains("energy", TString::kIgnoreCase))
        *fLog << "Energy: " << fEnergy << "GeV" << endl;
    if (str.Contains("impact", TString::kIgnoreCase))
        *fLog << "Impact: " << fImpact << "cm" << endl;
}

Bool_t MMcEvtBasic::SetupFits(fits &fin)
{
    if (ClassName()==TString("MMcEvtBasic") && fName!="MMcEvtBasic")
    {
        *fLog << err << "ERROR - SetupFits only supported if name equals MMcEvtBasic." << endl;
        return kFALSE;
    }

    //if (!fin.SetRefAddress("MMcEvtBasic.fPartId",         fPartId        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fEnergy",         fEnergy        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fImpact",         fImpact        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fTelescopePhi",   fTelescopePhi  ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fTelescopeTheta", fTelescopeTheta))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fTheta",          fTheta         ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvtBasic.fPhi",            fPhi           ))  return kFALSE;

    return kTRUE;
}
