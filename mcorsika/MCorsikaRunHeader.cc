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
!   Author(s): Thomas Bretz 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
               Qi Zhe,      06/2007 <mailto:qizhe@astro.uni-wuerzburg.de>

!   Copyright: Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCorsikaRunHeader
//
// Root storage container for the RUN HEADER information
//
// Class Version 2:
// ----------------
//  + UInt_t fParticleID
//  + Float_t fImpactMax
//  + Float_t fMagneticFieldX
//  + Float_t fMagneticFieldZ
//  + Float_t fMagneticFieldAz
//  + Float_t fAtmosphericLayers[5]
//  + Float_t fAtmosphericCoeffA[5]
//  + Float_t fAtmosphericCoeffB[5]
//  + Float_t fAtmosphericCoeffC[5]
//  + UInt_t  fCerenkovFlag
//
// Class Version 3:
// ----------------
//  + UInt_t  fNumReuse
//
////////////////////////////////////////////////////////////////////////////

#include "MCorsikaRunHeader.h"
#include "MCorsikaFormat.h"

#include <fstream>
#include <iomanip>

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"

ClassImp(MCorsikaRunHeader);

using namespace std;

const Double_t MCorsikaRunHeader::fgEarthRadius = 637131500; // [cm] Earth radius as defined in CORSIKA

// --------------------------------------------------------------------------
//
// Default constructor. Creates array which stores the pixel assignment.
//
//
MCorsikaRunHeader::MCorsikaRunHeader(const char *name, const char *title)
    : fNumObsLevel(0), fImpactMax(-1), fZdMin(0), fZdMax(-1),
    fAzMin(0), fAzMax(0),  fViewConeInnerAngle(0), fViewConeOuterAngle(-1)
{
    fName  = name  ? name  : "MCorsikaRunHeader";
    fTitle = title ? title : "Raw Run Header Information";
}

