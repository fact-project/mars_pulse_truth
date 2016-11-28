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
!   Author(s): Thomas Bretz, 05/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2002-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MGMap
//  =====
//
// This Map of TObjects connects TObjects with a TString. It can be used
// to create maps which displays tooltips it the mouse is above the objects.
//
// It is also a tool to convert TObjects which are drawn into
// a bitmap or to draw them into a TGFrame. Because in both cases the
// support for drawing such object must be programmed explicitly only
// simple objects (TLine, TMarker, etc) are supported.
//
//////////////////////////////////////////////////////////////////////////////
#include "MGMap.h"

#include <climits>  // INT_MAX (Ubuntu 8.10)

#include <TMath.h>

#include <TPad.h>    // gPad, TPad::GetMaxDistance()

#include <TLine.h>
#include <TMarker.h>

#include <TGToolTip.h>

ClassImp(MGMap);

using namespace std;

//
// THIS IS A WORKAROUND TO GET A MORE DIRECT ACCESS TO TGX11
//
/*
#include <TGX11.h>
class MGX11 : public TGX11
{
public:
    ULong_t GetGc(Int_t which) const
    {
        return (ULong_t)*TGX11::GetGC(which);
    }
    void DrawLine(Drawable_t id, TObject *o, Int_t dx, Int_t dy, Float_t r)
    {
        TLine *l = dynamic_cast<TLine*>(o);
        if (!l)
            return;

        SetLineColor(l->GetLineColor());
        SetLineWidth(l->GetLineWidth());
        SetLineStyle(l->GetLineStyle());

        if (l->GetLineColor()==kRed)
            SetLineColor(50);
        if (l->GetLineColor()==kBlue)
            SetLineColor(9);

        gVirtualX->DrawLine(id, GetGc(0),
                            dx+(l->GetX1()/r), dy-(l->GetY1()/r),
                            dx+(l->GetX2()/r), dy-(l->GetY2()/r));
    }
    void DrawMarker(Drawable_t id, TObject *o, Int_t dx, Int_t dy, Float_t r)
    {
        TMarker *m = dynamic_cast<TMarker*>(o);
        if (!m)
            return;

        SetLineColor(m->GetMarkerColor());
        SetLineStyle(kSolid);
        SetLineWidth(1);

        const Double_t x = dx+(m->GetX()/r);
        const Double_t y = dy-(m->GetY()/r);
        const Int_t l = (Int_t)(m->GetMarkerSize()*5)+1;

        switch (m->GetMarkerStyle())
        {
        case kPlus:
            gVirtualX->DrawLine(id, GetGc(0), x-l, y, x+l, y);
            gVirtualX->DrawLine(id, GetGc(0), x, y-l, x, y+l);
            break;
        case kCircle:
            for (int i=0; i<8; i++)
                gVirtualX->DrawLine(id, GetGc(0),
                                    x+l*cos(i*TMath::TwoPi()/8),
                                    y+l*sin(i*TMath::TwoPi()/8),
                                    x+l*cos(((i+1)%8)*TMath::TwoPi()/8),
                                    y+l*sin(((i+1)%8)*TMath::TwoPi()/8));
            break;
        case kCross:
            gVirtualX->DrawLine(id, GetGc(0), x-l, y-l, x+l, y+l);
            gVirtualX->DrawLine(id, GetGc(0), x-l, y+l, x+l, y-l);
            break;
        }
    }
};
*/
// --------------------------------------------------------------------------
//
// Constructor. For more details see TExMap
//
MGMap::MGMap(Int_t mapSize) : TExMap(mapSize)//, fToolTip(0)
{
//    fToolTip = new TGToolTip(0, "", 0);
}

// --------------------------------------------------------------------------
//
// Destructor. Deletes all objects of the map if kIsOwner is set via
// SetOwner.
//
MGMap::~MGMap()
{
    if (TestBit(kIsOwner))
        Delete();
}

// --------------------------------------------------------------------------
//
// Add an TObject to be drawn and if necessary a corresponding TString
// to the Map. You must take care of deleting both objects if SetOwner()
// was not called. Otherwise MGMap takes the ownership of the objects.
//
void MGMap::Add(TObject *k, TString *v)
{
    TExMap::Add((ULong_t)GetSize(), (Long_t)k, (Long_t)v);
}

// --------------------------------------------------------------------------
//
// Delete all objects stored in the TExMap
//
void MGMap::Delete(Option_t *)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, val;
#else
    Long64_t key, val;
