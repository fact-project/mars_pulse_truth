#ifndef MARS_MCalibrationHiLoPix
#define MARS_MCalibrationHiLoPix

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

class MCalibrationHiLoPix : public MCalibrationPix
{
private:
    Float_t fOffsetPerSlice;            // Offset from fit (per FADC slice)
    Float_t fGainRatio;                 // Ratio of gains from fit

public:
  MCalibrationHiLoPix(const char *name=NULL, const char *title=NULL);

  // Setter
  void SetGainRatio     (const Float_t f) { fGainRatio      = f; }
  void SetOffsetPerSlice(const Float_t f) { fOffsetPerSlice = f; }

  // Getter
  Float_t GetHiLoChargeRatio()         const { return GetHiGainMean();     }
  Float_t GetHiLoChargeRatioErr()      const { return GetHiGainMeanErr();  }
  Float_t GetHiLoChargeRatioSigma()    const { return GetHiGainSigma();    }
  Float_t GetHiLoChargeRatioSigmaErr() const { return GetHiGainSigmaErr(); }
  Float_t GetHiLoChargeRatioProb()     const { return GetHiGainProb();     }
									   
  Float_t GetHiLoTimeDiff()            const { return GetLoGainMean();     }
  Float_t GetHiLoTimeDiffErr()         const { return GetLoGainMeanErr();  }
  Float_t GetHiLoTimeDiffSigma()       const { return GetLoGainSigma();    }
  Float_t GetHiLoTimeDiffSigmaErr()    const { return GetLoGainSigmaErr(); }
  Float_t GetHiLoTimeDiffProb()        const { return GetLoGainProb();     }
  Float_t GetGainRatio     ()          const { return fGainRatio;          }
//  Float_t GetOffsetPerSlice()          const { return fOffsetPerSlice;     }

  ClassDef(MCalibrationHiLoPix, 2)	// Container HiLo conversion Calibration Results Pixel
};

#endif

