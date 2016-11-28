#ifndef MARS_MReportCC
#define MARS_MReportCC

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportRec;
class MCameraTH;
class MCameraTD;
class MCameraRecTemp;

class MReportWeather : public MReport
{
protected:
    Float_t fHumidity;        // [%]
    Float_t fTemperature;     // [deg] celsius
    Float_t fWindSpeed;       // [km/h]
    Float_t fSolarRadiation;  // [W/m^2] IR-Radiation

    Bool_t SetupReadingFits(fits &fits);

public:
    MReportWeather(const char *rep="WEATHER-REPORT")  : MReport(rep, kFALSE)
    {
        fName  = "MReportWeather";
        fTitle = "Class for Weather data";
    }

    void Print(Option_t *opt) const;

    ClassDef(MReportWeather, 1) // Class for Weather information
};

class MReportCC : public MReportWeather
{
private:
    Float_t fUPSStatus;       // arbitrary units (still not properly defined)
    Float_t fDifRubGPS;       // [us] Difference between the Rubidium clock time and the time provided by the GPS receiver

    MCameraTH      *fTH;      //! Discriminator thresholds
    MCameraTD      *fTD;      //! Discriminator delays
    MCameraRecTemp *fRecTemp; //! Receiver Board temperatures

    MReportRec     *fRecRep;  //! Pipe interpretation to MReportRec if necessary

    // Internal
    Bool_t SetupReading(MParList &plist);
    Bool_t InterpreteCC(TString &str, Int_t ver);
    Bool_t InterpreteSchedule(TString &str);
    Bool_t InterpreteStatusM2(TString &str);

    // MReport
    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportCC();

    Float_t GetHumidity() const       { return fHumidity; }
    Float_t GetTemperature() const    { return fTemperature; }
    Float_t GetWindSpeed() const      { return fWindSpeed; }
    Float_t GetSolarRadiation() const { return fSolarRadiation; }

    ClassDef(MReportCC, 3) // Class for CC-REPORT information
};

#endif
