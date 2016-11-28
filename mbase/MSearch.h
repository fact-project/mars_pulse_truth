#ifndef MARS_MSearch
#define MARS_MSearch

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif

#ifndef ROOT_TGWidget
#include <TGWidget.h>
#endif

enum EMarsWidgetMessageTypes {
    kS_START = 1
};

class MGList;

class MSearch : public TGTransientFrame, public TGWidget
{
private:
    MGList *fList;

    Bool_t SendSearch();
    void CloseWindow() { delete this; }

public:
    MSearch(const TGWindow *w, Int_t id=-1);
    virtual ~MSearch();

    Bool_t ProcessMessage(Long_t msg, Long_t mp1, Long_t mp2);

    ClassDef(MSearch, 0)   // A simple progress bar window for the Eventloop
};

#endif