#endif

    TExMapIter map(this);
    while (map.Next(key, val))
    {
        delete (TObject*)(key);
        if (val)
            delete (TString*)(val);
        /*
         Long_t key2, val2;
         TExMapIter map2(&fMapG);
         while (map2.Next(key2, val2))
         if (val==val2)
         {
         delete (TObject*)key;
         fMapG.Remove(key);
         }
         */
    }
    TExMap::Delete();
}

// --------------------------------------------------------------------------
//
// Paint all TObjects (which are supported) to a drawable with Id id.
// Scale is the distance of the center of your drawable to one edge in
// user coordinates.
//
// FIXME: Currently the width and height is hardcoded to 768x576 -
//        find a way to get it from the drawable.
//
/*
void MGMap::Paint(Drawable_t id, Float_t scale)
{
    if (id==0 && gPad)
        id = gVirtualX->GetWindowID(gPad->GetPixmapID());

    const Int_t w = 768;
    const Int_t h = 576;

    scale /= TMath::Hypot((float)w, (float)h)/2;

    Long_t key, val;
    TExMapIter map(this);
    while (map.Next(key, val))
    {
        TObject *o = (TObject*)key;
        ((MGX11*)gVirtualX)->DrawLine(id,   o, w/2, h/2, scale);
        ((MGX11*)gVirtualX)->DrawMarker(id, o, w/2, h/2, scale);
    }
}
*/

// --------------------------------------------------------------------------
//
// Convert root colors to arbitrary bitmap coordinates
//
UChar_t MGMap::Color(int col)
{
    switch (col)
    {
    case kBlack:  return 0;
    case kWhite:  return 0xff;
    case kYellow: return 0x0f;
    case kRed:    return 2;
    case kGreen:  return 2<<2;
    case kBlue:   return 2<<4;
    default:
        return 0;
    }
}

// --------------------------------------------------------------------------
//
// Draw a line into the buffer (size w*h) from (x1, y1) to (x2, y2) with
// the color col and the line style style (default: solid)
//
void MGMap::DrawLine(UChar_t *buf, int w, int h, Float_t x1, Float_t y1, Float_t x2, Float_t y2, UChar_t col, Int_t style)
{
    const Int_t    step = style==kSolid?1:3;
    const Double_t len  = TMath::Hypot(x2-x1, y2-y1);
    const Double_t dx   = (x2-x1)/len*step;
    const Double_t dy   = (y2-y1)/len*step;

    Double_t x = x1;
    Double_t y = y1;

    for (int i=0; i<len; i+=step)
    {
        x+= dx;
        y+= dy;

        const Int_t iy = TMath::Nint(y);
        if (iy<0 || iy>=h)
            continue;

        const Int_t ix = TMath::Nint(x);
        if (ix<0 || ix>=w)
            continue;

        buf[ix+iy*w] = col;
    }
}

// --------------------------------------------------------------------------
//
// Draw a box into the buffer (size w*h) from (x1, y1) to (x2, y2) with
// the color col and the line style style (default: solid)
//
void MGMap::DrawBox(UChar_t *buf, int w, int h, Float_t x1, Float_t y1, Float_t x2, Float_t y2, UChar_t col, Int_t style)
{
    DrawLine(buf, w, h, x1, y1, x2, y1, col, style);
    DrawLine(buf, w, h, x1, y2, x2, y1, col, style);
    DrawLine(buf, w, h, x1, y1, x1, y2, col, style);
    DrawLine(buf, w, h, x2, y1, x2, y2, col, style);
}

// --------------------------------------------------------------------------
//
// Draw a hexagon into the buffer (size w*h) around (x, y) with radius r and
// the color col.
//
void MGMap::DrawHexagon(UChar_t *buf, int w, int h, Float_t px, Float_t py, Float_t d, UChar_t col, Int_t style)
{
    const Int_t np = 6;

    const Double_t dy[np+1] = { .5   , 0.    , -.5   , -.5   , 0.    ,  .5   , .5    };
    const Double_t dx[np+1] = { .2886,  .5772,  .2886, -.2886, -.5772, -.2886, .2886 };

    //
    //  calculate the positions of the pixel corners
    //
    Double_t x[np+1], y[np+1];
    for (Int_t i=0; i<np+1; i++)
    {
        x[i] = px + dx[i]*d;
        y[i] = py + dy[i]*d;
    }

    for (int i=0; i<6; i++)
        DrawLine(buf, w, h, x[i], y[i], x[i+1], y[i+1], col, style);
}

