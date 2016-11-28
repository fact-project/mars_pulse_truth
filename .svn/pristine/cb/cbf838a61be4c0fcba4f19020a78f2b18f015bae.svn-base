#ifndef MARS_MCalibrationTestCalc
#define MARS_MCalibrationTestCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MCalibrationTestCalc                                                   //
//                                                                         //
// Integrates the time slices of the all pixels of a calibration event     //
// and substract the pedestal value                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MHCalibrationTestCam;
class MCalibrationTestCam;
class MBadPixelsCam;
class MGeomCam;

class MCalibrationTestCalc : public MTask
{
private:

  static const Float_t fgPhotErrLimit;  //! Default for fPhotErrLimit (now set to: 4.)

  // Variables
  Float_t fPhotErrLimit;                // Limit acceptance nr. cal. phots w.r.t. area idx mean (in sigmas)

  TString fOutputPath;                  // Path to the output file
  TString fOutputFile;                  // Name of the output file
  
  // Pointers
  MBadPixelsCam        *fBadPixels;     //! Bad Pixels
  MHCalibrationTestCam *fHTestCam;      //! Calibrated Photons in the camera
  MCalibrationTestCam  *fCam;           //! Storage Calibrated Photons in the camera
  MGeomCam             *fGeom;          //! Camera geometry

  // functions
  const char* GetOutputFile();

  void   FinalizeNotInterpolated();
  void   FinalizeCalibratedPhotons() const;
  Int_t  CalcMaxNumBadPixelsCluster();
  void   LoopNeighbours( const TArrayI &arr, TArrayI &known, Int_t &clustersize, const Int_t idx );
  
  Bool_t ReInit     (MParList *pList); 
  Int_t  PostProcess();

public:

  MCalibrationTestCalc(const char *name=NULL, const char *title=NULL);

  void SetOutputFile  ( TString file="TestCalibStat.txt" );
  void SetOutputPath  ( TString path="."                 );
  void SetPhotErrLimit( const Float_t f=fgPhotErrLimit   ) { fPhotErrLimit = f; }  
  
  ClassDef(MCalibrationTestCalc, 1)   // Task retrieving the results of MHCalibrationTestCam
};

#endif
