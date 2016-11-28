#ifndef MARS_MExtractTimeAndChargeDigitalFilterPeakSearch
#define MARS_MExtractTimeAndChargeDigitalFilterPeakSearch

#ifndef MARS_MExtractTimeAndChargeDigitalFilter
#include "MExtractTimeAndChargeDigitalFilter.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif

class TH1F;
class TH2F;
class MPedestalPix;
class MBadPixelsCam;

class MExtractTimeAndChargeDigitalFilterPeakSearch : public MExtractTimeAndChargeDigitalFilter
{
private:

  static const Byte_t fgHiGainFirst;           //! Default for fHiGainFirst           (now set to: 0)
  static const Byte_t fgHiGainLast;            //! Default for fHiGainLast            (now set to:18)
  static const Byte_t fgLoGainFirst;           //! Default for fLoGainFirst           (now set to: 2)
  static const Byte_t fgLoGainLast;            //! Default for fLoGainLast            (now set to:14)
  static const Byte_t fgOffsetLeftFromPeak;    //! Default for fOffsetLeftFromPeak    (now set to: 1)  
  static const Byte_t fgOffsetRightFromPeak;   //! Default for fOffsetRightFromPeak   (now set to: 2)  
  static const Byte_t fgPeakSearchWindowSize;  //! Default for fPeakSearchWindowSize  (now set to: 2)
  static const Int_t  fgHiGainFailureLimit;    //! Default for fHiGainMaxFailureLimit (now set to: 5)
  static const Int_t  fgLoGainFailureLimit;    //! Default for fHiGainMaxFailureLimit (now set to: 10)

  Byte_t  fOffsetLeftFromPeak;                 // Number of slices to start extraction before peak slice
  Byte_t  fOffsetRightFromPeak;                // Number of slices to stop  extraction after  peak slice
  Byte_t  fPeakSearchWindowSize;               // Size of FADC window in the search for the highest peak of all pixels.
  Int_t   fHiGainFailureLimit;                 // Limit for percentage of events ranging out of limits in high gain
  Int_t   fLoGainFailureLimit;                 // Limit for percentage of events ranging out of limits in low gain

  ULong_t fHiGainOutOfRangeLeft;               // Number of events out of range to the left side in high gain
  ULong_t fHiGainOutOfRangeRight;              // Number of events out of range to the right side in high gain
  ULong_t fLoGainOutOfRangeLeft;               // Number of events out of range to the left side in low gain
  ULong_t fLoGainOutOfRangeRight;              // Number of events out of range to the right side in low gain

  MBadPixelsCam *fBadPixels;                   // Bad Pixels Camera
  
  void    FindPeak(Byte_t *ptr, Byte_t *logain,
                   Byte_t &startslice, Int_t &max, Int_t &sat, Byte_t &satpos) const;

  Int_t   PreProcess(MParList *pList);
  Int_t   Process();
  Int_t   PostProcess();

  Int_t   ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:

  MExtractTimeAndChargeDigitalFilterPeakSearch(const char *name=NULL, const char *title=NULL);  
  ~MExtractTimeAndChargeDigitalFilterPeakSearch() { }
  
  void SetOffsetLeftFromPeak  ( Byte_t offset=fgOffsetLeftFromPeak   )  { fOffsetLeftFromPeak   = offset; }
  void SetOffsetRightFromPeak ( Byte_t offset=fgOffsetRightFromPeak  )  { fOffsetRightFromPeak  = offset; }
  void SetPeakSearchWindowSize( Byte_t size  =fgPeakSearchWindowSize )  { fPeakSearchWindowSize = size;   }
  void SetHiGainFailureLimit  ( Int_t  lim   =fgHiGainFailureLimit   )  { fHiGainFailureLimit   = lim;    }     
  void SetLoGainFailureLimit  ( Int_t  lim   =fgLoGainFailureLimit   )  { fLoGainFailureLimit   = lim;    }

  void Print(Option_t *o="") const;

  ClassDef(MExtractTimeAndChargeDigitalFilterPeakSearch, 1)   // Digital filter with global Peak Search
};

#endif
