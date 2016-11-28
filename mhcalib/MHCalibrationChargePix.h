#ifndef MARS_MHCalibrationChargePix
#define MARS_MHCalibrationChargePix

#ifndef MARS_MHCalibrationPix
#include "MHCalibrationPix.h"
#endif

class MHCalibrationChargePix : public MHCalibrationPix
{

protected:

  TH1F     fHAbsTime;      // Histogram containing the absolute arrival times 
                          
  Int_t    fAbsTimeNbins;  // Number of  bins used for the fHAbsTime 
  Axis_t   fAbsTimeFirst;  // Lower bound bin used for the fHAbsTime
  Axis_t   fAbsTimeLast;   // Upper bound bin used for the fHAbsTime

public:

  MHCalibrationChargePix(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationChargePix() {}

  void Reset();  
  void InitBins();
  
  // Setters
  void SetBinningAbsTime(Int_t n, Axis_t lo, Axis_t up) { fAbsTimeNbins=n; fAbsTimeFirst=lo; fAbsTimeLast=up; }

  // Getters
        TH1F *GetHAbsTime()       { return &fHAbsTime;  }
  const TH1F *GetHAbsTime() const { return &fHAbsTime;  }

  const Float_t  GetAbsTimeMean() const;
  const Float_t  GetAbsTimeRms() const;
  const Float_t  GetIntegral() const;

  // Fill histos
  Bool_t FillAbsTime(const Float_t t);

  // Draws
  void Draw(Option_t *opt="");

  ClassDef(MHCalibrationChargePix, 1)     // Base Histogram class for Charge Pixel Calibration
};

#endif /* MARS_MHCalibrationChargePix */
