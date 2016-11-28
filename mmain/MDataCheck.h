#ifndef MARS_MDataCheck
#define MARS_MDataCheck

#ifndef MARS_MBrowser
#include "MBrowser.h"
#endif

#ifndef MARS_MHFadcPix
#include "MHFadcPix.h"
#endif

class MDataCheck : public MBrowser
{
private:
    void View(const char *inputfile, const char *treeName, MHFadcPix::Type_t t);

public:
    MDataCheck(/*const TGWindow *main=NULL,*/ const TGWindow *p=NULL,
               const UInt_t w=500, const UInt_t h=500) ;

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(MDataCheck, 0) // GUI: The 'data-check' browser.
};

#endif


