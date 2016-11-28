#ifndef MARS_MGainFluctuationCam
#define MARS_MGainFluctuationCam

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MGainFluctuationPix
#include "MGainFluctuationPix.h"
#endif

class MGeomCam;
class MGainFluctuationPix;
class MGainFluctuationCamIter;

class MGainFluctuationCam : public MParContainer, public MCamEvent
{
    friend class MGainFluctuationCamIter;
private:
    UInt_t        fNumPixels;
     Int_t        fMaxIndex;
    TArrayI       fLut;        // Lookup tabel to lookup pixel by index
    TClonesArray *fPixels;     //-> FIXME: Change TClonesArray away from a pointer?

    void RebuildLut()
    {
        // Resize Lut
        fLut.Set(fMaxIndex+1);

        // Reset Lut
        fLut.Reset(-1);

        // Rebuild Lut
        for (UInt_t i=0; i<GetNumPixels(); i++)
        {
            const MGainFluctuationPix &pix = (*this)[i];
            fLut[pix.GetPixId()] = i;
        }
    }

public:
    MGainFluctuationCam(const char *name=NULL, const char *title=NULL);
    ~MGainFluctuationCam() { delete fPixels; }

    // Setter function to fill pixels
    MGainFluctuationPix *AddPixel(Int_t idx, Float_t gain);
    void FixSize();

    // Getter functions
    UInt_t  GetNumPixels() const { return fNumPixels; }
    Float_t GetGain(int i) const;

    Float_t GetGainMin(const MGeomCam *geom=NULL) const;
    Float_t GetGainMax(const MGeomCam *geom=NULL) const;

    // Getter functions to access single pixels
    MGainFluctuationPix &operator[](int i)       { return *(MGainFluctuationPix*)(fPixels->UncheckedAt(i)); }
    MGainFluctuationPix &operator[](int i) const { return *(MGainFluctuationPix*)(fPixels->UncheckedAt(i)); }

    MGainFluctuationPix *GetPixById(Int_t idx) const;

    void Sort(Int_t upto = kMaxInt)
    {
        // Sort pixels by index
        fPixels->Sort(upto);
        RebuildLut();
    } // For convinience: Sort pixels by index

    // class MParContainer
    void Reset();

    // class TObject
    void Print(Option_t *opt=NULL) const;
    void Clear(Option_t *opt=NULL) { Reset(); }

    // class MCamEvent
    Bool_t GetPixelContent(Double_t &val, Int_t idx,const MGeomCam&, Int_t type=1) const;
    void DrawPixelContent(Int_t num) const;

    // To build an iterator for this class
    operator TIterator*() const;

    ClassDef(MGainFluctuationCam, 1)    // class for an event containing cerenkov photons
};

class MGainFluctuationCamIter : public TObjArrayIter
{
private:
    Bool_t fUsedOnly;
public:
    MGainFluctuationCamIter(const MGainFluctuationCam *evt, Bool_t usedonly=kTRUE, Bool_t dir=kIterForward) : TObjArrayIter(evt->fPixels, dir), fUsedOnly(usedonly) { }
    TObject *Next();
    TIterator &operator=(const TIterator &) { return *this; }
    ClassDef(MGainFluctuationCamIter, 0)
};

inline MGainFluctuationCam::operator TIterator*() const { return new MGainFluctuationCamIter(this); }

#endif
