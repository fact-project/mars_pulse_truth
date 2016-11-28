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
!   Author(s): Thomas Bretz, 4/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2003-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MStatusDisplay
//
// This status display can be used (and is used) to display results in
// a tabbed window. The window can be written to and read from a root file
// (see Read and Write) or printed as a postscript file (see SaveAsPS).
//
// To write gif files of C-Macros use SaveAsGif()/SaveAsPNG() or SaveAsC().
// Direct printing to the default printer (via lpr) can be done by
// PrintPS().
//
// It has also to half status lines which can be used to display the status
// or something going on. Together with the status lines it has a progress
// bar which can display the progress of a job or loop.
// Access the progress bar by GetProgressBar()
//
// To add a new tab and get a pointer to the newly created TCanvas
// use AddTab.
//
// If you have a MStatusDisplay and you are not sure whether it was
// destroyed by the user meanwhile use:
//   gROOT->GetListOfSpecials()->FindObject(pointer);
// Each MStatusDisplay is added to list list by its constructor and
// removed from the list by the destructor.
//
// You can redirect an output to a MLog-logstream by calling SetLogStream().
// To disable redirction call SetLogStream(NULL)
//
// Because updates to the tabs are only done/displayed if a tab is active
// using the gui doesn't make things slower (<1%) if the first (legend
// tab) is displayed. This gives you the possibility to look into
// the current progress of a loop without loosing more time than the
// single update of the tab.
//
/////////////////////////////////////////////////////////////////////////////
#include "MStatusDisplay.h"

#include <errno.h>
#include <stdlib.h>               // mktemp (Ubunto 8.10)

#include <fstream>                // fstream

#include <TH1.h>                  // TH1::AddDirectory
#include <TPDF.h>                 // TPDF
#include <TSVG.h>                 // TSVG
#include <TEnv.h>                 // TEnv
#include <TLine.h>                // TLine
#include <TMath.h>
#include <TText.h>                // TText
#include <TFile.h>                // gFile
#include <TFrame.h>               // TFrame
#include <TStyle.h>               // gStyle
#include <TCanvas.h>              // TCanvas
#include <TSystem.h>              // gSystem
#include <TDatime.h>              // TDatime
#include <TRandom.h>              // TRandom
#include <TRegexp.h>              // TRegexp
#include <TThread.h>              // TThread::Self()
#include <TBrowser.h>             // TBrowser
#include <TObjArray.h>            // TObjArray
#include <TPostScript.h>          // TPostScript
#include <TMethodCall.h>          // TMethodCall

#include <TInterpreter.h>         // gInterpreter

#include <TGTab.h>                // TGTab
#include <TGLabel.h>              // TGLabel
#include <TG3DLine.h>             // TGHorizontal3DLine
#include <TGButton.h>             // TGPictureButton
#include <TGTextView.h>           // TGTextView
#include <TGComboBox.h>           // TGComboBox
#include <TGStatusBar.h>          // TGStatusBar
#include <TGFileDialog.h>         // TGFileDialog
#include <TGProgressBar.h>        // TGHProgressBar
#include <TGTextEditDialogs.h>    // TGPrintDialog
#include <TRootEmbeddedCanvas.h>  // TRootEmbeddedCanvas

#include "MString.h"

#include "MLog.h"                 // MLog
#include "MLogManip.h"            // inf, warn, err

#include "MGList.h"               // MGList
#include "MGMenu.h"               // MGMenu, TGMenu
#include "MSearch.h"              // MSearch
#include "MParContainer.h"        // MParContainer::GetDescriptor
#include "MStatusArray.h"         // MStatusArray

#if ROOT_VERSION_CODE <= ROOT_VERSION(5,22,00)
#include "../mhbase/MH.h"
#endif

#undef DEBUG
//#define DEBUG

ClassImp(MStatusDisplay);

using namespace std;

// ------------ Workaround for a non working TGTextView::Search -------------
#if ROOT_VERSION_CODE < ROOT_VERSION(3,02,05)
class MGTextView : public TGTextView
{
public:
    MGTextView(const TGWindow *parent, UInt_t w, UInt_t h, Int_t id = -1,
               UInt_t sboptions = 0, ULong_t back = GetWhitePixel()) :
    TGTextView(parent, w, h, id, sboptions, back) {}
    MGTextView(const TGWindow *parent, UInt_t w, UInt_t h, TGText *text,
               Int_t id = -1, UInt_t sboptions = 0, ULong_t back = GetWhitePixel()) :
    TGTextView(parent, w, h, text, id, sboptions, back) {}
    MGTextView(const TGWindow *parent, UInt_t w, UInt_t h, const char *string,
               Int_t id = -1, UInt_t sboptions = 0, ULong_t back = GetWhitePixel()) :
    TGTextView(parent, w, h, string, id, sboptions, back) {}

    void Mark(Long_t xPos, Long_t yPos) { TGTextView::Mark(xPos, yPos); }
    void UnMark()                       { TGTextView::UnMark(); }

    Bool_t Search(const char *string, Bool_t direction, Bool_t caseSensitive)
    {
        // Taken from TGTextView::Search and modified.

        TGLongPosition pos, pos2;
        pos2.fX = pos2.fY = 0;
        if (fIsMarked) {
            if (!direction)
            {
                pos2.fX = fMarkedStart.fX;
                pos2.fY = fMarkedStart.fY;
            }
            else
            {
                pos2.fX = fMarkedEnd.fX + 1;
                pos2.fY = fMarkedEnd.fY;
            }
        }
        if (!fText->Search(&pos, pos2, string, direction, caseSensitive))
            return kFALSE;
        UnMark();
        fIsMarked = kTRUE;
        fMarkedStart.fY = fMarkedEnd.fY = pos.fY;
        fMarkedStart.fX = pos.fX;
        fMarkedEnd.fX = fMarkedStart.fX + strlen(string);
        pos.fY = ToObjYCoord(fVisible.fY);
        if ((fMarkedStart.fY < pos.fY) ||
            (ToScrYCoord(fMarkedStart.fY) >= (Int_t)fCanvas->GetHeight()))
            pos.fY = fMarkedStart.fY;
        pos.fX = ToObjXCoord(fVisible.fX, pos.fY);
        if ((fMarkedStart.fX < pos.fX) ||
            (ToScrXCoord(fMarkedStart.fX, pos.fY) >= (Int_t)fCanvas->GetWidth()))
            pos.fX = fMarkedStart.fX;

        SetVsbPosition((ToScrYCoord(pos.fY) + fVisible.fY)/fScrollVal.fY);
        SetHsbPosition((ToScrXCoord(pos.fX, pos.fY) + fVisible.fX)/fScrollVal.fX);
        DrawRegion(0, (Int_t)ToScrYCoord(fMarkedStart.fY), fCanvas->GetWidth(),
                   UInt_t(ToScrYCoord(fMarkedEnd.fY+1) - ToScrYCoord(fMarkedEnd.fY)));

        return kTRUE;
    }
};
#else
#define MGTextView TGTextView
#endif

// --------------------------------------------------------------------------

TGCompositeFrame *MStatusDisplay::GetTabContainer(const char *name) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,03,05)
    if (!fTab)
        return 0;

   TGFrameElement *el;
   TGTabElement *tab = 0;
   TGCompositeFrame *comp = 0;

   TIter next(fTab->GetList());
   next();           // skip first container

   while ((el = (TGFrameElement *) next())) {
      el = (TGFrameElement *) next();
      comp = (TGCompositeFrame *) el->fFrame;
      next();
      tab = (TGTabElement *)el->fFrame;
      if (name == tab->GetText()->GetString()) {
         return comp;
      }
   }

   return 0;
#else
   return fTab ? fTab->GetTabContainer(name) : 0;
#endif
}

TGTabElement *MStatusDisplay::GetTabTab(const char *name) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,03,05)
    if (!fTab)
        return 0;

   TGFrameElement *el;
   TGTabElement *tab = 0;

   TIter next(fTab->GetList());
   next();           // skip first container

   while ((el = (TGFrameElement *) next())) {
      next();
      tab = (TGTabElement *)el->fFrame;
      if (name == tab->GetText()->GetString()) {
         return tab;
      }
   }

   return 0;
#else
   return fTab ? fTab->GetTabTab(name) : 0;
#endif
}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//
// Add menu bar to the GUI
//
void MStatusDisplay::AddMenuBar()
{
    //
    // File Menu
    //
    MGPopupMenu *filemenu = new MGPopupMenu(gClient->GetDefaultRoot());
    filemenu->AddEntry("New &Canvas",       kFileCanvas);
    filemenu->AddEntry("New &Browser",      kFileBrowser);
    filemenu->AddEntry("New &Tab",          kFileTab);
    filemenu->AddSeparator();

    const TString fname(MString::Format("Save %s.", gROOT->GetName()));
    MGPopupMenu *savemenu = new MGPopupMenu(gClient->GetDefaultRoot());
    savemenu->AddEntry(MString::Format("%s&ps",  fname.Data()),  kFileSaveAsPS);
    savemenu->AddEntry(MString::Format("%sp&df", fname.Data()),  kFileSaveAsPDF);
    savemenu->AddEntry(MString::Format("%s&svg", fname.Data()),  kFileSaveAsSVG);
    savemenu->AddSeparator();
    savemenu->AddEntry(MString::Format("%sp&ng", fname.Data()),  kFileSaveAsPNG);
    savemenu->AddEntry(MString::Format("%s&gif", fname.Data()),  kFileSaveAsGIF);
    savemenu->AddEntry(MString::Format("%s&jpg", fname.Data()),  kFileSaveAsJPG);
    savemenu->AddEntry(MString::Format("%s&xpm", fname.Data()),  kFileSaveAsXPM);
    savemenu->AddEntry(MString::Format("%s&tiff",fname.Data()),  kFileSaveAsTIFF);
    savemenu->AddEntry(MString::Format("%s&bmp", fname.Data()),  kFileSaveAsBMP);
    savemenu->AddEntry(MString::Format("%sx&ml", fname.Data()),  kFileSaveAsXML);
    savemenu->AddEntry(MString::Format("%scs&v", fname.Data()),  kFileSaveAsCSV);
    savemenu->AddSeparator();
    savemenu->AddEntry(MString::Format("%s&C",    fname.Data()), kFileSaveAsC);
    savemenu->AddEntry(MString::Format("%s&root", fname.Data()), kFileSaveAsRoot);
    savemenu->AddEntry(MString::Format("%s&root (plain)", fname.Data()), kFileSaveAsPlainRoot);
    savemenu->Associate(this);

    filemenu->AddEntry("&Open...",          kFileOpen);
    filemenu->AddPopup("&Save", savemenu);
    filemenu->AddEntry("Save &As...",       kFileSaveAs);
    filemenu->AddSeparator();
    filemenu->AddEntry("&Reset",            kFileReset);
    filemenu->AddSeparator();
    filemenu->AddEntry("&Print",            kFilePrint);
    filemenu->AddSeparator();
    filemenu->AddEntry("C&lose",            kFileClose);
    filemenu->AddEntry("E&xit",             kFileExit);
    filemenu->Associate(this);

    //
    // Tab Menu
    //
    MGPopupMenu *tabmenu = new MGPopupMenu(gClient->GetDefaultRoot());
    tabmenu->AddEntry("Next [&+]",          kTabNext);
    tabmenu->AddEntry("Previous [&-]",      kTabPrevious);
    tabmenu->AddSeparator();

    const TString fname2(MString::Format("Save %s-i.", gROOT->GetName()));
    MGPopupMenu *savemenu2 = new MGPopupMenu(gClient->GetDefaultRoot());
    savemenu2->AddEntry(MString::Format("%s&ps",  fname2.Data()),  kTabSaveAsPS);
    savemenu2->AddEntry(MString::Format("%sp&df", fname2.Data()),  kTabSaveAsPDF);
    savemenu2->AddEntry(MString::Format("%s&svg", fname2.Data()),  kTabSaveAsSVG);
    savemenu2->AddSeparator();
    savemenu2->AddEntry(MString::Format("%sp&ng", fname2.Data()),  kTabSaveAsPNG);
    savemenu2->AddEntry(MString::Format("%s&gif", fname2.Data()),  kTabSaveAsGIF);
    savemenu2->AddEntry(MString::Format("%s&jpg", fname2.Data()),  kTabSaveAsJPG);
    savemenu2->AddEntry(MString::Format("%s&xpm", fname2.Data()),  kTabSaveAsXPM);
    savemenu2->AddEntry(MString::Format("%s&tiff",fname2.Data()),  kTabSaveAsTIFF);
    savemenu2->AddEntry(MString::Format("%s&bmp", fname2.Data()),  kTabSaveAsBMP);
    savemenu2->AddEntry(MString::Format("%sx&ml", fname2.Data()),  kTabSaveAsXML);
    savemenu2->AddEntry(MString::Format("%scs&v", fname2.Data()),  kTabSaveAsCSV);
    savemenu2->AddSeparator();
    savemenu2->AddEntry(MString::Format("%s&C",    fname2.Data()), kTabSaveAsC);
    savemenu2->AddEntry(MString::Format("%s&root", fname2.Data()), kTabSaveAsRoot);
    savemenu2->Associate(this);

    tabmenu->AddPopup("&Save", savemenu2);
    tabmenu->AddEntry("Save tab &As...",    kTabSaveAs);
    tabmenu->AddSeparator();
    tabmenu->AddEntry("&Remove",            kTabRemove);
    tabmenu->AddSeparator();
    tabmenu->AddEntry("&Print",             kTabPrint);
    tabmenu->Associate(this);

    //
    // Loop Menu
    //
    MGPopupMenu *loopmenu = new MGPopupMenu(gClient->GetDefaultRoot());
    loopmenu->AddEntry("&Pause",       kLoopPause);
    loopmenu->AddEntry("Single S&tep", kLoopStep);
    loopmenu->AddSeparator();
    loopmenu->AddEntry("&Stop",  kLoopStop);
    loopmenu->Associate(this);

    loopmenu->DisableEntry(kLoopStep);

    //
    // Loop Menu
    //
    MGPopupMenu *sizemenu = new MGPopupMenu(gClient->GetDefaultRoot());
    sizemenu->AddEntry("Fit to 640x&480",   kSize640);
    sizemenu->AddEntry("Fit to 768x&576",   kSize768);
    sizemenu->AddEntry("Fit to 800x&600",   kSize800);
    sizemenu->AddEntry("Fit to 960x7&20",   kSize960);
    sizemenu->AddEntry("Fit to 1024x&768",  kSize1024);
    sizemenu->AddEntry("Fit to 1152x&864",  kSize1152);
    sizemenu->AddEntry("Fit to 1280x&1024", kSize1280);
    sizemenu->AddEntry("Fit to 1400x1050",  kSize1400);
    sizemenu->AddEntry("Fit to 1600x1200",  kSize1600);
    sizemenu->AddEntry("Fit to &Desktop",   kSizeOptimum);
    sizemenu->Associate(this);

    //
    // Log Menu
    //
    MGPopupMenu *logmenu = new MGPopupMenu(gClient->GetDefaultRoot());
    logmenu->AddEntry("&Copy Selected", kLogCopy);
    logmenu->AddEntry("Cl&ear all",     kLogClear);
    logmenu->AddSeparator();
    logmenu->AddEntry("Select &All",    kLogSelect);
    logmenu->AddSeparator();
    logmenu->AddEntry("&Find...",       kLogFind);
    logmenu->AddSeparator();
    logmenu->AddEntry("&Save",          kLogSave);
    logmenu->AddEntry("Save &append",   kLogAppend);
    logmenu->AddSeparator();
    logmenu->AddEntry("&Print",         kLogPrint);
    logmenu->Associate(this);

    //
    // Menu Bar
    //
    TGLayoutHints *layitem = new TGLayoutHints(kLHintsNormal, 0, 4, 0, 0);
    fList->Add(layitem);

    fMenuBar = new MGMenuBar(this, 1, 1, kHorizontalFrame);
    fMenuBar->AddPopup("&File", filemenu, layitem);
    fMenuBar->AddPopup("Lo&g",  logmenu,  layitem);
    fMenuBar->AddPopup("&Size", sizemenu, layitem);
    fMenuBar->AddPopup("&Tab",  tabmenu,  layitem);
    fMenuBar->AddPopup("&Loop", loopmenu, layitem);
    fMenuBar->BindKeys(this);
    AddFrame(fMenuBar);

    //
    // Line below menu bar
    //
    TGLayoutHints *laylinesep  = new TGLayoutHints(kLHintsTop|kLHintsExpandX);
    fList->Add(laylinesep);

    TGHorizontal3DLine *linesep = new TGHorizontal3DLine(this);
    AddFrame(linesep, laylinesep);

    //
    // Add everything to autodel list
    //
    fList->Add(savemenu);
    fList->Add(savemenu2);
    fList->Add(filemenu);
    fList->Add(loopmenu);
    fList->Add(sizemenu);
    fList->Add(fMenuBar);
    fList->Add(tabmenu);
    fList->Add(logmenu);
    fList->Add(linesep);
}

