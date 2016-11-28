#ifndef MARS_MCalibrationPix
#define MARS_MCalibrationPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCalibrationPix : public MParContainer
{
protected:

  Int_t   fPixId;             // Software PixId (needed to get Id in calls to Next())
  UInt_t  fFlags;             // Flag for the set bits
  Float_t fHiGainMean;        // Mean from fit to high gain values
  Float_t fHiGainMeanVar;     // Error of mean from fit to high gain values
  Float_t fHiGainNumBlackout; // Number blackout events in high-gain
  Float_t fHiGainNumPickup;   // Number pickup events in high-gain
  Float_t fHiGainRms;         // Histogram RMS of the high gain values
  Float_t fHiGainSigma;       // Sigma from fit to high gain values
  Float_t fHiGainSigmaVar;    // Error of sigma from fit to high gain values
  Float_t fHiGainProb;        // Probability of fit to high gain values
  Float_t fLoGainMean;        // Mean from fit to high gain values
  Float_t fLoGainMeanVar;     // Error of mean from fit to low gain values
  Float_t fLoGainNumBlackout; // Number blackout events in low-gain
  Float_t fLoGainNumPickup;   // Number pickup events in low-gain
  Float_t fLoGainRms;         // Histogram RMS of the low-gain values  
  Float_t fLoGainSigma;       // Sigma of from fit to low gain values
  Float_t fLoGainSigmaVar;    // Error of sigma from fit to low gain values
  Float_t fLoGainProb;        // Probability of fit to low gain values

  enum { kHiGainSaturation, kExcluded, kValid, kDebug };   // Possible bits to be sets

public:

  MCalibrationPix(const char *name=NULL, const char *title=NULL);
  ~MCalibrationPix() {}
  
  void Clear(Option_t *o="");
  void Copy(TObject& object) const;
  
  // Setter
  void SetPixId( const Int_t i )   { fPixId = i; }

  void SetHiGainMean        ( const Float_t f ) { fHiGainMean        = f; }
  void SetHiGainMeanVar     ( const Float_t f ) { fHiGainMeanVar     = f; }
  void SetHiGainNumBlackout ( const Float_t f ) { fHiGainNumBlackout = f; }
  void SetHiGainNumPickup   ( const Float_t f ) { fHiGainNumPickup   = f; }
  void SetHiGainProb        ( const Float_t f ) { fHiGainProb        = f; }
  void SetHiGainRms         ( const Float_t f ) { fHiGainRms         = f; }
  void SetHiGainSigma       ( const Float_t f ) { fHiGainSigma       = f; }  
  void SetHiGainSigmaVar    ( const Float_t f ) { fHiGainSigmaVar    = f; }
  void SetLoGainMean        ( const Float_t f ) { fLoGainMean        = f; }
  void SetLoGainMeanVar     ( const Float_t f ) { fLoGainMeanVar     = f; }
  void SetLoGainNumBlackout ( const Float_t f ) { fLoGainNumBlackout = f; }  
  void SetLoGainNumPickup   ( const Float_t f ) { fLoGainNumPickup   = f; }
  void SetLoGainProb        ( const Float_t f ) { fLoGainProb        = f; }
  void SetLoGainRms         ( const Float_t f ) { fLoGainRms         = f; }
  void SetLoGainSigma       ( const Float_t f ) { fLoGainSigma       = f; }
  void SetLoGainSigmaVar    ( const Float_t f ) { fLoGainSigmaVar    = f; }
  void SetMean       ( const Float_t f ) { IsHiGainSaturation() ? fLoGainMean        = f : fHiGainMean        = f      ; }
  void SetMeanVar    ( const Float_t f ) { IsHiGainSaturation() ? fLoGainMeanVar     = f : fHiGainMeanVar     = f  ;  }
  void SetNumBlackout( const Float_t f ) { IsHiGainSaturation() ? fLoGainNumBlackout = f : fHiGainNumBlackout = f ; }  
  void SetNumPickup  ( const Float_t f ) { IsHiGainSaturation() ? fLoGainNumPickup   = f : fHiGainNumPickup   = f ; }
  void SetProb       ( const Float_t f ) { IsHiGainSaturation() ? fLoGainProb        = f : fHiGainProb        = f      ; }
  void SetSigma      ( const Float_t f ) { IsHiGainSaturation() ? fLoGainSigma       = f : fHiGainSigma       = f     ; }
  void SetSigmaVar   ( const Float_t f ) { IsHiGainSaturation() ? fLoGainSigmaVar    = f : fHiGainSigmaVar    = f  ; }

  void SetDebug           ( const Bool_t  b = kTRUE );  
  void SetExcluded        ( const Bool_t  b = kTRUE );
  void SetHiGainSaturation( const Bool_t  b = kTRUE );
  void SetValid           ( const Bool_t  b = kTRUE );
  
  // Getters
  Float_t GetHiGainMean       () const { return fHiGainMean   ;     }
  Float_t GetHiGainMeanErr    () const;
  Float_t GetHiGainMeanVar    () const { return fHiGainMeanVar;     }
  Float_t GetHiGainMeanRelVar () const;
  Float_t GetHiGainProb       () const { return fHiGainProb   ;     }
  Float_t GetHiGainRms        () const { return fHiGainRms    ;     }  
  Float_t GetHiGainSigma      () const { return fHiGainSigma  ;     }
  Float_t GetHiGainSigmaErr   () const;
  Float_t GetHiGainSigmaVar   () const { return fHiGainSigmaVar;    }
  Float_t GetHiGainNumPickup  () const { return fHiGainNumPickup;   }
  Float_t GetHiGainNumBlackout() const { return fHiGainNumBlackout; }  
  Float_t GetHiLoMeansDivided         () const;
  Float_t GetHiLoMeansDividedErr      () const;
  Float_t GetHiLoSigmasDivided        () const;
  Float_t GetHiLoSigmasDividedErr     () const;

  virtual Float_t GetLoGainMean       () const { return fLoGainMean;        }
  virtual Float_t GetLoGainMeanErr    () const;
  virtual Float_t GetLoGainMeanVar    () const { return fLoGainMeanVar;     }
  virtual Float_t GetLoGainMeanRelVar () const;  
  virtual Float_t GetLoGainProb       () const { return fLoGainProb;        }
  virtual Float_t GetLoGainRms        () const { return fLoGainRms  ;       }
  virtual Float_t GetLoGainSigma      () const { return fLoGainSigma;       }  
  virtual Float_t GetLoGainSigmaErr   () const;
  virtual Float_t GetLoGainSigmaVar   () const { return fLoGainSigmaVar;    }
  virtual Float_t GetLoGainNumPickup  () const { return fLoGainNumPickup;   }
  virtual Float_t GetLoGainNumBlackout() const { return fLoGainNumBlackout; }  

  Float_t GetMean       () const { return IsHiGainSaturation() ? GetLoGainMean()        : GetHiGainMean()     ;   }
  Float_t GetMeanErr    () const { return IsHiGainSaturation() ? GetLoGainMeanErr()     : GetHiGainMeanErr()  ;   }
  Float_t GetMeanRelVar () const;
  Float_t GetMeanSquare () const;  
  Float_t GetRms        () const { return IsHiGainSaturation() ? GetLoGainRms()         : GetHiGainRms()      ;   }
  Float_t GetProb       () const { return IsHiGainSaturation() ? GetLoGainProb()        : GetHiGainProb()     ;   }
  Float_t GetSigma      () const { return IsHiGainSaturation() ? GetLoGainSigma()       : GetHiGainSigma()    ;   }
  Float_t GetSigmaErr   () const { return IsHiGainSaturation() ? GetLoGainSigmaErr()    : GetHiGainSigmaErr() ;   }
  Float_t GetSigmaRelVar() const;
  Float_t GetNumPickup  () const { return IsHiGainSaturation() ? GetLoGainNumPickup()   : GetHiGainNumPickup();   }
  Float_t GetNumBlackout() const { return IsHiGainSaturation() ? GetLoGainNumBlackout() : GetHiGainNumBlackout(); }
  Int_t   GetPixId      () const { return fPixId ;  }
  
  Bool_t  IsHiGainSaturation() const;
  Bool_t  IsDebug   ()         const;
  Bool_t  IsExcluded()         const;
  Bool_t  IsValid   ()         const;

  ClassDef(MCalibrationPix, 2)	// Base Container for Calibration Results Pixel
};

#endif

