#ifndef MARS_MExtractSlidingWindow
#define MARS_MExtractSlidingWindow

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

class MExtractSlidingWindow : public MExtractor
{
private:
  
  static const Byte_t fgHiGainFirst;      // First FADC slice Hi-Gain (currently set to: 3) 
  static const Byte_t fgHiGainLast;       // Last FADC slice Hi-Gain (currently set to: 14) 
  static const Byte_t fgLoGainFirst;      // First FADC slice Lo-Gain (currently set to: 3) 
  static const Byte_t fgLoGainLast;       // Last FADC slice Lo-Gain (currently set to: 14) 
  static const Byte_t fgHiGainWindowSize; // The extraction window Hi-Gain
  static const Byte_t fgLoGainWindowSize; // The extraction window Lo-Gain

  Byte_t  fWindowSizeHiGain;             // Number of Hi Gain slices in window
  Byte_t  fWindowSizeLoGain;             // Number of Lo Gain slices in window  
  
  void   FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &max, Byte_t &sat) const;
  void   FindSignalLoGain(Byte_t *ptr, Float_t &max, Byte_t &sat) const;  

  Bool_t  ReInit(MParList *pList);
  
public:
  MExtractSlidingWindow(const char *name=NULL, const char *title=NULL);

  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);
  void SetWindowSize(Byte_t windowh=fgHiGainWindowSize,
                     Byte_t windowl=fgLoGainWindowSize);

  ClassDef(MExtractSlidingWindow, 1) // Signal Extractor for sliding extraction window
};

#endif
