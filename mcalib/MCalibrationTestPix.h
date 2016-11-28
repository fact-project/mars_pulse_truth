#ifndef MARS_MCalibrationTestPix
#define MARS_MCalibrationTestPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCalibrationTestPix : public MParContainer
{
private:

  Int_t   fPixId;                // Software PixId (needed to get Id in calls to Next())
  Bool_t  fExcluded;             // If pixels is excluded or not (not interpolateable)

  Float_t fNumPhotons;           // Number of calibrated photons
  Float_t fNumPhotonsErr;        // Error Number of calibrated photons
  Float_t fNumPhotonsPerArea;    // Number of calibrated photons per mm^2     
  Float_t fNumPhotonsPerAreaErr; // Error Number of calibrated photons per mm^2

public:

  MCalibrationTestPix(const char *name=NULL, const char *title=NULL);
  ~MCalibrationTestPix() {}
  
  void Clear(Option_t *o="");
  
  // Getters
  Float_t GetNumPhotons          () const { return fNumPhotons;           }
  Float_t GetNumPhotonsErr       () const { return fNumPhotonsErr;        }
  Float_t GetNumPhotonsPerArea   () const { return fNumPhotonsPerArea;    }
  Float_t GetNumPhotonsPerAreaErr() const { return fNumPhotonsPerAreaErr; }
  Int_t   GetPixId               () const { return fPixId;                }
  
  Bool_t  IsExcluded             () const { return fExcluded;             }

  // Setters
  void SetPixId               ( const Int_t   i )       { fPixId                = i; }
  void SetExcluded            ( const Bool_t  b=kTRUE ) { fExcluded             = b; }
  void SetNumPhotons          ( const Float_t f )       { fNumPhotons           = f; }          
  void SetNumPhotonsErr       ( const Float_t f )       { fNumPhotonsErr        = f; }       
  void SetNumPhotonsPerArea   ( const Float_t f )       { fNumPhotonsPerArea    = f; }   
  void SetNumPhotonsPerAreaErr( const Float_t f )       { fNumPhotonsPerAreaErr = f; }


  ClassDef(MCalibrationTestPix, 1)	// Container for Calibration Test Results Pixel
};

#endif

