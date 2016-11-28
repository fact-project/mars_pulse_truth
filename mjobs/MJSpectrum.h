#ifndef MARS_MJSpectrum
#define MARS_MJSpectrum

#ifndef MARS_MJob
#include <MJob.h>
#endif

class TF1;
class TH1;
class TH1D;
class TH2D;

class MH3;
class MHn;
class MTask;
class MParList;
class MDataSet;
class MHEnergyEst;
class MAlphaFitter;
class MStatusArray;
class MWriteRootFile;
class MHCollectionArea;
class MMcSpectrumWeight;

class MJSpectrum : public MJob
{
private:
    MTask *fCutQ;
    MTask *fCut0;
    MTask *fCut1;
    MTask *fCut2;
    MTask *fCut3;
    MTask *fCutS;
    MTask *fEstimateEnergy;
    MTask *fCalcHadronness;
    MTask *fCalcDisp;

    Bool_t fForceTheta;
    Bool_t fForceRunTime;
    Bool_t fForceOnTimeFit;

    // Setup Histograms
    void SetupHistEvtDist(MHn &hist) const;
    void SetupHistEnergyEst(MHn &hist) const;
    void SetupHistDisp(MHn &hist) const;
    void SetupHistEnergyRes(MHn &hist) const;

    // Read Input
    Bool_t   ReadTask(MTask* &task, const char *name, Bool_t mustexist=kTRUE) const;
    Float_t  ReadInput(MParList &plist, TH1D &h1, TH1D &size);
    Bool_t   AnalyzeMC(const MDataSet &set, Float_t &impactmax, Float_t &emin/*, Float_t emax*/) const;
    Bool_t   ReadOrigMCDistribution(const MDataSet &set, TH1 &h, MMcSpectrumWeight &w) const;
    void     GetThetaDistribution(TH1D &temp1, TH2D &temp2) const;
    TString  GetHAlpha() const;
    Bool_t   Refill(MParList &plist, TH1D &h) /*const*/;
    Bool_t   InitWeighting(const MDataSet &set, MMcSpectrumWeight &w) const;

    // Write output
    void     SetupWriter(MWriteRootFile *write/*, const char *name*/) const;

    // Display Output
    void    PrintSetup(const MAlphaFitter &fit) const;
    Bool_t  DisplayResult(const TH2D &mh1) const;
    //Bool_t  IntermediateLoop(MParList &plist, MH3 &h1, TH1D &temp1, const MDataSet &set, MMcSpectrumWeight &w) const;
    TArrayD FitSpectrum(TH1D &spectrum) const;
    TArrayD DisplaySpectrum(MHCollectionArea &area, TH1D &excess, MHEnergyEst &hest, Double_t ontime) const;
    Bool_t  DisplaySize(MParList &plist, Double_t scale) const;
    void    DisplayCutEfficiency(const MHCollectionArea &area0, const MHCollectionArea &area1) const;
    Bool_t  PlotSame(MStatusArray &arr, MParList &plist, const char *name, const char *tab, const char *plot, Double_t scale) const;

public:
    MJSpectrum(const char *name=NULL, const char *title=NULL);
    ~MJSpectrum();

    Bool_t Process(const MDataSet &set);

    void ForceTheta(Bool_t b=kTRUE)   { fForceTheta=b; }
    void ForceRunTime(Bool_t b=kTRUE) { fForceRunTime=b; }
    void ForceOnTimeFit(Bool_t b=kTRUE) { fForceOnTimeFit=b; }

    void SetEnergyEstimator(const MTask *task);

    static TString FormFloat(Double_t d);
    static TString FormFlux(const TF1 &f, const char *unit);
    static TString FormString(const TF1 &f, Byte_t type=0);

    ClassDef(MJSpectrum, 0) // Proh'gram to calculate spectrum
};

#endif