// --------------------------------------------------------------------------
//
// Adds an empty TGCompositeFrame which might be filled by the user
//
void MStatusDisplay::AddUserFrame()
{
    TGLayoutHints *lay=new TGLayoutHints(kLHintsExpandX);
    fList->Add(lay);

    fUserFrame = new TGCompositeFrame(this, 1, 1);
    AddFrame(fUserFrame, lay);
    fList->Add(fUserFrame);
}

// --------------------------------------------------------------------------
//
// Add the title tab
//
void MStatusDisplay::AddMarsTab()
{
    // Create Tab1
    TGCompositeFrame *f = fTab->AddTab("-=MARS=-");

    // Add list of tabs

    TGComboBox *filter = new TGComboBox(f, kTabs);
    fList->Add(filter);
    filter->Associate(this);
    filter->AddEntry("-=MARS=-", 0);
    filter->Select(0);


    TGLayoutHints *lay3 = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10, 10, 5);
    fList->Add(lay3);
    f->AddFrame(filter, lay3);

    // Add MARS version
    TGLabel *l = new TGLabel(f, MString::Format("Official Release: V%s", MARSVER));
    fList->Add(l);

    filter->SetWidth(5*l->GetWidth()/4);
    filter->SetHeight(4*l->GetHeight()/3);
    filter->GetListBox()->SetHeight(l->GetHeight()*16);

    TGLayoutHints *layb = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10, 5, 5);
    fList->Add(layb);
    f->AddFrame(l, layb);

    // Add root version
    l = new TGLabel(f, MString::Format("Using ROOT v%s", ROOT_RELEASE));
    fList->Add(l);

    TGLayoutHints *lay = new TGLayoutHints(kLHintsCenterX|kLHintsTop);
    fList->Add(lay);
    f->AddFrame(l, lay);

    // Add Mars logo picture
    const TGPicture *pic2 = fList->GetPicture("marslogo.xpm");
    if (pic2)
    {
        TGPictureButton *mars  = new TGPictureButton(f, pic2, kPicMars);
        fList->Add(mars);
        mars->Associate(this);

        TGLayoutHints *lay2 = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY, 10, 10, 5, 5);
        fList->Add(lay2);
        f->AddFrame(mars, lay2);
    }

    // Add date and time
    l = new TGLabel(f, TDatime().AsString());
    fList->Add(l);
    f->AddFrame(l, lay);

    // Add copyright notice
    l = new TGLabel(f, MString::Format("(c) MARS Software Development, 2000-%d", TDatime().GetYear()));
    fList->Add(l);
    f->AddFrame(l, layb);

    TGLayoutHints *layc = new TGLayoutHints(kLHintsCenterX|kLHintsTop, 10, 10, 0, 5);
    fList->Add(layc);

    const char *txt = "<< Thomas Bretz >>";
    l = new TGLabel(f, txt);
    fList->Add(l);
    f->AddFrame(l, layc);
}

// --------------------------------------------------------------------------
//
// Adds the logbook tab to the GUI if it was not added previously.
//
// The logbook is updated four times a second only if the tab is visible.
//
// You can redirect an output to a MLog-logstream by calling SetLogStream().
// To disable redirction call SetLogStream(NULL)
//
// if enable==kFALSE the stdout is disabled/enabled. Otherwise stdout
// is ignored.
//
void MStatusDisplay::SetLogStream(MLog *log, Bool_t enable)
{
    if (gROOT->IsBatch())
        return;

    if (log && fLogBox==NULL)
    {
        fLogIdx = fTab->GetNumberOfTabs();

        // Create Tab1
        TGCompositeFrame *f = AddRawTab("-Logbook-");//fTab->AddTab("-Logbook-");

        // Create Text View
        fLogBox = new MGTextView(f, 1, 1); // , -1, 0, TGFrame::GetDefaultFrameBackground());
        if (fFont)
            fLogBox->SetFont(fFont);
        //fLogBox->Associate(this);

        // Add List box to the tab
        TGLayoutHints *lay = new TGLayoutHints(kLHintsNormal|kLHintsExpandX|kLHintsExpandY,2,2,2,2);
        f->AddFrame(fLogBox, lay);

        // layout and map tab
        Layout();
        MapSubwindows();

        // make it visible
        // FIXME: This is a workaround, because TApplication::Run is not
        //        thread safe against ProcessEvents. We assume, that if
        //        we are not in the Main-Thread ProcessEvents() is
        //        called by the TApplication Event Loop...
        if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
            gClient->ProcessEventsFor(fTab);
    }

    if (log)
    {
        fLog = log;

        log->SetOutputGui(fLogBox, kTRUE);
        log->EnableOutputDevice(MLog::eGui);
        if (!enable)
            log->DisableOutputDevice(MLog::eStdout);

        fLogTimer.Start();
    }
    else
    {
        fLogTimer.Stop();

        fLog->DisableOutputDevice(MLog::eGui);
        fLog->SetOutputGui(NULL);
        if (!enable)
            fLog->EnableOutputDevice(MLog::eStdout);

        fLog = &gLog;
    }
}

// --------------------------------------------------------------------------
//
// Add the Tabs and the predifined Tabs to the GUI
//
void MStatusDisplay::AddTabs()
{
    fTab = new TGTab(this, 300, 300);

    AddMarsTab();

    // Add fTab to Frame
    TGLayoutHints *laytabs = new TGLayoutHints(kLHintsNormal|kLHintsExpandX|kLHintsExpandY, 5, 5, 5);
    AddFrame(fTab, laytabs);

    fList->Add(fTab);
    fList->Add(laytabs);
}

// --------------------------------------------------------------------------
//
// Add the progress bar to the GUI. The Progress Bar range is set to
// (0,1) as default.
//
void MStatusDisplay::AddProgressBar()
{
    TGLayoutHints *laybar=new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5);
    fList->Add(laybar);

    fBar=new TGHProgressBar(this);
    fBar->SetRange(0, 1);
    fBar->ShowPosition();
    AddFrame(fBar, laybar);
    fList->Add(fBar);
}

// --------------------------------------------------------------------------
//
// Set the progress bar position between 0 and 1. The Progress bar range
// is assumed to be (0,1)
//
void MStatusDisplay::SetProgressBarPosition(Float_t p, Bool_t upd)
{
    if (!gClient || gROOT->IsBatch())
        return;

    fBar->SetPosition(p);
    if (upd)
        gClient->ProcessEventsFor(fBar);
}

// --------------------------------------------------------------------------
//
// Adds the status bar to the GUI
//
void MStatusDisplay::AddStatusBar()
{
    fStatusBar = new TGStatusBar(this, 1, 1);

    //
    // Divide it like the 'Golden Cut' (goldener Schnitt)
    //
    //     1-a     a
    // 1: ------|----
    //
    // a/(1-a) = (1-a)/1
    // a^2+a-1 = 0
    //       a = (-1+-sqrt(1+4))/2 = sqrt(5)/2-1/2 = 0.618
    //
    Int_t p[] = {38-2, 62-8, 10};

    fStatusBar->SetParts(p, 3);

    TGLayoutHints *layb = new TGLayoutHints(kLHintsNormal|kLHintsExpandX, 5, 4, 0, 3);
    AddFrame(fStatusBar, layb);

    fList->Add(fStatusBar);
    fList->Add(layb);
}

// --------------------------------------------------------------------------
//
// Change the text in the status line 1
//
void MStatusDisplay::SetStatusLine(const char *txt, Int_t i)
{
    if (gROOT->IsBatch())
        return;
    fStatusBar->SetText(txt, i);

    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        gClient->ProcessEventsFor(fStatusBar);
}

// --------------------------------------------------------------------------
//
// Display information about the name of a container
//
void MStatusDisplay::SetStatusLine2(const MParContainer &cont)
{
    SetStatusLine2(MString::Format("%s: %s", cont.GetDescriptor().Data(), cont.GetTitle()));
}

// --------------------------------------------------------------------------
//
// Get TGPopupMenu as defined by name from fMenuBar
//
TGPopupMenu *MStatusDisplay::GetPopup(const char *name)
{
    if (!fMenuBar)
        return 0;

    TGPopupMenu *m = fMenuBar->GetPopup(name);
    if (!m)
    {
        *fLog << warn << name << " doesn't exist in menu bar." << endl;
        return 0;
    }

    return m;
}

