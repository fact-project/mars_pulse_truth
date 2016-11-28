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
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

#include "MMonteCarlo.h"

#include <stdlib.h>
#include <iostream>

#include <TGLabel.h>        // TGLabel
#include <TGButton.h>       // TGTextButton
#include <TGTextEntry.h>    // TGTextEntry
#include <TGProgressBar.h>  // TGHProgressBar
#include <TGButtonGroup.h>  // TGVButtonGroup

#include "MGList.h"

ClassImp(MMonteCarlo);

using namespace std;

enum {
    kButCollArea  = 0x100,
    kButTrigRate  = 0x101,
    kButThreshold = 0x102
};

void MMonteCarlo::AddButtons()
{
    TGTextButton *carea = new TGTextButton(fTop2, "Collection Area", kButCollArea);
    TGTextButton *trate = new TGTextButton(fTop2, "Trigger Rate",    kButTrigRate);
    TGTextButton *thold = new TGTextButton(fTop2, "Threshold",       kButThreshold);

    fList->Add(carea);
    fList->Add(trate);
    fList->Add(thold);

    carea->Associate(this);
    trate->Associate(this);
    thold->Associate(this);

    TGLayoutHints *laybut = new TGLayoutHints(kLHintsNormal, 5, 5, 10, 10);
    fList->Add(laybut);

    fTop2->AddFrame(carea, laybut);
    fTop2->AddFrame(trate, laybut);
    fTop2->AddFrame(thold, laybut);
}

void MMonteCarlo::AddSetupTab()
{
    //
    // Create Setup Tab
    //
    TGCompositeFrame *frame = CreateNewTab("Setup");

    //
    // Create a button group (it alignes the buttons and make
    // them automatic radio buttons)
    // Create three (auto) radio buttons in the button group
    //
    TGVButtonGroup *group = new TGVButtonGroup(frame);
    fList->Add(group);

    fRadioButton1 = new TGRadioButton(group, "Use unnumbered trigger condition olny.");
    fRadioButton2 = new TGRadioButton(group, "Use only one trigger condition (specify number below).");
    fRadioButton3 = new TGRadioButton(group, "Use a number of trigger conditions (1..n).");

    fRadioButton1->SetState(kButtonDown);

    /*
     WARNING:
     Bacause of some strage and hidden dependencies the
     GetMainFrame call in the destructor of TGButton may fail if some
     of the other gui elemts is deleted first.
     AddFirst adds the buttons at the beginning of the deletion list,
     this seems to work.
     */
    fList->AddFirst(fRadioButton1);
    fList->AddFirst(fRadioButton2);
    fList->AddFirst(fRadioButton3);

    //
    // Add the button group (all buttons) as first line
    //
    frame->AddFrame(group);

    //
    // Create entry fields and labels for line 3 and 4
    //

    /*
     * --> use with root >=3.02 <--
     *

     TGNumberEntry *fNumEntry1 = new TGNumberEntry(frame, 3.0, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);
     TGNumberEntry *fNumEntry2 = new TGNumberEntry(frame, 2.5, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);

     */

    //
    // Align the lines:
    //  - top, left
    //  - padding: top=20, bottom=0, left=20, right=0
    //
    TGLayoutHints *layline = new TGLayoutHints(kLHintsNormal, 20, 0, 20);
    fList->Add(layline);


    //
    // Create a frame for line 3 and 4 to be able
    // to align entry field and label in one line
    //
    TGHorizontalFrame *f = new TGHorizontalFrame(frame, 0, 0);
    fNumEntry = new TGTextEntry(f, "****");
    fNumEntry->SetText("1");
    fList->Add(fNumEntry);

    // --- doesn't work like expected --- fNumEntry1->SetAlignment(kTextRight);
    // --- doesn't work like expected --- fNumEntry2->SetAlignment(kTextRight);

    TGLabel *l = new TGLabel(f, "Trigger Condition Setup.");
    l->SetTextJustify(kTextLeft);
    fList->Add(l);

    //
    // Align the text of the label centered, left in the row
    // with a left padding of 10
    //
    TGLayoutHints *laylabel = new TGLayoutHints(kLHintsCenterY|kLHintsLeft, 10); //, 10); //, 5, 5);
    fList->Add(laylabel);

    //
    // Add one entry field and the corresponding label to each line
    //
    f->AddFrame(fNumEntry);
    f->AddFrame(l, laylabel);

    //
    // Add line 3 and 4 to tab
    //
    frame->AddFrame(f, layline);
}

