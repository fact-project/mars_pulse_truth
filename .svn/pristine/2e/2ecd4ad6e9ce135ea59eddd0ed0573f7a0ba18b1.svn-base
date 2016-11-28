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
!   Author(s): Thomas Bretz, 9/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2005-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MagicJam
//
// Jam is a multi player game. It is controlled by its context menu (click
// with the right mouse button on one of the pixels in the game pad).
//
// Start the game with:
//  MagicJam jam;
//
// Rules:
//  MagicJam is a multiplayer game. The aim is to own the full game pad.
// Ech pixel in the pad can contain as many entries as it has neighbors.
// If it has more entries than neighbors it overflows into its neighbors.
// All pixels into which such an overflow occures will be captured by
// the current player. Player by player will be able to increase the
// entry in one pixel by one. Increase the contents of an empty pixel to
// one or by one in an owned pixel. The player who once will own the
// all pixels will win.
//
// Editor:
//  To edit or create a new game start the editor from the context menu
// of MagicJam. The editor is also controlled from its context menu.
// Use the curso keys to shift your game.
// Warning: You MUST create a single-island game. Games which have two
// islands might result in edless loops. (Currently there is no check!)
//
/////////////////////////////////////////////////////////////////////////////
#include "MagicJam.h"

#include <climits> // INT_MAX (Ubuntu 8.10)

#include <iostream>
#include <fstream>

#include <errno.h>

#include <KeySymbols.h>

#include <TMath.h>
#include <TRandom.h>

#include <TBox.h>
#include <TLine.h>
#include <TText.h>

#include <TStyle.h>
#include <TSystem.h>

#include <TNtuple.h>
#include <TASImage.h>
#include <TInterpreter.h>
#include <TGFileDialog.h>

#include "MH.h"
#include "MHexagon.h"
#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MDirIter.h"

ClassImp(MagicJamAbc);
ClassImp(MagicJamEditor);
ClassImp(MagicJam);

using namespace std;

// -------------------------------------------------------------------------
//                     The editor for MagicJam
// -------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
// Move the contents of the pad
//
void MagicJamEditor::Move(int dx, int dy)
{
    const TArrayC cpy(fUsed);

    Int_t h = cpy.GetSize()/fWidth;

    for (int i=0; i<cpy.GetSize(); i++)
    {
        Int_t x = i%fWidth;
        Int_t y = i/fWidth;

        x += dx   + fWidth;
        y += dy*2 + h;

        x %= fWidth;
        y %= h;

        fUsed[x+y*fWidth] = cpy[i];
    }
    gPad->GetPad(1)->Modified();
    gPad->GetPad(1)->Update();
    gPad->Modified();
    gPad->Update();
}

// --------------------------------------------------------------------------
//
// Initialize the pad with size wx, hy
//
void MagicJamEditor::InitGeom(int wx, int hy)
{
    const Float_t dx = 0.5*(wx-1);
    const Float_t dy = 0.5*(hy-1);

    fWidth = wx;

    MGeomCam cam(wx*hy, 1);

    for (int x=0; x<wx; x++)
        for (int y=0; y<hy; y++)
        {
            Float_t x0 = TMath::Odd(y) ? x-0.25 : x+0.25;

            cam.SetAt(x + y*fWidth, MGeomPix(x0-dx, (y-dy)*0.866, 1));
        }


    cam.InitGeometry();

    SetGeometry(cam);
}

// --------------------------------------------------------------------------
//
// Get index of pixel with position px, py
//
Int_t MagicJamEditor::GetPixelIndexFlt(Float_t px, Float_t py) const
{
    if (fNcells<=1)
        return -1;

    Int_t i;
    for (i=0; i<fNcells-2; i++)
    {
        if ((*fGeomCam)[i].IsInside(px, py))
            return i;
    }
    return -1;
}

// --------------------------------------------------------------------------
//
// Open the MagicJam editor. Load a game with name name.
//
MagicJamEditor::MagicJamEditor(const char *name) : fImage(NULL)
{
    InitGeom(20, 20);

    SetBit(kNoLegend);
    SetBit(kNoScale);

    Draw();

    gPad->SetEditable(kFALSE);

    if (name)
        LoadGame(name);
}

