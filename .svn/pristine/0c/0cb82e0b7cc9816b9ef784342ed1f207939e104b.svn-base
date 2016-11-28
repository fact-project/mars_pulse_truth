#ifndef MARS_MHPedestalCam
#define MARS_MHPedestalCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class MGeomCam;
class MPedestalCam;
class MHPedestalCam : public MHCalibrationCam
{

private:

  static const Int_t   fgNbins;           //! Default number of bins        (now set to: 100  )
  static const Axis_t  fgFirst;           //! Default lower histogram limit (now set to: -50. )
  static const Axis_t  fgLast;            //! Default upper histogram limit (now set to:  50. )
                                           
  static const TString gsHistName;        //! Default Histogram names
  static const TString gsHistTitle;       //! Default Histogram titles
  static const TString gsHistXTitle;      //! Default Histogram x-axis titles
  static const TString gsHistYTitle;      //! Default Histogram y-axis titles

  static const TString fgNamePedestalCam; //! "MPedestalCam"
  
  TString fNamePedestalCamOut;            // Name of the 'MPedestalCam' container with results

  ULong_t fNumEvents;                     // Number of processed events
  
  Float_t fExtractHiGainSlices;           // Number of FADC slices used for high gain signal extraction
  Float_t fExtractLoGainSlices;           // Number of FADC slices used for low  gain signal extraction

  MPedestalCam *fPedestalsOut;            //! Pedestal Cam with results
   
  Bool_t  fRenorm;                        // Flag if the results will be re-normalized

  Axis_t  fFitStart;                      // Charge value to start the fitting procedure
  
  MArrayD fSum;                           //! sum of values  
  MArrayD fSumSquare;                     //! sum of squared values
  MArrayD fAreaSum;                       //! averaged sum of values per area idx
  MArrayD fAreaSumSquare;                 //! averaged sum of squared values per area idx
  MArrayI fAreaNum;                       //! number of valid pixel with area idx
  MArrayI fSectorNum;                     //! number of valid pixel with sector idx
  MArrayD fSectorSum;                     //! averaged sum of values per sector
  MArrayD fSectorSumSquare;               //! averaged sum of squared values per sector
  
  Bool_t ReInitHists(MParList *pList);
  Bool_t FillHists(const MParContainer *par, const Stat_t w=1);
  Bool_t FinalizeHists();
  
  void   InitHiGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );
  
  void RenormResults();
  void FitHists();

public:

  MHPedestalCam(const char *name=NULL, const char *title=NULL);

 // Clone
  TObject *Clone(const char *name="") const;

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  void ResetHists();

  void SetFitStart          ( const Axis_t a=-0.5  ) { fFitStart = a;  }
  void SetNamePedestalCamOut( const char *name     ) { fNamePedestalCamOut = name; }
  void SetPedestalsOut      ( MPedestalCam *cam    ) { fPedestalsOut = cam; }
  void SetRenorm            ( const Bool_t b=kTRUE ) { fRenorm = b; }
  
  ClassDef(MHPedestalCam, 1)	// Histogram class for Charge Camera Pedestals 
};

#endif

