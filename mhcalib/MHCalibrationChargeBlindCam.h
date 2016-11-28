
#ifndef MARS_MHCalibrationChargeBlindCam
#define MARS_MHCalibrationChargeBlindCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

#ifndef MARS_MHCalibrationChargeBlindPix
#include "MHCalibrationChargeBlindPix.h"
#endif

class MRawEvtData;
class MCalibrationBlindPix;
class MHCalibrationChargeBlindPix;
class MHCalibrationChargeBlindCam : public MHCalibrationCam
{
private:

  static const Int_t   fgNbins;        //! Default for fNBins new style blind pixel (now set to:  128   )
  static const Axis_t  fgFirst;        //! Default for fFirst new style blind pixel (now set to: - 0.5 )
  static const Axis_t  fgLast;         //! Default for fLast  new style blind pixel (now set to: 511.5 )

  static const Axis_t  fgSPheCut;      //! Default for fSinglePheCut new style blind pixel (now set to: 20)
  
  static const TString gsHistName;     //! Default Histogram names
  static const TString gsHistTitle;    //! Default Histogram titles
  static const TString gsHistXTitle;   //! Default Histogram x-axis titles
  static const TString gsHistYTitle;   //! Default Histogram y-axis titles

  Axis_t fSPheCut;                    // Signal value upon which event considered as single-phe
  
  MRawEvtData *fRawEvt;               //!  Raw event data

  MHCalibrationChargeBlindPix::FitFunc_t fFitFunc; // The actual fit function type
  
  Bool_t SetupHists   (const MParList *pList );
  Bool_t ReInitHists  (      MParList *pList );
  Bool_t FillHists    (const MParContainer *par, const Stat_t w=1 );
  Bool_t FinalizeHists();

  void   ResetHistTitles();
  void   FitBlindPixel( MHCalibrationChargeBlindPix &hist, MCalibrationBlindPix &pix);

  Int_t ReadEnv        ( const TEnv &env, TString prefix, Bool_t print);  
  
public:

  MHCalibrationChargeBlindCam(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationChargeBlindCam() {}
  
  // Draw
  void  Draw(Option_t *opt="");
  void  SetFitFunc( const MHCalibrationChargeBlindPix::FitFunc_t func )  { fFitFunc = func;  }
  void  SetSPheCut( const Axis_t    a   =fgSPheCut )  { fSPheCut = a;     }    
  
  ClassDef(MHCalibrationChargeBlindCam, 1)	// Histogram class for Blind Pixel Calibration
};

#endif









