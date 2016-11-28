#ifndef MARS_MChisqEval
#define MARS_MChisqEval

#ifndef ROOT_MTask
#include "MTask.h"
#endif

class MData;
class MParameterD;

class MChisqEval : public MTask
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    MData   *fData0; // Data Member one (monte carlo data or chisq function)
    MData   *fData1; // Data Member two (measured data)

    MParameterD *fWeight;  //! Storage for weight
    MParameterD *fResult;  //! Storage for result

    TString fNameResult;
    TString fNameWeight;

    Double_t     fChisq;   //! Evaluated chi square
    Double_t     fSumW;    //! Sum of weights

    void StreamPrimitive(std::ostream &out) const;

    enum { kIsOwner = BIT(14) };

    Int_t PreProcess(MParList *plist);
    Int_t Process();
    Int_t PostProcess();

public:
    MChisqEval(const char *name=NULL, const char *title=NULL);
    MChisqEval(MData *y1, const char *name=NULL, const char *title=NULL);
    MChisqEval(MData *y1, MData *y2, const char *name=NULL, const char *title=NULL);
    ~MChisqEval();

    void SetY1(MData *data);
    void SetY2(MData *data);
    void SetY1(const TString data);
    void SetY2(const TString data);

    void SetOwner(Bool_t o=kTRUE) { o ? SetBit(kIsOwner) : ResetBit(kIsOwner); }
    void SetNameWeight(TString w="MWeight") { fNameWeight=w; }

    Double_t GetChisq() const { return fChisq; }
 
    ClassDef(MChisqEval, 0)
};

#endif
