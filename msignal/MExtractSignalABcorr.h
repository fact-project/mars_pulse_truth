#ifndef MARS_MExtractSignalABcorr
#define MARS_MExtractSignalABcorr

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MExtractSignalABcorr                                                          //
//                                                                         //
// Integrates the time slices of the all pixels of a calibration event     //
// and substract the pedestal value                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MRawRunHeader;

class MPedestalCam;
class MExtractedSignalCam;

class MExtractSignalABcorr : public MTask
{
private:
    static const Byte_t fgSaturationLimit;
    static const Byte_t fgFirst;
    static const Byte_t fgLast;

    MPedestalCam        *fPedestals;    // Pedestals of all pixels in the camera
    MExtractedSignalCam *fSignals;      // Extracted signal of all pixels in the camera

    MRawEvtData         *fRawEvt;       // raw event data (time slices)
    MRawRunHeader       *fRunHeader;    // RunHeader information

    Byte_t  fHiGainFirst;
    Byte_t  fLoGainFirst;

    Byte_t  fNumHiGainSamples;
    Byte_t  fNumLoGainSamples;

    Float_t fSqrtHiGainSamples;
    Float_t fSqrtLoGainSamples;

    Byte_t  fSaturationLimit;

    void   FindSignal(Byte_t *ptr, Int_t size, Int_t &sum, Byte_t &sat) const;

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    void   StreamPrimitive(ostream &out) const;

public:
    MExtractSignalABcorr(const char *name=NULL, const char *title=NULL);

    void SetRange(Byte_t hifirst=fgFirst, Byte_t hilast=fgLast, Byte_t lofirst=fgFirst, Byte_t lolast=fgLast);
    void SetSaturationLimit(Byte_t lim) { fSaturationLimit = lim; }

    ClassDef(MExtractSignalABcorr, 0) // Task to fill the Extracted Signal Containers from raw data
};

#endif
