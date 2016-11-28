#ifndef MARS_MCameraTD
#define MARS_MCameraTD

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

class MCameraTD : public MParContainer, public MCamEvent
{
    friend class MReportCC;
    friend class MReportRec;
private:
    TArrayC fTD;      // [au] discriminator delays
    Bool_t  fIsValid; // fTD contains valid information

    Bool_t InterpreteTD(TString &str, Int_t ver);

public:
    MCameraTD(Int_t size=577, const char *name=NULL, const char *title=NULL);

    Byte_t operator[](Int_t i) { return fTD[i]; }

    Byte_t GetMin() const;
    Byte_t GetMax() const;

    void Print(Option_t *opt=NULL) const;

    void Invalidate() { fTD.Reset(); fIsValid=kFALSE; }

    void SetValid(Bool_t v=kTRUE) { fIsValid=v; }
    Bool_t IsValid() const { return fIsValid; }

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const
    {
        if (idx>=fTD.GetSize())
            return kFALSE;

        val = fTD[idx];
        return val>0;
    }
    void DrawPixelContent(Int_t num) const
    {
    }

    ClassDef(MCameraTD, 2) // Storage Container for Discriminator Delays
};

#endif
