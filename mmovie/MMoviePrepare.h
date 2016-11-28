#ifndef MARS_MMoviePrepare
#define MARS_MMoviePrepare

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MExtractor;
class MMovieData;
class MPedestalCam;
class MRawRunHeader;
class MCalibConstCam;
class MCalibrationRelTimeCam;
class MPedestalSubtractedEvt;

class MMoviePrepare : public MTask
{
private:
    MPedestalSubtractedEvt *fEvt;    //! Slices with pedestal subtracted
    MCalibConstCam         *fCal;    //! Calibration constants signal
    MCalibrationRelTimeCam *fRel;    //! Calibration constants time
    MRawRunHeader          *fRun;    //! Run Header (sampling frequency)
    MGeomCam               *fCam;    //! Pixel size
    MPedestalCam           *fPed;    //! Fundamental pedestal (slice by slice)

    MMovieData             *fOut;    //! Prepared output container

    UInt_t fFirstSlice;
    UInt_t fLastSlice;

    // MTask
    Int_t  PreProcess(MParList *plist);
    Int_t  Process();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MMoviePrepare
    Double_t GetMedianCalibConst() const;
    Double_t GetMedianPedestalRms() const;

public:
    MMoviePrepare(const char *name=NULL, const char *title=NULL);

    void SetRangeFromExtractor(const MExtractor &ext);

    ClassDef(MMoviePrepare, 0) // Task to prepare a movie
};

#endif
