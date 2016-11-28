#ifndef MARS_MJTrainDisp
#define MARS_MJTrainDisp

#ifndef MARS_MJTrainRanForest
#include "MJTrainRanForest.h"
#endif

class TCanvas;

class MH3;
class MDataSet;

class MJTrainDisp : public MJTrainRanForest
{
private:
    static const TString fgTrainParameter;

    TString fTrainParameter;
    TString fResultFunction;

    Float_t fThetaCut;

    // To be moved to a base class
    TString fNameOutput;

    void DisplayHist(TCanvas &c, Int_t i, MH3 &mh3) const;
    void DisplayResult(MH3 &hsize, MH3 &henergy);

public:
    MJTrainDisp() : fTrainParameter(fgTrainParameter), fResultFunction("x"), fThetaCut(0.215), fNameOutput("Disp") { }

    void SetTrainFunc(const char *par, const char *res="x")
    {
        fTrainParameter = par;
        fResultFunction = res;
    }

    void SetThetaCut(Float_t cut=0.215) { fThetaCut=cut; }

    Bool_t Train(const char *out, const MDataSet &set, Int_t num);
    //Bool_t TrainGhostbuster(const char *out, const MDataSet &set, Int_t num);

    ClassDef(MJTrainDisp, 0)//Class to train Random Forest disp estimator
};

#endif