// --------------------------------------------------------------------------
//
// Read in one run header from the binary file
//
Bool_t MCorsikaRunHeader::ReadEvt(MCorsikaFormat * fInFormat)
{
    Float_t f[272];
    if (!fInFormat->Read(f, 272 * sizeof(Float_t)))
        return kFALSE;

    fRunNumber = TMath::Nint(f[0]);
    fNumEvents = 0;

    fRunStart.SetCorsikaTime(f[1]);

    fProgramVersion = f[2];
    fNumObsLevel    = TMath::Nint(f[3]);

    if (fNumObsLevel!=1)
    {
        *fLog << err << "ERROR - Currently only one observation level is allowed." << endl;
        return kFALSE;
    }

    memset(fObsLevel, 0, 10*4);
    memcpy(fObsLevel, f+4, fNumObsLevel*4);

    fSlopeSpectrum  = f[14];
    fEnergyMin      = f[15];
    fEnergyMax      = f[16];

    // Implemented in CORSIKA Version >= 6.822
    fImpactMax = -1;

    // CORSIKA scattering in a disc on the ground
    if (f[246]>0 && f[247]==0 && !fInFormat->IsEventioFormat())
    {
        *fLog << warn << "WARNING - Events scattered in a disc on the ground." << endl;
        fImpactMax = f[246];
    }

    // MMCS scattering in a disc perpendicular to the shower axis
    if (f[246]==0 && f[247]>0)
        fImpactMax = f[247];

    // CORSIKA scattering in a rectangle on the ground
    if (f[246]>0 && f[247]>0)
        *fLog << warn << "WARNING - Events scattered in a rectangle on the ground." << endl;

    // Implemented in CORSIKA Version >= 6.822
    memcpy(fAtmosphericLayers, f+248, 5*4);

    memcpy(fAtmosphericCoeffA, f+253, 5*4);
    memcpy(fAtmosphericCoeffB, f+258, 5*4);
    memcpy(fAtmosphericCoeffC, f+263, 5*4);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read in one event header. It is called for the first event header after  
// a run header                                                             
//
Bool_t MCorsikaRunHeader::ReadEventHeader(Float_t * g)
{

    // -------------------- Read first event header -------------------

    // FIXME: Add sanity checks!

    // f[76] Cherenkov flag:
    //        bit(1) : CERENKOV option compiled in
    //        bit(2) : IACT option compiled in
    //        bit(3) : CEFFIC option compiled in
    //        bit(4) : ATMEXT option compiled in
    //        bit(5) : ATMEXT option used with refraction enabled
    //        bit(6) : VOLUMEDET option compiled in
    //        bit(7) : CURVED option compiled in
    //        bit(9) : SLATN option compiled in
    //        11-21  : table number for externam athmosphere (but<1024)
    //
    // f[78]  Curved athmosphere? (0=flat, 1=curved)
    // f[84]  cherenkov bunch size
    // f[93]  flag for additinal muon information of particle output file
    // f[145] Muon multiple scattering flag


    fNumReuse = TMath::Nint(g[96]);  // Number i of uses of each cherenkov event

    fParticleID = TMath::Nint(g[1]);

    // MAGNETIC FIELD: x/z-component of earth magnetic field in muT
    fMagneticFieldX  =  g[69];  // x-component ( BX)
    fMagneticFieldZ  = -g[70];  // z-component (-BZ)
    fMagneticFieldAz =  g[91];  // Azimuth angle of magnetic north expressed in telescope coordinates

    // WITH rounding: unbelievable!
    fCerenkovFlag = TMath::Nint(g[75]);

    fZdMin = g[79];                // lower edge of theta in °
    fZdMax = g[80];                // upper edge of theta in °
    fAzMin = 180-g[81];            // lower edge of phi   in °
    fAzMax = 180-g[82];            // upper edge of phi   in °
    // FIXME: Correct for direction of magnetic field!

    if (TMath::Nint(g[83])!=1)
        *fLog << warn << "WARNING - Cherenkov bunch size not 1, but " << g[83] << endl;

    // g[84] Number of cherenkov detectors in x
    // g[85] Number of cherenkov detectors in y
    // g[86] Grid spacing x
    // g[87] Grid spacing y
    // g[88] Length of detectors in x
    // g[89] Length of detectors in y

    fImpactMax = -1;
/*
    // This is a trick to use CERARY for storage of the
    // maximum simulated impact
    if (TMath::Nint(g[84])==1 && TMath::Nint(g[85])==1 &&
        TMath::Nint(g[88])==1 && TMath::Nint(g[89])==1 &&
        g[86]==g[87])
        fImpactMax = g[86];
 */
    fWavelengthMin = g[94];        // Cherenkov bandwidth lower end in nm
    fWavelengthMax = g[95];        // Cherenkov bandwidth upper end in nm

    fViewConeInnerAngle = g[151];  // inner angle of view cone (°)
    fViewConeOuterAngle = g[152];  // outer angle of view cone (°)

    return kTRUE;
}

Bool_t MCorsikaRunHeader::ReadEvtEnd(MCorsikaFormat * fInFormat, Bool_t runNumberVerify)
{
    Float_t f[2];
    if (!fInFormat->Read(f, 2 * sizeof(Float_t)))
        return kFALSE;

    if (runNumberVerify)
      {
       const UInt_t runnum = TMath::Nint(f[0]);
       if (runnum!=fRunNumber)
       {
           *fLog << err << "ERROR - Mismatch in stream: Run number in RUNE (";
           *fLog << runnum << ") doesn't match RUNH (" << fRunNumber << ")." << endl;
           return kFALSE;
       }
      }

    fNumEvents = TMath::Nint(f[1]);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// print run header information on *fLog. The option 'header' supresses
// the pixel index translation table.
//
void MCorsikaRunHeader::Print(Option_t *t) const
{
    *fLog << all << endl;
    *fLog << "Run Number:     " << fRunNumber << "  (" << fRunStart.GetStringFmt("%d.%m.%Y") << ", V" << fProgramVersion << ")" << endl;
    *fLog << "Particle ID:    " << MMcEvt::GetParticleName(fParticleID) << endl;
    if (fNumEvents>0)
        *fLog << "Num Events:     " << fNumEvents << " (reuse " << fNumReuse << " times)" << endl;
    *fLog << "Obs Level:     ";
    for (Byte_t i=0; i<fNumObsLevel; i++)
        *fLog << " " << fObsLevel[i]/100. << "m";
    *fLog << endl;

    *fLog << "MagneticField:  X/Z=(" << fMagneticFieldX << "/";
    *fLog << fMagneticFieldZ << ")" << UTF8::kMu << "T  Az=" << fMagneticFieldAz*TMath::RadToDeg() << UTF8::kDeg << "  (magnetic North w.r.t. North)" << endl;

    *fLog << "Spectrum:       Slope=" << fSlopeSpectrum << "  (" << fEnergyMin << "GeV-" << fEnergyMax << "GeV)" <<  endl;
    *fLog << "Wavelength:     " << fWavelengthMin << "nm - " << fWavelengthMax << "nm" << endl;

    if (fImpactMax>0)
        *fLog << "ImpactMax:      " << fImpactMax << "cm" << endl;
    if (fViewConeOuterAngle>0)
        *fLog << "ViewCone:       " << fViewConeInnerAngle << UTF8::kDeg << " - " << fViewConeOuterAngle << UTF8::kDeg << endl;

    *fLog << "Zd/Az:          ";
    if (fZdMax>=0 && fZdMin<360)
    {
        *fLog << fZdMin << UTF8::kDeg;
        if (fZdMin==fZdMax)
            *fLog << " (fixed)";
        else
            *fLog << "-" << fZdMax << UTF8::kDeg;
        *fLog << " / " << fAzMin << UTF8::kDeg;
        if (fAzMin==fAzMax)
            *fLog << " (fixed)";
        else
            *fLog << "-" << fAzMax << UTF8::kDeg;
        *fLog << "  w.r.t. magnetic North." << endl;
    }

    if (fZdMin>=360) // 4010.7
        *fLog << "-trajectory-" << endl;


    if (fImpactMax>0)
        *fLog << "Max.sim.Impact: " << fImpactMax << "cm" << endl;

    *fLog << "Options used:  ";
    if (Has(kCerenkov))
        *fLog << " CERENKOV";
    if (Has(kIact))
        *fLog << " IACT";
    if (Has(kCeffic))
        *fLog << " CEFFIC";
    if (Has(kAtmext))
        *fLog << " ATMEXT" << GetNumAtmosphericModel();
    if (Has(kRefraction))
        *fLog << " +Refraction";
    if (Has(kVolumedet))
        *fLog << " VOLUMEDET";
    if (Has(kCurved))
        *fLog << " CURVED";
    if (Has(kSlant))
        *fLog << " SLANT";
    *fLog << " [" << hex << fCerenkovFlag << "]" << dec << endl;

    if (HasLayers())
    {
        *fLog << "Atm.Layers:    ";
        for (int i=0; i<5; i++)
            *fLog << " " << fAtmosphericLayers[i];
    }
    *fLog << endl;
    *fLog << "Atm.Coeff A:   ";
    for (int i=0; i<5; i++)
        *fLog << " " << fAtmosphericCoeffA[i];
    *fLog << endl;
    *fLog << "Atm.Coeff B:   ";
    for (int i=0; i<5; i++)
        *fLog << " " << fAtmosphericCoeffB[i];
    *fLog << endl;
    *fLog << "Atm.Coeff C:   ";
    for (int i=0; i<5; i++)
        *fLog << " " << fAtmosphericCoeffC[i];
    *fLog << endl;


}

