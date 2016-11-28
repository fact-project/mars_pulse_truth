/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  7/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MagicSnake
// ----------
//
// Camera Display Games: Snake
//
// Start the game by:
//   MagicSnake snake;
//
// Controll the worm using right/left. Make sure, that the mouse pointer
// is inside the canvas.
//
// Move the mouse pointer outside the canvas to pause the game.
//
// The pixel colors have the following meaning:
// --------------------------------------------
//  Green:   Food, collect all packages.
//  Red:     Bombs, don't touch it!
//  Yellow:  Transport, try it.
//  Magenta: Home, touch it to win the game.
//
// To restart the game use the context menu. It can only be accessed if
// the game has been stopped (either because you win the game or because
// you hit a bomb) With the context menu you can also toggle between
// different camera layouts.
//
////////////////////////////////////////////////////////////////////////////
#include "MagicSnake.h"

#include <iostream>

#include <KeySymbols.h>

#include <TColor.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TRandom.h>
#include <TInterpreter.h>

#include "MH.h"

#include "MGeomPix.h"
#include "MGeomCamCT1.h"
#include "MGeomCamMagic.h"

ClassImp(MagicSnake);

using namespace std;

void MagicSnake::Free()
{
    if (!fGeomCam)
        return;

    delete fGeomCam;

    delete fArray;
}

void MagicSnake::ChangeCamera()
{
    if (!fDone)
        Done("Changing Camera...", 22);

    static Bool_t ct1=kFALSE;

    cout << "Change to " << (ct1?"Magic":"CT1") << endl;

    if (ct1)
        SetNewCamera(new MGeomCamMagic);
    else
        SetNewCamera(new MGeomCamCT1);

    ct1 = !ct1;

    Reset();
    AppendPad();
}

void MagicSnake::SetNewCamera(MGeomCam *geom)
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

    fColors.Set(fNumPixels);
}

