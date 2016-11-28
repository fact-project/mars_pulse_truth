#ifndef MARS_MTriggerLiveTime
#define MARS_MTriggerLiveTime

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif 

#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class MTriggerLiveTime : public MParContainer
{
    friend class MReportTrigger;

private:
    static const Int_t gsNScalers=5;        // number of scalers

    TArrayL fLiveTime; // Array with the livetime
    TArrayL fDeadTime; // Array with the deadtime

public:
    MTriggerLiveTime() : fLiveTime(gsNScalers), fDeadTime(gsNScalers)  
    {
        fName  = "MTriggerLiveTime";
        fTitle = "Container for the Live-deadtime      ";
    }

    TArrayL GetLiveTime() const { return fLiveTime; }
    TArrayL GetDeadTime() const { return fDeadTime; }

    // !FIX ME!  Only live time is returned...
    Double_t operator[](const Int_t idx) 
      {  	
	if (idx > gsNScalers)
	  return -1;
	
	return fLiveTime[idx]; 
      }

    ClassDef(MTriggerLiveTime, 1) // Container for the Live-Deadtime
};

#endif
