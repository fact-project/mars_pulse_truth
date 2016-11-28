#ifndef MARS_MPSFFit
#define MARS_MPSFFit

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MPSFFit : public MParContainer
{
private:

    Float_t fMaximun;
    Float_t fMeanMinorAxis;
    Float_t fMeanMajorAxis;
    Float_t fSigmaMinorAxis;
    Float_t fSigmaMajorAxis;
    Float_t fChisquare;

public:
    MPSFFit(const char *name=NULL, const char *title=NULL);
    //~MPSFFit();

    void SetMaximun(Float_t Maximun_) {fMaximun=Maximun_;}
    void SetMeanMinorAxis(Float_t MeanMinorAxis_) {fMeanMinorAxis=MeanMinorAxis_;}
    void SetMeanMajorAxis(Float_t MeanMajorAxis_) {fMeanMajorAxis=MeanMajorAxis_;}
    void SetSigmaMinorAxis(Float_t SigmaMinorAxis_) {fSigmaMinorAxis=SigmaMinorAxis_;}
    void SetSigmaMajorAxis(Float_t SigmaMajorAxis_) {fSigmaMajorAxis=SigmaMajorAxis_;}
    void SetChisquare(Float_t Chisquare_) {fChisquare=Chisquare_;}

    Float_t GetMaximun() {return fMaximun;}
    Float_t GetMeanMinorAxis() {return fMeanMinorAxis;}
    Float_t GetMeanMajorAxis() {return fMeanMajorAxis;}
    Float_t GetSigmaMinorAxis() {return fSigmaMinorAxis;}
    Float_t GetSigmaMajorAxis() {return fSigmaMajorAxis;}
    Float_t GetChisquare() {return fChisquare;}

    void Reset();

    void Paint(Option_t *opt=NULL);
    void Print(Option_t *opt=NULL) const;

    ClassDef(MPSFFit, 1) // Container that holds the values of the PSF fit.
};

#endif
