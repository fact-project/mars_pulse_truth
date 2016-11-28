#ifndef MARS_MExtractSignal3
#define MARS_MExtractSignal3

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;

class MPedestalCam;
class MExtractedSignalCam;

class MExtractSignal3 : public MTask
{
private:
    static const Byte_t fgSaturationLimit;
    static const Byte_t fgFirst;
    static const Byte_t fgLast;
    static const Byte_t fgWindowSize;
    static const Byte_t fgPeakSearchWindowSize;

    MPedestalCam        *fPedestals;    // Pedestals of all pixels in the camera
    MExtractedSignalCam *fSignals;      // Extracted signal of all pixels in the camera

    MRawEvtData         *fRawEvt;       // raw event data (time slices)
    MRawRunHeader       *fRunHeader;    // RunHeader information


    Byte_t  fPeakSearchWindowSize; // Size of FADC window in the search for the highest peak
                                   // of all pixels.

    Byte_t  fNumHiGainSamples;
    Byte_t  fNumLoGainSamples;

    Byte_t  fWindowSizeHiGain;  // Number of Hi Gain slices in window
    Float_t fWindowSqrtHiGain;  // Square root of number of Hi Gain slices in window

    Byte_t  fWindowSizeLoGain;  // Number of Lo Gain slices in window
    Float_t fWindowSqrtLoGain;  // Square root of number of Lo Gain slices in window

    Byte_t  fSaturationLimit;

    void   FindSignal(Byte_t *ptr, Byte_t window, Int_t &sum, Int_t &sat) const;

    void   FindPeak(Byte_t *ptr, Byte_t window, Byte_t &startslice, Int_t &signal, Int_t &sat) const;


    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MExtractSignal3(const char *name=NULL, const char *title=NULL);

    void SetWindows(Byte_t windowh=fgWindowSize, Byte_t windowl=fgWindowSize, 
		    Byte_t peaksearchwindow=fgPeakSearchWindowSize);

    void SetSaturationLimit(Byte_t lim) { fSaturationLimit = lim; }

    ClassDef(MExtractSignal3, 0) // Extracted Signal algorithm #3
};

#endif
