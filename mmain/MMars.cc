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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Harald Kornmayer 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

#include "MMars.h"

#include <iostream>

#include <TApplication.h>  // gROOT->GetApplication()->...

#include <TGTab.h>         // TGTab
#include <TGMenu.h>        // TGPopupMenu
#include <TSystem.h>       // gSystem->Exec
#include <TGMsgBox.h>      // TGMsgBox
#include <TGButton.h>      // TGPictureButton
#include <TG3DLine.h>      // TGHorizontal3DLine

#include "MGList.h"
#include "MAnalysis.h"
#include "MDataCheck.h"
#include "MMonteCarlo.h"
#include "MCameraDisplay.h"

ClassImp(MMars);

using namespace std;

enum {
    kFileExit,
    kFileAbout,

    kPicMagic,
    kPicMars,

    //kButEvtDisplay,
    kButDataCheck,
    kButAnalysis,
    kButMonteCarlo,
    kButCameraDisplay
};

void MMars::CreateMenuBar()
{
    TGLayoutHints *laymenubar  = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX, 2, 2, 2, 2);
    TGLayoutHints *laymenuitem = new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0);
    TGLayoutHints *laylinesep  = new TGLayoutHints(kLHintsTop|kLHintsExpandX);

    fList->Add(laymenubar);
    fList->Add(laymenuitem);
    fList->Add(laylinesep);

    TGPopupMenu *filemenu = new TGPopupMenu(gClient->GetRoot());
    filemenu->AddEntry("E&xit", kFileExit);
    filemenu->Associate(this);

    TGMenuBar *menubar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
    menubar->AddPopup("&File", filemenu, laymenuitem);
    AddFrame(menubar, laymenubar);

    TGHorizontal3DLine *linesep = new TGHorizontal3DLine(this);
    AddFrame(linesep, laylinesep);

    fList->Add(filemenu);
    fList->Add(menubar);
    fList->Add(linesep);
}

void MMars::CreateTopFrame(TGHorizontalFrame *top)
{
    const TGPicture *pic1 = fList->GetPicture("magiclogo.xpm");
    if (pic1)
    {
        TGPictureButton *magic = new TGPictureButton(top, pic1, kPicMagic);
        fList->Add(magic);
        magic->Associate(this);
        TGLayoutHints *lay1 = new TGLayoutHints(kLHintsLeft,  10, 10, 20, 10);
        fList->Add(lay1);
        top->AddFrame(magic, lay1);
    }

    const TGPicture *pic2 = fList->GetPicture("marslogo.xpm");
    if (pic2)
    {
        TGPictureButton *mars  = new TGPictureButton(top, pic2, kPicMars);
        fList->Add(mars);
        mars->Associate(this);
        TGLayoutHints *lay2 = new TGLayoutHints(kLHintsRight, 10, 10, 10, 10);
        fList->Add(lay2);
        top->AddFrame(mars,  lay2);
    }
}

#include <TGLabel.h>

void MMars::CreateTextButton(TGVerticalFrame *tab,
                             const char *text, const char *descr,
                             const UInt_t id) const
{
    //
    // Create the button
    //
    TGHorizontalFrame *frame  = new TGHorizontalFrame(tab, 1, 1);
    TGTextButton      *button = new TGTextButton(frame, text, id);
    TGLabel           *label  = new TGLabel(frame, descr);
    TGLayoutHints     *hints1 = new TGLayoutHints(kLHintsLeft|kLHintsCenterY|kLHintsExpandX, 5, 5, 2, 2);

    //
    // Add button for 'auto-delete'
    //
    fList->Add(hints1);
    fList->Add(button);
    fList->Add(label);
    fList->Add(frame);

    //
    // Send button events (meesages) to this object (s. ProcessMessage)
    //
    button->Associate(this);

    //
    // Add button with corresponding layout to containing frame
    //
    tab->AddFrame(frame,    hints1);
    frame->AddFrame(button, hints1);
    frame->AddFrame(label,  hints1);
}

