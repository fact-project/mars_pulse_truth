#ifndef MARS_MUnfold
#define MARS_MUnfold

#ifndef ROOT_TH1
#include <TH1.h>
#endif

#ifndef ROOT_TMatrixD
#include <TMatrixD.h>
#endif

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TH2;
class TH3;
class TString;
class TVectorD;
class TH2D;
class TH3D;

class MUnfold : public MTask
{

private:

  TH1 *DrawMatrixClone(const TMatrixD &m, Option_t *opt);
  TH1 *DrawMatrixColClone(const TMatrixD &m, Option_t *opt, Int_t col);
  void PrintTH3Content(const TH3 &hist);
  void PrintTH3Error(const TH3 &hist);
  void PrintTH2Content(const TH2 &hist);
  void PrintTH2Error(const TH2 &hist);
  void PrintTH1Content(const TH1 &hist);
  void PrintTH1Error(const TH1 &hist);
  void CopyCol(TMatrixD &m, const TH1 &h, Int_t col);
  void CopyCol(TH1 &h, const TMatrixD &m, Int_t col);
  void CopyH2M(TMatrixD &m, const TH2 &h);
  void CopySqr(TMatrixD &m, const TH1 &h);
  Double_t GetMatrixSumRow(const TMatrixD &m, Int_t row);
  Double_t GetMatrixSumDiag(const TMatrixD &m);
  Double_t GetMatrixSumCol(const TMatrixD &m, Int_t col);
  Double_t GetMatrixSum(const TMatrixD &m);

public:
    TString   bintitle;

    UInt_t    fNa;        // Number of bins in the distribution to be unfolded
    UInt_t    fNb;        // Number of bins in the unfolded distribution

    TMatrixD  fMigrat;    // migration matrix                  (fNa, fNb)
    TMatrixD  fMigraterr2;// error**2 of migration matrix      (fNa, fNb)

    TMatrixD  fMigOrig;    // original migration matrix         (fNa, fNb)
    TMatrixD  fMigOrigerr2;// error**2 oforiginal migr. matrix  (fNa, fNb)

    TMatrixD  fMigSmoo;    // smoothed migration matrix M       (fNa, fNb)
    TMatrixD  fMigSmooerr2;// error**2 of smoothed migr. matrix (fNa, fNb)
    TMatrixD  fMigChi2;    // chi2 contributions for smoothing  (fNa, fNb)

    TMatrixD  fVa;        // distribution to be unfolded       (fNa)
    TMatrixD  fVacov;     // error matrix of fVa               (fNa, fNa)
    TMatrixD  fVacovInv; // inverse of fVacov                 (fNa, fNa)
    Double_t  fSpurVacov; // Spur of fVacov

    //    UInt_t    fVaevents;  // total number of events
    UInt_t    fVapoints;  // number of significant measurements

    TMatrixD  fVb;        // unfolded distribution             (fNb)
    TMatrixD  fVbcov;     // error matrix of fVb               (fNb, fNb)

    TMatrixD  fVEps;      // prior distribution                (fNb)
    TMatrixDColumn fVEps0;

    Double_t  fW;         // weight
    Double_t  fWbest;     // best weight
    Int_t     ixbest;

    TMatrixD  fResult;    // unfolded distribution and errors  (fNb, 5)
    TMatrixD  fChi2;      // chisquared contribution           (fNa, 1)

    Double_t  fChisq;     // total chisquared
    Double_t  fNdf;       // number of degrees of freedom
    Double_t  fProb;      // chisquared probability

    TMatrixD  G;          // G = M * M(transposed)             (fNa, fNa)
    TVectorD  EigenValue; // vector of eigenvalues lambda of G (fNa)
    TMatrixD  Eigen;      // matrix of eigen vectors of G      (fNa, fNa)
    Double_t  RankG;      // rank of G
    Double_t  tau;        // 1 / lambda_max
    Double_t  EpsLambda;

