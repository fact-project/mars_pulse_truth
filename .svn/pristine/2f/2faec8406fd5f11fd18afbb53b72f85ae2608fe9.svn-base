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

#include "MBrowser.h"

#include <iostream>
#include <sys/stat.h>       // S_ISDIR

#include <TSystem.h>        // gSystem

#include <TGTab.h>          // TGTab
#include <TGMenu.h>         // TGPopupMenu
#include <TG3DLine.h>       // TGHorizontal3DLine
#include <TGButton.h>       // TGTextButton
#include <TGMsgBox.h>       // TGMsgBox
#include <TBrowser.h>       // TBrowser
#include <TGTextEntry.h>    // TGTextEntry
#include <TGFSComboBox.h>   // TGFSComboBox, TGFSLboxEntry
#include <TGProgressBar.h>  // TGHProgressBar
#include <TGFSContainer.h>  // TGFileContainer

#include "MGList.h"

ClassImp(MBrowser);

using namespace std;

enum {
    kFileTBrowser,
    kFileClose,
    kButDirUp,
    kButListMode,
    kButDetailMode,
    kCBDirectory,
    kCBFilter,
    kTEFileName
};

void MBrowser::CreateMenuBar()
{
    //
    //  crate the menu bar
    //
    TGPopupMenu *filemenu = new TGPopupMenu(gClient->GetRoot());
    filemenu->AddEntry("Open &TBrowser", kFileTBrowser);
    filemenu->AddSeparator();
    filemenu->AddEntry("&Close", kFileClose);
    filemenu->Associate(this);
    fList->Add(filemenu);

    //
    //  the button messages are handled by main frame (this)
    //
    TGLayoutHints *laymenubar  = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX, 2, 2, 2, 2);
    TGLayoutHints *laymenuitem = new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0);

    fList->Add(laymenubar);
    fList->Add(laymenuitem);

    TGMenuBar *menubar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
    menubar->AddPopup("&File", filemenu, laymenuitem);
    AddFrame(menubar, laymenubar);
    fList->Add(menubar);
}

void MBrowser::CreateUpperFrame(TGCompositeFrame *frametop)
{
    TGLayoutHints *lay1 = new TGLayoutHints(kLHintsTop    |kLHintsExpandX, 2, 2, 2, 0);
    TGLayoutHints *lay2 = new TGLayoutHints(kLHintsCenterY|kLHintsExpandX, 2, 2, 2, 2);
    TGLayoutHints *lay3 = new TGLayoutHints(kLHintsBottom |kLHintsExpandX, 2, 2, 0, 2);
    fList->Add(lay1);
    fList->Add(lay2);
    fList->Add(lay3);

    //
    // *********** Create Contents of frame top (upper part) **********
    //
    fTop1 = new TGHorizontalFrame(frametop, 500, 50);
    fTop2 = new TGHorizontalFrame(frametop, 500, 50);
    fTop3 = new TGHorizontalFrame(frametop, 500, 50);

    // FIXME: If I use TGLayoutHints the Progress Bar doesn't disappear!
    frametop->AddFrame(fTop1);//, lay1);
    frametop->AddFrame(fTop2);//, lay2);
    frametop->AddFrame(fTop3);//, lay3);

    fList->Add(fTop1);
    fList->Add(fTop2);
    fList->Add(fTop3);
}

