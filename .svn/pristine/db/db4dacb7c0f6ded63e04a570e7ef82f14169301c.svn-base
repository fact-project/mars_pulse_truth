#ifndef MARS_MCalibrationBlindPix
#define MARS_MCalibrationBlindPix

#ifndef ROOT_MArrayF
#include <MArrayF.h>
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

class MCalibrationBlindPix : public MCalibrationPix
{
private:

  static const Float_t fgArea;         //! The Blind Pixel area in mm^2
  static const Float_t fgAttGreen;     //! Attenuation Filter at 520 nm 
  static const Float_t fgAttBlue ;     //! Attenuation Filter at 460 nm 
  static const Float_t fgAttUV   ;     //! Attenuation Filter at 370 nm 
  static const Float_t fgAttCT1  ;     //! Attenuation Filter at 370 nm
  static const Float_t fgAttErr;       //! Error Att. Filter at all w.l.
  static const Float_t fgQEGreen;      //! Quantum Efficiency at 520 nm
  static const Float_t fgQEBlue ;      //! Quantum Efficiency at 460 nm
  static const Float_t fgQEUV   ;      //! Quantum Efficiency at 370 nm
  static const Float_t fgQECT1  ;      //! Quantum Efficiency at 370 nm
  static const Float_t fgQEErrGreen;   //! Uncertainty QEUnCoated at 520 nm 
  static const Float_t fgQEErrBlue;    //! Uncertainty QEUnCoated at 460 nm 
  static const Float_t fgQEErrUV   ;   //! Uncertainty QEUnCoated at 370 nm 
  static const Float_t fgQEErrCT1  ;   //! Uncertainty QEUnCoated at 370 nmu
  static const Float_t fgCollEffGreen; //! Collecttion Efficiency
  static const Float_t fgCollEffBlue;  //! Collecttion Efficiency
  static const Float_t fgCollEffUV;    //! Collecttion Efficiency
  static const Float_t fgCollEffCT1;   //! Collecttion Efficiency
  static const Float_t fgCollEffErr;   //! Uncertainty Collection Efficiency

  Float_t fArea;                       // Blind Pixel Area
  Float_t fAreaErr;                    // Blind Pixel Area Error
  MArrayF fAtt;                        // Attenuation filter (per color)
  MArrayF fAttErr;                     // Error attnuation filter (per color)
  MArrayF fQE;                         // Quantum eff. (per color)
  MArrayF fQEErr;                      // Error Quantum eff. (per color)
  MArrayF fCollEff;                    // Coll eff. (per color)
  MArrayF fCollEffErr;                 // Error coll. eff. (per color)
                                        
  Float_t fLambda;                     // Mean Poisson fit
  Float_t fLambdaCheck;                // Mean Pedestal Check (Gauss) fit
  Float_t fLambdaCheckErr;             // Error mean pedestal Check fit
  Float_t fLambdaVar;                  // Variance lambda Poisson fit
  Float_t fFluxInsidePlexiglass;       // Number photons in INNER PIXEL inside the plexiglass
  Float_t fFluxInsidePlexiglassVar;    // Variance number of photons in INNER PIXEL 
  Float_t fMu0;                        // Position pedestal peak
  Float_t fMu0Err;                     // Error pos. pedestal-peak
  Float_t fMu1;                        // Position first photo-electron peak
  Float_t fMu1Err;                     // Error pos. first photo-electon peak
  Float_t fSigma0;                     // Width pedestal peak
  Float_t fSigma0Err;                  // Error width pedestal peak
  Float_t fSigma1;                     // Width first photo-electron peak  
  Float_t fSigma1Err;                  // Error width first photo-electron peak  

  enum { kOscillating, kPedestalFitOK, kSinglePheFitOK, kChargeFitValid, 
         kFluxInsidePlexiglassAvailable };   // Possible validity flags 

  MCalibrationCam::PulserColor_t fColor;     // Colour of the used pulser light

public:

  MCalibrationBlindPix(const char *name=NULL, const char *title=NULL);
  
  Bool_t CalcFluxInsidePlexiglass();
  void   Clear(Option_t *o="");
  void   Copy(TObject& object) const;
  
