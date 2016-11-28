#ifndef MARS_MExtractTimeFastSpline
#define MARS_MExtractTimeFastSpline

#ifndef MARS_MExtractTime
#include "MExtractTime.h"
#endif

class MPedestalPix;

class MExtractTimeFastSpline : public MExtractTime
{
private:
  static const Byte_t  fgHiGainFirst;    // Default for fHiGainFirst  (now set to: 2)
  static const Byte_t  fgHiGainLast;     // Default for fHiGainLast   (now set to: 14)
  static const Byte_t  fgLoGainFirst;    // Default for fLOGainFirst  (now set to: 3)
  static const Byte_t  fgLoGainLast;     // Default for fLoGainLast   (now set to: 14)
  static const Float_t fgResolution;     // Default for fResolution   (now set to: 0.003)
  static const Float_t fgRiseTime  ;     // Default for fRiseTime     (now set to: 1.5  )  

  Float_t *fHiGainFirstDeriv;            //!
  Float_t *fLoGainFirstDeriv;            //!
  Float_t *fHiGainSecondDeriv;           //!
  Float_t *fLoGainSecondDeriv;           //!

  Float_t fResolution;                   // The time resolution in FADC units
  Float_t fRiseTime  ;                   // The rise time of the pulse
  Byte_t  fStartBeforeMax;               // Slices to start searching for the halfmax before max
  
  void FindTimeHiGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;
  void FindTimeLoGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;

  Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:

  MExtractTimeFastSpline(const char *name=NULL, const char *title=NULL);
  ~MExtractTimeFastSpline();
    
  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);      
  void SetResolution(Float_t f=fgResolution)     { fResolution = f;  }
  void SetRiseTime  (Float_t f=fgRiseTime  )     { fRiseTime   = f; fStartBeforeMax = (Int_t)(1.5*fRiseTime); }  

  void Print(Option_t *o) const;
  
  ClassDef(MExtractTimeFastSpline, 1)   // Task to Extract the Arrival Times using a Fast Spline
};

#endif



