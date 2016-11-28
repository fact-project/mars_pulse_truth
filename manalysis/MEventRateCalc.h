#ifndef MARS_MEventRateCalc
#define MARS_MEventRateCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MTime;
class MEventRate;
class MParameterD;

class MEventRateCalc : public MTask
{
private:
    static const TString gsDefName;       //! Default name of container
    static const TString gsDefTitle;      //! Default title of container

    static const TString gsNameEventRate; //! default name of rate container
    static const TString gsNameTime;      //! Default name of time container
    static const TString gsNameTimeDiff;  //! default name of time-diff container
    static const TString gsNameTimeRate;  //! default name of time-rate container

    static const Int_t gsNumEvents;       //! Default number of events


    MTime       *fTime;       //! pointer to event time
    MTime       *fTimeRate;   //! pointer to time of event rate
    MEventRate  *fRate;       //! pointer to rate storage container
    MParameterD *fTimeDiff;   //! Difference of time between two consecutive events

    TString fNameEventRate;  // name of event rate container
    TString fNameTime;       // name of time container
    TString fNameTimeRate;   // name of event rate time container
    TString fNameTimeDiff;   // name of time-diff container

    TArrayD  fTimes;         //! internal array to store the last n event times

    Int_t fNumFirstEvent;    //! Number of first event in file

    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    void  StreamPrimitive(std::ostream &out) const;
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MEventRateCalc(const char *name=NULL, const char *title=NULL);

    void SetNumEvents(ULong_t num) { fTimes.Set(num); }

    void SetNameEventRate(const char *name) { fNameEventRate = name; }
    void SetNameTime(const char *name)      { fNameTime = name; }
    void SetNameTimeDiff(const char *name)  { fNameTimeDiff = name; }
    void SetNameTimeRate(const char *name)  { fNameTimeRate = name; }

    ClassDef(MEventRateCalc, 1)// Task to calculate event rates
};
 

#endif
