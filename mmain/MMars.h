#ifndef MARS_MMars
#define MARS_MMars

#ifndef MMARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif

class MGList;
class TGVerticalFrame;
class TGLayoutHints;

class MMars : public TGMainFrame
{
private:

    MGList *fList;

    void CreateTextButton(TGVerticalFrame *tab, const char *text,
                          const char *descr, const UInt_t id) const;

    void CreateMenuBar();
    void CreateTopFrame(TGHorizontalFrame *top);
    void CreateBottomFrame(TGHorizontalFrame *low);

    void DisplWarning(const char *txt);

public:
    MMars();
    ~MMars();

    void CloseWindow();

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MMars, 0) // GUI: Mars - the main window
};

#endif