void MBrowser::CreateDirListMenu(TGCompositeFrame *frame)
{
    //
    // Layout Dir-Listbox and buttons in one row (frame)
    //
    //  - layout:
    //    alignment: top, left
    //    padding:   5, 5, 5, 5
    //
    TGLayoutHints *laydir = new TGLayoutHints(kLHintsExpandX|kLHintsLeft|kLHintsCenterY); //, 5, 5, 5, 5);
    TGLayoutHints *layout = new TGLayoutHints(kLHintsRight|kLHintsCenterY, 10); //, 5, 5, 5);

    fList->Add(laydir);
    fList->Add(layout);

    //
    // Create Dir-Listbox and buttons in first frame
    //
    TGFSComboBox *dir = new TGFSComboBox(frame, kCBDirectory);
    dir->SetHeight(fEntry->GetHeight());
    dir->Associate(this);
    fList->Add(dir);
    frame->AddFrame(dir, laydir);

    const TGPicture *pic0 = fList->GetPicture("magic_t.xpm");
    if (pic0)
    {
        dir->GetListBox()->Resize(100, 150);
        TGTreeLBEntry *entry = new TGTreeLBEntry(dir->GetListBox()->GetContainer(),
                                                 new TGString("/magic/data"), pic0, 6000,
                                                 new TGString("/magic/data"));
        TGLayoutHints *laylb = new TGLayoutHints(kLHintsLeft|kLHintsTop, 14, 0, 0, 0);
        dir->AddEntry(entry, laylb);
        // Not necessary - deleted in ~TGLBContainer: fList->Add(laylb);
        // fList->Add(laylb);
    }

    //
    // Get the three picturs from the system (must be deleted by FreePicture)
    //
    const TGPicture *pic1 = fList->GetPicture("tb_list.xpm");
    if (pic1)
    {
        TGPictureButton *list = new TGPictureButton(frame, pic1, kButListMode);
        list->SetToolTipText("List Mode");
        list->SetState(kButtonUp);
        list->AllowStayDown(kTRUE);
        list->Associate(this);
        fList->Add(list);
        frame->AddFrame(list, layout);
    }

    const TGPicture *pic2 = fList->GetPicture("tb_details.xpm");
    if (pic2)
    {
        TGPictureButton *detail = new TGPictureButton(frame, pic2, kButDetailMode);
        detail->SetToolTipText("Details Mode");
        detail->SetState(kButtonEngaged);
        detail->AllowStayDown(kTRUE);
        detail->Associate(this);
        fList->Add(detail);
        frame->AddFrame(detail, layout);
    }

    const TGPicture *pic3 = fList->GetPicture("tb_uplevel.xpm");
    if (pic3)
    {
        TGPictureButton *cdup = new TGPictureButton(frame, pic3, kButDirUp);
        cdup->SetToolTipText("One Level up!");
        cdup->Associate(this);
        fList->Add(cdup);
        frame->AddFrame(cdup, layout);
    }
}

void MBrowser::CreateDirListBox(TGCompositeFrame *frame)
{
    //
    // Create file viewer (browser)
    //
    fFileView = new TGListView(frame, 540, 380);
    fList->Add(fFileView);

    TGViewPort *port = fFileView->GetViewPort();
    port->SetBackgroundColor(fgWhitePixel);

    fFileCont = new TGFileContainer(port, 100, 100, kVerticalFrame, fgWhitePixel);
    fList->Add(fFileCont);

    fFileView->SetContainer(fFileCont);
    fFileView->SetViewMode(kLVDetails);

    fFileCont->SetFilter("*.root");
    fFileCont->Associate(this);
    fFileCont->Sort(kSortByName);

    TGLayoutHints *layview = new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY); //, 5, 5, 5, 5);
    fList->Add(layview);

    frame->AddFrame(fFileView, layview);
}

void MBrowser::CreateTab1()
{
    static const char *filters[] =
    {
        "*.root <root-files>",
        "*      <All Files>",
        NULL
    };

    TGCompositeFrame *frame = CreateNewTab("Input File");

    //
    // Create three frames for the first tab
    //
    TGHorizontalFrame *tab1 = new TGHorizontalFrame(frame, 100, 100);
    TGVerticalFrame   *tab2 = new TGVerticalFrame  (frame, 100, 100);

    TGLayoutHints *laytab1   = new TGLayoutHints(kLHintsNormal|kLHintsExpandX,  10, 10, 10);
    TGLayoutHints *laytab2   = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10, 10, 10);
    TGLayoutHints *layfilter = new TGLayoutHints(kLHintsNormal|kLHintsExpandX,  10, 10, 10);

    fEntry = new TGTextEntry(frame, "", kTEFileName);
    fEntry->Associate(this);

    TGComboBox *filter = new TGComboBox(frame, kCBFilter);
    filter->SetHeight(fEntry->GetHeight());
    filter->Associate(this);
    for (int i=0; filters[i]; i++)
        filter->AddEntry(filters[i], i);
    filter->Select(0);

    frame->AddFrame(fEntry, laytab1);
    frame->AddFrame(tab1,   laytab1);
    frame->AddFrame(filter, layfilter);
    frame->AddFrame(tab2,   laytab2);

    CreateDirListMenu(tab1);
    CreateDirListBox(tab2);

    fList->Add(laytab1);
    fList->Add(laytab2);
    fList->Add(tab1);
    fList->Add(tab2);
    fList->Add(layfilter);
    fList->Add(fEntry);
    fList->Add(filter);
}

