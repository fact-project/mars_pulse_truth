#ifndef MARS_MGGroupFrame
#define MARS_MGGroupFrame

#ifndef ROOT_TGFrame
#include <TGFrame.h>
#endif
#ifndef ROOT_TGWidget
#include <TGWidget.h>
#endif

class MGTask;
class MGList;

class MGGroupFrame : public TGGroupFrame, public TGWidget
{
    MGTask *fTask;
    MGList *fList;

public:
    MGGroupFrame(MGTask *task,
                 const TGWindow *p, TGString *title,
                 UInt_t options = kVerticalFrame,
                 GContext_t norm=GetDefaultGC()(),
                 FontStruct_t font=GetDefaultFontStruct(),
                 ULong_t back=GetDefaultFrameBackground());
    MGGroupFrame(MGTask *task,
                 const TGWindow *p, const char *title,
                 UInt_t options = kVerticalFrame,
                 GContext_t norm=GetDefaultGC()(),
                 FontStruct_t font=GetDefaultFontStruct(),
                 ULong_t back=GetDefaultFrameBackground());
    virtual ~MGGroupFrame();

    void     AddToList(TObject *obj);
    TObject *FindWidget(Int_t id) const;

    virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);

    ClassDef(MGGroupFrame, 0) // An interface to widgets in a group frame (makes live easier)
};

#endif
