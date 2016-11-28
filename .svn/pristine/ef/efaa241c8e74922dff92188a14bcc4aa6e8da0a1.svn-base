/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 12/2000 <mailto:thomas.bretz@epfl.ch>
!   Author(s): Qi Zhe,       06/2007 <mailto:qizhe@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MPhotonData
//
//  Storage container to store Corsika events
//
// For details on the coordinate systems see our Wiki.
//
//   Version 1:
//   ----------
//    * First implementation
//
//   Version 2:
//   ----------
//    - fNumPhotons
//
/////////////////////////////////////////////////////////////////////////////
#include "MPhotonData.h"

#include <fstream>
#include <iostream>

#include <TMath.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MPhotonData);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MPhotonData::MPhotonData(/*const char *name, const char *title*/)
    : fPosX(0), fPosY(0), fCosU(0), fCosV(0), fTime(0), fWavelength(0),
    /*fNumPhotons(1),*/ fProductionHeight(0), fPrimary(MMcEvtBasic::kUNDEFINED),
      fTag(-1), fMirrorTag(-1), fWeight(1)
{
   // fName  = name  ? name  : "MPhotonData";
   // fTitle = title ? title : "Corsika Event Data Information";
}

/*
MPhotonData::MPhotonData(const MPhotonData &ph)
: fPosX(ph.fPosX), fPosY(ph.fPosY), fCosU(ph.fCosU), fCosV(ph.fCosV),
fTime(ph.fTime), fWavelength(ph.fWavelength), fNumPhotons(ph.fNumPhotons),
fProductionHeight(ph.fProductionHeight), fPrimary(ph.fPrimary),
fTag(ph.fTag), fWeight(ph.fWeight)
{
}
*/

// --------------------------------------------------------------------------
//
// Copy function. Copy all data members into obj.
//
void MPhotonData::Copy(TObject &obj) const
{
    MPhotonData &d = static_cast<MPhotonData&>(obj);

//    d.fNumPhotons       = fNumPhotons;
    d.fPosX             = fPosX;
    d.fPosY             = fPosY;
    d.fCosU             = fCosU;
    d.fCosV             = fCosV;
    d.fWavelength       = fWavelength;
    d.fPrimary          = fPrimary;
    d.fTime             = fTime;
    d.fTag              = fTag;
    d.fWeight           = fWeight;
    d.fProductionHeight = fProductionHeight;

    TObject::Copy(obj);
}

// --------------------------------------------------------------------------
//
// Return the square cosine of the Theta-angle == 1-CosU^2-CosV^2
//
Double_t MPhotonData::GetCosW2() const
{
    return 1 - GetSinW2();
}

// --------------------------------------------------------------------------
//
// Return the square sine of the Theta-angle == CosU^2+CosV^2
//
Double_t MPhotonData::GetSinW2() const
{
    return fCosU*fCosU + fCosV*fCosV;
}

// --------------------------------------------------------------------------
//
// return the cosine of the Theta-angle == sqrt(1-CosU^2-CosV^2)
//
Double_t MPhotonData::GetCosW() const
{
    return TMath::Sqrt(GetCosW2());
}

// --------------------------------------------------------------------------
//
// return the sine of the Theta-angle == sqrt(CosU^2+CosV^2)
//
Double_t MPhotonData::GetSinW() const
{
    return TMath::Sqrt(GetSinW2());
}

// --------------------------------------------------------------------------
//
// Return the theta angle in radians
//
Double_t MPhotonData::GetTheta() const
{
    return TMath::ASin(GetSinW());
}

// --------------------------------------------------------------------------
//
// Return a TQuaternion with the first three components x, y, and z
// and the fourth component the time.
//
TQuaternion MPhotonData::GetPosQ() const
{
    return TQuaternion(GetPos3(), fTime);
}

// --------------------------------------------------------------------------
//
// return a TQuaternion with the first three components the direction
// moving in space (GetDir3()) and the fourth component is the
// one devided by the speed of light (converted to cm/ns)
//
// FIXME: v in air!
//
TQuaternion MPhotonData::GetDirQ() const
{
    return TQuaternion(GetDir3(), 1./(TMath::C()*100/1e9));
}

// --------------------------------------------------------------------------
//
// Set the wavelength to a random lambda^-2 distributed value
// between wmin and wmax.
//
void MPhotonData::SimWavelength(Float_t wmin, Float_t wmax)
{
    const Double_t w = gRandom->Uniform(wmin, wmax);

    fWavelength = TMath::Nint(wmin*wmax / w);
}


