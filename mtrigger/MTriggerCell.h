#ifndef MARS_MTriggerCell
#define MARS_MTriggerCell

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif 

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MTriggerCell : public MParContainer
{
    friend class MReportTrigger;
private:
    static const Int_t gsNCells=32; //Number of fields with cell rates
                                    // 19 cells and 12 dummy 

    TArrayF fCellRate;       // Array of the measured L1 cell rates

public:
    MTriggerCell() : fCellRate(gsNCells)
    {
        fName  = "MTriggerCell";
        fTitle = "Container for the measured cell rates";
    }

    Float_t GetMean() const { return fCellRate.GetSum()/fCellRate.GetSize(); }

    TArrayF GetCellRate() const { return fCellRate; }

    Double_t operator[](const Int_t idx) 
      { 
	if (idx > gsNCells)
	  return kFALSE;
	
	return fCellRate[idx]; 
      }


    ClassDef(MTriggerCell, 1) // Container for the trigger cell rates
};

#endif
