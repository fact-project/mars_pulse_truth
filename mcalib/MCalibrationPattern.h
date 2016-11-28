#ifndef MARS_MCalibrationPattern
#define MARS_MCalibrationPattern

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationPattern : public MParContainer
{
    friend class MCalibrationPatternDecode;

public:

    enum CLColor_t 
      {
        kCLUV       = BIT(0),
        kCLGreen    = BIT(1),
        kCLAmber    = BIT(2),
        kCLRed      = BIT(3),
        kCLRedAmber   = kCLRed & kCLAmber,
        kCLRedGreen   = kCLRed & kCLGreen,
        kCLRedUV      = kCLRed & kCLUV   ,
        kCLAmberGreen = kCLAmber & kCLGreen,
        kCLAmberUV    = kCLAmber & kCLUV   ,
        kCLGreenUV    = kCLGreen & kCLUV   ,
        kCLRedAmberGreen = kCLRedAmber & kCLGreen,
        kCLRedGreenUV    = kCLRedGreen & kCLUV,
        kCLAmberGreenUV  = kCLAmberGreen & kCLUV,
        kCLAll           = kCLRedAmberGreen & kCLUV,
	kCLNone    = BIT(4)
      };
    
private:

    UShort_t  fCLStrength;     // Continuous light strenth  
    CLColor_t fCLColor;        // Color (combinations) of the continuous light
    Float_t   fPulserStrength; // Strength of the pulsed light (in nr. of equiv. LEDs)
    
    MCalibrationCam::PulserColor_t fPulserColor; // Colour of the pulsed light

public:

    MCalibrationPattern(const char *name=0, const char *title=0);

    void Reset() { fCLStrength=0; fCLColor=kCLNone; fPulserColor=MCalibrationCam::kNONE; fPulserStrength=0.; }

    UShort_t   GetCLStrength()     const { return fCLStrength; }
    CLColor_t  GetCLColor()        const { return fCLColor; }
    Float_t    GetPulserStrength() const { return fPulserStrength; }

    const MCalibrationCam::PulserColor_t GetPulserColor() const { return fPulserColor; }
    TString GetPulserColorStr() const { return MCalibrationCam::GetPulserColorStr(fPulserColor); }

    void SetPulserColor   ( const MCalibrationCam::PulserColor_t col) { fPulserColor = col; }
    void SetPulserStrength( const Float_t strength ) { fPulserStrength = strength; }

    ClassDef(MCalibrationPattern, 1) // Container storing the decoded calibration pattern
};

#endif
