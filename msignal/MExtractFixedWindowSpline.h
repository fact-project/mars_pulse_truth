#ifndef MARS_MExtractFixedWindowSpline
#define MARS_MExtractFixedWindowSpline

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

#ifndef MARS_MArrayF   
#include "MArrayF.h"
#endif

class MExtractFixedWindowSpline : public MExtractor
{

private:
  
  static const Byte_t  fgHiGainFirst;    // Default for fHiGainFirst  (now set to: 2)
  static const Byte_t  fgHiGainLast;     // Default for fHiGainLast   (now set to: 14)
  static const Byte_t  fgLoGainFirst;    // Default for fLOGainFirst  (now set to: 3)
  static const Byte_t  fgLoGainLast;     // Default for fLoGainLast   (now set to: 14)

  MArrayF fHiGainFirstDeriv;             //! Temporary storage
  MArrayF fLoGainFirstDeriv;             //! Temporary storage
  MArrayF fHiGainSecondDeriv;            //! Temporary storage
  MArrayF fLoGainSecondDeriv;            //! Temporary storage

  Bool_t ReInit    (MParList *pList);
  
  void   FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const;
  void   FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const;

public:

  MExtractFixedWindowSpline(const char *name=NULL, const char *title=NULL);
  ~MExtractFixedWindowSpline() {}
  
  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);
  
  ClassDef(MExtractFixedWindowSpline, 0)   // Task to Extract the Arrival Times using a Fast Spline
};

#endif



