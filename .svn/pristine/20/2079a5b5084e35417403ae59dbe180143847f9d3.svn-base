#ifndef MARS_MExtractFixedWindow
#define MARS_MExtractFixedWindow

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MExtractFixedWindow                                                          //
//                                                                         //
// Integrates the time slices of the all pixels in a fixed window          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

class MExtractFixedWindow : public MExtractor
{
private:
  static const Byte_t fgHiGainFirst;     // First FADC slice Hi-Gain (currently set to: 3) 
  static const Byte_t fgHiGainLast;      // Last FADC slice Hi-Gain (currently set to: 14) 
  static const Byte_t fgLoGainFirst;     // First FADC slice Lo-Gain (currently set to: 3) 
  static const Byte_t fgLoGainLast;      // Last FADC slice Lo-Gain (currently set to: 14) 

  void   FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const;
  void   FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const;

  void   StreamPrimitive(ostream &out) const;

  Bool_t ReInit(MParList *pList);
  
public:

  MExtractFixedWindow(const char *name=NULL, const char *title=NULL);

  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);

  void Print(Option_t *o="") const;
  
  ClassDef(MExtractFixedWindow, 1) // Signal Extractor for a fixed extraction window
};

#endif
