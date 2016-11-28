#ifndef MARS_MArrivalTimeCalc2
#define MARS_MArrivalTimeCalc2

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;

class MPedestalCam;
class MArrivalTimeCam;

class MArrivalTimeCalc2 : public MTask
{
private:
    static const Byte_t fgSaturationLimit;
    static const Byte_t fgFirst;
    static const Byte_t fgLast;
    static const Byte_t fgWindowSize;

    MPedestalCam        *fPedestals;    // Pedestals of all pixels in the camera

    MRawEvtData         *fRawEvt;       // raw event data (time slices)
    MRawRunHeader       *fRunHeader;    // RunHeader information

    
    MArrivalTimeCam        *fArrivalTime;  // Arrival Time of FADC sample
     
  
    Byte_t  fHiGainFirst;       // First hi gain to be used
    Byte_t  fLoGainFirst;       // First lo gain to be used

    Byte_t  fNumHiGainSamples;  // Number of hi gain to be used
    Byte_t  fNumLoGainSamples;  // Number of lo gain to be used

    Byte_t  fWindowSize;            // Number of gains in window
    Float_t fWindowSizeSqrt;        // Sqaure root of number of gains in window

    Byte_t  fSaturationLimit;

    void   FindSignalTime(Byte_t *ptr, Byte_t size, Float_t &time,  Float_t &deltatime, Int_t &sat, Float_t pedes, Float_t pedrms) const;

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MArrivalTimeCalc2(const char *name=NULL, const char *title=NULL);

    void SetRange(Byte_t hifirst=fgFirst, Byte_t hilast=fgLast, Byte_t lofirst=fgFirst, Byte_t lolast=fgLast, Byte_t windowsize=fgWindowSize);
    void SetSaturationLimit(Byte_t lim) { fSaturationLimit = lim; }

    ClassDef(MArrivalTimeCalc2, 0) // Calculates the arrival time as the mean time of the fWindowSize time slices
};

#endif


