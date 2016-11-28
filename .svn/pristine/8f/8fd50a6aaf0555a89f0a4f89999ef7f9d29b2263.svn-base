#ifndef MARS_MStatusDisplay
#define MARS_MStatusDisplay

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif

#ifndef ROOT_TTimer
#include <TTimer.h>
#endif

#ifndef ROOT_TImage
#include <TImage.h>
#endif

class MLog;
class MGList;
class MGMenuBar;
class MStatusArray;
class MParContainer;

class TPad;
class TTimer;
class TMutex;
class TCanvas;
class TVirtualPS;

class TGTab;
class TGTextView;
class TGStatusBar;
class TGPopupMenu;
class TGTabElement;
class TGProgressBar;
class TGHProgressBar;
class TGCompositeFrame;
class TRootEmbeddedCanvas;

class MStatusDisplay : public TGMainFrame
{
    friend class MStatusArray;
public:
    typedef enum {
        // kFile
        kFileBrowser, kFileCanvas, kFileTab, kFileOpen, kFileSave, kFileSaveAs,
        kFileSaveAsPS, kFileSaveAsPDF, kFileSaveAsSVG, kFileSaveAsRoot,
        kFileSaveAsPNG, kFileSaveAsGIF, kFileSaveAsJPG, kFileSaveAsXPM,
        kFileSaveAsBMP, kFileSaveAsCSV, kFileSaveAsTIFF, kFileSaveAsPlainRoot,
        kFileSaveAsXML, kFileSaveAsC, kFilePrint, kFilePrinterName,
        kFileClose, kFileExit, kFileReset,
        // kLoop
        kLoopNone, kLoopPause, kLoopStep, kLoopStop,
        // kTab
        kTabSave, kTabSaveAs, kTabSaveAsPS, kTabSaveAsPDF, kTabSaveAsSVG,
        kTabSaveAsRoot, kTabSaveAsPNG, kTabSaveAsGIF, kTabSaveAsJPG,
        kTabSaveAsXPM, kTabSaveAsBMP, kTabSaveAsCSV, kTabSaveAsTIFF,
        kTabSaveAsXML, kTabSaveAsC, kTabSaveAsPlainRoot,
        kTabPrint, kTabNext, kTabPrevious, kTabRemove,
        // kSize
        kSize640, kSize768, kSize800, kSize960, kSize1024, kSize1152,
        kSize1280, kSize1400, kSize1600, kSizeOptimum,
        // kLog
        kLogCopy, kLogClear, kLogSelect, kLogFind, kLogSave, kLogAppend,
        kLogPrint,
        // kPic
        kPicMagic, kPicMars,
        // kGui
        kSearch, kTabs,
        // kLastElement
        kLastElement
    } Status_t;

    enum
    {
        // TGMainFrame::kDontCallClose = BIT(14)
        kExitLoopOnExit  = BIT(15),
        kExitLoopOnClose = BIT(16)
     };

protected:
    TString fName;  // status display identifier (name) (gROOT->FindObject())
    TString fTitle; // status display title

    MLog             *fLog;
    MGList           *fList;
    TGCompositeFrame *fUserFrame;

    Bool_t ProcessMessage(Long_t msg, Long_t mp1, Long_t mp2);

private:
    TGHProgressBar   *fBar;
    TGTab            *fTab;
    TGLayoutHints    *fLayCanvas;

    TTimer  fTimer;
    TMutex *fMutex;

    MGMenuBar   *fMenuBar;
    TGStatusBar *fStatusBar;

    Status_t fStatus;

    Int_t fLogIdx;
    TTimer fLogTimer;
    TGTextView *fLogBox;

    FontStruct_t fFont;

    UInt_t fIsLocked;

    TList *fBatch;          //!

    // GUI setup
    void AddMenuBar();
    void AddUserFrame();
    void AddTabs();
    void AddProgressBar();
    void AddStatusBar();
    void AddMarsTab();
    void AddLogTab();

    // GUI event handling
    Bool_t ProcessMessageCommandMenu(Long_t mp1);
    Bool_t ProcessMessageCommand(Long_t submsg, Long_t mp1, Long_t mp2);
    Bool_t ProcessMessageTextview(Long_t submsg, Long_t mp1, Long_t mp2);
    Bool_t ProcessMessageUser(Long_t submsg, Long_t mp1, Long_t mp2);
    Bool_t Close();
    void   CloseWindow();
    Bool_t HandleConfigureNotify(Event_t *);

    // Access to the tabs
    TGCompositeFrame *GetTabContainer(const char *name) const;
    TGTabElement     *GetTabTab(const char *name) const;

    // Display update
    Bool_t HandleTimer(TTimer *timer=NULL);
    void UpdateTab(TGCompositeFrame *f);
    void UpdateMemory() const;

