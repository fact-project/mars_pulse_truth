#ifndef MARS_MJCalibration
#define MARS_MJCalibration

#ifndef MARS_MJCalib
#include "MJCalib.h"
#endif
#ifndef MARS_MCalibrationChargeCam
#include "MCalibrationChargeCam.h"
#endif
#ifndef MARS_MCalibrationBlindCam
#include "MCalibrationBlindCam.h"
#endif
#ifndef MARS_MCalibrationRelTimeCam
#include "MCalibrationRelTimeCam.h"
#endif
#ifndef MARS_MCalibrationChargePINDiode
#include "MCalibrationChargePINDiode.h"
#endif
#ifndef MARS_MCalibrationQECam
#include "MCalibrationQECam.h"
#endif
#ifndef MARS_MBadPixelsCam
#include "MBadPixelsCam.h"
#endif
#ifndef MARS_MPedestalCam
#include "MPedestalCam.h"
#endif

class TH1D;
class TPaveText;

class MTask;
class MParList;
class MPedestalCam;
class MExtractor;
class MExtractTime;
class MEvtLoop;
class MExtractBlindPixel;
class MHCalibrationChargeBlindCam;
class MHCamera;

class MJCalibration : public MJCalib
{
private:

  static const Int_t gkIFAEBoxInaugurationRun;          //! Run number of first IFAE box calibration
  static const Int_t gkSecondBlindPixelInstallation;    //! Run number upon which second blind pixel was installed
  static const Int_t gkSpecialPixelsContInstallation;   //! Run number upon which third blind pixel was installed
  static const Int_t gkThirdBlindPixelInstallation;     //! Run number upon which third blind pixel was installed  

  static const TString  fgReferenceFile;   //! default for fReferenceFile ("mjobs/calibrationref.rc")
  static const TString  fgHiLoCalibFile;   //! default for fHiLoCalibFile ("mjobs/hilocalib.root")  

  Double_t fConvFADC2PheMin;               //! Histogram minimum for conversion factor to phes
  Double_t fConvFADC2PheMax;               //! Histogram maximum for conversion factor to phes
  Double_t fConvFADC2PhotMin;              //! Histogram minimum for conversion factor to phs
  Double_t fConvFADC2PhotMax;              //! Histogram maixmum for conversion factor to phs
  Double_t fQEMin;                         //! Histogram minimum for quantum efficiency
  Double_t fQEMax;                         //! Histogram maximum for quantum efficiency
  Double_t fArrivalTimeMin;                //! Histogram minimum for arrival times 
  Double_t fArrivalTimeMax;                //! Histogram maximum for arrival times
  Double_t fTimeOffsetMin;                 //! Histogram minimum for time offsets
  Double_t fTimeOffsetMax;                 //! Histogram maximum for time offsets
  Double_t fTimeResolutionMin;             //! Histogram minimum for time resolutions
  Double_t fTimeResolutionMax;             //! Histogram maximum for time resolutions
  					   
  Float_t  fRefFADC2PheInner;              //! Reference value for the ratio phes/charge - inner
  Float_t  fRefFADC2PheOuter;              //! Reference value for the ratio phes/charge - outer
  Float_t  fRefConvFADC2PhotInner;         //! Reference value for the conversion factor to phs - inner
  Float_t  fRefConvFADC2PhotOuter;         //! Reference value for the conversion factor to phs - outer
  Float_t  fRefConvFADC2PheInner;          //! Reference value for the conversion factor to phs - inner
  Float_t  fRefConvFADC2PheOuter;          //! Reference value for the conversion factor to phs - outer
  Float_t  fRefQEInner;                    //! Reference value for the quantum eff. cascades - inner
  Float_t  fRefQEOuter;                    //! Reference value for the quantum eff. cascades - outer
  Float_t  fRefArrivalTimeInner;           //! Reference value for the arrival time - inner
  Float_t  fRefArrivalTimeOuter;           //! Reference value for the arrival time - outer
  Float_t  fRefArrivalTimeRmsInner;        //! Reference value for the arrival time - inner
  Float_t  fRefArrivalTimeRmsOuter;        //! Reference value for the arrival time - outer
  Float_t  fRefTimeOffsetOuter;            //! Reference value for the time offset - outer
  Float_t  fRefTimeResolutionInner;  	   //! Reference value for the time resolution - inner
  Float_t  fRefTimeResolutionOuter;	   //! Reference value for the time resolution - outer

  TString  fReferenceFile;                 // File name containing the reference values
  TString  fHiLoCalibFile;                 // File name containing the hi-lo calib. constants

  MExtractor     *fExtractor;                           // Signal extractor
  MExtractTime   *fTimeExtractor;                       // Arrival Time extractor

