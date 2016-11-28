#ifndef MARS_MTime
#define MARS_MTime

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MTime                                                                   //
//                                                                         //
// A generalized MARS time stamp                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef _CPP_IOSFWD
#include <iosfwd>
#endif

#ifndef ROOT_TTime
#include <TTime.h>
#endif

#include <TMath.h>

struct timeval;

class MTime : public MParContainer
{
public:
    static const UInt_t kHour;    // [ms] one hour
    static const UInt_t kDay;     // [ms] one day
    static const UInt_t kDaySec;  // [s] one day

    enum {
        kSunday, kMonday, kTuesday, kWednesday, kThursday, kFriday, kSaturday
    };

private:
    UInt_t fMjd;     // [d]  Day in the century        (Day of sun rise)
    TTime  fTime;    // [ms] Time of Day               (-11h<=x<13h)
    UInt_t fNanoSec; // [ns] NanoSec part of TimeOfDay (<1000000)


    ULong_t GetTime24() const
    {
        return (Long_t)fTime<0 ? (Long_t)fTime+kDay : (ULong_t)fTime;
    }
    void Init(const char *name, const char *title)
    {
        fName  = name  ? name  : "MTime";
        fTitle = title ? title : "Generalized time stamp";
    }

public:
    MTime(const char *name=NULL, const char *title=NULL)
    {
        Init(name, title);
        Clear();

        SetSqlDateTime(name);
    }
    MTime(const struct timeval &tm)
    {
        Init(NULL, NULL);
        Set(tm);
    }
    MTime(Double_t mjd);
    MTime(UShort_t y, Byte_t m, Byte_t d, Byte_t h=13, Byte_t min=0, Byte_t s=0, UShort_t ms=0, UInt_t ns=0);
    MTime(const MTime& t) : MParContainer(), fMjd(t.fMjd), fTime(t.fTime), fNanoSec(t.fNanoSec)
    {
        Init(NULL, NULL);
    }

    //static Int_t Hour() { return 3600; }
    //static Int_t Day()  { return 3600;*24 }

    void operator=(const MTime &t)
    {
        fMjd     = t.fMjd;
        fTime    = t.fTime;
        fNanoSec = t.fNanoSec;
    }

    void Clear(const Option_t *) { Clear(); }
    void Clear() { fMjd=0; fTime=0; fNanoSec=0; }

    void Print(Option_t *t=NULL) const;

    void Now();

    // Setter functions
    Bool_t   Set(UShort_t y, Byte_t m, Byte_t d, Byte_t h=13, Byte_t min=0, Byte_t s=0, UShort_t ms=0, UInt_t ns=0);
    void     Set(const struct timeval &tv);
    Bool_t   SetString(const char *str);
    Bool_t   SetSqlDateTime(TString str);
    Bool_t   SetSqlTimeStamp(const char *str);
    void     SetCT1Time(UInt_t mjd, UInt_t t1, UInt_t t0);
    void     SetCorsikaTime(Float_t time);
    Bool_t   SetStringFmt(const char *time, const char *fmt, const char *loc=0);
    Bool_t   SetBinary(const UInt_t t[6]);
    Bool_t   UpdMagicTime(Byte_t h, Byte_t m, Byte_t s, UInt_t ns);
    Bool_t   SetMjd(UInt_t mjd, ULong_t ms, UInt_t ns=0);
    void     SetMjd(Double_t m);
    void     SetJD(Double_t jd) { SetMjd(jd-2400000.5); }
    void     SetAxisTime(Double_t time);
    void     SetMoonPeriod(Double_t p);
    void     SetMagicPeriodStart(Int_t p);
    void     SetUnixTime(Long64_t sec, ULong64_t usec=0);
    void     SetEaster(Short_t year=0);

    // Getter functions
    Double_t GetJD() const { return GetMjd()+2400000.5; }
    Double_t GetMjd() const;
    Double_t GetGmst() const;
    TString  GetString() const;
    TString  GetStringFmt(const char *fmt=0, const char *loc=0) const;
    TString  GetSqlDateTime() const;
    TString  GetSqlTimeStamp() const;
    TString  GetFileName() const;
    void     GetBinary(UInt_t t[6]) const;
    void     GetDate(UShort_t &y, Byte_t &m, Byte_t &d) const;
    MTime    GetDateOfSunrise() const;
    void     GetDateOfSunrise(UShort_t &y, Byte_t &m, Byte_t &d) const;
    TTime    GetRootTime() const;
    TDatime  GetRootDatime() const;
    Double_t GetAxisTime() const;
    Double_t GetMoonPhase() const;
    Double_t GetMoonPeriod() const;
    Int_t    GetMagicPeriod() const;
    UInt_t   GetNightAsInt() const;
    Long_t   GetTime() const { return (Long_t)fTime; } // [ms] Time of Day returned in the range [-11h, 13h)
    void     GetTime(Byte_t &h, Byte_t &m, Byte_t &s, UShort_t &ms) const;
    void     GetTime(Byte_t &h, Byte_t &m, Byte_t &s) const
    {
        UShort_t ms;
        GetTime(h, m, s, ms);
    }

