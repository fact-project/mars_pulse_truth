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
!   Author(s): Thomas Bretz,  3/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MagicReversi
// ------------
//
// Camera Display Games: Reversi
//
// Start the game by:
//   MagicReversi reversi;
//
//  Rules:
//  ------
//
// Use the mouse to put a stone at some place. If between your newly
// placed stone and the next stone (in a row) of your color are stones
// of other colors this stones are won by you. You can only place a
// stone if you win at least one stone from your 'enemy'. If you
// cannot do so, you are skipped. If nobody can make a move anymore
// the game is over. The player has won who has the most stones in
// his own color.
// The present player is indicated by <*>
// Use the Escape key to abort a game.
// If the game was aborted or has been stopped youcan access the
// options in the context menu.
//
////////////////////////////////////////////////////////////////////////////
#include "MagicReversi.h"

#include <iostream>

#include <KeySymbols.h>

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

ClassImp(MagicReversi);

using namespace std;

/*
const Int_t MagicReversi::fColorBombs[7] = {
    22,
    kYellow,
    kGreen,
    kBlue,
    kCyan,
    kMagenta,
    kRed
};
*/
void MagicReversi::Free()
{
    if (!fGeomCam)
        return;

    fText->Delete();
    fFlags->Delete();

    delete fText;
    delete fFlags;

    delete fGeomCam;
}

void MagicReversi::ChangeCamera()
{
    static Bool_t ct1=kFALSE;

    cout << "Change to " << (ct1?"Magic":"CT1") << endl;

    if (ct1)
        SetNewCamera(new MGeomCamMagic);
    else
        SetNewCamera(new MGeomCamCT1);

    ct1 = !ct1;

    Reset();
}

void MagicReversi::SetNewCamera(MGeomCam *geom)
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
//    fNumBombs = fNumPixels/5;

    fText   = new TClonesArray("TText",    fNumPixels);
    fFlags  = new TClonesArray("TMarker",  fNumPixels);
    fColors.Set(fNumPixels);

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

/*
// ------------------------------------------------------------------------
//
// Draw all pixels of the camera
//  (means apend all pixelobjects to the current pad)
//
void MagicReversi::DrawHexagons()
{
    for (UInt_t i=0; i<fNumPixels; i++)
        (*this)[i].Draw();
}
*/

void MagicReversi::Init()
{
    //
    // Make sure, that the object is destroyed when the canvas/pad is
    // destroyed. Make also sure, that the interpreter doesn't try to
    // delete it a second time.
    //
    SetBit(kCanDelete);
    gInterpreter->DeleteGlobal(this);

    fNumUsers = 2;

    Draw();
}

// ------------------------------------------------------------------------
//
//  default constructor
//
MagicReversi::MagicReversi()
    : fGeomCam(NULL), fDone(NULL)
{
    SetNewCamera(new MGeomCamMagic);

    Init();
}

MagicReversi::MagicReversi(const MGeomCam &geom)
    : fGeomCam(NULL), fDone(NULL)
{
    SetNewCamera(static_cast<MGeomCam*>(geom.Clone()));

    Init();
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes TClonesArrays for hexagons and legend elements.
//
MagicReversi::~MagicReversi()
{
    Free();

    for (int i=0; i<6; i++)
        delete fUsrTxt[i];

    if (fDone)
        delete fDone;
}

// ------------------------------------------------------------------------
//
// This is called at any time the canvas should get repainted.
// Here we maintain an aspect ratio of 5/4=1.15. This makes sure,
// that the camera image doesn't get distorted by resizing the canvas.
//
void MagicReversi::Paint(Option_t *opt)
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
void MagicReversi::Draw(Option_t *option)
{
    //
    // if no canvas is yet existing to draw into, create a new one
    //
    if (!gPad)
        new TCanvas("MagicReversi", "Magic Reversi", 0, 0, 800, 800);

    gPad->SetBorderMode(0);

    //
    // Append this object, so that the aspect ratio is maintained
    // (Paint-function is called)
    //
    AppendPad(option);

    //
    // Draw the title text
    //
    for (int i=0; i<6; i++)
    {
        fUsrTxt[i] = new TText;
        fUsrTxt[i]->SetTextAlign(13);       // left/bottom
        fUsrTxt[i]->SetTextSize(0.03);
        fUsrTxt[i]->SetTextColor(kRed+i);  
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        fUsrTxt[i]->SetBit(kNoContextMenu|kCannotPick);
#endif
        fUsrTxt[i]->Draw();
    }

    //
    // Reset the game pad
    //
    Reset();
}

void MagicReversi::Update()
{
    int i;
    for (i=0; i<fNumUsers; i++)
    {
        TString txt = "Pixels: ";
        txt += fUsrPts[i];

        if (fNumUser==i)
            txt += " <*>";

        fUsrTxt[i]->SetText(-fRange*0.95, fRange-(i+1)*fRange*0.06, txt);
    }
    for (; i<6; i++)
        fUsrTxt[i]->SetText(0, 0, "");
}

void MagicReversi::TwoPlayer()
{
    fNumUsers = 2;
    Reset();
}

void MagicReversi::ThreePlayer()
{
    fNumUsers = 3;
    Reset();
}

void MagicReversi::FourPlayer()
{
    fNumUsers = 4;
    Reset();
}

void MagicReversi::FivePlayer()
{
    fNumUsers = 5;
    Reset();
}

void MagicReversi::SixPlayer()
{
    fNumUsers = 6;
    Reset();
}

// ------------------------------------------------------------------------
//
// reset the all pixel colors to a default value
//
void MagicReversi::Reset()
{
    if (fDone)
    {
        delete fDone;
        fDone = NULL;
    }

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        fColors[i] = kEmpty;
        (*fGeomCam)[i].ResetBit(kUserBits);

        GetFlag(i)->SetMarkerColor(kBlack);
        GetText(i)->SetText(0, 0, "");
    }

    fNumUser  = 0;

    for (int i=0; i<6; i++)
        fUsrPts[i]=0;

    for (int i=1; i<5*fNumUsers; i++)
    {
        fColors[i-1] = i%fNumUsers+kRed;
        fUsrPts[i%fNumUsers]++;
    }

    Update();

    gPad->SetFillColor(22);

#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    SetBit(kNoContextMenu);
#endif
}

