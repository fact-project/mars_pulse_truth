/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz  05/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// MLogo                                                                    //
//                                                                          //
// X based logo displayer. Displays a given logo after a call to Popup()    //
// until Popdown() or the destructor is called, but for a minimum of a      //
// given number of milliseconds.                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifdef HAVE_XPM
#include "MLogo.h"

#include <unistd.h>    // usleep
#include <iostream.h>  // cout
#include <sys/time.h>  // gettimeofday

#ifndef XpmSuccess
#define XpmSuccess       0
#endif
#ifndef XpmColorError
#define XpmColorError    1
#endif

MLogo::MLogo(int millisec)
   : fDisplay(0), fLogoWindow(0), fLogoPixmap(0), fMilliSec(millisec)
{
    // millisec: time of minimum stayup
}

void MLogo::Wait() const
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    tv.tv_sec  -= fPopupTime.tv_sec;
    tv.tv_usec -= fPopupTime.tv_usec;

    while (tv.tv_usec < 0)
    {
        tv.tv_usec += 1000000;
        tv.tv_sec--;
    }

    while (tv.tv_sec > 0)
        tv.tv_usec += 1000000;

    long sleep = fMilliSec*1000-tv.tv_usec;

    if (sleep <= 0)
        return;

    usleep(sleep);
}

Pixmap MLogo::GetLogo() const
{
#ifdef XpmVersion
    int depth = PlanesOfScreen(XDefaultScreenOfDisplay(fDisplay));

    if (depth <= 1)
        return 0;

    XWindowAttributes win_attr;
    XGetWindowAttributes(fDisplay, fLogoWindow, &win_attr);

    XpmAttributes attr;
    attr.valuemask = XpmVisual | XpmColormap | XpmDepth;
    attr.visual    = win_attr.visual;
    attr.colormap  = win_attr.colormap;
    attr.depth     = win_attr.depth;

#ifdef XpmColorKey              // Not available in XPM 3.2 and earlier

    switch (depth)
    {
    case 0:
        attr.valuemask &= ~XpmColorKey;
        break;
    case 1:
        attr.color_key = XPM_MONO;
        break;
    case 2:
        attr.color_key = XPM_GRAY;
        break;
    case 3:
    case 4:
        attr.color_key = XPM_GRAY4;
        break;
    default:
        attr.color_key = XPM_COLOR;
    }
#endif // defined(XpmColorKey)


    Pixmap logo;
#ifdef USE_MAGICLOGO
#include "../magiclogo.xpm"
    int ret = XpmCreatePixmapFromData(fDisplay, fLogoWindow,
                                      mag1, &logo, 0, &attr);
#else
#include "../marslogo.xpm"
    int ret = XpmCreatePixmapFromData(fDisplay, fLogoWindow,
                                      marslogo, &logo, 0, &attr);
#endif
    XpmFreeAttributes(&attr);

    if (ret == XpmSuccess || ret == XpmColorError)
        return logo;

    if (logo)
        XFreePixmap(fDisplay, logo);
#endif
    return 0;
}

void MLogo::Popup()
{
#ifdef XpmVersion
    if (fLogoWindow || fLogoPixmap || fDisplay)
#endif
        return;

    fDisplay = XOpenDisplay("");
    if (!fDisplay)
        return;

    int screen = DefaultScreen(fDisplay);

    fLogoWindow = XCreateSimpleWindow(fDisplay, DefaultRootWindow(fDisplay),
                                      -100, -100, 50, 50, 0,
                                      BlackPixel(fDisplay, screen),
                                      WhitePixel(fDisplay, screen));
    fLogoPixmap = GetLogo();

    Window root;
    int x, y;
    unsigned int w, h, bw, depth;
    XGetGeometry(fDisplay, fLogoPixmap,
                 &root, &x, &y, &w, &h, &bw, &depth);

    Screen *xscreen = XDefaultScreenOfDisplay(fDisplay);
    x = (WidthOfScreen(xscreen) - w) / 2;
    y = (HeightOfScreen(xscreen) - h) / 2;

    XMoveResizeWindow(fDisplay, fLogoWindow, x, y, w, h);
    XSync(fDisplay, False);   // make sure move & resize is done before mapping

    unsigned long valmask  = CWBackPixmap | CWOverrideRedirect;

    XSetWindowAttributes xswa;
    xswa.background_pixmap = fLogoPixmap;
    xswa.override_redirect = True;
    XChangeWindowAttributes(fDisplay, fLogoWindow, valmask, &xswa);

    XMapRaised(fDisplay, fLogoWindow);
    XSync(fDisplay, False);

    gettimeofday(&fPopupTime, 0);
}

void MLogo::Popdown()
{
    if (fLogoWindow && fLogoPixmap && fDisplay)
        Wait();

    if (fLogoWindow)
    {
        XUnmapWindow(fDisplay, fLogoWindow);
        XDestroyWindow(fDisplay, fLogoWindow);
        fLogoWindow = 0;
    }
    if (fLogoPixmap)
    {
        XFreePixmap(fDisplay, fLogoPixmap);
        fLogoPixmap = 0;
    }
    if (fDisplay)
    {
        XSync(fDisplay, False);
        XCloseDisplay(fDisplay);
        fDisplay = 0;
    }
}
#endif
