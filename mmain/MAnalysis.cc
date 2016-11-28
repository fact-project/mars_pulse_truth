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
!   Author(s): Thomas Bretz, 9/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */
#include "MAnalysis.h"

#include <iostream>

#include <TGLabel.h>       // TGlabel
#include <TGButton.h>      // TGTextButton
#include <TGTextEntry.h>   // TGNumberEntry

#include "MGList.h"
#include "MImgCleanStd.h"  // MImgCleanStd

ClassImp(MAnalysis);

using namespace std;

enum {
    kButHillas = 0x100
};

void MAnalysis::AddButtons()
{
    TGTextButton *hillas = new TGTextButton(fTop2, "Calculate Standard Hillas", kButHillas);

    hillas->Associate(this);

    fList->Add(hillas);

    TGLayoutHints *laybut = new TGLayoutHints(kLHintsCenterY|kLHintsLeft, 10, 10, 5, 5);
    fList->Add(laybut);

    fTop2->AddFrame(hillas, laybut);
}

void MAnalysis::AddSetupTab()
{
    //
    // Create Setup Tab
    //
    TGCompositeFrame *frame = CreateNewTab("Setup");
 /*
    TGGroupFrame *grp = new TGGroupFrame(frame, "Setup Display");
    fList->Add(grp);

    //
    // Align the lines:
    //  - top, left
    //  - padding: top=20, bottom=0, left=20, right=0
    //
    TGLayoutHints *laybut = new TGLayoutHints(kLHintsNormal, 10, 10, 10);
    fList->Add(laybut);

    //
    // Create check buttons for the first two lines
    //
    fCheckButton1 = new TGCheckButton(grp, "Display Hillas Histograms when finished");  //, M_CHECK_DISPLHIL);
    fCheckButton2 = new TGCheckButton(grp, "Display Star Map Histogram when finished"); //, M_CHECK_DISPLHIL);

    fList->AddFirst(fCheckButton1);
    fList->AddFirst(fCheckButton2);

    //
    // Create first two lines with the checkbuttons
    //
    grp->AddFrame(fCheckButton1, laybut);
    grp->AddFrame(fCheckButton2, laybut);
 */
    TGLayoutHints *laygrp = new TGLayoutHints(kLHintsNormal|kLHintsExpandX, 20, 20, 20);
    fList->Add(laygrp);
 /*
    frame->AddFrame(grp, laygrp);
  */

    /*
     * Create GUI for image cleaning
     */
    fImgClean = new MImgCleanStd;
    fImgClean->CreateGui(frame, laygrp);
    fList->Add(fImgClean);
}

MAnalysis::MAnalysis(/*const TGWindow *main,*/ const TGWindow *p,
                     const UInt_t w, const UInt_t h)
: MBrowser(/*main,*/ p, w, h)
{
    AddButtons();
    AddSetupTab();

    MapSubwindows();

    Layout();

    SetWindowName("Analysis Window");
    SetIconName("Analysis");

    MapWindow();
}

// ======================================================================

#include "MStatusDisplay.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MGeomCamMagic.h"
#include "MHHillas.h"
#include "MHStarMap.h"
#include "MReadMarsFile.h"
#include "MMcPedestalCopy.h"     // MMcPedestalCopy
#include "MMcPedestalNSBAdd.h"   // MMcPedestalNSB
#include "MCerPhotCalc.h"
#include "MImgCleanStd.h"
#include "MHillasCalc.h"
#include "MHillasSrcCalc.h"
#include "MSrcPosCam.h"
#include "MFillH.h"
#include "MEvtLoop.h"
#include "MHillas.h"
#include "MGeomApply.h"

