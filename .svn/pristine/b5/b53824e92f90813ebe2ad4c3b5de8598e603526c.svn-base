#ifndef MARS_MGDisplayAdc
#define MARS_MGDisplayAdc

#ifndef ROOT_TFrame
#include <TGFrame.h>    // TGTransientFrame
#endif

class TList;
class TCanvas;

class MHFadcCam;

class TGVSlider;
class TGListBox;
class TGTextButton;
class TGRadioButton;
class TRootEmbeddedCanvas;

class MGDisplayAdc : public TGTransientFrame
{
private:
    MHFadcCam *fHists;		// Pointer to Container with the histograms

    TList     *fList;
    TCanvas   *fCanvas;
    TGVSlider *fSlider;
    TGListBox *fHistoList;

    Int_t      fHistoType;

    void AddFrameTop(TGHorizontalFrame *frame);
    void AddFrameLow(TGHorizontalFrame *frame);

    //
    // Create a main frame with a number of different buttons.
    //
    void   UpdateHist();
    Bool_t BuildHistoList();

public:

    MGDisplayAdc(MHFadcCam *fHists ,
                 const TGWindow *p=NULL, const TGWindow *main=NULL,
                 UInt_t w=800, UInt_t h=500,
                 UInt_t options = kMainFrame|kVerticalFrame);
    ~MGDisplayAdc();

    void  CloseWindow();

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MGDisplayAdc, 0)
};

#endif