  MBadPixelsCam                   fBadPixels;           // Bad Pixels cam, can be set from previous runs  
  MCalibrationChargeCam           fCalibrationCam;      // Calibration conversion factors FADC2Phe  
  MCalibrationBlindCam            fCalibrationBlindCam; // Calibration from Blind Pixel(s)  
  MCalibrationChargePINDiode      fCalibrationPINDiode; // Calibration from PIN Diode
  MCalibrationQECam               fQECam;               // Quantum efficiency, can be set from previous runs
  MCalibrationRelTimeCam          fRelTimeCam;          // Calibration constants rel. times

  MPedestalCam                    fExtractorCam;        // Pedestal Cam with extractor resolution

  MCalibrationCam::PulserColor_t fColor;                // Colour of the pulsed LEDs

  enum  Display_t                                       // Possible Display types
    {
      kFullDisplay,
      kDataCheckDisplay,
      kNormalDisplay
    };

  Display_t fDisplayType;                              // Chosen Display type

  enum { kRelTimes, kDebug };                          // Possible flags

  Byte_t fFlags;                                       // Bit-field for the flags

  UInt_t fMinEvents;                                   // Minimum number of events

  TString fGeometry;                                   // Class name geometry

  Bool_t IsRelTimes     () const { return TESTBIT(fFlags,kRelTimes);       }
  Bool_t IsDebug        () const { return TESTBIT(fFlags,kDebug);          }

  MHCamera *DrawBadPixelPad(const MHCamera &h, Bool_t unsuit) const;
  void DrawTab(MParList &plist, const char *cont, const char *name, Option_t *opt);
  
  void DisplayResult(MParList &plist);
  void DisplayReferenceLines(const MHCamera &hist, const Int_t what) const;
  //void   DisplayOutliers      ( TH1D     *hist, const char* whatsmall, const char* whatbig ) const;

  Bool_t WriteResult   (MParList &plist);
  //Bool_t WriteEventloop( MEvtLoop &evtloop   ) const;
  Bool_t WriteTasks    ( MTask *t1, MTask *t2) const;
  Bool_t CheckEnvLocal ();

  // WORKAROUNDS!!!
  void   InitBlindPixel(MExtractBlindPixel &blindext,
                        MHCalibrationChargeBlindCam &blindcam);

  const char* GetOutputFileName() const;

  void ReadReferenceFile();
  Bool_t ReadHiLoCalibFile();

public:
  MJCalibration(const char *name=NULL, const char *title=NULL);

  const MCalibrationChargeCam  &GetCalibrationCam() const { return fCalibrationCam; }
  const MCalibrationRelTimeCam &GetRelTimeCam()     const { return fRelTimeCam;     }
  const MCalibrationQECam      &GetQECam()          const { return fQECam;          }
  const MBadPixelsCam          &GetBadPixels()      const { return fBadPixels;      }

  // Containers and pointers
  void SetBadPixels    ( const MBadPixelsCam &bad)                   { bad.Copy(fBadPixels); }
  void SetColor        ( const MCalibrationCam::PulserColor_t color) { fColor = color;       }
  void SetExtractor    ( MExtractor* ext)                            { fExtractor = ext;     }
  void SetExtractorCam ( const MPedestalCam  &ped)                   { ped.Copy(fExtractorCam); fExtractorCam.SetName(ped.GetName()); }
  void SetQECam        ( const MCalibrationQECam &qe)                { qe.Copy(fQECam);      }
  void SetTimeExtractor( MExtractTime* ext)                          { fTimeExtractor = ext; }

  // Displays
  void SetFullDisplay()      { fDisplayType = kFullDisplay;      }
  void SetDataCheckDisplay() { fDisplayType = kDataCheckDisplay; }
  void SetNormalDisplay()    { fDisplayType = kNormalDisplay;    }

  // Camera Geomtry
  void SetGeometry(const char *geom) { fGeometry = geom; }

  // Flags
  void SetRelTimeCalibration( const Bool_t b=kTRUE ) { b ? SETBIT(fFlags,kRelTimes)  : CLRBIT(fFlags,kRelTimes); }
  void SetDebug             ( const Bool_t b=kTRUE ) { b ? SETBIT(fFlags,kDebug)     : CLRBIT(fFlags,kDebug); }
  //void SetIntensity         ( const Bool_t b=kTRUE ) { b ? SETBIT(fFlags,kIntensity) : CLRBIT(fFlags,kIntensity); }

  // Files
  void SetReferenceFile( const TString ref=fgReferenceFile ) { fReferenceFile = ref; }
  void SetHiLoCalibFile( const TString ref=fgHiLoCalibFile ) { fHiLoCalibFile = ref; }

  // Precessing
  Bool_t Process(MPedestalCam &pedcam);

  // Public helper functions
  static void DisplayDoubleProject(const MHCamera &cam);
  static void FixDataCheckHist(TH1D &hist, Double_t min=-1111, Double_t max=-1111);

  ClassDef(MJCalibration, 0) // Tool to run a calibration per pulser colour and intensity
};

#endif
