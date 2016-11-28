#ifndef MARS_MGeom
#define MARS_MGeom

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class MGeomCam;

class TAttLine;
class TAttFill;
class TVector2;

class MGeom : public TObject
{
public:
    enum {
        kRightTop,
        kRight,
        kRightBottom,
        kLeftBottom,
        kLeft,
        kLeftTop,
        kTop,
        kBottom
    };

private:
    enum {
        kIsInOutermostRing = 0,
        kIsInOuterRing     = 1,
    };

protected:
    Float_t fX;            // [mm]   the x coordinate of the center
    Float_t fY;            // [mm]   the y coordinate of the center
    Float_t fA;            // [mm^2] Area of the pixel

    UInt_t fSector;        // Number of sector the pixels corresponds to
    UInt_t fAidx;          // Area index of the pixel

protected:
    Byte_t  fNumNeighbors; // number of valid neighbors
    Short_t fNeighbors[6]; // the IDs of the pixel next to it (we are assuming an hexagonal geometry)

    Byte_t fUserBits;

public:
    MGeom(Float_t x=0, Float_t y=0, UInt_t s=0, UInt_t aidx=0);

    void Copy(TObject &obj) const
    {
        MGeom &pix = (MGeom&)obj;
        pix.fX = fX;
        pix.fY = fY;
        pix.fA = fA;
        pix.fNumNeighbors = fNumNeighbors;
        pix.fSector = fSector;
        pix.fAidx = fAidx;
        pix.fUserBits = fUserBits;
        for (int i=0; i<6; i++)
            pix.fNeighbors[i] = fNeighbors[i];

        TObject::Copy(obj);
    }

    void Print(Option_t *opt=NULL) const;

    void Set(Float_t x, Float_t y, UInt_t s=0, UInt_t aidx=0) { fX=x; fY=y; fSector=s; fAidx=aidx; }
    void SetP(Float_t x, Float_t y) { fX=x; fY=y; }
    void SetP(const TVector2 &v);

    void SetNeighbors(Short_t i0=-1, Short_t i1=-1, Short_t i2=-1,
                      Short_t i3=-1, Short_t i4=-1, Short_t i5=-1);

    void CheckOuterRing(const MGeomCam &cam);

    Float_t GetX() const  { return fX; }
    Float_t GetY() const  { return fY; }

    virtual Float_t GetL() const = 0;    // Length of a side to a neighbor
    virtual Float_t GetT() const = 0;    // Maximum elongation

    UInt_t  GetSector() const { return fSector; }

    TVector2 GetP() const;

    Float_t GetDist() const;
    Float_t GetDist(const MGeom &pix) const;
    Float_t GetDist2(const MGeom &pix) const;
    Float_t GetAngle(const MGeom &pix) const;

    Float_t GetA() const    { return fA; /*fD*fD*gsTan60/2;*/ }
    Int_t   GetAidx() const { return fAidx; }

    Byte_t  GetNumNeighbors() const { return fNumNeighbors; }
    Short_t GetNeighbor(Byte_t i) const { return fNeighbors[i]; }

    Bool_t IsInOutermostRing() const { return TESTBIT(fUserBits, kIsInOutermostRing); }
    Bool_t IsInOuterRing() const     { return TESTBIT(fUserBits, kIsInOuterRing); }

    virtual Bool_t IsInside(Float_t px, Float_t py) const = 0;
    Int_t  GetDirection(const MGeom &pix) const;

    Int_t DistancetoPrimitive(Int_t px, Int_t py);

    virtual void PaintPrimitive(const TAttLine &line, const TAttFill &fill, Double_t scalexy=1, Double_t scaled=1) const = 0;

    ClassDef(MGeom, 1) // Geometry class describing the basics of a pixel
};

#endif
