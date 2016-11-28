#ifndef MARS_MGeomRectangle
#define MARS_MGeomRectangle

#ifndef MARS_MGeom
#include "MGeom.h"
#endif

class MGeomRectangle : public MGeom
{
private:
    Float_t fW;
    Float_t fH;

public:
    MGeomRectangle(Float_t x=0, Float_t y=0, Float_t w=1, Float_t h=-1, UInt_t s=0, UInt_t aidx=0);

    void Copy(TObject &obj) const
    {
        MGeomRectangle &pix = (MGeomRectangle&)obj;

        pix.fW = fW;
        pix.fH = fH;

        MGeom::Copy(obj);
        TObject::Copy(obj);
    }

    void    SetSize(Float_t w, Float_t h=-1) { fW=w; fH=h<0?fW:h; fA=fW*fH; }

    Float_t GetW() const { return fW; }
    Float_t GetH() const { return fH; }
    Float_t GetT() const;
    Float_t GetL() const;

    Bool_t  IsInside(Float_t px, Float_t py) const;
    void    PaintPrimitive(const TAttLine &line, const TAttFill &fill, Double_t scalexy=1, Double_t scaled=1) const;

    void Print(Option_t *opt=NULL) const;

    ClassDef(MGeomRectangle, 1) // Geometry class describing the rectangular geometry of one pixel
};

#endif
