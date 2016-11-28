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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MAstro
// ------
//
////////////////////////////////////////////////////////////////////////////
#include "MAstro.h"

#include <math.h>     // fmod on darwin

#include <iostream>

#include <TArrayD.h>  // TArrayD
#include <TVector3.h> // TVector3

#include "MTime.h"    // MTime::GetGmst
#include "MString.h"

#include "MAstroCatalog.h" // FIXME: replace by MVector3!

using namespace std;

ClassImp(MAstro);

const Double_t MAstro::kSynMonth = 29.53058868; // synodic month (new Moon to new Moon)
const Double_t MAstro::kEpoch0   = 44240.37917; // First full moon after 1980/1/1

Double_t MAstro::RadToHor()
{
    return 24/TMath::TwoPi();
}

Double_t MAstro::HorToRad()
{
    return TMath::TwoPi()/24;
}

Double_t MAstro::Trunc(Double_t val)
{
    // dint(A) - truncate to nearest whole number towards zero (double)
    return val<0 ? TMath::Ceil(val) : TMath::Floor(val);
}

Double_t MAstro::Round(Double_t val)
{
    // dnint(A) - round to nearest whole number (double)
    return val<0 ? TMath::Ceil(val-0.5) : TMath::Floor(val+0.5);
}

Double_t MAstro::Hms2Sec(Int_t deg, UInt_t min, Double_t sec, Char_t sgn)
{
    const Double_t rc = TMath::Sign((60.0 * (60.0 * (Double_t)TMath::Abs(deg) + (Double_t)min) + sec), (Double_t)deg);
    return sgn=='-' ? -rc : rc;
}

Double_t MAstro::Dms2Rad(Int_t deg, UInt_t min, Double_t sec, Char_t sgn)
{
    // pi/(180*3600):  arcseconds to radians
    //#define DAS2R 4.8481368110953599358991410235794797595635330237270e-6
    return Hms2Sec(deg, min, sec, sgn)*TMath::Pi()/(180*3600)/**DAS2R*/;
}

Double_t MAstro::Hms2Rad(Int_t hor, UInt_t min, Double_t sec, Char_t sgn)
{
    // pi/(12*3600):  seconds of time to radians
//#define DS2R 7.2722052166430399038487115353692196393452995355905e-5
    return Hms2Sec(hor, min, sec, sgn)*TMath::Pi()/(12*3600)/**DS2R*/;
}

Double_t MAstro::Dms2Deg(Int_t deg, UInt_t min, Double_t sec, Char_t sgn)
{
    return Hms2Sec(deg, min, sec, sgn)/3600.;
}

Double_t MAstro::Hms2Deg(Int_t hor, UInt_t min, Double_t sec, Char_t sgn)
{
    return Hms2Sec(hor, min, sec, sgn)/240.;
}

Double_t MAstro::Dms2Hor(Int_t deg, UInt_t min, Double_t sec, Char_t sgn)
{
    return Hms2Sec(deg, min, sec, sgn)/54000.;
}

Double_t MAstro::Hms2Hor(Int_t hor, UInt_t min, Double_t sec, Char_t sgn)
{
    return Hms2Sec(hor, min, sec, sgn)/3600.;
}

void MAstro::Day2Hms(Double_t day, Char_t &sgn, UShort_t &hor, UShort_t &min, UShort_t &sec)
{
    /* Handle sign */
    sgn = day<0?'-':'+';

    /* Round interval and express in smallest units required */
    Double_t a = Round(86400. * TMath::Abs(day)); // Days to seconds

    /* Separate into fields */
    const Double_t ah = Trunc(a/3600.);
    a -= ah * 3600.;
    const Double_t am = Trunc(a/60.);
    a -= am * 60.;
    const Double_t as = Trunc(a);

    /* Return results */
    hor = (UShort_t)ah;
    min = (UShort_t)am;
    sec = (UShort_t)as;
}

void MAstro::Rad2Hms(Double_t rad, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Day2Hms(rad/(TMath::Pi()*2), sgn, deg, min, sec);
}

void MAstro::Rad2Dms(Double_t rad, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Rad2Hms(rad*15, sgn, deg, min, sec);
}

void MAstro::Deg2Dms(Double_t d, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Day2Hms(d/24, sgn, deg, min, sec);
}

void MAstro::Deg2Hms(Double_t d, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Day2Hms(d/360, sgn, deg, min, sec);
}

void MAstro::Hor2Dms(Double_t h, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Day2Hms(h*15/24, sgn, deg, min, sec);
}

void MAstro::Hor2Hms(Double_t h, Char_t &sgn, UShort_t &deg, UShort_t &min, UShort_t &sec)
{
    Day2Hms(h/24, sgn, deg, min, sec);
}