MMonteCarlo::MMonteCarlo(/*const TGWindow *main,*/ const TGWindow *p,
                         const UInt_t w, const UInt_t h)
: MBrowser(/*main,*/ p, w, h)
{
    AddButtons();
    AddSetupTab();

    MapSubwindows();

    Layout();

    SetWindowName("MonteCarlo Main");
    SetIconName("MonteCarlo");

    MapWindow();
} 

// ======================================================================
#include <TObjArray.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MReadMarsFile.h"

#include "MHMcRate.h"
#include "MHMcEnergy.h"

#include "MMcTriggerRateCalc.h"
#include "MMcThresholdCalc.h"
#include "MMcCollectionAreaCalc.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,05)
#include "../mmc/MMcTrig.hxx" // FIXME: see FIXME below
#endif

Int_t MMonteCarlo::GetDim() const
{
    Int_t dim = atoi(fNumEntry->GetText());

    if (dim<0)
    {
        dim=0;
        fNumEntry->SetText("0");
    }

    if (fRadioButton1->GetState())
        dim = 0;

    if (fRadioButton2->GetState())
        dim = -dim;

    return dim;
}

void MMonteCarlo::CalculateCollectionArea()
{
    //
    // first we have to create our empty lists
    //
    MParList plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,05)
    //
    // FIXME: This line is needed that root finds the MMc-classes in the
    // dictionary when calling the constructor of MReadTree
    // I don't have any idea why...
    // Rem: This happens only in this GUI!
    //
    MMcTrig trig;
#endif

    //
    // Setup out tasks:
    //  - First we have to read the events
    //  - Then we can fill the efficiency histograms
    //
    MReadMarsFile read("Events", fInputFile);
    tlist.AddToList(&read);

    MMcCollectionAreaCalc effi;
    tlist.AddToList(&effi);

    //
    // set up the loop for the processing
    //
    MEvtLoop magic;
    magic.SetParList(&plist);

    //
    // Add ProgressBar to window
    //
    TGProgressBar *bar = CreateProgressBar(fTop2);
    magic.SetProgressBar(bar);

    //
    // Execute your analysis
    //
    Bool_t rc = magic.Eventloop();

    //
    // Remove progressbar from window
    //
    DestroyProgressBar(bar);

    if (!rc)
        return;

    //
    // Now the histogram we wanted to get out of the data is
    // filled and can be displayd
    //
    plist.FindObject("MHMcCollectionArea")->DrawClone();
}

void MMonteCarlo::CalculateTriggerRate()
{
    //
    // dim : = 0 -> root file with 1 trigger condition.
    //       > 0 -> number of trigger condition to be analised 
    //              in multi conditon file.
    //       < 0 -> selects the -dim trigger condition.
    //
    const Int_t dim = GetDim();

    MParList plist;
    MTaskList tlist;

    //
    // Setup the parameter list.
    //
    plist.AddToList(&tlist);

    const UInt_t from = dim>0 ?   1 : -dim;
    const UInt_t to   = dim>0 ? dim : -dim;
    const Int_t  num  = to-from+1;

    TObjArray hists(MParList::CreateObjList("MHMcRate", from, to));
    hists.SetOwner();

    //
    // Check if the list really contains the right number of histograms
    //
    if (hists.GetEntriesFast() != num)
        return;

    //
    // Set for each MHMcRate object the trigger condition number in the 
    // camera file (for the case of camera files with several conditions,
    // produced with the trigger_loop option.
    //
    if (dim < 0)
        ((MHMcRate*)(hists[0]))->SetTriggerCondNum(to);
    else
        for (UInt_t i=from; i<=to; i++)
            ((MHMcRate*)(hists[i-1]))->SetTriggerCondNum(i);

    //
    // Add the histograms to the parameter list.
    //
    plist.AddToList(&hists);

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,05)
    //
    // FIXME: This line is needed that root finds the MMc-classes in the
    // dictionary when calling the constructor of MReadTree
    // I don't have any idea why...
    // Rem: This happens only in this GUI!
    //
    MMcTrig trig;
