#ifndef MARS_MHPedestalPix
#define MARS_MHPedestalPix

#ifndef MARS_MHCalibrationPix
#include "MHCalibrationPix.h"
#endif

class MHPedestalPix : public MHCalibrationPix
{
public:

  MHPedestalPix(const char* name=NULL, const char* title=NULL);

    // Fits
  Bool_t FitDoubleGaus(  const Double_t xmin=0., 
                         const Double_t xmax=0.,
                         Option_t *option="RQ0");   
  Bool_t FitTripleGaus(  const Double_t xmin=0., 
                         const Double_t xmax=0.,
                         Option_t *option="RQ0");   

  ClassDef(MHPedestalPix, 1) // Base class for histogrammed pedestal events 
};

#endif
