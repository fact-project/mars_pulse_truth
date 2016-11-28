#ifndef MARS_MCameraHV
#define MARS_MCameraHV

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayS
#include <TArrayS.h>
#endif

class MCameraHV : public MParContainer, public MCamEvent
{
    friend class MReportCamera;
private:
    Byte_t  fStatus;        // CaCo monitored status of the High Voltage [0-9], Cam.HV.PS_state
    Bool_t  fStatusRamping; // CaCo monitored status of ramping the HV, Cam.HV_state

    Short_t fVoltageA;      // [V] Measured voltage of power supply A, hvps1
    Short_t fVoltageB;      // [V] Measured voltage of power supply B, hvps2

    Byte_t  fCurrentA;      // [mA] Measured current of power supply A, curr1
    Byte_t  fCurrentB;      // [mA] Measured current of power supply B, curr2

    TArrayS fHV;            // [V] Measured high Voltages for all PMTs

    TString fFileName;      // Filename for the HV settings

public:
    MCameraHV() : fHV(577)
    {
        fName  = "MCameraHV";
        fTitle = "Container storing information about the Camera HV";
    }

    Byte_t  GetStatus() const { return fStatus; }
    Bool_t  GetStatusRamping() const { return fStatusRamping; }

    Short_t GetVoltageA() const { return fVoltageA; }
    Short_t GetVoltageB() const { return fVoltageB; }

    Byte_t  GetCurrentA() const { return fCurrentA; }
    Byte_t  GetCurrentB() const { return fCurrentB; }

    Float_t GetMean() const { return fHV.GetSum()/fHV.GetSize(); }

    Double_t operator[](const UInt_t idx) { return fHV[idx]; }

    void   DrawPixelContent(Int_t num) const { }
    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const
    {
        if (idx>=fHV.GetSize())
            return kFALSE;

        val = fHV[idx];
        return val>0;
    }

    ClassDef(MCameraHV, 2) // Container storing information about the Camera HV
};

#endif