    // quantities stored for each weight :
    TVectorD SpSig;       // Spur of covariance matrix of fVbcov
    TVectorD SpAR;        // effective rank of G^tilde
    TVectorD chisq;       // chi squared (measures agreement between
    // fVa and the folded fVb)
    TVectorD SecDer;      // regularization term = sum of (2nd der.)**2
    TVectorD ZerDer;      // regularization term = sum of (fVb)**2
    TVectorD Entrop;      // regularization term = reduced cross-entropy
    TVectorD DAR2;        //
    TVectorD Dsqbar;      //

    Double_t SpurAR;
    Double_t SpurSigma;
    Double_t SecDeriv;
    Double_t ZerDeriv;
    Double_t Entropy;
    Double_t DiffAR2;
    Double_t Chisq;
    Double_t D2bar;

    TMatrixD Chi2;

    //

    // plots versus weight
    Int_t    Nix;
    Double_t xmin;
    Double_t xmax;
    Double_t dlogx;

    TH1D *hBchisq;
    TH1D *hBSpAR;
    TH1D *hBDSpAR;
    TH1D *hBSpSig;
    TH1D *hBDSpSig;
    TH1D *hBSecDeriv;
    TH1D *hBDSecDeriv;
    TH1D *hBZerDeriv;
    TH1D *hBDZerDeriv;
    TH1D *hBEntropy;
    TH1D *hBDEntropy;
    TH1D *hBDAR2;
    TH1D *hBD2bar;

    //
    TH1D *hEigen;

    // plots for the best solution
    TH2D *fhmig;
    TH2D *shmig;
    TH2D *shmigChi2;

    TH1D *fhb0;

    TH1D *fha;

    TH1D *hprior;

    TH1D *hb;

    Double_t CalcSpurSigma(TMatrixD &T, Double_t norm=1);

    // -----------------------------------------------------------------------
    //
    // Constructor
    //              copy histograms into matrices
    //
    MUnfold(TH1D &ha, TH2D &hacov, TH2D &hmig);
    ~MUnfold();
   
    void SetPriorConstant();
    Bool_t SetPriorRebin(TH1D &ha);
    Bool_t SetPriorInput(TH1D &hpr);
    Bool_t SetPriorPower(Double_t gamma);
    Bool_t SetInitialWeight(Double_t &weight);
    void PrintResults();
    Bool_t Schmelling(TH1D &hb0);
    void SchmellCore(Int_t full, Double_t &xiter, TMatrixD &gamma,
			      TMatrixD &dgamma, Double_t &dga2);
    Bool_t SmoothMigrationMatrix(TH2D &hmigorig);
    Bool_t Tikhonov2(TH1D &hb0);
    Bool_t Bertero(TH1D &hb0);
    Bool_t BertCore(Double_t &xiter);
    Bool_t CalculateG();
    Bool_t SelectBestWeight();
    Bool_t DrawPlots();
    Bool_t CallMinuit(
                      void (*fcnx)(Int_t &, Double_t *, Double_t &, Double_t *, Int_t),
                      UInt_t npar, char name[20][100],
                      Double_t vinit[20], Double_t step[20],
                      Double_t limlo[20], Double_t limup[20], Int_t fix[20]);
    TMatrixD &GetVb() { return fVb; }
    TMatrixD &GetVbcov() { return fVbcov; }
    TMatrixD &GetResult() { return fResult; }
    TMatrixD &GetChi2() { return fChi2; }
    Double_t &GetChisq() { return fChisq; }
    Double_t &GetNdf() { return fNdf; }
    Double_t &GetProb() { return fProb; }
    TMatrixD &GetMigSmoo() { return fMigSmoo; }
    TMatrixD &GetMigSmooerr2() { return fMigSmooerr2; }
    TMatrixD &GetMigChi2() { return fMigChi2; }

    ClassDef(MUnfold, 1)

};

#endif
