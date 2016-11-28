#ifndef MARS_MHDisp
#define MARS_MHDisp

#ifndef MARS_MHFalseSource
#include "MHFalseSource.h"
#endif
#ifndef ROOT_TH2
#include <TH2.h>
#endif
#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

class MParList;
class MHillasExt;
class MHillasSrc;
class MParameterD;
class MPointingDev;
class MSrcPosCam;

class MHDisp : public MHFalseSource
{
private:
    MParameterD  *fDisp;      //!
    MPointingDev *fDeviation; //!
    //MSrcPosCam   *fAxis;      //!
    MSrcPosCam   *fSrcAnti; //!


    TH2D         fHistBg;
    TH2D         fHistBg1;
    TH2D         fHistBg2;

    TVector2     fFormerSrc;
    Bool_t       fHalf;

    Double_t     fSmearing;    // [deg] Width of Gaus to fold with the events
    Bool_t       fWobble;      // This is for Wobble-Mode observations

    Double_t     fScaleMin;    // [deg] Minimum circle for integration of off-data for scaling
    Double_t     fScaleMax;    // [deg] Maximum circle for integration of off-data for scaling

    // MHDisp
    Double_t GetOffSignal(TH1 &h) const;

    void Update();

    void Profile1D(const char *name, const TH2 &h) const;
    void Profile(TH1 &h2, const TH2 &h1, Axis_t x0=0, Axis_t y0=0) const;
    void MakeSignificance(TH2 &s, const TH2 &h1, const TH2 &h2, const Double_t scale=1) const;
    void MakeDot(TH2 &h2) const;

    Double_t Sq(Double_t x, Double_t y) const { return x*x+y*y; }

public:
    MHDisp(const char *name=NULL, const char *title=NULL);

    // MHDisp
    void SetSmearing(Double_t s=-1) { fSmearing=s; }
    void SetWobble(Bool_t w=kTRUE)  { fWobble=w;   }

    void SetScaleMin(Double_t scale) { fScaleMin = scale; Update(); } //*MENU* *ARGS={scale=>fScaleMin}
    void SetScaleMax(Double_t scale) { fScaleMax = scale; Update(); } //*MENU* *ARGS={scale=>fScaleMax}
    void SetScale(Double_t min, Double_t max) { fScaleMin = min; fScaleMax = max; Update(); } //*MENU* *ARGS={min=>fScaleMin,max=>fScaleMax}

    Double_t GetScaleMin() { return fScaleMin; }
    Double_t GetScaleMax() { return fScaleMax; }

    // MHFalseSource (to be moved!)
    void SetOffData(const MHFalseSource &fs)
    {
        MHFalseSource::SetOffData(fs);
        if (dynamic_cast<const MHDisp*>(&fs))
        {
            const MHDisp &h = dynamic_cast<const MHDisp&>(fs);
            fWobble   = h.fWobble;
            fSmearing = h.fSmearing;

            h.fHistBg1.Copy(fHistBg1);
            h.fHistBg2.Copy(fHistBg2);

            fHistBg1.SetDirectory(0);
            fHistBg2.SetDirectory(0);
        }
    }

    // MH
    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    // TObject
    void Paint(Option_t *o="");
    void Draw(Option_t *o="");

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MHDisp, 3) //Class to provide a Disp map
};

#endif
