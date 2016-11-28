#ifndef MARS_MHPedestalCor
#define MARS_MHPedestalCor

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TProfile
#include <TProfile.h>
#endif
#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

class MHPedestalCor : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    TProfile fProf;

    Int_t Fill(const MParContainer *par, const Stat_t w=1);

public:
    MHPedestalCor(const char *name=NULL, const char *title=NULL);

    void GetAutoCorrelation(TH2 &h) const;
    void GetAutoCorrelation(TMatrix &m) const;
    TMatrix GetAutoCorrelation() const
    {
        TMatrix m; GetAutoCorrelation(m); return m;
    }

    //void Paint(Option_t *o="");
    void Draw(Option_t *o="");

    ClassDef(MHPedestalCor, 1) // Histogram to get signal auto correlation
};

#endif
