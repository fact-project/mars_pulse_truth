#ifndef MARS_MHMcRate
#define MARS_MHMcRate

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MHMcRate : public MParContainer
{

private:
    UShort_t fPartId;           // Type of particle

    Float_t fEnergyMax;         // Maximum Energy [TeV]
    Float_t fEnergyMin;         // Minimum Energy [TeV]

    Float_t fThetaMax;          // Maximum theta angle of run [?]
    Float_t fThetaMin;          // Minimum theta angle of run [?]
    Float_t fPhiMax;            // Maximum phi angle of run [?]
    Float_t fPhiMin;            // Minimum phi angle of run [?]

    Float_t fSolidAngle;        // Solid angle within which incident directions are distributed [sr]

    Float_t fImpactMax;         //[cm] Maximum impact parameter [cm]
    Float_t fImpactMin;         //[cm] Minimum impact parameter [cm]

    Float_t fBackTrig;          // Number of triggers from background
    Float_t fBackSim;           // Number of simulated showers for the background

    Float_t fSpecIndex;         // dn/dE = k * e^{- fSpecIndex}
    Float_t fFlux0;             // dn/dE = fFlux0 * E^{-a}

    Float_t fShowerRate;        // Showers rate in Hz
    Float_t fShowerRateError;   // Estimated error of shower rate in Hz

    Float_t fTriggerRate;       // Trigger rate in Hz
    Float_t fTriggerRateError;  // Estimated error for the trigger rate in Hz

    Float_t fMeanThreshold;     // Mean discriminator threshold of trigger pixels [mV]

    Short_t fMultiplicity;      // L1 trigger multiplicity.

    Short_t fTriggerCondNum;    // Trigger condition number, for the case of running over camra files containing several.

    void Init(const char *name, const char *title);

public:

    MHMcRate(const char *name=NULL, const char *title=NULL);
    MHMcRate(Float_t showrate,
             const char *name=NULL, const char *title=NULL);
    MHMcRate(Float_t specindex, Float_t flux0,
             const char *name=NULL, const char *title=NULL);

    void SetParticle(UShort_t part);
    void SetBackground(Float_t showers, Float_t triggers);
    void SetFlux(Float_t flux0, Float_t specindx);
    void SetIncidentRate(Float_t showerrate);

    void SetImpactMax(Float_t Impact) {fImpactMax=Impact;}
    void SetImpactMin(Float_t Impact) {fImpactMin=Impact;}

    void SetThetaMax(Float_t Theta) {fThetaMax=Theta;}
    void SetThetaMin(Float_t Theta) {fThetaMin=Theta;}
    void SetPhiMax(Float_t Phi) {fPhiMax=Phi;}
    void SetPhiMin(Float_t Phi) {fPhiMin=Phi;}

    void SetSolidAngle(Float_t Solid) {fSolidAngle=Solid;}
    void SetEnergyMax(Float_t Energy) {fEnergyMax=Energy;}
    void SetEnergyMin(Float_t Energy) {fEnergyMin=Energy;}

    void SetMultiplicity(Short_t nMul) {fMultiplicity = nMul;}
    void SetMeanThreshold(Float_t thresh) {fMeanThreshold = thresh;}

    void SetTriggerCondNum(Short_t num) {fTriggerCondNum = num;}

    void UpdateBoundaries(Float_t energy, Float_t theta, Float_t phi, Float_t impact);

    Float_t GetTriggerRate() {return fTriggerRate;}
    Float_t GetTriggerRateError() {return fTriggerRateError;}

    Short_t GetMultiplicity() {return fMultiplicity;}
    Float_t GetMeanThreshold() {return fMeanThreshold;}
    Short_t GetTriggerCondNum() {return fTriggerCondNum;}


    void CalcRate(Float_t trig, Float_t anal, Float_t simu);

    void Print(Option_t *o=NULL) const;

    void Draw(Option_t *o=NULL);

    ClassDef(MHMcRate, 1)  // Data Container to calculate trigger rate
};

#endif 