// --------------------------------------------------------------------------
//
// Delete fImage
//
MagicJamEditor::~MagicJamEditor()
{
    if (fImage)
        delete fImage;
}

// --------------------------------------------------------------------------
//
// Change width and height of pad. Preserve contents.
//
void MagicJamEditor::SetWidthHeight(Int_t dx, Int_t dy)
{
    TNtuple tup("", "", "x:y");
    tup.SetDirectory(0);

    for (UInt_t i=0; i<GetNumPixels(); i++)
    {
        if (IsUsed(i))
            tup.Fill((*fGeomCam)[i].GetX(), (*fGeomCam)[i].GetY());
    }
    InitGeom(dx, dy);

    fEntries=0;
    for (int i=0; i<tup.GetEntries(); i++)
    {
        tup.GetEntry(i);
        const Int_t idx = GetPixelIndexFlt(tup.GetArgs()[0], tup.GetArgs()[1]);
        if (idx>=0)
        {
            SetUsed(idx);
            fEntries++;
        }
    }
}

// --------------------------------------------------------------------------
//
// Save the current game
//
void MagicJamEditor::SaveGame(const char *name)
{
    ofstream fout(name);
    if (!fout)
    {
        cout << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return;
    }

    for (UInt_t i=0; i<GetNumPixels(); i++)
        if (IsUsed(i))
            fout << i%fWidth << " " << i/fWidth << endl;

    SetName(name);

}

// --------------------------------------------------------------------------
//
// Load a game
//
void MagicJamEditor::LoadGame(const char *name)
{
    InitGeom(1,1);
    Reset();

    ifstream fin(name);
    if (!fin)
    {
        cout << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return;
    }

    Int_t maxx=-INT_MAX;
    Int_t maxy=-INT_MAX;
    Int_t minx= INT_MAX;
    Int_t miny= INT_MAX;

    TNtuple tup("", "", "x:y");
    tup.SetDirectory(0);

    while (1)
    {
        Int_t x,y;
        fin >> x >> y;
        if (!fin)
            break;

        tup.Fill(x, y);

        maxx = TMath::Max(maxx, x);
        maxy = TMath::Max(maxy, y);
        minx = TMath::Min(minx, x);
        miny = TMath::Min(miny, y);
    }

    if (tup.GetEntries()==0)
    {
        cout << "File " << name << " contains no entries!" << endl;
        return;
    }

    if (TMath::Odd(miny))
        miny--;

    InitGeom((maxx-minx)+1, (maxy-miny)+1);

    for (int i=0; i<tup.GetEntries(); i++)
    {
        tup.GetEntry(i);

        const Int_t x = TMath::Nint(tup.GetArgs()[0]) - minx;
        const Int_t y = TMath::Nint(tup.GetArgs()[1]) - miny;
        SetUsed(x + y*fWidth);
    }

    fEntries=tup.GetEntries();;

    SetName(name);
}

void MagicJamEditor::ShowImageAsBackground(const char *name)
{
    if (fImage)
        delete fImage;

    fImage = new TASImage(name);
    if (!fImage->IsValid())
    {
        delete fImage;
        fImage = NULL;
        return;
    }

    fImage->SetEditable(kFALSE);

    SetFillStyle(4000);
}

void MagicJamEditor::RemoveBackgroundImage()
{
    if (fImage)
    {
        delete fImage;
        fImage=NULL;
    }

    SetFillStyle(1001);
}