    UInt_t Year() const     { UShort_t y; Byte_t m, d; GetDate(y,m,d); return y; }
    UInt_t Month() const    { UShort_t y; Byte_t m, d; GetDate(y,m,d); return m; }
    UInt_t Day() const      { UShort_t y; Byte_t m, d; GetDate(y,m,d); return d; }
    Byte_t WeekDay() const; // { return TMath::FloorNint(GetMjd()+3)%7; } // Return Day of the week: Sun=0, Mon=1, ..., Sat=6
    UInt_t Hour() const     { Byte_t h, m, s; GetTime(h,m,s); return h; }
    UInt_t Min() const      { Byte_t h, m, s; GetTime(h,m,s); return m; }
    UInt_t Sec() const      { Byte_t h, m, s; GetTime(h,m,s); return s; }
    UInt_t MilliSec() const { return (GetTime()+kDay)%1000; }
    UInt_t MicroSec() const { return fNanoSec/1000+MilliSec()*1000; }
    UInt_t NanoSec() const  { return fNanoSec+MilliSec()*1000000; }
    Int_t  Week() const     { Short_t y; return Week(y); }
    Int_t  Week(Short_t &year) const;
    UInt_t DayOfYear() const;

    Bool_t IsMidnight() const { return (Long_t)fTime==0 && fNanoSec==0; }
    Bool_t IsLeapYear() const;

    // I/O functions
    std::istream &ReadBinary(std::istream &fin);
    std::ostream &WriteBinary(std::ostream &out) const;

    void AsciiRead(std::istream &fin);
    Bool_t AsciiWrite(std::ostream &out) const;

    // Conversion functions
    operator double() const;   //[s]
    double operator()() const; //[s]

    Double_t    AsDouble() const    { return GetMjd(); }
    //const char *AsString() const    { return GetString(); }
    //const char *AsSQLString() const { return GetSqldateTime(); }

    // Calculation functions
    void AddMilliSeconds(UInt_t ms);
    void Minus1ns();
    void Plus1ns();
    void SetMean(const MTime &t0, const MTime &t1);
    void SetMean(Double_t t0, Double_t t1);
    /*
     MTime operator-(const MTime &tm1);
     void operator-=(const MTime &t);
     MTime operator+(const MTime &t1);
     void operator+=(const MTime &t);
     */

    // Base comparison operators
    bool operator<(const MTime &t) const;
    bool operator>(const MTime &t) const;

    // Derived comparison operators
    bool operator<=(const MTime &t) const;
    bool operator>=(const MTime &t) const;
    bool operator==(const MTime &t) const;
    bool operator!=(const MTime &t) const;
    bool operator!() const;

    static MTime GetEaster(Short_t year=-1);
    static Int_t GetMjdWeek1(Short_t year);

    ClassDef(MTime, 3)	//A generalized MARS time stamp
};

inline std::ostream &operator<<(std::ostream &out, const MTime &t)
{
    out << t.GetString();
    return out;
}

inline std::istream &operator>>(std::istream &in, MTime &t)
{
    TString date, time;
    date.ReadToDelim(in, ' ');
    time.ReadToDelim(in, ' ');
    t.SetString(Form("%s %s", date.Data(), time.Data()));
    return in;
}

inline MTime::operator double() const   //[s]
{
    return ((Double_t)fMjd*kDay+(Long_t)fTime+fNanoSec/1e6)/1000;
}

inline double MTime::operator()() const //[s]
{
    return operator double();
}

inline bool MTime::operator<(const MTime &t) const
{
    if (fMjd<t.fMjd)
        return true;
    if (fMjd==t.fMjd && fTime<t.fTime)
        return true;
    if (fMjd==t.fMjd && fTime==t.fTime && fNanoSec<t.fNanoSec)
        return true;
    return false;
}

inline bool MTime::operator>(const MTime &t) const
{
    if (fMjd>t.fMjd)
        return true;
    if (fMjd==t.fMjd && fTime>t.fTime)
        return true;
    if (fMjd==t.fMjd && fTime==t.fTime && fNanoSec>t.fNanoSec)
        return true;
    return false;
}

inline bool MTime::operator<=(const MTime &t) const
{
    return !operator>(t);
}

inline bool MTime::operator>=(const MTime &t) const
{
    return !operator<(t);
}

inline bool MTime::operator==(const MTime &t) const
{
    return fNanoSec==t.fNanoSec && fTime==t.fTime && fMjd==t.fMjd;
}

inline bool MTime::operator!=(const MTime &t) const
{
    return fNanoSec!=t.fNanoSec || fTime!=t.fTime || fMjd!=t.fMjd;
}

inline bool MTime::operator!() const
{
    return fNanoSec==0 && (ULong_t)fTime==0 && fMjd==0;
}

#endif
