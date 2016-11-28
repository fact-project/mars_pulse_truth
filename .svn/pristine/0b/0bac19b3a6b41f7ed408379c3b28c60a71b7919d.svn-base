#ifndef MARS_MGTask
#define MARS_MGTask

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MTask                                                                   //
//                                                                         //
// Abstract base class for a task                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;

class TGLayoutHints;
class TGCompositeFrame;

class MGGroupFrame;

class MGTask : public MTask
{
private:
    MGGroupFrame *fFrame;     //!

    virtual void CreateGuiElements(MGGroupFrame *) {}

    void ShowGui();
    void HideGui();

protected:
    TObject *FindWidget(Int_t id) const;

public:
    MGTask(const char *name=NULL, const char *title=NULL);
    virtual ~MGTask();

    Int_t CallPreProcess(MParList *plist);
    Int_t CallPostProcess();

    virtual Bool_t ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2);

    void CreateGui(TGCompositeFrame *f, TGLayoutHints *layout=NULL);

    ClassDef(MGTask, 0) // Abstract base class for a task having a gui
};

#endif
