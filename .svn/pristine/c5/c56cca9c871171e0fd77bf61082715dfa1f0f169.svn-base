#ifndef MARS_MHWeather
#define MARS_MHWeather

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TGraph
#include <TGraph.h>
#endif

class MReportCC;
class MReportPyrometer;
class MEventRate;

class MHWeather : public MH
{
private:
    TGraph fHumidity;        // Humidity from weather station
    TGraph fTemperature;     // Temperature from weather station
    TGraph fWindSpeed;       // Wind Speed from weather station

    TGraph fEventRate;       // Event rate from event times

    TGraph fCloudiness;      // Cloudiness from Pyrometer
    TGraph fTempAir;         // Temperature of the air from Pyrometer
    TGraph fTempSky;         // Temperature of the sky from Pyrometer

    MReportCC        *fReportCC;    //! CC report with data from weather station
    MReportPyrometer *fReportPyro;  //! Pyrometer report
    MEventRate       *fRate;        //! Event rate

    void ResetGraph(TGraph &g) const;
    void InitGraph(TGraph &g) const;
    void AddPoint(TGraph &g, Double_t x, Double_t y) const;
    void DrawGraph(TGraph &g, const char *y=0) const;
    void UpdateRightAxis(TGraph &g) const;
    void DrawRightAxis(const char *title, Int_t col=kBlack) const;

public:
    MHWeather(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    // If this is needed be aware that it would be called twice in star
    // Bool_t Finalize()

    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    ClassDef(MHWeather, 2) // Histogram to display weather and pyrometer data
};

#endif


