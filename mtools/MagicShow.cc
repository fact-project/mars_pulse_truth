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
!   Author(s): Thomas Bretz 07/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MagicShow
// ---------
//
// Tool to visualize Next Neighbours.
//
// Start the show by:
//   MagicShow show;
//
// Use the following keys:
// -----------------------
//
//   * Space:
//     Toggle between auto increment and manual increment
//
//   * Right/Left:
//     Increment/Decrement pixel number by 1
//
//   * Right/Left:
//     Increment/Decrement pixel number by 1
//
//   * Up/Down:
//     Increment/Decrement pixel number by 10
//
//   * PageUp/PageDown:
//     Increment/Decrement pixel number by 100
//
//   * Home/End:
//     Jump to first/last pixel
//
////////////////////////////////////////////////////////////////////////////
#include "MagicShow.h"

#include <iostream>

#include <KeySymbols.h>

#include <TCanvas.h>
#include <TInterpreter.h>

#include "MH.h"

#include "MGeomPix.h"
#include "MGeomCamCT1.h"
#include "MGeomCamMagic.h"

ClassImp(MagicShow);

using namespace std;

// ------------------------------------------------------------------------
//
// Free all onbects connected to a special camera geometry
//
void MagicShow::Free()
{
    if (!fGeomCam)
        return;

    delete fGeomCam;
}

// ------------------------------------------------------------------------
//
// Change camera from Magic to CT1 and back
//
void MagicShow::ChangeCamera()
{
    static Bool_t ct1=kFALSE;

    cout << "Change to " << (ct1?"Magic":"CT1") << endl;

    if (ct1)
        SetNewCamera(new MGeomCamMagic);
    else
        SetNewCamera(new MGeomCamCT1);

    ct1 = !ct1;

    Update();

    fColors.Reset();

    // FIXME: Reset all texts

    AppendPad();
}

// ------------------------------------------------------------------------
//
// Reset/set all veriables needed for a new camera geometry
//
void MagicShow::SetNewCamera(MGeomCam *geom)
{
    Free();

    //
    //  Set new camera
    //
    fGeomCam = geom;

    //
    //  create the hexagons of the display
    //
    fNumPixels = fGeomCam->GetNumPixels();
    fRange     = fGeomCam->GetMaxRadius();

    fNumPixel  = 0;//fNumPixels-1;

    fColors.Set(fNumPixels);

    for (int i=0; i<6; i++)
    {
        fText[i] = new TText(0, 0, "");
        fText[i]->SetTextFont(122);
        fText[i]->SetTextAlign(22);   // centered/centered
    }

}

void MagicShow::Init()
{
    //
    // Make sure, that the object is destroyed when the canvas/pad is
    // destroyed. Make also sure, that the interpreter doesn't try to
    // delete it a second time.
    //
    SetBit(kCanDelete);
    gInterpreter->DeleteGlobal(this);

    Draw();

    fTimer.TurnOn();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicShow::MagicShow()
    : fTimer(this, 250, kTRUE), fGeomCam(NULL), fNumPixel(-1), fAuto(kTRUE)
{
    SetNewCamera(new MGeomCamMagic);
    Init();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicShow::MagicShow(const MGeomCam &geom)
    : fTimer(this, 250, kTRUE), fGeomCam(NULL), fNumPixel(-1), fAuto(kTRUE)
{
    SetNewCamera(static_cast<MGeomCam*>(geom.Clone()));
    Init();
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MagicShow::~MagicShow()
{
    Free();

    for (int i=0; i<6; i++)
        delete fText[i];
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MagicShow::Paint(Option_t *opt)
{
    const Float_t r = fGeomCam->GetMaxRadius();

    MH::SetPadRange(-r, -r, r, r*1.1);

    TAttLine line;
    TAttFill fill;

    // FIXME:
    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGeom &pix = (*fGeomCam)[i];

        fill.SetFillColor(fColors[i]);
        pix.PaintPrimitive(line, fill);
    }

    for (int i=0; i<6; i++)
        fText[i]->Paint();
}

// ------------------------------------------------------------------------
//
// Call this function to draw the camera layout into your canvas.
// Setup a drawing canvas. Add this object and all child objects
// (hexagons, etc) to the current pad. If no pad exists a new one is
// created.
//
void MagicShow::Draw(Option_t *option)
{
    //
    // if no canvas is yet existing to draw into, create a new one
    //
    new TCanvas("MagicShow", "Magic Show Next Neighbours", 0, 0, 800, 800);

    gPad->SetBorderMode(0);
    gPad->SetFillColor(22);

    //
    // Append this object, so that the aspect ratio is maintained
    // (Paint-function is called)
    //
    AppendPad(option);

    fShow.SetTextAlign(23);   // centered/bottom
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fShow.SetBit(kNoContextMenu|kCannotPick);
#endif
    fShow.Draw();
}

// ------------------------------------------------------------------------
//
// Update Status text
//
void MagicShow::Update()
{
    TString txt = "Pixels: ";
    txt += fNumPixels;
    txt += "  Pixel: ";
    txt += fNumPixel;

    if (fAuto)
        txt += "  (auto)";

    fShow.SetText(0, fRange, txt);
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MagicShow::ExecuteEvent(Int_t event, Int_t keycode, Int_t keysym)
{
    if (event!=kKeyPress)
        return;

    switch (keysym)
    {
    case kKey_Space:
        fAuto = !fAuto;
        Update();
        gPad->Update();
        return;

    case kKey_Right:
        ChangePixel(+1);
        return;

    case kKey_Left:
        ChangePixel(-1);
        return;

    case kKey_Up:
        ChangePixel(+10);
        return;

    case kKey_Down:
        ChangePixel(-10);
        return;

    case kKey_PageUp:
        ChangePixel(+100);
        return;

    case kKey_PageDown:
        ChangePixel(-100);
        return;

    case kKey_Home:
        ChangePixel(-fNumPixel);
        return;

    case kKey_End:
        ChangePixel(fNumPixels-fNumPixel-1);
        return;
    }
}

// ------------------------------------------------------------------------
//
// Change the shown pixel by add indices
//
void MagicShow::ChangePixel(Int_t add)
{
    const MGeom &pix1=(*fGeomCam)[fNumPixel];

    fColors[fNumPixel] = kBackground;
    for (int i=0; i<pix1.GetNumNeighbors(); i++)
    {
        fColors[pix1.GetNeighbor(i)] = kBackground;
        if (TString(fText[i]->GetTitle()).IsNull())
            continue;

        fText[i]->SetText(0, 0, "");
    }

    fNumPixel += add;

    if (fNumPixel>=fNumPixels)
        fNumPixel = 0;
    if (fNumPixel<0)
        fNumPixel = fNumPixels-1;

    const MGeom &pix2=(*fGeomCam)[fNumPixel];

    fColors[fNumPixel] = kBlue;

    for (int i=0; i<pix2.GetNumNeighbors(); i++)
    {
        Int_t idx = pix2.GetNeighbor(i);

        fColors[idx] = kMagenta;

        TString num;
        num += idx;

        const MGeom &pix=(*fGeomCam)[idx];

        fText[i]->SetText(pix.GetX(), pix.GetY(), num);
        fText[i]->SetTextSize(0.3*pix.GetT()/fGeomCam->GetMaxRadius());
    }

    Update();

    gPad->Update();
}

// ------------------------------------------------------------------------
//
// If automatic is switched on step one pixel forward
//
Bool_t MagicShow::HandleTimer(TTimer *timer)
{
    if (fAuto)
        ChangePixel(+1);

    return kTRUE;
}
