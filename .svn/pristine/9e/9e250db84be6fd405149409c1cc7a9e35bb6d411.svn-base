#ifndef MARS_MGeomCam
#define MARS_MGeomCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif
#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif
#ifndef ROOT_TArrayS
#include <TArrayS.h>
#endif
#ifndef MARS_MQuaternion
#include "MQuaternion.h"
#endif

class TVector2;
class TArrayI;
class MGeom;

class MGeomCam : public MParContainer
{
private:
    UInt_t    fNumPixels;       // Number of pixels in this camera

    Float_t   fCamDist;         // [m] Average distance of the camera from the mirror
    Float_t   fConvMm2Deg;      // conversion factor to convert mm in the camera plain into degrees

    TObjArray fPixels;          // Array of singel pixels storing the geometry

    TArrayF   fMaxRadius;       // maximum radius of the part of the camera with the same pixel size (eg. for GUI layout)
    TArrayF   fMinRadius;       // minimum radius of the part of the camera with the same pixel size (eg. for GUI layout)
    TArrayF   fPixRatio;        // Array storing the ratio between size of pixel 0 and pixel idx (for speed reasons)
    TArrayF   fPixRatioSqrt;    // Array storing the square root ratio between size of pixel 0 and pixel idx (for speed reasons)

    TArrayS   fNumPixInSector;  // Number of pixels in sector
    TArrayS   fNumPixWithAidx;  // Number of pixels with one pixel sizes

//    Int_t     fNumSectors;      // Number of sectors
//    Int_t     fNumAreas;        // Number of different pixel sizes

    void CalcMaxRadius();
    void CalcNumSectors();
    void CalcNumAreas();
    void InitOuterRing();

    virtual void CreateNN();

protected:
    void SortNeighbors();

public:
    MGeomCam(UInt_t npix=0, Float_t dist=1, const char *name=NULL, const char *title=NULL);
    MGeomCam(const MGeomCam &cam) : MParContainer(cam) { cam.Copy(*this); }

    TObject *Clone(const char *name="") const;
    void Copy(TObject &o) const;

    // FIXME, workaround: this function is made public just to allow
    // the use of some camera files from the 0.7 beta version in which the 
    // array containing pixel ratios is not initialized.
    void CalcPixRatio();

    // FIXME, workaround broken streaming
    void StreamerWorkaround();

    void InitGeometry()
    {
        CreateNN();
        CalcNumSectors();
        CalcNumAreas();
        CalcMaxRadius();
        CalcPixRatio();
        InitOuterRing();
    }

    Float_t GetCameraDist() const { return fCamDist; }
    Float_t GetConvMm2Deg() const { return fConvMm2Deg; }

    UInt_t  GetNumPixels()  const { return fNumPixels; }

    Float_t GetMaxRadius() const;
    Float_t GetMinRadius() const;
    Float_t GetMaxRadius(const Int_t i) const;
    Float_t GetMinRadius(const Int_t i) const;

    Float_t GetDist(UShort_t i, UShort_t j=0) const;
    Float_t GetAngle(UShort_t i, UShort_t j=0) const;

    Float_t GetA() const;

    Float_t GetPixRatio(UInt_t i) const;
    Float_t GetPixRatioSqrt(UInt_t i) const;

    UInt_t  GetNumSectors() const { return fNumPixInSector.GetSize(); }
    UInt_t  GetNumAreas() const { return fNumPixWithAidx.GetSize(); }

    UShort_t GetNumPixInSector(UInt_t i) const { return (UShort_t)fNumPixInSector[i]; }
    UShort_t GetNumPixWithAidx(UInt_t i) const { return (UShort_t)fNumPixWithAidx[i]; }

    MGeom &operator[](Int_t i);
    MGeom &operator[](Int_t i) const;

    Int_t GetPixelIdx(const TVector2 &v) const;
    Int_t GetPixelIdxDeg(const TVector2 &v) const;
    Int_t GetPixelIdxXY(Float_t x, Float_t y) const;
    Int_t GetPixelIdxXYdeg(Float_t x, Float_t y) const
    {
        return GetPixelIdxXY(x/fConvMm2Deg, y/fConvMm2Deg);
    }

    void GetNeighbors(TArrayI &arr, UInt_t idx,          Float_t r) const;
    void GetNeighbors(TList &arr,   UInt_t idx,          Float_t r) const;
    void GetNeighbors(TArrayI &arr, const MGeom &pix, Float_t r) const;
    void GetNeighbors(TList &arr,   const MGeom &pix, Float_t r) const;

    Int_t GetDirection(UInt_t p1, UInt_t p2) const;
    Int_t GetNeighbor(UInt_t idx, Int_t dir) const;

    virtual Bool_t HitFrame(MQuaternion, const MQuaternion &) const { return kFALSE; }
            Bool_t HitFrame(MQuaternion p, const MQuaternion &u, Double_t margin) const;
    virtual Bool_t HitDetector(const MQuaternion &p, Double_t offset=0) const;

    void Print(Option_t *opt=NULL) const;

    void SetAt(UInt_t i, const MGeom &pix);

    ClassDef(MGeomCam, 5)  // Geometry base class for the camera
};

#endif
