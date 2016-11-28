#ifndef MARS_MCT1FindSupercuts
#define MARS_MCT1FindSupercuts

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MFilter;
class MEvtLoop;
class MH3;
class MCT1SupercutsCalc;
class MGeomCam;
class MHMatrix;
/*
#include "MFilter.h"
#include "MEvtLoop.h"
#include "MH3.h"
#include "MCT1SupercutsCalc.h"
#include "MGeomCam.h"
#include "MHMatrix.h"
*/

class MCT1FindSupercuts : public MParContainer
{
private:

  TString fFilenameTrain;
  TString fFilenameTest;

  Int_t   fHowManyTrain;
  Int_t   fHowManyTest;

  TString  fFilenameParam;

  TString  fHadronnessName;

  MCT1SupercutsCalc *fCalcHadTrain;
  MCT1SupercutsCalc *fCalcHadTest;

  MHMatrix          *fMatrixTrain;
  MHMatrix          *fMatrixTest;
  MGeomCam          *fCam;

  MEvtLoop *fObjectFit;

  MFilter  *fMatrixFilter; 

  // to comunicate with MINUIT -----------------
  // attention : dimensions must agree with those in 
  //             MMinuitInterface::CallMinuit()
  //char    fParName [80][100];
  TArrayD fVinit;
  TArrayD fStep;
  TArrayD fLimlo;
  TArrayD fLimup;
  TArrayI fFix;

  UInt_t     fNpar;

  TString    fMethod;

  Double_t fMin,   fEdm,   fErrdef;
  Int_t    fNpari, fNparx, fIstat;
  Int_t    fErrMinimize;
  //--------------------------------------------


public:
  MCT1FindSupercuts(const char *name=NULL, const char *title=NULL);
  ~MCT1FindSupercuts();

  void SetFilenameTraining(const TString &name, const Int_t howmany) 
      {fFilenameTrain = name;  fHowManyTrain = howmany; }

  void SetFilenameTest(const TString &name, const Int_t howmany)     
      {fFilenameTest     = name;  fHowManyTest  = howmany; }

  void SetFilenameParam(const TString &name)    {fFilenameParam  = name;}
  void SetHadronnessName(const TString &name)   {fHadronnessName = name;}

  void SetMatrixFilter(MFilter *filter)          {fMatrixFilter = filter;}

  Bool_t DefineTrainMatrix(const TString &name, MH3 &href,
                           const Int_t howmany, const TString &filetrain); 

  Bool_t DefineTestMatrix(const TString &name, MH3 &href,
                          const Int_t howmany, const TString &filetest);

  Bool_t DefineTrainTestMatrix(const TString &name, MH3 &href,
			 const Int_t howmanytrain, const Int_t howmanytest, 
                         const TString &filetrain, const TString &filetest);

  MHMatrix *GetMatrixTrain() { return fMatrixTrain; }
  MHMatrix *GetMatrixTest()  { return fMatrixTest;  }

  Bool_t ReadMatrix( const TString &filetrain, const TString &filetest);

  Bool_t FindParams(TString parSCinit, TArrayD &params, TArrayD &steps);
  Bool_t TestParams();

  ClassDef(MCT1FindSupercuts, 1) // Class for optimization of the Supercuts
};

#endif



















