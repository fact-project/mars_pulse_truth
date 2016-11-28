#ifndef MARS_MMuonSetup
#define MARS_MMuonSetup

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MMuonSetup : public MParContainer
{
private:
    Float_t fMargin;            // [deg] margin to evaluate muons. The defaut value is 0.2 deg (60mm)
    Float_t fThresholdArcPhi;   // [phe] The threshold value to define ArcPhi
    Float_t fThresholdArcWidth; // [phe] The threshold value to define ArcWidth

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MMuonSetup(const char *name=NULL, const char *title=NULL);

    // Getter
    Float_t GetMargin() const { return fMargin; }
    Float_t GetThresholdArcPhi() const { return fThresholdArcPhi; }
    Float_t GetThresholdArcWidth() const { return fThresholdArcWidth; }

    // Setter
    void SetMargin(Float_t margin)           { fMargin = margin; }
    void SetThresholdArcPhi(Float_t thres)   { fThresholdArcPhi = thres; }
    void SetThresholdArcWidth(Float_t thres) { fThresholdArcWidth = thres; }

    ClassDef(MMuonSetup, 1) // Container to hold setup for muon analysis
};
    
#endif
