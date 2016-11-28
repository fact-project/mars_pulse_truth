#ifndef MARS_MRawCrateData
#define MARS_MRawCrateData

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

//gcc 3.2
//class ifstream;
#include <iosfwd>

class MRawCrateData : public MParContainer
{
private:
    UShort_t fDAQCrateNumber;  // Crate number the information corresponds to
    UInt_t   fFADCEvtNumber;   // event number from the fadc
    UInt_t   fFADCClockTick;   // clock tick from the fadc (20MHz)
    Byte_t   fABFlags;         // flag describing in which two-slice block the trigger was raised

    Bool_t ReadEvtOld(std::istream& fin, UShort_t ver);

public:
    MRawCrateData();

    UChar_t GetDAQCrateNumber() const  { return fDAQCrateNumber; }
    UInt_t  GetFADCEvtNumber() const   { return fFADCEvtNumber;  }
    UInt_t  GetFADCClockTick() const   { return fFADCClockTick;  }
    Byte_t  GetABFlags() const         { return fABFlags;        }

    void Print(Option_t *t=NULL) const;

    Bool_t ReadEvt(std::istream& fin, UShort_t ver, UInt_t size);
    void   SkipEvt(std::istream& fin, UShort_t ver);

    ClassDef(MRawCrateData, 2) //Container to store the Raw CRATE DATA
};

#endif
