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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportDrive
//
// This is the class interpreting and storing the DRIVE-REPORT information.
//
// This is NOT the place to get the pointing position from. The class
// definition might change. But it is the place to calculate the
// correct pointing position from.
//
//
// Double_t fMjd;        // Modified Julian Date send by the drive system
//  This is the MJD as it was calculated by the drive system when the report
//  was send.
//
// Double_t fRa;         // [h]   Right ascension
// Double_t fDec;        // [deg] Declination
// Double_t fHa;         // [h]   Hour angle
//  Currently this describes the nominal source position
//
// Double_t fNominalZd;  // [deg] Nominal zenith distance
// Double_t fNominalAz;  // [deg] Nominal azimuth
//  The nominal local position like it was calculated in the last
//  control loop for time at which the last shaftencoder value has changed
//
// Double_t fCurrentZd;  // [deg] current zenith distance
// Double_t fCurrentAz;  // [deg] current azimuth
//  The current local position like it was calculated in the last
//  control loop from interpolated shaftencoder values for time at which
//  the last shaftencoder value has changed
//
// Double_t fErrorZd;    // [?] system error in the zenith angle axis
// Double_t fErrorAz;    // [?] sistem error in the azimuth angle axis
//  The system error on both axis derived from the two values above. Be
//  carefull, eg near the zenith we a huge deviation in azimuth
//  while having a small deviation in zenith angle might be meaingless.
//  Please use GetAbsError to get the absolute distance between the
//  twopositions.
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportDrive.h"

#include "fits.h"

#include "MLogManip.h"

#include "MAstro.h"

ClassImp(MReportDrive);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "DRIVE-REPORT"
//
MReportDrive::MReportDrive() : MReport("DRIVE-REPORT"),
    fMjd(0), fRa(0), fDec(0), fHa(0), fNominalZd(0), fNominalAz(0),
    fCurrentZd(0), fCurrentAz(0), fErrorZd(0), fErrorAz(0)
{
    fName  = "MReportDrive";
    fTitle = "Class for DRIVE-REPORT information (raw telescope position)";
}

Bool_t MReportDrive::SetupReadingFits(fits &file)
{
    return
        file.SetRefAddress("Ra",  fRa)  &&
        file.SetRefAddress("Dec", fDec) &&
        file.SetRefAddress("Ha",  fHa)  &&
        file.SetRefAddress("Az",  fNominalAz) &&
        file.SetRefAddress("Zd",  fNominalZd) &&
        file.SetRefAddress("dAz", fErrorAz)   &&
        file.SetRefAddress("dZd", fErrorZd);
}

Int_t MReportDrive::InterpreteFits(const fits &fits)
{
    fCurrentZd = fNominalZd - fErrorZd;
    fCurrentAz = fNominalAz - fErrorAz;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the body of the DRIVE-REPORT string
//
Int_t MReportDrive::InterpreteBody(TString &str, Int_t ver)
{
    MAstro::String2Angle(str, fRa);
    MAstro::String2Angle(str, fDec);
    MAstro::String2Angle(str, fHa);

    Int_t len;
    Int_t n=sscanf(str.Data(), "%lf %n", &fMjd, &len);
    if (n!=1)
    {
        *fLog << warn << "WARNING - Not enough arguments." << endl;
        return kCONTINUE;
    }

    str.Remove(0, len);

    MAstro::String2Angle(str, fNominalZd);
    MAstro::String2Angle(str, fNominalAz);
    MAstro::String2Angle(str, fCurrentZd);
    MAstro::String2Angle(str, fCurrentAz);

    n=sscanf(str.Data(), "%lf %lf %n", &fErrorZd, &fErrorAz, &len);
    if (n!=2)
    {
        *fLog << warn << "WARNING - Not enough arguments." << endl;
        return kCONTINUE;
    }

    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    if (ver>=200802200)
    {
        Int_t dummy; // Cosy armed or not
        n=sscanf(str.Data(), "%d %n", &dummy, &len);
        if (n!=1)
        {
            *fLog << warn << "WARNING - Not enough arguments." << endl;
            return kCONTINUE;
        }

        str.Remove(0, len);
        str = str.Strip(TString::kBoth);
    }

    if (ver>=200905170)
    {
        Int_t dummy; // Starguider switched on or not
        n=sscanf(str.Data(), "%d %n", &dummy, &len);
        if (n!=1)
        {
            *fLog << warn << "WARNING - Not enough arguments." << endl;
            return kCONTINUE;
        }

        str.Remove(0, len);
        str = str.Strip(TString::kBoth);
    }

    return str.IsNull() ? kTRUE : kCONTINUE;
}

// --------------------------------------------------------------------------
//
// GetAbsError [deg]
//
// Returns the absolute deviation from the nominal position calculated
// from the system error.
//
//
Double_t MReportDrive::GetAbsError() const
{
    return MAstro::GetDevAbs(fNominalZd, fCurrentZd, fErrorAz);
}

void MReportDrive::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": Mjd=" << fMjd << " Ra=" << fRa << " Dec=" << fDec;
    *fLog << " dZd=" << fErrorZd << " dAz=" << fErrorAz << " D=" << GetAbsError() << endl;
}
