#ifndef MARS_MSignalCam
#define MARS_MSignalCam

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MSignalPix
#include "MSignalPix.h"
#endif

class MGeomCam;
class MSignalPix;
class MSignalCamIter;

class MSignalCam : public MParContainer, public MCamEvent
{
    friend class MSignalCamIter;
private:
    Short_t       fNumIslands;                  //!
    Short_t       fNumSinglePixels;             //!
    Float_t       fSizeSinglePixels;            //!
    Float_t       fSizeSubIslands;              //!
    Float_t       fSizeMainIsland;              //!

    Int_t         fNumPixelsSaturatedHiGain;
    Int_t         fNumPixelsSaturatedLoGain;
    TClonesArray *fPixels;     //-> FIXME: Change TClonesArray away from a pointer?

    Double_t CalcIsland(const MGeomCam &geom, Int_t idx, Int_t num);

public:
    MSignalCam(const char *name=NULL, const char *title=NULL);
    ~MSignalCam() { delete fPixels; }

    // Setter function to fill pixels
    MSignalPix *AddPixel(Int_t idx, Float_t nph=0, Float_t er=0);
    void InitSize(const UInt_t i)
    {
        fPixels->ExpandCreate(i);
    }

    // Setter functions for use in image cleaning classes only
    void SetSinglePixels(Short_t num, Float_t size)  { fNumSinglePixels=num; fSizeSinglePixels=size; }
    void SetNumPixelsSaturated(UInt_t hi, UInt_t lo) { fNumPixelsSaturatedHiGain=hi;fNumPixelsSaturatedLoGain=lo; }
    void SetIslandInfo(Short_t num, Float_t main, Float_t sub)
    { fNumIslands = num; fSizeMainIsland=main; fSizeSubIslands=sub; }

    // Getter functions
    UInt_t  GetNumPixels() const { return fPixels->GetEntriesFast(); }
    Short_t GetNumIslands() const { return fNumIslands; };
    Short_t GetNumSinglePixels() const { return fNumSinglePixels; }
    Float_t GetSizeSinglePixels() const { return fSizeSinglePixels; }
    Float_t GetSizeSubIslands() const { return fSizeSubIslands; }
    Float_t GetSizeMainIsland() const { return fSizeMainIsland; }
    Int_t   GetNumPixelsSaturatedHiGain() const { return fNumPixelsSaturatedHiGain; }
    Int_t   GetNumPixelsSaturatedLoGain() const { return fNumPixelsSaturatedLoGain; }

    Int_t   GetNumPixelsUnmapped() const;

    Float_t GetNumPhotonsMin(const MGeomCam *geom=NULL) const;
    Float_t GetNumPhotonsMax(const MGeomCam *geom=NULL) const;

    Float_t GetRatioMin(const MGeomCam *geom=NULL) const;
    Float_t GetRatioMax(const MGeomCam *geom=NULL) const;

    Float_t GetErrorPhotMin(const MGeomCam *geom=NULL) const;
    Float_t GetErrorPhotMax(const MGeomCam *geom=NULL) const;

    // Getter functions to access single pixels
    MSignalPix &operator[](int i)       { return *(MSignalPix*)(fPixels->UncheckedAt(i)); }
    MSignalPix &operator[](int i) const { return *(MSignalPix*)(fPixels->UncheckedAt(i)); }

    // Functions to change the contained data
    Int_t CalcIslands(const MGeomCam &geom);

    // Functions for easy comparisons
    Bool_t CompareCleaning(const MSignalCam &cam) const;
    Bool_t CompareIslands(const MSignalCam &cam) const;

    // class MParContainer
    void Reset();

    // class TObject
    void Copy(TObject &obj) const;
    void Print(Option_t *opt=NULL) const;
    void Clear(Option_t *opt=NULL) { Reset(); }

    // class MCamEvent
    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    // To build an iterator for this class
    operator TIterator*() const;

    ClassDef(MSignalCam, 2) // class for an event containing cerenkov photons
};

class MSignalCamIter : public TObjArrayIter
{
private:
    Bool_t fUsedOnly;
    Int_t fIdx;

public:
    MSignalCamIter(const MSignalCam *evt, Bool_t usedonly=kTRUE, Bool_t dir=kIterForward) : TObjArrayIter(evt->fPixels, dir), fUsedOnly(usedonly), fIdx(-1) { }
    TObject *Next();
    void Reset() { fIdx=-1; TObjArrayIter::Reset(); }
    TIterator &operator=(const TIterator &) { return *this; }
    Int_t GetIdx() const { return fIdx; }
    ClassDef(MSignalCamIter, 0)
};

inline MSignalCam::operator TIterator*() const { return new MSignalCamIter(this); }

#endif
