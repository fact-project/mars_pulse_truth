#ifndef MARS_MRawEvtData
#define MARS_MRawEvtData

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

// gcc 3.2
//class ifstream;
#include <iosfwd>
#include <stdint.h>

class MRawRunHeader;
class MRawCrateArray;

class TArrayC;
class MArrayS;
class MArrayB;
class MArrayI;

class MRawEvtData : public MParContainer, public MCamEvent
{
    friend class MRawEvtPixelIter;
private:
    MRawRunHeader  *fRunHeader;   //! provides information about numbers

    // FIXME: COMMENT ABOUT ORDERING

    MArrayS *fHiGainPixId;        //-> list of pixel IDs of hi gain channel
    MArrayB *fHiGainFadcSamples;  //-> list of hi gain samples of all pixels (ordering: see fHiGainPixId) {fits: unit=mV ; name=Data }

    MArrayS *fLoGainPixId;        //-> list of pixel IDs of lo gain channel
    MArrayB *fLoGainFadcSamples;  //-> list of lo gain samples of all pixels (ordering: see fLoGainPixId)

    MArrayB *fABFlags;            //-> A information about the exact trigger position
    MArrayS *fStartCells;         // {fits: unit=uint16 ; name=StartCellData }

    UShort_t fTriggerType;       // {fits: unit=uint16 ; name=TriggerType }

    UShort_t fNumBytesPerSample;

    Bool_t fIsSigned;

    Int_t fConnectedPixels;       //!

    void InitArrays(UShort_t numconnected=0, UShort_t maxid=0);
    void DeleteArrays();

    Int_t GetNumBytes() const;

    Long_t GetSample(const void *ptr, Int_t n) // Helper for Draw
    {
        fIsSigned = kTRUE;
        switch (fNumBytesPerSample)
        {
        case 1: return fIsSigned ? (Long_t)reinterpret_cast<const Char_t*>(ptr)[n]  : (Long_t)reinterpret_cast<const Byte_t*>(ptr)[n];
        case 2: return fIsSigned ? (Long_t)reinterpret_cast<const Short_t*>(ptr)[n] : (Long_t)reinterpret_cast<const UShort_t*>(ptr)[n];
        case 4: return fIsSigned ? (Long_t)reinterpret_cast<const Int_t*>(ptr)[n]   : (Long_t)reinterpret_cast<const UInt_t*>(ptr)[n];
        }
        return 0;
    }


public:
    MRawEvtData(const char *name=NULL, const char *title=NULL);
    ~MRawEvtData();

    void InitRead(MRawRunHeader *rh)
    {
        //
        // you have to set this before you can read information
        // from a magic binary file
        //
        fRunHeader  = rh;
    }

    void Clear(Option_t * = NULL);
    void Print(Option_t * = NULL) const;
    void Draw (Option_t * = NULL);

    void ResetPixels();
    void ResetPixels(UShort_t npix, UShort_t maxid);
    void InitStartCells();
    void AddPixel(UShort_t nOfPixel, const TArrayC &data);
    void Set(const MArrayI &data);
    void SetIndices(const MArrayS &idx);
    void SetIndices();

    UShort_t GetNumHiGainSamples() const;
    UShort_t GetNumLoGainSamples() const;
    UInt_t   GetNumSamples() const { return GetNumHiGainSamples()+GetNumLoGainSamples(); }
    UShort_t GetNumPixels() const;

    UShort_t GetNumBytesPerSample() const { return fNumBytesPerSample; }
    UInt_t   GetScale() const { return 1<<((fNumBytesPerSample-1)*8); }
    Long_t   GetMin() const   { return fIsSigned ? -GetMax()-1 : 0; }
    Long_t   GetMax() const   {
        switch (fNumBytesPerSample)
        {
        case 1: return fIsSigned ?       0x7f :       0xff;
        case 2: return fIsSigned ?     0x7fff :     0xffff;
        case 4: return fIsSigned ? 0x7fffffff : 0xffffffff;
        }
    return 0;
    }
    Byte_t   *GetSamples() const;
    UShort_t *GetStartCells() const;
    UShort_t *GetPixelIds() const;

    Bool_t HasStartCells() const;

    void ReadPixel(std::istream &fin, Int_t npix);
    void SetABFlag(Int_t npix, Bool_t ab);
    void SkipEvt(std::istream &fin);

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void   DrawPixelContent(Int_t num) const
    {
        TString s("HIST");
        s += num;
        const_cast<MRawEvtData*>(this)->Draw(s);
    }

    void Copy(TObject &named)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
        const
#endif
        ;

    ClassDef(MRawEvtData, 8) //Container to store the raw Event Data
};

#endif
