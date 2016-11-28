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
!   Author(s):
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcEvt
//
// This class handles and contains the MonteCarlo information
// with which the events have been generated
// This information exists for each event.
//
// Note: All the azimuth Phi angles in this and other MC classes follow
// the convention in the Corsika program (see Corsika manual). There, phi
// is the azimuth of the momentum vector of particles, and is measured
// from the north direction, anticlockwise (i.e, west is phi=90 degrees).
// When it refers to the telescope orientation, it is the azimuth of a 
// vector along the telescope axis, going from the camera to the mirror. 
// So, fTelescopeTheta=90, fTelescopePhi = 0 means the telescope is 
// pointing horizontally towards South. For an explanation, see also 
// TDAS 02-11.
//
// Version 4: 
//   - Added member fFadcTimeJitter
//
// Version 5:
//   - removed fPartId, fEnergy, fImpact, fTelescopeTheta, fTelescopePhi
//   - derives now from MMcEvtBasic which contains all these values
//   - moved ParticleId_t to base class MMcEvtBasic
//
// Version 6:
//   - added fEventReuse
//
// Version 7:
//  - moved fPhi   to MMcEvtBasic
//  - moved fTheta to MmcEvtBasic
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcEvt.hxx"

#include "fits.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

ClassImp(MMcEvt);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Calls Clear()
//
MMcEvt::MMcEvt()
{
    fName  = "MMcEvt";
    fTitle = "Event info from Monte Carlo";

    Clear();
}

// --------------------------------------------------------------------------
//
// Constructor. Use this to set all data members
//
// THIS FUNCTION IS FOR THE SIMULATION OLNY.
// DON'T USE THIS MEMBERFUNCTION IN THE ANALYSIS.
//
MMcEvt::MMcEvt(UInt_t  fEvtNum,    ParticleId_t usPId, Float_t  fEner,
               Float_t  fThi0,     Float_t  fFirTar,   Float_t  fzFirInt,
               Float_t  fThet,     Float_t  fPhii,     Float_t  fCorD,
               Float_t  fCorX,     Float_t  fCorY,     Float_t  fImpa,
               Float_t  fTPhii,    Float_t  fTThet,    Float_t  fTFirst,
               Float_t  fTLast,    Float_t  fL_Nmax,   Float_t  fL_t0,
               Float_t  fL_tmax,   Float_t  fL_a,      Float_t  fL_b,
               Float_t  fL_c,      Float_t  fL_chi2,   UInt_t   uiPin,
               UInt_t   uiPat,     UInt_t   uiPre,     UInt_t   uiPco,
               UInt_t   uiPelS,    UInt_t   uiPelC,    Float_t  elec,
               Float_t  muon,      Float_t  other,     Float_t  fadc_jitter,
               Int_t    reuse)
{
    fName  = "MMcEvt";
    fTitle = "Event info from Monte Carlo";

    Fill(fEvtNum, usPId, fEner, fThi0, fFirTar, fzFirInt, fThet,
	 fPhii, fCorD, fCorX, fCorY, fImpa, fTPhii, fTThet, fTFirst,
	 fTLast, fL_Nmax, fL_t0, fL_tmax, fL_a, fL_b, fL_c, fL_chi2,
	 uiPin, uiPat, uiPre, uiPco, uiPelS, uiPelC, elec, muon, other,
	 fadc_jitter, reuse);
}

// --------------------------------------------------------------------------
//
//  reset all values to values as nonsense as possible
//
void MMcEvt::Clear(Option_t *opt)
{
    fPartId = kUNDEFINED;
    fEnergy = -1;

    fCoreD  =  0;
    fCoreX  =  0;
    fCoreY  =  0;
    fImpact = -1;

    fPhotIni          = 0;
    fPassPhotAtm      = 0;
    fPassPhotRef      = 0;
    fPassPhotCone     = 0;
    fPhotElfromShower = 0;
    fPhotElinCamera   = 0;

    fElecCphFraction=0;
    fMuonCphFraction=0;
    fOtherCphFraction=0;

    fEventReuse = 0;    
}

