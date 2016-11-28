#ifndef MARS_MHCalibrationTestTimeCam
#define MARS_MHCalibrationTestTimeCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

class MHCalibrationTestTimeCam : public MHCalibrationCam
{

private:

  static const Int_t   fgNbins;        //! Default for fNBins  (now set to: 600  )
  static const Axis_t  fgFirst;        //! Default for fFirst  (now set to: -0.5 )
  static const Axis_t  fgLast;         //! Default for fLast   (now set to: 29.5 )
  static const Float_t fgProbLimit;    //! Default for fProbLimit (now set to: 0.000001)  
  
  static const TString gsHistName;     //! Default Histogram names
  static const TString gsHistTitle;    //! Default Histogram titles
  static const TString gsHistXTitle;   //! Default Histogram x-axis titles
  static const TString gsHistYTitle;   //! Default Histogram y-axis titles
  
  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);
  Bool_t FinalizeHists();
  void    FinalizeBadPixels();
  
public:

  MHCalibrationTestTimeCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationTestTimeCam() {}

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  void CalcAverageSigma();
  
  ClassDef(MHCalibrationTestTimeCam, 0)	// Histogram class for Relative Time Camera Calibration
};

#endif

