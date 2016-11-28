#ifndef MARS_MArrivalTimeCam
#define MARS_MArrivalTimeCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class TClonesArray;
class MArrivalTimePix;

class MArrivalTimeCam : public MParContainer, public MCamEvent
{
private:
    TClonesArray *fArray; //-> FIXME: Change TClonesArray away from a pointer?

//    Byte_t fFirstUsedSliceHiGain;
//    Byte_t fFirstUsedSliceLoGain;

//    Byte_t fLastUsedSliceHiGain;
//    Byte_t fLastUsedSliceLoGain;

public:

    MArrivalTimeCam(const char *name=NULL, const char *title=NULL);
    ~MArrivalTimeCam();

    void Print(Option_t *o="") const;
    void Clear(Option_t *o="");
    void SetLogStream(MLog *lg);
    void Reset() { Clear(); }

    void InitSize(const UInt_t i);
    Int_t GetSize() const;

//    Byte_t GetNumUsedFADCSlices() const       { return fLastUsedSliceHiGain-fFirstUsedSliceHiGain+1; }
//    Byte_t GetNumUsedHiGainFADCSlices() const { return fLastUsedSliceHiGain-fFirstUsedSliceHiGain+1; }
//    Byte_t GetNumUsedLoGainFADCSlices() const { return fLastUsedSliceLoGain-fFirstUsedSliceLoGain+1; }

//    Byte_t GetFirstUsedSliceHiGain() const    { return fFirstUsedSliceHiGain; }
//    Byte_t GetLastUsedSliceHiGain() const     { return fLastUsedSliceHiGain; }

//    Byte_t GetFirstUsedSliceLoGain() const    { return fFirstUsedSliceLoGain; }
//    Byte_t GetLastUsedSliceLoGain() const     { return fLastUsedSliceLoGain; }
/*
    void   SetUsedFADCSlices(Byte_t firsth, Byte_t lasth, 
                             Byte_t firstl, Byte_t lastl)
    {
      fFirstUsedSliceHiGain    = firsth;
      fLastUsedSliceHiGain     = lasth;
      fFirstUsedSliceLoGain    = firstl;
      fLastUsedSliceLoGain     = lastl;
    }
  */
    MArrivalTimePix &operator[](Int_t i);
    const MArrivalTimePix &operator[](Int_t i) const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MArrivalTimeCam, 2)	// Storage Container for the ArrivalTime in the camera
};

#endif

