#ifndef MARS_MCalibrationQEPix
#define MARS_MCalibrationQEPix

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationQEPix : public MCalibrationPix
{
private:

  static const Float_t gkDefaultQEGreen;      //! Default QE at 520 nm (now set to: 0.192)
  static const Float_t gkDefaultQEBlue;       //! Default QE at 460 nm (now set to: 0.27 )
  static const Float_t gkDefaultQEUV;         //! Default QE at 370 nm (now set to: 0.285)
  static const Float_t gkDefaultQECT1;        //! Default QE at 370 nm (now set to: 0.285)
  static const Float_t gkDefaultQEGreenErr;   //! Uncertainty Def. QE  at 520 nm (now set to: 0.05)
  static const Float_t gkDefaultQEBlueErr;    //! Uncertainty Def. QE  at 460 nm (now set to: 0.07)
  static const Float_t gkDefaultQEUVErr;      //! Uncertainty Def. QE  at 370 nm (now set to: 0.07)
  static const Float_t gkDefaultQECT1Err;     //! Uncertainty Def. QE  at 370 nm (now set to: 0.07)
  static const Float_t gkLightGuidesEffGreen;    //! Default Light guides efficiency at 520 nm
  static const Float_t gkLightGuidesEffGreenErr; //! Uncertainty Def. Light guides efficiency at 520 nm
  static const Float_t gkLightGuidesEffBlue;     //! Default Light guides efficiency at 460 nm
  static const Float_t gkLightGuidesEffBlueErr;  //! Uncertainty Def. Light guides efficiency at 460 nm
  static const Float_t gkLightGuidesEffUV;       //! Default Light guides efficiency at 370 nm
  static const Float_t gkLightGuidesEffUVErr;    //! Uncertainty Def. Light guides efficiency at 370 nm
  static const Float_t gkLightGuidesEffCT1;      //! Default Light guides efficiency at 370 nm
  static const Float_t gkLightGuidesEffCT1Err;   //! Uncertainty Def. Light guides efficiency at 370 nm
  static const Float_t gkPMTCollectionEff;    //! Default Collection efficiency of the PMTs
  static const Float_t gkPMTCollectionEffErr; //! Uncertainty Def. Collection efficiency of the PMTs (0.01)
  
  MArrayF fQEBlindPixel;                     // Calibrated QEs    (Blind Pixel Method)
  MArrayF fQEBlindPixelVar;                  // Variance cal. QEs (Blind Pixel Method)
  MArrayF fQECombined;                       // Calibrated QEs    (Combined Method)
  MArrayF fQECombinedVar;                    // Variance cal. QEs (Combined Method)
  MArrayF fQEFFactor;                        // Calibrated QEs    (F-Factor Method)
  MArrayF fQEFFactorVar;                     // Variance cal. QEs (F-Factor Method)
  MArrayF fQEPINDiode;                       // Calibrated QEs    (PIN Diode Method)
  MArrayF fQEPINDiodeVar;                    // Variance cal. QEs (PIN Diode Method)
                                             
  Float_t fAvNormBlindPixel;                 // Normalization w.r.t. default QE (Blind Pixel Method)
  Float_t fAvNormBlindPixelVar;              // Variance norm. w.r.t. def. QE (Blind Pixel Method)
  Float_t fAvNormCombined;                   // Normalization w.r.t. default QE (Combined Method)
  Float_t fAvNormCombinedVar;                // Variance norm. w.r.t. def. QE (Combined Method)
  Float_t fAvNormFFactor;                    // Normalization w.r.t. default QE (F-Factor Method)
  Float_t fAvNormFFactorVar;                 // Variance norm. w.r.t. def. QE (F-Factor Method)
  Float_t fAvNormPINDiode;                   // Normalization w.r.t. default QE (PIN Diode Method)
  Float_t fAvNormPINDiodeVar;                // Variance norm. w.r.t. def. QE (PIN Diode Method)
  Float_t fAverageQE;                        // Average QE for Cascade spectrum (default 0.18)

  TArrayC fValidFlags;                       // Bit-field for valid flags, one array entry for each color
  Byte_t  fAvailableFlags;                   // Bit-field for available flags
  
  enum { kBlindPixelMethodValid, kFFactorMethodValid, 
	 kPINDiodeMethodValid, kCombinedMethodValid,
         kAverageQEBlindPixelAvailable, kAverageQEFFactorAvailable,
         kAverageQEPINDiodeAvailable, kAverageQECombinedAvailable  };

  void  AddAverageBlindPixelQEs( const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw );
  void  AddAverageFFactorQEs  ( const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw );
  void  AddAveragePINDiodeQEs  ( const MCalibrationCam::PulserColor_t col, Float_t &wav, Float_t &sumw );

  const Float_t GetAvNormBlindPixelRelVar()  const;
  const Float_t GetAvNormCombinedRelVar()  const;
  const Float_t GetAvNormFFactorRelVar()  const;
  const Float_t GetAvNormPINDiodeRelVar()  const;  

public:

  static const Float_t gkDefaultAverageQE;    //! Default QE folded into Cascade spectrum (now set to: 0.18)
  static const Float_t gkDefaultAverageQEErr; //! Uncertainty Def. QE Cascade spectrum    (now set to: 0.02)

  MCalibrationQEPix(const char *name=NULL, const char *title=NULL);
  ~MCalibrationQEPix() {}
  
  void Clear(Option_t *o="");
  void Copy (TObject& object) const;  

  // Getters
  const Float_t GetAverageQE() const;
  const Float_t GetAverageQERelVar() const;
  
  Float_t GetDefaultQE                   ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetDefaultQERelVar             ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetLightGuidesEff              ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetLightGuidesEffRelVar        ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetLightGuidesEff              ()                   const;
  Float_t GetLightGuidesEffRelVar        ()                   const;
  Float_t GetQEBlindPixel                ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetQEBlindPixelErr             ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetQEBlindPixelRelVar          ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetQECascadesBlindPixel        ()                   const;
  Float_t GetQECascadesBlindPixelErr     ()                   const;
  Float_t GetQECascadesBlindPixelVar     ()                   const;
  Float_t GetQECascadesCombined          ()                   const;  
  Float_t GetQECascadesCombinedErr       ()                   const;
  Float_t GetQECascadesCombinedVar       ()                   const;    
  Float_t GetQECascadesFFactor           ()                   const;  
  Float_t GetQECascadesFFactorErr        ()                   const;
  Float_t GetQECascadesFFactorVar        ()                   const;    
  Float_t GetQECascadesPINDiode          ()                   const;
  Float_t GetQECascadesPINDiodeErr       ()                   const;
  Float_t GetQECascadesPINDiodeVar       ()                   const;  
  Float_t GetQECombined                  ( const MCalibrationCam::PulserColor_t col ) const;        
  Float_t GetQECombinedErr               ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetQECombinedRelVar            ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetQEFFactor                   ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetQEFFactorErr                ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetQEFFactorRelVar             ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetQEPINDiode                  ( const MCalibrationCam::PulserColor_t col ) const;        
  Float_t GetQEPINDiodeErr               ( const MCalibrationCam::PulserColor_t col ) const;
  Float_t GetQEPINDiodeRelVar            ( const MCalibrationCam::PulserColor_t col ) const;  
  Float_t GetPMTCollectionEff()                                                       const;
  Float_t GetPMTCollectionEffRelVar()                                                 const;

  Bool_t  IsAverageQEBlindPixelAvailable ()                                           const;
  Bool_t  IsAverageQECombinedAvailable   ()                                           const;  
  Bool_t  IsAverageQEFFactorAvailable    ()                                           const;  
  Bool_t  IsAverageQEPINDiodeAvailable   ()                                           const;  
  Bool_t  IsBlindPixelMethodValid        ()                                           const;
  Bool_t  IsBlindPixelMethodValid        ( const MCalibrationCam::PulserColor_t col ) const;
  Bool_t  IsCombinedMethodValid          ()                                           const;  
  Bool_t  IsCombinedMethodValid          ( const MCalibrationCam::PulserColor_t col ) const;  
  Bool_t  IsFFactorMethodValid           ()                                           const;  
  Bool_t  IsFFactorMethodValid           ( const MCalibrationCam::PulserColor_t col ) const;  
  Bool_t  IsPINDiodeMethodValid          ()                                           const;  
  Bool_t  IsPINDiodeMethodValid          ( const MCalibrationCam::PulserColor_t col ) const;  

  // Setters 
  void SetAverageQE            ( const Float_t f )  { fAverageQE            = f; }
  void SetAvNormBlindPixel     ( const Float_t f )  { fAvNormBlindPixel     = f; }      
  void SetAvNormBlindPixelVar  ( const Float_t f )  { fAvNormBlindPixelVar  = f; }   
  void SetAvNormCombined       ( const Float_t f )  { fAvNormCombined       = f; }        
  void SetAvNormCombinedVar    ( const Float_t f )  { fAvNormCombinedVar    = f; }     
  void SetAvNormFFactor        ( const Float_t f )  { fAvNormFFactor        = f; }         
  void SetAvNormFFactorVar     ( const Float_t f )  { fAvNormFFactorVar     = f; }      
  void SetAvNormPINDiode       ( const Float_t f )  { fAvNormPINDiode       = f; }        
  void SetAvNormPINDiodeVar    ( const Float_t f )  { fAvNormPINDiodeVar    = f; }     
  void SetAverageQEBlindPixelAvailable   ( const Bool_t b=kTRUE );
  void SetAverageQECombinedAvailable     ( const Bool_t b=kTRUE );
  void SetAverageQEFFactorAvailable      ( const Bool_t b=kTRUE );
  void SetAverageQEPINDiodeAvailable     ( const Bool_t b=kTRUE );
  void SetBlindPixelMethodValid          ( const Bool_t b, const MCalibrationCam::PulserColor_t col);
  void SetCombinedMethodValid            ( const Bool_t b, const MCalibrationCam::PulserColor_t col);
  void SetFFactorMethodValid             ( const Bool_t b, const MCalibrationCam::PulserColor_t col);  
  void SetPINDiodeMethodValid            ( const Bool_t b, const MCalibrationCam::PulserColor_t col);  
  void SetQEBlindPixel    ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEBlindPixel   [col] = f; }
  void SetQEBlindPixelVar ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEBlindPixelVar[col] = f; }
  void SetQECombined      ( Float_t f, MCalibrationCam::PulserColor_t col) { fQECombined     [col] = f; }
  void SetQECombinedVar   ( Float_t f, MCalibrationCam::PulserColor_t col) { fQECombinedVar  [col] = f; }
  void SetQEFFactor       ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEFFactor      [col] = f; }
  void SetQEFFactorVar    ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEFFactorVar   [col] = f; }
  void SetQEPINDiode      ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEPINDiode     [col] = f; }
  void SetQEPINDiodeVar   ( Float_t f, MCalibrationCam::PulserColor_t col) { fQEPINDiodeVar  [col] = f; }

  // Updates
  Bool_t  UpdateBlindPixelMethod( const Float_t plex );
  Bool_t  UpdateCombinedMethod  ();
  Bool_t  UpdateFFactorMethod   ( const Float_t plex );
  Bool_t  UpdatePINDiodeMethod  ();

  ClassDef(MCalibrationQEPix, 3)     // Container Quantum Efficieny Calibration Results Pixel
};

#endif

