#ifndef MARS_MagicCivilization
#define MARS_MagicCivilization

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

class MagicCivilization : public TObject
{
private:
    enum
    {
        kBackground   = 50,
        kHasFlag      = BIT(17),
        kHasCreation  = BIT(18),
        kUserBits     = 0x7fc000 // 14-23 are allowed
    };

    TTimer    fTimer;           // timer rising the 500ms interrputs

    MGeomCam *fGeomCam;         // pointer to camera geometry

    UShort_t  fNumPixels;       // number of pixels in the present geometry
    Short_t   fNumInit;         // number of bombs in the field
    Float_t   fRange;           // the range in millimeters of the present geometry

    Bool_t    fAuto;

    Char_t    fLimit;

    UShort_t  fNumCivilizations;
    UInt_t    fStep;

    TText     fCivilization;    // TText showing the numbers of pixels and bombs

    TArrayI fColors;

    void   Update();
    void   Free();
    void   SetNewCamera(MGeomCam *);

    Bool_t HandleTimer(TTimer *timer);
    void   Draw(Option_t *option="");
    void   Paint(Option_t *option="");
    void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Int_t  DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

public:
    MagicCivilization(Byte_t lim=2, UShort_t init=200);
    ~MagicCivilization();

    void ChangeCamera(); //*MENU*
    void Reset();        //*MENU*

    ClassDef(MagicCivilization, 0) // Tool to visualize next neighbours
};

#endif
