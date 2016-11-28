#ifndef MARS_MPulseShape
#define MARS_MPulseShape

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TF1;
class MSpline3;

class MPulseShape : public MParContainer
{
private:
    MSpline3          *fSpline;     // Spline to describe the pulse shape

    TString fFileName;              // File name of a file describing the pulse shape

    TString fFunction;              // Function describing the pulse shape
    Int_t   fNpx;                   // Number of sampling points
    Float_t fXmin;                  // lower edge of sampling
    Float_t fXmax;                  // upper edge of sampling

    Bool_t ReadFile(const char *fname);
    Bool_t SetFunction(const char *func, Int_t n, Double_t xmin, Double_t xmax);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

public:
    MPulseShape(const char *name=0, const char *title=0);
    ~MPulseShape();

    void SetFunction(const TF1 &f);
    void SetFunction(const char *f)  { fFunction=f; fFileName=""; }
    void SetFileName(const char *n)  { fFileName=n; fFunction=""; }

    void Clear(Option_t *o="");
    void Paint(Option_t *o="");

    MSpline3 *GetSpline() const { return fSpline; }

    Float_t GetPulseWidth() const;
    Float_t GetXmin() const;
    Float_t GetXmax() const;

    ClassDef(MPulseShape, 1) // Parameter container to hold the setup for a pulse shape
};

#endif
