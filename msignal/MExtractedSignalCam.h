#ifndef MARS_MExtractedSignalCam
#define MARS_MExtractedSignalCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class TClonesArray;
class MExtractedSignalPix;

class MExtractedSignalCam : public MParContainer, public MCamEvent
{
private:

  TClonesArray *fArray;                //-> FIXME: Change TClonesArray away from a pointer?

  Byte_t fFirstUsedSliceHiGain;        // First High Gain FADC used for extraction (incl.)
  Int_t  fFirstUsedSliceLoGain;        // First Low Gain FADC used for extraction (incl.)
  
  Byte_t fLastUsedSliceHiGain;         // Last High Gain FADC used for extraction (incl.)
  Byte_t fLastUsedSliceLoGain;         // Last Low Gain FADC used for extraction (incl.)
  
  Float_t fUsedWindowHiGain;           // High Gain FADC extraction window 
  Float_t fUsedWindowLoGain;           // Low Gain FADC extraction window 

public:
    MExtractedSignalCam(const char *name=NULL, const char *title=NULL);
    ~MExtractedSignalCam();

    void    Print(Option_t *o="") const;
    void    Clear(Option_t *o="");
    void    Reset() { Clear(); }

    void    InitSize(const UInt_t i);
    Int_t   GetSize () const;

    Byte_t  GetFirstUsedSliceHiGain()    const { return fFirstUsedSliceHiGain; }
    Int_t   GetFirstUsedSliceLoGain()    const { return fFirstUsedSliceLoGain; }
    Byte_t  GetLastUsedSliceHiGain()     const { return fLastUsedSliceHiGain;  }
    Byte_t  GetLastUsedSliceLoGain()     const { return fLastUsedSliceLoGain;  }

    Float_t GetNumUsedHiGainFADCSlices() const { return fUsedWindowHiGain;     }
    Float_t GetNumUsedLoGainFADCSlices() const { return fUsedWindowLoGain;     }

    void    SetLogStream     ( MLog *lg  );
    void    SetUsedFADCSlices(Byte_t firsth, Byte_t lasth, Float_t winh,
                              Int_t  firstl, Byte_t lastl, Float_t winl)
    {
      fFirstUsedSliceHiGain    = firsth;
      fLastUsedSliceHiGain     = lasth;
      fUsedWindowHiGain        = winh;
      fFirstUsedSliceLoGain    = firstl;
      fLastUsedSliceLoGain     = lastl;
      fUsedWindowLoGain        = winl;
    }

    MExtractedSignalPix &operator[](Int_t i);
    const MExtractedSignalPix &operator[](Int_t i) const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MExtractedSignalCam, 4)	// Storage Container for extracted signals in the camera
};

#endif

