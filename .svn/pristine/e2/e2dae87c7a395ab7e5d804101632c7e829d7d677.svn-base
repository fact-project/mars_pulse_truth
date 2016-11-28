#ifndef MARS_MCompProbCalc
#define MARS_MCompProbCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMcEvt;
class MParList;
class MParameterD;

class MCompProbCalc : public MTask
{
private:
    MParameterD *fHadronness; //! Output container (Hadronness)

    TList *fData;           //! List of MDataChains to be used
    TList *fHistVar;        //! List of variable bin size histograms

    void Fill(TList &list);
    void SetBinningHistVar();

public:
    MCompProbCalc(const char *name=NULL, const char *title=NULL);
    ~MCompProbCalc();

    Int_t PreProcess(MParList *plist);
    Int_t Process();

    ClassDef(MCompProbCalc, 1) // Task to calculate composite probabilities
};

#endif
