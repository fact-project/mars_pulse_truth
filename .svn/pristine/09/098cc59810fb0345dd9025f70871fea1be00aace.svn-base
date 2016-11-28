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
!   Author(s): Robert Wagner  10/2002 <mailto:magicsoft@rwagner.de>
!   Author(s): Thomas Bretz   2/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2002-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MObservatory
//
// BE EXTREMLY CARFEFULL CHANGING THIS CLASS! THE TRACKING SYSTEM IS BASED
// ON IT!
//
/////////////////////////////////////////////////////////////////////////////
#include "MObservatory.h"

#include <TArrayD.h>
#include <TVector3.h>

#include "MTime.h"
#include "MAstro.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MObservatory);

using namespace std;

void MObservatory::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MObservatory";
    fTitle = title ? title : "Storage container for coordinates of an observatory";   
}

// --------------------------------------------------------------------------
//
// Default constructor sets name and title of instace.
// Default location is kMagic1
//
MObservatory::MObservatory(const char *name, const char *title)
{
    Init(name, title);

    SetLocation(kMagic1);
}

// --------------------------------------------------------------------------
//
// For example "MObservator(MObservatory::kMagic1)"
//
MObservatory::MObservatory(LocationName_t key, const char *name, const char *title)
{
    Init(name, title);

    SetLocation(key);
}

// --------------------------------------------------------------------------
//
// Calls SetLocation
//
MObservatory::MObservatory(Double_t lon, Double_t lat, const char *name)
{
    Init();

    SetLocation(lon, lat, 0, name);
}

// --------------------------------------------------------------------------
//
// Calls SetLocation
//
MObservatory::MObservatory(Double_t lon, Double_t lat, Double_t h, const char *name)
{
    Init();

    SetLocation(lon, lat, h, name);
}

// --------------------------------------------------------------------------
//
// BE EXTREMLY CARFEFULL CHANGING THIS CLASS! THE TRACKING SYSTEM IS BASED
// ON IT!
//
void MObservatory::SetLocation(LocationName_t name)
{
    switch (name)
    {
        // BE EXTREMLY CARFEFULL CHANGING THIS CLASS!
        // THE TRACKING SYSTEM IS BASED ON IT!
    case kMagic1:
        // Values taken from the GPS Receiver (avg 24h)
        // on 29/04/2004 at 17h30 in the counting house
        fLatitude  = MAstro::Dms2Rad(28, 45, 42.462, '+');
        fLongitude = MAstro::Dms2Rad(17, 53, 26.525, '-');
        fHeight    = 2199.4; // m
        fObservatoryName = "Observatorio del Roque de los Muchachos (Magic1)";
        break;

    case kWuerzburgCity:
        fLatitude  = MAstro::Dms2Rad(51, 38, 48.0);
        fLongitude = MAstro::Dms2Rad( 9, 56, 36.0);
        fHeight    = 300;
        fObservatoryName = "Wuerzburg City";
        break;

    case kTuorla:
        fLatitude  = MAstro::Dms2Rad(60, 24, 57.0);
        fLongitude = MAstro::Dms2Rad(22, 26, 42.0);
        fHeight    = 53;
        fObservatoryName = "Tuorla";
        break;
    }

    fObservatoryKey = name;

    fSinLatitude = TMath::Sin(fLatitude);
    fCosLatitude = TMath::Cos(fLatitude);
}

// --------------------------------------------------------------------------
//
// Longitude/Latitude [rad]
// Height             [m]
//
void MObservatory::SetLocation(Double_t lon, Double_t lat, Double_t h, const char *name)
{
    fLongitude = lon;
    fLatitude  = lat;
    fHeight    = h;

    fSinLatitude = TMath::Sin(fLatitude);
    fCosLatitude = TMath::Cos(fLatitude);

    if (name)
        fObservatoryName = name;
}

void MObservatory::Print(Option_t *) const
{
    *fLog << all;
    *fLog << underline << fObservatoryName << ":" << endl;
    *fLog << "Latitude:  " << TMath::Abs(fLatitude*kRad2Deg)  << " deg " << (fLatitude  > 0 ? "N" : "S") << endl;
    *fLog << "Longitude: " << TMath::Abs(fLongitude*kRad2Deg) << " deg " << (fLongitude < 0 ? "E" : "W") << endl;
    *fLog << "Height:    " << fHeight << "m" << endl;
}

// --------------------------------------------------------------------------
//
// Get the corresponding rotation angle of the sky coordinate system
// seen with an Alt/Az telescope.
//
// For more information see MAstro::RotationAngle
//
void MObservatory::RotationAngle(Double_t theta, Double_t phi, Double_t &sin, Double_t &cos) const
{
    MAstro::RotationAngle(fSinLatitude, fCosLatitude, theta, phi, sin, cos);
}

// --------------------------------------------------------------------------
//
// Get the corresponding rotation angle of the sky coordinate system
// seen with an Alt/Az telescope.
//
// For more information see MAstro::RotationAngle
//
Double_t MObservatory::RotationAngle(Double_t theta, Double_t phi) const
{
    return MAstro::RotationAngle(fSinLatitude, fCosLatitude, theta, phi);
}

// --------------------------------------------------------------------------
//
// Get the time (as mjd) of sunrise/sunset at the day floor(mjd)
// above/below alt[deg]
//
// For more information see MAstro::GetSunRiseSet
//
// A TArrayD(2) is returned with the sunrise in TArray[0] and the
// sunset in TArrayD[1].
//
TArrayD MObservatory::GetSunRiseSet(Double_t mjd, Double_t alt) const
{
    return MAstro::GetSunRiseSet(mjd, GetLongitudeDeg(), GetLatitudeDeg(), alt);
}

// --------------------------------------------------------------------------
//
// Setup the observatory location from resource file:
//    MObservatory.Name: Magic1, WuerzburgCity
//
Int_t MObservatory::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (IsEnvDefined(env, prefix, "Name", print))
    {
        TString name = GetEnvValue(env, prefix, "Name", "Magic1");
        name = name.Strip(TString::kBoth);

        if (name==(TString)"Magic1")
        {
            SetLocation(kMagic1);
            return kTRUE;
        }

        if (name==(TString)"WuerzburgCity")
        {
            SetLocation(kWuerzburgCity);
            return kTRUE;
        }

        *fLog << err << "MObservatory::ReadEnv - ERROR: Observatory " << name << " unknown." << endl;
        return kERROR;
    }

    return kFALSE;

}
