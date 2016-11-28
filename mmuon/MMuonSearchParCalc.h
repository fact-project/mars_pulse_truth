#ifndef MARS_MMuonSearchParCalc
#define MARS_MMuonSearchParCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MHillas;
class MMuonSearchPar;
class MGeomCam;
class MSignalCam;

class MMuonSearchParCalc : public MTask
{
private:
    MGeomCam       *fGeomCam;     //!
    MSignalCam     *fSignalCam;   //!
    MHillas        *fHillas;      //! Pointer to the source independent hillas parameters
    MMuonSearchPar *fMuonPar;     //! Pointer to the output container for the new image parameters

    Int_t PreProcess(MParList *plist);
    Int_t Process();

public:
    MMuonSearchParCalc(const char *name=NULL, const char *title=NULL);

    ClassDef(MMuonSearchParCalc, 0) // task to calculate muon parameters
};

#endif

