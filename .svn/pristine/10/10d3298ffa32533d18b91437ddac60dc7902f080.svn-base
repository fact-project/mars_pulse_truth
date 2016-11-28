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
// MagicDomino
// -----------
//
// Magic Camera games.
//
// Start the show by:
//   MagicDomino show;
//
// Use the following keys:
// -----------------------
//
//   * Cursor up/down, left/right:
//     Move tile
//
//   * Space:
//     Rotate tile
//
//   * Enter:
//     Set tile
//
//   * Esc:
//     Skip tile
//
//  Rules:
//  ------
//
//   Each hexagon in the tile must at least have one neighbor
//   which has the same color. It is not allowed to put a tile onto
//   another one. The game is over if you have skipped three tiles
//   in a row.
//
////////////////////////////////////////////////////////////////////////////
#include "MagicDomino.h"

#include <iostream>

#include <KeySymbols.h>

#include <TMath.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TInterpreter.h>

#include "MH.h"

#include "MGeomPix.h"
#include "MGeomCamCT1.h"
#include "MGeomCamMagic.h"

ClassImp(MagicDomino);

using namespace std;

// ------------------------------------------------------------------------
//
// Free all onbects connected to a special camera geometry
//
void MagicDomino::Free()
{
    if (!fGeomCam)
        return;

    delete fGeomCam;
}

// ------------------------------------------------------------------------
//
// Change camera from Magic to CT1 and back
//
void MagicDomino::ChangeCamera()
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

// ------------------------------------------------------------------------
//
// Reset/set all veriables needed for a new camera geometry
//
void MagicDomino::SetNewCamera(MGeomCam *geom)
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
// remove the pixel numbers in the tile from the pad
//
void MagicDomino::RemoveNumbers()
{
    for (int i=0; i<6; i++)
        fText[i]->SetText(0, 0, "");
}

// ------------------------------------------------------------------------
//
// Reset/restart the game
//
void MagicDomino::Reset()
{
    for (UInt_t i=0; i<fNumPixels; i++)
    {
        fColors[i] = kBackground;
        (*fGeomCam)[i].ResetBit(kUserBits);
    }


#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    SetBit(kNoContextMenu);
#endif

    if (fDone)
    {
        delete fDone;
        fDone = NULL;
    }


    gPad->SetFillColor(22);

    fNumPixel = -1;
    fNumTile  =  0;
    fPoints   =  0;

    NewTile();
}

