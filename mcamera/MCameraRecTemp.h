#ifndef MARS_MCameraRecTemp
#define MARS_MCameraRecTemp

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MCameraRecTemp : public MParContainer
{
    friend class MReportCC;
    friend class MReportRec;
private:
    TArrayF fRecTemp; // [deg C] receiver board temperatures
    Bool_t  fIsValid; // fRecTemp contains valid information

    Bool_t InterpreteRecTemp(TString &str);

public:
    MCameraRecTemp(Int_t size=76, const char *name=NULL, const char *title=NULL);

    Float_t operator[](Int_t i) const { return fRecTemp[i]; }

    void Invalidate() { fRecTemp.Reset(); fIsValid=kFALSE; }

    void SetValid(Bool_t v=kTRUE) { fIsValid=v; }
    Bool_t IsValid() const { return fIsValid; }

    Float_t GetMin() const;
    Float_t GetMax() const;

    void Print(Option_t *opt=NULL) const;

    ClassDef(MCameraRecTemp, 2) // Storage Container for Receiver Board Temperature
};

#endif