#endif

    //
    // Setup out tasks:
    //  - First we have to read the events
    //  - Then we can calculate rates, for what the number of
    //    triggered showers from a empty reflector file for the
    //    analised trigger conditions should be set (BgR[])
    //
    MReadMarsFile read("Events", fInputFile);
    tlist.AddToList(&read);

    // We calculate only shower rate (not including NSB-only triggers)
    Float_t* BgR = new Float_t[num];
    memset(BgR, 0, num*sizeof(Float_t));

    MMcTriggerRateCalc crate(dim, BgR, 100000);
    tlist.AddToList(&crate);

    //
    // set up the loop for the processing
    //
    MEvtLoop magic;
    magic.SetParList(&plist);

    //
    // Add ProgressBar to window
    //
    TGProgressBar *bar = CreateProgressBar(fTop2);
    magic.SetProgressBar(bar);

    //
    // Execute your analysis
    //
    Bool_t rc = magic.Eventloop();

    //
    // Remove progressbar from window
    //
    DestroyProgressBar(bar);

    delete BgR;

    if (!rc)
        return;

    hists.Print();
}

void MMonteCarlo::CalculateThreshold()
{
    const Int_t dim = GetDim();

    MParList plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Create numtriggerconditions histograms of type MHMcEnergy
    // and store the histograms in an TObjArray
    //
    const UInt_t from = dim>0 ?   1 : -dim;
    const UInt_t to   = dim>0 ? dim : -dim;
    const Int_t  num  = to-from+1;

    TObjArray hists(MParList::CreateObjList("MHMcEnergy", from, to));
    hists.SetOwner();

    //
    // Check if the list really contains the right number of histograms
    //
    if (hists.GetEntriesFast() != num)
        return;

    //
    // Add the histograms to the paramater list.
    //
    plist.AddToList(&hists);

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,05)
    //
    // FIXME: This line is needed that root finds the MMc-classes in the
    // dictionary when calling the constructor of MReadTree
    // I don't have any idea why...
    // Rem: This happens only in this GUI!
    //
    MMcTrig trig;
#endif

    //
    // Setup the task list
    //
    MReadTree read("Events", fInputFile);

    MMcThresholdCalc calc(dim);
    tlist.AddToList(&read);
    tlist.AddToList(&calc);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Add ProgressBar to window
    //
    TGProgressBar *bar = CreateProgressBar(fTop2);
    evtloop.SetProgressBar(bar);

    //
    // Execute your analysis
    //
    Bool_t rc = evtloop.Eventloop();

    //
    // Remove progressbar from window
    //
    DestroyProgressBar(bar);

    if (!rc)
        return;

    //
    // Now you can display the results
    //
    hists.R__FOR_EACH(TObject, DrawClone)();
}

// ======================================================================

Bool_t MMonteCarlo::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    if (GET_MSG(msg) == kC_COMMAND && GET_SUBMSG(msg) == kCM_BUTTON)
        switch (parm1)
        {
        case kButCollArea:
            CalculateCollectionArea();
            return kTRUE;

        case kButTrigRate:
            CalculateTriggerRate();
            return kTRUE;

        case kButThreshold:
            CalculateThreshold();
            return kTRUE;
        }

    return MBrowser::ProcessMessage(msg, parm1, parm2);
}
