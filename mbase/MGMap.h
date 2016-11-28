#ifndef MARS_MGMap
#define MARS_MGMap

#ifndef ROOT_GuiTypes
#include <GuiTypes.h> // Drawable_t
#endif
#ifndef ROOT_TExMap
#include <TExMap.h>
#endif

class TString;
class TGToolTip;

#include <iostream>

class MGMap : public TExMap
{
private:
    //    TGToolTip *fToolTip;   //! The tooltip currently displayed

    enum {
        kIsOwner    = BIT(14),
        // kNoToolTips = BIT(15), // suppress tooltips
    };

    //    void ShowToolTip(Int_t px, Int_t py, const char *txt);

public:
    MGMap(Int_t mapSize = 100);
    ~MGMap();

    void     SetOwner(Bool_t o=kTRUE) { o ? SetBit(kIsOwner) : ResetBit(kIsOwner); }
    void     Add(TObject *k, TString *v=0);
    void     Delete(Option_t *opt="");
    void     Clear(Option_t *o="") { TExMap::Delete(o); }

    //    void   SetNoToolTips(Bool_t b=kTRUE) { b ? SetBit(kNoToolTips) : ResetBit(kNoToolTips); } // *TOGGLE* *GETTER=HasNoToolTips
    //    Bool_t HasNoToolTips() const         { return TestBit(kNoToolTips); }

    void     Paint(Option_t *o="");
    void     Paint(UChar_t *buf, int w, int h, Float_t scale);
    //void     Paint(Drawable_t id, Float_t scale);

    void     DrawLine(TObject *o, UChar_t *buf, int w, int h, Double_t scale);
    void     DrawMarker(TObject *o, UChar_t *buf, int w, int h, Double_t scale);

    //void     EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);
    TObject *PickObject(Int_t px, Int_t py, TString &str) const;
    char    *GetObjectInfo(Int_t px, Int_t py) const;
    Int_t    DistancetoPrimitive(Int_t px, Int_t py);

    static UChar_t Color(int col);
    static void    DrawCircle(UChar_t *buf, int w, int h, Float_t x, Float_t y, Float_t r, UChar_t col);
    static void    DrawHexagon(UChar_t *buf, int w, int h, Float_t x, Float_t y, Float_t r, UChar_t col, Int_t style=1);
    static void    DrawLine(UChar_t *buf, int w, int h, Float_t x1, Float_t y1, Float_t x2, Float_t y2, UChar_t col, Int_t style=1);
    static void    DrawBox(UChar_t *buf, int w, int h, Float_t x1, Float_t y1, Float_t x2, Float_t y2, UChar_t col, Int_t style=1);
    static void    DrawDot(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, UChar_t col);
    static void    DrawMultiply(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, Float_t size, UChar_t col);
    static void    DrawCross(UChar_t *buf, int w, int h, Float_t cx, Float_t cy, Float_t size, UChar_t col);

    ClassDef(MGMap, 1) // Special TExMap supporting enhanced drawing and bitmap drawing
};

#endif
