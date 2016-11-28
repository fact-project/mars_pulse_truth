#ifndef MARS_MExtractedSignalPINDiode
#define MARS_MExtractedSignalPINDiode

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MExtractedSignalPINDiode : public MParContainer
{
private:

  Float_t fExtractedSignal;    // Extracted signal amplitude
  Float_t fExtractedSignalErr; // Error extracted signal amplitude
  Float_t fExtractedTime;      // Position of signal amplitude
  Float_t fExtractedTimeErr;   // Error position of signal amplitude
  Float_t fExtractedSigma;     // Width Gauss fit 
  Float_t fExtractedSigmaErr;  // Error of width
  Float_t fExtractedChi2;      // Chi2 Gauss fit   
 
  Byte_t fNumFADCSamples;      // Number of used FADC slices
  Byte_t fFirst;               // First FADC slice to start extraction
  
  Bool_t fSaturated;           // FADC saturation occurrance flag

public:

  MExtractedSignalPINDiode(const char* name=NULL, const char* title=NULL);
  
  void Clear(Option_t *o="");
  
  // Getter
  Float_t GetExtractedSignal()    const { return fExtractedSignal;       }
  Float_t GetExtractedSignalErr() const { return fExtractedSignalErr;    }
  Float_t GetExtractedTime()      const { return fExtractedTime;         }
  Float_t GetExtractedTimeErr()   const { return fExtractedTimeErr;      }
  Float_t GetExtractedSigma()     const { return fExtractedSigma;        }
  Float_t GetExtractedSigmaErr()  const { return fExtractedSigmaErr;     }
  Float_t GetExtractedChi2()      const { return fExtractedChi2;         }  
  Byte_t  GetNumFADCSamples()     const { return fNumFADCSamples;        }
  
  Bool_t  IsValid()    const;   
  
  // Print
  void Print(Option_t *o="") const;

  // Setter
  void SetExtractedSignal(const Float_t sig, const Float_t sigerr);
  void SetExtractedSigma(  const Float_t sig, const Float_t sigerr);
  void SetExtractedTime(  const Float_t sig, const Float_t sigerr);
  void SetExtractedChi2(  const Float_t chi ) { fExtractedChi2 = chi; }
  void SetSaturation  (   const Bool_t b=kTRUE) { fSaturated = b;  }
  void SetUsedFADCSlices( const Byte_t first, const Byte_t num);

  ClassDef(MExtractedSignalPINDiode, 2)	// Storage Container for Extracted Signal information of one pixel
};

#endif
