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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MPointingPos
//
// In this container we store the corrected pointing position of the
// telscope. The pointing coordinates are read into MReportDrive together
// with its time.
//
// MPointingPosCalc afterwards calculates corrections and checks for the
// cosistency of the coordinates. The result (the real coordinates)
// are stored in this container. No further correction should be necessary
// using MPointingPos.
//
// If you need the rotation angle of the starfield in the camera you can
// get it from here.
//
/////////////////////////////////////////////////////////////////////////////
#include "MPointingPos.h"

#include "fits.h"

#include "MLog.h"

#include "MTime.h"
#include "MAstro.h"
#include "MString.h"
#include "MObservatory.h"
#include "MPointingDev.h"
#include "MAstroSky2Local.h"

ClassImp(MPointingPos);

using namespace std;

const TString MPointingPos::gsDefName  = "MPointingPos";
const TString MPointingPos::gsDefTitle = "Container storing the (corrected) telescope pointing position";

Double_t MPointingPos::GetZdRad() const
{
    return fZd*TMath::DegToRad();
}

Double_t MPointingPos::GetAzRad() const
{
    return fAz*TMath::DegToRad();
}

Double_t MPointingPos::GetRaRad() const
{
    return fRa*TMath::DegToRad()*15;
}

Double_t MPointingPos::GetDecRad() const
{
    return fDec*TMath::DegToRad();
}

// --------------------------------------------------------------------------
//
// Get the corresponding rotation angle of the sky coordinate system
// seen with an Alt/Az telescope calculated from the stored local
// (Zd/Az) coordinates.
//
// Return angle [rad] in the range -pi, pi
//
// For more information see MAstro::RotationAngle
//
Double_t MPointingPos::RotationAngle(const MObservatory &o) const
{
    return o.RotationAngle(fZd*TMath::DegToRad(), fAz*TMath::DegToRad());
}

// --------------------------------------------------------------------------
//
// Get the corresponding rotation angle of the sky coordinate system
// seen with an Alt/Az telescope calculated from the stored sky
// (Ra/Dec) coordinates.
//
// Return angle [rad] in the range -pi, pi
//
// For more information see MAstro::RotationAngle
//
Double_t MPointingPos::RotationAngle(const MObservatory &o, const MTime &t, const MPointingDev *dev) const
{
    return dev ?
        MAstroSky2Local(t, o).RotationAngle(GetRaRad(), GetDecRad(), dev->GetDevZdRad(), dev->GetDevAzRad()):
        MAstroSky2Local(t, o).RotationAngle(GetRaRad(), GetDecRad());
}

TString MPointingPos::GetString(Option_t *o) const
{
    TString opt(o);

    if (opt.IsNull())
        opt = "radeczdaz";

    TString rc;

    if (opt.Contains("ra", TString::kIgnoreCase))
        rc += MString::Format(" Ra=%s", MAstro::GetStringHor(fRa).Data());

    if (opt.Contains("ha", TString::kIgnoreCase))
        rc += MString::Format(" Ha=%s", MAstro::GetStringHor(fHa).Data());

    if (opt.Contains("dec", TString::kIgnoreCase))
        rc += MString::Format(" Dec=%s", MAstro::GetStringDeg(fDec).Data());

    if (opt.Contains("zd", TString::kIgnoreCase))
        rc += MString::Format(" Zd=%s", MAstro::GetStringDeg(fZd).Data());

    if (opt.Contains("az", TString::kIgnoreCase))
        rc += MString::Format(" Az=%s", MAstro::GetStringDeg(fAz).Data());

    if (fTitle!=gsDefTitle)
        rc += MString::Format(" <%s>", fTitle.Data());

    return rc.Strip(TString::kBoth);
}

void MPointingPos::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": " << GetString(o) << endl;
}

Bool_t MPointingPos::SetupFits(fits &fin)
{
    if (!fin.SetRefAddress(Form("%s.fZd",  fName.Data()), fZd))  return kFALSE;
    if (!fin.SetRefAddress(Form("%s.fAz",  fName.Data()), fAz))  return kFALSE;
    if (!fin.SetRefAddress(Form("%s.fRa",  fName.Data()), fRa))  return kFALSE;
    if (!fin.SetRefAddress(Form("%s.fDec", fName.Data()), fDec)) return kFALSE;
    if (!fin.SetRefAddress(Form("%s.fHa",  fName.Data()), fHa))  return kFALSE;

    return kTRUE;
}