  // Getters
  const Float_t GetAreaRelVar   () const;
  const Float_t GetAtt          () const; 
  const Float_t GetAttRelVar    () const; 
  const Float_t GetQE           () const; 
  const Float_t GetQERelVar     () const; 
  const Float_t GetCollEff      () const;
  const Float_t GetCollEffRelVar() const;   

  const MCalibrationCam::PulserColor_t GetColor () const { return fColor;                }
  const Float_t GetLambda                       () const { return fLambda;               }
  const Float_t GetLambdaErr                    () const;
  const Float_t GetLambdaRelVar                 () const;  
  const Float_t GetLambdaCheck                  () const { return fLambdaCheck;          }
  const Float_t GetLambdaCheckErr               () const { return fLambdaCheckErr;       }
  const Float_t GetFluxInsidePlexiglass         () const { return fFluxInsidePlexiglass; }
  const Float_t GetFluxInsidePlexiglassErr      () const;
  const Float_t GetFluxInsidePlexiglassRelVar   () const;  
  const Float_t GetMu0                          () const { return fMu0;                  }
  const Float_t GetMu0Err                       () const { return fMu0Err;               }
  const Float_t GetMu1                          () const { return fMu1;                  }
  const Float_t GetMu1Err                       () const { return fMu1Err;               }
  const Float_t GetSigma0                       () const { return fSigma0;               }
  const Float_t GetSigma0Err                    () const { return fSigma0Err;            } 
  const Float_t GetSigma1                       () const { return fSigma1;               }
  const Float_t GetSigma1Err                    () const { return fSigma1Err;            }

  const Bool_t  IsOscillating                   () const;
  const Bool_t  IsChargeFitValid                () const;
  const Bool_t  IsPedestalFitOK                 () const;
  const Bool_t  IsSinglePheFitOK                () const;
  const Bool_t  IsFluxInsidePlexiglassAvailable () const;

  void Print(Option_t *opt=NULL) const;
  
  // Setters
  void SetArea      ( Float_t f )               { fArea    = f; }
  void SetAreaErr   ( Float_t f )               { fAreaErr = f; }  
  
  void SetAtt       ( Int_t n, Float_t *array ) { fAtt       .Set(n,array); }
  void SetAttErr    ( Int_t n, Float_t *array ) { fAttErr    .Set(n,array); }
  void SetQE        ( Int_t n, Float_t *array ) { fQE        .Set(n,array); }
  void SetQEErr     ( Int_t n, Float_t *array ) { fQEErr     .Set(n,array); }
  void SetCollEff   ( Int_t n, Float_t *array ) { fCollEff   .Set(n,array); }
  void SetCollEffErr( Int_t n, Float_t *array ) { fCollEffErr.Set(n,array); }
  
  void SetColor          ( const MCalibrationCam::PulserColor_t color ) { fColor      = color; }
  void SetLambda         ( const Float_t f )                            { fLambda         = f; }
  void SetLambdaVar      ( const Float_t f )                            { fLambdaVar      = f; }
  void SetLambdaCheck    ( const Float_t f )                            { fLambdaCheck    = f; }
  void SetLambdaCheckErr ( const Float_t f )                            { fLambdaCheckErr = f; }
  void SetMu0            ( const Float_t f )                            { fMu0            = f; }
  void SetMu0Err         ( const Float_t f )                            { fMu0Err         = f; }
  void SetMu1            ( const Float_t f )                            { fMu1            = f; }
  void SetMu1Err         ( const Float_t f )                            { fMu1Err         = f; }
  void SetSigma0         ( const Float_t f )                            { fSigma0         = f; }
  void SetSigma0Err      ( const Float_t f )                            { fSigma0Err      = f; }
  void SetSigma1         ( const Float_t f )                            { fSigma1         = f; }
  void SetSigma1Err      ( const Float_t f )                            { fSigma1Err      = f; }

  void SetOscillating    ( const Bool_t  b=kTRUE );
  void SetChargeFitValid ( const Bool_t  b=kTRUE );
  void SetPedestalFitOK  ( const Bool_t  b=kTRUE );
  void SetSinglePheFitOK ( const Bool_t  b=kTRUE );
  void SetFluxInsidePlexiglassAvailable( const Bool_t b=kTRUE);

  ClassDef(MCalibrationBlindPix, 2)	// Container Calibration Results Blind Pixel
};

#endif
