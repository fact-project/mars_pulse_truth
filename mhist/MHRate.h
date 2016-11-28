#ifndef MARS_MHRate
#define MARS_MHRate

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TGraph
#include <TGraph.h>
#endif
#ifndef ROOT_TProfile
#include <TProfile.h>
#endif
#ifndef MARS_MTime
#include "MTime.h"
#endif

//class MTime;
class MEventRate;
class MPointingPos;

class MHRate : public MH
{
private:
    TGraph   fRateTime;      // Graph of rate versus time
    TProfile fRateZd;        // Profile of rate vs Zd
    TH1D     fRate;          // Rate histogram

    //MTime        *fTime;   //!
    MPointingPos *fPointPos; //!
    MEventRate   *fEvtRate;  //!

    MTime fLast;             //!

    Double_t fMean;          //!
    Int_t    fCounter;       //!

    void ResetGraph(TGraph &g) const;
    void InitGraph(TGraph &g) const;
    void AddPoint(TGraph &g, Double_t x, Double_t y) const;
    void DrawGraph(TGraph &g, const char *y=0) const;
    //void UpdateRightAxis(TGraph &g) const;
    //void DrawRightAxis(const char *title) const;

public:
    MHRate(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    ClassDef(MHRate, 1) // Histogram to display information about rate
};

#endif


