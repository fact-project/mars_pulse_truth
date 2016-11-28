#ifndef MARS_MBrowser
#define MARS_MBrowser

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif

class MGList;
class TGTab;
class TGListView;
class TGTextEntry;
class TGFSComboBox;
class TGProgressBar;
class TGPictureButton;
class TGFileContainer;

class MBrowser : public TGMainFrame/*TGTransientFrame*/
{
private:
    TGTab            *fTabs;

    TGTextEntry      *fEntry;

    TGFileContainer  *fFileCont;
    TGListView       *fFileView;

    void CreateMenuBar();
    void CreateTab1();
    void CreateUpperFrame(TGCompositeFrame *frameup);
    void CreateLowerFrame(TGCompositeFrame *framelow);
    void CreateDirListMenu(TGCompositeFrame *frame);
    void CreateDirListBox(TGCompositeFrame *frame);

    void SetFileName(TString name);

    void SetFilter();
    void SetDir();
    void SetViewMode(const Int_t mode);

protected:
    MGList *fList;
    TString fInputFile;

    TGCompositeFrame *CreateNewTab(const char *name);

    void DisplError(const char *txt);
    void DisplWarning(const char *txt);
    void DisplInfo(const char *txt);

    void ChangeDir(const char *txt=NULL);

    TGProgressBar *CreateProgressBar(TGHorizontalFrame *frame);
    void DestroyProgressBar(TGProgressBar *bar);

    TGHorizontalFrame *fTop1;
    TGHorizontalFrame *fTop2;
    TGHorizontalFrame *fTop3;

 public: 
     MBrowser(/*const TGWindow *main=NULL,*/ const TGWindow *p=NULL,
              const UInt_t w=500, const UInt_t h=500) ;

     virtual ~MBrowser();

     void   CloseWindow();

     Bool_t InputFileSelected();

     virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

     ClassDef(MBrowser, 0)   // Base class for the Gui-Browsers
} ; 

#endif


