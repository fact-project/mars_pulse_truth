#ifndef MARS_MFRealTimePeriod
#define MARS_MFRealTimePeriod

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MFRealTimePeriod : public MFilter
{
private:
    ULong_t fTime;     //!
    Bool_t  fResult;   //!

    ULong_t fMilliSec;

public:
    MFRealTimePeriod(UInt_t millis=1000) : fTime(0), fMilliSec(millis)
    {
        fName  = "MFRealTimePeriod";
        fTitle = "Filter allowing execution of a task only after a given real time interval";
    }

    void SetTime(UInt_t millis) { fMilliSec = millis; }

    Int_t Process();
    Bool_t IsExpressionTrue() const { return fResult; }

    ClassDef(MFRealTimePeriod, 0) //Filter allowing execution of a task only after a given real time interval
};

#endif
