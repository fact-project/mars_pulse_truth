#ifndef MARS_MCameraCentralPix
#define MARS_MCameraCentralPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraCentralPix : public MParContainer
{
    friend class MReportCamera;

private:
    Bool_t fStatus; // Status of central pixel

    Int_t fDC;     // DC current in ADC counts

public:
    MCameraCentralPix() : fDC(0)
    {
        fName  = "MCameraCentralPix";
        fTitle = "Container storing information about the Camera Central Pixel";
    }

    Bool_t GetStatus() const     { return fStatus; }
    Bool_t GetStatusCPix() const { return fStatus; } // deprecated - for compatibility only

    Int_t GetDC() const { return fDC; }

    ClassDef(MCameraCentralPix, 2) // Container storing information about the Central Pixel status
};

#endif
