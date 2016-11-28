#ifndef MARS_MPedPhotCalc
#define MARS_MPedPhotCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MPedPhotCam;
class MSignalCam;
class MBadPixelsCam;
class MPedPhotCalc : public MTask
{

  MPedPhotCam   *fPedestals;  // Pedestals of all pixels in the camera
  MSignalCam    *fCerPhot;    // Calibrated Cherenkov events
  MBadPixelsCam *fBadPixels;  // Bad Pixels
  
  TArrayF fSumx;   // sum of values
  TArrayF fSumx2;  // sum of squared values

  Bool_t ReInit(MParList *pList);

  Int_t PreProcess(MParList *pList);
  Int_t Process();
  Int_t PostProcess();
  
public:
  MPedPhotCalc(const char *name=NULL, const char *title=NULL);

  ClassDef(MPedPhotCalc, 0) //Task to calculate pedestals in units of photons
};

#endif
