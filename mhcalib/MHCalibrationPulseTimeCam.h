#ifndef MARS_MHCalibrationPulseTimeCam
#define MARS_MHCalibrationPulseTimeCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

class TH1F;
class MRawEvtData;
class MExtractedSignalCam;
class MHCalibrationChargePix;

class MHCalibrationPulseTimeCam : public MHCalibrationCam
{
private:

  static const UInt_t  fgSaturationLimit;      //! Default for fSaturationLimit (now set to: 245)
  static const UInt_t  fgLowerSignalLimit;     //! Default for fLowerSignalLimit
  static const Int_t   fgNumPixelsRequired;    //! Default for fNumPixelsRequired

  static const Int_t   fgHiGainNbins;          //! Nr. bins of HiGain Histograms  (now set to:  550  )
  static const Axis_t  fgHiGainFirst;          //! First Bin of HiGain Histograms (now set to: -100.5)
  static const Axis_t  fgHiGainLast;           //! Last Bin of HiGain Histograms  (now set to:  999.5)
  static const Float_t fgProbLimit;            //! The default for fProbLimit    (now set to: 0.00001)

  static const TString fgReferenceFile;        //! default for fReferenceFile ("mjobs/calibrationref.rc")

  static const TString gsHistName;             //! Default Histogram names
  static const TString gsHistTitle;            //! Default Histogram titles
  static const TString gsHistXTitle;           //! Default Histogram x-axis titles
  static const TString gsHistYTitle;           //! Default Histogram y-axis titles

  UInt_t  fSaturationLimit;                    // Highest FADC slice value until being declared saturated
  UInt_t  fLowerSignalLimit;                   // Lower signal limit for pulse time extraction
  Int_t   fNumPixelsRequired;                  // Minimum Number of pixels required for averageing

  TString fReferenceFile;                      // File name containing the reference values

  Float_t fInnerRefTime;                       // The reference mean arrival time inner pixels
  Float_t fOuterRefTime;                       // The reference mean arrival time outer pixels

  MExtractedSignalCam *fSignalCam;             //! Signal cam for extraction range
  MBadPixelsCam *fBadPixels;                   //! Bad Pixels
  
  void   InitHiGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );

  Bool_t SetupHists(const MParList *pList);
  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);

  Bool_t FinalizeHists();

  void   DrawDataCheckPixel(MHCalibrationPix &pix, const Float_t refline);
  void   DisplayRefLines   ( const TH1F *hist,           const Float_t refline) const;
  void   CalcHists         (MHCalibrationPix &hist, MCalibrationPix &pix) const;

  Int_t ReadEnv        ( const TEnv &env, TString prefix, Bool_t print);
  
public:

  MHCalibrationPulseTimeCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationPulseTimeCam() {}

  // Clone
  TObject *Clone(const char *name="") const;

  // Draw
  void   Draw(const Option_t *opt);

  void SetReferenceFile    ( const TString ref=fgReferenceFile     ) { fReferenceFile     = ref; }
  void SetSaturationLimit  ( const UInt_t  lim=fgSaturationLimit   ) { fSaturationLimit   = lim; }
  void SetLowerSignalLimit ( const UInt_t  lim=fgLowerSignalLimit  ) { fLowerSignalLimit  = lim; }
  void SetNumPixelsRequired( const Byte_t  lim=fgNumPixelsRequired ) { fNumPixelsRequired = lim; }

  Double_t GetNumEvents() const;


  Bool_t GetPixelContent ( Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const { return kTRUE; }
  void   DrawPixelContent( Int_t num )  const;    

  ClassDef(MHCalibrationPulseTimeCam, 2)	// Histogram class for Pulse Time Checks
};

#endif