// --------------------------------------------------------------------------
//
// Paint the game pad
//
void MagicJamEditor::Paint(Option_t *o)
{
    TString str = Form("Jam Editor pad %dx%d", fWidth, GetNumPixels()/fWidth);
    SetTitle(str);

    Float_t maxx = 0;
    Float_t minx = 0;
    Float_t maxy = 0;
    Float_t miny = 0;

    for (UInt_t i=0; i<GetNumPixels(); i++)
    {
        const Float_t x = (*fGeomCam)[i].GetX();
        const Float_t y = (*fGeomCam)[i].GetY();

        maxx = TMath::Max(maxx, x);
        minx = TMath::Min(minx, x);
        maxy = TMath::Max(maxy, y);
        miny = TMath::Min(miny, y);
    }

    if (fImage)
    {
        const Float_t r = fGeomCam->GetMaxRadius();

        MH::SetPadRange(-r*1.02, -r*1.02, TestBit(kNoLegend) ? r : 1.15*r, r*1.2);

        Double_t x1, y1, x2, y2;
        gPad->GetRange(x1, y1, x2, y2);

        gPad->SetLeftMargin  (    (minx-x1)/(x2-x1));
        gPad->SetBottomMargin(    (miny-y1)/(y2-y1));
        gPad->SetRightMargin (1 - (maxx-x1)/(x2-x1));
        gPad->SetTopMargin   (1 - (maxy-y1)/(y2-y1));

        fImage->Paint();
    }

    gStyle->SetOptStat(11);
    MHCamera::Paint(o);

    TBox box;
    box.SetLineColor(kBlack);
    box.SetFillStyle(0);
    box.PaintBox(minx-1, miny-1, maxx+1, maxy+1);

    TLine l;
    //l.SetLineStyle(kDashed);
    l.SetLineColor(15);
    l.PaintLine(0, miny, 0, maxy);
    l.PaintLine(minx, 0, maxx, 0);
}

// --------------------------------------------------------------------------
//
// Call MHCamera Draw connect a signal to catch the keynoard events
//
void MagicJamEditor::Draw(Option_t *o)
{
    MHCamera::Draw();

    // This seems to be necessary due to the pad-in-pad geometry
    // otherwise I don't get the information about the keys pressed
    gPad->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                               "MagicJamEditor", this,
                               "EventInfo(Int_t,Int_t,Int_t,TObject*)");
}

// --------------------------------------------------------------------------
//
// Process the keyboard events
//
void MagicJamEditor::EventInfo(Int_t event, Int_t px, Int_t py, TObject *o)
{
    if (event==kKeyPress)
    {
        switch (py)
        {
        case kKey_Left:
            Move(-1, 0);
            break;

        case kKey_Right:
            Move(1, 0);
            break;

        case kKey_Up:
            Move(0, 1);
            break;

        case kKey_Down:
            Move(0, -1);
            break;

        default:
            return;
        }

        TVirtualPad *p = dynamic_cast<TVirtualPad*>(o);
        if (p)
        {
            p->Modified();
            p->Update();
        }
    }
}

// --------------------------------------------------------------------------
//
// Execute mouse events
//
void MagicJamEditor::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    const Int_t idx = GetPixelIndex(px, py);
    if (idx<0)
        return;

    switch (event)
    {
    case kButton1Down:
        if (IsUsed(idx))
        {
            ResetUsed(idx);
            fEntries--;
        }
        else
        {
            SetUsed(idx);
            fEntries++;
        }
        break;

    case kButton1Motion:
        if (!IsUsed(idx))
        {
            SetUsed(idx);
            fEntries++;
        }
        break;

    default:
        return;
    }

    gPad->Modified();
    gPad->Update();
}


// -------------------------------------------------------------------------
//                        MagicJam -- the Game
// -------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
// Fill 1 in a single pixel. If the pixel has more content than neighbors
// it overflows into its neighbors. Called recursivly.
//
void MagicJam::FillPix(Int_t idx)
{
    if (CountPlayers()==1)
        return;

    const Int_t newval = fBinEntries[idx+1]+1;

    const MGeom &gpix = (*fGeomCam)[idx];
    const Int_t n = gpix.GetNumNeighbors();

    SetBinContent(idx+1, fNumPlayer);
    SetUsed(idx);

    if (newval <= n)
    {
        fBinEntries[idx+1] = newval;
        return;
    }

    fEntries -= n;
    fBinEntries[idx+1] = 1;

    for (int i=0; i<n; i++)
        FillPix(gpix.GetNeighbor(i));

    gPad->Modified();
    gPad->Update();
}

