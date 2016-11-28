#ifndef MARS_MExtractTimeHighestIntegral
#define MARS_MExtractTimeHighestIntegral

#ifndef MARS_MExtractTime
#include "MExtractTime.h"
#endif

class MExtractTimeHighestIntegral : public MExtractTime
{
private:
  static const Byte_t fgHiGainFirst;
  static const Byte_t fgHiGainLast;
  static const Byte_t fgLoGainFirst;
  static const Byte_t fgLoGainLast;
  static const Byte_t fgHiGainWindowSize;
  static const Byte_t fgLoGainWindowSize;

  Byte_t  fHiGainWindowSize;            // Number of gains in window
  Float_t fHiGainWindowSizeSqrt;        // Sqaure root of number of gains in window
  Byte_t  fLoGainWindowSize;            // Number of gains in window
  Float_t fLoGainWindowSizeSqrt;        // Sqaure root of number of gains in window

  void FindTimeHiGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;
  void FindTimeLoGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;

  Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:
  MExtractTimeHighestIntegral(const char *name=NULL, const char *title=NULL);

  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);        
  void SetWindowSize(Byte_t windowh=fgHiGainWindowSize,
                     Byte_t windowl=fgLoGainWindowSize);

  void Print(Option_t *o="") const;

  ClassDef(MExtractTimeHighestIntegral, 1) // Task to Extract the Arrival Times As the mean time of the fWindowSize time slices
};

#endif


