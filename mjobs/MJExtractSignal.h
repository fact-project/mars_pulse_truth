#ifndef MARS_MJExtractSignal
#define MARS_MJExtractSignal

#ifndef MARS_MPedPhotCam
#include "MPedPhotCam.h"
#endif
#ifndef MARS_MBadPixelsCam
#include "MBadPixelsCam.h"
#endif

class TCanvas;
class MHCamera;
class MCamEvent;
class MRunIter;
class MParList;
class MPedestalCam;
class MCalibrationChargeCam;
class MCalibrationQECam;
class MExtractor;
class MExtractTime;

class MJExtractSignal : public MParContainer
{
private:
    TString fOutputPath;

    MRunIter *fRuns;
    
    MExtractor   *fExtractor;                  // Signal extractor
    MExtractTime *fTimeExtractor;              // Arrival time extractor  

    MPedPhotCam   fPedPhotCam;
    MBadPixelsCam fBadPixels;

    void   DisplayResult(MParList &plist);
    Bool_t WriteResult();

    Bool_t ReadPedPhotCam();

    Bool_t ProcessFileD(MPedestalCam &pedcam);
    Bool_t ProcessFileP(MPedestalCam &pedcam, MCalibrationChargeCam &calcam, MCalibrationQECam &qecam);

public:
    MJExtractSignal(const char *name=NULL, const char *title=NULL);

    void SetInput(MRunIter *iter) { fRuns = iter; }
    void SetOutputPath(const char *path=".");

    TString GetOutputFileP() const;
    TString GetOutputFileD() const;

    MPedPhotCam &GetPedPhotCam() { return fPedPhotCam; }
    const MBadPixelsCam &GetBadPixels()  const { return fBadPixels; }

    void SetExtractor(MExtractor* ext)              { fExtractor = ext; }
    void SetTimeExtractor(MExtractTime* ext)         { fTimeExtractor = ext; }
    void SetBadPixels(const MBadPixelsCam &bad) { bad.Copy(fBadPixels); }

    Bool_t ProcessD(MPedestalCam &pedcam);
    Bool_t ProcessP(MPedestalCam &pedcam, MCalibrationChargeCam &calcam, MCalibrationQECam &qecam);

    ClassDef(MJExtractSignal, 0) // Tool to create a pedestal file (MPedestalCam)
};

#endif