// ------------------------------------------------------------------------
//
void MagicDomino::Init()
{
    for (int i=0; i<6; i++)
    {
        fText[i] = new TText(0, 0, "");
        fText[i]->SetTextFont(122);
        fText[i]->SetTextAlign(22);   // centered/centered
    }

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
MagicDomino::MagicDomino()
    : fGeomCam(NULL), fDir(kBottom), fDone(NULL)
{
    SetNewCamera(new MGeomCamMagic);
    Init();
}
// ------------------------------------------------------------------------
//
//
MagicDomino::MagicDomino(const MGeomCam &geom)
    : fGeomCam(NULL), fDir(kBottom), fDone(NULL)
{
    SetNewCamera(static_cast<MGeomCam*>(geom.Clone()));
    Init();
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MagicDomino::~MagicDomino()
{
    Free();

    RemoveNumbers();
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MagicDomino::Paint(Option_t *opt)
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
void MagicDomino::Draw(Option_t *option)
{
    //
    // if no canvas is yet existing to draw into, create a new one
    //
    new TCanvas("MagicDomino", "Magic Domino Next Neighbours", 0, 0, 800, 800);

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

    fDomino.SetTextAlign(23);   // centered/bottom
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fDomino.SetBit(kNoContextMenu|kCannotPick);
#endif
    fDomino.Draw();
}

void MagicDomino::Update()
{
    TString txt = "Points: ";
    txt += fPoints;
    txt += "  Tile: ";
    txt += fNumTile;

    switch (fSkipped)
    {
    case 0:
        fDomino.SetTextColor(8/*kGreen*/);
        break;
    case 1:
        fDomino.SetTextColor(kYellow);
        break;
    case 2:
        fDomino.SetTextColor(kRed);
        break;
    default:
        fDomino.SetTextColor(kWhite);
        break;
    }
    fDomino.SetText(0, fRange, txt);
}

// ------------------------------------------------------------------------
//
// Choose new colors for the tile
//
void MagicDomino::NewColors()
{
    TRandom rnd(0);
    for (int i=0; i<3; i++)
    {
        Int_t color = (Int_t)rnd.Uniform(5)+2;
        fNewColors[i*2]   = color;
        fNewColors[i*2+1] = color;
    }
}

// ------------------------------------------------------------------------
//
// Create a new tile
//
void MagicDomino::NewTile()
{
    if (fNumPixel>=0)
    {
        MGeom &pix=(*fGeomCam)[fNumPixel];
        pix.ResetBit(kIsTile);
        for (int i=0; i<pix.GetNumNeighbors(); i++)
            (*fGeomCam)[pix.GetNeighbor(i)].ResetBit(kIsTile);

        fPoints += pix.GetNumNeighbors();
    }

    RemoveNumbers();

    fNumPixel = -1;
    fSkipped  =  0;
    fNumTile++;

    NewColors();
    Update();

    gPad->Update();
}

// ------------------------------------------------------------------------
//
//  Check for at least one correct color for each pixel in tile.
//  Ignore the tile itself and all background pixels. Check whether
//  background of tile is empty.
//
Bool_t MagicDomino::CheckTile()
{
    if (fNumPixel<0)
        return kFALSE;

    for (int i=0; i<7; i++)
        if (fOldColors[i]!=kBackground)
            return kFALSE;

    Int_t cnt=0;
    const MGeom &pix1=(*fGeomCam)[fNumPixel];
    for (int i=0; i<pix1.GetNumNeighbors(); i++)
    {
        const Int_t idx1 = pix1.GetNeighbor(i);
        const MGeom &pix2 = (*fGeomCam)[idx1];

        Byte_t ignored = 0;
        Byte_t found   = 0;
        for (int j=0; j<pix2.GetNumNeighbors(); j++)
        {
            const Int_t idx2 = pix2.GetNeighbor(j);
            const MGeom &hex = (*fGeomCam)[idx2];

            if (hex.TestBit(kIsTile) || fColors[idx2]==kBackground)
            {
                ignored++;
                continue;
            }

            if (fColors[idx2]==fColors[idx1])
                found++;
        }
        if (ignored==pix2.GetNumNeighbors() || found>0)
            cnt++;
    }

    return cnt==pix1.GetNumNeighbors();
}

// ------------------------------------------------------------------------
//
// Game over!
//
void MagicDomino::Done()
{
    fDone = new TText(0, 0, "Game Over!");
    fDone->SetTextColor(kWhite);  // white
    fDone->SetTextAlign(22);  // centered/centered
    fDone->SetTextSize(0.05); // white
    fDone->Draw();

    fDomino.SetTextColor(kBlue);

    gPad->SetFillColor(kRed);
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fDone->SetBit(kNoContextMenu|kCannotPick);
    ResetBit(kNoContextMenu);
#endif

    gPad->Modified();
    gPad->Update();
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MagicDomino::ExecuteEvent(Int_t event, Int_t keycode, Int_t keysym)
{
    if (event!=kKeyPress || fDone)
        return;

    switch (keysym)
    {
    case kKey_Escape:
        fPoints -= 6;
        fSkipped++;
        if (fSkipped==3)
        {
            Done();
            return;
        }
        NewColors();
        RotateTile(0);
        return;

    case kKey_Space:
        RotateTile(-1);
        return;

    case kKey_Return:
        if (CheckTile())
            NewTile();
        return;

    case kKey_Right:
        fDir = kRight;
        Step(kRight);
        return;

    case kKey_Left:
        fDir = kLeft;
        Step(kLeft);
        return;

    case kKey_Up:
        Step(kTop|fDir);
        return;

    case kKey_Down:
        Step(kBottom|fDir);
        return;

    case kKey_Plus:
        RotateTile(+1);
        return;

    case kKey_Minus:
        RotateTile(-1);
        return;
    }
}

// ------------------------------------------------------------------------
//
// Rotate the colors in the tile
//
void MagicDomino::RotateTile(Int_t add)
{
    fPosition += add+6;  // Make result positive
    fPosition %= 6;      // Align result between 0 and 5

    HideTile();
    ShowTile();

    Update();

    gPad->Modified();
    gPad->Update();
}

// ------------------------------------------------------------------------
//
// Hide the tile from the pad
//
void MagicDomino::HideTile()
{
    if (fNumPixel<0)
        return;

    RemoveNumbers();

    MGeom &pix1=(*fGeomCam)[fNumPixel];
    fColors[fNumPixel] = fOldColors[6];
    pix1.ResetBit(kIsTile);
    for (int i=0; i<pix1.GetNumNeighbors(); i++)
    {
        Int_t idx = pix1.GetNeighbor(i);

        fColors[idx] = fOldColors[i];
        (*fGeomCam)[idx].ResetBit(kIsTile);
    }
}

// ------------------------------------------------------------------------
//
// Show the tile on the pad
//
void MagicDomino::ShowTile()
{
    if (fNumPixel<0)
        return;

    Int_t indices[6];
    GetSortedNeighbors(indices);

    RemoveNumbers();

    MGeom &pix2=(*fGeomCam)[fNumPixel];
    fOldColors[6] = fColors[fNumPixel];
    fColors[fNumPixel] = kBlack;
    pix2.SetBit(kIsTile);
    for (int i=0; i<pix2.GetNumNeighbors(); i++)
    {
        Int_t idx = pix2.GetNeighbor(i);

        fOldColors[i] = fColors[idx];

        int j=0;
        while (indices[j]!=i)
            j++;

        MGeom &pix = (*fGeomCam)[idx];

        fColors[idx] = fNewColors[(j+fPosition)%6];
        pix.SetBit(kIsTile);

        TString num;
        num += idx;

        fText[i]->SetText(pix.GetX(), pix.GetY(), num);
        fText[i]->SetTextSize(0.3*pix.GetT()/fGeomCam->GetMaxRadius());
    }
}

// ------------------------------------------------------------------------
//
// Hide the tile, change its position, show it again, update status text
//
void MagicDomino::ChangePixel(Int_t add)
{
    HideTile();

    fNumPixel = add;

    ShowTile();

    Update();

    gPad->Update();
}

// ------------------------------------------------------------------------
//
// Analyse the directions of the next neighbors
//
Short_t MagicDomino::AnalysePixel(Int_t dir)
{
    const MGeom &pix=(*fGeomCam)[fNumPixel<0?0:fNumPixel];

    Double_t fAngle[6] = { -10, -10, -10, -10, -10, -10 };

    for (int i=0; i<pix.GetNumNeighbors(); i++)
    {
        MGeom &next = (*fGeomCam)[pix.GetNeighbor(i)];
        fAngle[i] = atan2(next.GetY()-pix.GetY(), next.GetX()-pix.GetX());
    }

    Int_t indices[6];
    TMath::Sort(6, fAngle, indices); // left, top, right, bottom

    for (int i=0; i<pix.GetNumNeighbors(); i++)
    {
        const Int_t    idx   = pix.GetNeighbor(indices[i]);
        const Double_t angle = fAngle[indices[i]]*180/TMath::Pi();

        if (angle<-149 && dir==kLeft)
            return idx;
        if (angle>-149 && angle<-90 && dir==kBottomLeft)
            return idx;
        //if (angle==-90 && dir==kBottom)
        //    return idx;
        if (angle>-90 && angle<-31 && dir==kBottomRight)
            return idx;
        if (angle>-31 && angle<31 && dir==kRight)
            return idx;
        if (angle>31 && angle<90 && dir==kTopRight)
            return idx;
        //if (angle==90 && dir==kTop)
        //    return idx;
        if (angle>90 && angle<149 && dir==kTopLeft)
            return idx;
        if (angle>149 && dir==kLeft)
            return idx;
    }
    return -1;
}

// ------------------------------------------------------------------------
//
// Sort the next neighbort from the left, top, right, bottom
//
void MagicDomino::GetSortedNeighbors(Int_t indices[6])
{
    const MGeom &pix=(*fGeomCam)[fNumPixel<0?0:fNumPixel];

    Double_t fAngle[6] = { -10, -10, -10, -10, -10, -10 };

    for (int i=0; i<pix.GetNumNeighbors(); i++)
    {
        MGeom &next = (*fGeomCam)[pix.GetNeighbor(i)];
        fAngle[i] = atan2(next.GetY()-pix.GetY(), next.GetX()-pix.GetX());
    }

    TMath::Sort(6, fAngle, indices); // left, top, right, bottom
}

// ------------------------------------------------------------------------
//
// Move tile one step in the given direction
//
void MagicDomino::Step(Int_t dir)
{
    Short_t newidx = AnalysePixel(dir);
    if (newidx<0)
        return;
    ChangePixel(newidx);
}

