#ifndef MARS_MHPhotonEvent
#define MARS_MHPhotonEvent

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TProfile2D
#include <TProfile2D.h>
#endif

class MPhotonEvent;

class MHPhotonEvent : public MH
{
private:
    TH2F       fHistXY;
    TH2F       fHistUV;
    TProfile2D fHistT;
    TH1F       fHistWL;
    TH1F       fHistH;

    Int_t      fType;
    Bool_t     fPermanentReset;

    // MHPhotonEvent
    void Init(const char *name, const char *title);
    void SetBinningXY(Int_t num, Double_t max);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MH
    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t weight=1);
    //Bool_t Finalize();

public:
    MHPhotonEvent(Double_t max, const char *name=0, const char *title=0);
    MHPhotonEvent(Int_t type=3, const char *name=0, const char *title=0);

    void PermanentReset(Bool_t b=kTRUE) { fPermanentReset=b; }

    // TObject
    void Draw(Option_t *o="");
    void Paint(Option_t *o="");

    void Clear(Option_t *o="")
    {
        fHistXY.Reset();
        fHistUV.Reset();
        fHistT.Reset();
        fHistWL.Reset();
        fHistH.Reset();
    }

    const TH2F &GetHistXY() const { return fHistXY; }
    const TProfile2D &GetHistT() const  { return fHistT; }

    ClassDef(MHPhotonEvent, 3) // Histogram to display the information of MPhotonEvents
};

#endif
