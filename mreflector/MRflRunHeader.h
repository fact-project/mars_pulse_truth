#ifndef MARS_MRflRunHeader
#define MARS_MRflRunHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MRflRunHeader : public MParContainer
{
    Int_t fRunNumber;

public:
    MRflRunHeader(const char *name=NULL, const char *title=NULL);

    Int_t GetRunNumber() const { return fRunNumber; }
    void SetRunNumber(Int_t n) { fRunNumber = n; }

    ClassDef(MRflRunHeader, 0) // Header of an run from the reflector program
};

#endif
