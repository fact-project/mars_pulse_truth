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
!   Author(s): Thomas Bretz, 9/2002 <mailto:tbretz@astro-uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MSearch
//
// Simple search dialog (usefull for TGTextViews)
//
// Sends: kC_USER, KS_START, mp1, txt
//
//   with mp1: bit0 on=case sensitive
//             bit1 on=backward
//        mp2: char* pointing to the text to search for
//
// WARNING: Do not store mp2, immeditaly copy the text to a local
//          location!
//
/////////////////////////////////////////////////////////////////////////////
#include "MSearch.h"

#include <stdlib.h>         // rand (Ubuntu 8.10)

#include <TSystem.h>      // gSystem
#include <TGLabel.h>      // TGLabel
#include <TGButton.h>     // TGButton
#include <TGTextEntry.h>  // TGTextEntry

#include "MGList.h"

ClassImp(MSearch);

enum
{
    kSearchText, kCase, kDirection, kSearch, kCancel
};

// --------------------------------------------------------------------------
//
// Default constructor. w is the window which will receive the message.
// Id is currently useless.
//
MSearch::MSearch(const TGWindow *w, Int_t id) : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 1, 1), TGWidget(id)
{
    Associate(w);

    fList = new MGList;
    fList->SetOwner();

    // set the smallest and biggest size of the Main frame
    SetWMSizeHints(320, 110, 250, 50, 0, 0);
    Move(rand()%100+50, rand()%100+50);

    // -------------------------------------------------------------

    TGLayoutHints *lay4=new TGLayoutHints(kLHintsNormal|kLHintsExpandX);
    TGLayoutHints *lay0=new TGLayoutHints(kLHintsNormal|kLHintsExpandX, 4, 8, 4);
    TGLayoutHints *lay1=new TGLayoutHints(kLHintsNormal,  6, 4, 8);
    TGLayoutHints *lay2=new TGLayoutHints(kLHintsNormal, 69, 4, 4, 4);
    TGLayoutHints *lay3=new TGLayoutHints(kLHintsNormal, 69, 4, 4, 4);
    TGLayoutHints *lay5=new TGLayoutHints(kLHintsNormal,  5, 5, 5);
    TGLayoutHints *lay7=new TGLayoutHints(kLHintsCenterX);

    // -------------------------------------------------------------
    //  Create Widgets
    // -------------------------------------------------------------

    TGHorizontalFrame  *f = new TGHorizontalFrame(this, 1, 1);
    TGLabel        *label = new TGLabel(this, "Find Text:");
    TGTextEntry    *entry = new TGTextEntry(f, "", kSearchText);
    TGCheckButton   *box1 = new TGCheckButton(this, "Match upper/lower case", kCase);
    TGCheckButton   *box2 = new TGCheckButton(this, "Search backward", kDirection);
    TGHorizontalFrame *f2 = new TGHorizontalFrame(this, 1, 1);
    TGHorizontalFrame *f3 = new TGHorizontalFrame(f2, 1, 1);
    TGTextButton    *txt1 = new TGTextButton(f3, "Search", kSearch);
    TGTextButton    *txt2 = new TGTextButton(f3, "Cancel", kCancel);

    txt1->Associate(this);
    txt2->Associate(this);

    // -------------------------------------------------------------
    //  Layout the widgets in the frame
    // -------------------------------------------------------------

    AddFrame(f, lay4);
    f->AddFrame(label, lay1);
    f->AddFrame(entry, lay0);
    AddFrame(box1, lay2);
    AddFrame(box2, lay3);
    AddFrame(f2, lay4);
    f2->AddFrame(f3, lay7);
    f3->AddFrame(txt1, lay5);
    f3->AddFrame(txt2, lay5);

    // -------------------------------------------------------------

    entry->Associate(this);
    txt1->Associate(this);
    txt1->Associate(this);

    // -------------------------------------------------------------

    fList->Add(lay0);
    fList->Add(lay1);
    fList->Add(lay2);
    fList->Add(lay3);
    fList->Add(lay4);
    fList->Add(lay5);
    fList->Add(lay7);
    fList->Add(f);
    fList->Add(f2);
    fList->Add(f3);
    fList->Add(label);
    fList->Add(entry);
    fList->Add(box1);
    fList->Add(box2);
    fList->Add(txt1);
    fList->Add(txt2);

    // -------------------------------------------------------------

    Layout();

    MapSubwindows();

    SetWindowName("Search in Text");
    SetIconName("Search");

    MapWindow();
}

// --------------------------------------------------------------------------
//
// Destruct the window with all its tiles and widgets.
//
MSearch::~MSearch()
{
    delete fList;
}

// --------------------------------------------------------------------------
//
// Send the search message to the associated TGWindow.
// See class description.
//
Bool_t MSearch::SendSearch()
{
    if (!fMsgWindow)
        return kTRUE;

    const TGCheckButton &b1 = *(TGCheckButton*)fList->FindWidget(kCase);
    const TGCheckButton &b2 = *(TGCheckButton*)fList->FindWidget(kDirection);
    const TGTextEntry   &e  = *(TGTextEntry*)  fList->FindWidget(kSearchText);

    const Long_t msg = MK_MSG(kC_USER, (EWidgetMessageTypes)kS_START);
    const Long_t mp1 = (b1.GetState()<<1) | b2.GetState();
    const Long_t mp2 = (Long_t)e.GetText();

    SendMessage(fMsgWindow, msg, mp1, mp2);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Process messages from the widgets.
//
Bool_t MSearch::ProcessMessage(Long_t msg, Long_t mp1, Long_t /*mp2*/)
{
    // Can be found in WidgetMessageTypes.h
    switch (GET_MSG(msg))
    {
    case kC_COMMAND:
        switch (GET_SUBMSG(msg))
        {
        case kCM_BUTTON:
            switch (mp1)
            {
            case kSearch:
                return SendSearch();
            case kCancel:
                delete this;
                return kTRUE;
            }
            return kTRUE;

        }
        return kTRUE;
    }
    return kTRUE;
}
