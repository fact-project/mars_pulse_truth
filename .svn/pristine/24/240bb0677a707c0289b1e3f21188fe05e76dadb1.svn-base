#ifndef MARS_MagicReversi
#define MARS_MagicReversi

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

class MGeomCam;
class MHexagon;

class MagicReversi : public TObject
{
private:
    MGeomCam      *fGeomCam;       // pointer to camera geometry

    UInt_t         fNumPixels;     // number of pixels in the present geometry
    Float_t        fRange;         // the range in millimeters of the present geometry

    TClonesArray  *fText;          // array of all texts
    TClonesArray  *fFlags;         // array of all texts

    TArrayI fColors;

    TText         *fDone;          // TText showing the 'Game over'
    TText         *fUsrTxt[6];     // TText showing the numbers of pixels and bombs

    Int_t          fNumUsers;
    Int_t          fNumUser;
    Int_t          fUsrPts[6];

    enum
    {
        kEmpty     = 50,
        kIsVisible = BIT(16),
        kHasBomb   = BIT(17),
        kHasFlag   = BIT(18),
        kUserBits  = 0x7fc000 // 14-23 are allowed
    };

    TText   *GetText(Int_t i) { return (TText*)fText->At(i); }
    TMarker *GetFlag(Int_t i) { return (TMarker*)fFlags->At(i); }

    void  Done();
    void  Update();
    void  SetNewCamera(MGeomCam *);
    void  Free();
    void  Init();

    Bool_t Flip(Int_t idx, Bool_t flip);
    Bool_t CheckMoves();

    void  Paint(Option_t *option="");
    void  Draw(Option_t *option="");
    void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MagicReversi();
    MagicReversi(const MGeomCam &geom);
    ~MagicReversi();

    void Reset();        //*MENU*
    void ChangeCamera(); //*MENU*
    void TwoPlayer(); //*MENU*
    void ThreePlayer(); //*MENU*
    void FourPlayer(); //*MENU*
    void FivePlayer(); //*MENU*
    void SixPlayer(); //*MENU*

    ClassDef(MagicReversi, 0) // Magic Camera Games: Reversi
};

#endif
