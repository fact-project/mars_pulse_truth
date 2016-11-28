#ifndef MARS_MagicDomino
#define MARS_MagicDomino

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TText
#include <TText.h>
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

class MagicDomino : public TObject
{
private:
    enum
    {
        kBackground  = 50,
        kIsTile      = BIT(16),
        kRight       = BIT(17),
        kLeft        = BIT(18),
        kTop         = BIT(19),
        kBottom      = BIT(20),
        kUserBits    = 0x7fc000, // 14-23 are allowed

        kBottomLeft  = kBottom|kLeft,
        kBottomRight = kBottom|kRight,
        kTopLeft     = kTop|kLeft,
        kTopRight    = kTop|kRight

    };

    MGeomCam *fGeomCam;         // pointer to camera geometry

    UShort_t  fNumPixels;       // number of pixels in the present geometry
    Short_t   fNumPixel;        // number of actual pixel
    Int_t     fNumTile;         // number of setteled tiles
    Float_t   fRange;           // the range in millimeters of the present geometry

    Int_t     fPoints;          // points you got

    Int_t     fDir;             // direction you move the tile

    Int_t     fOldColors[7];    // colors of hexagons which are under the tile
    Int_t     fNewColors[6];    // colors of the tile itself
    Byte_t    fPosition;        // rotation position of colors in tile
    Byte_t    fSkipped;         // number of skipped tiles

    TText     fDomino;          // TText showing status informations
    TText    *fText[6];         // ttext showing the six numbers in the tile
    TText    *fDone;            // game over text

    TArrayI fColors;

    Bool_t  CheckTile();
    void    NewTile();
    void    NewColors();
    void    HideTile();
    void    ShowTile();
    void    RotateTile(Int_t add);
    void    Update();
    void    Free();
    void    Done();
    void    RemoveNumbers();
    void    SetNewCamera(MGeomCam *);
    void    ChangePixel(Int_t add);
    void    Step(Int_t dir);
    Short_t AnalysePixel(Int_t dir);
    void    GetSortedNeighbors(Int_t indices[6]);
    void    Init();

    void  Draw(Option_t *option="");
    void  Paint(Option_t *option="");
    void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MagicDomino();
    MagicDomino(const MGeomCam &geom);
    ~MagicDomino();

    void Reset();        //*MENU*
    void ChangeCamera(); //*MENU*

    ClassDef(MagicDomino, 0) // Magic Camera Games: Some kind of Domino
};

#endif
