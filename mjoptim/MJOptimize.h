#ifndef MARS_MJOptimize
#define MARS_MJOptimize

#ifndef MARS_MJOptimizeBase
#include "MJOptimizeBase.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class TMinuit;

class MAlphaFitter;

class MTask;
class MEvtLoop;
class MParList;
class MFilter;
class MFilterList;
class MFitParameters;
class MFitParametersCalc;

class MHMatrix;
class MGeomCam;
class MRead;
class MReadTree;

class MJOptimize : public MJOptimizeBase
{
public:
    enum Optimizer_t
    {
        kMigrad,      // Minimize by the method of Migrad
        kSimplex,     // Minimize by the method of Simplex
        kMinimize,    // Migrad + Simplex (if Migrad fails)
        kMinos,       // Minos error determination
        kImprove,     // Local minimum search
        kSeek,        // Minimize by the method of Monte Carlo
        kNone         // Skip optimization
    };

private:
    Int_t fNumEvents;

    TList fFilter;

    void AddPoint(TList *l, Int_t idx, Float_t val) const;
    TList *GetPlots() const;

    MEvtLoop *fEvtLoop;    //!

    // Minuit Interface
    static void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    Double_t Fcn(const TArrayD &par, TMinuit *minuit=0);

    Int_t Minuit(TMinuit &minuit, const char *cmd) const;
    Int_t Migrad(TMinuit &minuit) const   { return Minuit(minuit, "MIGRAD"); }
    Int_t Simplex(TMinuit &minuit) const  { return Minuit(minuit, "SIMPLEX"); }
    Int_t Minimize(TMinuit &minuit) const { return Minuit(minuit, "MINIMIZE"); }
    Int_t Seek(TMinuit &minuit) const     { return Minuit(minuit, "SEEK"); }
    Int_t Improve(TMinuit &minuit) const  { return Minuit(minuit, "IMPROVE"); }
    Int_t Minos(TMinuit &minuit) const    { return Minuit(minuit, "MINOS"); }

    TArrayD fParameters;   //!
    TArrayD fLimLo;        //!
    TArrayD fLimUp;        //!
    TArrayD fStep;         //!

    Optimizer_t fType;
    UInt_t  fNumMaxCalls;
    Float_t fTolerance;
    Int_t   fTestTrain;
    TString fNameMinimizationValue;

    Bool_t Optimize(MEvtLoop &evtloop);

protected:
    TList   fNamesOn;
    TList   fNamesOff;

    TString fNameOut;

    void   AddRulesToMatrix(MHMatrix &m) const;
    void   SetupFilters(MFilterList &list, MFilter *filter=0) const;
    Bool_t AddSequences(MRead &read, TList &list) const;
    Bool_t FillMatrix(MReadTree &read, MParList &l, Bool_t userules=kFALSE);

public:
    MJOptimize();

    // I/O
    void AddSequenceOn(const char *fname, const char *dir="");
    void AddSequenceOff(const char *fname, const char *dir="");

    void ResetSequences();

    // Interface for filter cuts
    void AddFilter(const char *rule);

    // Steering of optimization
    void SetNumEvents(UInt_t n);
    void SetDebug(UInt_t n);
    void SetNameOut(const char *name="") { fNameOut = name; }
    void SetOptimizer(Optimizer_t o);
    void SetNumMaxCalls(UInt_t num=0) { fNumMaxCalls=num; }
    void SetTolerance(Float_t tol=0)  { fTolerance=tol; }
    void EnableTestTrain(Int_t b=1)   { fTestTrain=b; } // Use 1 and -1
    void SetNameMinimizationValue(const char *name="MinimizationValue") { fNameMinimizationValue = name; }

    // Parameter access
    void SetParameters(const TArrayD &par);
    void SetParameter(Int_t idx, Double_t start, Double_t lo=0, Double_t up=0, Double_t step=-1)
    {
        if (fParameters.GetSize()<=idx)
        {
            fParameters.Set(idx+1);
            fLimLo.Set(idx+1);
            fLimUp.Set(idx+1);
            fStep.Set(idx+1);
        }

        fParameters[idx] = start;
        fLimLo[idx] = lo;
        fLimUp[idx] = up;
        if (step<=0)
            fStep[idx] = start==0 ? 0.1 : TMath::Abs(start*0.15);
        else
            fStep[idx] = step;
    }
    void FixParameter(Int_t idx, Double_t start, Double_t lo=0, Double_t up=0, Double_t step=-1)
    {
        if (fParameters.GetSize()<=idx)
        {
            fParameters.Set(idx+1);
            fLimLo.Set(idx+1);
            fLimUp.Set(idx+1);
            fStep.Set(idx+1);
        }

        fParameters[idx] = start;
        fLimLo[idx] = 0;
        fLimUp[idx] = 0;
        fStep[idx]  = 0;
    }

    const TArrayD &GetParameters() const { return fParameters; }

    // Generalized optimizing routines
    Bool_t Optimize(MParList &list);

    ClassDef(MJOptimize, 0) // Class for optimization of the Supercuts
};

#endif
