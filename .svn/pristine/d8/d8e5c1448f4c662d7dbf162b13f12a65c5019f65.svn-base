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
// MagicCivilization
// ---------
//
// Tool to visualize Next Neighbours.
//
// Start the show by:
//   MagicCivilization show;
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
#include "MagicCivilization.h"

#include <iostream>

#include <KeySymbols.h>

#include <TCanvas.h>
#include <TRandom.h>
#include <TInterpreter.h>

#include "MH.h"

#include "MGeomPix.h"
#include "MGeomCamCT1.h"
#include "MGeomCamMagic.h"

ClassImp(MagicCivilization);

using namespace std;

void MagicCivilization::Free()
{
    if (!fGeomCam)
        return;

    delete fGeomCam;
}

void MagicCivilization::ChangeCamera()
{
    static Bool_t ct1=kFALSE;

    cout << "Change to " << (ct1?"Magic":"CT1") << endl;

    if (ct1)
        SetNewCamera(new MGeomCamMagic);
    else
        SetNewCamera(new MGeomCamCT1);

    ct1 = !ct1;

    AppendPad();
}

void MagicCivilization::SetNewCamera(MGeomCam *geom)
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

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicCivilization::MagicCivilization(Byte_t lim, UShort_t init)
    : fTimer(this, 500, kTRUE), fGeomCam(NULL), fNumInit(init), fLimit(lim)
{
    SetNewCamera(new MGeomCamMagic);

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
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MagicCivilization::~MagicCivilization()
{
    fTimer.TurnOff();
    Free();
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MagicCivilization::Paint(Option_t *opt)
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
 /*
    for (int i=0; i<6; i++)
        fText[i]->Paint();
        */
}

void MagicCivilization::Reset()
{
    if (fNumInit>=fNumPixels)
        fNumInit = fNumPixels-1;
    if (fNumInit<0)
        fNumInit = 0;

    if (fLimit<0)
        fLimit=6;
    if (fLimit>6)
        fLimit=0;


    TRandom rnd(0);
    for (int i=0; i<fNumPixels; i++)
        (*fGeomCam)[i].ResetBit(kUserBits);

    for (int i=0; i<fNumInit; i++)
    {
        Int_t idx;

        do idx = (Int_t)rnd.Uniform(fNumPixels);
        while ((*fGeomCam)[idx].TestBit(kHasFlag));

        if (idx>=fNumPixels)
            cout << "!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

        (*fGeomCam)[idx].SetBit(kHasFlag);
    }

    fAuto = kFALSE;
    fStep = 0;

    fColors.Reset();

    Update();

    gPad->Modified();
    gPad->Update();
}

// ------------------------------------------------------------------------
//
// Call this function to draw the camera layout into your canvas.
// Setup a drawing canvas. Add this object and all child objects
// (hexagons, etc) to the current pad. If no pad exists a new one is
// created.
//
void MagicCivilization::Draw(Option_t *option)
{
    //
    // if no canvas is yet existing to draw into, create a new one
    //
    new TCanvas("MagicCivilization", "Magic Civilization", 0, 0, 800, 800);

    gPad->SetBorderMode(0);
    gPad->SetFillColor(22);

    //
    // Append this object, so that the aspect ratio is maintained
    // (Paint-function is called)
    //
    AppendPad(option);

    fCivilization.SetTextAlign(23);   // centered/bottom
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fCivilization.SetBit(kNoContextMenu|kCannotPick);
#endif
    fCivilization.Draw();

    Reset();
}

void MagicCivilization::Update()
{
    TString txt = "Lim: ";
    txt += (int)fLimit;
    txt += "  Init: ";
    txt += fNumInit;
    txt += "  On: ";
    txt += fNumCivilizations;
    txt += "  Step: ";
    txt += fStep;

    if (!fAuto)
        txt += "  (paused)";

    fCivilization.SetText(0, fRange, txt);
}



// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MagicCivilization::ExecuteEvent(Int_t event, Int_t keycode, Int_t keysym)
{
    if (event!=kKeyPress)
        return;

    switch (keysym)
    {
    default:
        return;

    case kKey_Space:
        if ((fNumCivilizations==0 || fNumCivilizations==fNumPixels) && !fAuto)
            Reset();
        fAuto = !fAuto;
        Update();
        gPad->Update();
        return;

    case kKey_Right:
        fNumInit += 1;;
        break;

    case kKey_Left:
        fNumInit -= 1;
        break;

    case kKey_Up:
        fNumInit += 10;
        break;

    case kKey_Down:
        fNumInit -= 10;
        break;

    case kKey_PageUp:
        fNumInit += 100;
        break;

    case kKey_PageDown:
        fNumInit -= 100;
        break;

    case kKey_Plus:
        fLimit++;
        break;

    case kKey_Minus:
        fLimit--;
        break;
    }

    Reset();
}

Bool_t MagicCivilization::HandleTimer(TTimer *timer)
{
    if (!fAuto)
        return kTRUE;

    for (int i=0; i<fNumPixels; i++)
    {
        MGeom &pix = (*fGeomCam)[i];

        Byte_t cnt=0;
        for (int j=0; j<pix.GetNumNeighbors(); j++)
            if ((*fGeomCam)[pix.GetNeighbor(j)].TestBit(kHasFlag))
                cnt++;

        cnt += (6-pix.GetNumNeighbors())*cnt/6;

        if (cnt>fLimit)
            pix.SetBit(kHasCreation);
    }

    fNumCivilizations = 0;
    for (int i=0; i<fNumPixels; i++)
    {
        MGeom &pix = (*fGeomCam)[i];

        if (pix.TestBit(kHasCreation))
        {
            pix.SetBit(kHasFlag);
            fColors[i] = kBlack;
            fNumCivilizations++;
        }
        else
        {
            pix.ResetBit(kHasFlag);
            fColors[i] = kBackground;
        }
        pix.ResetBit(kHasCreation);
    }

    if (fNumCivilizations==0 || fNumCivilizations==fNumPixels)
        fAuto = kFALSE;

    fStep++;

    Update();

    gPad->Update();

    return kTRUE;
}

