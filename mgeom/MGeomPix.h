#ifndef MARS_MGeomPix
#define MARS_MGeomPix

#ifndef MARS_MGeom
#include "MGeom.h"
#endif

class MGeomPix : public MGeom
{ 
public:
    static const Float_t gsTan60; // tan(60/kRad2Deg);
    static const Float_t gsTan30; // tan(30/kRad2Deg);

    static const Float_t gsCos60; // cos(60/kRad2Deg);
    static const Float_t gsSin60; // sin(30/kRad2Deg);

private:
    Float_t fD;            // [mm]   the d coordinate of the pixel (dist between two parallel sides)

    Double_t fCosPhi;
    Double_t fSinPhi;

    // Store fD*fCosPhi  ???
    // Store fD*fSinPhi  ???
    // Store array and check if initialized before needed?

public:
    // Helper to calculate hexagonal geometry
    enum { kDirCenter, kDirNE, kDirN, kDirNW, kDirSW, kDirS, kDirSE };

    static Double_t CalcXY(Int_t dir, Int_t ring, Int_t i, Double_t &x, Double_t &y);

    // MGeomPix
    MGeomPix(Float_t x=0, Float_t y=0, Float_t d=1, UInt_t s=0, UInt_t aidx=0);

    // TObject
    void Copy(TObject &obj) const
    {
        MGeomPix &pix = (MGeomPix&)obj;

        pix.fD = fD;

        MGeom::Copy(obj);
        TObject::Copy(obj);
    }

    void Print(Option_t *opt=NULL) const;

    // Setter
    void SetD(Float_t d=1) { fD=d; fA=d*d*gsSin60; }
    void SetPhi(Double_t phi=0);

    Float_t GetD() const  { return fD; }         // Distance between two parallel sides
    Float_t GetL() const  { return fD*gsTan30; } // Length of one of the parallel sides
    Float_t GetT() const  { return fD/gsTan60; } // Half distance between two opposite edges (traverse)

    Float_t GetDx() const { return fD; }         // Distance of two rows in x-direction (without rotation)
    Float_t GetDy() const { return fD*gsSin60; } // Distance of two rows in y-direction (without rotation)

    // Helper
    Bool_t  IsInside(Float_t px, Float_t py) const;
    void    PaintPrimitive(const TAttLine &line, const TAttFill &fill, Double_t scalexy=1, Double_t scaled=1) const;

    ClassDef(MGeomPix, 5) // Geometry class describing the geometry of one pixel
};

#endif
