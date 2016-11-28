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
// MineSweeper
// -----------
//
// Camera Display Games: Mine Sweeper
//
// Start the game by:
//   MineSweeper mine;
//
// It is the well known Mine Sweeper.
// Set a mark using a single mouse click.
// Open a pixel using a double click.
//
// Try to open all pixels without bombs. If you open a pixel with no
// bomb around all pixels around are opened.
//
// To restart the game use the context menu. It can only be accessed if
// the game has been stopped (either because you win the game or because
// you hit a bomb) With the context menu you can also toggle between
// different camera layouts.
//
////////////////////////////////////////////////////////////////////////////
#include "MineSweeper.h"

#include <iostream>

#include <TText.h>
#include <TMarker.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TInterpreter.h>

#include "MH.h"

#include "MGeomPix.h"
#include "MGeomCamCT1.h"
#include "MGeomCamMagic.h"

ClassImp(MineSweeper);

using namespace std;

const Int_t MineSweeper::fColorBombs[7] = {
    22,
    kYellow,
    kGreen,
    kBlue,
    kCyan,
    kMagenta,
    kRed
};

void MineSweeper::Free()
{
    if (!fGeomCam)
        return;

    fText->Delete();
    fFlags->Delete();

    delete fText;
    delete fFlags;

    delete fGeomCam;
}

void MineSweeper::ChangeCamera()
{
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

void MineSweeper::SetNewCamera(MGeomCam *geom)
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

    //
    // Construct all hexagons. Use new-operator with placement
    //
    fNumBombs = fNumPixels/5;

    fText   = new TClonesArray("TText",    fNumPixels);
    fFlags  = new TClonesArray("TMarker",  fNumPixels);
    fColors.Set(fNumPixels);
    //fPixels = new TClonesArray("MHexagon", fNumPixels);

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGeom &pix = (*fGeomCam)[i];

        TText &t = *new ((*fText)[i]) TText;
        t.SetTextFont(122);
        t.SetTextAlign(22);   // centered/centered
        t.SetTextSize(0.3*pix.GetT()/fRange);
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        t.SetBit(kNoContextMenu|kCannotPick);
#endif

        TMarker &m = *new ((*fFlags)[i]) TMarker(pix.GetX(), pix.GetY(), kOpenStar);
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        m.SetBit(kNoContextMenu|kCannotPick);
#endif
    }
}

void MineSweeper::Init()
{
    //
    // Make sure, that the object is destroyed when the canvas/pad is
    // destroyed. Make also sure, that the interpreter doesn't try to
    // delete it a second time.
    //
    SetBit(kCanDelete);
    gInterpreter->DeleteGlobal(this);

    Draw();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MineSweeper::MineSweeper()
    : fGeomCam(NULL), fDone(NULL), fShow(NULL)
{
    SetNewCamera(new MGeomCamMagic);
    Init();
}

MineSweeper::MineSweeper(const MGeomCam &geom)
    : fGeomCam(NULL), fDone(NULL), fShow(NULL)
{
    SetNewCamera(static_cast<MGeomCam*>(geom.Clone()));
    Init();
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MineSweeper::~MineSweeper()
{
    Free();

    delete fShow;

    if (fDone)
        delete fDone;
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MineSweeper::Paint(Option_t *opt)
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

        //
        // Adopt absolute sized of markers to relative range
        //
        Float_t r = (*fGeomCam)[i].GetT()*gPad->XtoAbsPixel(1)/325;
        GetFlag(i)->SetMarkerSize(20.0*r/fRange);

        if (pix.TestBit(kHasFlag))
            GetFlag(i)->Paint();

        GetText(i)->Paint();
    }
}

// ------------------------------------------------------------------------
//
// Call this function to draw the camera layout into your canvas.
// Setup a drawing canvas. Add this object and all child objects
// (hexagons, etc) to the current pad. If no pad exists a new one is
// created.
//
void MineSweeper::Draw(Option_t *option)
{
    // root 3.02:
    // gPad->SetFixedAspectRatio()

    //
    // if no canvas is yet existing to draw into, create a new one
    //
    if (!gPad)
        new TCanvas("MineSweeper", "Magic Mine Sweeper", 0, 0, 800, 800);

    gPad->SetBorderMode(0);

    //
    // Append this object, so that the aspect ratio is maintained
    // (Paint-function is called)
    //
    AppendPad(option);

    //
    // Draw the title text
    //
    fShow = new TText;
    fShow->SetTextAlign(23);   // centered/bottom
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fShow->SetBit(kNoContextMenu|kCannotPick);
#endif
    fShow->Draw();
    //
    // Reset the game pad
    //
    Reset();
}

void MineSweeper::Update(Int_t num)
{
    TString txt = "Pixels: ";
    txt += fNumPixels;
    txt += "  Bombs: ";
    txt += num;

    fShow->SetText(0, fRange, txt);
}

// ------------------------------------------------------------------------
//
// reset the all pixel colors to a default value
//
void MineSweeper::Reset()
{
    if (fDone)
    {
        delete fDone;
        fDone = NULL;
    }

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        fColors[i] = kHidden;
        (*fGeomCam)[i].ResetBit(kUserBits);

        GetFlag(i)->SetMarkerColor(kBlack);
        GetText(i)->SetText(0, 0, "");
    }
    Update(fNumBombs);

    TRandom rnd(0);
    for (int i=0; i<fNumBombs; i++)
    {
        Int_t idx;

        do idx = (Int_t)rnd.Uniform(fNumPixels);
        while ((*fGeomCam)[idx].TestBit(kHasBomb));

        (*fGeomCam)[idx].SetBit(kHasBomb);
    }

    gPad->SetFillColor(22);

#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    gPad->SetBit(kNoContextMenu);
    SetBit(kNoContextMenu);
#endif
}

