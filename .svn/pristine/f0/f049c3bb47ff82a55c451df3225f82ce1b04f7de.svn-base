#ifndef MARS_MCameraCalibration
#define MARS_MCameraCalibration

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraCalibration : public MParContainer
{
    friend class MReportCamera;
private:
    Byte_t fStatus;           // CaCo monitored calibration state: o=off, 1=on, CamCalib_state
    Byte_t fStatusCANbus;     // CaCo monitored CANbus state: o=off, 1=on, Cal.CanBus_state
    Byte_t fStatusIO;         // CaCo monitored I/O state: o=off, 1=on, Cal.IO_state
    Byte_t fStatusLoVoltage;  // CaCo monitored LV PS state: o=off, 1=on, Cal.LV_state

    Bool_t fRequestHiVoltage; // Requested status: o=off, 1=on, bcalib_HV_Off/On
    Bool_t fRequestLoVoltage; // Requested status: o=off, 1=on, bcalib_LV_Off/On
    Bool_t fRequestContLight; // Requested status: o=off, 1=on, bcalib_ContinuousLight_Off/On
    Bool_t fRequestPinDiode;  // Requested status: o=off, 1=on, bcalib_PinDiode_Off/On

public:
    MCameraCalibration()
    {
        fName  = "MCameraCalibration";
        fTitle = "Container storing information about the Camera calibration system";
    }

    Byte_t GetStatus() const           { return fStatus; }
    Byte_t GetStatusCANbus() const     { return fStatusCANbus; }
    Byte_t GetStatusIO() const         { return fStatusIO; }
    Byte_t GetStatusLoVoltage() const  { return fStatusLoVoltage; }

    Bool_t GetRequestHiVoltage() const { return fRequestHiVoltage; }
    Bool_t GetRequestLoVoltage() const { return fRequestLoVoltage; }
    Bool_t GetRequestContLight() const { return fRequestContLight; }
    Bool_t GetRequestPinDiode() const  { return fRequestPinDiode; }

    ClassDef(MCameraCalibration, 1) // Container storing information about the Camera calibration system
};

#endif
