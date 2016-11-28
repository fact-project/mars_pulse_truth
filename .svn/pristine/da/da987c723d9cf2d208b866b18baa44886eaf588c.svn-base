#ifndef MARS_MTriggerPatternDecode
#define MARS_MTriggerPatternDecode

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MRawEvtHeader;
class MRawRunHeader;
class MTriggerPattern;

class MTriggerPatternDecode : public MTask
{
private:
    MRawRunHeader   *fRunHeader;
    MRawEvtHeader   *fEvtHeader;
    MTriggerPattern *fPattern;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MTriggerPatternDecode(const char *name=0, const char *title=0);

    ClassDef(MTriggerPatternDecode, 1) // Task to decode the Trigger Pattern
};

#endif
