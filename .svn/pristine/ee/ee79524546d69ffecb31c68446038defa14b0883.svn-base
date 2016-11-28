#ifndef MARS_MEventDisplay
#define MARS_MEventDisplay

#ifndef MARS_MStatusDisplay
#include "MStatusDisplay.h"
#endif

class TGLabel;
class TGTextEntry;
class MEvtLoop;

class MParList;
class MTaskList;
class MReadTree;

class MEventDisplay : public MStatusDisplay
{
private:
    enum
    {
        kEvtPrev = MStatusDisplay::kLastElement + 1,
        kEvtNext,
        kEvtNumber,
        kShowMuon,
        kShowImgPar
    };

    TGCompositeFrame *fTab1;
    TGCompositeFrame *fTab2;

    TGLabel  *fNumOfEvts;
    TCanvas  *fCanvas;
    MEvtLoop *fEvtLoop;

    void AddTopFramePart1(TGCompositeFrame *frame,
                          const char *filename, const char *treename);
    void AddTopFramePart2(TGCompositeFrame *frame);
    void AddMiddleFrame(TGCompositeFrame *frame);
    void AddGeometryTabs();
    void AddUserFrame(const char *filename);
    void ShowHide();

    void UpdateDisplay();
    void SetupTaskList(const char *tname, TString fname, const char *path);

    void ReadFirstEvent();
    void ReadinEvent(Int_t dir=0);

    Int_t GetFileType(const char *tree, const char *fname) const;

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

public:
    MEventDisplay(const char *fname, const char *path="");
    ~MEventDisplay();

    ClassDef(MEventDisplay, 0) // Display for camera images (cerenkov events)
};

#endif