void MineSweeper::Done(TString txt, Int_t col)
{
    for (unsigned int j=0; j<fNumPixels; j++)
        if ((*fGeomCam)[j].TestBit(kHasBomb))
        {
            fColors[j] = kBlack;
            GetFlag(j)->SetMarkerColor(kWhite);
        }

    fDone = new TText(0, 0, txt);
    fDone->SetTextColor(kWhite);  // white
    fDone->SetTextAlign(22);  // centered/centered
    fDone->SetTextSize(0.05); // white
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fDone->SetBit(kNoContextMenu|kCannotPick);
#endif
    fDone->Draw();

    gPad->SetFillColor(col);

#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    gPad->ResetBit(kNoContextMenu);
    ResetBit(kNoContextMenu);
#endif
}

// ------------------------------------------------------------------------
//
// Check whether a hexagon should be opened and which color/number should
// be visible
//
void MineSweeper::OpenHexagon(Int_t idx)
{
    MGeom &pix=(*fGeomCam)[idx];

    if (pix.TestBit(kIsVisible))
        return;

    pix.SetBit(kIsVisible);
    pix.ResetBit(kHasFlag);

    Int_t cnt=0;
    for (int j=0; j<pix.GetNumNeighbors(); j++)
        if ((*fGeomCam)[pix.GetNeighbor(j)].TestBit(kHasBomb))
            cnt++;

    fColors[idx] = fColorBombs[cnt];

    TString str;
    if (cnt)
        str += cnt;

    TText *txt = GetText(idx);
    txt->SetText(pix.GetX(), pix.GetY(), str);

    if (cnt)
        return;

    for (int j=0; j<pix.GetNumNeighbors(); j++)
        OpenHexagon(pix.GetNeighbor(j));
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MineSweeper::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    if (event==kMouseMotion   || event==kMouseEnter    || event==kMouseLeave    ||
        event==kButton1Up     || event==kButton2Up     || event==kButton3Up     ||
        event==kButton1Motion || event==kButton2Motion || event==kButton3Motion ||
                                 event==kButton2Double || event==kButton3Double ||
        fDone)
        return;

    /*
    if (event==kKeyPress && py==0x1000)
    {
        Reset();
        return;
    }
    */

    UInt_t idx;
    for (idx=0; idx<fNumPixels; idx++)
        if ((*fGeomCam)[idx].DistancetoPrimitive(px, py)<=0)
            break;

    if (idx==fNumPixels)
        return;

    MGeom &pix=(*fGeomCam)[idx];

    if (event==kButton1Double)
    {
        OpenHexagon(idx);

        if (pix.TestBit(kHasBomb))
            Done("Argh... you hit the Bomb!!!", kRed);
    }

    if (event==kButton1Down && !pix.TestBit(kIsVisible))
        pix.InvertBit(kHasFlag);

    UInt_t vis=fNumBombs;
    UInt_t flg=fNumBombs;
    for (UInt_t i=0; i<fNumPixels; i++)
    {
        if ((*fGeomCam)[i].TestBit(kIsVisible))
            vis++;
        if ((*fGeomCam)[i].TestBit(kHasFlag))
            flg--;
    }

    Update(flg);

    if (vis==fNumPixels && !fDone)
        Done("Great! Congratulations, you did it!", kGreen);

    gPad->Modified();

    /*
     switch (event)
     {
     case kNoEvent:       cout << "No Event" << endl; break;
     case kButton1Down:   cout << "Button 1 down" << endl; break;
     case kButton2Down:   cout << "Button 2 down" << endl; break;
     case kButton3Down:   cout << "Button 3 down" << endl; break;
     case kKeyDown:       cout << "Key down" << endl; break;
     case kKeyUp:         cout << "Key up" << endl; break;
     case kKeyPress:      cout << "Key press" << endl; break;
     case kButton1Locate: cout << "Button 1 locate" << endl; break;
     case kButton2Locate: cout << "Button 2 locate" << endl; break;
     case kButton3Locate: cout << "Button 3 locate" << endl; break;
    }
    */
}
