#ifndef MARS_MAstro
#define MARS_MAstro

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif
#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

class TArrayD;
class TVector3;
class MTime;

class MAstro
{
private:
    static const Double_t kSynMonth; // synodic month (new Moon to new Moon)
    static const Double_t kEpoch0;   // First full moon after 1980/1/1

    static Double_t Round(Double_t val);
    static Double_t Trunc(Double_t val);

    static Double_t Kepler(Double_t m, Double_t ecc);

public:
    virtual ~MAstro() { }

    // Unit conversion
    static Double_t Parsec2Meter()    { return 3.0856776e+16; }
    static Double_t Parsec2Lighyear() { return 3.2615644; }

    static Double_t DegToHor()        { return 15; }
    static Double_t HorToDeg()        { return 1./15; }

    static Double_t RadToHor(); // { return 24/TMath::TwoPi(); }
    static Double_t HorToRad(); // { return TMath::TwoPi()/24; }

    // Angle/Time conversion functions
    static Double_t Hms2Sec(Int_t deg, UInt_t min, Double_t sec, char sgn='+');
    static Double_t Dms2Rad(Int_t deg, UInt_t min, Double_t sec, Char_t sgn='+');
    static Double_t Hms2Rad(Int_t hor, UInt_t min, Double_t sec, Char_t sgn='+');
    static Double_t Dms2Deg(Int_t deg, UInt_t min, Double_t sec, Char_t sgn='+');
    static Double_t Hms2Deg(Int_t hor, UInt_t min, Double_t sec, Char_t sgn='+');
    static Double_t Dms2Hor(Int_t deg, UInt_t min, Double_t sec, Char_t sgn='+');
    static Double_t Hms2Hor(Int_t hor, UInt_t min, Double_t sec, Char_t sgn='+');

    static void Day2Hms(Double_t day, Char_t &sgn, UShort_t &h, UShort_t &m, UShort_t &s);
    static void Rad2Dms(Double_t rad, Char_t &sgn, UShort_t &d, UShort_t &m, UShort_t &s);
    static void Rad2Hms(Double_t rad, Char_t &sgn, UShort_t &h, UShort_t &m, UShort_t &s);
    static void Deg2Dms(Double_t deg, Char_t &sgn, UShort_t &d, UShort_t &m, UShort_t &s);
    static void Deg2Hms(Double_t deg, Char_t &sgn, UShort_t &h, UShort_t &m, UShort_t &s);
    static void Hor2Dms(Double_t hor, Char_t &sgn, UShort_t &d, UShort_t &m, UShort_t &s);
    static void Hor2Hms(Double_t hor, Char_t &sgn, UShort_t &h, UShort_t &m, UShort_t &s);

    static void Day2Hm(Double_t day, Char_t &sgn, UShort_t &h, Double_t &m);
    static void Rad2Dm(Double_t rad, Char_t &sgn, UShort_t &d, Double_t &m);
    static void Rad2Hm(Double_t rad, Char_t &sgn, UShort_t &h, Double_t &m);
    static void Deg2Dm(Double_t deg, Char_t &sgn, UShort_t &d, Double_t &m);
    static void Deg2Hm(Double_t deg, Char_t &sgn, UShort_t &h, Double_t &m);
    static void Hor2Dm(Double_t rad, Char_t &sgn, UShort_t &d, Double_t &m);
    static void Hor2Hm(Double_t rad, Char_t &sgn, UShort_t &h, Double_t &m);

    // Print funtions
    static TString GetStringDeg(Double_t deg, const char *fmt="%c%d:%02d:%02d");
    static TString GetStringHor(Double_t deg, const char *fmt="%c%d:%02d:%02d");

    // Angle treatment functions
    static Bool_t  String2Angle(TString &str, Double_t &ret);
    static Bool_t  Coordinate2Angle(const TString &str, Double_t &ret);
    static TString Angle2Coordinate(Double_t val);

    static Double_t AngularDistance(Double_t theta0, Double_t phi0, Double_t theta1, Double_t phi1);

    // Time treatment functions
    static void  Mjd2Ymd(UInt_t mjd, UShort_t &y, Byte_t &m, Byte_t &d);
    static Int_t Ymd2Mjd(UShort_t y, Byte_t m, Byte_t d);

    static UInt_t Mjd2Yymmdd(UInt_t mjd);
    static UInt_t Yymmdd2Mjd(UInt_t yymmdd);

    static Double_t UT2GMST(Double_t ut1);

    // Rotation angle between local and sky coordinate system
    static void     RotationAngle(Double_t sinl, Double_t cosl, Double_t theta, Double_t phi, Double_t &sin, Double_t &cos);
    static Double_t RotationAngle(Double_t sinl, Double_t cosl, Double_t theta, Double_t phi);

    static void     GetMoonRaDec(Double_t mjd, Double_t &ra, Double_t &dec);

    static Double_t GetMoonPhase(Double_t mjd);
    static Double_t GetMoonPeriod(Double_t mjd);
    static Double_t GetMoonPeriodMjd(Double_t p);
    static Int_t    GetMagicPeriod(Double_t mjd);
    static Double_t GetMagicPeriodStart(Int_t p);

    static Double_t EstimateCulminationTime(Double_t mjd, Double_t longit, Double_t ra);

    // Estimate some parameters around the sun
    static Double_t GetSunRaDec(Double_t mjd, Double_t &ra, Double_t &dec);
    static Double_t GetEquationOfTime(Double_t mjd);
    static Double_t GetNoonTime(Double_t mjd, Double_t longit);
    static Double_t GetTimeFromNoonToAlt(Double_t mjd, Double_t latit, Double_t alt);
    static TArrayD  GetSunRiseSet(Double_t mjd, Double_t longit, Double_t latit, Double_t alt=0);

    // Get distance between v1 and v0 in a plain perpendicular to v0 in distance dist
    static TVector2 GetDistOnPlain(const TVector3 &v0, TVector3 v1, Double_t dist=1);

    // Calculate absolute misspointing from error in zd and error in az
    static Double_t GetDevAbs(Double_t nomzd, Double_t devzd, Double_t devaz);

    static Int_t GetEasterOffset(UShort_t year);

    ClassDef(MAstro, 0)
};

#endif
