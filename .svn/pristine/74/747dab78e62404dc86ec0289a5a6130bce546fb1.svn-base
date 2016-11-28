#ifndef MARS_MFiltercutsCalc
#define MARS_MFiltercutsCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFiltercutsCalc                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MParList;
class MFilterList;
class MHadronness;


class MFiltercutsCalc : public MTask
{
private:
    MFilterList *fList;       //->
    MHadronness *fHadronness; //!
    TString     fHadronnessName;  // name of container to store hadronness

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    void AddToList(MFilter *f);

public:
    MFiltercutsCalc(const char *name=NULL, const char *title=NULL);
    ~MFiltercutsCalc();

    void SetHadronnessName(const TString name) { fHadronnessName = name; }
    TString GetHadronnessName() const { return fHadronnessName; }

    //void AddCut(const char *name, Double_t lo, Double_t up);
    //void AddCut(const char *name, Double_t val);
    void AddCut(const char *cut);
    void AddCut(MFilter *f);

    void Print(Option_t *opt=NULL) const;

    ClassDef(MFiltercutsCalc, 0) // A class to evaluate Filtercuts
};

#endif
