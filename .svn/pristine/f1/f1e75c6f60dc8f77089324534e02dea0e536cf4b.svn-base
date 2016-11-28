#ifndef MARS_MProgressBar
#define MARS_MProgressBar

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif

class MGList;
class TGProgressBar;
class TGHProgressBar;

class MProgressBar : public TGTransientFrame
{
private:
    MGList         *fList;
    TGHProgressBar *fBar;

public:
     MProgressBar();
     virtual ~MProgressBar();

     TGProgressBar *GetBar() const { return (TGProgressBar*)fBar; }

     ClassDef(MProgressBar, 0)   // A simple progress bar window for the Eventloop
};

#endif


