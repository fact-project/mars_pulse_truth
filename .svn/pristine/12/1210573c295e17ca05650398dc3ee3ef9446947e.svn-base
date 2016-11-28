#ifndef MARS_MDigitalSignal
#define MARS_MDigitalSignal

#ifndef ROOT_TObject
#include <TObject.h>
#endif

//#include <climits>

class MDigitalSignal : public TObject
{
    Double_t fStart;
    Double_t fLength;

    Int_t fIndex;

public:
    MDigitalSignal(Double_t start=0, Double_t len=1e12/*FLT_MAX*/) : fStart(start), fLength(len), fIndex(-1)
    {
    }

    MDigitalSignal(const MDigitalSignal &ttl) : TObject(ttl), fStart(ttl.fStart), fLength(ttl.fLength), fIndex(ttl.fIndex)
    {
    }

    MDigitalSignal(const MDigitalSignal &ttl1, const MDigitalSignal &ttl2);

    // Setter
    void SetIndex(Int_t idx=-1) { fIndex=idx; }

    // Getter
    Bool_t IsValid() const { return fLength>0; }

    Double_t GetStart() const  { return fStart; }
    Double_t GetEnd() const    { return fStart+fLength; }
    Double_t GetLength() const { return fLength; }

    Int_t GetIndex() const { return fIndex; }
    Bool_t IsConnected() const { return fIndex>=0; }

    // TObject
    Bool_t IsSortable() const { return kTRUE; }
    Int_t  Compare(const TObject *obj) const;
    void   Print(Option_t *o="") const;

    // MDigitalSignal
    Bool_t Overlap(const TObject &obj) const;
    Bool_t Combine(const TObject &obj);

    MDigitalSignal GetCoincidence(const MDigitalSignal &ttl) const
    {
        return MDigitalSignal(*this, ttl);
    }

    const MDigitalSignal operator&&(const MDigitalSignal &ttl) const
    {
        return GetCoincidence(ttl);
    }
    const MDigitalSignal operator||(const MDigitalSignal &ttl) const
    {
        MDigitalSignal cpy(ttl);
        return !cpy.Combine(*this) ? MDigitalSignal(0, -1) : cpy;
    }

    ClassDef(MDigitalSignal, 1) // A digital signal with a start time and a length
};

#endif