void MMars::CreateBottomFrame(TGHorizontalFrame *low)
{
    //
    // Create Tab Container
    //
    TGLayoutHints *laytabs = new TGLayoutHints(kLHintsBottom|kLHintsExpandX|kLHintsExpandY, 5, 5, 5, 5);
    fList->Add(laytabs);

    TGTab *tabs = new TGTab(low, 1, 1);
    fList->Add(tabs);
    low->AddFrame(tabs, laytabs);

    //
    // Create Tab1
    //
    TGCompositeFrame *tf = tabs->AddTab("Control");

    TGLayoutHints   *laytab = new TGLayoutHints(kLHintsCenterY|kLHintsExpandX);
    TGVerticalFrame *tab    = new TGVerticalFrame(tf, 1, 1);
    fList->Add(laytab);
    fList->Add(tab);

//    CreateTextButton(tab, "Event Display",  "Histograms: Pix per Event",
//                     kButEvtDisplay);
    CreateTextButton(tab, "Data Check",     "Histograms: Pix per Run",
                     kButDataCheck);
    CreateTextButton(tab, "Analysis",       "Calculate image parameters",
                     kButAnalysis);
    CreateTextButton(tab, "Monte Carlo",    "Calculate MC stuff",
                     kButMonteCarlo);
    CreateTextButton(tab, "Camera Display", "Display Cerenkov Photons",
                     kButCameraDisplay);

    tf->AddFrame(tab, laytab);
}

MMars::MMars()
: TGMainFrame(gClient->GetRoot(), 400, 400, kVerticalFrame)
{
    //
    // Create the deletion list
    //
    fList = new MGList;
    fList->SetOwner();

    //
    // Create the MenuBar
    //
    CreateMenuBar();

    //
    // create and layout the frame/contents
    //
    TGHorizontalFrame *top = new TGHorizontalFrame(this, 1, 1);
    TGHorizontalFrame *low = new TGHorizontalFrame(this, 1, 1);

    TGHorizontal3DLine *linesep = new TGHorizontal3DLine(this);

    fList->Add(top);
    fList->Add(low);
    fList->Add(linesep);

    CreateTopFrame(top);
    CreateBottomFrame(low);

    TGLayoutHints *layout1 = new TGLayoutHints(kLHintsTop|kLHintsExpandX);
    TGLayoutHints *layout2 = new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY);
    fList->Add(layout1);
    fList->Add(layout2);

    AddFrame(top,     layout1);
    AddFrame(linesep, layout1);
    AddFrame(low,     layout2);

    //
    //   Map the window, set up the layout, etc.
    //
    Move(rand()%100, rand()%100);

    Layout();

    MapSubwindows();

    SetWindowName("MARS Main Window");
    SetIconName("MARS");

    MapWindow();
} 

// ======================================================================

MMars::~MMars()
{
    delete fList;
}  
// ======================================================================

void MMars::CloseWindow()
{
   // Got close message for this MainFrame. Calls parent CloseWindow()
   // (which destroys the window) and terminate the application.
   // The close message is generated by the window manager when its close
   // window menu item is selected.

   TGMainFrame::CloseWindow();
   gROOT->GetApplication()->Terminate(0);
}

void MMars::DisplWarning(const char *txt)
{
    Int_t retval;
    new TGMsgBox(fClient->GetRoot(), this,
                 "WARNING!", txt,
                 kMBIconExclamation, 4, &retval);
}

Bool_t MMars::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{     
    // Process events generated by the buttons in the frame.

    switch (GET_MSG(msg))
    {
    case kC_COMMAND:
        switch (GET_SUBMSG(msg))
        {
        case kCM_BUTTON:

            switch (parm1)
            {
/*
            case kButEvtDisplay:
                new MEvtDisp();
                return kTRUE;
*/
            case kButDataCheck:
                new MDataCheck(/*this*/);
                return kTRUE;

            case kButAnalysis:
                new MAnalysis(/*this*/);
                return kTRUE;

            case kButMonteCarlo:
                new MMonteCarlo(/*this*/);
                return kTRUE;

            case kButCameraDisplay:
                new MCameraDisplay(/*this*/);
                return kTRUE;

            case kPicMagic:
                cout << "Trying to start 'netscape http://hegra1.mppmu.mpg.de/MAGICWeb/'..." << endl;
                gSystem->Exec("netscape http://hegra1.mppmu.mpg.de/MAGICWeb/ &");
                return kTRUE;

            case kPicMars:
                cout << "Trying to start 'netscape http://magic.astro.uni-wuerzburg.de/mars/'..." << endl;
                gSystem->Exec("netscape http://magic.astro.uni-wuerzburg.de/mars/ &");
                return kTRUE;
            }

	case kCM_MENU:
            if (parm1!=kFileExit)
                return kTRUE;

            CloseWindow();
            return kTRUE;
	}
    }

    return kTRUE;
}
