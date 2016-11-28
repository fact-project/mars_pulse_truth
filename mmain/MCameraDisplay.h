#ifndef MARS_MCameraDisplay
#define MARS_MCameraDisplay

#ifndef MARS_MBrowser
#include "MBrowser.h"
#endif

class MCameraDisplay : public MBrowser
{
public:
    MCameraDisplay(/*const TGWindow *main=NULL,*/ const TGWindow *p=NULL,
                   const UInt_t w=500, const UInt_t h=500) ;

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MCameraDisplay, 0) // GUI: The 'camera display' browser.
};

#endif


