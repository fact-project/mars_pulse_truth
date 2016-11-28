#ifndef MARS_MCameraPowerSupply
#define MARS_MCameraPowerSupply

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraPowerSupply : public MParContainer
{
    friend class MReportCamera;
private:
    Float_t fVoltagePos5V;         // [V] voltage_pos5  (+5V)
    Float_t fVoltagePos12V;        // [V] voltage_pos12 (+12V)
    Float_t fVoltageNeg12V;        // [V] voltage_neg12 (-12V)
    Float_t fVoltageOptLinkPos12V; // [V] volatge_opt_link_pos12 (+12V)

    Float_t fCurrentPos5V;         // [A] current_pos5  (+5V)
    Float_t fCurrentPos12V;        // [A] current_pos12 (+12V)
    Float_t fCurrentNeg12V;        // [A] current_neg12 (-12V)
    Float_t fCurrentOptLinkPos12V; // [A] current_opt_link_pos12 (+12V)

public:
    MCameraPowerSupply()
    {
        fName  = "MCameraPowerSupply";
        fTitle = "Container storing information about the Camera power supply";
    }

    Float_t GetVoltagePos5V() const         { return fVoltagePos5V; }
    Float_t GetVoltagePos12V() const        { return fVoltagePos12V; }
    Float_t GetVoltageNeg12V() const        { return fVoltageNeg12V; }
    Float_t GetVoltageOptLinkPos12V() const { return fVoltageOptLinkPos12V; }

    Float_t GetCurrentPos5V() const         { return fCurrentPos5V; }
    Float_t GetCurrentPos12V() const        { return fCurrentPos12V; }
    Float_t GetCurrentNeg12V() const        { return fCurrentNeg12V; }
    Float_t GetCurrentOptLinkPos12V() const { return fCurrentOptLinkPos12V; }

    ClassDef(MCameraPowerSupply, 1) // Container storing information about the Camera power supply
};

#endif
