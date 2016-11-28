#ifndef MARS_MHAlpha
#define MARS_MHAlpha

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

#ifndef MARS_MAlphaFitter
#include "MAlphaFitter.h"
#endif

#ifndef ROOT_TH3
#include <TH3.h>
#endif

class MParList;
class MParameterD;
class MParameterI;
class MHillas;
class MHMatrix;
class MPointingPos;

class MHAlpha : public MH
{
protected:
    TH3D    fHist;              // Alpha vs. theta and energy
    TH1D    fHistTime;          //! temporary histogram to get alpha vs. time

    TString fNameParameter;

    MParContainer *fParameter;  //!

    const TH3D *fOffData;

    MAlphaFitter fFit;          // SEEMS THAT STREAMER HAS SOME PROBLEMS... MAYBE IF FUNC IS USED AT THE SAME TIME FOR FITS (PAINT)

private:
    TH1D fHEnergy;              // excess events vs energy
    TH1D fHTheta;               // excess events vs theta
    TH1D fHTime;                // excess events vs time;

    MParameterD   *fResult;     //!
    MParameterD   *fSigma;      //!
    MParameterD   *fEnergy;     //!
    MParameterI   *fBin;        //!
    MPointingPos  *fPointPos;   //!

    MTime   *fTimeEffOn;        //! Time to steer filling of fHTime
    MTime   *fTime;             //! Event-Time to finalize fHTime
    MTime    fLastTime;         //! Last fTimeEffOn

    UShort_t fNumTimeBins;      // Number of time bins to fill together
    UShort_t fNumRebin;         //!

    //const TString fNameProjAlpha;  //! This should make sure, that gROOT doen't confuse the projection with something else
protected:
    MHillas  *fHillas;          //!
    MHMatrix *fMatrix;          //!
    Int_t fMap[15];             //!

    Bool_t fSkipHistTime;
    Bool_t fSkipHistTheta;
    Bool_t fSkipHistEnergy;
    Bool_t fForceUsingSize;

private:
     Float_t FitEnergyBins(Bool_t paint=kFALSE);
    void FitThetaBins(Bool_t paint=kFALSE);

    void UpdateAlphaTime(Bool_t final=kFALSE);
    void InitAlphaTime(const MTime &t);
    void FinalAlphaTime(MBinning &bins);

    void SetBin(Int_t bin);

    void PaintText(Double_t val, Double_t error) const;
    void PaintText(const TH1D &h) const;

    Int_t DistancetoPrimitive(Int_t px, Int_t py);

    virtual Bool_t      GetParameter(const MParList &pl);
    virtual Double_t    GetVal() const;
    virtual const char *GetParameterRule() const
    {
        return "MHillasSrc.fAlpha";
    }

public:
    MHAlpha(const char *name=NULL, const char *title=NULL);

    // MH
    Bool_t SetupFill(const MParList *pl);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    // MHAlpha
    const TH3D &GetHist() const { return fHist; }
    const MAlphaFitter &GetAlphaFitter() const { return fFit; }

    const TH1D &GetHEnergy() const { return fHEnergy; }

    const TH3D *GetOffData() const { return fOffData; }

    // Setter
    void SetNameParameter(const char *name) { fNameParameter=name; }
    virtual void SetOffData(const MHAlpha &h)
    {
        fOffData        = &h.fHist;
        fForceUsingSize =  h.fForceUsingSize;
        fNumTimeBins    =  h.fNumTimeBins;
    }
    void SetNumTimeBins(UShort_t n) { fNumTimeBins = n; }
/*
    void SetSizeCuts(Float_t min, Float_t max)   { fSizeMin=min; fSizeMax=max; }
    void SetSizeMin(Float_t min)                 { fSizeMin=min; }
    void SetSizeMax(Float_t max)                 { fSizeMax=max; }
    void SetEnergyCuts(Float_t min, Float_t max) { fEnergyMin=min; fEnergyMax=max; }
    void SetEnergyMin(Float_t min)               { fEnergyMin=min; }
    void SetEnergyMax(Float_t max)               { fEnergyMax=max; }

    void SetCuts(const MHAlpha &h) {
        fSizeMin = h.fSizeMin; fEnergyMin = h.fEnergyMin;
        fSizeMax = h.fSizeMax; fEnergyMax = h.fEnergyMax;
    }
    */

    void SkipHistTime(Bool_t b=kTRUE)   { fSkipHistTime=b; }
    void SkipHistTheta(Bool_t b=kTRUE)  { fSkipHistTheta=b; }
    void SkipHistEnergy(Bool_t b=kTRUE) { fSkipHistEnergy=b; }
    void ForceUsingSize(Bool_t b=kTRUE) { fForceUsingSize=b; }

    Bool_t FitAlpha()
    {
        return fFit.FitAlpha(fHist, fOffData);
    }

    void DrawNicePlot(const char *title="FACT observation", const char *watermark="preliminary", Int_t binlo=-1, Int_t binhi=9999) { DrawNicePlot(kTRUE, title, watermark, binlo, binhi); } //*MENU*
    void DrawNicePlot(Bool_t newc, const char *title=0, const char *watermark=0, Int_t binlo=-1, Int_t binhi=9999);
    void DrawAll() { DrawAll(kTRUE); } //*MENU*
    void DrawAll(Bool_t newc);

    virtual void InitMapping(MHMatrix *mat, Int_t type=0);
    void StopMapping();

    void ApplyScaling();

    // TObject
    void Paint(Option_t *opt="");
    void Draw(Option_t *option="");
    void RecursiveRemove(TObject *obj);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MHAlpha, 1) // Alpha-Plot which is fitted online
};

#endif