void MAnalysis::CalculateHillas()
{
    //
    // This is a demonstration program which calculates the Hillas
    // parameter out of a Magic root file.

    const Bool_t displhillas  = kTRUE;//fCheckButton1->GetState();
    const Bool_t displstarmap = kTRUE;//fCheckButton2->GetState();

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // The geometry container must be created by yourself to make sure
    // that you don't choos a wrong geometry by chance
    //
    MGeomCamMagic geomcam;
    plist.AddToList(&geomcam);

    MSrcPosCam source;
    plist.AddToList(&source);
    /*
     MSrcPosCam source("Source");
     source.SetXY(0, 0);
     plist.AddToList(&source);

     MSrcPosCam antisrc("AntiSrc");
     antisrc.SetXY(240, 0);
     plist.AddToList(&antisrc);
     */

    //
    // Now setup the tasks and tasklist:
    //
    //  1) read in the data from a magic root file   MReadTree
    //  2) calculate number of cerenkov photons      MCerPhotCalc
    //  3) clean the image                           MImgCleanStd
    //  4) calculate hillas                          MHillasCalc
    //  5) fill the hillas into the histograms       MFillH
    //

    //
    // The first argument is the tree you want to read.
    //   Events:     Cosmic ray events
    //   PedEvents:  Pedestal Events
    //   CalEvents:  Calibration Events
    //
    MReadMarsFile read("Events", fInputFile);
    //read.DisableAutoScheme();

    MGeomApply         apply;
    MMcPedestalCopy    pcopy;
    MMcPedestalNSBAdd  pdnsb;
    MCerPhotCalc       ncalc;
    //MBlindPixelCalc    blind;
    MHillasCalc        hcalc;
    MHillasSrcCalc     csrc1;
    /*
     MHillasSrcCalc     csrc1("Source",  "HillasSource");
     MHillasSrcCalc     csrc2("AntiSrc", "HillasAntiSrc");
     */

    MFillH hfill("MHHillas",       "MHillas");
    MFillH hfill2("MHHillasExt",   "MHillasSrc");
    MFillH hfill2s("MHHillasSrc",  "MHillasSrc");
    MFillH hfill3("MHNewImagePar", "MNewImagePar");
    MFillH sfill("MHStarMap",      "MHillas");
    /*
     MFillH hfill2s("HistSource  [MHHillasSrc]", "HillasSource");
     MFillH hfill2a("HistAntiSrc [MHHillasSrc]", "HillasAntiSrc");
     */

    tlist.AddToList(&read);
    tlist.AddToList(&apply);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pdnsb);
    tlist.AddToList(&ncalc);
    tlist.AddToList(fImgClean);
    //tlist.AddToList(&blind);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&csrc1);
    //tlist.AddToList(&csrc2);

    if (displhillas)
    {
        tlist.AddToList(&hfill);
        tlist.AddToList(&hfill2);
        tlist.AddToList(&hfill2s);
        tlist.AddToList(&hfill3);
    }

    if (displstarmap)
        tlist.AddToList(&sfill);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Add ProgressBar to window
    //
    TGProgressBar *bar = NULL;
    if (displhillas || displstarmap)
    {
        MStatusDisplay *d=new MStatusDisplay;
        evtloop.SetDisplay(d);
    }
    else
    {
        bar = CreateProgressBar(fTop2);
        evtloop.SetProgressBar(bar);
    }


    //
    // Execute your analysis
    //
    /*Bool_t rc =*/ evtloop.Eventloop();

    //
    // Remove progressbar from window
    //
    if (bar)
        DestroyProgressBar(bar);

 /*
    if (!rc)
        return;

    //
    // After the analysis is finished we can display the histograms
    //
    if (displhillas)
    {
        plist.FindObject("MHHillas")->DrawClone();
        plist.FindObject("MHHillasExt")->DrawClone();
        plist.FindObject("MHHillasSrc")->DrawClone();
        plist.FindObject("MHNewImagePar")->DrawClone();
    }

    if (displstarmap)
        plist.FindObject("MHStarMap")->DrawClone();

    cout << endl;
    cout << "Calculation of Hillas Parameters finished without error!" << endl;
 */
}

// ======================================================================

Bool_t MAnalysis::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    // Process events generated by the buttons in the frame.

    if (GET_MSG(msg)!=kC_COMMAND || GET_SUBMSG(msg)!=kCM_BUTTON)
        return MBrowser::ProcessMessage(msg, parm1, parm2);

    switch (parm1)
    {
    case kButHillas:
        if (!InputFileSelected())
        {
            DisplError("No Input (root) File selected!");
            return kTRUE;
        }

        switch (parm1)
        {
        case kButHillas:
            CalculateHillas();
            return kTRUE;
        }
        return kTRUE;
    }

    return MBrowser::ProcessMessage(msg, parm1, parm2);
}
