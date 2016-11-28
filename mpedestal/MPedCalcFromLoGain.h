#ifndef MARS_MPedCalcFromLoGain
#define MARS_MPedCalcFromLoGain

#ifndef MARS_MExtractPedestal
#include "MExtractPedestal.h"
#endif

#ifndef ROOT_MArrayB
#include "MArrayB.h"
#endif

//class MRawEvtPixelIter;
class MPedestalPix;

class MPedCalcFromLoGain : public MExtractPedestal
{
private:
    static const UShort_t fgExtractWinFirst;  // First FADC slice to use for pedestal calculation (currently set to: 15)
    static const UShort_t fgExtractWinSize;   // number of successive slices used to calculate pedestal (currently set to: 6)
    static const UInt_t   fgNumDump;          //!

    UInt_t  fNumEventsDump;            // Number of event after which MPedestalCam gets updated
    UInt_t  fNumAreasDump;             // Number of events after which averaged areas gets updated
    UInt_t  fNumSectorsDump;           // Number of events after which averaged sectors gets updated

    Bool_t  fPedestalUpdate;           // Flag if the pedestal shall be updated after every fNumEventsDump

    // MParContainer
    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Bool_t ReInit(MParList *pList);
    Int_t  PostProcess();

    // MExtractPedestal
    void   Calc();

public:
    MPedCalcFromLoGain(const char *name=NULL, const char *title=NULL);

    // Getters
    void SetNumEventsDump (UInt_t dumpevents=fgNumDump)  { fNumEventsDump  = dumpevents; }
    void SetNumAreasDump  (UInt_t dumpevents=fgNumDump)  { fNumAreasDump   = dumpevents; }
    void SetNumSectorsDump(UInt_t dumpevents=fgNumDump)  { fNumSectorsDump = dumpevents; }
    void SetNumDump       (UInt_t n=fgNumDump) { fNumEventsDump=n; fNumAreasDump=n; fNumSectorsDump=n; }

    void SetPedestalUpdate(Bool_t b=kTRUE) { fPedestalUpdate      = b; }

    Bool_t SetRangeFromExtractor(const MExtractor &ext)
    {
        return MExtractPedestal::SetRangeFromExtractor(ext, kTRUE);
    }

    void Print(Option_t *o="") const;

    ClassDef(MPedCalcFromLoGain, 1)   // Task to calculate pedestals from data runs
};

#endif
