#ifndef MARS_MMonteCarlo
#define MARS_MMonteCarlo

#ifndef MARS_MBrowser
#include "MBrowser.h"
#endif

class TGTextEntry;
class TGRadioButton;

class MMonteCarlo : public MBrowser
{
private:
    TGRadioButton *fRadioButton1;
    TGRadioButton *fRadioButton2;
    TGRadioButton *fRadioButton3;

    TGTextEntry   *fNumEntry;

    Int_t GetDim() const;

    void CalculateCollectionArea();
    void CalculateTriggerRate();
    void CalculateThreshold();

    void AddButtons();
    void AddSetupTab();

public:
    MMonteCarlo(/*const TGWindow *main=NULL,*/ const TGWindow *p=NULL,
                const UInt_t w=500, const UInt_t h=500);

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MMonteCarlo, 0) // GUI: The 'monte-carlo' browser
};

#endif


