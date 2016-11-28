#ifndef MARS_MHCalibrationChargeCam
#define MARS_MHCalibrationChargeCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class TH1F;
class MExtractedSignalCam;
class MCalibrationChargePix;
class MHCalibrationChargePix;
class MPedestalSubtractedEvt;

class MHCalibrationChargeCam : public MHCalibrationCam
{
private:

  static const Int_t   fgChargeHiGainNbins;          //! Nr. bins of HiGain Histograms
  static const Axis_t  fgChargeHiGainFirst;          //! First Bin of HiGain Histograms
  static const Axis_t  fgChargeHiGainLast;           //! Last Bin of HiGain Histograms
  static const Int_t   fgChargeLoGainNbins;          //! First Bin of LoGain Histograms
  static const Axis_t  fgChargeLoGainFirst;          //! First Bin of LoGain Histograms
  static const Axis_t  fgChargeLoGainLast;           //! Last Bin of LoGain Histograms
  static const Float_t fgProbLimit;                  //! The default for fProbLimit
  
  static const TString fgReferenceFile;              //! default for fReferenceFile

  static const TString gsHistName;                   //! Default Histogram names
  static const TString gsHistTitle;                  //! Default Histogram titles
  static const TString gsHistXTitle;                 //! Default Histogram x-axis titles
  static const TString gsHistYTitle;                 //! Default Histogram y-axis titles
  
  static const TString gsAbsHistName;                //! Default Histogram names abs.times
  static const TString gsAbsHistTitle;               //! Default Histogram titles abs.times
  static const TString gsAbsHistXTitle;              //! Default Histogram x-axis titles abs.times
  static const TString gsAbsHistYTitle;              //! Default Histogram y-axis titles abs.times
  
  static const Float_t fgNumHiGainSaturationLimit;   //! Default for fNumHiGainSaturationLimit
  static const Float_t fgNumLoGainSaturationLimit;   //! Default for fNumLoGainSaturationLimit
  static const Float_t fgNumLoGainBlackoutLimit;     //! Default for fNumLoGainBlackoutLimit (now at: 0.05)

  static const Float_t fgLoGainBlackoutLimit;        //! Default for low-gain blackout limit (now at: 3.5)
  static const Float_t fgLoGainPickupLimit;          //! Default for low-gian pickup limit   (now at: 3.5)

  static const Float_t fgTimeLowerLimit;             //! Default for fTimeLowerLimit
  static const Float_t fgTimeUpperLimit;             //! Default for fTimeUpperLimit
  
  Int_t   fLoGainNbins;                              // Number of LoGain bins
  Axis_t  fLoGainFirst;                              // Lower histogram limit low gain
  Axis_t  fLoGainLast;                               // Upper histogram limit low gain

  Float_t fNumLoGainBlackoutLimit;                   // Rel. amount blackout logain events until pixel is declared unsuitable

  TString fAbsHistName;                              // Histogram names abs.times
  TString fAbsHistTitle;                             // Histogram titles abs. times
  TString fAbsHistXTitle;                            // Histogram x-axis titles abs. times
  TString fAbsHistYTitle;                            // Histogram y-axis titles abs. times
  
  TString fReferenceFile;                            // File name containing the reference values

  Float_t fInnerRefCharge;                           // The reference mean arrival time inner pixels
  Float_t fOuterRefCharge;                           // The reference mean arrival time outer pixels

  MArrayD fSumhiarea  ;                              //!
  MArrayD fSumloarea  ;                              //!
  MArrayD fTimehiarea ;                              //!
  MArrayD fTimeloarea ;                              //!
  MArrayD fSumhisector;                              //!
  MArrayD fSumlosector;                              //!
  MArrayD fTimehisector;                             //!
  MArrayD fTimelosector;                             //!

  MArrayI fSathiarea  ;                              //!
  MArrayI fSatloarea  ;                              //!
  MArrayI fSathisector;                              //!
  MArrayI fSatlosector;                              //!

  Float_t fTimeLowerLimit;                           // Limit dist. to first signal slice (in units of FADC slices) 
  Float_t fTimeUpperLimit;                           // Limit dist. to last signal slice  (in units of FADC slices) 
  
  MPedestalSubtractedEvt *fRawEvt;                   //!  Raw event data
  MExtractedSignalCam    *fSignal;                   //!

  Bool_t SetupHists(const MParList *pList);
  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);

  void   InitHiGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );
  void   InitLoGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );

  void   FinalizeAbsTimes (MHCalibrationChargePix &hist, MCalibrationChargePix &pix, MBadPixelsPix &bad,
                           Int_t first, Int_t last);
  Bool_t FinalizeHists();
  void   FinalizeBadPixels();

  void   DrawDataCheckPixel(MHCalibrationChargePix &pix, const Float_t refline);
  void   DisplayRefLines   ( const TH1F *hist,           const Float_t refline) const;

  Int_t ReadEnv        ( const TEnv &env, TString prefix, Bool_t print);
  
public:

  MHCalibrationChargeCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationChargeCam() {}
  
  // Clone
  TObject *Clone(const char *name="") const;

  // Draw
  void   Draw(const Option_t *opt);

  void SetAbsHistName   ( const char *name )  { fAbsHistName   = name; }
  void SetAbsHistTitle  ( const char *name )  { fAbsHistTitle  = name; }
  void SetAbsHistXTitle ( const char *name )  { fAbsHistXTitle = name; }
  void SetAbsHistYTitle ( const char *name )  { fAbsHistYTitle = name; }

  void SetBinningLoGain(Int_t n, Axis_t lo, Axis_t up) { fLoGainNbins=n; fLoGainFirst=lo; fLoGainLast=up; }

  void SetNumLoGainBlackoutLimit( const Float_t f=fgNumLoGainBlackoutLimit ) { fNumLoGainBlackoutLimit = f; }

  void SetReferenceFile ( const TString ref=fgReferenceFile ) { fReferenceFile  = ref; }

  void SetTimeLowerLimit( const Float_t f=fgTimeLowerLimit )  { fTimeLowerLimit = f; }
  void SetTimeUpperLimit( const Float_t f=fgTimeUpperLimit )  { fTimeUpperLimit = f; }

  // MHCamEvent
  Bool_t GetPixelContent ( Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const { return kTRUE; }
  void   DrawPixelContent( Int_t num )  const;    

  ClassDef(MHCalibrationChargeCam, 3)	// Histogram class for Charge Camera Calibration
};

#endif
