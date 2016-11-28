#ifndef MARS_MCT1Supercuts
#define MARS_MCT1Supercuts

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MCT1Supercuts : public MParContainer
{
private:
    TArrayD fParameters; // supercut parameters
    TArrayD fStepsizes;  // step sizes of supercut parameters

    Double_t *fLengthUp; //!
    Double_t *fLengthLo; //!
    Double_t *fWidthUp;  //!
    Double_t *fWidthLo;  //!
    Double_t *fDistUp;   //!
    Double_t *fDistLo;   //!
    Double_t *fAsymUp;   //!
    Double_t *fAsymLo;   //!

    Double_t *fConcUp;   //!
    Double_t *fConcLo;   //!
    Double_t *fLeakage1Up;   //!
    Double_t *fLeakage1Lo;   //!

    Double_t *fAlphaUp;  //!


public:
    MCT1Supercuts(const char *name=NULL, const char *title=NULL);

    void InitParameters();

    Bool_t SetParameters(const TArrayD &d);
    Bool_t SetStepsizes(const TArrayD &d);

    const TArrayD &GetParameters() const { return fParameters; }
    const TArrayD &GetStepsizes()  const { return fStepsizes;  }

    const Double_t *GetLengthUp() const { return fLengthUp; }
    const Double_t *GetLengthLo() const { return fLengthLo; }
    const Double_t *GetWidthUp() const  { return fWidthUp; }
    const Double_t *GetWidthLo() const  { return fWidthLo; }
    const Double_t *GetDistUp() const   { return fDistUp; }
    const Double_t *GetDistLo() const   { return fDistLo; }
    const Double_t *GetAsymUp() const   { return fAsymUp; }
    const Double_t *GetAsymLo() const   { return fAsymLo; }

    const Double_t *GetConcUp() const   { return fConcUp; }
    const Double_t *GetConcLo() const   { return fConcLo; }

    const Double_t *GetLeakage1Up() const   { return fLeakage1Up; }
    const Double_t *GetLeakage1Lo() const   { return fLeakage1Lo; }

    const Double_t *GetAlphaUp() const  { return fAlphaUp; }

    ClassDef(MCT1Supercuts, 1) // A container for the Supercut parameters
};

#endif