// --------------------------------------------------------------------------
//
// Default constructor. Opens a window with a progress bar. Get a pointer
// to the bar by calling GetBar. This pointer can be used for the
// eventloop.
//
// Be carefull: killing or closing the window while the progress meter
//   is still in use may cause segmentation faults. Please kill the window
//   always by deleting the corresponding object.
//
// You can give either width or height. (Set the value not given to -1)
// The other value is calculated accordingly. If width and height are
// given height is ignored. If width=height=0 an optimum size from
// the desktop size is calculated.
//
// Update time default: 10s
//
MStatusDisplay::MStatusDisplay(Int_t w, Int_t h, Long_t t)
: TGMainFrame((TGWindow*)((gClient?gClient:new TGClient),NULL), 1, 1), fName("MStatusDisplay"), fLog(&gLog), fBar(NULL), fTab(NULL), fTimer(this, t, kTRUE), fStatus(kLoopNone), fLogIdx(-1), fLogTimer(this, 250, kTRUE), fLogBox(NULL), fIsLocked(0)
{
    // p==NULL means: Take gClient->GetRoot() if not in batch mode
    // see TGWindow::TGWindow()

    // Make sure that the display is removed via RecursiveRemove
    // from whereever possible.
    SetBit(kMustCleanup);

    //
    // This is a possibility for the user to check whether this
    // object has already been deleted. It will be removed
    // from the list in the destructor.
    //
    gROOT->GetListOfSpecials()->Add(this);

    fFont = gVirtualX->LoadQueryFont("7x13bold");
    fMutex = new TMutex;

    //
    // In case we are in batch mode use a list of canvases
    // instead of the Root Embedded Canvases in the TGTab
    //
    fBatch = new TList;
    fBatch->SetOwner();

    //
    // Create a list handling GUI widgets
    //
    fList = new MGList;
    fList->SetOwner();

    //
    // Create the layout hint for the root embedded canavses
    //
    fLayCanvas = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY);
    fList->Add(fLayCanvas);

    //
    // Add Widgets (from top to bottom)
    //
    // In newer root versions gClient!=NULL in batch mode!
    if (!gClient || !gClient->GetRoot() || gROOT->IsBatch()) // BATCH MODE
    {
        Resize(644, 484);
        return;
    }

    AddMenuBar();
    AddUserFrame();
    AddTabs();
    AddProgressBar();
    AddStatusBar();

    //
    // set the smallest and biggest size of the Main frame
    // and move it to its appearance position
    SetWMSizeHints(566, 476, 2048, 1536, 1, 1);
    MoveResize(rand()%100+566, rand()%100+476, 566, 476);
    if (h>0)
        SetDisplayHeight(h);
    if (w>0)
        SetDisplayWidth(w);
    if (w==0 && h==0)
        SetOptimumSize();

    //
    // Now do an automatic layout of the widgets and display the window
    //
    Layout();
    MapSubwindows();

    SetWindowName("Status Display");
    SetIconName("Status Display");

    MapWindow();

    UpdateMemory();

    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        gSystem->ProcessEvents();
}

// --------------------------------------------------------------------------
//
// Destruct the window with all its tiles. Also the Progress Bar object
// is deleted.
//
MStatusDisplay::~MStatusDisplay()
{
    fTimer.Stop();

#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,01)
    fTab = NULL; // See HandleEvent
#endif

    //
    // Delete object from global object table so it cannot
    // be deleted by chance a second time
    //
    gInterpreter->DeleteGlobal(this);

    //
    // This is a possibility for the user to check whether this
    // object has already been deleted. It has been added
    // to the list in the constructor.
    //
    gROOT->GetListOfSpecials()->Remove(this);

    SetLogStream(NULL);

    //
    // Delete the list of objects corresponding to this object
    //
    delete fList;

    //
    // Delete the list of canvases used in batch mode
    // instead of the Root Embedded Canvases in the TGTab
    //
    delete fBatch;

    //
    // Delete the font used for the logging window
    //
    if (fFont)
        gVirtualX->DeleteFont(fFont);

    //
    // Delete mutex
    //
    delete fMutex;
}

// --------------------------------------------------------------------------
//
// Takes a TGCompositeFrame as argument. Searches for the first
// TRootEmbeddedCanvas which is contained by it and returns a pointer
// to the corresponding TCanvas. If it isn't found NULL is returned.
//
TRootEmbeddedCanvas *MStatusDisplay::GetEmbeddedCanvas(TGCompositeFrame &cf)
{
    TIter Next(cf.GetList());

    TGFrameElement *f;
    while ((f=(TGFrameElement*)Next()))
        if (f->fFrame->InheritsFrom(TRootEmbeddedCanvas::Class()))
            return (TRootEmbeddedCanvas*)f->fFrame;

    return NULL;
}

// --------------------------------------------------------------------------
//
// Takes a TGCompositeFrame as argument. Searches for the first
// TRootEmbeddedCanvas which is contained by it and returns a pointer
// to the corresponding TCanvas. If it isn't found NULL is returned.
//
TCanvas *MStatusDisplay::GetCanvas(TGCompositeFrame &cf)
{
    TRootEmbeddedCanvas *ec = GetEmbeddedCanvas(cf);
    return ec ? ec->GetCanvas() : NULL;
}

// --------------------------------------------------------------------------
//
// Returns the range of tabs containing valid canvases for the condition
// num.
//
void MStatusDisplay::GetCanvasRange(Int_t &from, Int_t &to, Int_t num) const
{
    const Int_t max  = gROOT->IsBatch() ? fBatch->GetSize()+1 : fTab->GetNumberOfTabs();

    from = num<0 ?   1 : num;
    to   = num<0 ? max : num+1;
}

// --------------------------------------------------------------------------
//
// Returns GetCanvas of the i-th Tab.
//
TCanvas *MStatusDisplay::GetCanvas(int i) const
{
    if (gROOT->IsBatch())
        return (TCanvas*)fBatch->At(i-1);

    if (i<0 || i>=fTab->GetNumberOfTabs())
    {
        *fLog << warn << "MStatusDisplay::GetCanvas: Out of range." << endl;
        return NULL;
    }

    return GetCanvas(*fTab->GetTabContainer(i));
}

// --------------------------------------------------------------------------
//
// Returns j-th pad of the i-th Tab. 
// Sets the pad to fill an entire window.
//
// This function can be used if single pad's out of an MStatusDisplay 
// have to be stored to file. 
//
// ATTENTION: This function modifies the requested tab in MStatusDisplay itself!
//
TVirtualPad *MStatusDisplay::GetFullPad(const Int_t i, const Int_t j)
{
    if (!GetCanvas(i))
    {
        *fLog << warn << "MStatusDisplay::GetFullPad: i-th canvas not dound." << endl;
        return NULL;
    }

    TVirtualPad *vpad = GetCanvas(i)->GetPad(j);
    if (!vpad)
    {
        *fLog << warn << "MStatusDisplay::GetFullPad: Pad is out of range." << endl;
        return NULL;
    }

    vpad->SetPad(0.,0.,1.,1.);
    return vpad;
}

// --------------------------------------------------------------------------
//
// Searches for a TRootEmbeddedCanvas in the TGCompositeFramme of the
// Tab with the name 'name'. Returns the corresponding TCanvas or
// NULL if something isn't found.
//
TCanvas *MStatusDisplay::GetCanvas(const TString &name) const
{
    if (gROOT->IsBatch())
        return (TCanvas*)fBatch->FindObject(name);

    TGFrameElement *f;
    TIter Next(fTab->GetList());
    while ((f=(TGFrameElement*)Next()))
    {
        TObject *frame = f->fFrame;
        if (!frame->InheritsFrom(TGTabElement::Class()))
            continue;

        TGTabElement *tab = (TGTabElement*)frame;
        if (tab->GetString()==name)
            break;
    }

    // Search for the next TGCompositeFrame in the list
    while ((f=(TGFrameElement*)Next()))
    {
        TObject *frame = f->fFrame;
        if (frame->InheritsFrom(TGCompositeFrame::Class()))
            return GetCanvas(*(TGCompositeFrame*)frame);
    }

    return NULL;
}

