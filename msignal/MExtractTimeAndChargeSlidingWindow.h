#ifndef MARS_MExtractTimeAndChargeSlidingWindow
#define MARS_MExtractTimeAndChargeSlidingWindow

#ifndef MARS_MExtractTimeAndCharge
#include "MExtractTimeAndCharge.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MPedestalPix;

class MExtractTimeAndChargeSlidingWindow : public MExtractTimeAndCharge
{
private:
  static const Byte_t fgHiGainFirst;      //! Default for fHiGainFirst  (now set to: 2)
  static const Byte_t fgHiGainLast;       //! Default for fHiGainLast   (now set to: 14)
  static const Byte_t fgLoGainFirst;      //! Default for fLOGainFirst  (now set to: 2)
  static const Byte_t fgLoGainLast;       //! Default for fLoGainLast   (now set to: 14)
  static const Byte_t fgHiGainWindowSize; //! The extraction window Hi-Gain
  static const Byte_t fgLoGainWindowSize; //! The extraction window Lo-Gain

  Float_t fHiGainWindowSizeSqrt;         // Square root of number of gains in window
  Float_t fLoGainWindowSizeSqrt;         // Square root of number of gains in window
  
  MArrayF fHiGainSignal;                 //! Need fast access to the signals in a float way
  MArrayF fLoGainSignal;                 //! Store them in separate arrays

  Bool_t  InitArrays();
  
  Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:

  MExtractTimeAndChargeSlidingWindow(const char *name=NULL, const char *title=NULL);
  ~MExtractTimeAndChargeSlidingWindow() {}

  void SetRange    ( Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0 );  
  void SetWindowSize(Int_t windowh=fgHiGainWindowSize,
                     Int_t windowl=fgLoGainWindowSize);

  void FindTimeAndChargeHiGain(Byte_t *first, Byte_t *logain, Float_t &sum, Float_t &dsum,
                               Float_t &time, Float_t &dtime,
                               Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag);
  void FindTimeAndChargeLoGain(Byte_t *first, Float_t &sum,  Float_t &dsum,
                               Float_t &time, Float_t &dtime,
                               Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag);

  ClassDef(MExtractTimeAndChargeSlidingWindow, 1)   // Task to Extract Times and Charges using a Sliding Window
};

#endif



