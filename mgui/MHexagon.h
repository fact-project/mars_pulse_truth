#ifndef MARS_MHexagon
#define MARS_MHexagon

//////////////////////////////////////////////////////////////
//
//   MHexagon
//
//   A Hexagon for the MAGIC event display
//
//////////////////////////////////////////////////////////////

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TObject
#include <TObject.h>
#endif 

#ifndef ROOT_TAttLine
#include <TAttLine.h>
#endif 

#ifndef ROOT_TAttFill
#include <TAttFill.h>
#endif 

class TOrdCollection;

class MHexagon : public TObject, public TAttLine, public TAttFill
{
private:
    static const Double_t fgCos60;
    static const Double_t fgSin60;

public:
    static const Double_t fgDx[6];   // X coordinate of the six edges
    static const Double_t fgDy[6];   // Y coordinate of the six edges

protected:

    Float_t fX;  // X coordinate  of center
    Float_t fY;  // Y coordinate  of center
    Float_t fD;  // diameter D or better distance between opposite sides

    Float_t fPhi; // Rotation angle

public:

    MHexagon();
    MHexagon(Float_t x, Float_t y, Float_t d, Float_t phi=0);
    MHexagon(const MHexagon &hexagon);

    virtual void  Copy(TObject &hexagon)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
const
#endif
        ;

    Int_t DistancetoPrimitive(Int_t px, Int_t py, Float_t conv);
    virtual Int_t DistancetoPrimitive(Int_t px, Int_t py)
    {
        return DistancetoPrimitive(px, py, 1);
    }

    virtual void  DrawHexagon(Float_t x, Float_t y, Float_t d, Float_t phi=0);

    //virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);

    virtual void  ls(const Option_t *Option="") const;
    virtual void  Paint(Option_t *Option="");
    virtual void  PaintHexagon(Float_t x, Float_t y, Float_t d, Float_t phi=0);
    virtual void  Print(Option_t *Option="") const; // *MENU*
    virtual void  SavePrimitive(std::ostream &out, Option_t *);
    virtual void  SavePrimitive(std::ofstream &out, Option_t *);

    Float_t GetX() const { return fX; }
    Float_t GetY() const { return fY; }
    Float_t GetD() const { return fD; }
    Float_t GetPhi() const { return fPhi; }

    ClassDef(MHexagon, 2)    // A hexagon for MAGIC
};

#endif