    // Display helper functions
    void RemoveTab(int i);
    void SetStatusLine(const char *txt, Int_t idx);

    // Menu helper functions
    TGPopupMenu *GetPopup(const char *name);

    // Drawing helper
    void DrawClonePad(TCanvas &newc, TCanvas &oldc) const;
    void GetCanvasRange(Int_t &from, Int_t &to, Int_t num=-1) const;

    // MStatusArray interface
    Bool_t Display(const TObjArray &list, const char *tab=0);
    void FillArray(MStatusArray &list, Int_t num=-1) const;

    // I/O helper function
    const TString &AddExtension(TString &name, const TString &ext, Int_t num=-1) const;

    TCanvas *InitWriteTab(Int_t num, TString &name);
    TCanvas *InitWriteTab(Int_t num) { TString s; return InitWriteTab(num, s); }
    Int_t    InitWriteDisplay(Int_t num, TString &name, const TString &ext);

    void UpdatePSHeader(const TString &name) const;

    Bool_t SaveAsImage(Int_t num, TString name, TImage::EImageFileTypes type);
    Int_t  SaveAsVGF(Int_t num, TString name, const TString addon, const TString ext);

    void PSToolsRange(TVirtualPS &vps, Float_t w, Float_t h) const;
    void PSToolsTextNDC(TVirtualPS &vps, Double_t u, Double_t v, const char *string) const;
    TString PrintDialog(TString &p, TString &c, TString &t, const char *ext=0);

public:
     MStatusDisplay(Int_t w=-1, Int_t h=-1, Long_t t=1000);
     virtual ~MStatusDisplay();

     // Name and title handling
     virtual void SetName(const char *name) { fName = name; }
     virtual void SetTitle(const char *title="") { SetTitle(title, kTRUE); }
     virtual const char *GetName() const { return fName.Data(); }
     virtual const char *GetTitle() const { return fTitle.Data(); }

     virtual void SetTitle(const char *title, Bool_t replace)
     {
         if (fTitle.IsNull() || replace) fTitle = title;
     }

     // Getter / display access
     Bool_t   HasCanvas(const TCanvas *c) const;
     TCanvas *GetCanvas(int i) const;
     TCanvas *GetCanvas(const TString &name) const;
     TVirtualPad *GetFullPad(const Int_t canvas, const Int_t pad);

     Bool_t CdCanvas(const TString &name);
     Bool_t CheckTabForCanvas(int num) const;

     TObject *FindObjectInCanvas(const char *obj, const char *base, const char *canv) const;
     TObject *FindObjectInCanvas(const char *obj, const char *canv) const { return FindObjectInCanvas(obj, obj, canv); }
     TObject *FindObject(const char *obj, const char *base) const { return FindObjectInCanvas(obj, base, 0); }
     TObject *FindObject(const char *obj) const { return FindObjectInCanvas(obj, obj, 0); }
     TObject *FindObject(const TObject *) const { return 0;  }

     Int_t GetNumTabs() const;

     void PrintContent(Option_t *o="") const;

     // Display Manipulation
     TCanvas &AddTab(const char *name, const char *title=0);
     TGCompositeFrame *AddRawTab(const char *name);

     void SetProgressBarPosition(Float_t p, Bool_t upd=kFALSE);
     TGProgressBar *GetBar() const { return (TGProgressBar*)fBar; }

     void SetStatusLine1(const char *txt) { SetStatusLine(txt, 0); }
     void SetStatusLine2(const char *txt) { SetStatusLine(txt, 1); }
     void SetStatusLine2(const MParContainer &cont);

     void Reset();
     void SetNoContextMenu(Bool_t flag=kTRUE);

     // Update handling
     void StartUpdate(Int_t millisec=-1);
     void StopUpdate();
     void SetUpdateTime(Long_t t);

     void Update() { HandleTimer(&fTimer); HandleTimer(&fLogTimer); }

     // Log stream interface
     void SetLogStream(MLog *log, Bool_t enable=kFALSE);

     // Size options
     void SetCanvasWidth(UInt_t w);
     void SetCanvasHeight(UInt_t h);

     void SetDisplayWidth(UInt_t w);
     void SetDisplayHeight(UInt_t h);

     void SetOptimumSize();
     void SetDisplaySize(UInt_t w, UInt_t h);

