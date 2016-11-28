#ifndef MARS_MMcUnfoldCoeffCalc
#define MARS_MMcUnfoldCoeffCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#include <TH2.h>

class MParList;
class MMcEvt;
class MMcEvtBasic;
class MEnergyEst;
class MBinning;
class MHMcUnfoldCoeff;

class MMcUnfoldCoeffCalc : public MTask
{
 private:
  const MMcEvt*       fMcEvt;
  const MMcEvtBasic*  fMcEvtBasic;
  const MEnergyEst*   fEnergyEst;

  MBinning*           fBinsTheta;  // coarse zenith angle binning
  MBinning*           fBinsEnergy; // coarse energy binning
  
  TF1*                fSpectrum;   // Tentative energy spectrum. 
  
  MHMcUnfoldCoeff*    fUnfoldCoeff; // container holding coefficients histogram
  
  TString fObjName;

  Int_t  PreProcess(MParList *pList);
  Int_t  Process();
  Int_t  PostProcess();
  
 public:
  MMcUnfoldCoeffCalc(const char *name = NULL, const char *title = NULL);

  void SetSpectrum(TF1 *f) { fSpectrum = f; }

  ClassDef(MMcUnfoldCoeffCalc, 0) // Task to calculate the coefficients for unfolding
};

#endif 

