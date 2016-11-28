#ifndef MARS_MExtractTimeSpline
#define MARS_MExtractTimeSpline

#ifndef MARS_MExtractTime
#include "MExtractTime.h"
#endif

class MPedestalPix;

class MExtractTimeSpline : public MExtractTime
{
private:
  static const Byte_t fgHiGainFirst;
  static const Byte_t fgHiGainLast;
  static const Byte_t fgLoGainFirst;
  static const Byte_t fgLoGainLast;

  Byte_t *fXHiGain;
  Byte_t *fXLoGain;  
  
  void FindTimeHiGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;
  void FindTimeLoGain(Byte_t *first, Float_t &time, Float_t &dtime, Byte_t &sat, const MPedestalPix &ped) const;
  
public:

  MExtractTimeSpline(const char *name=NULL, const char *title=NULL);
  ~MExtractTimeSpline();
    
  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);      
  
  ClassDef(MExtractTimeSpline, 0)   // Task to Extract the Arrival Times using a Spline
};

#endif



