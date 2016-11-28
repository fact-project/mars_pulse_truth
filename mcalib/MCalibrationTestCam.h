#ifndef MARS_MCalibrationTestCam
#define MARS_MCalibrationTestCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationTestCam : public MCalibrationCam
{
private:
  
  TArrayI fNumUninterpolated;             // Number uninterpolated Pixels per area index 
  Int_t   fNumUninterpolatedInMaxCluster; // Number of not interpolateable pixels in biggest cluster

  void Add(const UInt_t a, const UInt_t b);
  void AddArea(const UInt_t a, const UInt_t b);
  void AddSector(const UInt_t a, const UInt_t b);
  
public:

  MCalibrationTestCam(const char *name=NULL, const char *title=NULL);
  
  void Clear (Option_t *o="");
  
  const Int_t                GetNumUninterpolated ( Int_t aidx ) const { 
	return fNumUninterpolated[aidx];  }
  const Int_t                GetNumUninterplatedInMaxCluster ()  const { 
	return fNumUninterpolatedInMaxCluster; }
  Bool_t                     GetPixelContent      ( Double_t &val, Int_t idx, 
	                                            const MGeomCam &cam, Int_t type=0) const;
  
  void  InitAverageAreas       ( const UInt_t i);

  // Prints
  void  Print (Option_t *o="") const;

  // Setters   
  void  SetNumUninterpolated            ( const UInt_t i, const Int_t aidx );
  void  SetNumUninterpolatedInMaxCluster( const UInt_t i ) { fNumUninterpolatedInMaxCluster = i; }

  // Draw
  void DrawPixelContent( Int_t num) const {}
  
  ClassDef(MCalibrationTestCam, 1) // Container Test Calibration Results Camera
};

#endif
