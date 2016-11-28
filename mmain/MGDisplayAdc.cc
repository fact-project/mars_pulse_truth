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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer, 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

#include "MGDisplayAdc.h"

#include <stdlib.h>              // mkstemp
#include <iostream>              // cout for debugging

#include <TSystem.h>             // gSystem
#include <TCanvas.h>             // TCanvas.h
#include <TGSlider.h>            // TGVSlider
#include <TGButton.h>            // TGTextButton
#include <TGMsgBox.h>            // TGMsgBox
#include <TGListBox.h>           // TGListBox
#include <TGButtonGroup.h>       // TGVButtonGroup
#include <TRootEmbeddedCanvas.h> // TRootEmbeddedCanvas

#include "MHFadcCam.h"

using namespace std;

ClassImp(MGDisplayAdc);

enum ComIdentDisplayAdc
{
    M_BUTTON_SAVE,
    M_BUTTON_PRINT,
    M_BUTTON_PRINTALL,
    M_BUTTON_CLOSE,

    M_BUTTON_PREV,
    M_BUTTON_NEXT,

    M_LIST_HISTO,
    M_RADIO_HI,
    M_RADIO_LO,
    M_RADIO_LH,
    M_BUTTON_RESET,
    M_VSId1
};

void MGDisplayAdc::AddFrameTop(TGHorizontalFrame *frame)
{
    //
    // left part of top frame
    //
    TGVerticalFrame *left = new TGVerticalFrame(frame, 80, 300, kFitWidth);
    fList->Add(left);

    fHistoList = new TGListBox (left, M_LIST_HISTO);
    fHistoList->Associate(this);
    fHistoList->Resize(100, 405);

    fList->Add(fHistoList);

    TGLayoutHints *laylist = new TGLayoutHints(kLHintsNormal, 10, 10, 10, 10);
    fList->Add(laylist);

    left->AddFrame(fHistoList, laylist);

    //
    //    middle part of top frame
    //
    TGVerticalFrame *mid = new TGVerticalFrame(frame, 80, 20, kFitWidth);
    fList->Add(mid);

    // ---

    TGTextButton *prev = new TGTextButton(mid, "Prev Histo", M_BUTTON_PREV);
    TGTextButton *next = new TGTextButton(mid, "Next Histo", M_BUTTON_NEXT);
    prev->Associate(this);
    next->Associate(this);

    fList->Add(prev);
    fList->Add(next);

    // ---

    fSlider = new TGVSlider(mid, 250, kSlider1|kScaleBoth, M_VSId1);
    fSlider->Associate(this);

    fList->Add(fSlider);

    // ---

    TGVButtonGroup *group = new TGVButtonGroup(mid);
    fList->Add(group);

    TGRadioButton *radio1 = new TGRadioButton(group, "&High Gain",     M_RADIO_HI);
    TGRadioButton *radio2 = new TGRadioButton(group, "&Low Gain",      M_RADIO_LO);
    TGRadioButton *radio3 = new TGRadioButton(group, "H&igh/Low Gain", M_RADIO_LH);

    /* FIXME:

    ~TGRadioButton calls TGRadioButton::TGFrame::GetMainFrame
    which calles fParent->GetFrame()

    fList->Add(radio1);
    fList->Add(radio2);
    fList->Add(radio3);
    */

    radio3->SetState(kButtonDown);

    radio1->Associate(this);
    radio2->Associate(this);
    radio3->Associate(this);

    // ---

    TGLayoutHints *laybut1 = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10,  0, 10);
    TGLayoutHints *laybut2 = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10, 10,  5);
    TGLayoutHints *layslid = new TGLayoutHints(kLHintsCenterX|kLHintsTop);

    fList->Add(laybut1);
    fList->Add(laybut2);
    fList->Add(layslid);

    mid->AddFrame(prev,    laybut1);
    mid->AddFrame(fSlider, layslid);
    mid->AddFrame(next,    laybut2);
    mid->AddFrame(group,   laybut2);

    //
    //    right part of top frame
    //
    TGVerticalFrame *right = new TGVerticalFrame(frame, 100, 100, kFitWidth);
    fList->Add(right);

    TRootEmbeddedCanvas *canvas = new TRootEmbeddedCanvas("fECanv", right, 100, 100);
    fList->Add(canvas);

    TGLayoutHints *laycanv = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY|kLHintsExpandX|kLHintsExpandY, 10, 10, 10, 10);
    fList->Add(laycanv);

    right->AddFrame(canvas, laycanv);


    TGTextButton *reset = new TGTextButton(right, "Reset histo", M_BUTTON_RESET);
    reset->Associate(this);
    fList->Add(reset);

    TGLayoutHints *layreset = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10, 0, 10);
    fList->Add(layreset);

    right->AddFrame(reset, layreset);

    // ---

    fCanvas = canvas->GetCanvas();

    //
    // layout the three subframes
    //
    TGLayoutHints *layframe1 = new TGLayoutHints(kLHintsTop, 10, 10, 10, 10);
    TGLayoutHints *layframe2 = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY|kLHintsExpandX|kLHintsExpandY, 10, 10, 10, 10);
    fList->Add(layframe1);
    fList->Add(layframe2);

    frame->AddFrame(left,  layframe1);
    frame->AddFrame(mid,   layframe1);
    frame->AddFrame(right, layframe2);
}

