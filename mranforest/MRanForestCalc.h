#ifndef MARS_MRanForestCalc
#define MARS_MRanForestCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_MDataPhrase
#include "MDataPhrase.h"
#endif

class MDataArray;
class MParameterD;
class MHMatrix;

class MRanForestCalc : public MTask
{
public:
    enum EstimationMode_t
    {
        kMean,
        kMaximum,
        kFit
    };

private:
    static const TString gsDefName;      //! Default Name
    static const TString gsDefTitle;     //! Default Title
    static const TString gsNameOutput;   //! Default Output name
    static const TString gsNameEvalFunc; //! Evaluation function name

    MDataArray  *fData;                 //! Used to store the MDataChains to get the event values
    MParameterD *fRFOut;                //! Used to store result
    MHMatrix    *fTestMatrix;           //! Test Matrix used in Process (together with MMatrixLoop)
    MDataPhrase  fFunc;                 //! Function to apply to the result

    TObjArray    fEForests;             //! List of forests read or to be written

    Int_t        fNumTrees;             //! Training parameters
    Int_t        fNumTry;               //! Training parameters
    Int_t        fNdSize;               //! Training parameters

    Int_t        fNumObsoleteVariables; //! Training parameters
    Bool_t       fLastDataColumnHasWeights; //! Training parameters

    TString      fFileName;             // File name to forest
    TString      fNameOutput;           // Name of output container

    Bool_t       fDebug;                // Debugging of eventloop while training on/off

    EstimationMode_t fEstimationMode;   // Mode of estimation in case of multi random forest regression

private:
    // MTask
    Int_t PreProcess(MParList *plist);
    Int_t Process();

    // MRanForestCalc
    Int_t ReadForests(MParList &plist);
    Double_t Eval() const;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // Train Interface
    Int_t Train(const MHMatrix &n, const TArrayD &grid, Int_t ver);

public:
    MRanForestCalc(const char *name=NULL, const char *title=NULL);
    ~MRanForestCalc();

    // TObject
    void Print(Option_t *o="") const; //*MENU*

    // Setter for estimation
    void SetFileName(TString filename)            { fFileName = filename; }
    void SetEstimationMode(EstimationMode_t op)   { fEstimationMode = op; }
    void SetNameOutput(TString name=gsNameOutput) { fNameOutput = name; }

    // Setter for training
    void SetNumTrees(UShort_t n=100) { fNumTrees = n; }
    void SetNdSize(UShort_t n=5)     { fNdSize   = n; }
    void SetNumTry(UShort_t n=0)     { fNumTry   = n; }
    void SetDebug(Bool_t b=kTRUE)    { fDebug    = b; }

    Bool_t SetFunction(const char *name="x");

    void SetNumObsoleteVariables(Int_t n=1)          { fNumObsoleteVariables = n; }
    void SetLastDataColumnHasWeights(Bool_t b=kTRUE) { fLastDataColumnHasWeights = b; }

    // Train Interface
    Int_t TrainMultiRF(const MHMatrix &n, const TArrayD &grid)
    {
        // One yes/no-classification forest is trained for each bin
        return Train(n, grid, 0);
    }
    Int_t TrainSingleRF(const MHMatrix &n, const TArrayD &grid=TArrayD())
    {
        // w/o Grid: Last Column contains classifier
        // w/  Grid: Last Column will be converted by grid into classifier
        return Train(n, grid, grid.GetSize()==0 ? 2 : 1);
    }
    Int_t TrainRegression(const MHMatrix &n)
    {
        // Use last column for regression
        return Train(n, TArrayD(), 3);
    }

    Int_t Train(const MHMatrix &m, Bool_t regression)
    {
        return regression ? TrainRegression(m) : TrainSingleRF(m);
    }

    // Test Interface
    void  SetTestMatrix(MHMatrix *m=0) { fTestMatrix=m; }
    void  InitMapping(MHMatrix *m=0)   { fTestMatrix=m; }

    ClassDef(MRanForestCalc, 1) // Task to calculate RF output and for RF training
};

#endif
