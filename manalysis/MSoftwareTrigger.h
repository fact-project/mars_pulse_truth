#ifndef MARS_MSoftwareTrigger
#define MARS_MSoftwareTrigger

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MSoftwareTrigger : public MParContainer
{
private:
     Short_t fPatch;
    Double_t fBaseline;
    UShort_t fPosition;
    Double_t fAmplitude;

public:
    MSoftwareTrigger(const char *name=NULL, const char *title=NULL);

    void SetData(Short_t i, Double_t bl, UShort_t pos, Double_t ampl)
    {
        fPatch = i;
        fBaseline = bl;
        fAmplitude = ampl;
        fPosition = pos;
    }

    ClassDef(MSoftwareTrigger, 1) // Storage Container for software trigger
};

#endif

