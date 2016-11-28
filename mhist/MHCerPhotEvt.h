#ifndef MARS_MHCerPhotEvt
#define MARS_MHCerPhotEvt

#ifndef MARS_MH
#include "MH.h"
#endif

class MCamDisplay;
class MCamEvent;

class MHCerPhotEvt : public MH
{
private:
    MCamDisplay *fSum;      // storing the sum
    MCamEvent   *fEvt;      //! the current event

    TString fNameEvt;

    Bool_t SetupFill(const MParList *pList);
    Bool_t Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

public:
    MHCerPhotEvt(const char *name=NULL, const char *title=NULL);
    ~MHCerPhotEvt();

    void SetNameEvt(const TString name) { fNameEvt = name; }

    TH1 *GetHistByName(const TString name="");

    ClassDef(MHCerPhotEvt, 1) // Histogram to sum camera events
};

#endif