void MAstro::Day2Hm(Double_t day, Char_t &sgn, UShort_t &hor, Double_t &min)
{
    /* Handle sign */
    sgn = day<0?'-':'+';

    /* Round interval and express in smallest units required */
    Double_t a = Round(86400. * TMath::Abs(day)); // Days to seconds

    /* Separate into fields */
    const Double_t ah = Trunc(a/3600.);
    a -= ah * 3600.;

    /* Return results */
    hor = (UShort_t)ah;
    min = a/60.;
}

void MAstro::Rad2Hm(Double_t rad, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Day2Hm(rad/(TMath::Pi()*2), sgn, deg, min);
}

void MAstro::Rad2Dm(Double_t rad, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Rad2Hm(rad*15, sgn, deg, min);
}

void MAstro::Deg2Dm(Double_t d, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Day2Hm(d/24, sgn, deg, min);
}

void MAstro::Deg2Hm(Double_t d, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Rad2Hm(d/360, sgn, deg, min);
}

void MAstro::Hor2Dm(Double_t h, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Day2Hm(h*15/24, sgn, deg, min);
}

void MAstro::Hor2Hm(Double_t h, Char_t &sgn, UShort_t &deg, Double_t &min)
{
    Day2Hm(h/24, sgn, deg, min);
}

TString MAstro::GetStringDeg(Double_t deg, const char *fmt)
{
    Char_t sgn;
    UShort_t d, m, s;
    Deg2Dms(deg, sgn, d, m, s);

    return MString::Format(fmt, sgn, d, m ,s);
}

TString MAstro::GetStringHor(Double_t deg, const char *fmt)
{
    Char_t sgn;
    UShort_t h, m, s;
    Hor2Hms(deg, sgn, h, m, s);

    return MString::Format(fmt, sgn, h, m ,s);
}

