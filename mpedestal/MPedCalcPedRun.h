#ifndef MARS_MPedCalcPedRun
#define MARS_MPedCalcPedRun

#ifndef MARS_MExtractPedestal
#include "MExtractPedestal.h"
#endif

class MTriggerPattern;
class MPedestalPix;

class MPedCalcPedRun : public MExtractPedestal
{
private:
    static const UShort_t fgExtractWinFirst;  // First FADC slice Hi-Gain (currently set to: 3)
    static const UShort_t fgExtractWinSize;   // Extraction Size Hi-Gain (currently set to: 14)
    static const UInt_t   gkFirstRunWithFinalBits; // First Run with pedestal trigger bit at place 3

    Bool_t  fIsFirstPedRun;    //! Flag to tell if the first run out of many is used
    Bool_t  fIsNotPedRun;      //! Flag to tell if the current run is a pedestal run

    MTriggerPattern *fTrigPattern;  //! Trigger pattern decoded

    Bool_t IsPedBitSet();

    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    void   Calc();
    Int_t  PostProcess();

    //void CheckExtractionWindow();
    //UInt_t CalcSums(const MRawEvtPixelIter &pixel, UInt_t &ab0, UInt_t &ab1);
    //void CalcExtractor(const MRawEvtPixelIter &pixel, Float_t &sum, MPedestalPix &ped);

public:
    MPedCalcPedRun(const char *name=NULL, const char *title=NULL);

    void Print(Option_t *o="") const;
    void Reset();

    Bool_t SetRangeFromExtractor(const MExtractor &ext)
    {
        return MExtractPedestal::SetRangeFromExtractor(ext, kFALSE);
    }
    
    Int_t Finalize();

    ClassDef(MPedCalcPedRun, 2)   // Task to calculate pedestals from pedestal runs
};

#endif
