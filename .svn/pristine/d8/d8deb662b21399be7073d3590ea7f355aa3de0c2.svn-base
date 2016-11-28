#ifndef MARS_MRawEvtPixelIter
#define MARS_MRawEvtPixelIter
///////////////////////////////////////////////////////////////////////////////
//
//  MRawEvtPixelIter
//
//  class to iterate over all pixels of one event.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

class MRawEvtData;

class MRawEvtPixelIter : public TObject
{
private:
    UShort_t  fNumHiGainEntry;  //! actual number of entry in fHiGainPixId
    UShort_t  fNumLoGainEntry;  //! actual number of entry in fLoGainPixId

    UShort_t *fHiGainId;        //! actual entry of fHiGainPixId
    UShort_t *fLoGainId;        //! actual entry of fLoGainPixId

    Byte_t   *fHiGainPos;       //! pointer to hi-gain samples of actual pixel
    Byte_t   *fLoGainPos;       //! pointer to lo-gain samples of actual pixel

    Byte_t   *fABFlags;         //! pointer to AB flags
    UShort_t *fStartCell;      //! pointer to AB flags

    UShort_t  fNumEntry;

    UInt_t fNumBytesHiGain;   //!
    UInt_t fNumBytesLoGain;   //!

    UShort_t fNumBytesPerSample; //!

    MRawEvtData *fData;         //! pointer to object which we are iterating

public:
    MRawEvtPixelIter(MRawEvtData *dat);

    MRawEvtData *Next();

    Bool_t Jump(UShort_t id)
    {
        //
        // Jump to the pixel with the pixel-id ID
        // If it doesn't exist return FALSE
        //
        Reset();
        while (Next())
            if (GetPixelId() == id)
                return kTRUE;
        return kFALSE;
    }

    UShort_t GetPixelId() const
    {
        //
        // return Id of actual pixel
        //
        return *fHiGainId;
    }

    UShort_t GetNumEntry() const
    {
        return fNumHiGainEntry;
    }

    void *GetHiGainSamples() const
    {
        //
        // return a pointer to the fadc samples of the hi gains
        // WARNING: Don't forget to get the number of valid entries
        //          (GetNumSamples) to know how many entries of the array
        //          belong to the actual pixel
        //
        return fHiGainPos;
    }

    UInt_t GetNumBytes() const { return fNumBytesHiGain+fNumBytesLoGain; }

    Bool_t HasLoGain() const { return fNumBytesLoGain>0; }
    Bool_t IsABFlagValid() const { return fABFlags ? kTRUE : kFALSE; }
    Bool_t HasABFlag() const
    {
        //
        // return kTRUE  the lo gains exist for the actual pixel, else return kFALSE
        //
        return TESTBIT(GetABFlag(), GetPixelId()%8);
    }
    Byte_t GetABFlag() const
    {
        //
        // return kTRUE  the lo gains exist for the actual pixel, else return kFALSE
        //
        return fABFlags ? fABFlags[GetPixelId()/8] : 0;
    }

    void *GetLoGainSamples() const
    {
        //
        // return a pointer to the fadc samples of the lo gains if they exist
        // for the actual pixel, else return zero
        //
        return HasLoGain() ? fLoGainPos : NULL;
    }

    UShort_t GetStartCell() const
    {
        return fStartCell ? *fStartCell : 0;
    }

    void Reset();

    void Draw(Option_t *t="GRAPH");

    ClassDef(MRawEvtPixelIter, 0) // iterates over all pixels of one MRawEvtData object
};

#endif
