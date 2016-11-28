#ifndef MARS_MLogo
#define MARS_MLogo

#ifdef HAVE_XPM

#ifndef __CINT__

#include <X11/Xlib.h>

#include "Xpm.h"

class MLogo
{
    Display *fDisplay;         // display handle
    Window   fLogoWindow;      // window handle
    Pixmap   fLogoPixmap;      // pixmap handle
    long     fMilliSec;        // stayup time

    struct timeval fPopupTime; // time of popup

    void   Wait() const;
    Pixmap GetLogo() const;

public:
    MLogo(int millisec=777);
    ~MLogo()
    {
        Popdown();
    }

    void Popup();
    void Popdown();
};

#endif // __CINT__
#endif // HAVE_XPM
#endif // MARS_MLogo