// --------------------------------------------------------------------------
//
// Count tiles of player 
//
Int_t MagicJam::CountPlayer(Int_t player) const
{
    Int_t sum = 0;
    for (int i=0; i<GetNbinsX(); i++)
        if (IsUsed(i) && TMath::Nint(GetBinContent(i+1))==player)
            sum += fBinEntries[i+1];
    return sum;
}

// --------------------------------------------------------------------------
//
// Count number of players still able to move
//
Int_t MagicJam::CountPlayers() const
{
    if (GetEntries()<=fNumPlayers)
        return 0;

    Int_t cnt=0;
    for (int i=0; i<fNumPlayers; i++)
        if (CountPlayer(i)>0)
            cnt++;

    return cnt;
}

// --------------------------------------------------------------------------
//
// Start MagicJam. Loads the default game jam.txt and initializes a
// two player game
//
MagicJam::MagicJam(const char *jam, MagicJam *game) : fNumPlayers(0), fGame(game)
{
    SetTitle("Magic Jam (game)");

    SetBit(kNoLegend);
    SetBit(kNoScale);
    SetBit(kMustCleanup);

    LoadGame(jam);

    gInterpreter->DeleteGlobal(this);
    gROOT->GetListOfCleanups()->Add(this);

    if (fGame!=NULL)
    {
        SetBit(kNoContextMenu);
        SetBit(kNoStats);
        return;
    }

    if (!fGeomCam)
        return;

    SetNumPlayers(2);
    SetMinMax(-0.5, 9.5);

    Draw();
    gPad->SetEditable(kFALSE);
}

// --------------------------------------------------------------------------
//
// Remove fGame if it was deleted
//
void MagicJam::RecursiveRemove(TObject *obj)
{
    if (fGame==obj)
        fGame=0;
}

// --------------------------------------------------------------------------
//
// Set a new number of players. Restarts the game
//
void MagicJam::SetNumPlayers(Byte_t n)
{
    if (n<2 || n>9)
        return;

    fNumPlayers = n;
    fNumPlayer  = 0;

    Reset();
}

// --------------------------------------------------------------------------
//
// Loads a new geometry (game). Restarts the game.
//
void MagicJam::LoadGame(const char *name)
{
    /*
    if (gPad)
    {
    static const char *gOpenTypes[] =
    {
        "Template files",  "*.txt",
        "All files",    "*",
        NULL,           NULL
    };

    static TString dir(".");

    TGFileInfo fi; // fFileName and fIniDir deleted in ~TGFileInfo

    fi.fFileTypes = (const char**)gOpenTypes;
    fi.fIniDir    = StrDup(dir);

    dlg = new TGFileDialog(0, 0, kFDOpen, &fi);

    // Checks is meanwhile the game has been closed!
    if (!gPad)
        return;

    cout << "done" << endl;

    cout << gPad << endl;

    if (!fi.fFilename || gPad==NULL)
        return;

    cout << "test" << endl;

    dir  = fi.fIniDir;
    name = fi.fFilename;
    }
    //if (!gROOT->GetListOfSpecials()->FindObject(this))
    //    return;

    cout << "Done." << endl;
    */
    //***



    ifstream fin(name);
    if (!fin)
    {
        cout << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return;
    }

    Int_t maxx=-INT_MAX;
    Int_t maxy=-INT_MAX;
    Int_t minx= INT_MAX;
    Int_t miny= INT_MAX;

    TNtuple tup("", "", "x:y");
    tup.SetDirectory(0);

    while (1)
    {
        Int_t x,y;
        fin >> x >> y;
        if (!fin)
            break;

        tup.Fill(x, y);

        maxx = TMath::Max(maxx, x);
        maxy = TMath::Max(maxy, y);
        minx = TMath::Min(minx, x);
        miny = TMath::Min(miny, y);
    }

    if (tup.GetEntries()==0)
    {
        cout << "File " << name << " contains no entries!" << endl;
        return;
    }

    MGeomCam cam(tup.GetEntries());

    for (int i=0; i<tup.GetEntries(); i++)
    {
        tup.GetEntry(i);

        const Int_t x = TMath::Nint(tup.GetArgs()[0]);
        const Int_t y = TMath::Nint(tup.GetArgs()[1]);

        Float_t dx = -minx-0.5*(maxx-minx+1);
        Float_t dy = -miny-0.5*(maxy-miny+1);

        dx += TMath::Odd(y) ? x-0.25 : x+0.25;
        dy += y;

        cam.SetAt(i, MGeomPix(dx, dy*0.866));
    }

    for (UInt_t i=0; i<cam.GetNumPixels(); i++)
    {
        Int_t nn[6] = { -1, -1, -1, -1, -1, -1 };
        Int_t idx=0;

        for (UInt_t j=0; j<cam.GetNumPixels(); j++)
            if (i!=j && cam.GetDist(i,j)<1.5)
                nn[idx++] = j;

        cam[i].SetNeighbors(nn[0], nn[1], nn[2], nn[3], nn[4], nn[5]);
    }

    cam.InitGeometry();

    SetGeometry(cam);

    SetName(name);
    Reset();
}

