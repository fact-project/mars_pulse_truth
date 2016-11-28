#ifndef MARS_MHCalibrationTestPix
#define MARS_MHCalibrationTestPix

#ifndef MARS_MHCalibrationPix
#include "MHCalibrationPix.h"
#endif

class MHCalibrationTestPix : public MHCalibrationPix
{

private:

  static const Int_t   fgChargeNbins;        // Default for fNBins          (now set to: 2000  )
  static const Axis_t  fgChargeFirst;        // Default for fFirst          (now set to: -0.5  )
  static const Axis_t  fgChargeLast;         // Default for fLast           (now set to: 1999.5)

public:

  MHCalibrationTestPix(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationTestPix() {}

  const Float_t  GetIntegral()          const;
  
  ClassDef(MHCalibrationTestPix, 1)     // Base Histogram class for Test Pixel Calibration
};

#endif
