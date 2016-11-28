#ifndef MARS_MUnfoldSpectrum
#define MARS_MUnfoldSpectrum

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TH1D;
class TH2D;
class TH3D;

class MUnfoldSpectrum : public MTask
{
 public:
  MUnfoldSpectrum();
  ~MUnfoldSpectrum();

 private:
  TH2D *fToBeUnfolded;
  TH2D *fUnfolded;
  TH3D *fMigrationMatrix;
  Short_t fUnfoldingMethod;
  Short_t fPrior;
  TH1D *fPriorRebinHist;
  TH1D *fPriorInputHist;
  Double_t fPriorPowerGamma;

 public:
  void Calc();
  void SetDistToUnfold(TH2D* dist) { fToBeUnfolded = dist; }
  void SetMigrationMatrix(TH3D* migm) { fMigrationMatrix = migm; }
  TH2D* GetUnfDist() { return fUnfolded; }
  void SetPriorConstant();
  void SetPriorRebin(TH1D *ha);
  void SetPriorInput(TH1D *hpr);
  void SetPriorPower(Double_t gamma);
  void SetUnfoldingMethod(Short_t method) { fUnfoldingMethod = method; }
  
  ClassDef(MUnfoldSpectrum, 1)

};

#endif
