#ifndef MARS_MOnlineDisplay
#define MARS_MOnlineDisplay

#ifndef MARS_MStatusDisplay
#include "MStatusDisplay.h"
#endif

class TGLabel;
class TGTextEntry;
class MEvtLoop;

class MParList;
class MTaskList;
class MReadTree;
class MGTask;

class MOnlineDisplay : public MStatusDisplay
{
    friend class MOnlineDump;
public:
    enum
    {
        kSlider = MStatusDisplay::kSearch + 1,
        kFreeze
    };

private:
    TGCompositeFrame *fTab1;
    TGCompositeFrame *fTab2;

    TCanvas *fCanvas;

    MGTask *fTask;

    void AddTopFramePart1(TGCompositeFrame *frame);
    void AddTopFramePart2(TGCompositeFrame *frame);
    void AddUserFrame();

    void UpdateDisplay();

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

public:
    MOnlineDisplay();

    void SetTask(MGTask *t) { fTask=t; }

    ClassDef(MOnlineDisplay, 0) // Display for camera images (cerenkov events)
};

#endif


