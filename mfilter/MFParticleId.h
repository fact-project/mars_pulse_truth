#ifndef MARS_MFParticleId
#define MARS_MFParticleId

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFParticleId                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcEvt;
class MParList;

class MFParticleId : public MFilter
{
private:
    MMcEvt *fMcEvt;
    TString fContName;

    typedef enum { kEEqual, kENotEqual } FilterType_t;
    FilterType_t fFilterType;

    Bool_t fResult;    //!
    Int_t  fValue;

    void Init(const char type, const Int_t val,
              const char *name, const char *title);

    void StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MFParticleId(const char *cname="MMcEvt", const char type='=', const Int_t val=0,
                 const char *name=NULL, const char *title=NULL);
    MFParticleId(MMcEvt *mcevt, const char type='=', const Int_t val=0,
                 const char *name=NULL, const char *title=NULL);

    Bool_t IsExpressionTrue() const;

    ClassDef(MFParticleId, 1) // A Filter for the (corsika) particle Id
};

#endif
