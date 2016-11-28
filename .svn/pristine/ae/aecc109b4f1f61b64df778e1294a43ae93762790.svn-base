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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTime
//
// A generalized MARS time stamp.
//
//
// We do not use floating point values here, because of several reasons:
//  - having the times stored in integers only is more accurate and
//    more reliable in comparison conditions
//  - storing only integers gives similar bit-pattern for similar times
//    which makes compression (eg gzip algorithm in TFile) more
//    successfull
//
// Note, that there are many conversion function converting the day time
// into a readable string. Also a direct interface to SQL time strings
// is available.
//
// If you are using MTime containers as axis lables in root histograms
// use GetAxisTime(). Make sure that you use the correct TimeFormat
// on your TAxis (see GetAxisTime())
//
//
// WARNING: Be carefull changing this class. It is also used in the
//          MAGIC drive software cosy as VERY IMPORTANT stuff!
//
// Remarke: If you encounter strange behaviour, check the casting.
//          Note, that on Linux machines ULong_t and UInt_t is the same.
//
//
// Version 1:
// ----------
//  - first version
//
// Version 2:
// ----------
//  - removed fTimeStamp[2]
//
// Version 3:
// ----------
//  - removed fDurtaion - we may put it back when it is needed
//  - complete rewrite of the data members (old ones completely replaced)
//
/////////////////////////////////////////////////////////////////////////////
#include "MTime.h"

#include <iomanip>

#ifndef __USE_XOPEN
#define __USE_XOPEN // on some systems needed for strptime
#endif

#include <time.h>     // struct tm
#include <sys/time.h> // struct timeval

#include <TTime.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MAstro.h"
#include "MString.h"

ClassImp(MTime);

using namespace std;

const UInt_t MTime::kHour   = 3600000;         // [ms] one hour
const UInt_t MTime::kDay    = MTime::kHour*24; // [ms] one day
const UInt_t MTime::kDaySec = 3600*24;         // [s] one day

// --------------------------------------------------------------------------
//
// Constructor. Calls SetMjd(d) for d>0 in all other cases the time
// is set to the current UTC time.
//
MTime::MTime(Double_t d)
{
    Init(0, 0);
    if (d<=0)
        Now();
    else
        d>2400000 ? SetJD(d) : SetMjd(d);
}

// --------------------------------------------------------------------------
//
// Constructor. Calls Set(y, m, d, h, min, s, ms, ns).
// To check validity test for (*this)==MTime()
//
MTime::MTime(UShort_t y, Byte_t m, Byte_t d, Byte_t h, Byte_t min, Byte_t s, UShort_t ms, UInt_t ns)
{
    Set(y, m, d, h, min, s, ms, ns);
}

// --------------------------------------------------------------------------
//
// Return date as year(y), month(m), day(d)
//
void MTime::GetDate(UShort_t &y, Byte_t &m, Byte_t &d) const
{
    MAstro::Mjd2Ymd((Long_t)fTime<0?fMjd-1:fMjd, y, m, d);
}

// --------------------------------------------------------------------------
//
// Return date as year(y), month(m), day(d). If the time is afternoon
// (>=13:00:00) the date of the next day is returned.
//
void MTime::GetDateOfSunrise(UShort_t &y, Byte_t &m, Byte_t &d) const
{
    MAstro::Mjd2Ymd(fMjd, y, m, d);
}

// --------------------------------------------------------------------------
//
// Return date as year(y), month(m), day(d). If the time is afternoon
// (>=13:00:00) the date of the next day is returned.
//
MTime MTime::GetDateOfSunrise() const
{
    UShort_t y;
    Byte_t m;
    Byte_t d;

    MAstro::Mjd2Ymd(fMjd, y, m, d);

    return MTime(y, m, d);
}

// --------------------------------------------------------------------------
//
// GetMoonPhase - calculate phase of moon as a fraction:
//  Returns -1 if calculation failed
//
//  see MAstro::GetMoonPhase
//
Double_t MTime::GetMoonPhase() const
{
    return MAstro::GetMoonPhase(GetMjd());
}

// --------------------------------------------------------------------------
//
// Calculate the Period to which the time belongs to. The Period is defined
// as the number of synodic months ellapsed since the first full moon
// after Jan 1st 1980 (which was @ MJD=44240.37917)
//
//   see MAstro::GetMoonPeriod
//
Double_t MTime::GetMoonPeriod() const
{
    return MAstro::GetMoonPeriod(GetMjd());
}