void MGDisplayAdc::AddFrameLow(TGHorizontalFrame *frame)
{
    //
    //   the low frame for the control buttons
    //
    TGTextButton *but1 = new TGTextButton(frame, "Save",     M_BUTTON_SAVE);
    TGTextButton *but2 = new TGTextButton(frame, "Print",    M_BUTTON_PRINT);
    TGTextButton *but3 = new TGTextButton(frame, "PrintAll", M_BUTTON_PRINTALL);
    TGTextButton *but4 = new TGTextButton(frame, "Close",    M_BUTTON_CLOSE);

    but1->Associate(this);
    but2->Associate(this);
    but3->Associate(this);
    but4->Associate(this);

    fList->Add(but1);
    fList->Add(but2);
    fList->Add(but3);
    fList->Add(but4);

    TGLayoutHints *laybut = new TGLayoutHints(kLHintsNormal, 10, 10, 5, 5);
    fList->Add(laybut);

    frame->AddFrame(but1, laybut);
    frame->AddFrame(but2, laybut);
    frame->AddFrame(but3, laybut);
    frame->AddFrame(but4, laybut);
}

MGDisplayAdc::MGDisplayAdc(MHFadcCam *histos,
                           const TGWindow *p, const TGWindow *main,
                           UInt_t w, UInt_t h,
                           UInt_t options)
: TGTransientFrame(p?p:gClient->GetRoot(), main?main:gClient->GetRoot(), w, h, options),
  fHistoType(M_RADIO_LH)
{
    fHists = (MHFadcCam*)histos->Clone();

    fList = new TList;
    fList->SetOwner();

    //
    // Create the to frames
    //
    TGHorizontalFrame *frametop = new TGHorizontalFrame(this, 60, 20, kFitWidth);
    TGHorizontalFrame *framelow = new TGHorizontalFrame(this, 60, 20, kFixedWidth);

    //
    // Add frames to 'autodel'
    //
    fList->Add(frametop);
    fList->Add(framelow);

    //
    // Add the gui elements to the two frames
    //
    AddFrameTop(frametop);
    AddFrameLow(framelow);

    //
    // layout the two frames in this frame
    //
    TGLayoutHints *laytop = new TGLayoutHints(kLHintsTop|kLHintsExpandX,    10, 10, 10, 10);
    TGLayoutHints *laylow = new TGLayoutHints(kLHintsBottom|kLHintsExpandX, 10, 10, 10, 10);

    AddFrame(frametop, laytop);
    AddFrame(framelow, laylow);

    //
    // Setup interieur
    //
    BuildHistoList();

    //
    // Here the initial display is set to entry with id 1
    //
    fHistoList->Select(1);
    UpdateHist();

    //
    // Setup frame
    //
    MapSubwindows();

    Layout();

    SetWindowName("ADC Spectra");
    SetIconName("ADC Spectra");

    MapWindow();
    SetWMSizeHints(950, 500, 1000, 1000, 1, 1);
}  

MGDisplayAdc::~MGDisplayAdc()
{ 
    delete fList;
    delete fHists;
}  

void MGDisplayAdc::CloseWindow()
{
    //
    // The close message is generated by the window manager when its close
    // window menu item is selected.
    //
    delete this;
}

