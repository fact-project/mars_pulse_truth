#ifndef MARS_MCameraActiveLoad
#define MARS_MCameraActiveLoad

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraActiveLoad : public MParContainer
{
    friend class MReportCamera;

private:
    Bool_t fStatus;               // Status of active loads (0xff means: n/a)

    Float_t fVoltage360A;         // [V] Voltage of 360V power supply A
    Float_t fIntens360A;          // [mA] Intensity of 360V power supply A
    Float_t fVoltage360B;         // [V] Voltage of 360V power supply B
    Float_t fIntens360B;          // [mA] Intensity of 360V power supply B
    Float_t fVoltage175A;         // [V] Voltage of 175V power supply A
    Float_t fIntens175A;          // [mA] Intensity of 175V power supply A
    Float_t fVoltage175B;         // [V] Voltage of 175V power supply B
    Float_t fIntens175B;          // [mA] Intensity of 175V power supply B

public:
    MCameraActiveLoad()
    {
        fName  = "MCameraActiveLoad";
        fTitle = "Container storing information about the Camera Active Load system";
    }

    Bool_t  GetStatus() const      { return fStatus;      }

    Float_t GetVoltage360A() const { return fVoltage360A; }
    Float_t GetIntens360A() const  { return fIntens360A;  }
    Float_t GetVoltage360B() const { return fVoltage360B; }
    Float_t GetIntens360B() const  { return fIntens360B;  }
    Float_t GetVoltage175A() const { return fVoltage175A; }
    Float_t GetIntens175A() const  { return fIntens175A;  }
    Float_t GetVoltage175B() const { return fVoltage175B; }
    Float_t GetIntens175B() const  { return fIntens175B;  }

    ClassDef(MCameraActiveLoad, 1) // Container storing information about the Active Loads of the Camera
};

#endif
