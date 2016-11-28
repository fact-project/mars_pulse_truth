#ifndef MARS_MHCalibrationPix
#define MARS_MHCalibrationPix

#ifndef MARS_MHGausEvents
#include "MHGausEvents.h"
#endif

class MHCalibrationPix : public MHGausEvents
{
private:

  const static Float_t  fgBlackoutLimit; //! Default for fBlackoutLimit (now set to: 5. )
  const static Float_t  fgPickupLimit;   //! Default for fPickupLimit   (now set to: 5. )
  
protected:

  Float_t  fBlackoutLimit;               // Lower nr sigmas from mean until event is considered blackout
  Int_t    fSaturated;                   // Number of events classified as saturated
  Float_t  fPickupLimit;                 // Upper nr sigmas from mean until event is considered pickup

public:

  MHCalibrationPix(const char* name=NULL, const char* title=NULL);

  void  Clear(Option_t *o="");
  void  Reset();
  
  // Getters
  const Double_t GetBlackout() const;
  const Double_t GetPickup() const;
  const Int_t    GetSaturated() const { return fSaturated;      }

  // Fits
  Bool_t RepeatFit(const Option_t *option="RQ0"); // Repeat fit within limits defined by fPickupLimit
  void   BypassFit();                             // Take mean and RMS from the histogram
  
  // Setters
  void  AddSaturated        ( const Int_t    i                   ) { fSaturated     += i;   }
  void  SetBlackoutLimit    ( const Float_t  lim=fgBlackoutLimit ) { fBlackoutLimit  = lim; }
  void  SetPickupLimit      ( const Float_t  lim=fgPickupLimit   ) { fPickupLimit    = lim; }

  ClassDef(MHCalibrationPix, 1) // Base class for calibration events 
};

#endif
