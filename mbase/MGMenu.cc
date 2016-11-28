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
!   Author(s): Thomas Bretz, 4/2003 <mailto:tbretz@astro-uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGMenu, MGPopupMenu
//
/////////////////////////////////////////////////////////////////////////////
#include "MGMenu.h"


MGMenuEntry::MGMenuEntry(TGMenuEntry *ent)
{
    memcpy(this, ent, sizeof(*ent));
}

// -------------------------------------------------------------
//
// Return the keycode corresponding to the hot string of
// a TGmenuEntry
//
UInt_t MGPopupMenu::GetKeyCode(TGMenuEntry *el)
{
    TGHotString *str = MGMenuEntry(el).GetLabel();
    return gVirtualX->KeysymToKeycode(str->GetHotChar());
}

// -------------------------------------------------------------
//
// Bind all hot keys used in this popup menu subsequentlt
// to the TGMainFrame and send a HandleKey message to the
// TGWindow
//
void MGPopupMenu::BindKeys(const TGWindow *w, TGMainFrame *frame)
{
    TIter Next(fEntryList);
    TGMenuEntry *el;

    //
    // Loop Through all entries
    //
    while ((el=(TGMenuEntry*)Next()))
    {
        switch (el->GetType())
        {
            //
            // For seperators and labels nothing to do
            //
        case kMenuSeparator:
        case kMenuLabel:
            continue;

            //
            // For an entry and a popup menu bind the hot key
            // In case of a popup menu call BindKeys subsequently
            //
        case kMenuEntry:
        case kMenuPopup:
            frame->BindKey(w, GetKeyCode(el), kKeyMod1Mask);
            if (el->GetType()==kMenuPopup)
                MGMenuEntry(el).GetPopup()->BindKeys(w, frame);
            continue;
        }
    }
}

/*
 kMenuActiveMask  = BIT(0),
 kMenuEnableMask  = BIT(1),
 kMenuDefaultMask = BIT(2),
 kMenuCheckedMask = BIT(3),
 kMenuRadioMask   = BIT(4),
 kMenuHideMask    = BIT(5)
 */

// -------------------------------------------------------------
//
//  Handle a keyboard event. Return kFALSE in case of a
//  successfully send message, which means: close all open
//  popups.
//
Bool_t MGPopupMenu::HandleKey(Event_t *evt)
{
    //
    // Loop through all entries in this popup menu. If the entry is
    // an open popup menu send the key event to the open popup.
    //
    TIter Next(fEntryList);
    TGMenuEntry *el;
    while ((el=(TGMenuEntry*)Next()))
    {
        if (el->GetType()==kMenuPopup && el->GetStatus()&kMenuActiveMask)
            return MGMenuEntry(el).GetPopup()->HandleKey(evt);
    }

    Next.Reset();

    //
    // If no open popup is found search the pressed key in this
    // popup menu.
    //
    while ((el=(TGMenuEntry*)Next()))
    {
        switch (el->GetType())
        {
            //
            // Do nothing
            //
        case kMenuSeparator:
        case kMenuLabel:
            continue;

            //
            // If the keycode corresponds to the hot key
            // of this entry and the entry is enabled activate the entry
            // and send the corresponding message/
            //
        case kMenuEntry:
            if (GetKeyCode(el)==evt->fCode && el->GetStatus()&kMenuEnableMask)
            {
                Activate(el);
                SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_MENU),
                            el->GetEntryId(), (Long_t)MGMenuEntry(el).GetUserData());
                return kFALSE;
            }
            continue;

            //
            // If the keycode corresponds to the hot key
            // of this popup menu activate the popup menu.
            //
        case kMenuPopup:
            if (GetKeyCode(el)!=evt->fCode)
                continue;

            Activate(el);
            HandleTimer(NULL);
            return kTRUE;
        }
    }
    return kTRUE;
}

// -------------------------------------------------------------
//
// Bind the keys of all popups subsequently to the given main
// frame. The menu bar hot keys are already bound by TGMenuBar.
// Bind the Escape key to close the popups, too.
//
void MGMenuBar::BindKeys(TGMainFrame *frame)
{
    TGFrameElement *el=NULL;
    TIter Next(fList);
    while ((el = (TGFrameElement *)Next()))
        ((MGPopupMenu*)((TGMenuTitle *) el->fFrame)->GetMenu())->BindKeys(this, frame);

    frame->BindKey(this, 9/*ESC*/, 0);
}

// -------------------------------------------------------------
//
// Handle the keyboard event send to this menu bar.
//
Bool_t MGMenuBar::HandleKey(Event_t *event)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,02,00)
    //
    // If this isn't a pressed key do nothing
    //
    if (event->fType != kGKeyPress)
        return kTRUE;

    //
    // Check whether one popup is alreadu open
    //
    TGFrameElement *el=NULL;
    TIter Next(fList);
    while ((el = (TGFrameElement *)Next()))
    {
        if (!((TGMenuTitle*)el->fFrame)->GetState())
            continue;

        TGMenuTitle &f = *(TGMenuTitle*)el->fFrame;

        //
        // If a open popup is found redirect the key event to this
        // popup menu
        //
        Bool_t rc = ((MGPopupMenu*)f.GetMenu())->HandleKey(event);

        //
        // If a message could be successfully send or the escape key
        // was pressed close the popup.
        //
        if (rc && event->fCode!=9/*ESC*/)
            return kTRUE;

        f.SetState(kFALSE);
        gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab pointer
        gVirtualX->SetKeyAutoRepeat(kTRUE); // set in TGMainFrame::HandleKey
        fCurrent = 0;
        return kTRUE;
    }
#endif

    return TGMenuBar::HandleKey(event);
}

void MGMenuBar::BindKeys(Bool_t b)
{
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,02,00)
    TGMenuBar::BindKeys(b);
#endif
}  // root>=4.02.00
