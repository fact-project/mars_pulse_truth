#ifndef MARS_MineSweeper
#define MARS_MineSweeper

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class TText;
class TMarker;
class TVirtualPad;

class MGeom;
class MGeomCam;
class MHexagon;

class MineSweeper : public TObject
{
private:
    static const Int_t fColorBombs[7]; // colors for the hexagons

    MGeomCam      *fGeomCam;       // pointer to camera geometry

    UInt_t         fNumPixels;     // number of pixels in the present geometry
    Int_t          fNumBombs;      // number of bombs in total
    Float_t        fRange;         // the range in millimeters of the present geometry

    TClonesArray  *fText;          // array of all texts
    TClonesArray  *fFlags;         // array of all texts

    TArrayI fColors;

    TText         *fDone;          // TText showing the 'Game over'
    TText         *fShow;          // TText showing the numbers of pixels and bombs

    enum
    {
        kHidden    = 50,
        kIsVisible = BIT(16),
        kHasBomb   = BIT(17),
        kHasFlag   = BIT(18),
        kUserBits  = 0x7fc000 // 14-23 are allowed
    };

    TText   *GetText(Int_t i) { return (TText*)fText->At(i); }
    TMarker *GetFlag(Int_t i) { return (TMarker*)fFlags->At(i); }

    void  OpenHexagon(Int_t idx);
    void  Done(TString, Int_t);
    void  Update(Int_t);
    void  SetNewCamera(MGeomCam *);
    void  PaintPrimitives();
    void  Free();
    void  Init();

    void  Paint(Option_t *option="");
    void  Draw(Option_t *option="");
    void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MineSweeper();
    MineSweeper(const MGeomCam &geom);
    ~MineSweeper();

    void Reset();        //*MENU*
    void ChangeCamera(); //*MENU*

    ClassDef(MineSweeper, 0) // Magic Camera Games: Mine Sweeper
};

#endif
