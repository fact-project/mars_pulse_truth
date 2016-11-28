#ifndef MARS_MHCalibrationRelTimeCam
#define MARS_MHCalibrationRelTimeCam

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
class MGeomCam;
class MHCalibrationRelTimeCam : public MHCalibrationCam
{

private:

  static const Float_t fgNumHiGainSaturationLimit;   //! The default for fNumHiGainSaturationLimit (now at: 0.25)
  static const Int_t   fgNbins;                      //! Default number of bins        (now set to: 900   )
  static const Axis_t  fgFirst;                      //! Default lower histogram limit (now set to: -13.5 )
  static const Axis_t  fgLast;                       //! Default upper histogram limit (now set to:  13.5 )
  static const Float_t fgProbLimit;                  //! The default for fProbLimit    (now set to: 0.0)  
  
  static const TString gsHistName;                   //! Default Histogram names
  static const TString gsHistTitle;                  //! Default Histogram titles
  static const TString gsHistXTitle;                 //! Default Histogram x-axis titles
  static const TString gsHistYTitle;                 //! Default Histogram y-axis titles
  
  static const TString fgReferenceFile;              //! default for fReferenceFile ("mjobs/calibrationref.rc")

  Float_t fInnerRefTime;                             // The reference mean arrival time inner pixels
  Float_t fOuterRefTime;                             // The reference mean arrival time outer pixels

  TString  fReferenceFile;                           // File name containing the reference values

  MArrayD fSumareahi  ;                              //
  MArrayD fSumarealo  ;                              //
  MArrayD fSumsectorhi;                              //
  MArrayD fSumsectorlo;                              //
  MArrayI fNumareahi  ;                              //
  MArrayI fNumarealo  ;                              //
  MArrayI fNumsectorhi;                              //
  MArrayI fNumsectorlo;                              //

  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);
  Bool_t FinalizeHists();
  void   FinalizeBadPixels();

  void   CheckOverflow(MHCalibrationPix &pix) const;
  void   DrawDataCheckPixel(MHCalibrationPix &pix, const Float_t refline);
  void   DisplayRefLines( const TH1F *hist, const Float_t refline) const;

  Int_t  ReadEnv ( const TEnv &env, TString prefix, Bool_t print );
  
public:

  MHCalibrationRelTimeCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationRelTimeCam() {}

  // Clone
  TObject *Clone(const char *name="") const;

  // Setters
  void  SetReferenceFile ( const TString ref=fgReferenceFile ) { fReferenceFile  = ref; }

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  // Draw
  void Draw(const Option_t *opt);

  ClassDef(MHCalibrationRelTimeCam, 1)	// Histogram class for Relative Time Camera Calibration
};

#endif