// --------------------------------------------------------------------------
//
// Set the time corresponding to the moon period.
//
//   see MTime::GetMoonPeriod and MAstro::GetMoonPeriod
//
void MTime::SetMoonPeriod(Double_t p)
{
    SetMjd(MAstro::GetMoonPeriodMjd(p));
}

// --------------------------------------------------------------------------
//
// To get the moon period as defined for MAGIC observation we take the
// nearest integer mjd, eg:
//   53257.8 --> 53258
//   53258.3 --> 53258
 // Which is the time between 13h and 12:59h of the following day. To
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
//   GetMoonPeriod()-284
//
//  see MAstro::GetMagicPeriod
//
Int_t MTime::GetMagicPeriod() const
{
    return MAstro::GetMagicPeriod(GetMjd());
}

// --------------------------------------------------------------------------
//
// Set the start time (noon) of a MAGIC period
//
//   see MTime::GetMagicPeriod and MAstro::GetMagicPeriod
//
void MTime::SetMagicPeriodStart(Int_t p)
{
    SetMjd(MAstro::GetMagicPeriodStart(p));
}

UInt_t MTime::GetNightAsInt() const
{
    UShort_t y;
    Byte_t m, d;

    MAstro::Mjd2Ymd(fMjd+0.5, y, m, d);

    return UInt_t(y)*10000 + UInt_t(m)*100 + d;
}

// --------------------------------------------------------------------------
//
// Return the time in the range [0h, 24h) = [0h0m0.000s - 23h59m59.999s]
//
void MTime::GetTime(Byte_t &h, Byte_t &m, Byte_t &s, UShort_t &ms) const
{
    Long_t tm = GetTime24();
    ms  = tm%1000;            // [ms]
    tm /= 1000;               // [s]
    s   = tm%60;              // [s]
    tm /= 60;                 // [m]
    m   = tm%60;              // [m]
    tm /= 60;                 // [h]
    h   = tm;                 // [h]
}

// --------------------------------------------------------------------------
//
//  Return time as MJD (=JD-24000000.5)
//
Double_t MTime::GetMjd() const
{
    return fMjd+(Double_t)(fNanoSec/1e6+(Long_t)fTime)/kDay;
}

// --------------------------------------------------------------------------
//
// Return a time which is expressed in milliseconds since 01/01/1995 0:00h
// This is compatible with root's definition used in gSystem->Now()
// and TTime.
// Note, gSystem->Now() returns local time, such that it may differ
// from GetRootTime() (if you previously called MTime::Now())
//
TTime MTime::GetRootTime() const
{
    return (ULong_t)((GetMjd()-49718)*kDay);
}

// --------------------------------------------------------------------------
//
// Return a time which is expressed in seconds since 01/01/1970 0:00h
// This is compatible with root's definition used in the constructor of
// TDatime.
//
TDatime MTime::GetRootDatime() const
{
    return TDatime((UInt_t)((GetMjd()-40587)*kDaySec));
}

// --------------------------------------------------------------------------
//
// Return a time which is expressed in seconds since 01/01/1995 0:00h
// This is compatible with root's definition used in TAxis.
// Note, a TAxis always displayes (automatically) given times in
// local time (while here we return UTC) such, that you may encounter
// strange offsets. You can get rid of this by calling:
//    TAxis::SetTimeFormat("[your-format] %F1995-01-01 00:00:00 GMT");
// Note that an offset of 1970-01-01 does NOT work, because root
// assumes that this is local time, tries to convert that to UTC,
// which gives a negative unix time stamp in Europe, and ignores it.
//
Double_t MTime::GetAxisTime() const
{
    return (GetMjd()-49718)*kDaySec;
}

// --------------------------------------------------------------------------
//
// Counterpart of GetAxisTime
//
void MTime::SetAxisTime(Double_t time)
{
    SetMjd(time/kDaySec+49718);
}

// --------------------------------------------------------------------------
//
// Set unix time (seconds since epoche 1970-01-01 00:00)
//
void MTime::SetUnixTime(Long64_t sec, ULong64_t usec)
{
    const Long64_t totsec = sec + usec/1000000;
    const UInt_t   mjd    = totsec/kDaySec + 40587;

    const UInt_t   ms     = totsec%kDaySec*1000 + (usec/1000)%1000;
    const UInt_t   us     = usec%1000;

    SetMjd(mjd, ms, us*1000);
}

