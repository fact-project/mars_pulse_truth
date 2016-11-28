#ifndef MARS_MHDrsCalibration
#define MARS_MHDrsCalibration

#ifndef MARS_MHCamEvent
#include "MHCamEvent.h"
#endif

#ifndef MARS_DrsCalib
#include "DrsCalib.h"
#endif

class MRawRunHeader;
class MRawEvtData;
struct MDrsCalibration;

class MHDrsCalibration : public MHCamEvent
{
private:
    MRawRunHeader   *fRun;      //!
    MRawEvtData     *fEvt;      //!
    MDrsCalibration *fResult;   //!

    std::vector<float> fBuffer; //! Compilation bufer for writing output files
    DrsCalibrate       fData;   //!

    Short_t fStep;

    Double_t fScale;

    UInt_t fNumPixels;
    UInt_t fNumSamples;

    TString fOutputPath;

    void InitHistogram();

    template<typename T>
        Bool_t CopyData(std::vector<T> &dest) const;

public:
    MHDrsCalibration(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    Bool_t ResetSecondaryBaseline()
    {
        if (fStep!=2)
            return kFALSE;

        for (int i=1024*1440*4+4; i<1440*1024*6+160*1024*2+4; i++)
            fBuffer[i] = 0;

        fStep=1;

        return kTRUE;
    }

    void SetOutputPath(const char *path) { fOutputPath = path; }

    void Paint(Option_t *o="");

    Short_t GetNumUnderflows(float lvl) const;

    ClassDef(MHDrsCalibration, 1) // A list of histograms storing the Fadc spektrum of one pixel
};

#endif