Bool_t MGDisplayAdc::BuildHistoList()
{
    //
    //   looks in the container of the AdcSpectra and reads in the
    //   Histogramms in there.
    //
    //   In the class MHFadcCam are in fact two lists. One for the high and
    //   one for the low gain. Here we will use only the high gain list!!!
    //   With some special options (settings in the gui) we will also be able
    //   to plot the low gain
    //
    const Int_t nhi = fHists->GetEntries();

    Int_t n=1;
    for (Int_t i=0; i<nhi; i++)
        if (fHists->HasHi(i))
            fHistoList->AddEntry(fHists->GetHistHi(i)->GetName(), n++);

    fSlider->SetRange(1, n);

    fHistoList->MapSubwindows();
    fHistoList->Layout();

    return kTRUE;
} 

void MGDisplayAdc::UpdateHist()
{
    const Int_t selected = fHistoList->GetSelected();

    if (selected<0)
    {
        cout << "MGDisplayAdc: No histograms found in list. " << endl;
        return;
    }

    fHistoList->Select(selected); // ???

    fCanvas->Clear();

    const Int_t idx = fHistoList->GetSelectedEntry()->EntryId()-1; //selected-1;

    if (!fHists->HasHi(idx))
        return;

    const Int_t type = fHists->HasLo(idx) ? fHistoType : M_RADIO_HI;

    switch (type)
    {
    case M_RADIO_HI:
    case M_RADIO_LO:
        fCanvas->Divide(1, 1);

        fCanvas->cd();
        if (type==M_RADIO_HI)
            fHists->DrawHi(idx);
        else
            fHists->DrawLo(idx);
        break;

    case M_RADIO_LH:
        fCanvas->Divide(1, 2);

        fCanvas->cd(1);
        fHists->DrawHi(idx);

        fCanvas->cd(2);
        fHists->DrawLo(idx);
        break;
    }

    fHistoList->SetTopEntry(selected);

    fCanvas->Modified();
    fCanvas->Update();

    fSlider->SetPosition(selected);
}

Bool_t MGDisplayAdc::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{     
    //
    // Process events generated by the buttons in the frame.
    //
    switch (GET_MSG(msg))
    {
    case kC_COMMAND:
        switch (GET_SUBMSG(msg))
        {
        case kCM_BUTTON:
            switch (parm1)
            {
            case M_BUTTON_SAVE:
                fCanvas->SaveAs();
                cout << "Sorry, no well implemented!" << endl;
                return kTRUE;

            case M_BUTTON_PRINT:
                {
                    char *txt = (char*)"canvasXXXXXX";
                    mkstemp(txt);
                    TString name = txt;
                    name += ".ps";
                    fCanvas->SaveAs(name);
                    cout << "Calling 'lpr " << name << ".ps'" << endl;
                    gSystem->Exec(TString("lpr ")+name+".ps");
                    gSystem->Exec(TString("rm ")+name+".ps");
                }
                cout << "Sorry, not well implemented!" << endl;
                return kTRUE;

            case M_BUTTON_RESET:
                cout << "Sorry, not yet implemented!" << endl;
                return kTRUE;

            case M_BUTTON_CLOSE:
                CloseWindow();
                return kTRUE;

            case M_BUTTON_PREV:
            case M_BUTTON_NEXT:
                {
                    const Int_t selected = fHistoList->GetSelected();

                    if ((parm1==M_BUTTON_PREV && selected==1) ||
                        (parm1==M_BUTTON_NEXT && selected==fHistoList->GetNumberOfEntries()))
                        return kTRUE;

                    fHistoList->Select(parm1==M_BUTTON_PREV ? selected-1 : selected+1);
                    UpdateHist();
                }
                return kTRUE;
            }
            return kTRUE;

        case kCM_RADIOBUTTON:
            switch(parm1)
            {
            case M_RADIO_HI:
            case M_RADIO_LO:
            case M_RADIO_LH:
                fHistoType = parm1;
                UpdateHist();
                return kTRUE;
            }
            return kTRUE;
        }
		
    case kCM_LISTBOX:
        if (GET_SUBMSG(msg) == M_LIST_HISTO)
            UpdateHist();

        return kTRUE;

    case kC_VSLIDER:
        if (GET_SUBMSG(msg)!=kSL_POS || parm1!=M_VSId1)
            return kTRUE;

        // Check for the slider movement and synchronise with TGListBox
        if (parm2<1 || parm2>fHistoList->GetNumberOfEntries())
            return kTRUE;

        fHistoList->Select(parm2);
        UpdateHist();
        return kTRUE;
    }

    return kTRUE;
}
