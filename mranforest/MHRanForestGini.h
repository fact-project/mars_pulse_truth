#ifndef MARS_MHRanForestGini
#define MARS_MHRanForestGini

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif
#ifndef ROOT_TGraph
#include <TGraph.h>
#endif
#ifndef ROOT_TPaveText
#include <TPaveText.h>
#endif

class MParList;
class MRanForest;
class MRanTree;

class MHRanForestGini : public MH
{
private:
    MRanForest *fRanForest;  //!

    TArrayF fGini;           //!

    TGraph  fGraphGini;
    TGraph  fGraphError;
    TGraph  fGraphNodes;

    TPaveText fRules;

public:
    MHRanForestGini(Int_t nbins=100, const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void Draw(Option_t *opt="");

    ClassDef(MHRanForestGini, 1)
};

#endif
