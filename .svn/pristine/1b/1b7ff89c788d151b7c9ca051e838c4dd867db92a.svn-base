#ifndef MARS_MagicSnake
#define MARS_MagicSnake

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

class MagicSnake : public TObject
{
private:
    enum {
        kRightTop,
        kRight,
        kRightBottom,
        kLeftBottom,
        kLeft,
        kLeftTop
    };

    enum
    {
        kBackground   = 50,
        kHasBomb      = BIT(16),
        kHasFood      = BIT(17),
        kHasWorm      = BIT(18),
        kHasTransport = BIT(19),
        kHasDoor      = BIT(20),
        kUserBits     = 0x7fc000 // 14-23 are allowed

    };

    Byte_t    fLength;          // actual length of worm
    Int_t    *fArray;           // inices of pixels which are 'wormed'
    Char_t    fDirection;       // actual direction of worm

    TTimer    fTimer;           // timer rising the 500ms interrputs

    MGeomCam *fGeomCam;         // pointer to camera geometry

    UShort_t  fTransport[2];    // pixel ids with the yellow transpoters
    UInt_t    fNumPixels;       // number of pixels in the present geometry
    Byte_t    fNumBombs;        // number of bombs in the field
    Byte_t    fNumFood;         // number of food packages
    Float_t   fRange;           // the range in millimeters of the present geometry

    TArrayI fColors;

    TText    *fDone;            // TText showing the 'Game over'
    TText    *fPaused;          // TText showing the 'Game over'
    TText     fShow;            // TText showing the numbers of pixels and bombs

    TVirtualPad   *fDrawingPad; // pad in which we are drawing

    void   Done(TString, Int_t col);
    void   Step(Int_t newpix);
    void   Update();
    void   Free();
    void   DrawHexagons();
    Int_t  ScanNeighbours();
    void   SetNewCamera(MGeomCam *);
    void   SetWormColor();
    void   Pause(Bool_t yes=kTRUE);
    void   Init();

    Bool_t HandleTimer(TTimer *timer);
    void   Draw(Option_t *option="");
    void   Paint(Option_t *option="");
    void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t  DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MagicSnake();
    MagicSnake(const MGeomCam &geom);
    ~MagicSnake();

    void Reset();        //*MENU*
    void ChangeCamera(); //*MENU*

    ClassDef(MagicSnake, 0) // Magic Camera Games: Snake
};

#endif
