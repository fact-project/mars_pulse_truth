#ifndef MARS_MMarquardt
#define MARS_MMarquardt

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TVectorD
#include "TVectorD.h"
#endif

#ifndef ROOT_TMatrixD
#include "TMatrixD.h"
#endif


class MMarquardt : public MParContainer
{
private:

  Int_t fNumStepMax;  // maximum number of steps allowed in the minimization
  Double_t fLoopChi2; // minimization will stop when the change in chi2 
                      // is less than fLoopChi2

  Int_t    fNpar;          // number of parameters
  Int_t    fNumStep;       // number of steps made
  Double_t fChi2;          // final chi2
  Double_t fdChi2;         // change of chi2 in last step

  // working space for Marquardt
  TVectorD fdParam;

  TVectorD fParam;
  TMatrixD fCovar;
  TVectorD fGrad;

  TVectorD fmyParam;
  TMatrixD fmyCovar;
  TVectorD fmyGrad;

  Double_t fCHIq;
  Double_t fLambda;
  Bool_t   (*fFunc)(TVectorD &, TMatrixD &, TVectorD &, Double_t &);

  //working space for GaussJordan
  TVectorD fIxc, fIxr, fIp;

  Bool_t FirstCall();
  Bool_t NextStep();
  Bool_t CovMatrix();
  Bool_t GaussJordan(Int_t &n, TMatrixD &covar, TVectorD &beta);


public:
    MMarquardt(const char *name=NULL, const char *title=NULL);
    ~MMarquardt();

    void SetNpar(Int_t npar, Int_t numstepmax, Double_t loopchi2);

    Int_t Loop(Bool_t (*fcn)(TVectorD &, TMatrixD &, TVectorD &, Double_t &),
               TVectorD &);

    ClassDef(MMarquardt, 0) // Class for Marquardt minimization
};

#endif






