TGCompositeFrame *MBrowser::CreateNewTab(const char *name)
{
    return fTabs->AddTab(name);
}

void MBrowser::CreateLowerFrame(TGCompositeFrame *framelow)
{
    //
    // *********** Create Contents of frame low (downer part) **********
    //

    //
    // ----- Create Object holding the Tabs -----
    //
    fTabs = new TGTab(framelow, 400, 400);
    fList->Add(fTabs);

    TGLayoutHints *laytabs = new TGLayoutHints(kLHintsBottom|kLHintsExpandX|kLHintsExpandY, 5, 5, 5, 5);
    fList->Add(laytabs);

    framelow->AddFrame(fTabs, laytabs);

    //
    // --- Create the first tab of the tabs ---
    //
    CreateTab1();
}

MBrowser::MBrowser(/*const TGWindow *main,*/ const TGWindow *p,
                   const UInt_t w, const UInt_t h)
//: TGTransientFrame(p?p:gClient->GetRoot(),
//                   main?main:gClient->GetRoot(), w, h)
: TGMainFrame(p?p:gClient->GetRoot(), w, h)
{
    fList = new MGList;
    fList->SetOwner();

    CreateMenuBar();

    TGLayoutHints *laylinesep  = new TGLayoutHints(kLHintsTop|kLHintsExpandX);
    fList->Add(laylinesep);

    TGHorizontal3DLine *linesep = new TGHorizontal3DLine(this);
    fList->Add(linesep);
    AddFrame(linesep, laylinesep);

    //
    // ---- Create the top window with a lot of buttons ----
    //
    TGLayoutHints *layframetop = new TGLayoutHints(kLHintsExpandX);
    fList->Add(layframetop);

    TGCompositeFrame *frametop = new TGCompositeFrame(this, 300, 100, kVerticalFrame);
    fList->Add(frametop);
    AddFrame(frametop, layframetop);

    linesep = new TGHorizontal3DLine(this);
    fList->Add(linesep);
    AddFrame(linesep, laylinesep);

    //
    // ---- Create the low window with a tabs in it ----
    //
    TGLayoutHints *layframelow = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY);
    fList->Add(layframelow);

    TGCompositeFrame *framelow = new TGCompositeFrame(this, 300, 100, kHorizontalFrame);
    fList->Add(framelow);

    AddFrame(framelow, layframelow);

    CreateUpperFrame(frametop);
    CreateLowerFrame(framelow);

    //
    //   Map the window, set up the layout, etc.
    //
    ChangeDir();
    SetWMSizeHints(400, 350, 1000, 1000, 10, 10); // set the smallest and biggest size of the Main frame
    Move(rand()%100+50, rand()%100+50);
}


MBrowser::~MBrowser()
{
    delete fList;
} 

TGProgressBar *MBrowser::CreateProgressBar(TGHorizontalFrame *frame)
{
    static TGLayoutHints laybar(kLHintsCenterY|kLHintsRight/*|kLHintsExpandX*/,
                                10, 10);

    TGHProgressBar *bar=new TGHProgressBar(frame);

    bar->SetWidth(150);
    bar->ShowPosition();

    frame->AddFrame(bar, &laybar);

    Layout();
    MapSubwindows();

    return bar;
}

void MBrowser::DestroyProgressBar(TGProgressBar *bar)
{
    TGHorizontalFrame *frame = (TGHorizontalFrame*)bar->GetParent();

    frame->RemoveFrame(bar);

    Layout();
    MapSubwindows();

    delete bar;
}

void MBrowser::CloseWindow()
{
   // Got close message for this MainFrame. Calls parent CloseWindow()
   // (which destroys the window) and terminate the application.
   // The close message is generated by the window manager when its close
   // window menu item is selected.

    delete this;
}

Bool_t MBrowser::InputFileSelected() 
{
    //
    // Checks if there is a selected input root file
    //
    return !fInputFile.IsNull(); //[0]!='\0';
}

void MBrowser::DisplError(const char *txt)
{
    Int_t retval;
    new TGMsgBox(fClient->GetRoot(), this, "Error!", txt,
                 kMBIconExclamation, 4, &retval);
}

void MBrowser::DisplWarning(const char *txt)
{
    Int_t retval;
    new TGMsgBox(fClient->GetRoot(), this, "Warning!", txt,
                 kMBIconExclamation, 4, &retval);
}

