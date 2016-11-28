#ifndef MARS_MPedestalSubtractedEvt
#define MARS_MPedestalSubtractedEvt

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif
#ifndef MARS_MArrayS
#include "MArrayS.h"
#endif

typedef UShort_t USample_t;

class MPedestalSubtractedEvt : public MParContainer, public MCamEvent
{
private:
    MArrayF fSamples;         // list of all samples with pedestal subtracted
    MArrayS fSamplesRaw;      // list of all samples (raw)

    UInt_t fNumSamples;       // number of samples per pixel
    UInt_t fNumPixels;        // number of pixels

public:
    MPedestalSubtractedEvt(const char *name=NULL, const char *title=NULL)
        : fNumSamples(0), fNumPixels(0)
    {
        fName = name ? name : "MPedestalSubtractedEvt";
        fTitle = title ? title : "";
    }

    void InitSize(const UInt_t i) { fNumPixels=i; }
    void InitSamples(UInt_t samples, UInt_t pixels=0);

    Float_t   *GetSamples(UInt_t pixel=0) const;
    USample_t *GetSamplesRaw(UInt_t pixel=0) const;

    UInt_t   GetNumSamples() const { return fNumSamples; }
    UShort_t GetNumPixels() const  { return fNumPixels; }

    Int_t GetSaturation(const Int_t idx, Int_t limit, Int_t &first, Int_t &last) const;
    //void  InterpolateSaturation(const Int_t idx, Int_t limit, Int_t first, Int_t last) const;

    Int_t GetMax(const Int_t pixidx, const Int_t first, const Int_t last, Float_t &val) const;
    Int_t GetMax(const Int_t pixidx, Float_t &val) const
    {
        return GetMax(pixidx, 0, fNumSamples-1, val);
    }
    Int_t GetMaxPos(const Int_t pixidx, const Int_t first, const Int_t last) const
    {
        Float_t val;
        return GetMax(pixidx, first, last, val);
    }
    Int_t GetMaxPos(const Int_t pixidx, Float_t &val) const
    {
        return GetMax(pixidx, 0, fNumSamples-1, val);
    }

    Int_t GetMaxPos(const Int_t pixidx) const
    {
        Float_t val;
        return GetMax(pixidx, 0, fNumSamples-1, val);
    }

    Int_t GetRawMax(const Int_t idx, const Int_t first, const Int_t last, UInt_t &val) const;
    Int_t GetRawMax(const Int_t pixidx, UInt_t &val) const
    {
        return GetRawMax(pixidx, 0, fNumSamples-1, val);
    }
    Int_t GetRawMaxPos(const Int_t pixidx, const Int_t first, const Int_t last) const
    {
        UInt_t val;
        return GetRawMax(pixidx, first, last, val);
    }
    Int_t GetRawMaxPos(const Int_t pixidx, UInt_t &val) const
    {
        return GetRawMax(pixidx, 0, fNumSamples-1, val);
    }

    Int_t GetRawMaxPos(const Int_t pixidx) const
    {
        UInt_t val;
        return GetRawMax(pixidx, 0, fNumSamples-1, val);
    }

    UInt_t GetRawMaxVal(const Int_t idx, const Int_t first, const Int_t last) const
    {
        UInt_t val;
        GetRawMax(idx, first, last, val);
        return val;
    }

    void GetStat(const Int_t idx, Float_t &mean, Float_t &rms) const;

    Int_t GetSaturation(const Int_t pixidx, Int_t limit) const
    {
        Int_t first=0;
        Int_t last=fNumSamples-1;
        return GetSaturation(pixidx, limit, first, last);
    }

    Int_t GetIntegralRaw(Int_t idx, Int_t first, Int_t last) const
    {
        USample_t *ptr = GetSamplesRaw(idx);

        const USample_t *end = ptr + last - first + 1;

        Int_t sum = 0;
        while (ptr<end)
            sum += *ptr++;

        return sum;
    }

    void Print(Option_t *o="") const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void   DrawPixelContent(Int_t) const { }

    ClassDef(MPedestalSubtractedEvt, 6) //Container to store the raw Event Data
};

#endif
