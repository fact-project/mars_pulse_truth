#ifndef MARS_MGMenu
#define MARS_MGMenu

#ifndef ROOT_TGMenu
#include <TGMenu.h>
#endif

class MGPopupMenu;

class MGMenuEntry : public TGMenuEntry
{
    /*
     Workaround for the protected data mambers of TGMenuEntry
     */
public:
    MGMenuEntry(TGMenuEntry *ent);
    ~MGMenuEntry()
    {
        fLabel=0;
    }

    MGPopupMenu *GetPopup() { return (MGPopupMenu*)fPopup; }
    TGHotString *GetLabel() { return fLabel; }
    void *GetUserData()     { return fUserData; }
};

class MGPopupMenu : public TGPopupMenu
{
public:
    MGPopupMenu(const TGWindow* p = 0, UInt_t w = 10, UInt_t h = 10, UInt_t options = 0) :
        TGPopupMenu(p, w, h, options)
    {
    }

    UInt_t GetKeyCode(TGMenuEntry *el);

    void BindKeys(const TGWindow *w, TGMainFrame *frame);

    Bool_t HandleKey(Event_t *evt);
};

class MGMenuBar : public TGMenuBar
{
public:
    MGMenuBar(const TGWindow* p, UInt_t w, UInt_t h, UInt_t options = kHorizontalFrame|kRaisedFrame) :
        TGMenuBar(p, w, h, options) {}

    void BindKeys(TGMainFrame *frame);                   // root<=3.10.02
    void BindKeys(Bool_t b);

    Bool_t HandleKey(Event_t *event);
};

#endif
