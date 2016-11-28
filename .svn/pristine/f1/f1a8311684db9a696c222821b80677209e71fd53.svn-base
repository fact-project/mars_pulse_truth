#ifndef MARS_MCameraLV
#define MARS_MCameraLV

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCameraPowerSupply
#include <MCameraPowerSupply.h>
#endif

class MCameraLV : public MParContainer
{
    friend class MReportCamera;
private:
    Byte_t  fStatus;                  // CaCo monitored LV PS status:  , Cam.LV_state
    Bool_t  fRequestPowerSupply;      // Requested status: o=off, 1=on, blv_ps_status

    Float_t fTemp;                    // Measured status: o=off, 1=on, blv_temp
    Byte_t  fHumidity;                // Measured status: o=off, 1=on, blv_RelativeHumidity

    MCameraPowerSupply fPowerSupplyA; // power supply camera part A
    MCameraPowerSupply fPowerSupplyB; // power supply camera part B

public:
    MCameraLV()
    {
        fName  = "MCameraLV";
        fTitle = "Container storing information about the Camera LV";
    }

    Byte_t  GetStatus() const { return fStatus; }
    Bool_t  GetRequestPowerSupply() const { return fRequestPowerSupply; }

    Float_t GetTemp() const { return fTemp; }
    Byte_t  GetHumidity() const { return fHumidity; }

    const MCameraPowerSupply &GetPowerSupplyA() const { return fPowerSupplyA; }
    const MCameraPowerSupply &GetPowerSupplyB() const { return fPowerSupplyB; }

    ClassDef(MCameraLV, 1) // Container storing information about the Camera LV
};

#endif
