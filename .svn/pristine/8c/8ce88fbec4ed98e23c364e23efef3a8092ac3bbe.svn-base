#ifndef MARS_MArrivalTimeCalc
#define MARS_MArrivalTimeCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;
class MArrivalTimeCam;
class MPedestalCam;
class MArrivalTimeCalc : public MTask
{

  static const Byte_t fgSaturationLimit; // Default for fSaturationLimit
  Byte_t              fSaturationLimit; // Maximum FADC counts before being considered as saturated
  
  MRawEvtData    *fRawEvt;     // raw event data (time slices)
  MRawRunHeader  *fRunHeader;  // RunHeader information
  MPedestalCam   *fPedestals;  // pedestal information
  
  MArrivalTimeCam *fArrTime;   // Container with the photons arrival times
  
  Int_t PreProcess(MParList *pList);
  Int_t Process();
  Int_t PostProcess() {return kTRUE;}
  
  Float_t Calc(const Byte_t *fadcSamples, const Short_t nslices, const UInt_t idx);
  
public:
    MArrivalTimeCalc(const char *name=NULL, const char *title=NULL);
    ~MArrivalTimeCalc(){}
 
    void SetSaturationLimit(const Byte_t lim=fgSaturationLimit) { fSaturationLimit = lim; }
    
    ClassDef(MArrivalTimeCalc, 0)   // Task to calculate Arrival Times from raw data
};

#endif



