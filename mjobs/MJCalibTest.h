#ifndef MARS_MJCalibTest
#define MARS_MJCalibTest

#ifndef MARS_MJCalib
#include "MJCalib.h"
#endif
#ifndef MARS_MCalibrationTestCam
#include "MCalibrationTestCam.h"
#endif
#ifndef MARS_MHCalibrationTestTimeCam
#include "MHCalibrationTestTimeCam.h"
#endif
#ifndef MARS_MBadPixelsCam
#include "MBadPixelsCam.h"
#endif

class MParList;
class MPedestalCam;
class MCalibrationRelTimeCam;
class MCalibrationChargeCam;
class MCalibrationQECam;
class MExtractor;
class MExtractTime;

class MJCalibTest : public MJCalib
{
private:

  Bool_t  fUseCosmicsFilter;

  MExtractor   *fExtractor;                  // Signal extractor
  MExtractTime *fTimeExtractor;              // Arrival time extractor  
  
  MBadPixelsCam            fBadPixels;
  MCalibrationTestCam      fTestCam;
  MHCalibrationTestTimeCam fTestTimeCam;

  enum  Display_t                                      // Possible Display types
    {
      kDataCheckDisplay,
      kNormalDisplay
    };
  
  Display_t fDisplayType;                              // Chosen Display type

  TString fGeometry;
  
  void   DisplayResult(MParList &plist);

  Bool_t WriteResult();
  
  Bool_t ReadCalibration(TObjArray &o, MBadPixelsCam &bpix,
                         MExtractor* &ext1, MExtractor* &ext2, TString &geom) const;

  const char* GetOutputFileName() const;

public:

  MJCalibTest(const char *name=NULL, const char *title=NULL);
  
  const char* GetOutputFile() const;
  
  MCalibrationTestCam      &GetTestCam()      { return fTestCam; }
  MHCalibrationTestTimeCam &GetTestTimeCam()  { return fTestTimeCam; }  
  const MBadPixelsCam &GetBadPixels()  const { return fBadPixels; }
  
  void SetExtractor(MExtractor* ext)              { fExtractor = ext; }
  void SetTimeExtractor(MExtractTime* ext)         { fTimeExtractor = ext; }
  void SetBadPixels(const MBadPixelsCam &bad) { bad.Copy(fBadPixels); }
  void SetUseCosmicsFilter( const Bool_t b )    { fUseCosmicsFilter = b;  }

  // Displays
  void SetDataCheckDisplay() { fDisplayType = kDataCheckDisplay; }
  void SetNormalDisplay()    { fDisplayType = kNormalDisplay;    }
  
  Bool_t Process(MPedestalCam &pedcam);
  
  ClassDef(MJCalibTest, 0) // Tool to calibrate and test the calibration run itself
};

#endif
