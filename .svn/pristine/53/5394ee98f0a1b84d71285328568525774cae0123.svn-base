#ifndef MARS_MExtractSignal2
#define MARS_MExtractSignal2

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;

class MPedestalCam;
class MExtractedSignalCam;
//class MArrivalTime;

class MExtractSignal2 : public MTask
{
private:
    static const Byte_t fgSaturationLimit;
    static const Byte_t fgFirst;
    static const Byte_t fgLast;
    static const Byte_t fgWindowSize;

    MPedestalCam        *fPedestals;    // Pedestals of all pixels in the camera
    MExtractedSignalCam *fSignals;      // Extracted signal of all pixels in the camera

    MRawEvtData         *fRawEvt;       // raw event data (time slices)
    MRawRunHeader       *fRunHeader;    // RunHeader information

    /*
     MArrivalTime        *fArrivalTime;  // Arrival Time of FADC sample
     */
  
    Byte_t  fHiGainFirst;       // First hi gain to be used
    Byte_t  fLoGainFirst;       // First lo gain to be used

    Byte_t  fNumHiGainSamples;  // Number of hi gain to be used
    Byte_t  fNumLoGainSamples;  // Number of lo gain to be used

    Byte_t  fWindowSizeHiGain;  // Number of Hi Gain slices in window
    Float_t fWindowSqrtHiGain;  // Sqaure root of number of Hi Gain slices in window

    Byte_t  fWindowSizeLoGain;  // Number of Lo Gain slices in window
    Float_t fWindowSqrtLoGain;  // Sqaure root of number of Lo Gain slices in window

    Byte_t  fSaturationLimit;

    void   FindSignal(Byte_t *ptr, Byte_t size, Byte_t window, Int_t &max, Int_t &sat) const;

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MExtractSignal2(const char *name=NULL, const char *title=NULL);

    void SetRange(Byte_t hifirst=fgFirst, Byte_t hilast=fgLast, Byte_t windowh=fgWindowSize,
                  Byte_t lofirst=fgFirst, Byte_t lolast=fgLast, Byte_t windowl=fgWindowSize);
    void SetSaturationLimit(Byte_t lim) { fSaturationLimit = lim; }

    ClassDef(MExtractSignal2, 0) // Extracted Signal as highest integral content
};

#endif
