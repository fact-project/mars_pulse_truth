#ifndef MARS_MCalibrateRelTimes
#define MARS_MCalibrateRelTimes

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MBadPixelsCam;
class MCalibrationRelTimeCam;
class MArrivalTimeCam;
class MRawRunHeader;
class MSignalCam;

class MCalibrateRelTimes : public MTask
{
private:
    MCalibrationRelTimeCam *fCalibrations; // Calibration rel. time constants
    MBadPixelsCam          *fBadPixels;    // Bad Pixels information
    MArrivalTimeCam        *fSignals;      // Extracted Arrival Time
    MSignalCam             *fArrivalTime;  // Calibrated arrival times

    UShort_t fFreq;                        //! [MHz] Sampling Frequency

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:

    MCalibrateRelTimes(const char *name=NULL, const char *title=NULL);

    ClassDef(MCalibrateRelTimes, 0)   // Task to calculate calibrated relative arrival times
};
 

#endif
