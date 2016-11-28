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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */
#include "MOnlineDisplay.h"

//
// C-lib
//
#include <stdlib.h>              // atoi

//
// root
//
#include <TList.h>               // TList::Add
#include <TStyle.h>              // gStyle->SetOptStat
#include <TCanvas.h>             // TCanvas::cd

//
// root GUI
//
#include <TGLabel.h>             // TGLabel
#include <TGButton.h>            // TGPictureButton
#include <TGSlider.h>            // TGSlider
#include <TG3DLine.h>            // TGHorizontal3DLine
#include <TGTextEntry.h>         // TGTextEntry
#include <TGButtonGroup.h>       // TGVButtonGroup
#include <TRootEmbeddedCanvas.h> // TRootEmbeddedCanvas

//
// General
//
#include "MGList.h"              // MGList

#include "MParList.h"            // MParList::AddToList
#include "MEvtLoop.h"            // MEvtLoop::GetParList
#include "MTaskList.h"           // MTaskList::AddToList

//
// Tasks
//
#include "MReadMarsFile.h"       // MReadMarsFile
#include "MGeomApply.h"          // MGeomApply
#include "MFDataMember.h"        // MFDataMember
#include "MMcPedestalCopy.h"     // MMcPedestalCopy
#include "MMcPedestalNSBAdd.h"   // MMcPedestalNSBAdd
#include "MCerPhotCalc.h"        // MCerPhotCalc
#include "MCerPhotAnal2.h"       // MCerPhotAnal2
#include "MImgCleanStd.h"        // MImgCleanStd
#include "MHillasCalc.h"         // MHillasCalc
#include "MHillasSrcCalc.h"      // MHillasSrcCalc
#include "MFillH.h"              // MFillH
#include "MGTask.h"              // MGTask

//
// Container
//
#include "MHEvent.h"             // MHEvent
#include "MHCamera.h"            // MHCamera
#include "MRawEvtData.h"         // MRawEvtData

ClassImp(MOnlineDisplay);

// --------------------------------------------------------------------------
//
//  Constructor.
//
MOnlineDisplay::MOnlineDisplay() : MStatusDisplay(), fTask(0)
{
    //
    // Add MOnlineDisplay GUI elements to the display
    //
    AddUserFrame();

    //
    // Show new part of the window, resize to correct aspect ratio
    //
    // FIXME: This should be done by MStatusDisplay automatically
    Resize(GetWidth(), GetHeight() + fUserFrame->GetDefaultHeight());
    SetWindowName("Online Display");
    MapSubwindows();
}

// --------------------------------------------------------------------------
//
//  Add the top part of the frame: This is filename and treename display
//
void MOnlineDisplay::AddTopFramePart1(TGCompositeFrame *vf1)
{
    TGLabel *file = new TGLabel(vf1, new TGString("Magic Online Analysis -- MONA"));
    TGLayoutHints *laystd = new TGLayoutHints(kLHintsCenterX, 5, 5);
    fList->Add(file);
    fList->Add(laystd);
    vf1->AddFrame(file,  laystd);
}

// --------------------------------------------------------------------------
//
//  Add the second part of the top frame: This are the event number controls
//
void MOnlineDisplay::AddTopFramePart2(TGCompositeFrame *vf1)
{
    //
    // --- the top2 part of the window ---
    //
    TGHorizontalFrame *top2 = new TGHorizontalFrame(vf1, 1, 1);
    fList->Add(top2);

    //
    // Create the gui elements
    //
    TGTextButton *freeze = new TGTextButton(top2, " Freeze ", kFreeze);
    freeze->SetUserData(freeze);
    freeze->Associate(this);
    freeze->SetToolTipText("Freeze the current Event");

    fList->Add(freeze);

    //
    // add the gui elements to the frame
    //
    TGLayoutHints *laystd = new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 5);
    fList->Add(laystd);

    top2->AddFrame(freeze, laystd);

    TGLayoutHints *laystd2 = new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5);
    fList->Add(laystd2);
    vf1->AddFrame(top2, laystd2);

    //
    // Add trailing line...
    //
    TGHorizontal3DLine *line = new TGHorizontal3DLine(vf1);
    TGLayoutHints *layline = new TGLayoutHints(kLHintsExpandX);
    fList->Add(line);
    fList->Add(layline);
    vf1->AddFrame(line, layline);
}

// --------------------------------------------------------------------------
//
//  Add the user frame part of the display
//
void MOnlineDisplay::AddUserFrame()
{
    fUserFrame->ChangeOptions(kHorizontalFrame);

    // Slider
    TGVSlider *slider = new TGVSlider(fUserFrame, 1, kSlider1, kSlider);
    slider->SetRange(10, 300);
    slider->SetScale(15);
    slider->SetPosition(10);
    slider->Associate(this);
    //slider->SetToolTipText("Change the update frequency of the event display (1-30s)");

    // frame1/2
    TGCompositeFrame *vf1 = new TGVerticalFrame(fUserFrame, 1, 1);
    TGCompositeFrame *vf2 = new TGVerticalFrame(fUserFrame, 1, 1);

    AddTopFramePart1(vf1);
    AddTopFramePart2(vf1);

    // create root embedded canvas and add it to the tab
    TRootEmbeddedCanvas *ec = new TRootEmbeddedCanvas("Slices", vf2, vf1->GetDefaultHeight()*3/2, vf1->GetDefaultHeight(), 0);
    vf2->AddFrame(ec);
    fList->Add(ec);

    // set background and border mode of the canvas
    fCanvas = ec->GetCanvas();
    fCanvas->SetBorderMode(0);
    gROOT->GetListOfCanvases()->Add(fCanvas);
    //fCanvas->SetBorderSize(1);
    //fCanvas->SetBit(kNoContextMenu);
    //fCanvas->SetFillColor(16);

    // layout
    TGLayoutHints *lays = new TGLayoutHints(kLHintsExpandY);
    TGLayoutHints *lay = new TGLayoutHints(kLHintsExpandX);
    fUserFrame->AddFrame(slider, lays);
    fUserFrame->AddFrame(vf1, lay);
    fUserFrame->AddFrame(vf2);

    fList->Add(lay);
    fList->Add(lays);
    fList->Add(vf1);
    fList->Add(vf2);
    fList->Add(slider);
}

// --------------------------------------------------------------------------
//
//  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
//
// Processes information from all GUI items.
// Selecting an item usually generates an event with 4 parameters.
// The first two are packed into msg (first and second bytes).
// The other two are parm1 and parm2.
//
Bool_t MOnlineDisplay::ProcessMessage(Long_t msg, Long_t mp1, Long_t mp2)
{
    if (fTask && fTask->ProcessMessage(GET_MSG(msg), GET_SUBMSG(msg), mp1, mp2))
        return kTRUE;

    return MStatusDisplay::ProcessMessage(msg, mp1, mp2);
}
