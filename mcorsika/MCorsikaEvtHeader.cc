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
!   Author(s): Thomas Bretz 11/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCorsikaEvtHeader 
//
// Class Version 2:
// ----------------
//  - UInt_t fParticleID
//
//
// Class Version 3:
// ----------------
//  - UInt_t fNumReuse
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MCorsikaEvtHeader.h"
#include "MCorsikaFormat.h"

#include <iomanip>
#include <fstream>

#include "fits.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

//#include "MMcEvt.hxx"

ClassImp(MCorsikaEvtHeader);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Create the array to store the data.
//
MCorsikaEvtHeader::MCorsikaEvtHeader(const char *name, const char *title)
    : fNumReuse((UInt_t)-1), fX(0), fY(0)
{
    fName  = name  ? name  : "MCorsikaEvtHeader";
    fTitle = title ? title : "Raw Event Header Information";
}

// --------------------------------------------------------------------------
//
// Return Impact (distance of ground incident point from telescope axis)
//
// Distance d between a point q and a line u (via p):
//   d = | ( vec(q) - vec(p) ) x vec(u) |  /  | vec(u) |
//
// If p = 0
//
//   ==>   d = | vec(q) x vec(u) |  /  | vec(u) |
//          w :=  q = (x/y/0)
//          v :=  u = (Alt/Az)
//
// For Alt/Az = Theta/Phi:
//
//  x = r sin(theta) cos(phi)
//  y = r sin(theta) sin(phi)
//  z = r cos(theta)
//
//                   (   -q3*u2    )   ( -cos(theta)*y                                     )
// vec(q) x vec(u) = (    q3*u1    ) = (  cos(theta)*x                                     )
//                   ( q1*u2-q2*u1 )   ( sin(theta) cos(phi) * y - sin(theta) sin(phi) * x )
//
//   ==> d = sqrt( cos(theta)^2 (x^2  + y^2 ) +
//                 sin(theta)^2 ( cos(phi) y + sin(phi) x)^2 )
//
Double_t MCorsikaEvtHeader::GetImpact() const
{
    const Double_t c = TMath::Cos(fZd);
    const Double_t s = TMath::Sin(fZd);

    const Double_t p = TMath::Cos(fAz)*fX - TMath::Sin(fAz)*fY;

    return TMath::Sqrt(c*c*(fX*fX + fY*fY) + s*s* p*p);
}

// --------------------------------------------------------------------------
//
//  This member function prints all Data of one Event to *fLog.
//
void MCorsikaEvtHeader::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << "Event Number:              " << dec << fEvtNumber << " (reused=" << fNumReuse << ")" << endl;
//    *fLog << "Particle ID:               " << MMcEvt::GetParticleName(fParticleID) << endl;
    *fLog << "Energy:                    " << fTotalEnergy << "GeV" << endl;
    *fLog << "Starting Altitude:         " << fStartAltitude << "g/cm" << UTF8::kSquare << endl;
    *fLog << "Number of 1st Target:      " << fFirstTargetNum << endl,
    *fLog << "Height of 1st Interaction: " << fFirstInteractionHeight/100. << "m" << endl;
    *fLog << "Momentum X/Y/Z (GeV/c):    " << fMomentumX << "/" << fMomentumY << "/" << fMomentumZ << endl;
    *fLog << "Zenith/Azimuth Angle:      " << fZd*TMath::RadToDeg() << UTF8::kDeg << "/" << fAz*TMath::RadToDeg() << UTF8::kDeg << endl;
    *fLog << "Impact X/Y:                " << fX/100. << "m/" << fY/100. << "m  (r=" << TMath::Hypot(fX, fY)/100. << "m)" << endl;
    *fLog << "Weighted Num Photons:      " << fWeightedNumPhotons << endl;
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// get the EVTH information from the input parameter f
//
Int_t MCorsikaEvtHeader::ReadEvt(Float_t * f)
{

    fEvtNumber  = TMath::Nint(f[0]);
//    fParticleID = TMath::Nint(f[1]);

    fTotalEnergy            = f[2];
    fStartAltitude          = f[3];
    fFirstTargetNum         = f[4];
    fFirstInteractionHeight = f[5]; // FIXME: Could be negative

    // Pointing opposite particle direction
    // (x=north, y=west, z=upwards)
    //fMomentumX = f[6];
    //fMomentumY = f[7];
    //fMomentumZ = f[8];
    // Pointing along particle direction
    // (x=east, y=north, z=upwards)
    fMomentumX =  f[7];
    fMomentumY = -f[6];
    fMomentumZ = -f[8];  // Does this minus make sense?!

    fZd        = f[9];
    fAz        = TMath::Pi()-f[10];

    // f[96] // Number of reuse of event [max=20]
    fTotReuse = TMath::Nint(f[96]);
    if (fTotReuse > 20)
      {
      *fLog << err << "Number of reuse of shower is " << fTotReuse 
                   << " But maximum implemented: 20" << endl;
      return kERROR;
      }

    memcpy(fTempX, f +97, 20*sizeof(Float_t));
    memcpy(fTempY, f+117, 20*sizeof(Float_t));


    // FIXME: Check fNumReuse<20
//    fX =  f[117 + fNumReuse];
//    fY = -f[ 97 + fNumReuse];

    fWeightedNumPhotons = 0;

    return kTRUE;
}


// this member function is for reading the event end block

Int_t MCorsikaEvtHeader::ReadEvtEnd(MCorsikaFormat * fInFormat)
{
    Float_t f[272];
    if (!fInFormat->Read(f, 272 * sizeof(Float_t)))
        return kERROR;

    const UInt_t evtnum = TMath::Nint(f[0]);
    if (evtnum!=fEvtNumber)
    {
        *fLog << err << "ERROR - Mismatch in stream: Event number in EVTE (";
        *fLog << evtnum << ") doesn't match EVTH (" << fEvtNumber << ")." << endl;
        return kERROR;
    }

    // FIXME: What's the meaning of this if in case of reusing the event this number
    //        exists only once?
    fWeightedNumPhotons = f[1];

    return fInFormat->Eof() ? kERROR : kTRUE;
}

Bool_t MCorsikaEvtHeader::SetupFits(fits &fin)
{
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fEvtNumber",              fEvtNumber              ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fNumReuse",               fNumReuse               ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fTotalEnergy",            fTotalEnergy            ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fStartAltitude",          fStartAltitude          ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fFirstTargetNum",         fFirstTargetNum         ))  return kFALSE;
    if (!fin.SetRefAddress( "CorsikaEvtHeader.fFirstInteractionHeight", fFirstInteractionHeight ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fMomentumX",              fMomentumX              ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fMomentumY",              fMomentumY              ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fMomentumZ",              fMomentumZ              ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fZd",                     fZd                     ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fAz",                     fAz                     ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fX",                      fX                      ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fY",                      fY                      ))  return kFALSE;
    if (!fin.SetRefAddress("MCorsikaEvtHeader.fWeightedNumPhotons",     fWeightedNumPhotons     ))  return kFALSE;

    return kTRUE;
}