// --------------------------------------------------------------------------
//
// Interpretes a string ' - 12 30 00.0' or '+ 12 30 00.0'
// as floating point value -12.5 or 12.5. If interpretation is
// successfull kTRUE is returned, otherwise kFALSE. ret is not
// touched if interpretation was not successfull. The successfull
// interpreted part is removed from the TString.
//
Bool_t MAstro::String2Angle(TString &str, Double_t &ret)
{
    Char_t  sgn;
    Int_t   d, len;
    UInt_t  m;
    Float_t s;

    // Skip whitespaces before %c and after %f
    int n=sscanf(str.Data(), " %c %d %d %f %n", &sgn, &d, &m, &s, &len);

    if (n!=4 || (sgn!='+' && sgn!='-'))
        return kFALSE;

    str.Remove(0, len);

    ret = Dms2Deg(d, m, s, sgn);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interpretes a string '-12:30:00.0', '12:30:00.0' or '+12:30:00.0'
// as floating point value -12.5, 12.5 or 12.5. If interpretation is
// successfull kTRUE is returned, otherwise kFALSE. ret is not
// touched if interpretation was not successfull.
//
Bool_t MAstro::Coordinate2Angle(const TString &str, Double_t &ret)
{
    Char_t  sgn = str[0]=='-' ? '-' : '+';
    Int_t   d;
    UInt_t  m;
    Float_t s;

    const int n=sscanf(str[0]=='+'||str[0]=='-' ? str.Data()+1 : str.Data(), "%d:%d:%f", &d, &m, &s);

    if (n!=3)
        return kFALSE;

    ret = Dms2Deg(d, m, s, sgn);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns val=-12.5 as string '-12:30:00'
//
TString MAstro::Angle2Coordinate(Double_t val)
{
    Char_t  sgn;
    UShort_t d,m,s;

    Deg2Dms(val, sgn, d, m, s);

    return MString::Format("%c%02d:%02d:%02d", sgn, d, m, s);
}

// --------------------------------------------------------------------------
//
//  Return year y, month m and day d corresponding to Mjd.
//
void MAstro::Mjd2Ymd(UInt_t mjd, UShort_t &y, Byte_t &m, Byte_t &d)
{
    // Express day in Gregorian calendar
    const ULong_t jd   = mjd + 2400001;
    const ULong_t n4   = 4*(jd+((6*((4*jd-17918)/146097))/4+1)/2-37);
    const ULong_t nd10 = 10*(((n4-237)%1461)/4)+5;

    y = n4/1461L-4712;
    m = ((nd10/306+2)%12)+1;
    d = (nd10%306)/10+1;
}

// --------------------------------------------------------------------------
//
//  Return Mjd corresponding to year y, month m and day d.
//
Int_t MAstro::Ymd2Mjd(UShort_t y, Byte_t m, Byte_t d)
{
    // Month lengths in days
    static int months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Validate month
    if (m<1 || m>12)
        return -1;

    // Allow for leap year
    months[1] = (y%4==0 && (y%100!=0 || y%400==0)) ? 29 : 28;

    // Validate day
    if (d<1 || d>months[m-1])
        return -1;

    // Precalculate some values
    const Byte_t  lm = 12-m;
    const ULong_t lm10 = 4712 + y - lm/10;

    // Perform the conversion
    return 1461L*lm10/4 + (306*((m+9)%12)+5)/10 - (3*((lm10+188)/100))/4 + d - 2399904;
}

// --------------------------------------------------------------------------
//
// Convert a mjd to a number yymmdd. The century is just cut away, e.g.
//   54393 -->  71020   (2007/10/20)
//   50741 --> 971020   (1997/10/20)
//   17868 -->  71020   (1907/10/20)
//
UInt_t MAstro::Mjd2Yymmdd(UInt_t mjd)
{
    UShort_t y;
    Byte_t m, d;
    Mjd2Ymd(mjd, y, m, d);

    return d + m*100 + (y%100)*10000;
}

// --------------------------------------------------------------------------
//
// Convert a yymmdd number to mjd. The century is defined as 2000 for
// yy<70, 1900 otherwise.
//    71020 --> 54393 (2007/10/20)
//   971020 --> 50741 (1997/10/20)
//
UInt_t MAstro::Yymmdd2Mjd(UInt_t yymmdd)
{
    const Byte_t   dd =  yymmdd%100;
    const Byte_t   mm = (yymmdd/100)%100;
    const UShort_t yy = (yymmdd/10000)%100;

    return Ymd2Mjd(yy + (yy<70 ? 2000 : 1900), mm, dd);
}

// --------------------------------------------------------------------------
//
//  theta0, phi0    [rad]: polar angle/zenith distance, azimuth of 1st object
//  theta1, phi1    [rad]: polar angle/zenith distance, azimuth of 2nd object
//  AngularDistance [rad]: Angular distance between two objects
//
Double_t MAstro::AngularDistance(Double_t theta0, Double_t phi0, Double_t theta1, Double_t phi1)
{
    // v1 = theta0/phi0 --> theta0 / 0
    // v2 = theta1/phi1 --> theta1 / phi1-phi0
    // acos(alpha) = v1*v2/|v1||v2|    |v1|*|v2|=1
    const Double_t sin2 = TMath::Sin(theta0)*TMath::Sin(theta1);
    const Double_t cos2 = TMath::Cos(theta0)*TMath::Cos(theta1);

    return TMath::ACos(sin2*TMath::Cos(phi1-phi0) + cos2);
    /*
     TVector3 v0;
     v0.SetMagThetaPhi(1, theta0, phi0);

     TVector3 v1;
     v1.SetMagThetaPhi(1, theta0, phi0);

     return v0.Angle(v1);
     */
}

// --------------------------------------------------------------------------
//
// Calls MTime::GetGmst() Better use MTime::GetGmst() directly
//
Double_t MAstro::UT2GMST(Double_t ut1)
{
    return MTime(ut1).GetGmst();
}

// --------------------------------------------------------------------------
//
// RotationAngle 
// 
// calculates the angle for the rotation of the sky coordinate system
// with respect to the local coordinate system. This is identical
// to the rotation angle of the sky image in the camera.
//
//  sinl  [rad]: sine of observers latitude
//  cosl  [rad]: cosine of observers latitude
//  theta [rad]: polar angle/zenith distance
//  phi   [rad]: rotation angle/azimuth
//
// Return sin/cos component of angle
//
// The convention is such, that the rotation angle is -pi/pi if
// right ascension and local rotation angle are counted in the
// same direction, 0 if counted in the opposite direction.
//
// (In other words: The rotation angle is 0 when the source culminates)
//
// Using vectors it can be done like:
//    TVector3 v, p;
//    v.SetMagThetaPhi(1, theta, phi);
//    p.SetMagThetaPhi(1, TMath::Pi()/2-latitude, 0);
//    v = v.Cross(l)); 
//    v.RotateZ(-phi);
//    v.Rotate(-theta)
//    rho = TMath::ATan2(v(2), v(1));
//
// For more information see TDAS 00-11, eqs. (18) and (20)
//
void MAstro::RotationAngle(Double_t sinl, Double_t cosl, Double_t theta, Double_t phi, Double_t &sin, Double_t &cos)
{
    const Double_t sint = TMath::Sin(theta);
    const Double_t cost = TMath::Cos(theta);

    const Double_t snlt = sinl*sint;
    const Double_t cslt = cosl*cost;

    const Double_t sinp = TMath::Sin(phi);
    const Double_t cosp = TMath::Cos(phi);

    const Double_t v1 = sint*sinp;
    const Double_t v2 = cslt - snlt*cosp;

    const Double_t denom = TMath::Sqrt(v1*v1 + v2*v2);

    sin =   cosl*sinp      / denom; // y-component
    cos = (snlt-cslt*cosp) / denom; // x-component
}

// --------------------------------------------------------------------------
//
// RotationAngle 
// 
// calculates the angle for the rotation of the sky coordinate system
// with respect to the local coordinate system. This is identical
// to the rotation angle of the sky image in the camera.
//
//  sinl  [rad]: sine of observers latitude
//  cosl  [rad]: cosine of observers latitude
//  theta [rad]: polar angle/zenith distance
//  phi   [rad]: rotation angle/azimuth
//
// Return angle [rad] in the range -pi, pi
//
// The convention is such, that the rotation angle is -pi/pi if
// right ascension and local rotation angle are counted in the
// same direction, 0 if counted in the opposite direction.
//
// (In other words: The rotation angle is 0 when the source culminates)
//
// Using vectors it can be done like:
//    TVector3 v, p;
//    v.SetMagThetaPhi(1, theta, phi);
//    p.SetMagThetaPhi(1, TMath::Pi()/2-latitude, 0);
//    v = v.Cross(l)); 
//    v.RotateZ(-phi);
//    v.Rotate(-theta)
//    rho = TMath::ATan2(v(2), v(1));
//
// For more information see TDAS 00-11, eqs. (18) and (20)
//
Double_t MAstro::RotationAngle(Double_t sinl, Double_t cosl, Double_t theta, Double_t phi)
{
    const Double_t snlt = sinl*TMath::Sin(theta);
    const Double_t cslt = cosl*TMath::Cos(theta);

    const Double_t sinp = TMath::Sin(phi);
    const Double_t cosp = TMath::Cos(phi);

    return TMath::ATan2(cosl*sinp, snlt-cslt*cosp);
}

// --------------------------------------------------------------------------
//
// Estimates the time at which a source culminates.
//
// ra: right ascension [rad]
// elong: observers longitude [rad]
// mjd: modified julian date (utc)
//
// return time in [-12;12]
//
Double_t MAstro::EstimateCulminationTime(Double_t mjd, Double_t elong, Double_t ra)
{
    // startime at 1.1.2000 for greenwich 0h
    const Double_t gmt0 = 6.664520;

    // difference of startime for greenwich for two calendar days [h]
    const Double_t d0 = 0.06570982224;

    // mjd of greenwich 1.1.2000 0h
    const Double_t mjd0 = 51544;

    // mjd today
    const Double_t mjd1 = TMath::Floor(mjd);

    // scale between star-time and sun-time
    const Double_t scale = 1;//1.00273790926;

    const Double_t UT = (ra-elong)*RadToHor() - (gmt0 + d0 * (mjd1-mjd0))/scale;

    return fmod(2412 + UT, 24) - 12;
}

// --------------------------------------------------------------------------
//
// Kepler - solve the equation of Kepler
//
Double_t MAstro::Kepler(Double_t m, Double_t ecc)
{
    m *= TMath::DegToRad();

    Double_t delta = 0;
    Double_t e = m;
    do {
        delta = e - ecc * sin(e) - m;
        e -= delta / (1 - ecc * cos(e));
    } while (fabs(delta) > 1e-6);

    return e;
}

// --------------------------------------------------------------------------
//
// GetMoonPhase - calculate phase of moon as a fraction:
//  Returns -1 if calculation failed
//
Double_t MAstro::GetMoonPhase(Double_t mjd)
{
    /****** Calculation of the Sun's position. ******/

    // date within epoch
    const Double_t epoch = 44238;       // 1980 January 0.0
    const Double_t day = mjd - epoch;
    if (day<0)
    {
        cout << "MAstro::GetMoonPhase - Day before Jan 1980" << endl;
        return -1;
    }

    // mean anomaly of the Sun
    const Double_t n = fmod(day*360/365.2422, 360);

    const Double_t elonge = 278.833540; // ecliptic longitude of the Sun at epoch 1980.0
    const Double_t elongp = 282.596403; // ecliptic longitude of the Sun at perigee

    // convert from perigee co-ordinates to epoch 1980.0
    const Double_t m = fmod(n + elonge - elongp + 360, 360);

    // solve equation of Kepler
    const Double_t eccent = 0.016718; // eccentricity of Earth's orbit
    const Double_t k   = Kepler(m, eccent);
    const Double_t ec0 = sqrt((1 + eccent) / (1 - eccent)) * tan(k / 2);
    // true anomaly
    const Double_t ec  = 2 * atan(ec0) * TMath::RadToDeg();

    // Sun's geocentric ecliptic longitude
    const Double_t lambdasun = fmod(ec + elongp + 720, 360);


    /****** Calculation of the Moon's position. ******/

    // Moon's mean longitude.
    const Double_t mmlong  = 64.975464;  // moon's mean lonigitude at the epoch
    const Double_t ml      = fmod(13.1763966*day + mmlong + 360, 360);
    // Moon's mean anomaly.
    const Double_t mmlongp = 349.383063; // mean longitude of the perigee at the epoch
    const Double_t mm      = fmod(ml - 0.1114041*day - mmlongp + 720, 360);
    // Evection.
    const Double_t ev   = 1.2739 * sin((2 * (ml - lambdasun) - mm)*TMath::DegToRad());
    // Annual equation.
    const Double_t sinm = TMath::Sin(m*TMath::DegToRad());
    const Double_t ae   = 0.1858 * sinm;
    // Correction term.
    const Double_t a3   = 0.37 * sinm;
    // Corrected anomaly.
    const Double_t mmp  = (mm + ev - ae - a3)*TMath::DegToRad();
    // Correction for the equation of the centre.
    const Double_t mec  = 6.2886 * sin(mmp);
    // Another correction term.
    const Double_t a4   = 0.214 * sin(2 * mmp);
    // Corrected longitude.
    const Double_t lp   = ml + ev + mec - ae + a4;
    // Variation.
    const Double_t v    = 0.6583 * sin(2 * (lp - lambdasun)*TMath::DegToRad());
    // True longitude.
    const Double_t lpp  = lp + v;
    // Age of the Moon in degrees.
    const Double_t age  = (lpp - lambdasun)*TMath::DegToRad();

    // Calculation of the phase of the Moon.
    return (1 - TMath::Cos(age)) / 2;
}

// --------------------------------------------------------------------------
//
// Calculate the Period to which the time belongs to. The Period is defined
// as the number of synodic months ellapsed since the first full moon
// after Jan 1st 1980 (which was @ MJD=44240.37917)
//
Double_t MAstro::GetMoonPeriod(Double_t mjd)
{
    const Double_t et = mjd-kEpoch0; // Elapsed time
    return et/kSynMonth;
}

// --------------------------------------------------------------------------
//
// Convert a moon period back to a mjd
//
// See also
//   MAstro::GetMoonPeriod
//
Double_t MAstro::GetMoonPeriodMjd(Double_t p)
{
    return p*kSynMonth+kEpoch0;
}

// --------------------------------------------------------------------------
//
// To get the moon period as defined for MAGIC observation we take the
// nearest integer mjd, eg:
//   53257.8 --> 53258
//   53258.3 --> 53258
// Which is the time between 12h and 11:59h of the following day. To
// this day-period we assign the moon-period at midnight. To get
// the MAGIC definition we now substract 284.
//
// For MAGIC observation period do eg:
//   GetMagicPeriod(53257.91042)
// or
//   MTime t;
//   t.SetMjd(53257.91042);
//   GetMagicPeriod(t.GetMjd());
// or
//   MTime t;
//   t.Set(2004, 1, 1, 12, 32, 11);
//   GetMagicPeriod(t.GetMjd());
//
// To get a floating point magic period use
//   GetMoonPeriod(mjd)-284
//
Int_t MAstro::GetMagicPeriod(Double_t mjd)
{
    const Double_t mmjd   = (Double_t)TMath::Nint(mjd);
    const Double_t period = GetMoonPeriod(mmjd);

    return (Int_t)TMath::Floor(period)-284;
}

// --------------------------------------------------------------------------
//
// Get the start time (12h noon) of the MAGIC period p.
//
// See also
//   MAstro::GetMagicPeriod
//
Double_t MAstro::GetMagicPeriodStart(Int_t p)
{
    return TMath::Floor(GetMoonPeriodMjd(p+284))+0.5;
}

// --------------------------------------------------------------------------
//
// Returns right ascension and declination [rad] of the sun at the
// given mjd (ra, dec).
//
// returns the mean longitude [rad].
//
// from http://xoomer.alice.it/vtomezzo/sunriset/formulas/index.html
//
Double_t MAstro::GetSunRaDec(Double_t mjd, Double_t &ra, Double_t &dec)
{
    const Double_t T = (mjd-51544.5)/36525;// +  (h-12)/24.0;

    const Double_t T2 = T<0 ? -T*T : T*T;
    const Double_t T3 = T*T*T;

    // Find the ecliptic longitude of the Sun

    // Geometric mean longitude of the Sun
    const Double_t L = 280.46646 + 36000.76983*T + 0.0003032*T2;

    // mean anomaly of the Sun
    Double_t g = 357.52911 + 35999.05029*T - 0.0001537*T2;
    g *= TMath::DegToRad();

    // Longitude of the moon's ascending node
    Double_t omega = 125.04452 - 1934.136261*T + 0.0020708*T2 + T3/450000;
    omega *= TMath::DegToRad();

    const Double_t coso = cos(omega);
    const Double_t sino = sin(omega);

    // Equation of the center
    const Double_t C = (1.914602 - 0.004817*T - 0.000014*T2)*sin(g) +
        (0.019993 - 0.000101*T)*sin(2*g) + 0.000289*sin(3*g);

    // True longitude of the sun
    const Double_t tlong = L + C;

    // Apperent longitude of the Sun (ecliptic)
    Double_t lambda = tlong - 0.00569 - 0.00478*sino;
    lambda *= TMath::DegToRad();

    // Obliquity of the ecliptic
    Double_t obliq = 23.4392911 - 0.01300416667*T - 0.00000016389*T2 + 0.00000050361*T3 + 0.00255625*coso;
    obliq *= TMath::DegToRad();

    // Find the RA and DEC of the Sun
    const Double_t sinl =  sin(lambda);

    ra  = atan2(cos(obliq) * sinl, cos(lambda));
    dec = asin(sin(obliq) * sinl);

    return L*TMath::DegToRad();
}

// --------------------------------------------------------------------------
//
// Returns right ascension and declination [rad] of the moon at the
// given mjd (ra, dec).
//
void MAstro::GetMoonRaDec(Double_t mjd, Double_t &ra, Double_t &dec)
{
    // Mean Moon orbit elements as of 1990.0
    const Double_t l0 = 318.351648 * TMath::DegToRad();
    const Double_t P0 =  36.340410 * TMath::DegToRad();
    const Double_t N0 = 318.510107 * TMath::DegToRad();
    const Double_t i  =   5.145396 * TMath::DegToRad();

    Double_t sunra, sundec, g;
    {
        const Double_t T = (mjd-51544.5)/36525;// +  (h-12)/24.0;
        const Double_t T2 = T<0 ? -T*T : T*T;

        GetSunRaDec(mjd, sunra, sundec);

        // mean anomaly of the Sun
        g = 357.52911 + 35999.05029*T - 0.0001537*T2;
        g *= TMath::DegToRad();
    }

    const Double_t sing   = sin(g)*TMath::DegToRad();

    const Double_t D      = (mjd-47891) * TMath::DegToRad();
    const Double_t l      =    13.1763966*D + l0;
    const Double_t MMoon  = l  -0.1114041*D - P0; // Moon's mean anomaly M
    const Double_t N      = N0 -0.0529539*D;      // Moon's mean ascending node longitude

    const Double_t C      = l-sunra;
    const Double_t Ev     = 1.2739 * sin(2*C-MMoon) * TMath::DegToRad();
    const Double_t Ae     = 0.1858 * sing;
    const Double_t A3     = 0.37   * sing;
    const Double_t MMoon2 = MMoon+Ev-Ae-A3;  // corrected Moon anomaly

    const Double_t Ec     = 6.2886 * sin(MMoon2)  * TMath::DegToRad();  // equation of centre
    const Double_t A4     = 0.214  * sin(2*MMoon2)* TMath::DegToRad();
    const Double_t l2     = l+Ev+Ec-Ae+A4; // corrected Moon's longitude

    const Double_t V      = 0.6583 * sin(2*(l2-sunra)) * TMath::DegToRad();
    const Double_t l3     = l2+V; // true orbital longitude;

    const Double_t N2     = N -0.16*sing;

    ra  = fmod( N2 + atan2( sin(l3-N2)*cos(i), cos(l3-N2) ), TMath::TwoPi() );
    dec = asin(sin(l3-N2)*sin(i) );
}

// --------------------------------------------------------------------------
//
// Return Euqation of time in hours for given mjd
//
Double_t MAstro::GetEquationOfTime(Double_t mjd)
{
    Double_t ra, dec;
    const Double_t L = fmod(GetSunRaDec(mjd, ra, dec), TMath::TwoPi());

    if (L-ra>TMath::Pi())
        ra += TMath::TwoPi();

    return 24*(L - ra)/TMath::TwoPi();
}

// --------------------------------------------------------------------------
//
// Returns noon time (the time of the highest altitude of the sun)
// at the given mjd and at the given observers longitude [deg]
//
// The maximum altitude reached at noon time is
//   altmax = 90.0 + dec - latit;
//   if (dec > latit)
//      altmax = 90.0 + latit - dec;
// dec=Declination of the sun
//
Double_t MAstro::GetNoonTime(Double_t mjd, Double_t longit)
{
    const Double_t equation = GetEquationOfTime(TMath::Floor(mjd));
    return 12. + equation - longit/15;
}

// --------------------------------------------------------------------------
//
// Returns the time (in hours) between noon (the sun culmination)
// and the sun being at height alt[deg] (90=zenith, 0=horizont)
//
//       civil twilight:      0deg to  -6deg
//       nautical twilight:  -6deg to -12deg
//       astronom twilight: -12deg to -18deg
//
// latit is the observers latitude in rad
//
// returns -1 in case the sun doesn't reach this altitude.
// (eg. alt=0: Polarnight or -day)
//
// To get the sun rise/set:
//    double timediff = MAstro::GetTimeFromNoonToAlt(mjd, latit*TMath::DegToRad(), par[0]);
//    double noon     = MAstro::GetNoonTime(mjd, longit);
//    double N        = TMath::Floor(mjd)+noon/24.;
//    double risetime = N-timediff/24.;
//    double settime  = N+timediff/24.;
//
Double_t MAstro::GetTimeFromNoonToAlt(Double_t mjd, Double_t latit, Double_t alt)
{
    Double_t ra, dec;
    GetSunRaDec(mjd, ra, dec);

    const Double_t h = alt*TMath::DegToRad();

    const Double_t arg = (sin(h) - sin(latit)*sin(dec))/(cos(latit)*cos(dec));

    return TMath::Abs(arg)>1 ? -1 : 12*acos(arg)/TMath::Pi();
}

// --------------------------------------------------------------------------
//
// Returns the time of the sunrise/set calculated before and after
// the noon of floor(mjd) (TO BE IMPROVED)
//
// Being longit and latit the longitude and latitude of the observer
// in deg and alt the hight above or below the horizont in deg.
//
//       civil twilight:      0deg to  -6deg
//       nautical twilight:  -6deg to -12deg
//       astronom twilight: -12deg to -18deg
//
// A TArrayD(2) is returned with the the mjd of the sunrise in
// TArray[0] and the mjd of the sunset in TArrayD[1].
//
TArrayD MAstro::GetSunRiseSet(Double_t mjd, Double_t longit, Double_t latit, Double_t alt)
{
    const Double_t timediff = MAstro::GetTimeFromNoonToAlt(mjd, latit*TMath::DegToRad(), alt);
    const Double_t noon     = MAstro::GetNoonTime(mjd, longit);

    const Double_t N = TMath::Floor(mjd)+noon/24.;

    const Double_t rise = timediff<0 ? N-0.5 : N-timediff/24.;
    const Double_t set  = timediff<0 ? N+0.5 : N+timediff/24.;

    TArrayD rc(2);
    rc[0] = rise;
    rc[1] = set;
    return rc;
}

// --------------------------------------------------------------------------
//
// Returns the distance in x,y between two polar-vectors (eg. Alt/Az, Ra/Dec)
// projected on aplain in a distance dist. For Magic this this the distance
// of the camera plain (1700mm) dist also determins the unit in which
// the TVector2 is returned.
//
// v0 is the reference vector (eg. the vector to the center of the camera)
// v1 is the vector to which we determin the distance on the plain
//
//  (see also MStarCamTrans::Loc0LocToCam())
//
TVector2 MAstro::GetDistOnPlain(const TVector3 &v0, TVector3 v1, Double_t dist)
{
    v1.RotateZ(-v0.Phi());
    v1.RotateY(-v0.Theta());
    v1.RotateZ(-TMath::Pi()/2); // exchange x and y
    v1 *= dist/v1.Z();

    return v1.XYvector(); //TVector2(v1.Y(), -v1.X());//v1.XYvector();
}

// --------------------------------------------------------------------------
//
// Calculate the absolute misspointing from the nominal zenith angle nomzd
// and the deviations in zd (devzd) and az (devaz).
// All values given in deg, the return value, too.
//
Double_t MAstro::GetDevAbs(Double_t nomzd, Double_t meszd, Double_t devaz)
{
    nomzd *= TMath::DegToRad();
    meszd *= TMath::DegToRad();
    devaz *= TMath::DegToRad();

    const double x = sin(meszd) * sin(nomzd) * cos(devaz);
    const double y = cos(meszd) * cos(nomzd);

    return acos(x + y) * TMath::RadToDeg();
}

// --------------------------------------------------------------------------
//
// Returned is the offset (number of days) which must be added to
// March 1st of the given year, eg:
//
//    Int_t offset = GetDayOfEaster(2004);
//
//    MTime t;
//    t.Set(year, 3, 1);
//    t.SetMjd(t.GetMjd()+offset);
//
//    cout << t << endl;
//
//  If the date coudn't be calculated -1 is returned.
//
//  The minimum value returned is 21 corresponding to March 22.
//  The maximum value returned is 55 corresponding to April 25.
//
// --------------------------------------------------------------------------
//
// Gauss'sche Formel zur Berechnung des Osterdatums
// Wann wird Ostern gefeiert? Wie erfährt man das Osterdatum für ein
// bestimmtes Jahr, ohne in einen Kalender zu schauen?
//
// Ostern ist ein "bewegliches" Fest. Es wird am ersten Sonntag nach dem
// ersten Frühlingsvollmond gefeiert. Damit ist der 22. März der früheste
// Termin, der 25. April der letzte, auf den Ostern fallen kann. Von
// diesem Termin hängen auch die Feste Christi Himmelfahrt, das 40 Tage
// nach Ostern, und Pfingsten, das 50 Tage nach Ostern gefeiert wird, ab.
//
// Von Carl Friedrich Gauß (Mathematiker, Astronom und Physiker;
// 1777-1855) stammt ein Algorithmus, der es erlaubt ohne Kenntnis des
// Mondkalenders die Daten der Osterfeste für die Jahre 1700 bis 2199 zu
// bestimmen.
//
// Gib eine Jahreszahl zwischen 1700 und 2199 ein:
//
// Und so funktioniert der Algorithmus:
//
// Es sei:
//
// J	die Jahreszahl
// a	der Divisionsrest von	J/19
// b	der Divisionsrest von	J/4
// c	der Divisionsrest von	J/7
// d	der Divisionsrest von	(19*a + M)/30
// e	der Divisionsrest von	(2*b + 4*c + 6*d + N)/7
//
// wobei M und N folgende Werte annehmen:
//
// für die Jahre	M	N
//  1583-1599          22       2
//  1600-1699          22       2
//  1700-1799          23       3
//  1800-1899          23       4
//  1900-1999          24       5
//  2000-2099          24       5
//  2100-2199          24       6
//  2200-2299          25       0
//  2300-2399          26       1
//  2400-2499          25       1
//
// Dann fällt Ostern auf den
// (22 + d + e)ten März
//
// oder den
// (d + e - 9)ten April
//
// Beachte:
// Anstelle des 26. Aprils ist immer der 19. April zu setzen,
// anstelle des 25. Aprils immer dann der 18. April, wenn d=28 und a>10.
//
// Literatur:
// Schüler-Rechenduden
// Bibliographisches Institut
// Mannheim, 1966
//
// --------------------------------------------------------------------------
//
// Der Ostersonntag ist ein sog. unregelmäßiger Feiertag. Alle anderen
// unregelmäßigen Feiertage eines Jahres leiten sich von diesem Tag ab:
//
//    * Aschermittwoch ist 46 Tage vor Ostern.
//    * Pfingsten ist 49 Tage nach Ostern.
//    * Christi Himmelfahrt ist 10 Tage vor Pfingsten.
//    * Fronleichnam ist 11 Tage nach Pfingsten.
//
// Man muß also nur den Ostersonntag ermitteln, um alle anderen
// unregelmäßigen Feiertage zu berechnen. Doch wie geht das?
//
// Dazu etwas Geschichte:
//
// Das 1. Kirchenkonzil im Jahre 325 hat festgelegt:
//
//    * Ostern ist stets am ersten Sonntag nach dem ersten Vollmond des
//      Frühlings.
//    * Stichtag ist der 21. März, die "Frühlings-Tagundnachtgleiche".
//
// Am 15.10.1582 wurde von Papst Gregor XIII. der bis dahin gültige
// Julianische Kalender reformiert. Der noch heute gültige Gregorianische
// Kalender legt dabei folgendes fest:
//
// Ein Jahr hat 365 Tage und ein Schaltjahr wird eingefügt, wenn das Jahr
// durch 4 oder durch 400, aber nicht durch 100 teilbar ist. Hieraus
// ergeben sich die zwei notwendigen Konstanten, um den Ostersonntag zu
// berechnen:
//
//   1. Die Jahreslänge von und bis zum Zeitpunkt der
//      Frühlings-Tagundnachtgleiche: 365,2422 mittlere Sonnentage
//   2. Ein Mondmonat: 29,5306 mittlere Sonnentage
//
// Mit der "Osterformel", von Carl Friedrich Gauß (1777-1855) im Jahre 1800
// entwickelt, läßt sich der Ostersonntag für jedes Jahr von 1583 bis 8202
// berechnen.
//
// Der früheste mögliche Ostertermin ist der 22. März. (Wenn der Vollmond
// auf den 21. März fällt und der 22. März ein Sonntag ist.)
//
// Der späteste mögliche Ostertermin ist der 25. April. (Wenn der Vollmond
// auf den 21. März fällt und der 21. März ein Sonntag ist.)
//
Int_t MAstro::GetEasterOffset(UShort_t year)
{
    if (year<1583 || year>2499)
    {
        cout << "MAstro::GetDayOfEaster - Year " << year << " not between 1700 and 2199" << endl;
        return -1;
    }

    Int_t M=0;
    Int_t N=0;
    switch (year/100)
    {
    case 15: 
    case 16: M=22; N=2; break;
    case 17: M=23; N=3; break;
    case 18: M=23; N=4; break;
    case 19: 
    case 20: M=24; N=5; break;
    case 21: M=24; N=6; break;
    case 22: M=25; N=0; break;
    case 23: M=26; N=1; break;
    case 24: M=25; N=1; break;
    }

    const Int_t a = year%19;
    const Int_t b = year%4;
    const Int_t c = year%7;
    const Int_t d = (19*a + M)%30;
    const Int_t e = (2*b + 4*c + 6*d + N)%7;

    if (e==6 && d==28 && a>10)
        return 48;

    if (d+e==35)
        return 49;

    return d + e + 21;
}
