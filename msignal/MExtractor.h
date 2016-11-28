#ifndef MARS_MExtractor
#define MARS_MExtractor

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MExtractor                                                              //
//                                                                         //
// Base class for the signal extractors                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;

class MPedestalSubtractedEvt;
class MExtractedSignalCam;

class MExtractor : public MTask
{
private:
  static const Float_t fgOffsetLoGain;     //! Default for fOffsetLoGain (now set to 1.51 (= 5ns)
  
  Bool_t  fNoiseCalculation;               //! Flag if extractor determines noise contribution from pedestal file.

  Float_t fResolutionPerPheHiGain;         // Extractor-dependent charge resolution per phe for high-gain (see TDAS-0502).
  Float_t fResolutionPerPheLoGain;         // Extractor-dependent charge resolution per phe for low-gain  (see TDAS-0502).
  
protected:
  static const UInt_t  fgSaturationLimit;  //! Default for fSaturationLimit (now set to: 254)
  static const char   *fgNameSignalCam;    //! "MExtractedSignalCam"
  
  Float_t fOffsetLoGain;                   // Offset of the low-gain signal w.r.t. the High-Gain slices

  MExtractedSignalCam *fSignals;           //! Extracted signal of all pixels in the camera

  MRawEvtData         *fRawEvt;            //! Raw event data (time slices)
  MRawRunHeader       *fRunHeader;         //! RunHeader information

  MPedestalSubtractedEvt *fSignal;         //!

  UShort_t fHiGainFirst;                   // First FADC slice nr. to extract the High Gain signal
  UShort_t fHiGainLast;                    // Last FADC slice nr. to extract the High Gain signal
  Int_t    fLoGainFirst;                   // First FADC slice nr. to extract the Low Gain signal
  Byte_t   fLoGainLast;                    // Last FADC slice nr. to extract the Low Gain signal
                                           
//  Byte_t   fHiLoLast;                      // Number of slices in fLoGainSamples counted for the High-Gain signal
                                           
  Float_t  fNumHiGainSamples;              // Number High Gain FADC slices used to extract the signal
  Float_t  fNumLoGainSamples;              // Number Low  Gain FADC slices used to extract the signal
                                           
  Float_t  fSqrtHiGainSamples;             // Sqrt. nr. High Gain FADC slices used to extract the signal
  Float_t  fSqrtLoGainSamples;             // Sqrt. nr. Low  Gain FADC slices used to extract the signal

  UInt_t   fSaturationLimit;               // Highest FADC slice value until being declared saturated

  TString  fNameSignalCam;                 // Name of the 'MExtractedSignalCam' container

  // MExtractor
  virtual void FindSignalHiGain(Byte_t */*firstused*/, Byte_t */*lowgain*/, Float_t &/*sum*/, Byte_t &/*sat*/) const { }
  virtual void FindSignalLoGain(Byte_t */*firstused*/, Float_t &/*sum*/, Byte_t &/*sat*/) const { }

  void SetResolutionPerPheHiGain(Float_t f) { fResolutionPerPheHiGain=f; }
  void SetResolutionPerPheLoGain(Float_t f) { fResolutionPerPheLoGain=f; }

  Int_t   PreProcessStd(MParList *pList);

  // MTask
  Int_t   PreProcess( MParList *pList );
  Bool_t  ReInit    ( MParList *pList );
  Int_t   Process   ();
  void    StreamPrimitive(std::ostream &out) const;
  Int_t   ReadEnv(const TEnv &env, TString prefix, Bool_t print);


public:
  MExtractor(const char *name=NULL, const char *title=NULL);
  
  // getter
  UShort_t GetHiGainFirst()      const { return fHiGainFirst;      }
  UShort_t GetHiGainLast ()      const { return fHiGainLast ;      }
  Int_t    GetLoGainFirst()      const { return fLoGainFirst;      }
  Byte_t   GetLoGainLast ()      const { return fLoGainLast ;      }
  Float_t  GetNumHiGainSamples() const { return fNumHiGainSamples; }
  Float_t  GetNumLoGainSamples() const { return fNumLoGainSamples; }
  Float_t  GetOffsetLoGain()     const { return fOffsetLoGain;     }
  Float_t  GetResolutionPerPheHiGain() const { return fResolutionPerPheHiGain; }
  Float_t  GetResolutionPerPheLoGain() const { return fResolutionPerPheLoGain; }
  UInt_t   GetSaturationLimit() const { return fSaturationLimit; }

  Bool_t  HasLoGain() const { return fLoGainFirst>0 || fLoGainLast>0; }
  Bool_t  IsNoiseCalculation () const { return fNoiseCalculation; }

  // Setter
  Float_t SetResolutionPerPheHiGain() const { return fResolutionPerPheHiGain; }
  Float_t SetResolutionPerPheLoGain() const { return fResolutionPerPheLoGain; }

  virtual void SetRange(UShort_t hifirst=0, UShort_t hilast=0, Int_t lofirst=0, Byte_t lolast=0);

  void SetOffsetLoGain    ( const Float_t  f=fgOffsetLoGain          ) { fOffsetLoGain     = f;    }
  void SetSaturationLimit ( const UInt_t lim=fgSaturationLimit       ) { fSaturationLimit  = lim;  }
  void SetNameSignalCam   ( const char *name=fgNameSignalCam ) { fNameSignalCam    = name; }
  void SetNoiseCalculation( const Bool_t   b=kTRUE                   ) { fNoiseCalculation = b;    }

  // TObject
  void Clear(Option_t * = "")
    {
      fHiGainFirst = fHiGainLast = fLoGainFirst = fLoGainLast = /*fHiLoLast =*/ 0;
    }

  void Print(Option_t *o="") const; //*MENU*

  ClassDef(MExtractor, 9) // Signal Extractor Base Class
};

#endif