// --------------------------------------------------------------------------
//
// Draw a circle into the buffer (size w*h) around (x, y) with radius r and
// the color col.
//
void MGMap::DrawCircle(UChar_t *buf, int w, int h, Float_t x, Float_t y, Float_t r, UChar_t col)
{
    const Int_t n = TMath::Nint(sqrt(2.)*r*TMath::Pi()/2);
    for (int i=0; i<n-1; i++)
    {
        const Double_t angle = TMath::TwoPi()*i/n;

        const Double_t dx = r*cos(angle);
        const Double_t dy = r*sin(angle);

        const Int_t x1 = TMath::Nint(x+dx);
        const Int_t x2 = TMath::Nint(x-dx);

        const Int_t y1 = TMath::Nint(y+dy);
        if (y1>=0 && y1<h)
        {
            if (x1>=0 && x1<w)
                buf[x1+y1*w] = col;

            if (x2>=0 && x2<w)
                buf[x2+y1*w] = col;
        }

        const Int_t y2 = TMath::Nint(y-dy);
        if (y2>=0 && y2<h)
        {
            if (x1>=0 && x1<w)
                buf[x1+y2*w] = col;

            if (x2>=0 && x2<w)
                buf[x2+y2*w] = col;
        }
    }
}

// --------------------------------------------------------------------------
//
// Draw a dot into the buffer (size w*h) at (x, y) with color col.
//
void MGMap::DrawDot(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, UChar_t col)
{
    const Int_t x1 = TMath::Nint(cx);
    const Int_t y1 = TMath::Nint(cy);

    if (x1>=0 && y1>=0 && x1<w && y1<h)
        buf[x1+y1*w] = col;
}

// --------------------------------------------------------------------------
//
// Draw a line into the buffer. The TObject must be a TLine.
// Currently only solid and non sloid line are supported.
//
void MGMap::DrawLine(TObject *o, UChar_t *buf, int w, int h, Double_t scale)
{
    TLine *l = dynamic_cast<TLine*>(o);
    if (!l)
        return;

    const Double_t x1 = 0.5*w-(l->GetX1()/scale);
    const Double_t x2 = 0.5*w-(l->GetX2()/scale);
    const Double_t y1 = 0.5*h-(l->GetY1()/scale);
    const Double_t y2 = 0.5*h-(l->GetY2()/scale);

    const Int_t col = Color(l->GetLineColor());
    DrawLine(buf, w, h, x1, y1, x2, y2, col, l->GetLineStyle());
}

void MGMap::DrawMultiply(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, Float_t size, UChar_t col)
{
    DrawLine(buf, w, h, cx-size, cy-size, cx+size, cy+size, col);
    DrawLine(buf, w, h, cx+size, cy-size, cx-size, cy+size, col);
}

void MGMap::DrawCross(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, Float_t size, UChar_t col)
{
    DrawLine(buf, w, h, cx-size, cy, cx+size, cy, col);
    DrawLine(buf, w, h, cx, cy-size, cx, cy+size, col);
}

// --------------------------------------------------------------------------
//
// Draw marker into the buffer. The TObject must be a TMarker.
// Currently kCircle, kMultiply and KDot are supported.
//
void MGMap::DrawMarker(TObject *o, UChar_t *buf, int w, int h, Double_t scale)
{
    TMarker *m = dynamic_cast<TMarker*>(o);
    if (!m)
        return;

    Double_t x = 0.5*w-(m->GetX()/scale);
    Double_t y = 0.5*h-(m->GetY()/scale);

    Int_t col = Color(m->GetMarkerColor());

    switch (m->GetMarkerStyle())
    {
    case kCircle:
        DrawCircle(buf, w, h, x, y, m->GetMarkerSize()*2+1, col);
        break;
    case kDot:
        DrawDot(buf, w, h, x, y, col);
        break;
    case kMultiply:
        DrawMultiply(buf, w, h, x, y, m->GetMarkerSize()*2+1, col);
        break;
    case kCross:
        DrawCross(buf, w, h, x, y, m->GetMarkerSize()*2+1, col);
        break;
    }
}

// --------------------------------------------------------------------------
//
// Paint all elements to the pad by calling their Paint() function
//
void MGMap::Paint(Option_t *)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, val;
#else
    Long64_t key, val;
#endif

    TExMapIter map(this);
    while (map.Next(key, val))
        ((TObject*)key)->Paint();
}