// --------------------------------------------------------------------------
//
// Restart the game.
//
void MagicJam::Reset(Option_t *o)
{
    MHCamera::Reset("");

    if (fNumPlayers==0)
    {
        SetAllUsed();
        return;
    }

    const Int_t max = TMath::Max((Int_t)GetNumPixels()/25, 1);
    for (Int_t i=0; i<fNumPlayers; i++)
    {
        for (Int_t x=0; x<max; x++)
        {
            const Int_t idx = gRandom->Integer(GetNumPixels()-1);

            if (IsUsed(idx))
            {
                x--;
                continue;
            }

            SetUsed(idx);
            SetBinContent(idx+1, i);
            fBinEntries[idx+1] = 1;
        }
    }
}

// --------------------------------------------------------------------------
//
// Star an editor to make your own game.
//
void MagicJam::StartEditor() const
{
    TCanvas *c=new TCanvas;

    c->SetName("Editor");
    c->SetTitle("MagicJam Editor");

    MagicJamEditor *build=new MagicJamEditor;

    build->SetBit(kCanDelete);
}

// --------------------------------------------------------------------------
//
// Open the current game in the editor.
//
void MagicJam::OpenInEditor() const
{
    TCanvas *c=new TCanvas;

    c->SetName("Editor");
    c->SetTitle("MagicJam Editor");

    MagicJamEditor *build=new MagicJamEditor(GetName());

    build->SetBit(kCanDelete);
}

// --------------------------------------------------------------------------
//
// Show the Jam games of this directory
//
void  MagicJam::ShowDirectory(const char *dir)
{
    MDirIter Next(dir, "*.jam");

    TList list;

    TString file;
    while (1)
    {
        file=Next();
        if (file.IsNull())
            break;

        MagicJam *jam = new MagicJam(file, this);
        if (jam->GetNumPixels()==0)
        {
            delete jam;
            continue;
        }
        list.Add(jam);
    }

    const Int_t n1 = TMath::Nint(TMath::Ceil(list.GetEntries()/3.));
    const Int_t n2 = TMath::Nint(TMath::Ceil((float)list.GetEntries()/n1));

    TCanvas *c = new TCanvas;
    c->SetBorderMode(0);
    c->SetFillColor(kWhite);
    c->Divide(n1, n2, 0, 0);
    c->SetBit(kNoContextMenu);
    c->SetBit(kCannotPick);

    Int_t i=0;

    TObject *o=0;

    TIter NextObj(&list);

    while ((o=NextObj()))
    {
        list.Remove(o);

        c->cd(++i);
        gPad->SetFillColor(kWhite);
        o->SetBit(kCanDelete);
        o->Draw();
        gPad->SetBit(kCannotPick);
    }

    // Delete empty pads
    for (; i<=n1*n2; i++)
        delete c->GetPad(i);
}

