#ifndef MARS_MHCalibrationTestCam
#define MARS_MHCalibrationTestCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

#ifndef MARS_MArrayF
#include <MArrayF.h>
#endif
#ifndef MARS_MArrayI
#include <MArrayI.h>
#endif

class MHCalibrationTestCam : public MHCalibrationCam
{

private:

  static const Int_t   fgNbins;        //! Default for fNBins  (now set to: 2000  )
  static const Axis_t  fgFirst;        //! Default for fFirst  (now set to: -0.5  )
  static const Axis_t  fgLast;         //! Default for fLast   (now set to: 1999.5)
  static const Float_t fgProbLimit;    //! The default for fProbLimit (now set to: 0.0000001)  
  
  static const TString gsHistName;     //! Default Histogram names
  static const TString gsHistTitle;    //! Default Histogram titles
  static const TString gsHistXTitle;   //! Default Histogram x-axis titles
  static const TString gsHistYTitle;   //! Default Histogram y-axis titles
  
  MArrayF fMeanMeanPhotPerArea;
  MArrayF fRmsMeanPhotPerArea   ;
  MArrayF fMeanSigmaPhotPerArea;
  MArrayF fRmsSigmaPhotPerArea   ;

  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);
  Bool_t FinalizeHists();

public:

  MHCalibrationTestCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationTestCam() {}

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  const Float_t  GetMeanMeanPhotPerArea  ( const Int_t aidx ) const { return fMeanMeanPhotPerArea  [aidx]; }
  const Float_t  GetMeanSigmaPhotPerArea ( const Int_t aidx ) const { return fMeanSigmaPhotPerArea [aidx]; }
  const Float_t  GetRmsMeanPhotPerArea   ( const Int_t aidx ) const { return fRmsMeanPhotPerArea   [aidx]; }
  const Float_t  GetRmsSigmaPhotPerArea  ( const Int_t aidx ) const { return fRmsSigmaPhotPerArea  [aidx]; }

  void CalcAverageSigma();
  
  ClassDef(MHCalibrationTestCam, 1)	// Histogram class for Relative Time Camera Calibration
};

#endif

