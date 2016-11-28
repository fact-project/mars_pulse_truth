#ifndef MARS_MTriggerPrescFact
#define MARS_MTriggerPrescFact

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif 

#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class MTriggerPrescFact : public MParContainer
{
    friend class MReportTrigger;

private:
    
    static const Int_t gsNPrescFacts=8;        // number of factors

    TArrayL fPrescFact; // Array with the prescaling factors

public:
    MTriggerPrescFact() : fPrescFact(gsNPrescFacts)  
    {
        fName  = "MTriggerPrescFact";
        fTitle = "Container for the L2 Prescaling Factors      ";
    }

    TArrayL GetPrescFactors() const { return fPrescFact; }

    Double_t operator[](const Int_t idx) 
      {  
	if (idx > gsNPrescFacts)
	  return kFALSE;
	
	return fPrescFact[idx]; 
      }

    ClassDef(MTriggerPrescFact, 1) // Container for the L2 Prescaling Factors
};

#endif
