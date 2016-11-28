#ifndef MARS_MParameterCalc
#define MARS_MParameterCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MData;
class MParameterD;

class MParameterCalc : public MTask
{
private:
    MData       *fData;          //-> Input MDataPhrase with rule
    MParameterD *fParameter;     //!  Output parameter

    TString      fNameParameter; //!  Name for output parameter

    ULong_t      fCounter[2];    //!  Counterf for continues

    // MTask
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    Int_t PreProcess(MParList *plist);
    Int_t Process();
    Int_t PostProcess();


public:
    MParameterCalc(const char *def=0, const char *name=NULL, const char *title=NULL);
    ~MParameterCalc();

    // Setter
    Bool_t SetRule(const char *rule);
    void   SetVariables(const TArrayD &);
    void   SetNameParameter(const char *name) { fNameParameter=name; }

    // TObject
    void Print(Option_t *o="") const; //*MENU*

    ClassDef(MParameterCalc, 1) // Task to estimate the energy by a rule
};

#endif