void MagicReversi::Done()
{
    Int_t max = 0;
    Int_t winner = 0;

    for (int i=0; i<6; i++)
        if (fUsrPts[i]>max)
        {
            winner = i;
            max = fUsrPts[i];
        }

    TString txt = "Player #";
    txt += winner+1;
    txt += " wins (";
    txt += max;
    txt += ")";

    fDone = new TText(0, 0, txt);
    fDone->SetTextColor(kWhite);
    fDone->SetTextAlign(22);  
    fDone->SetTextSize(0.05); 
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    fDone->SetBit(kNoContextMenu|kCannotPick);
#endif
    fDone->Draw();

    gPad->SetFillColor(winner+kRed);

#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
    ResetBit(kNoContextMenu);
#endif
}

Bool_t MagicReversi::Flip(Int_t origidx, Bool_t flip)
{
    const Int_t col = kRed+fNumUser;

    int test[6] = {0,0,0,0,0,0};

    for (int dir=MGeomPix::kRightTop; dir<=MGeomPix::kLeftTop; dir++)
    {
        Int_t idx = origidx;
        Int_t length = 0;

        while (1)
        {
            idx = fGeomCam->GetNeighbor(idx, dir);
            if (idx<0 || fColors[idx]==kEmpty)
                break;

            if (fColors[idx]==col)
            {
                if (length!=0)
                    test[dir] = length;
                break;
            }

            length++;
        }
    }

    int cnt = 0;

    for (int dir=MGeomPix::kRightTop; dir<=MGeomPix::kLeftTop; dir++)
    {
        Int_t idx = origidx;

        if (test[dir])
            cnt++;

        if (flip)
            for (int i=0; i<test[dir]; i++)
            {
                idx = fGeomCam->GetNeighbor(idx, dir);

                fUsrPts[fColors[idx]-kRed]--;
                fUsrPts[fNumUser]++;

                fColors[idx] = col;
            }
    }

    return cnt ? kTRUE : kFALSE;
}

Bool_t MagicReversi::CheckMoves()
{
    for (unsigned int i=0; i<fNumPixels; i++)
        if (fColors[i]==kEmpty && Flip(i, kFALSE))
            return kTRUE;
    return kFALSE;
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MagicReversi::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    if (event==kMouseMotion   || event==kMouseEnter    || event==kMouseLeave    ||
        event==kButton1Up     || event==kButton2Up     || event==kButton3Up     ||
        event==kButton1Motion || event==kButton2Motion || event==kButton3Motion ||
                                 event==kButton2Double || event==kButton3Double ||
        fDone)
        return;

    if (event==kKeyPress && py==kKey_Escape)
    {
        Done();
        gPad->Modified();
        gPad->Update();
        return;
    }

    UInt_t idx;
    for (idx=0; idx<fNumPixels; idx++)
        if ((*fGeomCam)[idx].DistancetoPrimitive(px, py)<=0)
            break;

    if (idx==fNumPixels)
        return;

    if (event==kButton1Down && fColors[idx]==kEmpty)
    {
        if (!Flip(idx, kTRUE))
            return;

        fUsrPts[fNumUser]++;

        fColors[idx] = kRed+fNumUser;

        Int_t start = fNumUser;

        fNumUser++;
        fNumUser%=fNumUsers;

        while (!CheckMoves())
        {
            cout << "Sorry, no moves possible for player #" << fNumUser << endl;
            fNumUser++;
            fNumUser%=fNumUsers;

            if (fNumUser==start)
            {
                Done();
                break;
            }
        }

        Update();
    }

    gPad->Modified();
}
