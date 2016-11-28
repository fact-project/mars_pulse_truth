#ifndef MARS_MTriggerIPR
#define MARS_MTriggerIPR

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif 

#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class MTriggerIPR : public MParContainer, public MCamEvent
{
    friend class MReportTrigger;

private:
    static const Int_t gsNTrigPix=397;  // number of trigger pixels

    TArrayL fIPR;                       // [Hz] IPR (Individual Pixel Rates)

public:
    MTriggerIPR() : fIPR(gsNTrigPix)
    {
        fName  = "MTriggerIPR";
        fTitle = "Trigger-Container for the Individual Pixel Rate (IPR)";
    }

    Float_t GetMean() const { return fIPR.GetSum()/fIPR.GetSize(); }

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const
    {
      if (idx >= gsNTrigPix)
	return kFALSE;

      val = fIPR[idx];
      return val>0;
    }

    Double_t operator[](const Int_t idx) 
      {
	if (idx > gsNTrigPix)
	  return kFALSE;

	return fIPR[idx]; 
      } 

    void DrawPixelContent(Int_t num) const
    {
    }

    ClassDef(MTriggerIPR, 1) // Trigger-Container for the Individual Pixel Rate (IPR)
};

#endif
