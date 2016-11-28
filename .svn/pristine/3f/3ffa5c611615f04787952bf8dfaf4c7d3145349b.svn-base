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
!   Author(s): Benjamin Riegel, 01/2005 <mailto:riegel@astro.uni-wuerzburg.de>
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportStarguider
//
// This is the class interpreting and storing the STARG-REPORT information.
//
// This is the place to get the azimuth-/zenith mispointing of the telescope
// given by the starguider-camera.
//
//
// Starguider reports are available since 2004/11/17.
// The nomnial pointing position is available since 2005/03/22
// The sky brightness and the number of identified stars since 2005/03/17
//
// Position at which the starguider camera is pointing in real:
//       pointing position = nominal position - dev
//
// Class Version 1:
// ----------------
//  + Double_t fDevAz;          // [arcmin]   azimuth mispointing
//  + Double_t fDevZd;          // [arcmin]   zenith  mispointing
//
//
// Class Version 2:
// ----------------
//  + Double_t fNominalZd;          // [deg] Nominal zenith distance
//  + Double_t fNominalAz;          // [deg] Nominal azimuth
//
//  + Float_t  fCameraCenterX;      // [CCD pix] PMT Camera center found
//  + Float_t  fCameraCenterY;      // [CCD pix] PMT Camera center found
//
//  + UInt_t   fNumIdentifiedStars; // Number of stars identified by starguider algorithm
//
//  + Double_t fSkyBrightness;      // [au] Sky Brightness as calcualted from the CCD image
//  + Double_t fMjd;                // Modified Julian Date matching the nominal position
//
//
// Class Version 3:
// ----------------
//  + UInt_t   fNumCorrelatedStars; // Number of correlated stars identified by starguider algorithm
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportStarguider.h"

#include "MLogManip.h"

#include "MAstro.h"

ClassImp(MReportStarguider);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Initialize identifier to "STARG-REPORT"
//
MReportStarguider::MReportStarguider() : MReport("STARG-REPORT")/*,
    fDevAz(0), fDevZd(0), fNominalZd(0), fNominalAz(0),
    fCameraCenterX(0), fCameraCenterY(0), fNumIdentifiedStars(0),
    fNumCorrelatedStars(0), fSkyBrightness(0)*/
{
    fName  = "MReportStarguider";
    fTitle = "Class for STARG-REPORT information (telescope mispointing)";

    Clear();
}

// --------------------------------------------------------------------------
//
// Interprete the body of the STARG-REPORT string
//
void MReportStarguider::Clear(Option_t *o)
{
    fDevAz              = 0;
    fDevZd              = 0;

    fNominalZd          = 0;
    fNominalAz          = 0;

    fCameraCenterX      = 0;
    fCameraCenterY      = 0;

    fNumIdentifiedStars = 0;
    fNumCorrelatedStars = 0;

    fSkyBrightness      = 0;
}

// --------------------------------------------------------------------------
//
// Interprete the body of the STARG-REPORT string
//
Int_t MReportStarguider::InterpreteBody(TString &str, Int_t ver)
{
    Int_t len;
    Int_t n=sscanf(str.Data(), "%lf %lf %n", &fDevZd, &fDevAz, &len);
    if (n!=2)
    {
        *fLog << warn << "WARNING - Not enough arguments." << endl;
        return kCONTINUE;
    }

    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    if (ver<200503170)
    {
        // Fix a problem with the units
        fDevAz *= 60./TMath::RadToDeg();
        fDevZd *= 60./TMath::RadToDeg();
        return str.IsNull() ? kTRUE : kCONTINUE;
    }

    MAstro::String2Angle(str, fNominalZd);   // Nom Zd
    MAstro::String2Angle(str, fNominalAz);   // Nom Az

    if (ver<200503220)
    {
        // Until a fix in the software the written position was nonsense
        fNominalZd = 0;
        fNominalAz = 0;
    }

    n=sscanf(str.Data(), "%f %f %d %lf %lf %n",
             &fCameraCenterX, &fCameraCenterY, &fNumIdentifiedStars,
             &fSkyBrightness, &fMjd, &len);
    if (n!=5)
    {
        *fLog << warn << "WARNING - Not enough arguments." << endl;
        return kCONTINUE;
    }

    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    // Seems that hasn't yet been implemented
    if ((ver>=200508290 && ver<200509300) ||
        (ver>=200603080))
    {          
        // For the moment this are only placeholders....
        Float_t dx, dy;
        n=sscanf(str.Data(), "%f %f %n", &dx, &dy, &len);
        if (n!=2 && ((n!=0&&n!=EOF) || ver!=200603080))
        {
            *fLog << warn << "WARNING - Not enough arguments." << endl;
            return kCONTINUE;
        }

        str.Remove(0, len);
        str = str.Strip(TString::kBoth);
    }

    if (ver>=200605080)
    {
        fNumCorrelatedStars = fNumIdentifiedStars;
        n=sscanf(str.Data(), "%df %n", &fNumIdentifiedStars, &len);
        if (n!=1)
        {
            *fLog << warn << "WARNING - Not enough arguments." << endl;
            *fLog << str << endl;
            return kCONTINUE;
        }

        str.Remove(0, len);
        str = str.Strip(TString::kBoth);
    }

    return str.IsNull() ? kTRUE : kCONTINUE;
}

Double_t MReportStarguider::GetDevAbs() const
{
    // Crosscheck SIGN!!!!
    return MAstro::GetDevAbs(fNominalZd, fNominalZd-fDevZd/60, fDevAz/60)*60;
}

void MReportStarguider::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << " DevZd=" << fDevZd << " DevAz=" << fDevAz << endl;
    *fLog << " NominalZd=" << fNominalZd << " NominalAz=" << fDevAz << " MJD=" << fMjd << endl;
    *fLog << " CameraCenterX=" << fCameraCenterX << " CameraCenterY=" << fCameraCenterY << endl;
    *fLog << " NumIdentifiedStars=" << fNumIdentifiedStars << endl;
    *fLog << " NumIdentifiedStars=" << fNumIdentifiedStars << " SkyBrightness=" << fSkyBrightness << endl;
}
