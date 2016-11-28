#ifndef MARS_MExtractAmplitudeSpline
#define MARS_MExtractAmplitudeSpline

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

class MExtractAmplitudeSpline : public MExtractor
{

private:
  
  static const Byte_t  fgHiGainFirst;    // Default for fHiGainFirst  (now set to: 2)
  static const Byte_t  fgHiGainLast;     // Default for fHiGainLast   (now set to: 14)
  static const Byte_t  fgLoGainFirst;    // Default for fLOGainFirst  (now set to: 3)
  static const Byte_t  fgLoGainLast;     // Default for fLoGainLast   (now set to: 14)
  static const Float_t fgResolution;     // Default for fResolution   (now set to: 0.003)

  Float_t *fHiGainSignal;                     // Need fast access to the signals in a float way
  Float_t *fLoGainSignal;                     // Need fast access to the signals in a float way  
  Float_t *fHiGainFirstDeriv;
  Float_t *fLoGainFirstDeriv;  
  Float_t *fHiGainSecondDeriv;
  Float_t *fLoGainSecondDeriv;  

  Float_t fResolution;                        // The time resolution in FADC units
  
  Bool_t ReInit    (MParList *pList);
  
  void   FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const;
  void   FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const;

public:

  MExtractAmplitudeSpline(const char *name=NULL, const char *title=NULL);
  ~MExtractAmplitudeSpline();
  
  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);
  void SetResolution(Float_t f=fgResolution)     { fResolution = f;  }
  
  ClassDef(MExtractAmplitudeSpline, 0)   // Task to Extract the Amplitude using a Fast Spline
};

#endif



