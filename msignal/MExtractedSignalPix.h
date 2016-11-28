#ifndef MARS_MExtractedSignalPix
#define MARS_MExtractedSignalPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MExtractedSignalPix : public MParContainer
{
private:

  Float_t fExtractedSignalHiGain;      // mean value of the extracted signal
  Float_t fExtractedSignalHiGainError; // error of the mean value of the extracted signal
  Float_t fExtractedSignalLoGain;      // mean value of the extracted signal
  Float_t fExtractedSignalLoGainError; // error of the mean value of the extracted signal

  Byte_t fNumHiGainSaturated;          // Number of how many hi-gain slices saturated
  Byte_t fNumLoGainSaturated;          // Number of how many lo-gain slices saturated

public:
  MExtractedSignalPix(const char* name=NULL, const char* title=NULL);

  void Clear(Option_t *o="");
  void Print(Option_t *o="") const;

  // Setter
  void SetExtractedSignal( Float_t sig, Float_t sigerr=0);
  void SetExtractedSignal( Float_t sighi, Float_t sighierr,Float_t siglo, Float_t sigloerr);
  void SetGainSaturation ( Byte_t higain, Byte_t logain=0);
    
  // Getter
  Float_t GetExtractedSignalHiGain()      const { return fExtractedSignalHiGain;       }
  Float_t GetExtractedSignalHiGainError() const { return fExtractedSignalHiGainError;  }

  Float_t GetExtractedSignalLoGain()      const { return fExtractedSignalLoGain;       }
  Float_t GetExtractedSignalLoGainError() const { return fExtractedSignalLoGainError;  }

  Byte_t GetNumHiGainSaturated()          const { return fNumHiGainSaturated; }
  Byte_t GetNumLoGainSaturated()          const { return fNumLoGainSaturated; }

  Bool_t IsHiGainSaturated()              const { return fNumHiGainSaturated>0;        }
  Bool_t IsLoGainSaturated()              const { return fNumLoGainSaturated>0;        }

  Bool_t IsLoGainValid()                  const { return fExtractedSignalLoGainError>=0; }
  Bool_t IsHiGainValid()                  const { return fExtractedSignalHiGainError>=0; }

  ClassDef(MExtractedSignalPix, 3) // Storage Container for Extracted Signal information of one pixel
};

#endif
