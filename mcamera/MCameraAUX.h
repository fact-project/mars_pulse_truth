#ifndef MARS_MCameraAUX
#define MARS_MCameraAUX

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraAUX : public MParContainer
{
    friend class MReportCamera;
private:
    Bool_t  fRequestCaosLEDs;    // Requested status: o=off, 1=on, bAux_CaOsLEDs_Off/On
    Bool_t  fRequestFansFADC;    // Requested status: o=off, 1=on, bAux_FADC_FANs_Off/On

    Bool_t  fStatusCaosLEDs;     // Monitored status: o=off, 1=on, Cam.CaOs.LED_state
    Bool_t  fStatusFansFADC;     // Monitored status: o=off, 1=on, Cam.FADC.Fans_state

    Float_t fTempCountingHouse1; // [deg C] Temperature sensor in the counting house 1
    Float_t fTempCountingHouse2; // [deg C] Temperature sensor in the counting house 2
    Float_t fTempCountingHouse3; // [deg C] Temperature sensor in the counting house 3

public:
    MCameraAUX()
    {
        fName  = "MCameraAUX";
        fTitle = "Container storing information about the Camera auxiliary system";
    }

    Bool_t  GetRequestCaosLEDs() const    { return fRequestCaosLEDs; }
    Bool_t  GetRequestFansFADC() const    { return fRequestFansFADC; }

    Bool_t  GetStatusCaosLEDs() const     { return fStatusCaosLEDs;  }
    Bool_t  GetStatusFansFADC() const     { return fStatusFansFADC;  }

    Float_t GetTempCountingHouse1() const { return fTempCountingHouse1; }
    Float_t GetTempCountingHouse2() const { return fTempCountingHouse2; }
    Float_t GetTempCountingHouse3() const { return fTempCountingHouse3; }

    ClassDef(MCameraAUX, 2) // Container storing information about the Camera auxiliary system
};

#endif
