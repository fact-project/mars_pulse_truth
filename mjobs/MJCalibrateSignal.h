#ifndef MARS_MJCalibrateSignal
#define MARS_MJCalibrateSignal

#ifndef MARS_MJCalib
#include "MJCalib.h"
#endif

class TEnv;
class TList;
class MTask;
class MParList;
class MGeomCam;
class MSequence;
class MExtractor;
class MPedestalCam;
class MBadPixelsCam;
class MRunIter;

class MJCalibrateSignal : public MJCalib
{
private:
    MExtractor *fExtractor;

    Bool_t fIsInterlaced;               // Distinguish interlaced from other calibration
    Bool_t fIsRelTimesUpdate;           // Choose to update relative times from interlaced
    Bool_t fIsMovieMode;                // Choose to encode a movie
    Bool_t fIsTestMode;                 // Testmode to calibrate the cal run

    Bool_t CheckEnvLocal();

    void   DisplayResult(MParList &plist);

    Bool_t WriteResult() const;
    Bool_t ReadCalibration(TObjArray &o, MBadPixelsCam &bpix,
                           MExtractor* &ext1, MExtractor* &ext2, TString &geom) const;
    Bool_t ReadExtractorCosmics(MExtractor* &ext1) const;

    const char* GetInputFile() const;
    
public:
    MJCalibrateSignal(const char *name=NULL, const char *title=NULL);
    ~MJCalibrateSignal();

    Bool_t Process(MPedestalCam &camab, MPedestalCam &cam1, MPedestalCam &cam2);

    void SetInterlaced    (const Bool_t b=kTRUE) { fIsInterlaced     = b; }
    void SetRelTimesUpdate(const Bool_t b=kTRUE) { fIsRelTimesUpdate = b; }
    void SetMovieMode     (const Bool_t b=kTRUE) { fIsMovieMode      = b; }
    void SetTestMode      (const Bool_t b=kTRUE) { fIsTestMode       = b; }

    void SetExtractor(const MExtractor *ext=NULL);

    ClassDef(MJCalibrateSignal, 0) // Tool to create a pedestal file (MPedestalCam)
};

#endif
