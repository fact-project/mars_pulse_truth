#ifndef MARS_MCalibrationRelTimePix
#define MARS_MCalibrationRelTimePix

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

class MCalibrationRelTimePix : public MCalibrationPix
{
private:

public:

  MCalibrationRelTimePix(const char *name=NULL, const char *title=NULL);
  ~MCalibrationRelTimePix() {}
  
  Float_t GetTimeOffset()       const { return GetMean();        }
  Float_t GetTimeOffsetErr()    const { return GetMeanErr();     }
  Float_t GetTimePrecision()    const { return GetSigma();       }
  Float_t GetTimePrecisionErr() const { return GetSigmaErr();    }

  ClassDef(MCalibrationRelTimePix, 1)	// Container Rel. Arrival Time Calibration Results Pixel
};

#endif