// --------------------------------------------------------------------------
//
// Paint all objects into a buffer of w*h UChar_ts. The scale
// gives you the conversio factor to convert pad coordinates into
// buffer pixels - it is the distance from the center of the buffer
// to one of its edges.
//
void MGMap::Paint(UChar_t *buf, int w, int h, Float_t scale)
{
    scale /= TMath::Hypot((float)w, (float)h)/2;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, val;
#else
    Long64_t key, val;
#endif

    TExMapIter map(this);
    while (map.Next(key, val))
    {
        TObject *o = (TObject*)key;
        DrawLine(o, buf, w, h, scale);
        DrawMarker(o, buf, w, h, scale);
    }
}

// --------------------------------------------------------------------------
//
// Search for an object at px, py. Return the pointer to it
// if found. Set str accordingly if a corresponding TString is found.
//
TObject *MGMap::PickObject(Int_t px, Int_t py, TString &str) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, val;
#else
    Long64_t key, val;
#endif

    TExMapIter map(this);
    while (map.Next(key, val))
    {
        if (!val)
            continue;

        TObject *o=(TObject*)key;
        if (o->DistancetoPrimitive(px, py)>TPad::GetMaxPickDistance())
            continue;

        str = *(TString*)val;
        return o;
    }
    return NULL;
}

// ------------------------------------------------------------------------
//
// Returns string containing info about the object at position (px,py).
// Returned string will be re-used (lock in MT environment).
// The text will be truncated to 128 charcters
//
char *MGMap::GetObjectInfo(Int_t px, Int_t py) const
{
    TString str;
    PickObject(px, py, str);

    static char txt[129];
    txt[128]=0;

    return strncpy(txt, str.Data(), 128);
}

// --------------------------------------------------------------------------
//
// Calculate distance to primitive by checking all gui elements
//
Int_t MGMap::DistancetoPrimitive(Int_t px, Int_t py)
{
    Int_t min = INT_MAX;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, val;
#else
    Long64_t key, val;
#endif

    TExMapIter map(this);
    while (map.Next(key, val))
    {
        TObject *o=(TObject*)key;

        const Int_t d = o->DistancetoPrimitive(px, py);

        if (d<TPad::GetMaxPickDistance())
            return 0;

        if (d<min)
            min=d;
    }

    return min;
}

// --------------------------------------------------------------------------
//
// Displays a tooltip
//
/*
void MGMap::ShowToolTip(Int_t px, Int_t py, const char *txt)
{
    if (TestBit(kNoToolTips))
        return;

    Int_t x=0;
    Int_t y=0;

    const Window_t id1 = gVirtualX->GetWindowID(gPad->GetCanvasID());
    const Window_t id2 = fToolTip->GetParent()->GetId();

    Window_t id3;
    gVirtualX->TranslateCoordinates(id1, id2, px, py, x, y, id3);

    // Show tool tip
    fToolTip->SetText(txt);
    fToolTip->Show(x+4, y+4);
}
*/

// --------------------------------------------------------------------------
//
// This function was connected to all created canvases. It is used
// to redirect GetObjectInfo into our own status bar.
//
// The 'connection' is done in Draw. It seems that 'connected'
// functions must be public.
//
/*
void MGMap::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
    TVirtualPad *c = (TVirtualPad*)gTQSender; // gTQSender==TCanvas

    gPad = c ? c->GetSelectedPad() : NULL;
    if (!gPad)
        return;

    // Try to find a corresponding object with kCannotPick set and
    // an available TString (for a tool tip)
    TString str;
    if (!selected || selected==this)
        selected =  PickObject(px, py, str);

    if (!selected)
        return;

    // Handle some gui events
    switch (event)
    {
    case kMouseMotion:
        if (!fToolTip->IsMapped() && !str.IsNull())
            ShowToolTip(px, py, str);
        break;

    case kMouseLeave:
        if (fToolTip->IsMapped())
            fToolTip->Hide();
        break;

    default:
        ExecuteEvent(event, px, py);
        break;
    }
}
*/

/*
void MAstroCatalog::RecursiveRemove(TObject *obj)
{
    ULong_t hash;
    Long_t key, val;

    TExMapIter map(&fMapG);
    while (map.Next(hash, key, val))
    {
        if (key != (Long_t)obj)
            continue;

        fMapG.Remove(hash, key);
        delete (TObject*)(key);
        if (val)
            delete (TString*)(val);
        break;
    }
}
*/