// --------------------------------------------------------------------------
//
// Set MTime to time expressed in a 'struct timeval'
//
void MTime::Set(const struct timeval &tv)
{
    SetUnixTime(tv.tv_sec, tv.tv_usec);
}

// --------------------------------------------------------------------------
//
// Set this to the date of easter corresponding to the given year.
// If calculation was not possible it is set to MTime()
//
// The date corresponding to the year of MTime(-1) is returned
// if year<0
//
// The date corresponding to the Year() is returned if year==0.
//
//  for more information see: GetEaster and MAstro::GetEasterOffset()
//
void MTime::SetEaster(Short_t year)
{
    *this = GetEaster(year==0 ? Year() : year);
}

// --------------------------------------------------------------------------
//
// Set a time expressed in MJD, Time of Day (eg. 23:12.779h expressed
// in milliseconds) and a nanosecond part.
//
Bool_t MTime::SetMjd(UInt_t mjd, ULong_t ms, UInt_t ns)
{
    // [d]  mjd  (eg. 52320)
    // [ms] time (eg. 17h expressed in ms)
    // [ns] time (ns part of time)

    if (ms>kDay-1 || ns>999999)
        return kFALSE;

    const Bool_t am = ms<kHour*13; // day of sunrise?

    fMjd     = am ? mjd : mjd + 1;
    fTime    = (Long_t)(am ? ms  : ms-kDay);
    fNanoSec = ns;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set MTime to given MJD (eg. 52080.0915449892)
//
void MTime::SetMjd(Double_t m)
{
    const UInt_t   mjd  = (UInt_t)TMath::Floor(m);
    const Double_t frac = fmod(m, 1)*kDay; // [ms] Fraction of day
    const UInt_t   ns   = (UInt_t)fmod(frac*1e6, 1000000);

    SetMjd(mjd, (ULong_t)TMath::Floor(frac), ns);
}

// --------------------------------------------------------------------------
//
// Set MTime to given time and date
//
Bool_t MTime::Set(UShort_t y, Byte_t m, Byte_t d, Byte_t h, Byte_t min, Byte_t s, UShort_t ms, UInt_t ns)
{
    if (h>23 || min>59 || s>59 || ms>999 || ns>999999)
        return kFALSE;

    const Int_t mjd = MAstro::Ymd2Mjd(y, m, d);
    if (mjd<0)
        return kFALSE;

    const ULong_t tm = ((((h*60+min)*60)+s)*1000)+ms;

    return SetMjd(mjd, tm, ns);
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "dd.mm.yyyy hh:mm:ss.fff"
//
Bool_t MTime::SetString(const char *str)
{
    if (!str)
        return kFALSE;

    UInt_t y, mon, d, h, m, s, ms;
    const Int_t n = sscanf(str, "%02u.%02u.%04u %02u:%02u:%02u.%03u",
                           &d, &mon, &y, &h, &m, &s, &ms);

    return n==7 ? Set(y, mon, d, h, m, s, ms) : kFALSE;
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "yyyy-mm-dd hh:mm:ss"
//
Bool_t MTime::SetSqlDateTime(TString str)
{
    if (str.IsNull())
        return kFALSE;

    str.ReplaceAll("T", "");

    if (str.Contains('.'))
        str += "000";

    UInt_t  y, mon, d, h, m, s, ms;

    if (7==sscanf(str, "%04u-%02u-%02u %02u:%02u:%02u.%03u", &y, &mon, &d, &h, &m, &s, &ms))
        return Set(y, mon, d, h, m, s, ms);

    if (6==sscanf(str, "%04u-%02u-%02u %02u:%02u:%02u", &y, &mon, &d, &h, &m, &s))
        return Set(y, mon, d, h, m, s);

    if (5==sscanf(str, "%04u-%02u-%02u %02u:%02u", &y, &mon, &d, &h, &m))
        return Set(y, mon, d, h, m);

    if (4==sscanf(str, "%04u-%02u-%02u %02u", &y, &mon, &d, &h))
        return Set(y, mon, d, h);

    if (3==sscanf(str, "%04u-%02u-%02u", &y, &mon, &d))
        return Set(y, mon, d);

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "yyyymmddhhmmss"
//
Bool_t MTime::SetSqlTimeStamp(const char *str)
{
    if (!str)
        return kFALSE;

    UInt_t y, mon, d, h, m, s;
    const Int_t n = sscanf(str, "%04u%02u%02u%02u%02u%02u",
                           &y, &mon, &d, &h, &m, &s);

    return n==6 ? Set(y, mon, d, h, m, s) : kFALSE;
}

// --------------------------------------------------------------------------
//
// Set MTime to time expressed as in CT1 PreProc files
//
void MTime::SetCT1Time(UInt_t mjd, UInt_t t1, UInt_t t0)
{
    // int   isecs_since_midday; // seconds passed since midday before sunset (JD of run start)
    // int   isecfrac_200ns;     // fractional part of isecs_since_midday
    // fTime->SetTime(isecfrac_200ns, isecs_since_midday);
    fNanoSec         = (200*t1)%1000000;
    const ULong_t ms = (200*t1)/1000000 + t0+12*kHour;

    fTime = (Long_t)(ms<13*kHour ? ms : ms-kDay);

    fMjd = mjd+1;
}

// --------------------------------------------------------------------------
//
// Set MTime to time expressed as float (yymmdd.ffff)
//  for details see MAstro::Yymmdd2Mjd
//
void MTime::SetCorsikaTime(Float_t t)
{
    const UInt_t   yymmdd = (UInt_t)TMath::Floor(t);
    const UInt_t   mjd    = MAstro::Yymmdd2Mjd(yymmdd);
    const Double_t frac   = fmod(t, 1)*kDay; // [ms] Fraction of day
    const UInt_t   ns     = (UInt_t)fmod(frac*1e6, 1000000);

    SetMjd(mjd, (ULong_t)TMath::Floor(frac), ns);
}

// --------------------------------------------------------------------------
//
// Update the magic time. Make sure, that the MJD is set correctly.
// It must be the MJD of the corresponding night. You can set it
// by Set(2003, 12, 24);
//
// It is highly important, that the time correspoding to the night is
// between 13:00:00.0 (day of dawning) and 12:59:59.999 (day of sunrise)
//
Bool_t MTime::UpdMagicTime(Byte_t h, Byte_t m, Byte_t s, UInt_t ns)
{
    if (h>23 || m>59 || s>59 || ns>999999999)
         return kFALSE;

    const ULong_t tm = ((((h*60+m)*60)+s)*1000)+ns/1000000;

    fTime = (Long_t)(tm<kHour*13 ? tm  : tm-kDay); // day of sunrise?
    fNanoSec = ns%1000000;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Conversion from Universal Time to Greenwich mean sidereal time,
//  with rounding errors minimized.
//
//  The result is the Greenwich Mean Sidereal Time (radians)
//
//  There is no restriction on how the UT is apportioned between the
//  date and ut1 arguments.  Either of the two arguments could, for
//  example, be zero and the entire date+time supplied in the other.
//  However, the routine is designed to deliver maximum accuracy when
//  the date argument is a whole number and the ut argument lies in
//  the range 0 to 1, or vice versa.
//
//  The algorithm is based on the IAU 1982 expression (see page S15 of
//  the 1984 Astronomical Almanac).  This is always described as giving
//  the GMST at 0 hours UT1.  In fact, it gives the difference between
//  the GMST and the UT, the steady 4-minutes-per-day drawing-ahead of
//  ST with respect to UT.  When whole days are ignored, the expression
//  happens to equal the GMST at 0 hours UT1 each day.
//
//  In this routine, the entire UT1 (the sum of the two arguments date
//  and ut) is used directly as the argument for the standard formula.
//  The UT1 is then added, but omitting whole days to conserve accuracy.
//
//  The extra numerical precision delivered by the present routine is
//  unlikely to be important in an absolute sense, but may be useful
//  when critically comparing algorithms and in applications where two
//  sidereal times close together are differenced.
//
Double_t MTime::GetGmst() const
{
    const Double_t ut = (Double_t)(fNanoSec/1e6+(Long_t)fTime)/kDay;

    // Julian centuries since J2000.
    const Double_t t = (ut -(51544.5-fMjd)) / 36525.0;

    // GMST at this UT1
    const Double_t r1 = 24110.54841+(8640184.812866+(0.093104-6.2e-6*t)*t)*t;
    const Double_t r2 = 86400.0*ut;

    const Double_t sum = (r1+r2)/kDaySec;

    return fmod(sum, 1)*TMath::TwoPi();//+TMath::TwoPi();
}

// --------------------------------------------------------------------------
//
// Return Day of the week: Sun=0, Mon=1, ..., Sat=6
//
Byte_t MTime::WeekDay() const
{
    return TMath::FloorNint(GetMjd()+3)%7;
} 

// --------------------------------------------------------------------------
//
// Get the day of the year represented by day, month and year.
// Valid return values range between 1 and 366, where January 1 = 1.
//
UInt_t MTime::DayOfYear() const
{
    MTime jan1st;
    jan1st.Set(Year(), 1, 1);

    const Double_t newyear = TMath::Floor(jan1st.GetMjd());
    const Double_t mjd     = TMath::Floor(GetMjd());

    return TMath::Nint(mjd-newyear)+1;
}

// --------------------------------------------------------------------------
//
// Return Mjd of the first day (a monday) which belongs to week 1 of
// the year give as argument. The returned Mjd might be a date in the
// year before.
//
//  see also MTime::Week()
//
Int_t MTime::GetMjdWeek1(Short_t year)
{
    MTime t;
    t.Set(year, 1, 4);

    return (Int_t)t.GetMjd() + t.WeekDay() - 6;
}

// --------------------------------------------------------------------------
//
// Get the week of the year. Valid week values are between 1 and 53.
// If for a january date a week number above 50 is returned the
// week belongs to the previous year. If for a december data 1 is
// returned the week already belongs to the next year.
//
// The year to which the week belongs is returned in year.
//
// Die Kalenderwochen werden für Jahre ab 1976 berechnet, da mit
// Geltung vom 1. Januar 1976 der Wochenbeginn auf Montag festgelegt
// wurde. Die erste Woche ist definiert als die Woche, in der
// mindestens 4 der ersten 7 Januartage fallen (also die Woche, in der
// der 4. Januar liegt). Beides wurde damals festgelegt in der DIN 1355
// (1974). Inhaltlich gleich regelt das die Internationale Norm
// ISO 8601 (1988), die von der Europäischen Union als EN 28601 (1992)
// übernommen und in Deutschland als DIN EN 28601 (1993) umgesetzt
// wurde.
//
Int_t MTime::Week(Short_t &year) const
{
    // Possibilities for Week 1:
    //
    //    Mo 4.Jan:   Mo  4. - So 10.    -0   6-6
    //    Di 4.Jan:   Mo  3. - So  9.    -1   6-5
    //    Mi 4.Jan:   Mo  2. - So  8.    -2   6-4
    //    Do 4.Jan:   Mo  1. - So  7.    -3   6-3
    //    Fr 4.Jan:   Mo 31. - So  6.    -4   6-2
    //    Sa 4.Jan:   Mo 30. - So  5.    -5   6-1
    //    So 4.Jan:   Mo 29. - So  4.    -6   6-0
    //
    const Int_t mjd2 = GetMjdWeek1(Year()-1);
    const Int_t mjd0 = GetMjdWeek1(Year());
    const Int_t mjd3 = GetMjdWeek1(Year()+1);

    // Today
    const Int_t mjd = (Int_t)GetMjd();

    // Week belongs to last year, return week of last year
    if (mjd<mjd0)
    {
        year = Year()-1;
        return (mjd-mjd2)/7 + 1;
    }

    // Check if Week belongs to next year (can only be week 1)
    if ((mjd3-mjd)/7==1)
    {
        year = Year()+1;
        return 1;
    }

    // Return calculated Week
    year = Year();
    return (mjd-mjd0)/7 + 1;
}

// --------------------------------------------------------------------------
//
// Is the given year a leap year.
// The calendar year is 365 days long, unless the year is exactly divisible
// by 4, in which case an extra day is added to February to make the year
// 366 days long. If the year is the last year of a century, eg. 1700, 1800,
// 1900, 2000, then it is only a leap year if it is exactly divisible by
// 400. Therefore, 1900 wasn't a leap year but 2000 was. The reason for
// these rules is to bring the average length of the calendar year into
// line with the length of the Earth's orbit around the Sun, so that the
// seasons always occur during the same months each year.
//
Bool_t MTime::IsLeapYear() const
{
    const UInt_t y = Year();
    return (y%4==0) && !((y%100==0) && (y%400>0));
}

// --------------------------------------------------------------------------
//
// Set the time to the current system time. The timezone is ignored.
// If everything is set correctly you'll get UTC.
//
void MTime::Now()
{
#ifdef __LINUX__
    struct timeval tv;
    if (gettimeofday(&tv, NULL)<0)
        Clear();
    else
        Set(tv);
#else
#error __LINUX__ not set
#endif
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "dd.mm.yyyy hh:mm:ss.fff"
//
TString MTime::GetString() const
{
    UShort_t y, ms;
    Byte_t mon, d, h, m, s;

    GetDate(y, mon, d);
    GetTime(h, m, s, ms);

    return MString::Format("%02d.%02d.%04d %02d:%02d:%02d.%03d", d, mon, y, h, m, s, ms);
}

// --------------------------------------------------------------------------
//
// Return contents as a string format'd with strftime:
// Here is a short summary of the most important formats. For more
// information see the man page (or any other description) of
// strftime...
//
//  %a  The abbreviated weekday name according to the current locale.
//  %A  The full weekday name according to the current locale.
//  %b  The abbreviated month name according to the current locale.
//  %B  The full month name according to the current locale.
//  %c  The preferred date and time representation for the current locale.
//  %d  The day of the month as a decimal number (range  01 to 31).
//  %e  Like %d, the day of the month as a decimal number,
//      but a leading zero is replaced by a space.
//  %H  The hour as a decimal number using a 24-hour clock (range 00 to 23)
//  %k  The hour (24-hour clock) as a decimal number (range 0 to 23);
//      single digits are preceded by a blank.
//  %m  The month as a decimal number (range 01 to 12).
//  %M  The minute as a decimal number (range 00 to 59).
//  %R  The time in 24-hour notation (%H:%M).  For a
//      version including the seconds, see %T below.
//  %S  The second as a decimal number (range 00 to 61).
//  %T  The time in 24-hour notation (%H:%M:%S).
//  %x  The preferred date representation for the current
//      locale without the time.
//  %X  The preferred time representation for the current
//      locale without the date.
//  %y  The year as a decimal number without a century (range 00 to 99).
//  %Y  The year as a decimal number including the century.
//  %+  The date and time in date(1) format.
//
// The default is: Tuesday 16.February 2004 12:17:22
//
// The maximum size of the return string is 128 (incl. NULL)
//
// For dates before 1. 1.1902  a null string is returned
// For dates after 31.12.2037 a null string is returned
//
// To change the localization use loc, eg loc = "da_DK", "de_DE".
// Leaving the argument empty will just take the default localization.
//
// If loc is "", each part of the locale that should be modified is set
// according  to the environment variables. The details are implementation
// dependent.  For glibc, first (regardless of category), the  environment
// variable  LC_ALL  is  inspected, next the environment variable with the
// same name as the category (LC_COLLATE, LC_CTYPE, LC_MESSAGES,  LC_MONE?
// TARY,  LC_NUMERIC,  LC_TIME) and finally the environment variable LANG.
// The first existing environment variable is used.
//
// A  locale  name  is  typically  of the form language[_territory][.code?
// set][@modifier], where language is an ISO 639 language code,  territory
// is an ISO 3166 country code, and codeset is a character set or encoding
// identifier like ISO-8859-1 or UTF-8.   For  a  list  of  all  supported
// locales, try "locale -a", cf. locale(1).
//
TString MTime::GetStringFmt(const char *fmt, const char *loc) const
{
    if (!fmt)
        fmt = "%A %e.%B %Y %H:%M:%S";

    UShort_t y, ms;
    Byte_t mon, d, h, m, s;

    GetDate(y, mon, d);
    GetTime(h, m, s, ms);

    // If date<1902 strftime crahses on my (tbretz) laptop
    // it doesn't crash in the DC.
    //    if (y<1902 || y>2037)
    //        return "";

    struct tm time;
    time.tm_sec   = s;
    time.tm_min   = m;
    time.tm_hour  = h;
    time.tm_mday  = d;
    time.tm_mon   = mon-1;
    time.tm_year  = y-1900;
    time.tm_isdst = -1;

    // -1: If dst, isdst is set to 1 but hour is not changed
    //  0: If dst, hour is changed

    // Get old local
    const TString locale = setlocale(LC_TIME, 0);

    // Set new local (e.g. Montag instead of Monday)
    setlocale(LC_TIME, loc);

    // recalculate tm_yday and tm_wday
    mktime(&time);

    // We get errors for example for 1910-01-01
    //    if (mktime(&time)<0)
    //        return "";

    char ret[128];
    const size_t rc = strftime(ret, 127, fmt, &time);

    setlocale(LC_TIME, locale);

    return rc ? ret : "";
}

// --------------------------------------------------------------------------
//
// Set the time according to the format fmt.
// Default is "%A %e.%B %Y %H:%M:%S"
//
// For more information see GetStringFmt
//
Bool_t MTime::SetStringFmt(const char *time, const char *fmt, const char *loc)
{
    if (!fmt)
        fmt = "%A %e.%B %Y %H:%M:%S";

    struct tm t;
    memset(&t, 0, sizeof(struct tm));

    const TString locale = setlocale(LC_TIME, 0);

    setlocale(LC_TIME, loc);
    strptime(time, fmt, &t);
    setlocale(LC_TIME, locale);

    return Set(t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "yyyy-mm-dd hh:mm:ss"
//
TString MTime::GetSqlDateTime() const
{
    return GetStringFmt("%Y-%m-%d %H:%M:%S");
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "yyyymmddhhmmss"
//
TString MTime::GetSqlTimeStamp() const
{
    return GetStringFmt("%Y%m%d%H%M%S");
}

// --------------------------------------------------------------------------
//
// Return contents as a TString of the form:
//   "yyyymmdd_hhmmss"
//
TString MTime::GetFileName() const
{
    return GetStringFmt("%Y%m%d_%H%M%S");
}

// --------------------------------------------------------------------------
//
// Print MTime as string
//
void MTime::Print(Option_t *) const
{
    UShort_t yea, ms;
    Byte_t mon, day, h, m, s;

    GetDate(yea, mon, day);
    GetTime(h, m, s, ms);

    *fLog << all << GetDescriptor() << ": ";
    *fLog << GetString() << MString::Format(" (+%dns)", fNanoSec) << endl;
} 

// --------------------------------------------------------------------------
//
// Calls Set(t[0], t[1], t[2], t[3], t[4], t[5], 0)
//
Bool_t MTime::SetBinary(const UInt_t t[6])
{
    return Set(t[0], t[1], t[2], t[3], t[4], t[5], 0);
}

// --------------------------------------------------------------------------
//
// Assign:
//    t[0] = year;
//    t[1] = month;
//    t[2] = day;
//    t[3] = hour;
//    t[4] = minute;
//    t[5] = second;
//
void MTime::GetBinary(UInt_t t[6]) const
{
    UShort_t yea, ms;
    Byte_t mon, day, h, m, s;

    GetDate(yea, mon, day);
    GetTime(h, m, s, ms);

    t[0] = yea;
    t[1] = mon;
    t[2] = day;
    t[3] = h;
    t[4] = m;
    t[5] = s;
}

// --------------------------------------------------------------------------
//
// Read seven bytes representing y, m, d, h, m, s
//
istream &MTime::ReadBinary(istream &fin)
{
    UShort_t y;
    Byte_t mon, d, h, m, s;

    fin.read((char*)&y,   2);
    fin.read((char*)&mon, 1);
    fin.read((char*)&d,   1);
    fin.read((char*)&h,   1);
    fin.read((char*)&m,   1);
    fin.read((char*)&s,   1); // Total=7

    Set(y, mon, d, h, m, s, 0);

    return fin;
}

// --------------------------------------------------------------------------
//
// Write seven bytes representing y, m, d, h, m, s
//
ostream &MTime::WriteBinary(ostream &out) const
{
    UShort_t y, ms;
    Byte_t mon, d, h, m, s;

    GetDate(y, mon, d);
    GetTime(h, m, s, ms);

    out.write((char*)&y,   2);
    out.write((char*)&mon, 1);
    out.write((char*)&d,   1);
    out.write((char*)&h,   1);
    out.write((char*)&m,   1);
    out.write((char*)&s,   1); // Total=7

    return out;
}

void MTime::AddMilliSeconds(UInt_t ms)
{
    fTime += ms;

    fTime += 11*kHour;
    fMjd  += (Long_t)fTime/kDay;
    fTime  = (Long_t)fTime%kDay;
    fTime -= 11*kHour;
}

void MTime::Plus1ns()
{
    fNanoSec++;

    if (fNanoSec<1000000)
        return;

    fNanoSec = 0;
    fTime += 1;

    if ((Long_t)fTime<(Long_t)kDay*13)
        return;

    fTime = 11*kDay;
    fMjd++;
}   

void MTime::Minus1ns()
{
    if (fNanoSec>0)
    {
        fNanoSec--;
        return;
    }

    fTime -= 1;
    fNanoSec = 999999;

    if ((Long_t)fTime>=-(Long_t)kDay*11)
        return;

    fTime = 13*kDay-1;
    fMjd--;
}   

/*
MTime MTime::operator-(const MTime &tm1)
{
    const MTime &tm0 = *this;

    MTime t0 = tm0>tm1 ? tm0 : tm1;
    const MTime &t1 = tm0>tm1 ? tm1 : tm0;

    if (t0.fNanoSec<t1.fNanoSec)
    {
        t0.fNanoSec += 1000000;
        t0.fTime -= 1;
    }

    t0.fNanoSec -= t1.fNanoSec;
    t0.fTime    -= t1.fTime;

    if ((Long_t)t0.fTime<-(Long_t)kHour*11)
    {
        t0.fTime += kDay;
        t0.fMjd--;
    }

    t0.fMjd -= t1.fMjd;

    return t0;
}

void MTime::operator-=(const MTime &t)
{
    *this = *this-t;
}

MTime MTime::operator+(const MTime &t1)
{
    MTime t0 = *this;

    t0.fNanoSec += t1.fNanoSec;

    if (t0.fNanoSec>999999)
    {
        t0.fNanoSec -= 1000000;
        t0.fTime += kDay;
    }

    t0.fTime += t1.fTime;

    if ((Long_t)t0.fTime>=(Long_t)kHour*13)
    {
        t0.fTime -= kDay;
        t0.fMjd++;
    }

    t0.fMjd += t1.fMjd;

    return t0;
}

void MTime::operator+=(const MTime &t)
{
    *this = *this+t;
}
*/

void MTime::SetMean(const MTime &t0, const MTime &t1)
{
    // This could be an operator+
    *this = t0;

    fNanoSec += t1.fNanoSec;

    if (fNanoSec>999999)
    {
        fNanoSec -= 1000000;
        fTime += kDay;
    }

    fTime += t1.fTime;

    if ((Long_t)fTime>=(Long_t)kHour*13)
    {
        fTime -= kDay;
        fMjd++;
    }

    fMjd += t1.fMjd;

    // This could be an operator/
    if ((Long_t)fTime<0)
    {
        fTime += kDay;
        fMjd--;
    }

    Int_t reminder = fMjd%2;
    fMjd /= 2;

    fTime += reminder*kDay;
    reminder = (Long_t)fTime%2;
    fTime /= 2;

    fNanoSec += reminder*1000000;
    fNanoSec /= 2;

    fTime += 11*kHour;
    fMjd  += (Long_t)fTime/kDay;
    fTime  = (Long_t)fTime%kDay;
    fTime -= 11*kHour;
}

void MTime::SetMean(Double_t t0, Double_t t1)
{
    const Double_t mean = (t0+t1)*(0.5/kDaySec);
    SetMjd(mean);
}

void MTime::AsciiRead(istream &fin)
{
    fin >> *this;
}

Bool_t MTime::AsciiWrite(ostream &out) const
{
    out << *this;
    return bool(out);
}

// --------------------------------------------------------------------------
//
// Calculate the day of easter for the given year.
// MTime() is returned if this was not possible.
//
// In case of the default argument or the year less than zero
// the date of eastern of the current year (the year corresponding to
// MTime(-1)) is returned.
//
//  for more information see: MAstro::GetDayOfEaster()
//
MTime MTime::GetEaster(Short_t year)
{
    if (year<0)
        year = MTime(-1).Year();

    const Int_t day = MAstro::GetEasterOffset(year);
    if (day<0)
        return MTime();

    MTime t;
    t.Set(year, 3, 1);
    t.SetMjd(t.GetMjd() + day);

    return t;
}
