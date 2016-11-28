#ifndef MARS_MCalibrateDrsTimes
#define MARS_MCalibrateDrsTimes

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MBadPixelsCam;
class MDrsCalibrationTime;
class MArrivalTimeCam;
class MRawRunHeader;
class MRawEvtData;
class MSignalCam;

class MCalibrateDrsTimes : public MTask
{
private:
    MRawRunHeader       *fRunHeader;
    MRawEvtData         *fRaw;
    MDrsCalibrationTime *fCalib; // Calibration rel. time constants
    MBadPixelsCam       *fBadPixels;    // Bad Pixels information
    MArrivalTimeCam     *fSignals;      // Extracted Arrival Time
    MSignalCam          *fArrivalTime;  // Calibrated arrival times
    MSignalCam          *fArrivalTimeU; // Uncalibrated arrival times

    UShort_t fFreq;                        //! [MHz] Sampling Frequency

    TString fNameArrivalTime;
    TString fNameCalibrated;
    TString fNameUncalibrated;

    Bool_t fIsTimeMarker;

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MCalibrateDrsTimes(const char *name=NULL, const char *title=NULL);

    void SetNameArrivalTime(const char *name) { fNameArrivalTime = name; }
    void SetNameCalibrated(const char *name) { fNameCalibrated = name; }
    void SetNameUncalibrated(const char *name) { fNameUncalibrated = name; }

    void SetTimeMarker(Bool_t tm=kTRUE) { fIsTimeMarker = tm; }

    ClassDef(MCalibrateDrsTimes, 0)   // Task to calculate calibrated relative arrival times
};
 

#endif
