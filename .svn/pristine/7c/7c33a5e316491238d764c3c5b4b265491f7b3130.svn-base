#ifndef MARS_MHMcEnergy
#define MARS_MHMcEnergy

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TH1;
class TH1F;
class TF1;

class MHMcEnergy : public MParContainer
{
private:

    TH1F *fHist;  // histogram with the logarith of the energy

    Float_t fThreshold;
    Float_t fThresholdErr;
    Float_t fGaussPeak;
    Float_t fGaussSigma;

    Float_t CalcThreshold(TF1 *gauss);
    Float_t CalcThresholdErr(TF1 *gauss);

    Float_t CalcGaussPeak(TF1 *gauss);
    Float_t CalcGaussSigma(TF1 *gauss);

    void DrawLegend() const;

public:

    MHMcEnergy(const char *name=NULL, const char *title=NULL);
    ~MHMcEnergy();

    void SetName(const char *name);

    Float_t GetThreshold() const { return fThreshold; }
    Float_t GetThresholdErr() const { return fThresholdErr; }

    Float_t GetGaussPeak() const { return fGaussPeak; }
    Float_t GetGaussSigma() const { return fGaussSigma; };

    void Fill(Float_t log10E, Float_t w);
    void Fit(Axis_t xxmin, Axis_t xxmax);
    void SetNumBins(Int_t nbins = 100);

    TH1 *GetHistByName(const TString name);

    void Draw(Option_t* option = "");

    void Print(Option_t* option = NULL) const;

    ClassDef(MHMcEnergy, 1)  // Histogram container for montecarlo energy threshold
};

#endif