// --------------------------------------------------------------------------
//
// Use this to set all data members
//
// THIS FUNCTION IS FOR THE SIMULATION OLNY.
// DON'T USE THIS MEMBERFUNCTION IN THE ANALYSIS.
//
void MMcEvt::Fill( UInt_t  fEvtNum,    ParticleId_t usPId, Float_t  fEner,
		   Float_t  fThi0,     Float_t  fFirTar,   Float_t  fzFirInt,
		   Float_t  fThet,     Float_t  fPhii,     Float_t  fCorD, 
		   Float_t  fCorX,     Float_t  fCorY,     Float_t  fImpa, 
		   Float_t  fTPhii,    Float_t  fTThet,    Float_t  fTFirst,
		   Float_t  fTLast,    Float_t  fL_Nmax,   Float_t  fL_t0,
		   Float_t  fL_tmax,   Float_t  fL_a,      Float_t  fL_b,
		   Float_t  fL_c,      Float_t  fL_chi2,   UInt_t   uiPin, 
		   UInt_t   uiPat,     UInt_t   uiPre,     UInt_t   uiPco,  
		   UInt_t   uiPelS,    UInt_t   uiPelC,    Float_t  elec,
                   Float_t  muon,      Float_t  other,     Float_t  fadc_jitter,
                   Int_t    reuse)
{
    //
    //  All datamembers are filled with the correspondin parameters.
    //
    //  Don't use this memberfunction in analysis
    //
    fEvtNumber = fEvtNum;
    fPartId = usPId  ;
    fEnergy = fEner  ;
    fThick0 = fThi0;
    fFirstTarget = fFirTar;
    fZFirstInteraction = fzFirInt;

    fTheta  = fThet ;
    fPhi    = fPhii ;

    fCoreD  = fCorD ;
    fCoreX  = fCorX ;
    fCoreY  = fCorY ;
    fImpact = fImpa ;

    fTelescopePhi = fTPhii;
    fTelescopeTheta = fTThet;
    fTimeFirst = fTFirst;
    fTimeLast = fTLast;
    fLongiNmax = fL_Nmax;
    fLongit0 = fL_t0;
    fLongitmax = fL_tmax;
    fLongia = fL_a;
    fLongib = fL_b;
    fLongic = fL_c;
    fLongichi2 = fL_chi2;

    fPhotIni      = uiPin ;
    fPassPhotAtm  = fPhotIni-uiPat ;
    fPassPhotRef  = fPassPhotAtm-uiPre ;
    fPassPhotCone = uiPco ;
    fPhotElfromShower = uiPelS ;
    fPhotElinCamera = uiPelC ;

    fElecCphFraction=elec;
    fMuonCphFraction=muon;
    fOtherCphFraction=other;

    fFadcTimeJitter = fadc_jitter;

    fEventReuse = reuse;
}

// --------------------------------------------------------------------------
//
// Print the contents of the container.
//
//  if you specify an option only the requested data members are printed:
//  allowed options are:
//   id, energy, impact
//
void MMcEvt::Print(Option_t *opt) const
{
    MMcEvtBasic::Print(opt);

    TString str(opt);
    if (str.IsNull())
        *fLog << " Photoelectrons: " << fPhotElfromShower << endl;
}

// --------------------------------------------------------------------------
//
// Return a proper description of the monte carlo event
//
TString MMcEvt::GetDescription(const TString &s) const
{
    TString txt("#splitline{");

    txt += GetParticleName();
    txt += " ";
    txt += s;
    txt += "}{  E=";
    txt += GetEnergyStr();
    txt += "  r=";
    txt += TMath::Nint(GetImpact()/100);
    txt += "m  Zd=";
    txt += MString::Format("%.1f", GetTelescopeTheta()*TMath::RadToDeg());
    txt += "\\circ  ";
    if (GetPhotElfromShower()>=10000)
        txt += MString::Format("%dk", TMath::Nint(GetPhotElfromShower()/1000.));
    else
        if (GetPhotElfromShower()>=1000)
            txt += MString::Format("%.1fk", GetPhotElfromShower()/1000.);
        else
            txt += GetPhotElfromShower();
    txt += "PhEl}";

    return txt;
}

Bool_t MMcEvt::SetupFits(fits &fin)
{
    if (fName!="MMcEvt")
    {
        *fLog << err << "ERROR - SetupFits only supported if name equals MMcEvt." << endl;
        return kFALSE;
    }

    if (!fin.SetRefAddress("MMcEvt.fEvtNumber",         fEvtNumber        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fThick0",            fThick0           ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fFirstTarget",       fFirstTarget      ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fZFirstInteraction", fZFirstInteraction))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fCoreD",             fCoreD            ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fCoreX",             fCoreX            ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fCoreY",             fCoreY            ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fTimeFirst",         fTimeFirst        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fTimeLast",          fTimeLast         ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongiNmax",         fLongiNmax        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongit0",           fLongit0          ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongitmax",         fLongitmax        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongia",            fLongia           ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongib",            fLongib           ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongic",            fLongic           ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fLongichi2",         fLongichi2        ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPhotIni",           fPhotIni          ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPassPhotAtm",       fPassPhotAtm      ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPassPhotRef",       fPassPhotRef      ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPassPhotCone",      fPassPhotCone     ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPhotElfromShower",  fPhotElfromShower ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fPhotElinCamera",    fPhotElinCamera   ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fElecCphFraction",   fElecCphFraction  ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fMuonCphFraction",   fMuonCphFraction  ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fOtherCphFraction",  fOtherCphFraction ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fFadcTimeJitter",    fFadcTimeJitter   ))  return kFALSE;
    if (!fin.SetRefAddress("MMcEvt.fEventReuse",        fEventReuse       ))  return kFALSE;

    return MMcEvtBasic::SetupFits(fin);
}
