#ifndef MARS_MHPointing
#define MARS_MHPointing

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TGraph
#include <TGraph.h>
#endif

class MTime;
class MReportDrive;
class MReportStarguider;

class MHPointing : public MH
{
private:
    MReportDrive      *fReportCosy;  //!
    MReportStarguider *fReportSG;    //!

    TGraph             fDevTimeSG;   // Starguider deviation versus time
    TGraph             fDevTimeCosy; // Drive deviation versus time

    TGraph             fBrightness;  // Arbitrary sky brightness

    TGraph             fNumStars;    // Number of stars identified by starguider
    TGraph             fNumStarsCor; // Number of correlated stars identified by starguider

    TGraph             fDevZd;       // Starguider deviation Zd
    TGraph             fDevAz;       // Starguider deviation Az

    TGraph             fPosZd;       // Position Zd
    //TGraph             fPosAz;       // Position Az

    void ResetGraph(TGraph &g) const;
    void InitGraph(TGraph &g) const;
    void AddPoint(TGraph &g, Double_t x, Double_t y) const;
    void DrawGraph(TGraph &g, const char *y=0) const;
    void UpdateRightAxis(TGraph &g1, TGraph &g2) const;
    void DrawRightAxis(const char *title) const;

public:
    MHPointing(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    ClassDef(MHPointing, 2) // Histogram to display tracking/pointing information
};

#endif


