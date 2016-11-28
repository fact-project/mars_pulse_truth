#ifndef MARS_MHTriggerLvl0
#define MARS_MHTriggerLvl0

#ifndef MARS_MH
#include "MH.h"
#endif

class MHCamera;
class MCamEvent;

class MHTriggerLvl0 : public MH
{
private:
    MHCamera  *fSum;  // storing the sum
    MCamEvent *fEvt;  //! the current event

    TString fNameEvt;

    Int_t fType;
    Double_t fThreshold;

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

public:
    MHTriggerLvl0(Double_t t=0, const char *name=NULL, const char *title=NULL);
    ~MHTriggerLvl0();

    void SetNameEvt(const TString name) { fNameEvt = name; }
    void SetType(Int_t type) { fType = type; }
    void SetThreshold(Double_t t) { fThreshold = t; }

    TH1 *GetHistByName(const TString name="") const;

    void Draw(Option_t * ="");

    void PrintOutliers(Float_t s) const;

    ClassDef(MHTriggerLvl0, 1) // Histogram to count how often a pixel is above threshold
};

#endif
