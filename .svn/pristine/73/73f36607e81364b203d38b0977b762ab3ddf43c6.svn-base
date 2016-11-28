#ifndef MARS_MJOptimizeCuts
#define MARS_MJOptimizeCuts

#ifndef MARS_MJOptimize
#include "MJOptimize.h"
#endif

class MHAlpha;

class MJOptimizeCuts : public MJOptimize
{
private:
    TString fNameHist;

    MHAlpha *CreateNewHist(const char *name=0) const;

    Bool_t RunOnCore(MHAlpha &hist, const char *fname, MFilter *filter, MAlphaFitter *fit);
    Bool_t RunOnOffCore(MHAlpha &histon, MHAlpha &histoff, const char *fname, MFilter *filter, MAlphaFitter *fit, const char *tree);

public:
    MJOptimizeCuts(const char *name="MHAlpha") : MJOptimize(), fNameHist(name) { }

    // Optimization On-Off/Wobble
    Bool_t RunOnOff(const char *fname, MFilter *filter, MAlphaFitter *fit=0, const char *tree="Events");
    Bool_t RunOnOff(const char *fname, MAlphaFitter *fit=0, const char *tree="Events")
    {
        return RunOnOff(fname, 0, fit, tree);
    }
    Bool_t RunOnOff(MFilter *filter, MAlphaFitter *fit=0, const char *tree="Events")
    {
        return RunOnOff(0, filter, fit, tree);
    }
    Bool_t RunOnOff(MAlphaFitter *fit=0, const char *tree="Events")
    {
        return RunOnOff(fit, tree);
    }

    // Optimization On-only
    Bool_t RunOn(const char *fname, MFilter *filter, MAlphaFitter *fit=0);
    Bool_t RunOn(const char *fname, MAlphaFitter *fit=0)
    {
        return RunOn(fname, 0, fit);
    }

    Bool_t RunOn(MFilter *filter, MAlphaFitter *fit=0)
    {
        return RunOn(0, filter, fit);
    }
    Bool_t RunOn(MAlphaFitter *fit=0)
    {
        return RunOn(0, 0, fit);
    }

    ClassDef(MJOptimizeCuts, 0) // Class for optimization of the Supercuts
};

#endif
