#ifndef MARS_MHEffectiveOnTime
#define MARS_MHEffectiveOnTime

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef MARS_MTime
#include "MTime.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef ROOT_TH2
#include <TH2.h>
#endif
#ifndef ROOT_TF1
#include <TF1.h>
#endif

class MTime;
class MPointingPos;
class MParameterDerr;

class MParList;

class MHEffectiveOnTime : public MH
{
private:
    MPointingPos   *fPointPos; //! Container to get the zenith angle from
    MTime           fLastTime; //! Time-Stamp of last event

    MTime          *fTime;     //! Time-Stamp of "effective on-time" event
    MParameterDerr *fParam;    //! Output container for effective on-time

    TH2D fH2DeltaT;      // Counts vs Delta T and Theta
    TH1D fH1DeltaT;      //! Counts vs Delta T (for a time interval)

    TH1D fHThetaEffOn;   // Effective On time versus Theta
    TH1D fHThetaProb;    // Chisq prob fit of Effective On time versus Theta
    TH1D fHThetaNDF;     // NDF vs Theta
    TH1D fHThetaLambda;  // Slope (rate) vs Theta

    TH1D fHTimeEffOn;    // Effective On time versus Time
    TH1D fHTimeProb;     // Chisq prob fit of Effective On time versus Time
    TH1D fHTimeLambda;   // Slope (rate) vs Time

    Bool_t fIsFinalized; // Flag telling you whether fHThetaEffOn is the final result

    UInt_t fNumEvents;   // Number of events to be used for a bin in time
    UInt_t fFirstBin;    // For bin for fit

    Double_t fTotalTime;    //[s] Total time accumulated from run-header
    Double_t fEffOnTime;    //[s] On time as fitted from the DeltaT distribution
    Double_t fEffOnTimeErr; //[s] On time error as fitted from the DeltaT distribution

    // MHEffectiveOnTime
    Bool_t FitH(TH1D *h, Double_t *res, Bool_t paint=kFALSE) const;
    void FitThetaBins();
    void FitTimeBin();
    void PaintProb(TH1 &h) const;
    void PaintText(Double_t val, Double_t error, Double_t range=-1) const;
    void PaintText(Double_t *res) const;
    void DrawRightAxis(const char *title);
    void UpdateRightAxis(TH1 &h);
    void PrintStatistics();

    // MH
    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MHEffectiveOnTime(const char *name=NULL, const char *title=NULL);

    // Setter
    void SetNumEvents(Int_t i) { fNumEvents=i; }
    void SetFirstBin(Int_t i)  { fFirstBin=i;  }

    // Getter
    Double_t GetTotalTime() const { return fTotalTime; }
    Double_t GetEffOnTime() const { return fEffOnTime; }

    const TH1D &GetHEffOnTheta() const { return fHThetaEffOn; }
    const TH1D &GetHEffOnTime() const { return fHTimeEffOn; }

    const TH2D &GetH2DeltaT() const { return fH2DeltaT; }

    // TObject
    void Draw(Option_t *option="");
    void Paint(Option_t *opt="");

    ClassDef(MHEffectiveOnTime, 4) // Histogram to determin effective On-Time vs Time and Zenith Angle
};

#endif