void MBrowser::DisplInfo(const char *txt)
{
    Int_t retval;
    new TGMsgBox(fClient->GetRoot(), this, "Information!", txt,
                 kMBIconExclamation, 4, &retval);
}

void MBrowser::ChangeDir(const char *txt)
{
    fFileCont->ChangeDirectory(txt?txt:gSystem->WorkingDirectory());

    const char *dir = fFileCont->GetDirectory();

    TGFSComboBox *cbox = (TGFSComboBox*)fList->FindWidget(kCBDirectory);
    if (cbox)
        cbox->Update(dir);
}

void MBrowser::SetFileName(TString name)
{
    if (name[0]!='/')
    {
        name.Insert(0, "/");
        name.Insert(0, fFileCont->GetDirectory());
    }

    if (gSystem->AccessPathName(name, kFileExists))
        return;

    if (name.EndsWith(".root", TString::kIgnoreCase))
        fInputFile = name;

    fEntry->SetText(gSystem->BaseName(name));

    ChangeDir(gSystem->DirName(name));
}

void MBrowser::SetDir()
{
    TGFSComboBox *cbox = (TGFSComboBox*)fList->FindWidget(kCBDirectory);
    if(!cbox)
        return;

    const TGTreeLBEntry *entry = (TGTreeLBEntry*)cbox->GetSelectedEntry();

    ChangeDir(entry->GetPath()->GetString());
}

void MBrowser::SetFilter()
{
    //
    // Try to get widget from list
    //
    TGComboBox *filter = (TGComboBox*)fList->FindWidget(kCBFilter);
    if (!filter)
        return;

    //
    // Get the selected text from the list box
    //
    const TGTextLBEntry *selected = (TGTextLBEntry*)filter->GetListBox()->GetSelectedEntry();

    //
    // find filter and remove leading spaces
    //
    TString txt = selected->GetText()->GetString();
    if (txt.First('<') < 0)
        return;

    //
    // Set new filter and refresh the file container
    //
    txt.Remove(txt.First('<'));

    TString ftxt = txt.Strip();
    fFileCont->SetFilter(ftxt);
    fFileCont->DisplayDirectory();
}

void MBrowser::SetViewMode(const Int_t mode)
{
    fFileView->SetViewMode(mode ? kLVList : kLVDetails);

    TGButton *but = (TGButton*)fList->FindWidget(mode);
    if(!but)
        return;

    but->SetState(kButtonUp);
}

// --------------------------------------------------------------------------
//
// Process events generated by the gui elements in the frame.
//
Bool_t MBrowser::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
    switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
        if (GET_SUBMSG(msg)!=kTE_ENTER)
            return kTRUE;

        SetFileName(fEntry->GetText());
        return kTRUE;

    case kC_COMMAND:
        switch (GET_SUBMSG(msg))
        {
        case kCM_MENU:
            switch (parm1)
            {
            case kFileClose:
                CloseWindow();
                return kTRUE;

            case kFileTBrowser:
                new TBrowser();
                return kTRUE;
            }
            return kTRUE;

        case kCM_BUTTON:
            switch (parm1)
            {
            case kButDirUp:
                //
                //  goto the parent directory
                //
                ChangeDir("..");
                return kTRUE;

            case kButListMode:
            case kButDetailMode:
                SetViewMode(parm1);
                return kTRUE;
            }
            return kTRUE;

        case kCM_COMBOBOX:
            switch (parm1)
            {
            case kCBDirectory:
                SetDir();
                return kTRUE;
            case kCBFilter:
                SetFilter();
                return kTRUE;
            }
            return kTRUE;
        }
        return kTRUE;

    case kC_CONTAINER:
        switch (GET_SUBMSG(msg))
        {

            //      case kCT_ITEMCLICK:
            // 	printf ("itemclick\n");
            //      break;

        case kCT_ITEMDBLCLICK:
            //
            //  process the double click in the file view container
            //
            if (parm1 != kButton1 || fFileCont->NumSelected() != 1)
                return kTRUE;

            //
            //  one file selected
            //
            void *dummy = NULL;
            const TGFileItem *item = (TGFileItem *)fFileCont->GetNextSelected(&dummy);

            const char *str = item->GetItemName()->GetString();

            //
            // if the user choose a directory
            // change to this directory
            //
            if (S_ISDIR(item->GetType()))
            {
                ChangeDir(str);
                return kTRUE;
            }

            SetFileName(str);
            return kTRUE;
        }
    }
    return kTRUE;
}
