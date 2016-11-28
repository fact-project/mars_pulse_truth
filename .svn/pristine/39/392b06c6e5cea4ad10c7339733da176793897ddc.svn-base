#ifndef MARS_MTriggerBit
#define MARS_MTriggerBit

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif 

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MTriggerBit : public MParContainer
{
    friend class MReportTrigger;

private:
    
    static const Int_t gsNBits=20;        // number of output bits

    TArrayF fBit; // Array with the output bit rates

public:
    MTriggerBit() : fBit(gsNBits)  
    {
        fName  = "MTriggerBit";
        fTitle = "Container for the L2 output bits rates ";
    }

    TArrayF GetTriggerBit() const { return fBit; }

    Double_t operator[](const Int_t idx) 
      {  
	if (idx > gsNBits)
	  return kFALSE;
	
	return fBit[idx]; 
      }

    ClassDef(MTriggerBit, 1) // Container for the L2 output bits rates
};

#endif