     // Graphics output
     Int_t  SaveAsPS(TString name="",  const TString addon="") { return SaveAsVGF(-1, name, addon, "ps"); }
     Int_t  SaveAsPDF(TString name="", const TString addon="") { return SaveAsVGF(-1, name, addon, "pdf"); }
     Int_t  SaveAsSVG(TString name="", const TString addon="") { return SaveAsVGF(-1, name, addon, "svg"); }
     Bool_t SaveAsPNG(TString name="")  { return SaveAsPNG(-1, name); }
     Bool_t SaveAsGIF(TString name="")  { return SaveAsGIF(-1, name); }
     Bool_t SaveAsXPM(TString name="")  { return SaveAsXPM(-1, name); }
     Bool_t SaveAsJPG(TString name="")  { return SaveAsJPG(-1, name); }
     Bool_t SaveAsTIFF(TString name="") { return SaveAsTIFF(-1, name); }
     Bool_t SaveAsBMP(TString name="")  { return SaveAsBMP(-1, name); }
     Bool_t SaveAsXML(TString name="")  { return SaveAsXML(-1, name); }
     Bool_t SaveAsC(TString name="")    { return SaveAsC(-1, name); }
     Int_t  SaveAsRoot(TString name="") { return SaveAsRoot(-1, name); }
     Int_t  SaveAsPlainRoot(TString name="") { return SaveAsRoot(-1, name, kTRUE); }
     Int_t  SaveAs(TString name)        { return SaveAs(-1, name); }

     Bool_t SaveAsCSV(TString name="", Char_t delim='\t') { return SaveAsCSV(-1, name, delim); }
     Int_t  PrintPS() { return PrintPS(-1); }

     void   SaveAs(const char *c, const Option_t *o) const;

     Int_t  SaveAsPS(Int_t num, TString name="",  const TString addon="") { return SaveAsVGF(num, name, addon, "ps"); }
     Int_t  SaveAsPDF(Int_t num, TString name="", const TString addon="") { return SaveAsVGF(num, name, addon, "pdf"); }
     Int_t  SaveAsSVG(Int_t num, TString name="", const TString addon="") { return SaveAsVGF(num, name, addon, "svg"); }
     Bool_t SaveAsPNG(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kPng); }
     Bool_t SaveAsGIF(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kGif); }
     Bool_t SaveAsXPM(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kXpm); }
     Bool_t SaveAsJPG(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kJpeg); }
     Bool_t SaveAsTIFF(Int_t num, TString name="") { return SaveAsImage(num, name, TImage::kTiff); }
     Bool_t SaveAsBMP(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kBmp); }
     Bool_t SaveAsXML(Int_t num, TString name="")  { return SaveAsImage(num, name, TImage::kXml); }
     Bool_t SaveAsC(Int_t num, TString name="");
     Int_t  SaveAsRoot(Int_t num, TString name="", Bool_t plain=kFALSE);
     Int_t  SaveAsPlainRoot(Int_t num, TString name="") { return SaveAsRoot(num, name, kTRUE); }
     Int_t  SaveAs(Int_t num, TString name);

     Bool_t SaveAsCSV(Int_t num, TString name="", Char_t delim='\t');
     //Bool_t SaveAsXML(Int_t num, TString name="",);
     Int_t  PrintPS(Int_t num, const char *p=0, const char *cmd=0, const char *tmp=0);
     Bool_t PrintLog(const char *p=0, const char *c=0);

     Bool_t SaveLogAsPS(const char *name) const;

     // I/O
     Int_t  SaveAs(Int_t num=-1);
     Int_t  Open(TString fname, const char *name="MStatusDisplay");
     Int_t  Open();

     // Root I/O
     Int_t Write(Int_t num, const char *name="MStatusDisplay", Option_t *opt="") const;

     Int_t Read(const char *name, const char *tab);
     Int_t Read(const char *name="MStatusDisplay") { return Read(name, 0); }
     Int_t Write(const char *name=0, Int_t =0, Int_t =0) { return Write(-1, name?name:"MStatusDisplay"); }
     Int_t Write(const char *name=0, Int_t =0, Int_t =0) const { return Write(-1, name?name:"MStatusDisplay"); }

     // Eventloop interface
     Status_t CheckStatus() const { return fStatus; }
     void ClearStatus() { fStatus=kLoopPause; ProcessMessageCommandMenu(kLoopPause); }
     void SetPause() { fStatus=kLoopNone; ProcessMessageCommandMenu(kLoopPause); }
     void SetLoopStep() { SetPause(); fStatus = kLoopStep; }

     void Lock() { fIsLocked++; }
     void UnLock() { if (fIsLocked>0) fIsLocked--; }

     // GUI event handling
     void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);
     Bool_t HandleEvent(Event_t *event);

     // Public helper functions
     static TRootEmbeddedCanvas *GetEmbeddedCanvas(TGCompositeFrame &cf);
     static TCanvas *GetCanvas(TGCompositeFrame &f);

     ClassDef(MStatusDisplay, 0)   // Window for a status display
};

#endif
