#ifndef MARS_MPhotonEvent
#define MARS_MPhotonEvent

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

#include <iosfwd>

using namespace std;

class MPhotonData;
class MCorsikaRunHeader;
class MCorsikaFormat;

class MPhotonEvent : public MParContainer
{
private:
    TClonesArray fData;

public:
    MPhotonEvent(const char *name=NULL, const char *title=NULL);

    void Sort(Bool_t force=kFALSE);
    Bool_t IsSorted() const { return fData.IsSorted(); }

    // Getter/Setter
    Int_t GetNumPhotons() const { return fData.GetEntriesFast(); }
    Int_t GetNumExternal() const;

    Float_t  GetTimeFirst() const;
    Float_t  GetTimeLast() const;
    Double_t GetTimeMedianDev() const;

    Double_t GetMeanX() const;
    Double_t GetMeanY() const;
    Double_t GetMeanT() const;

    TClonesArray &GetArray() { return fData; }
    const TClonesArray &GetArray() const { return fData; }

    Bool_t AsciiWrite(ostream &out) const;

    MPhotonData &Add(Int_t n);
    MPhotonData &Add();

    MPhotonData *GetFirst() const;
    MPhotonData *GetLast() const;

    MPhotonData &operator[](UInt_t idx);
    const MPhotonData &operator[](UInt_t idx) const;

    Int_t Shrink(Int_t n);
    void Resize(Int_t n);

    void AddXY(Double_t x, Double_t y);
    void SimWavelength(Float_t wmin, Float_t wmax);

    // I/O
    Int_t ReadEventIoEvt(MCorsikaFormat *fInFormat);
    Int_t ReadEventIoEvtCompact(MCorsikaFormat *fInFormat);
    Int_t ReadCorsikaEvt(Float_t * data, Int_t numEvents, Int_t arrayIdx);

    // TObject
    void Paint(Option_t *o="");
    void Print(Option_t * = NULL) const;
    //void Clear(Option_t * = NULL);

    ClassDef(MPhotonEvent, 1) //Container to store the raw Event Data
};

// FIXME: Should we merge this into MPhotonEvent?
class MPhotonStatistics : public MParContainer
{
private:
    Float_t fTimeFirst;  //! Start of (simulated) sampling window
    Float_t fTimeLast;   //! Start of (simulated) sampling window

    Float_t fLength;     // Time between first and last photon
    Float_t fTimeMedDev; // Median deviation

//    Float_t fOffset;
//    Float_t fWindow;

    Int_t fMaxIndex;     //!

public:
    MPhotonStatistics(const char *name=NULL, const char *title=NULL) : fMaxIndex(-1)
    {
        fName  = name  ? name  : "MPhotonStatistics";
        fTitle = title ? title : "Corsika Event Data Information";
    }

    void SetTime(Float_t first, Float_t last) { fTimeFirst=first; fTimeLast=last; }
    void SetLength(Float_t len) { fLength=len; }
    void SetMaxIndex(UInt_t idx) { fMaxIndex=idx; }
    void SetTimeMedDev(Float_t dev) { fTimeMedDev=dev; }

//    Float_t GetRawTimeFirst() const { return fTimeFirst; }
//    Float_t GetRawTimeLast() const { return fTimeLast; }

    Float_t GetTimeFirst() const { return fTimeFirst; }
    Float_t GetTimeLast() const { return fTimeLast; }

    Float_t GetLength() const { return fLength; }
    Float_t GetTimeMedDev() const { return fTimeMedDev; }

    Int_t GetMaxIndex() const { return fMaxIndex; }

//    Bool_t IsValid() const { return fTimeLast>=fTimeFirst; }

    ClassDef(MPhotonStatistics, 1)
};
#endif
