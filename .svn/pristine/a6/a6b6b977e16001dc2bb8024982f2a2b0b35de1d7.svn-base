#ifndef MARS_MagicShow
#define MARS_MagicShow

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TText
#include <TText.h>
#endif
#ifndef ROOT_TTimer
#include <TTimer.h>
#endif
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class TMarker;
class TVirtualPad;

class MGeomCam;
class MHexagon;

class MagicShow : public TObject
{
private:
    enum
    {
        kBackground = 50,
        kUserBits   = 0x7fc000 // 14-23 are allowed

    };

    TTimer    fTimer;           // timer rising the 500ms interrputs

    MGeomCam *fGeomCam;         // pointer to camera geometry

    UShort_t  fNumPixels;       // number of pixels in the present geometry
    Short_t   fNumPixel;        // number of pixel actually shown
    Float_t   fRange;           // the range in millimeters of the present geometry

    Bool_t    fAuto;            // automatic stepping on/off

    TText     fShow;            // TText showing the numbers of pixels and bombs
    TText    *fText[6];         // ttext showing the pixel numbers of the neighbors

    TArrayI fColors;

    void   Update();
    void   Free();
    void   SetNewCamera(MGeomCam *);
    void   ChangePixel(Int_t add);
    void   Init();

    Bool_t HandleTimer(TTimer *timer);
    void   Draw(Option_t *option="");
    void   Paint(Option_t *option="");
    void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t  DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MagicShow();
    MagicShow(const MGeomCam &geom);
    ~MagicShow();

    void ChangeCamera(); //*MENU*

    ClassDef(MagicShow, 0) // Tool to visualize next neighbours
};

#endif
