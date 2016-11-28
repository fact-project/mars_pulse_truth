#ifndef MARS_MAnalysis
#define MARS_MAnalysis

#ifndef MARS_MBrowser
#include "MBrowser.h"
#endif

class TGTextEntry;
class TGCheckButton;

class MImgCleanStd;

class MAnalysis : public MBrowser
{
private:
    TGCheckButton *fCheckButton1;
    TGCheckButton *fCheckButton2;

//    TGTextEntry   *fNumEntry1;
//    TGTextEntry   *fNumEntry2;

    MImgCleanStd  *fImgClean;

    void CalculateHillas();

    void AddButtons();
    void AddSetupTab();

public:
    MAnalysis(/*const TGWindow *main=NULL,*/ const TGWindow *p=NULL,
              const UInt_t w=500, const UInt_t h=500) ;

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MAnalysis, 0) // GUI: The 'Analysis' browser.
};

#endif


