#ifndef MARS_MCalibrateData
#define MARS_MCalibrateData

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MGeomCam;
class MBadPixelsCam;
class MPedestalCam;
class MPedPhotCam;
class MSignalCam;
class MCalibConstCam;
class MExtractedSignalCam;

class MCalibrationChargeCam;
class MCalibrationQECam;

class MCalibrateData : public MTask
{
public:
    enum CalibrationMode_t
    {
        kSkip       =  0,
        kNone       =  1,
        kFlatCharge =  2,
        kBlindPixel =  3,
        kFfactor    =  4,
        kPinDiode   =  5,
        kCombined   =  6,
        kDummy      =  7
    };

    enum PedestalType_t
    {
        kNo    = BIT(0),
        kRun   = BIT(1),
        kEvent = BIT(2)
    };

    enum SignalType_t
    {
        kPhe,
        kPhot
    };

    static const CalibrationMode_t gkDefault;

private:
  static const Float_t gkCalibConvMinLimit; //! Minimum limit for conv. factor
  static const Float_t gkCalibConvMaxLimit; //! Maximum limit for conv. factor

  Float_t  fCalibConvMinLimit;           // Minimum limit for conv. factor
  Float_t  fCalibConvMaxLimit;           // Maximum limit for conv. factor
    
  MGeomCam                       *fGeomCam;       //! Camera geometry container
  MBadPixelsCam                  *fBadPixels;     //! Bad Pixels information
  MCalibrationChargeCam          *fCalibrations;  //! Calibration constants
  MCalibrationQECam              *fQEs;           //! Quantum efficiencies
  MExtractedSignalCam            *fSignals;       //! Integrated charge in FADCs counts
  MSignalCam                     *fCerPhotEvt;    //! Cerenkov Photon Event used for calculation
  MCalibConstCam                 *fCalibConstCam; //! Temporary calib consts storage
  
  CalibrationMode_t fCalibrationMode;    // Flag defining the calibration mode (CalibrationMode_t)
  Byte_t   fPedestalFlag;                // Flags defining to calibrate the pedestal each event or each run
  Byte_t   fSignalType;                  // Flag defining the signal type (kphot or kphe)
  
  Float_t  fRenormFactor;                // Possible renormalization factor for signals (-> phes)
  Float_t  fScaleFactor;                 // Possible scale factor for signals

  TString  fFileNameScale;               // File name for list of scale factors

  TList   fNamesPedestal;                // Names of input and output pedestal conatainer
  TList   fPedestalCams;                 //! List of pointers to input MPedestalCam
  TList   fPedPhotCams;                  //! List of pointers to corresponding output MPedPhotCam

  MArrayF fCalibConsts;                  //! Array of calibration constants for each pixel, calculated only once!
  MArrayF fCalibFFactors;                //! Array of calibration F-Factors for each pixel, calculated only once!  
  MArrayF fHiLoConv;                     //! Array of calibration constants for each pixel, calculated only once!
  MArrayF fHiLoConvErr;                  //! Array of calibration F-Factors for each pixel, calculated only once!  

  Int_t  Calibrate(Bool_t data, Bool_t pedestal) const;

  Int_t  PreProcess(MParList *pList);
  Bool_t ReInit(MParList *pList);
  Int_t  Process();
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  void   StreamPrimitive(std::ostream &out) const;

public:
    MCalibrateData(CalibrationMode_t calmode=gkDefault,
                   const char *name=NULL, const char *title=NULL);

  void   AddPedestal(const char *name="Cam");
  void   AddPedestal(const char *pedestal, const char *pedphot);

  void   EnablePedestalType(PedestalType_t i)     { fPedestalFlag |=  i;      }

  //Int_t  GetSize() const { return fCalibConsts.GetSize(); }

  void   Print(Option_t *o="") const;

  void   SetPedestalFlag(PedestalType_t i=kRun)   { fPedestalFlag  =  i;      }
  Bool_t TestPedestalFlag(PedestalType_t i) const { return fPedestalFlag&i ? kTRUE : kFALSE; }
  
  void   SetCalibrationMode ( CalibrationMode_t calmode=gkDefault) { fCalibrationMode=calmode; }
  void   SetSignalType      ( SignalType_t      sigtype=kPhe    ) { fSignalType     =sigtype; }  

  void   SetCalibConvMinLimit( const Float_t f=gkCalibConvMinLimit ) { fCalibConvMinLimit = f; }
  void   SetCalibConvMaxLimit( const Float_t f=gkCalibConvMaxLimit ) { fCalibConvMaxLimit = f; }

  void   SetScaleFactor(const Float_t f=1) { fScaleFactor=f; }

  Bool_t UpdateConversionFactors( const MCalibrationChargeCam *updatecam=NULL);

  ClassDef(MCalibrateData, 1)   // Task to calibrate FADC counts into photons or photo-electrons
};

#endif   /* MCalibrateData */
