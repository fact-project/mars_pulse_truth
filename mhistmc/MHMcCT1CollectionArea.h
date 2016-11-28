#ifndef MARS_MHMcCT1CollectionArea
#define MARS_MHMcCT1CollectionArea

#ifndef MARS_MH
#include "MH.h"
#endif

class TH2D;

class MHMcCT1CollectionArea : public MH
{
public:
    enum FillType_t {
        kLog10,
        kLinear
    };

private:
    TH2D *fHistAll; //  all simulated showers
    TH2D *fHistSel; //  the selected showers
    TH2D *fHistCol; //  the collection area

    FillType_t fEaxis;

public:
    MHMcCT1CollectionArea(const char *name=NULL, const char *title=NULL);
    ~MHMcCT1CollectionArea();

    Bool_t SetupFill(const MParList *pList);
    Bool_t Fill(const MParContainer *par, const Stat_t w=1);

    void DrawAll(Option_t *option="");
    void DrawSel(Option_t *option="");

    void SetEaxis(FillType_t x) { fEaxis = x; }

    const TH2D *GetHist() const { return fHistCol; }
    const TH2D *GetHAll() const { return fHistAll; }
    const TH2D *GetHSel() const { return fHistSel; }

    void Draw(Option_t *option="");
    TObject *DrawClone(Option_t *option="") const;

    void CalcEfficiency();

    ClassDef(MHMcCT1CollectionArea, 1)  // Data Container to calculate Collection Area
};

#endif