// --------------------------------------------------------------------------
//
// Set the data member according to the 8 floats read from a reflector-file.
// This function MUST reset all data-members, no matter whether these are
// contained in the input stream.
//
Int_t MPhotonData::FillRfl(Float_t f[8])
{
    // Check coordinate system!!!!
    fWavelength = TMath::Nint(f[0]);
    fPosX = f[1];  // [cm]
    fPosY = f[2];  // [cm]
    fCosU = f[3];  // cos to x
    fCosV = f[4];  // cos to y
    fTime = f[5];  // [ns]
    fProductionHeight = f[6];

    // f[7]: Camera inclination angle

    fPrimary    = MMcEvtBasic::kUNDEFINED;
//    fNumPhotons =  1;
    fTag        = -1;
    fWeight     =  1;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the data member according to the 7 floats read from a corsika-file.
// This function MUST reset all data-members, no matter whether these are
// contained in the input stream.
//
// Currently we exchange x and y and set y=-y to convert Corsikas coordinate
// system intpo our own.
//
Int_t MPhotonData::FillCorsika(Float_t f[7], Int_t i)
{
    const UInt_t n = TMath::Nint(f[0]);

    if (n==0)
        // FIXME: Do we need to decode the rest anyway?
        return kCONTINUE;

    // Check reuse
    if (i >=0)
      {
       const Int_t reuse = (n/1000)%100; // Force this to be 1!
       if (reuse!=i)
           return kCONTINUE;
      }

    // This seems to be special to mmcs
    fWavelength = n%1000;
    fPrimary    = MMcEvtBasic::ParticleId_t(n/100000);

    // x=north, y=west
    //fPosX = f[1];  // [cm]
    //fPosY = f[2];  // [cm]
    //fCosU = f[3];  // cos to x
    //fCosV = f[4];  // cos to y
    // x=west, y=south
    fPosX =  f[2];  // [cm]
    fPosY = -f[1];  // [cm]

    fCosU =  f[4];  // cos to x
    fCosV = -f[3];  // cos to y

    fTime =  f[5];  // [ns]

    fProductionHeight = f[6]; // [cm]

    // Now reset all data members which are not in the stream
    fTag    = -1;
    fWeight =  1;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the data member according to the 8 shorts read from a eventio-file.
// This function MUST reset all data-members, no matter whether these are
// contained in the input stream.
//
// Currently we exchange x and y and set y=-y to convert Corsikas coordinate
// system into our own.
//
Int_t MPhotonData::FillEventIO(Short_t f[8])
{
    // From 5.5 compact_bunch:
    // https://www.mpi-hd.mpg.de/hfm/~bernlohr/iact-atmo/iact_refman.pdf

    // photons in this bunch f[6]/100.

    fPosY             = -f[0]/10.;              // ypos relative to telescope [cm]
    fPosX             =  f[1]/10.;              // xpos relative to telescope [cm]
    fCosV             = -f[2]/30000.;           // cos to y
    fCosU             =  f[3]/30000.;           // cos to x

    fTime             =  f[4]/10.;              // a relative arival time [ns]
    fProductionHeight =  pow(10, f[5]/1000.);   // altitude of emission a.s.l. [cm]
    fWavelength       =  f[7];                  // wavelength [nm]: 0 undetermined, <0 already in p.e.

    // Now reset all data members which are not in the stream
    fPrimary    = MMcEvtBasic::kUNDEFINED;
    fTag        = -1;
    fWeight     =  1;

    return 1;
}

// --------------------------------------------------------------------------
//
// Set the data member according to the 8 floats read from a eventio-file.
// This function MUST reset all data-members, no matter whether these are
// contained in the input stream.
//
// Currently we exchange x and y and set y=-y to convert Corsikas coordinate
// system into our own.
//
Int_t MPhotonData::FillEventIO(Float_t f[8])
{
    // photons in this bunch
    const UInt_t n = TMath::Nint(f[6]);
    if (n==0)
        return 0;

    fPosX             =  f[1];              // xpos relative to telescope [cm]
    fPosY             = -f[0];              // ypos relative to telescope [cm]
    fCosU             =  f[3];              // cos to x
    fCosV             = -f[2];              // cos to y
    //fTime             =  f[4];              // a relative arival time [ns]
    //fProductionHeight =  f[5];              // altitude of emission [cm]
    fWavelength       =  0;                 // so far always zeor = unspec. [nm]

    // Now reset all data members which are not in the stream
    fPrimary    = MMcEvtBasic::kUNDEFINED;
    fTag        = -1;
    fWeight     =  1;

    return n-1;
}

/*
// --------------------------------------------------------------------------
//
// Read seven floats from the stream and call FillCorsika for them.
//
Int_t MPhotonData::ReadCorsikaEvt(istream &fin)
{
    Float_t f[7];
    fin.read((char*)&f, 7*4);

    const Int_t rc = FillCorsika(f);

    return rc==kTRUE ? !fin.eof() : rc;
}

// --------------------------------------------------------------------------
//
// Read eight floats from the stream and call FillRfl for them.
//
Int_t MPhotonData::ReadRflEvt(istream &fin)
{
    Float_t f[8];
    fin.read((char*)&f, 8*4);

    const Int_t rc = FillRfl(f);

    return rc==kTRUE ? !fin.eof() : rc;
}
*/

// --------------------------------------------------------------------------
//
// Print contents. The tag and Weight are only printed if they are different
// from the default.
//
void MPhotonData::Print(Option_t *) const
{
    gLog << inf << endl;
//    gLog << "Num Photons:      " << fNumPhotons << " from " << MMcEvtBasic::GetParticleName(fPrimary) << endl;
    gLog << "Origin:           " << MMcEvtBasic::GetParticleName(fPrimary) << endl;
    gLog << "Wavelength:       " << fWavelength << "nm" << endl;
    gLog << "Pos X/Y  Cos U/V: " << fPosX << "/" << fPosY << "   " << fCosU << "/" << fCosV << endl;
    gLog << "Time/Prod.Height: " << fTime << "ns/" << fProductionHeight/100 << "m" << endl;
    if (fTag>=0)
        gLog << "Tag:              " << fTag << endl;
    if (fWeight!=1)
        gLog << "Weight:           " << fWeight << endl;
}