// --------------------------------------------------------------------------
//
// Process mouse events
//
void MagicJam::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    if (fNumPlayers==0) // We are in ShowDirectory-Mode
    {
        if (event!=kButton1Down)
            return;

        if (fGame)
        {
            // Do a simple search for the pad containing fGame
            TIter Next(gROOT->GetListOfCanvases());
            TCanvas *c=0;
            while((c=(TCanvas*)Next()))
            {
                TVirtualPad *pad1 = c->GetPad(1);
                if (!pad1)
                    continue;

                if (!pad1->GetListOfPrimitives()->FindObject(fGame))
                    continue;

                fGame->LoadGame(GetName());

                pad1->Modified();
                pad1->Update();
                return;
            }
        }

        // No pad found. Open new canvas for it
        new TCanvas;
        new MagicJam(GetName());

        return;
    }

    if (CountPlayers()==1) // We already have a winner
        return;

    if (event==kButton1Down)
    {
        const Int_t idx = GetPixelIndex(px, py);
        if (idx<0)
            return;

        if (IsUsed(idx) && TMath::Nint(GetBinContent(idx+1))!=fNumPlayer)
        {
            const Int_t col = gPad->GetFillColor();

            gPad->SetFillColor(kRed);
            gPad->Modified();
            gPad->Update();

            gSystem->Sleep(250);

            gPad->SetFillColor(col);
            gPad->Modified();
            gPad->Update();
            return;
        }

        FillPix(idx);

        do
        {
            fNumPlayer++;
            fNumPlayer %= fNumPlayers;
        }
        while (CountPlayers()>1 && CountPlayer(fNumPlayer)==0);

        gPad->Modified();
        gPad->Update();
    }
}

// --------------------------------------------------------------------------
//
// Paint game pad
//
void MagicJam::Paint(Option_t *o)
{
    if (GetNumPixels()==0)
        return;

    gStyle->SetOptStat(11);

    Int_t palette[50] =
    {
        4, 4, 4, 4, 4,  // light blue
        3, 3, 3, 3, 3,  // light green
        6, 6, 6, 6, 6,  // magenta
        5, 5, 5, 5, 5,  // yellow
        7, 7, 7, 7, 7,  // cyan
        //8, 8, 8, 8, 8,  // dark green
        //11, 11, 11, 11,11, // light gray
        12, 12, 12, 12,12, // dark gray
        1, 1, 1, 1, 1,  // black
        46, 46, 46, 46, 46,
        51, 51, 51, 51, 51,   // brown
    };

    gStyle->SetPalette(50, palette);
    MHCamera::Paint(o);

    TText txt;
    txt.SetTextAlign(13);       // left/bottom
    txt.SetTextSize(0.03);

    const Double_t range = fGeomCam->GetMaxRadius();

    Int_t max = 0;
    Int_t num = 0;
    Int_t pos = 0;
    for (int i=0; i<fNumPlayers; i++)
    {
        const Int_t cnt = CountPlayer(i);

        if (cnt>max)
        {
            max = cnt;
            num = i;
        }
        if (cnt==0)
            continue;

        TString str = Form("Player #%d: %d %s", i+1, cnt, i==fNumPlayer?"<*>":"");

        txt.SetTextColor(GetColor(i, fMinimum, fMaximum, kFALSE));
        txt.PaintText(-range*0.95, range-pos*range*0.06, str);

        pos++;
    }

    if (CountPlayers()==1)
    {
        TString str = "And the Winner... is... player #";
        str += Form("%d (Score=%d)", num+1, max);
        txt.SetTextColor(kRed);  // white
        txt.SetTextAlign(22);    // centered/centered
        txt.SetTextSize(0.05);   // white
        txt.PaintText(0, 0, str);
    }

    if (fNumPlayers==0)
    {
        txt.SetTextSize(0.075);
        txt.SetTextColor(kBlack);

        txt.SetTextAlign(13); // left/top
        txt.PaintTextNDC(0.02, 0.99, GetName());

        //txt.SetTextAlign(11); // left/bottom
        //txt.PaintTextNDC(0.01, 0.01, Form("%d", GetNumPixels()));
        txt.PaintTextNDC(0.02, 0.91, Form("%d", GetNumPixels()));
    }
}

// --------------------------------------------------------------------------
//
// Calls draw of the base class MHCamera with the necessary options
//
void MagicJam::Draw(Option_t *o)
{
    MHCamera::Draw(fNumPlayers>0 ? "pixelentries nopal" : "nopal");
    gPad->SetBit(kCannotPick);
}
