#ifndef MARS_MParSpline
#define MARS_MParSpline

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TF1;       // MParSpline
class TSpline3;
class MSpline3;

class MParSpline : public MParContainer
{
private:
    MSpline3 *fSpline;   // Spline hold by this container

    // MParSpline
    MSpline3 *ReadSpline(const char *fname) const;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MParSpline(const char *name=0, const char *title=0);
    ~MParSpline()  { Clear(); }

    // MParSpline
    const MSpline3 *GetSpline() const { return fSpline; }

    Double_t GetXmin() const;
    Double_t GetXmax() const;
    Double_t GetWidth() const;

    Bool_t IsValid() const { return fSpline ? kTRUE : kFALSE; }

    Double_t Eval(Double_t val) const;

    void SetFunction(const TF1 &f);
    Bool_t SetFunction(const char *func, Int_t n, Double_t xmin, Double_t xmax);

    //void InitUnity(UInt_t n, Float_t min, Float_t max);

    void Multiply(const TSpline3 &spline);
    void Multiply(UInt_t n, const Double_t *x, const Double_t *y);
    void Multiply(const char *fname);

    Bool_t ReadFile(const char *fname);

    // TObject
    void Clear(Option_t *o="");
    void Paint(Option_t *o="");

    void RecursiveRemove(TObject *obj);

    ClassDef(MParSpline, 1) // Container to hold a MSpline3
};

#endif
