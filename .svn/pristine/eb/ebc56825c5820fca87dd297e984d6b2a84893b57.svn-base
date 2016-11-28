#ifndef MARS_MHDrsCalibrationTime
#define MARS_MHDrsCalibrationTime

#ifndef MARS_DrsCalib
#include "DrsCalib.h"
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

class MRawEvtData;
class MDrsCalibrationTime;
class MPedestalSubtractedEvt;

class MHDrsCalibrationTime : public MH
{
private:
    TH2F fHist;

    MRawEvtData            *fRaw;     //!
    MPedestalSubtractedEvt *fEvt;     //!
    MDrsCalibrationTime    *fCal;     //!

    DrsCalibrateTime fData; //

    void InitHistogram();

    template<typename T>
        Bool_t CopyData(std::vector<T> &dest) const;

public:
    MHDrsCalibrationTime(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void Draw(Option_t *o="");
    void Paint(Option_t *o="");

    void PlotAll();

    //const MDrsCalibration &GetCalib() const { return fData; }

    ClassDef(MHDrsCalibrationTime, 1) // A list of histograms storing the Fadc spektrum of one pixel
};

#endif