// --------------------------------------------------------------------------
//
// Calls TCanvas::cd(), for the canvas returned by GetCanvas.
//
Bool_t MStatusDisplay::CdCanvas(const TString &name)
{
    TCanvas *c = GetCanvas(name);
    if (!c)
        return kFALSE;

    c->cd();
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the number of user added tabs (not that in batch mode this
// exclude tabs without a canvas)
//
Int_t MStatusDisplay::GetNumTabs() const
{
    return gROOT->IsBatch() ? fBatch->GetEntries() : fTab->GetNumberOfTabs()-1;
}

TGCompositeFrame *MStatusDisplay::AddRawTab(const char *name)
{
    // Add new tab
    TGCompositeFrame *f = fTab->AddTab(name);

    TGComboBox *box = (TGComboBox*)fList->FindWidget(kTabs);
    box->AddEntry(name, box->GetListBox()->GetNumberOfEntries());

    // layout and map new tab
    Layout();
    MapSubwindows();
    Layout();

    UpdateMemory();

    // display new tab in the main frame
    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        gClient->ProcessEventsFor(fTab);

    *fLog << inf3 << "Adding Raw Tab '" << name << "' (" << f->GetWidth() << "x";
    *fLog << f->GetHeight() << ")" << endl;

    // return pointer to new canvas
    return f;
}

// --------------------------------------------------------------------------
//
// This function was connected to all created canvases. It is used
// to redirect GetObjectInfo into our own status bar.
//
// The 'connection' is done in AddTab
//
void MStatusDisplay::EventInfo(Int_t /*event*/, Int_t px, Int_t py, TObject *selected)
{
    //  Writes the event status in the status bar parts
    if (!selected)
        return;

    TCanvas *c = (TCanvas*)gTQSender;

    TVirtualPad* save=gPad;

    gPad = c ? c->GetSelectedPad() : NULL;

    if (gPad)
    {
        // Find the object which will get picked by the GetObjectInfo
        // due to buffer overflows in many root-versions
        // in TH1 and TProfile we have to work around and implement
        // our own GetObjectInfo which make everything a bit more
        // complicated.
#if ROOT_VERSION_CODE > ROOT_VERSION(5,22,00)
        SetStatusLine2(selected->GetObjectInfo(px,py));
#else
        TObjLink *link=0;
        static_cast<TPad*>(gPad)->Pick(px, py, link);

        const TObject *o = link ? link->GetObject() : 0;
        if (o)
            SetStatusLine2(MH::GetObjectInfo(px, py, *o));
#endif
    }

    gPad=save;
}

// --------------------------------------------------------------------------
//
// Adds a new tab with the name 'name'. Adds a TRootEmbeddedCanvas to the
// tab and returns a reference to the corresponding TCanvas.
//
TCanvas &MStatusDisplay::AddTab(const char *name, const char *title)
{
    /*
    if (GetCanvas(name))
    {
        *fLog << warn;
        *fLog << "WARNING - A canvas '" << name << "' is already existing in the Status Display." << endl;
        *fLog << "          This can cause unexpected crahes!" << endl;
    }*/

    gROOT->SetSelectedPad(0);

    if (gROOT->IsBatch())
    {
        // 4 = 2*default border width of a canvas
        const UInt_t cw = GetWidth();
        const UInt_t ch = 2*cw/3 + 25; // 25: Menu, etc

        // The constructor of TCanvas adds the canvas to the global list
        // of canvases gROOT->GetListOfCanvases(). If a canvas with an
        // identical name exists already in this list, the canvas is
        // deleted. In normal operation this might make sense and doesn't harm
        // because the embedded canvases behave different.
        // By creating the canvas without a name it is made sure that no
        // older canvas/tab vanished silently from the system (deleted from
        // the construtor). To make the handling of our canvases nevertheless
        // work well the name is set later. The list of canvases is also
        // part of the list of cleanups, thus fBatch need not to be added
        // to the list of cleanups.
        TCanvas *c = new TCanvas("", title?title:"", -cw, ch);
        c->SetName(name);
        c->SetFillColor(10); // White
        c->SetFrameBorderMode(0);
        c->SetBorderMode(0);
        fBatch->Add(c);

        *fLog << inf3 << "Adding Canvas '" << name << "' (" << c->GetWw() << "x";
        *fLog << c->GetWh() << ", TCanvas=" << c << ")" << endl;

        // Remove the canvas from the global list to make sure it is
        // not found by gROOT->FindObject
        //gROOT->GetListOfCanvases()->Remove(c);
        //gROOT->GetListOfCleanups()->Add(c);

        return *c;
    }

    // Add new tab
    TGCompositeFrame *f = fTab->AddTab(name);

    // create root embedded canvas and add it to the tab
    TRootEmbeddedCanvas *ec = new TRootEmbeddedCanvas(name, f, f->GetWidth(), f->GetHeight(), kSunkenFrame);
    f->AddFrame(ec, fLayCanvas);
    fList->Add(ec);

    // set background and border mode of the canvas
    TCanvas &c = *ec->GetCanvas();

    if (title)
        c.SetTitle(title);

    c.SetFillColor(10); // White
    c.SetFrameBorderMode(0);
    c.SetBorderMode(0);

    // If kNoContextMenu set set kNoContextMenu of the canvas
    if (TestBit(kNoContextMenu))
        c.SetBit(kNoContextMenu);

    // Connect all TCanvas::ProcessedEvent to this->EventInfo
    // This means, that after TCanvas has processed an event
    // EventInfo of this class is called, see TCanvas::HandleInput
    c.Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
              "MStatusDisplay", this, "EventInfo(Int_t,Int_t,Int_t,TObject*)");

    // Make sure that root itself doesn't try to call GetObjectInfo
    // This is now handled from EventsInfo. This is necessary
    // due to the buffer overflow bug in GetObjectInfo of
    // TProfile and TH1
    c.ResetBit(TCanvas::kShowEventStatus);

    // Remove the canvas from the global list to make sure it is
    // not found by gROOT->FindObject
    //gROOT->GetListOfCanvases()->Remove(&c);
    //gROOT->GetListOfCleanups()->Add(&c);

    TGComboBox *box = (TGComboBox*)fList->FindWidget(kTabs);
    box->AddEntry(name, box->GetListBox()->GetNumberOfEntries());

    // layout and map new tab
    Layout();          // seems to layout the TGCompositeFrame
    MapSubwindows();   // maps the TGCompositeFrame
    Layout();          // layout the embedded canvas in the frame

    UpdateMemory();

    // display new tab in the main frame
    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        gClient->ProcessEventsFor(fTab);

    *fLog << inf3 << "Adding Tab '" << name << "' (" << f->GetWidth() << "x";
    *fLog << f->GetHeight() << ", TCanvas=" << &c << ")" << endl;

    // return pointer to new canvas
    return c;
}

// --------------------------------------------------------------------------
//
// Update a canvas in a tab, takes the corresponding TGCompositeFrame
// as an argument. This is necessary, because not all functions
// changing the contents of a canvas or pad can call SetModified()
// for the corresponding tab. If this is not called correctly the
// tab won't be updated calling TCanvas::Update(). So we simply
// redraw it by our own (instead we could recursively call
// TPad::Modified() for everything contained by the TCanvas and
// call TCanvas::Update() afterwards)
//
void MStatusDisplay::UpdateTab(TGCompositeFrame *f)
{
    if (!f)
        return;

    //
    // If we are in a multithreaded environment (gThreadXAR) we
    // have to make sure, that thus function is called from
    // the main thread.
    //
    if (gThreadXAR)
    {
        // Tell the X-Requester how to call this method
        const TString str = MString::Format("%ld", (ULong_t)f);

        TMethodCall call(IsA(), "UpdateTab", "NULL");
        void *arr[4] = { NULL, &call, this, (void*)(const char*)str };

        // If this is not the main thread return
        if (((*gThreadXAR)("METH", 4, arr, NULL)))
            return;
    }

    TCanvas *c=GetCanvas(*f);
    if (!c)
        return;

    const TString name  = c->GetName();
    const TString title = c->GetTitle();

    if (!gROOT->IsBatch())
        fStatusBar->SetText(title, 0);

    // It seems that we cannot use this because an update
    // initiated by a gui event can interfere with the regular
    // update from the timer and then ProcessEvents will
    // end in an infinite loop.
    // SetStatusLine1(title);


    //
    // Secure calls to update the tabs against itself, at least
    // c->Paint() or c->Flush() may crash X (bad drawable).
    // This makes sure, that a X call is not interuppted by
    // another X-call which was started from an gui interrrupt
    // in the same thread
    //
    const Int_t rc = fMutex->TryLock();
    if (rc==13)
        gLog << warn << "MStatusDisplay::UpdateTab - mutex is already locked by this thread." << endl;

    if (rc)
        return;

#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,02)
    TPad *padsav = (TPad*)gPad;
    if (!gPad)
        c->cd();
#endif

    if (!c->IsBatch())
        c->FeedbackMode(kFALSE);  // Goto double buffer mode

    //
    // Doing this ourself gives us the possibility to repaint
    // the canvas in any case (Paint() instead of PaintModified())
    //
    c->Paint();                   // Repaint all pads
    c->Flush();                   // Copy all pad pixmaps to the screen

#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,02)
    if (padsav)
        padsav->cd();
    else
        gPad=NULL;
#endif

    //c->SetCursor(kCross);

    // Old version
    //c->Modified();
    //c->Update();
    //c->Paint();

    fMutex->UnLock();
}

TString MStatusDisplay::PrintDialog(TString &p, TString &c, TString &t, const char *ext)
{
    // If not in batch mode open a user dialog
    if (!gROOT->IsBatch())
    {
        char *cprinter = StrDup(p);
        char *ccmd     = StrDup(c);

        Int_t rc=0;
        new TGPrintDialog(fClient->GetRoot(), this, 400, 150, &cprinter, &ccmd, &rc);
        if (rc)
        {
            p = cprinter; // default has been changed
            c = ccmd;
        }

        delete [] cprinter;
        delete [] ccmd;

        if (!rc)
            return "";
    }

    if (c.Contains("%f") && ext)
    {
        // Get temporary file name
        TString name = "mars";

        FILE *f = gSystem->TempFileName(name, t);
        if (!f)
        {
            *fLog << warn << "MStatusDisplay::PrintDialog: Couldn't create temporary file in " << t << endl;
            SetStatusLine2("failed!");
            return "";
        }
        fclose(f);

        // remove temp file
        gSystem->Unlink(name);
        name += ".";
        name += ext;

        t = name;
    }

    // compile command
    TString cmd(c);

    // if sprinter.IsNull we assume that everything around %p can
    // be omitted and the program uses some kind of default
    if (p.IsNull())
    {
        TString sub;
        while (1)
        {
            sub = TString(cmd(TRegexp(" .*%p.* "))).Strip(TString::kBoth);
            if (sub.IsNull())
                break;

            cmd.ReplaceAll(sub, "");
        }
    }

    cmd.ReplaceAll("%p", p);
    cmd.ReplaceAll("%f", t);

    return cmd;
}

// --------------------------------------------------------------------------
//
// Saves the given canvas (pad) or all pads (num<0) as a temporary
// postscript file and prints it.
//
// The default command line c is: lpr -P%p %f
//   %p: printer name
//   %f: temporary file name
//
// The default printer name p is: <empty>
//
// Both can be changed in .rootrc by:
//   PrintPS.Printer
//   PrintPS.Command
//
// Ant the location of the temporary file t can by changed by
//   Print.Directory
// the default is the system default directory (normally /tmp)
//
Int_t MStatusDisplay::PrintPS(Int_t num, const char *p, const char *c, const char *t)
{
    static TString sprinter = gEnv->GetValue("PrintPS.Printer", p&&*p?p:"");
    static TString scmd     = gEnv->GetValue("PrintPS.Command", c&&*c?c:"lpr -P%p %f");

    TString tmp = gEnv->GetValue("Print.Directory", t&&*t?t:gSystem->TempDirectory());

    TString cmd = PrintDialog(sprinter, scmd, tmp, "ps");
    if (cmd.IsNull())
        return 0;

    // set status lines
    SetStatusLine1("Printing...");
    SetStatusLine2("");

    // print to temporary file
    const Int_t pages = SaveAsPS(num, tmp);

    // check
    if (!pages)
    {
        *fLog << warn << "MStatusDisplay::Print: Sorry, couldn't save file as temporary postscript!" << endl;
        SetStatusLine2("Failed!");
        return 0;
    }

    // execute command
    *fLog << dbg << "Executing: " << cmd << endl;
    gSystem->Exec(cmd);

    // remove temporary file
    gSystem->Unlink(tmp);

    SetStatusLine2(MString::Format("Done (%dpage(s))", pages));

    return pages;
}

// --------------------------------------------------------------------------
//
// Remove tab no i if this tab contains a TRootEmbeddedCanvas
//
void MStatusDisplay::RemoveTab(int i)
{
    TGCompositeFrame *f = fTab->GetTabContainer(i);
    if (!f)
        return;

    TRootEmbeddedCanvas *ec = GetEmbeddedCanvas(*f);
    if (!ec)
        return;

    TCanvas *c = ec->GetCanvas();
    if (!c)
        return;

    // Repair the "Workaround" being in the RecursiveRemove list
    // but not in a list checked by gROOT->FindObject
    //gROOT->GetListOfCleanups()->Remove(c);
    //gROOT->GetListOfCanvases()->Add(c);

    // FIXME: Due to our workaround this is necessary for a successfull deletion
    //c->cd();

    const TString name(c->GetName());

    f->RemoveFrame(ec);
    delete fList->Remove(ec);

    fTab->RemoveTab(i);
    fTab->SetTab(0);

    TGComboBox *box = (TGComboBox*)fList->FindWidget(kTabs);
    box->RemoveEntry(i);
    for (int j=i; j<box->GetListBox()->GetNumberOfEntries(); j++)
    {
        TGTextLBEntry *entry = (TGTextLBEntry *)box->GetListBox()->Select(j+1, kFALSE);
        box->AddEntry(entry->GetText()->GetString(), j);
        box->RemoveEntry(j+1);
    }
    box->GetListBox()->Select(0);

    // Looks strange...
    // const Int_t n = fTab->GetNumberOfTabs();
    // fTab->SetTab(i<=n-1 ? i : i-1);

    // layout and map new tab
    Layout();          // seems to layout the TGCompositeFrame
    MapSubwindows();   // maps the TGCompositeFrame
    Layout();          // layout the embedded canvas in the frame

    UpdateMemory();

    // display new tab in the main frame
    // FIXME: This is a workaround, because TApplication::Run is not
    //        thread safe against ProcessEvents. We assume, that if
    //        we are not in the Main-Thread ProcessEvents() is
    //        called by the TApplication Event Loop...
    if (!TThread::Self()/*gApplication->InheritsFrom(TRint::Class())*/)
        gClient->ProcessEventsFor(fTab);

    *fLog << inf << "Removed Tab #" << i << " '" << name << "'" << endl;
}

// --------------------------------------------------------------------------
//
// Use this to check whether the MStatusDisplay still contains the
// TCanvas c. It could be removed meanwhile by menu usage.
//
Bool_t MStatusDisplay::HasCanvas(const TCanvas *c) const
{
    if (!c)
        return kFALSE;

    // If you encounter unexpected crashes here, check if
    // a canvas is existing twice in the list or has been
    // deleted by accident. (Check AddTab)

    if (gROOT->IsBatch())
        return (Bool_t)fBatch->FindObject(c);

    for (int i=1; i<fTab->GetNumberOfTabs(); i++)
        if (c==GetCanvas(i))
            return kTRUE;
    return kFALSE;
}

/*
    if (...)
       fMenu->AddPopup("&CaOs", fCaOs, NULL);
    else
       fMenu->RemovePopup("CaOs");
    fMenu->Resize(fMenu->GetDefaultSize());
    MapSubwindows();
    MapWindow();
 */

void MStatusDisplay::Reset()
{
    if (gROOT->IsBatch())
    {
        fBatch->Delete();
        return;
    }

    for (int i=fTab->GetNumberOfTabs()-1; i>0; i--)
        RemoveTab(i);
}

Bool_t MStatusDisplay::SaveLogAsPS(const char *n) const
{
    TString name(n);
    AddExtension(name, "ps");

    // Code taken from TGTextEdit::Print
    const TString pipe = MString::Format("a2ps -o%s", name.Data());
    FILE *p = gSystem->OpenPipe(pipe, "w");
    if (!p)
    {
        *fLog << err << "ERROR - Couldn't open pipe " << pipe << ": " << strerror(errno) << endl;
        return kFALSE;
    }

    TGText *text = fLogBox->GetText();

    char   *buf1, *buf2;
    Long_t  len;
    ULong_t i = 0;
    TGLongPosition pos;

    Bool_t rc = kTRUE;

    pos.fX = pos.fY = 0;
    while (pos.fY < text->RowCount())
    {
        len = text->GetLineLength(pos.fY);
        buf1 = text->GetLine(pos, len);
        buf2 = new char[len + 2];
        strncpy(buf2, buf1, (UInt_t)len);
        buf2[len]   = '\n';
        buf2[len+1] = '\0';
        while (buf2[i] != '\0') {
            if (buf2[i] == '\t') {
                ULong_t j = i+1;
                while (buf2[j] == 16 && buf2[j] != '\0')
                    j++;
                strcpy(buf2+i+1, buf2+j);
            }
            i++;
        }

        len = sizeof(char)*(strlen(buf2)+1);

        const size_t ret = fwrite(buf2, len, 1, p);
        delete [] buf1;
        delete [] buf2;

        if (ret!=1)
        {
            *fLog << err << "ERROR - fwrite to pipe " << pipe << " failed: " << strerror(errno) << endl;
            rc = kFALSE;
            break;
        }

        pos.fY++;
    }
    gSystem->ClosePipe(p);

    return rc;
}

// --------------------------------------------------------------------------
//
// Print the log text.
//
// The default command line c is: a2ps -P%p
//   %p: printer name
//
// The default printer name p is: <empty>
//
// Both can be changed in .rootrc by:
//   PrintText.Printer
//   PrintText.Command
//
Bool_t MStatusDisplay::PrintLog(const char *p, const char *c)
{
    static TString sprinter = gEnv->GetValue("PrintText.Printer", p&&*p?p:"");
    static TString scmd     = gEnv->GetValue("PrintText.Command", c&&*c?c:"a2ps -P%p");

    TString tmp;
    TString cmd = PrintDialog(sprinter, scmd, tmp);
    if (cmd.IsNull())
        return kFALSE;

    // set status lines
    SetStatusLine1("Printing...");
    SetStatusLine2("");

    // print to temporary file
    if (!SaveLogAsPS(cmd))
    {
        *fLog << warn << "MStatusDisplay::PrintLog: Sorry, couldn't create postscript!" << endl;
        SetStatusLine2("Failed!");
        return kFALSE;
    }

    // execute command
    *fLog << dbg << "Executing: " << cmd << endl;
    gSystem->Exec(cmd);

    SetStatusLine2("Done.");

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process the kC_COMMAND, kCM_MENU  messages
//
Bool_t MStatusDisplay::ProcessMessageCommandMenu(Long_t id)
{
    switch (id)
    {
    case kLoopPause:
        {
            TGPopupMenu *m = GetPopup("Loop");
            if (!m)
                return kTRUE;

            if (fStatus==kLoopNone)
            {
                fStatus = (Status_t)kLoopPause;
                m->CheckEntry(kLoopPause);
                m->EnableEntry(kLoopStep);
                return kTRUE;
            }
            if (fStatus==kLoopPause)
            {
                fStatus = (Status_t)kLoopNone;
                m->UnCheckEntry(kLoopPause);
                m->DisableEntry(kLoopStep);
                return kTRUE;
            }
        }
        return kTRUE;

    case kLoopStep:
        fStatus = (Status_t)kLoopStep;
        return kTRUE;

    case kLoopStop:
    case kFileClose:
    case kFileExit:
        if (id==kFileExit || id==kFileClose)
            if (Close())
                delete this;
        fStatus = (Status_t)id;
        return kTRUE;

    case kFileCanvas:
        new TCanvas;
        return kTRUE;

    case kFileBrowser:
        new TBrowser;
        return kTRUE;

    case kFileTab:
        AddTab(MString::Format("%d", fTab->GetNumberOfTabs()));
        return kTRUE;

    case kFileReset:
        Reset();
        return kTRUE;

    case kFileOpen:
        Open();
        return kTRUE;

    case kFileSaveAs:
        SaveAs();
        return kTRUE;

    case kFileSaveAsPS:
        SaveAsPS();
        return kTRUE;

    case kFileSaveAsPDF:
        SaveAsPDF();
        return kTRUE;

    case kFileSaveAsSVG:
        SaveAsSVG();
        return kTRUE;

    case kFileSaveAsPNG:
        SaveAsPNG();
        return kTRUE;

    case kFileSaveAsGIF:
        SaveAsGIF();
        return kTRUE;

    case kFileSaveAsXPM:
        SaveAsXPM();
        return kTRUE;

    case kFileSaveAsJPG:
        SaveAsJPG();
        return kTRUE;

    case kFileSaveAsTIFF:
        SaveAsTIFF();
        return kTRUE;

    case kFileSaveAsBMP:
        SaveAsBMP();
        return kTRUE;

    case kFileSaveAsXML:
        SaveAsXML();
        return kTRUE;

    case kFileSaveAsCSV:
        SaveAsCSV();
        return kTRUE;

    case kFileSaveAsC:
        SaveAsC();
        return kTRUE;

    case kFileSaveAsRoot:
        SaveAsRoot();
        return kTRUE;

    case kFileSaveAsPlainRoot:
        SaveAsPlainRoot(kTRUE);
        return kTRUE;

    case kFilePrint:
        PrintPS();
        return kTRUE;

    case kTabSaveAs:
        SaveAs(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsPS:
        SaveAsPS(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsPDF:
        SaveAsPDF(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsSVG:
        SaveAsSVG(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsPNG:
        SaveAsPNG(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsGIF:
        SaveAsGIF(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsXPM:
        SaveAsXPM(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsJPG:
        SaveAsJPG(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsTIFF:
        SaveAsTIFF(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsBMP:
        SaveAsBMP(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsXML:
        SaveAsXML(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsCSV:
        SaveAsCSV(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsC:
        SaveAsC(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsPlainRoot:
        SaveAsPlainRoot(fTab->GetCurrent());
        return kTRUE;

    case kTabSaveAsRoot:
        SaveAsRoot(fTab->GetCurrent());
        return kTRUE;

    case kTabPrint:
        PrintPS(fTab->GetCurrent());
        return kTRUE;

    case kTabNext:
        fTab->SetTab(fTab->GetCurrent()+1);
        return kTRUE;

    case kTabPrevious:
        fTab->SetTab(fTab->GetCurrent()-1);
        return kTRUE;

    case kTabRemove:
        RemoveTab(fTab->GetCurrent());
        return kTRUE;

    case kSize640:
        SetDisplaySize(640, 480);
        return kTRUE;
    case kSize768:  
        SetDisplaySize(768, 576);
        return kTRUE;
    case kSize800:  
        SetDisplaySize(800, 600);
        return kTRUE;
    case kSize960:  
        SetDisplaySize(960, 720);
        return kTRUE;
    case kSize1024: 
        SetDisplaySize(1024, 768);
        return kTRUE;
    case kSize1152:
        SetDisplaySize(1152, 864);
        return kTRUE;
    case kSize1280: 
        SetDisplaySize(1280, 1024);
        return kTRUE;
    case kSize1400:
        SetDisplaySize(1400, 1050);
        return kTRUE;
    case kSize1600:
        SetDisplaySize(1600, 1200);
        return kTRUE;
    case kSizeOptimum:
        SetOptimumSize();
        return kTRUE;

    case kLogClear:
        fLogBox->Clear();
        return kTRUE;
    case kLogCopy:
        fLogBox->Copy();
        return kTRUE;
    case kLogSelect:
        fLogBox->SelectAll();
        return kTRUE;
    case kLogFind:
        new MSearch(this);
        return kTRUE;
    case kLogSave:
        SetStatusLine1("Saving log...");
        SetStatusLine2("");
        *fLog << inf << "Saving log... " << flush;
        if (fLogBox->GetText()->Save(MString::Format("%s.log", gROOT->GetName())))
        {
            *fLog << "done." << endl;
            SetStatusLine2("done.");
        }
        else
        {
            *fLog << "failed!" << endl;
            SetStatusLine2("Failed!");
        }
        return kTRUE;

    case kLogAppend:
        SetStatusLine1("Appending log...");
        SetStatusLine2("");
        *fLog << inf << "Appending log... " << flush;
        if (fLogBox->GetText()->Append(MString::Format("%s.log", gROOT->GetName())))
        {
            *fLog << "done." << endl;
            SetStatusLine2("done.");
        }
        else
        {
            *fLog << "failed!" << endl;
            SetStatusLine2("Failed!");
        }
        return kTRUE;

    case kLogPrint:
        PrintLog();
        return kTRUE;
#ifdef DEBUG
    default:
        cout << "Command-Menu #" << id << endl;
#endif
    }
    return kTRUE;

}

// --------------------------------------------------------------------------
//
// Process the kC_COMMAND messages
//
Bool_t MStatusDisplay::ProcessMessageCommand(Long_t submsg, Long_t mp1, Long_t mp2)
{
    switch (submsg)
    {
    case kCM_MENU: // 1
        return ProcessMessageCommandMenu(mp1); // mp2=userdata
    case kCM_TAB:  // 8
        /*
         for (int i=0; i<fTab->GetNumberOfTabs(); i++)
         fTab->GetTabContainer(i)->UnmapWindow();
         */
        UpdateTab(fTab->GetTabContainer(mp1));
        //fTab->GetTabContainer(mp1)->MapWindow();

        /*
        if (mp1>0)
            fMenu->AddPopup("&CaOs", fCaOs, NULL);
        else
            fMenu->RemovePopup("CaOs");
        fMenu->Resize(fMenu->GetDefaultSize());
        MapSubwindows();
        MapWindow();
        */
        return kTRUE;
    case kCM_COMBOBOX: // 7
        if (mp1==kTabs)
            fTab->SetTab(mp2);
        return kTRUE;
#ifdef DEBUG
    case kCM_MENUSELECT: // 2
        cout << "Command-Menuselect #" << mp1 << " (UserData=" << (void*)mp2 << ")" << endl;
        return kTRUE;

    case kCM_BUTTON: // 3
        cout << "Command-Button." << endl;
        return kTRUE;

    case kCM_CHECKBUTTON: // 4
        cout << "Command-CheckButton." << endl;
        return kTRUE;

    case kCM_RADIOBUTTON: // 5
        cout << "Command-RadioButton." << endl;
        return kTRUE;

    case kCM_LISTBOX: // 6
        cout << "Command-Listbox #" << mp1 << " (LineId #" << mp2 << ")" << endl;
        return kTRUE;
    default:
        cout << "Command: " << "Submsg:" << submsg << " Mp1=" << mp1 << " Mp2=" << mp2 << endl;
#endif
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process the kC_TEXTVIEW messages
//
Bool_t MStatusDisplay::ProcessMessageTextview(Long_t /*submsg*/, Long_t /*mp1*/, Long_t /*mp2*/)
{
    // kC_TEXTVIEW, kTXT_ISMARKED, widget id, [true|false]                  //
    // kC_TEXTVIEW, kTXT_DATACHANGE, widget id, 0                           //
    // kC_TEXTVIEW, kTXT_CLICK2, widget id, position (y << 16) | x)         //
    // kC_TEXTVIEW, kTXT_CLICK3, widget id, position (y << 16) | x)         //
    // kC_TEXTVIEW, kTXT_F3, widget id, true                                //
    // kC_TEXTVIEW, kTXT_OPEN, widget id, 0                                 //
    // kC_TEXTVIEW, kTXT_CLOSE, widget id, 0                                //
    // kC_TEXTVIEW, kTXT_SAVE, widget id, 0                                 //
#ifdef DEBUG
    switch (submsg)
    {
    case kTXT_ISMARKED:
        cout << "Textview-IsMarked #" << mp1 << " " << (mp2?"yes":"no") << endl;
        return kTRUE;

    case kTXT_DATACHANGE:
        cout << "Textview-DataChange #" << mp1 << endl;
        return kTRUE;

    case kTXT_CLICK2:
        cout << "Textview-Click2 #" << mp1 << " x=" << (mp2&0xffff) << " y= " << (mp2>>16) << endl;
        return kTRUE;

    case kTXT_CLICK3:
        cout << "Textview-Click3 #" << mp1 << " x=" << (mp2&0xffff) << " y= " << (mp2>>16) << endl;
        return kTRUE;

    case kTXT_F3:
        cout << "Textview-F3 #" << mp1 << endl;
        return kTRUE;

    case kTXT_OPEN:
        cout << "Textview-Open #" << mp1 << endl;
        return kTRUE;

    case kTXT_CLOSE:
        cout << "Textview-Close #" << mp1 << endl;
        return kTRUE;

    case kTXT_SAVE:
        cout << "Textview-Save #" << mp1 << endl;
        return kTRUE;

    default:
        cout << "Textview: " << "Submsg:" << submsg << " Mp1=" << mp1 << " Mp2=" << mp2 << endl;
    }
#endif
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process the kC_USER messages
//
Bool_t MStatusDisplay::ProcessMessageUser(Long_t submsg, Long_t mp1, Long_t mp2)
{
    // kS_START, case sensitive | backward<<1, char *txt
    switch (submsg)
    {
    case kS_START:
        fLogBox->Search((char*)mp2, !(mp1&2>>1), mp1&1);
        return kTRUE;
#ifdef DEBUG
    default:
        cout << "User: " << "Submsg:" << submsg << " Mp1=" << mp1 << " Mp2=" << mp2 << endl;
#endif
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process the messages from the GUI
//
Bool_t MStatusDisplay::ProcessMessage(Long_t msg, Long_t mp1, Long_t mp2)
{
    // Can be found in WidgetMessageTypes.h
#ifdef DEBUG
    cout << "Msg: " << GET_MSG(msg) << " Submsg:" << GET_SUBMSG(msg);
    cout << " Mp1=" << mp1 << " Mp2=" << mp2 << endl;
#endif
    switch (GET_MSG(msg))
    {
    case kC_COMMAND:  // 1
        return ProcessMessageCommand(GET_SUBMSG(msg), mp1, mp2);

    case kC_TEXTVIEW: // 9
        return ProcessMessageTextview(GET_SUBMSG(msg), mp1, mp2);

    case kC_USER:     // 1001
        return ProcessMessageUser(GET_SUBMSG(msg), mp1, mp2);
    }
#ifdef DEBUG
    cout << "Msg: " << GET_MSG(msg) << " Submsg:" << GET_SUBMSG(msg);
    cout << " Mp1=" << mp1 << " Mp2=" << mp2 << endl;
#endif
    return kTRUE;
}

Bool_t MStatusDisplay::Close()
{
    // Got close message for this MainFrame. Calls parent CloseWindow()
    // (which destroys the window) and terminate the application.
    // The close message is generated by the window manager when its close
    // window menu item is selected.

    // CloseWindow must be overwritten because otherwise CloseWindow
    // and the destructor are calling DestroyWindow which seems to be
    // in conflict with the TRootEmbeddedCanvas.

    // FIXME: Make sure that the Status Display is deleted from every
    //        where (eg Eventloop) first!

    //gLog << dbg << fName << " is on heap: " << (int)IsOnHeap() << endl;

    if (TestBit(kExitLoopOnExit) || TestBit(kExitLoopOnClose))
    {
        //gLog << dbg << "CloseWindow() calling ExitLoop." << endl;
        gSystem->ExitLoop();
    }

    if (fIsLocked<=0 && IsOnHeap())
        return kTRUE;

    fStatus = kFileExit;
    return kFALSE;
}

void MStatusDisplay::CloseWindow()
{
    if (Close())
        delete this;
}

// --------------------------------------------------------------------------
//
// Calls SetBit(kNoContextMenu) for all TCanvas objects found in the
// Tabs.
//
void MStatusDisplay::SetNoContextMenu(Bool_t flag)
{
    if (fIsLocked>1 || gROOT->IsBatch())
        return;

    flag ? SetBit(kNoContextMenu) : ResetBit(kNoContextMenu);

    for (int i=1; i<fTab->GetNumberOfTabs(); i++)
    {
        TCanvas *c = GetCanvas(i);
        if (c)
            flag ? c->SetBit(kNoContextMenu) : c->ResetBit(kNoContextMenu);
    }
}

// --------------------------------------------------------------------------
//
// Update the memory display in the status bar
//
void MStatusDisplay::UpdateMemory() const
{
    const TString path = MString::Format("/proc/%d/status", gSystem->GetPid());
    if (gSystem->AccessPathName(path, kFileExists))
        return;

    TEnv env(path);
    const UInt_t kb = env.GetValue("VmSize", 0);
    if (kb==0)
        return;

    char type = 'k';
    Float_t val = kb;

    if (val>999)
    {
        type = 'M';
        val /= 1000;
    }
    if (val>999)
    {
        type = 'G';
        val /= 1000;
    }
    const TString txt = MString::Format("%.1f%c", val, type);
    fStatusBar->SetText(txt, 2);
}

// --------------------------------------------------------------------------
//
// Updates the canvas (if existing) in the currenly displayed Tab.
// The update intervall is controlled by StartUpdate and StopUpdate
//
Bool_t MStatusDisplay::HandleTimer(TTimer *timer)
{
    if (gROOT->IsBatch())
        return kTRUE;

    UpdateMemory();

    const Int_t c = fTab->GetCurrent();

    // Skip Legend Tab
    if (c==0)
        return kTRUE;

    // Update a canvas tab (if visible)
    if (timer==&fTimer && c!=fLogIdx)
    {
        UpdateTab(fTab->GetCurrentContainer());
        return kTRUE;
    }

    // update the logbook tab (if visible)
    if (timer==&fLogTimer && c==fLogIdx)
    {
        fLog->UpdateGui();

        /*
        if (!fLogBox->TestBit(kHasChanged))
            return kTRUE;

            fLogBox->ResetBit(kHasChanged);
            */
        return kTRUE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Find an object in a canvas (uses MStatusArray as helper)
//
void MStatusDisplay::PrintContent(Option_t *o) const
{
    MStatusArray(*this).Print(o);
}

// --------------------------------------------------------------------------
//
// Find an object in a canvas (uses MStatusArray as helper)
//
TObject *MStatusDisplay::FindObjectInCanvas(const char *obj, const char *base, const char *canv) const
{
    return MStatusArray(*this).FindObjectInCanvas(obj, base, canv);
}

// --------------------------------------------------------------------------
//
// Draws a clone of a canvas into a new canvas. Taken from TCanvas.
//
void MStatusDisplay::DrawClonePad(TCanvas &newc, TCanvas &oldc) const
{
    //copy pad attributes
    newc.Range(oldc.GetX1(),oldc.GetY1(),oldc.GetX2(),oldc.GetY2());
    newc.SetTickx(oldc.GetTickx());
    newc.SetTicky(oldc.GetTicky());
    newc.SetGridx(oldc.GetGridx());
    newc.SetGridy(oldc.GetGridy());
    newc.SetLogx(oldc.GetLogx());
    newc.SetLogy(oldc.GetLogy());
    newc.SetLogz(oldc.GetLogz());
    newc.SetBorderSize(oldc.GetBorderSize());
    newc.SetBorderMode(oldc.GetBorderMode());
    ((TAttLine&)oldc).Copy((TAttLine&)newc);
    ((TAttFill&)oldc).Copy((TAttFill&)newc);
    ((TAttPad&)oldc).Copy((TAttPad&)newc);

    // This must be there: Otherwise GetDrawOption() won't work
    TVirtualPad *padsav = gPad;
    oldc.cd();

    const Bool_t store = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);

    //copy primitives
    TObject *obj;
    TIter next(oldc.GetListOfPrimitives());
    while ((obj=next()))
    {
        // Old line - I think it is not necessary anymore because of the cd()
        //gROOT->SetSelectedPad(&newc);

        // Now make a clone of the object
        TObject *clone = obj->Clone();

        // Clone also important bits (FIXME: Is this correct)
        clone->SetBit(obj->TestBits(kCannotPick|kNoContextMenu));

        // Now make sure that the clones are deleted at a later time
        clone->SetBit(kCanDelete|kMustCleanup);

        // FIXME: This is a workaround for the problem with the MAstroCatalog in
        // MHFalseSource. It doesn't harm. We'll still try to find the reason
        //if (clone->IsA()==TPad::Class())
        //    gROOT->GetListOfCleanups()->Add(clone);

        // Add the clone and its draw-option to the current pad
        TVirtualPad *save2 = gPad;
        gPad = &oldc; // Don't do this before Clone()!
        newc.GetListOfPrimitives()->Add(clone, obj->GetDrawOption());
        gPad = save2;
    }
    newc.Modified();
    newc.Update();

    TH1::AddDirectory(store);

    padsav->cd();
}

// --------------------------------------------------------------------------
//
// Display the contexts of a TObjArray in the display (all canvases)
//
Bool_t MStatusDisplay::Display(const TObjArray &list, const char *tab)
{
    TIter Next(&list);

    TObject *o=Next();
    if (!o)
    {
        *fLog << err << "MStatusDisplay::Display: No entry in TObjArray." << endl;
        return kFALSE;
    }

    // The first entry in the list is a TNamed which is expected to
    // contain the status display title.
    fTitle = o->GetTitle();

    TCanvas *c;
    while ((c=(TCanvas*)Next()))
        //if (!GetCanvas(c->GetName()))
        if (c->InheritsFrom(TCanvas::Class()))
            if (!tab || c->GetName()==(TString)tab)
                DrawClonePad(AddTab(c->GetName(), c->GetTitle()), *c);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Reads the contents of a saved MStatusDisplay from a file.
//
Int_t MStatusDisplay::Read(const char *name, const char *tab)
{
    if (!gFile)
    {
        *fLog << warn << "MStatusDisplay::Read: No file found. Please create a TFile first." << endl;
        return 0;
    }

    if (!gFile->IsOpen())
    {
        *fLog << warn << "MStatusDisplay::Read: File not open. Please open the TFile first." << endl;
        return 0;
    }

    MStatusArray list;

    Int_t n = list.Read(name);

    //
    // If no status display was found with this name try to find canvases
    // in the file and display them instead.
    //
    if (n==0)
    {
        // Either read the title from the file or create our own
        TNamed *title=0;
        gFile->GetObject("Title", title); // FIXME: Is the list allowed to take ownership?
        list.Add(title ? title : new TNamed(GetName(), GetTitle()));

        const Bool_t store = TH1::AddDirectoryStatus();
        TH1::AddDirectory(kFALSE);

        TIter Next(gFile->GetListOfKeys());
        TObject *key = 0;
        while ((key=Next()))
        {
            MStatusArray *arr=0;
            gFile->GetObject(key->GetName(), arr);
            if (arr)
            {
                Display(*arr, tab);
                n++;
                continue;
            }

            if (tab && key->GetName()!=(TString)tab)
                continue;

            TCanvas *c=0;
            gFile->GetObject(key->GetName(), c);

            // If key doesn't correspond to a TCanvas create a new canvas
            // and draw the object itself instead.
            if (!c)
            {
                TObject *obj = gFile->Get(key->GetName());

                if (MParContainer::Overwrites(TObject::Class(), *obj, "Draw") ||
                    MParContainer::Overwrites(TObject::Class(), *obj, "Paint"))
                {
                    AddTab(key->GetName(), key->GetTitle());
                    obj->SetBit(kCanDelete);
                    obj->Draw();
                }
                // FIXME: Is the object deleted?
                continue;
            }

            // Add the canvas to the list
            const TString name  = c->GetName();
            const TString title = c->GetTitle();
            if (title.IsNull() || name==title)
                c->SetTitle(gFile->GetName());
            list.Add(c);
        }

        TH1::AddDirectory(store);

        if (list.GetEntries()<=1)
        {
            *fLog << warn << "MStatusDisplay::Read: No drawable objects read from " << gFile->GetName() << endl;
            return 0;
        }

        *fLog << inf << "MStatusDisplay: " << list.GetEntries()-1 << " canvases directly read from file." << endl;
    }


    if (!Display(list, tab))
    {
        *fLog << err << "MStatusDisplay::Display: No entries found." << endl;
        return 0;
    }

    if (n==0)
        return list.GetEntries();

    *fLog << inf << "MStatusDisplay: Key " << name << " with " << n << " keys read from file." << endl;
    return n;
}

// --------------------------------------------------------------------------
//
// Add all canvases to the MStatusArray
//
void MStatusDisplay::FillArray(MStatusArray &list, Int_t num) const
{
    Int_t from, to;
    GetCanvasRange(from, to, num);

    TCanvas *c;
    for (int i=from; i<to; i++)
        if ((c = GetCanvas(i)))
            list.Add(c);
}

// --------------------------------------------------------------------------
//
// Writes the contents of a MStatusDisplay to a file.
//
Int_t MStatusDisplay::Write(Int_t num, const char *name, Option_t *opt) const
{
    if (!gFile)
    {
        *fLog << warn << "MStatusDisplay::Write: No file found. Please create a TFile first." << endl;
        return 0;
    }

    if (!gFile->IsOpen())
    {
        *fLog << warn << "MStatusDisplay::Write: File not open. Please open the TFile first." << endl;
        return 0;
    }

    if (!gFile->IsWritable())
    {
        *fLog << warn << "MStatusDisplay::Write: File not writable." << endl;
        return 0;
    }

    if (num==0)
    {
        *fLog << warn << "MStatusDisplay::Write: Tab doesn't contain an embedded Canvas... skipped." << endl;
        return 0;
    }

    if (!gROOT->IsBatch() && num>=fTab->GetNumberOfTabs())
    {
        *fLog << warn << "MStatusDisplay::Write: Tab doesn't exist... skipped." << endl;
        return 0;
    }
    if (gROOT->IsBatch() && num>fBatch->GetSize())
    {
        *fLog << warn << "MStatusDisplay::Write: Tab doesn't exist... skipped." << endl;
        return 0;
    }

    Int_t n = 0; // Number of keys written

    TNamed named("Title", fTitle.Data());

    if (TString(opt)=="plain")
    {
        // Get canvas range to store
        Int_t from, to;
        GetCanvasRange(from, to, num);

        n += named.Write();

        TCanvas *c;
        for (int i=from; i<to; i++)
            if ((c = GetCanvas(i)))
                n += c->Write();
    }
    else
    {
        // Be careful: So far Display() assumes that the TNamed
        // is the first entry in the list
        MStatusArray list;
        list.Add(&named);

        FillArray(list, num);

        n += list.Write(name, kSingleKey);
    }

    *fLog << inf << "MStatusDisplay: " << n << " keys written to file as key " << name << "." << endl;

    return n;
}

// --------------------------------------------------------------------------
//
// Use this to start the synchronous (GUI eventloop driven) tab update.
// Can also be used to change the update intervall. If millisec<0
// the intervall given in SetUpdateTime is used. If the intervall in
// SetUpdateTime is <0 nothing is done. (Call SetUpdateTime(-1) to
// disable the automatic update in a MEventloop.
//
void MStatusDisplay::StartUpdate(Int_t millisec)
{
    if (fIsLocked>1)
        return;

    if (fTimer.GetTime()<TTime(0))
        return;
    fTimer.Start(millisec);
}

// --------------------------------------------------------------------------
//
// Stops the automatic GUI update
//
void MStatusDisplay::StopUpdate()
{
    if (fIsLocked>1)
        return;

    fTimer.Stop();
}

// --------------------------------------------------------------------------
//
// Set the update interval for the GUI update, see StartUpdate.
//
void MStatusDisplay::SetUpdateTime(Long_t t)
{
    fTimer.SetTime(t);
}

// --------------------------------------------------------------------------
//
// If the filename name doesn't end with ext, ext is added to the end.
// If name.IsNull() "status" is assumed and the a number (>0) is added
// as "status-6".
// The extension is returned.
//
const TString &MStatusDisplay::AddExtension(TString &name, const TString &ext, Int_t num) const
{
    if (name.IsNull())
    {
        name = gROOT->GetName();
        if (num>0)
        {
            name += "-";
            name += num;
        }
    }

    if (name.EndsWith("."+ext))
        return ext;

    name += ".";
    name += ext;

    return ext;
}

Bool_t MStatusDisplay::CheckTabForCanvas(int num) const
{
    if (gROOT->IsBatch())
        return (num>0 && num<=fBatch->GetSize()) || num<0;

    if (num>=fTab->GetNumberOfTabs())
    {
        *fLog << warn << "Tab #" << num << " doesn't exist..." << endl;
        return kFALSE;
    }
    if (num==0)
    {
        *fLog << warn << "Tab #" << num << " doesn't contain an embedded canvas..." << endl;
        return kFALSE;
    }
    if (fTab->GetNumberOfTabs()<2 || !gPad)
    {
        *fLog << warn << "Sorry, you must have at least one existing canvas (gPad!=NULL)" << endl;
        return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Insert the following two lines into the postscript header:
//
//   %%DocumentPaperSizes: a4
//   %%Orientation: Landscape
//
void MStatusDisplay::UpdatePSHeader(const TString &name) const
{
    const TString newstr("%%DocumentPaperSizes: a4\n%%Orientation: Landscape\n");

    TString tmp(name+"XXXXXX");

    // FIXME: Use mkstemp instead
    if (!mkstemp(const_cast<char*>(tmp.Data())))
    {
        *fLog << err << "ERROR - MStatusDisplay::UpdatePSHeader: mktemp failed." << endl;
        return;
    }

    ifstream fin(name);
    ofstream fout(tmp);
    if (!fout)
    {
        *fLog << err << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return;
    }

    char c;

    TString str;
    fin >> str >> c;                // Read "%!PS-Adobe-2.0\n"
    fout << str << endl << newstr;

    // Doing it in blocks seems not to gain much for small (MB) files
    while (fin)
    {
        fin.read(&c, 1);
        fout.write(&c, 1);
    }

    gSystem->Unlink(name);
    gSystem->Rename(tmp, name);
}

// --------------------------------------------------------------------------
//
void MStatusDisplay::PSToolsRange(TVirtualPS &vps, Float_t psw, Float_t psh) const
{
    if (vps.InheritsFrom(TPostScript::Class()))
        static_cast<TPostScript&>(vps).Range(psw, psh);
    // if (vps.InheritsFrom(TPDF::Class()))
    //     static_cast<TPDF&>(vps).Range(psw*0.69, psh*0.69);
    // if (vps.InheritsFrom(TSVG::Class()))
    //     static_cast<TSVG&>(vps).Range(psw, psh);
}

// --------------------------------------------------------------------------
//
void MStatusDisplay::PSToolsTextNDC(TVirtualPS &vps, Double_t u, Double_t v, const char *string) const
{
    if (vps.InheritsFrom(TPostScript::Class()))
        static_cast<TPostScript&>(vps).TextNDC(u, v, string);
    if (vps.InheritsFrom(TPDF::Class()))
        static_cast<TPDF&>(vps).TextNDC(u, v, string);
    // if (vps.InheritsFrom(TSVG::Class()))
    //    static_cast<TSVG&>(vps).TextNDC(u, v, string);
}

// --------------------------------------------------------------------------
//
Int_t MStatusDisplay::InitWriteDisplay(Int_t num, TString &name, const TString &ext)
{
    SetStatusLine1(MString::Format("Writing %s file...",ext.Data()));
    SetStatusLine2("Please be patient!");

    if (!CheckTabForCanvas(num))
    {
        SetStatusLine2("Failed!");
        return 0;
    }

    AddExtension(name, ext, num);

    if (num<0)
        *fLog << inf << "Open " << ext << "-File: " << name << endl;

    return num;
}

// --------------------------------------------------------------------------
//
TCanvas *MStatusDisplay::InitWriteTab(Int_t num, TString &name)
{
    const Int_t i = TMath::Abs(num);

    TCanvas *c = GetCanvas(i);
    if (!c)
    {
        if (num<0)
            *fLog << inf << " - ";
        *fLog << "Tab #" << i << " doesn't contain an embedded Canvas... skipped." << endl;
        return 0;
    }

    SetStatusLine2(MString::Format("Tab #%d", i));

    //
    // Paint canvas into root file
    //
    if (num<0 && !name.IsNull())
    {
        Bool_t found = kFALSE;
        if (name.Index("%%%%name%%%%")>=0)
        {
            name.ReplaceAll("%%name%%", c->GetName());
            found = kTRUE;
        }

        if (name.Index("%%%%title%%%%")>=0)
        {
            name.ReplaceAll("%%title%%", c->GetTitle());
            found = kTRUE;
        }

        if (name.Index("%%%%tab%%%%")>=0)
        {
            name.ReplaceAll("%%tab%%", MString::Format("%d", i));
            found = kTRUE;
        }

        if (!found)
            name.Insert(name.Last('.'), MString::Format("-%d", i));
    }

    if (num<0)
        *fLog << inf << " - ";
    *fLog << inf << "Writing Tab #" << i;

    if (!name.IsNull())
        *fLog << " to " << name;

    *fLog << ": " << c->GetName() << "... " << flush;

    return c;
}

// This is a stupid workaround to get rid of the damned clipping
// of the text. Who the hell needs clipping?
class MyCanvas : public TCanvas
{
public:
    void Scale(Double_t val)
    {
        fAbsXlowNDC = -val;
        fAbsYlowNDC = -val;
        fAbsWNDC    = 1+2*val;
        fAbsHNDC    = 1+2*val;
    }
};

// --------------------------------------------------------------------------
//
// Write some VGF (vector graphics format). Currently PS, PDF and SVG
// is available. Specified by ext.
//
// In case of num<0 all tabs are written into the VGF file. If num>0
// the canvas in the corresponding tab is written to the file.
// Name is the name of the file (with or without extension).
//
// Returns the number of pages written.
//
// To write all tabs you can also use SaveAsVGF(name, ext)
//
// If the third argument is given a bottom line is drawn with the text
// under it. If no argument is given a bottom line is drawn if
// fTitle (SetTitle) is not empty.
//
Int_t MStatusDisplay::SaveAsVGF(Int_t num, TString name, const TString addon, const TString ext)
{
    num = InitWriteDisplay(num, name, ext);
    if (num==0)
        return 0;

    TPad       *padsav = (TPad*)gPad;
    TVirtualPS *psave  = gVirtualPS;

    TDatime d;

    Int_t type = -1;

    TVirtualPS *ps =0;
    if (!ext.CompareTo("ps", TString::kIgnoreCase))
    {
        gStyle->SetColorModelPS(1);
        ps = new TPostScript(name, 112);
        type = 1;
    }
    if (!ext.CompareTo("pdf", TString::kIgnoreCase))
    {
        ps = new TPDF(name, 112);
        type = 2;
    }
    if (!ext.CompareTo("svg", TString::kIgnoreCase))
    {
        ps = new TSVG(name, 112);
        type = 3;
    }

    if (!ps)
    {
        *fLog << err << "Extension " << ext << " unknown..." << endl;
        SetStatusLine2("Failed!");
        return 0;
    }

    ps->SetBit(TPad::kPrintingPS);
    if (type==1)
        ps->PrintFast(13, "/nan {1} def ");

    gVirtualPS = ps;

    //
    // Create some GUI elements for a page legend
    //
    TLine line;

    int page = 1;

    //
    // Maintain tab numbers
    //
    Int_t from, to;
    GetCanvasRange(from, to, num);

    for (int i=from; i<to; i++)
    {
        TCanvas *c = InitWriteTab(num<0?-i:i);
        if (c==0)
            continue;

        //
        // Init page and page size, make sure, that the canvas in the file
        // has the same Aspect Ratio than on the screen.
        //
        if (type==1 || i>from)
            ps->NewPage();

        //
        // 28 is used here to scale the canvas into a height of 28,
        // such that the page title can be set above the canvas...
        //
        Float_t psw = 28.0; // A4 - width  (29.7)
        Float_t psh = 21.0; // A4 - height (21.0)

        const Float_t cw = c->GetWw();
        const Float_t ch = c->GetWh();

        if (psw/psh>cw/ch)
            psw = cw/ch*psh;
        else
            psh = ch/cw*psw;

        PSToolsRange(*ps, psw, psh);

        //
        // Clone canvas and change background color and schedule for
        // deletion
        //

        //const Bool_t store = c->IsBatch();
        //c->SetBatch(kTRUE);
        c->Paint();
        //c->SetBatch(store);

        //
        // Change/fix the canvas coordinate system for the overlaying text.
        // This is necessary because root clip everything away which is
        // outside a predefined area, which is (0,0)/(1,1)
        //
        const Double_t height = 0.015;  // Text height
        const Double_t off    = 0.005;  // Line offset from text

        const Double_t bot = height+off;
        const Double_t top = 1-bot;

        static_cast<MyCanvas*>(c)->Scale(bot);

        // If gPad is not set to c all follwing commands will
        // get the wrong numbers for alignment
        gPad = c;

        // Separator Lines
        line.PaintLineNDC(0.01, top, 0.99, top);
        line.PaintLineNDC(0.01, bot, 0.99, bot);

        //
        // Print overlaying text (NDC = %)
        //
        // align phi col font size (11=left top)
        const TString txt(addon.IsNull() ? fTitle : addon);

        // Text Attributes
        TAttText(11, 0, kBlack, 22, height).Copy(*ps);

        // Text on top
        ps->SetTextAlign(11); // left bottom
        PSToolsTextNDC(*ps, 0.01, top+off, c->GetName());

        ps->SetTextAlign(21); // cent bottom
        PSToolsTextNDC(*ps, 0.50, top+off, TString("MARS V" MARSVER " - Modular Analysis and Reconstruction Software - ")+d.AsString());

        ps->SetTextAlign(31); // right bottom
        PSToolsTextNDC(*ps, 0.99, top+off, MString::Format("Page No.%i (%i)", page++, i));

        // Text on bottom
        ps->SetTextAlign(13); // left top
        PSToolsTextNDC(*ps, 0.01, bot-off, c->GetTitle());

        ps->SetTextAlign(23); // cent top
        PSToolsTextNDC(*ps, 0.50, bot-off, txt);

        ps->SetTextAlign(33); // right top
        PSToolsTextNDC(*ps, 0.99, bot-off, MString::Format("(c) 2000-%d, Thomas Bretz", TDatime().GetYear()));

        static_cast<MyCanvas*>(c)->Scale(0);

        //
        // Finish drawing page
        //
        *fLog << "done." << endl;
    }

    gPad = NULL; // Important!

    ps->Close();
    delete ps;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    if (type==1)
    {
        SetStatusLine2("Updating header of PS file...");

        if (num<0)
            *fLog << inf3 << " - Updating header of PS file... " << flush;
        UpdatePSHeader(name);
        if (num<0)
            *fLog << inf3 << "done." << endl;
    }
#endif

    gVirtualPS = psave;
    if (padsav)
        padsav->cd();

    if (num<0)
        *fLog << inf << "done." << endl;

    SetStatusLine2(MString::Format("Done (%dpages)", page-1));

    return page-1;
}

// --------------------------------------------------------------------------
//
Bool_t MStatusDisplay::SaveAsImage(Int_t num, TString name, TImage::EImageFileTypes type)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    if (gROOT->IsBatch())
    {
        *fLog << warn << "Sorry, writing image-files is not available in batch mode." << endl;
        return 0;
    }
#endif

    TString ext;
    switch (type)
    {
    case TImage::kXpm:
    case TImage::kZCompressedXpm:  ext = "xpm";     break;
    case TImage::kPng:             ext = "png";     break;
    case TImage::kJpeg:            ext = "jpg";     break;
    case TImage::kGif:             ext = "gif";     break;
    case TImage::kTiff:            ext = "tiff";    break;
    case TImage::kBmp:             ext = "bmp";     break;
    case TImage::kXml:             ext = "xml";     break;
    //case TImage::kGZCompressedXpm: ext = "xpm.gz";  break;
    //case TImage::kPpm:             ext = "ppm";     break;
    //case TImage::kPnm:             ext = "pnm";     break;
    //case TImage::kIco:             ext = "ico";     break;
    //case TImage::kCur:             ext = "cur";     break;
    //case TImage::kXcf:             ext = "xcf";     break;
    //case TImage::kXbm:             ext = "xbm";     break;
    //case TImage::kFits:            ext = "fits";    break;
    //case TImage::kTga:             ext = "tga";     break;
    default:
        *fLog << warn << "Sorry, unknown or unsupported file type..." << endl;
        return 0;
    }

    num = InitWriteDisplay(num, name, ext);
    if (num==0)
        return 0;

    TPad *padsav = (TPad*)gPad;

    Int_t counter = 0;

    //
    // Maintain tab numbers
    //
    Int_t from, to;
    GetCanvasRange(from, to, num);

    for (int i=from; i<to; i++)
    {
        TString writename(name);

        TCanvas *c = InitWriteTab(num<0 ? -i : i, writename);
        if (!c)
            continue;

        //
        // Paint canvas into root file
        //

        // TImage *img = TImage::Create();
        // img->FromPad(c);
        // img->WriteImage(writename, type);
        // delete img;

        // FIXME: Not all file types are supported by Print()
        c->Print(writename);

        if (num<0)
            *fLog << "done." << endl;

        counter++;
    }

    if (padsav)
        padsav->cd();

    *fLog << inf << "done." << endl;

    SetStatusLine2("Done.");

    return counter>0;
}

// --------------------------------------------------------------------------
//
Bool_t MStatusDisplay::SaveAsC(Int_t num, TString name)
{
    num = InitWriteDisplay(num, name, "C");
    if (num==0)
        return kFALSE;

    TPad *padsav = (TPad*)gPad;

    Int_t counter = 0;

    //
    // Maintain tab numbers
    //
    Int_t from, to;
    GetCanvasRange(from, to, num);

    for (int i=from; i<to; i++)
    {
        TString writename(name);

        TCanvas *c = InitWriteTab(num<0 ? -i : i, writename);
        if (!c)
            continue;

        //
        // Clone canvas and change background color and schedule for
        // deletion
        //
        c->SaveSource(writename, "");

        if (num<0)
            *fLog << "done." << endl;

        counter++;
    }

    if (padsav)
        padsav->cd();

    *fLog << inf << "done." << endl;

    SetStatusLine2("Done.");

    return counter>0;
}

// --------------------------------------------------------------------------
//
// In case of num<0 all tabs are written into a root file. As plain
// TCanvas objects if plain==kTRUE otherwise in a MStatusArray. If num>0
// the canvas in the corresponding tab is written to the file.
// Name is the name of the file (with or without extension).
//
// Returns the number of keys written.
//
// To write all tabs you can also use SaveAsRoot(name)
//
Int_t MStatusDisplay::SaveAsRoot(Int_t num, TString name, Bool_t plain)
{
    num = InitWriteDisplay(num, name, "root");
    if (num==0)
        return -1;

    TFile *fsave = gFile;
    TFile file(name, "RECREATE", GetTitle(), 9);
    const Int_t keys = Write(num, "MStatusDisplay", plain ? "plain" : "");
    gFile = fsave;

    SetStatusLine2("Done.");

    return keys;
}

// --------------------------------------------------------------------------
//
Bool_t MStatusDisplay::SaveAsCSV(Int_t num, TString name, Char_t delim)
{
    num = InitWriteDisplay(num, name, "csv");
    if (num==0)
        return kFALSE;

    gSystem->ExpandPathName(name);

    ofstream fout(name);
    if (!fout)
    {
        *fLog << err << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return kFALSE;
    }

    fout << 0 << delim << GetName() << delim << GetTitle() << endl;

    Int_t from, to;
    GetCanvasRange(from, to, num);

    for (int i=from; i<to; i++)
    {
        TCanvas *c;
        if (!(c = GetCanvas(i)))
        {
            if (num<0)
                *fLog << inf << " - ";
            *fLog << "Tab #" << i << " doesn't contain an embedded Canvas... skipped." << endl;
            continue;
        }

        fout << i << delim << c->GetName() << delim << c->GetTitle() << endl;
    }

    SetStatusLine2("Done.");

    return kTRUE;
}

/*
Bool_t MStatusDisplay::SaveAsCSV(Int_t num, TString name)
{
    num = InitWriteDisplay(num, name, "csv");
    if (num==0)
        return kFALSE;

    gSystem->ExpandPathName(name);

    ofstream fout(name);
    if (!fout)
    {
        *fLog << err << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return kFALSE;
    }

    fout << "<?xml version=\"1.0\"?>" << endl;
    fout << "<display name='" << GetName() << "'>" << endl;
    fout << "   <file>" << name << "</file>" << endl;
    fout << "   <status>" << endl;
    fout << "      <name>" << GetName() << "</name>" << endl;
    fout << "      <title>" << GetTitle() << "</title>" << endl;
    fout << "   </status>" << endl;
    fout << "   <tabs>" << endl;

    fout << 0 << delim << GetName() << delim << GetTitle() << endl;

    Int_t from, to;
    GetCanvasRange(from, to, num);

    for (int i=from; i<to; i++)
    {
        TCanvas *c;
        if (!(c = GetCanvas(i)))
        {
            if (num<0)
                *fLog << inf << " - ";
            *fLog << "Tab #" << i << " doesn't contain an embedded Canvas... skipped." << endl;
            continue;
        }

        fout << "      <tab index='" << i << "'>" << endl;
        fout << "         <index>" << i << "</index>" << endl;
        fout << "         <name>" << c->GetName()  << "</name>" << endl;
        fout << "         <title>" << c->GetName()  << "</title>" << endl;
        fout << "      </tab>" << endl;
    }

    fout << "  </tabs>" << endl;
    fout << "</display>" << endl;

    SetStatusLine2("Done.");

    return kTRUE;
}
*/

// --------------------------------------------------------------------------
//
void MStatusDisplay::SaveAs(const char *c, const Option_t *o) const
{
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,18,00)
    TGObject::SaveAs(c, o);
#endif
}

// --------------------------------------------------------------------------
//
// Determin File type to save file as by extension. Allowed extensions are:
//   root, ps, pdf, svg, gif, png, jpg, xpm, C
//
// returns -1 if file type is unknown. Otherwise return value of SaveAs*
//
Int_t MStatusDisplay::SaveAs(Int_t num, TString name)
{
    if (name.EndsWith(".root")) return SaveAsRoot(num, name); // kFileSaveAsRoot
    if (name.EndsWith(".ps"))   return SaveAsPS(num, name);   // kFileSaveAsPS
    if (name.EndsWith(".pdf"))  return SaveAsPDF(num, name);  // kFileSaveAsPDF
    if (name.EndsWith(".svg"))  return SaveAsSVG(num, name);  // kFileSaveAsSVG
    if (name.EndsWith(".gif"))  return SaveAsGIF(num, name);  // kFileSaveAsGIF
    if (name.EndsWith(".png"))  return SaveAsPNG(num, name);  // kFileSaveAsPNG
    if (name.EndsWith(".bmp"))  return SaveAsBMP(num, name);  // kFileSaveAsBMP
    if (name.EndsWith(".xml"))  return SaveAsXML(num, name);  // kFileSaveAsXML
    if (name.EndsWith(".jpg"))  return SaveAsJPG(num, name);  // kFileSaveAsJPG
    if (name.EndsWith(".xpm"))  return SaveAsXPM(num, name);  // kFileSaveAsXPM
    if (name.EndsWith(".csv"))  return SaveAsCSV(num, name);  // kFileSaveAsCSV
    if (name.EndsWith(".tiff")) return SaveAsTIFF(num, name); // kFileSaveAsTIFF
    if (name.EndsWith(".C"))    return SaveAsC(num, name);    // kFileSaveAsC
    return -1;
}

// --------------------------------------------------------------------------
//
//  Opens a save as dialog
//
Int_t MStatusDisplay::SaveAs(Int_t num)
{
    static const char *gSaveAsTypes[] =
    {
        "PostScript",   "*.ps",
        "Acrobat pdf",  "*.pdf",
        "SVG vector",   "*.svg",
        "Gif files",    "*.gif",
        "Png files",    "*.png",
        "Gif files",    "*.gif",
        "Jpeg files",   "*.jpeg",
        "Xpm files",    "*.xpm",
        "Bmp files",    "*.bmp",
        "Xml files",    "*.xml",
        "Tiff files",   "*.tiff",
        "Csv files",    "*.csv",
        "Macro files",  "*.C",
        "ROOT files",   "*.root",
        "All files",    "*",
        NULL,           NULL
    };

    static TString dir(".");

    TGFileInfo fi; // fFileName and fIniDir deleted in ~TGFileInfo

    fi.fFileTypes = (const char**)gSaveAsTypes;
    fi.fIniDir    = StrDup(dir);

    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);

    if (!fi.fFilename)
        return 0;

    dir = fi.fIniDir;

    const Int_t rc = SaveAs(num, fi.fFilename);
    if (rc>=0)
        return rc;

    Warning("MStatusDisplay::SaveAs", "Unknown Extension: %s", fi.fFilename);
    return 0;
}

// --------------------------------------------------------------------------
//
//  Open contents of a MStatusDisplay with key name from file fname.
//
Int_t MStatusDisplay::Open(TString fname, const char *name)
{
    TFile file(fname, "READ");
    if (file.IsZombie())
    {
        gLog << warn << "WARNING - Cannot open file " << fname << endl;
        return 0;
    }

    return Read(name);
}

// --------------------------------------------------------------------------
//
//  Opens an open dialog
//
Int_t MStatusDisplay::Open()
{
    static const char *gOpenTypes[] =
    {
        "ROOT files", "*.root",
        "All files",  "*",
        NULL,           NULL
    };

    static TString dir(".");

    TGFileInfo fi; // fFileName and fIniDir deleted in ~TGFileInfo

    fi.fFileTypes = (const char**)gOpenTypes;
    fi.fIniDir    = StrDup(dir);

    new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);

    if (!fi.fFilename)
        return 0;

    dir = fi.fIniDir;

    return Open(fi.fFilename);
}

// --------------------------------------------------------------------------
//
//  Change width of display. The height is calculated accordingly.
//
void MStatusDisplay::SetDisplayWidth(UInt_t dw)
{
    if (gROOT->IsBatch())
    {
        SetCanvasWidth(dw);
        return;
    }

    // 4 == 2*default border with of canvas
    dw -= 4;

    // Difference between canvas size and display size
    const UInt_t cw = GetWidth() -fTab->GetWidth();
    const UInt_t ch = GetHeight()-fTab->GetHeight()+fTab->GetTabHeight();

    const UInt_t dh = TMath::Nint((dw - cw)/1.5 + ch);

    Resize(dw, dh); // Set display size
}

// --------------------------------------------------------------------------
//
//  Change height of display. The width is calculated accordingly.
//
void MStatusDisplay::SetDisplayHeight(UInt_t dh)
{
    if (gROOT->IsBatch())
    {
        SetCanvasHeight(dh);
        return;
    }

    // 4 == 2*default border with of canvas
    dh -= 4;

    // Difference between canvas size and display size
    const UInt_t cw = GetWidth() -fTab->GetWidth();
    const UInt_t ch = GetHeight()-fTab->GetHeight()+fTab->GetTabHeight();

    const UInt_t dw = TMath::Nint((dh - ch)*1.5 + cw);

    Resize(dw, dh); // Set display size
}

// --------------------------------------------------------------------------
//
//  Change width of canvas. The height is calculated accordingly.
//
void MStatusDisplay::SetCanvasWidth(UInt_t w)
{
    // 4 == 2*default border with of canvas
    w += 4;

    if (gROOT->IsBatch())
    {
        Resize(w, 3*w/2);
        return;
    }

    // Difference between canvas size and display size
    const UInt_t cw = GetWidth() -fTab->GetWidth();
    const UInt_t ch = GetHeight()-fTab->GetHeight()+fTab->GetTabHeight();

    const UInt_t h  = TMath::Nint(w/1.5 + ch);

    Resize(w + cw, h); // Set display size
}

// --------------------------------------------------------------------------
//
//  Change height of canvas. The width is calculated accordingly.
//
void MStatusDisplay::SetCanvasHeight(UInt_t h)
{
    // 4 == 2*default border with of canvas
    h += 4;

    if (gROOT->IsBatch())
    {
        Resize(2*h/3, h);
        return;
    }

    // Difference between canvas size and display size
    const UInt_t cw = GetWidth() -fTab->GetWidth();
    const UInt_t ch = GetHeight()-fTab->GetHeight()+fTab->GetTabHeight();

    // 4 == 2*default border with of canvas
    const UInt_t dw  = TMath::Nint((h+4)*1.5 + cw);

    Resize(dw, h + ch); // Set display size
}

// --------------------------------------------------------------------------
//
// Calculate width and height of the display such that it fits into the
// defined box.
//
void MStatusDisplay::SetDisplaySize(UInt_t w, UInt_t h)
{
    if (gROOT->IsBatch())
        return;

    SetDisplayHeight(h);

    if (GetWidth()>w)
        SetDisplayWidth(w);
}

// --------------------------------------------------------------------------
//
//  Calculate an optimum size for the display from the desktop size
//
void MStatusDisplay::SetOptimumSize()
{
    if (gROOT->IsBatch())
        return;

    const UInt_t w = TMath::Nint(0.95*gClient->GetDisplayWidth());
    const UInt_t h = TMath::Nint(0.95*gClient->GetDisplayHeight());

    SetDisplaySize(w, h);
}


Bool_t MStatusDisplay::HandleConfigureNotify(Event_t *evt)
{
    //
    // The initialization of the GUI is not yet enough finished...
    //
    if (!fTab)
        return kTRUE;

    UInt_t w = evt->fWidth;
    UInt_t h = evt->fHeight;

    const Bool_t wchanged = w!=GetWidth()-fTab->GetWidth();
    const Bool_t hchanged = h!=GetHeight()-fTab->GetHeight();

    if (!wchanged && !hchanged)
    {
        Layout();
        // FIXME: Make sure that this doesn't result in endless loops.
        return kTRUE;
    }

    if (GetWidth()==1 && GetHeight()==1)
        return kTRUE;

    // calculate the constant part of the window
    const UInt_t cw = GetWidth() -fTab->GetWidth();
    const UInt_t ch = GetHeight()-fTab->GetHeight()+fTab->GetTabHeight();

    // calculate new size of frame (canvas @ 2:3)
    if (hchanged)
        w = TMath::Nint((h-ch)*1.5+cw);
    else
        h = TMath::Nint((w-cw)/1.5+ch);

    // resize frame
    Resize(w, h);

    return kTRUE;
}

Bool_t MStatusDisplay::HandleEvent(Event_t *event)
{
    // Instead  of doing this in CloseWindow (called from HandleEvent)
    // we do it here. This makes sure, that handle event doesn't
    // execute code after deleting this.
    if (event->fType==kDestroyNotify)
    {
        if (Close())
            delete this;
//        Close();
        return kTRUE;
    }

    const Bool_t rc = TGMainFrame::HandleEvent(event);

    //
    // This fixes a bug in older root versions which makes
    // TCanvas crash if gPad==NULL. So we make sure, that
    // gPad!=NULL -- be carfull, this may have other side
    // effects.
    //
#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,01)
    if (!gPad && fTab)
        for (int i=0; i<fTab->GetNumberOfTabs(); i++)
        {
            TCanvas *c = GetCanvas(i);
            if (c)
            {
                c->cd();
                gLog << dbg << "MStatusDisplay::HandleEvent - Workaround: gPad=" << gPad << "." << endl;
                break;
            }
        }
#endif

    return rc;
}
