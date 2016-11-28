#ifndef MARS_MMcCalibrationUpdate
#define MARS_MMcCalibrationUpdate

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MCalibrateData
#include "MCalibrateData.h"
#endif

class MCalibrationChargeCam;
class MCalibrationQECam;
class MPedPhotCam;
class MGeomCam;
class MMcFadcHeader;
class MExtractedSignalCam;

class MMcCalibrationUpdate : public MTask
{
private:
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Bool_t ReInit(MParList *pList);

    MCalibrationChargeCam *fCalCam;
    MCalibrationQECam     *fQECam;    
    MPedPhotCam           *fPedPhotCam;
    MGeomCam              *fGeom;
    MMcFadcHeader         *fHeaderFadc;
    MExtractedSignalCam   *fSignalCam;

    Float_t fADC2PhElInner; // Conversion factor from ADC counts to photo-electrons
    Float_t fADC2PhElOuter; // for inner and outer pixels.

    Bool_t  fFillCalibrationCam;
    Bool_t  fOuterPixelsGainScaling;

    Float_t fAmplitude;      // FADC parameters from camera simulation (see camera manual)
    Float_t fAmplitudeOuter; // to be read from the MMcFadcHeader.
    Float_t fConversionHiLo; // Ratio of high to low gain to be used for low gain signal calibration.

    Float_t fHeaderLow2HiGainFactor; // Ratio of high to low gain, as read from the MMcFadcHeader
    Float_t fUserLow2HiGainFactor;   // Ratio of high to low gain, as set manually by the user


    Float_t fOuterPixelsLightCollection; 
    // Photon collection efficiency of outer pixels w.r.t. inner ones.

    MCalibrateData::SignalType_t fSignalType;  
    // Type defined in MCalibrateData.h. Indicates whether calibration is 
    // made in photons or in photoelectrons! Default: MCalibrateData::kPhe

    
public:
    MMcCalibrationUpdate(const char *name=NULL, const char *title=NULL);

    void SetOuterPixelsGainScaling(Bool_t tf) 
      { fOuterPixelsGainScaling = tf; }

    void SetUserLow2HiGainFactor(Float_t x) { fUserLow2HiGainFactor = x; }

    void SetSignalType (MCalibrateData::SignalType_t x) { fSignalType = x; }
    MCalibrateData::SignalType_t GetSignalType () const { return fSignalType; }

    ClassDef(MMcCalibrationUpdate, 0)   // Task which obtains, for MC files, the pedestal mean and rms, and the calibration factor from ADC counts to photons. 
};

#endif
