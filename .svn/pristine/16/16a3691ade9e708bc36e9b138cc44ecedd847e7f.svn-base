#ifndef MARS_MCameraDC
#define MARS_MCameraDC

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MCameraDC : public MParContainer, public MCamEvent
{
    friend class MReportCamera;
    friend class MReportCurrents;
private:
    Byte_t  fStatus; // CaCo monitored status of the High Voltage [0-9], Cam.HV.PS_state

    TArrayF fArray;  // [nA] Unsigned Int!

    Int_t Interprete(TString &str, Int_t len=0);

public:
    MCameraDC(Int_t size=577, const char *name=NULL, const char *title=NULL);

    Byte_t GetStatus() const { return  fStatus; }

    void SetCurrent(Int_t i, Float_t val) { fArray[i] = (Int_t)val; }
    Float_t GetCurrent(Int_t i) const { return (*this)[i]; }
    Float_t &operator[](Int_t i) { return (Float_t&)fArray[i]; }
    const Float_t &operator[](Int_t i) const { return (*const_cast<MCameraDC*>(this))[i]; }

    Float_t GetMin() const;
    Float_t GetMax() const;

    void Print(Option_t *opt=NULL) const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const
    {
        if (idx>=fArray.GetSize())
            return kFALSE;

        val = fArray[idx];
        return val>0;
    }
    void DrawPixelContent(Int_t num) const
    {
    }

    ClassDef(MCameraDC, 2) // Storage Container for the Currents (PRELIMINARY)
};

#endif