void MagicSnake::Init()
{
    //
    // Make sure, that the object is destroyed when the canvas/pad is
    // destroyed. Make also sure, that the interpreter doesn't try to
    // delete it a second time.
    //
    SetBit(kCanDelete);
    gInterpreter->DeleteGlobal(this);

    Draw();

    Pause();

    fTimer.TurnOn();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicSnake::MagicSnake()
    : fTimer(this, 500, kTRUE), fGeomCam(NULL), fDone(NULL), fPaused(NULL)
{
    SetNewCamera(new MGeomCamMagic);
    Init();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicSnake::MagicSnake(const MGeomCam &geom)
    : fTimer(this, 500, kTRUE), fGeomCam(NULL), fDone(NULL), fPaused(NULL)
{
    SetNewCamera(static_cast<MGeomCam*>(geom.Clone()));
    Init();
}

void MagicSnake::Pause(Bool_t yes)
{
    if (yes && !fPaused)
    {
        fPaused = new TText(0, 0, "Paused!");
        fPaused->SetTextColor(kWhite);
        fPaused->SetTextAlign(22);
        fPaused->SetTextSize(0.05); // white
        fPaused->Draw();
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        fPaused->SetBit(kNoContextMenu|kCannotPick);
        ResetBit(kNoContextMenu);
#endif
        //fDrawingPad->Update();
    }

    if (yes)
        return;

    if (!fDone)
    {
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        //fDrawingPad->SetBit(kNoContextMenu);
        SetBit(kNoContextMenu);
#endif
    }
    if (!fPaused)
        return;

    //fDrawingPad->Update();

    delete fPaused;
    fPaused=NULL;
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MagicSnake::~MagicSnake()
{
    Free();
    Pause(kFALSE);

    if (fDone)
        delete fDone;
/*
    if (fDrawingPad->GetListOfPrimitives()->FindObject(this)==this)
    {
        fDrawingPad->RecursiveRemove(this);
        delete fDrawingPad;
    }*/
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MagicSnake::Paint(Option_t *opt)
{
    const Float_t r = fGeomCam->GetMaxRadius();

    MH::SetPadRange(-r, -r, r, r*1.1);

    TAttLine line;
    TAttFill fill;

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGeom &pix = (*fGeomCam)[i];

        fill.SetFillColor(fColors[i]);
        pix.PaintPrimitive(line, fill);
    }
}

// ------------------------------------------------------------------------
//
// Call this function to draw the camera layout into your canvas.
// Setup a drawing canvas. Add this object and all child objects
// (hexagons, etc) to the current pad. If no pad exists a new one is
// created.
//
void MagicSnake::Draw(Option_t *option)
{
    //
    // if no canvas is yet existing to draw into, create a new one
    //
    new TCanvas("MagicSnake", "Magic Snake", 0, 0, 800, 800);

    //fDrawingPad = gPad;
    gPad->SetBorderMode(0);

    //
    // Append this object, so that the aspect ratio is maintained
    // (Paint-function is called)
    //
    AppendPad(option);

    //
    // Reset the game pad
    //
    Reset();

    fShow.SetTextAlign(23);   // centered/bottom
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fShow.SetBit(kNoContextMenu|kCannotPick);
#endif
    fShow.Draw();
}

void MagicSnake::Update()
{
    TString txt = "Pixels: ";
    txt += fNumPixels;
    txt += "  Bombs: ";
    txt += fNumBombs;
    txt += "  Food: ";
    txt += fNumFood;

    fShow.SetText(0, fRange, txt);
}

// ------------------------------------------------------------------------
//
// reset the all pixel colors to a default value
//
void MagicSnake::Reset()
{
    fDirection = fGeomCam->InheritsFrom("MGeomCamCT1") ? kLeft : kRight;
    fLength = 2;

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        fColors[i] = kBackground;
        (*fGeomCam)[i].ResetBit(kUserBits);
    }

    (*fGeomCam)[0].SetBit(kHasWorm);

    fNumBombs = fNumPixels/30;

    TRandom rnd(0);
    for (int i=0; i<fNumBombs; i++)
    {
        Int_t idx;

        do idx = (Int_t)rnd.Uniform(fNumPixels);
        while ((*fGeomCam)[idx].TestBits(kHasBomb|kHasWorm));

        (*fGeomCam)[idx].SetBit(kHasBomb);
        fColors[idx] = kRed;
    }

    fNumFood = fNumPixels/6;
    // FIXME. gROOT->GetColor doesn't allow more than 100 colors!
    if (fNumFood>46)
        fNumFood=46;

    fArray = new Int_t[fNumFood+3];

    fArray[0] = 0;
    fArray[1] = 1;

    for (int i=0; i<fNumFood+3; i++)
    {
        Float_t f = (float)i/(fNumFood+3);
        gROOT->GetColor(51+i)->SetRGB(f, f, f);
    }
    for (int i=0; i<fNumFood; i++)
    {
        Int_t idx;

        do idx = (Int_t)rnd.Uniform(fNumPixels);
        while ((*fGeomCam)[idx].TestBits(kHasBomb|kHasFood|kHasWorm));

        (*fGeomCam)[idx].SetBit(kHasFood);
        fColors[idx] = kGreen;
    }

    SetWormColor();

    for (int i=0; i<2; i++)
    {
        Int_t idx;

        do idx = (Int_t)rnd.Uniform(fNumPixels);
        while ((*fGeomCam)[idx].TestBits(kHasBomb|kHasFood|kHasWorm|kHasTransport));

        fTransport[i] = idx;
        (*fGeomCam)[idx].SetBit(kHasTransport);
        fColors[idx] = kYellow;
    }

    gPad->SetFillColor(22);

#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    //fDrawingPad->SetBit(kNoContextMenu);
    SetBit(kNoContextMenu);
#endif

    if (!fDone)
        return;

    delete fDone;
    fDone = NULL;
}

void MagicSnake::Done(TString txt, Int_t col)
{
    //(*this)[fArray[fLength-1]].SetFillColor(kBlue);

    fDone = new TText(0, 0, txt);
    fDone->SetTextColor(kWhite);  // white
    fDone->SetTextAlign(22);  // centered/centered
    fDone->SetTextSize(0.05); // white
    fDone->Draw();
    gPad->SetFillColor(col);
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fDone->SetBit(kNoContextMenu|kCannotPick);
    ResetBit(kNoContextMenu);
#endif
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MagicSnake::ExecuteEvent(Int_t event, Int_t keycode, Int_t keysym)
{
    if (event==kMouseEnter || event==kMouseMotion)
    {
        Pause(kFALSE);
        return;
    }

    if (fDone)
        return;

    if (event==kMouseLeave)
    {
        Pause();
        return;
    }
    if (event!=kKeyPress)
        return;

    switch (keysym)
    {
    case kKey_Left:
        fDirection --;
        break;

    case kKey_Right:
        fDirection ++;
        break;

    case kKey_Escape:
        Done("Reset...", 22);
        Reset();
        return;

    default:
        cout << "Keysym=0x" << hex << keysym << endl;
    }

     if (fDirection < kRightTop)
         fDirection = kLeftTop;
     if (fDirection > kLeftTop)
         fDirection = kRightTop;
}

void MagicSnake::Step(Int_t newpix)
{
    if ((*fGeomCam)[newpix].TestBit(kHasTransport))
    {
        fColors[fArray[0]] = kBackground;
        (*fGeomCam)[fArray[0]].ResetBit(kHasWorm);

        for (int i=1; i<fLength; i++)
            fArray[i-1] = fArray[i];

        fArray[fLength-1] = newpix==fTransport[0]?fTransport[1]:fTransport[0];

        return;
    }

    if (!(*fGeomCam)[newpix].TestBit(kHasFood))
    {
        MGeom &pix = (*fGeomCam)[fArray[0]];

        if (!pix.TestBit(kHasTransport))
        {
            if (pix.TestBit(kHasDoor))
                fColors[fArray[0]] = kMagenta;
            else
                fColors[fArray[0]] = kBackground;
        }

        pix.ResetBit(kHasWorm);

        for (int i=1; i<fLength; i++)
            fArray[i-1] = fArray[i];

        fArray[fLength-1] = newpix;
    }
    else
    {
        fArray[fLength++] = newpix;
        (*fGeomCam)[newpix].ResetBit(kHasFood);

        fNumFood--;

        if (fNumFood==0)
            for (int i=1; i<7; i++)
            {
                fColors[i] = kMagenta;
                (*fGeomCam)[i].SetBit(kHasDoor);
            }
    }

    SetWormColor();
}

void MagicSnake::SetWormColor()
{
    for (int i=0; i<fLength; i++)
    {
        const Int_t idx = fArray[i];

        MGeom &pix = (*fGeomCam)[idx];

        if (pix.TestBit(kHasTransport))
            continue;

        pix.SetBit(kHasWorm);

        fColors[idx] = 51+fLength-i;
    }
}

Int_t MagicSnake::ScanNeighbours()
{
    const Int_t first = fArray[fLength-1];

    const MGeom &pix=(*fGeomCam)[first];

    Double_t dx = pix.GetX();
    Double_t dy = pix.GetY();

    Int_t newpix = -1;
    for (int i=0; i<pix.GetNumNeighbors(); i++)
    {
        const Int_t     idx  = pix.GetNeighbor(i);
        const MGeom &next = (*fGeomCam)[idx];

        const Double_t x = next.GetX();
        const Double_t y = next.GetY();

        switch (fDirection)
        {
        case kRightTop:    if (x>=dx && y>dy) { newpix=idx; dy=y; } continue;
        case kRight:       if (x>dx)          { newpix=idx; dx=x; } continue;
        case kRightBottom: if (x>=dx && y<dy) { newpix=idx; dy=y; } continue;
        case kLeftTop:     if (x<=dx && y>dy) { newpix=idx; dy=y; } continue;
        case kLeft:        if (x<dx)          { newpix=idx; dx=x; } continue;
        case kLeftBottom:  if (x<=dx && y<dy) { newpix=idx; dy=y; } continue;
        }
    }

    if (newpix<0)
        return -1;

    const MGeom &np = (*fGeomCam)[newpix];

    if (fNumFood==0 && np.TestBit(kHasDoor))
        return -4;

    if (np.TestBit(kHasBomb))
        return -2;

    if (np.TestBit(kHasWorm))
        return -3;

    return newpix;
}

Bool_t MagicSnake::HandleTimer(TTimer *timer)
{     
    if (fDone || fPaused)
        return kTRUE;

    const Int_t newpix = ScanNeighbours();

    switch (newpix)
    {
    case -1:
        Done("You crashed! Don't drink and drive!", kRed);
        break;
    case -2:
        Done("Ouch, you found the bomb!", kRed);
        break;
    case -3:
        Done("Argh... don't eat yourself!", kRed);
        break;
    case -4:
        Done("Congratulations! You won the game!", kGreen);
        break;
    default:
        Step(newpix);
    }

    Update();

    gPad->Modified();
    gPad->Update();

    return kTRUE;
}
