#ifndef MARS_MBadPixelsPix
#define MARS_MBadPixelsPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MBadPixelsPix : public MParContainer
{
private:
    TArrayI fInfo;

public:
    MBadPixelsPix(const char* name=NULL, const char* title=NULL);

    enum UnsuitableType_t {
        kUnsuitableRun = BIT(1),
        kUnsuitableEvt = BIT(2),
        kUnsuitable    = kUnsuitableRun|kUnsuitableEvt,
        kUnreliableRun = BIT(3),
        kUnreliableEvt = BIT(4),
        kUnreliable    = kUnreliableRun|kUnreliableEvt
    };

    static const Int_t fgRunMask; // All types which are not event wise determined

    // All types are initialized to normal behaviour
    enum UncalibratedType_t {
      kPreviouslyExcluded       = BIT(0),
      kHiGainNotFitted          = BIT(1),
      kLoGainNotFitted          = BIT(2), 
      kRelTimeNotFitted         = BIT(3),
      kHiGainOscillating        = BIT(4),
      kLoGainOscillating        = BIT(5),
      kRelTimeOscillating       = BIT(6),
      kLoGainSaturation         = BIT(7),
      kChargeIsPedestal         = BIT(8),
      kChargeErrNotValid        = BIT(9),
      kChargeRelErrNotValid     = BIT(10),
      kChargeSigmaNotValid      = BIT(11),
      kMeanTimeInFirstBin       = BIT(12),
      kMeanTimeInLast2Bins      = BIT(13), 
      kDeviatingNumPhes         = BIT(14),
      kDeviatingNumPhots        = BIT(15),
      kDeviatingFFactor         = BIT(16),
      kDeviatingRelTimeResolution  = BIT(17),
      kConversionHiLoNotValid   = BIT(18),
      kHiGainOverFlow           = BIT(19),
      kLoGainOverFlow           = BIT(20),
      kHiLoNotFitted            = BIT(21),
      kHiLoOscillating          = BIT(22),
      kDeadPedestalRms          = BIT(23),
      kDeviatingAbsTimeResolution  = BIT(24),
      kLoGainBlackout           = BIT(25),
    };
    
    // This is just a start..
    enum HardwareType_t {
      kHVNotNominal             = BIT(1)
    };
    
      
    void Reset();
    void Clear(Option_t *o="");
    void Copy(TObject &object) const
    {
        static_cast<MBadPixelsPix&>(object).fInfo = fInfo;
    }

    // Setter
    void SetUnsuitable  (UnsuitableType_t   typ) { fInfo[0] |= typ; }
    void SetUncalibrated(UncalibratedType_t typ) { fInfo[1] |= typ; }
    void SetHardware    (HardwareType_t     typ) { fInfo[2] |= typ; }

    // Getter
    Bool_t IsUnsuitable  (UnsuitableType_t   typ) const { return fInfo[0]&typ; }
    Bool_t IsUncalibrated(UncalibratedType_t typ) const { return fInfo[1]&typ; }
    Bool_t IsHardwareBad (HardwareType_t     typ) const { return fInfo[2]&typ; }

    Bool_t IsUnsuitable() const   { return fInfo[0]&kUnsuitable; }
    Bool_t IsUnreliable() const   { return fInfo[0]&kUnreliable; }
    Bool_t IsUncalibrated() const { return fInfo[1]!=0; }

    Bool_t IsOK()  const { return fInfo[0]==0; }
    Bool_t IsBad() const { return fInfo[0]!=0; }

    Bool_t IsLoGainBad() const { return IsUnsuitable  (kUnsuitableRun      )
				     || IsUncalibrated(kLoGainSaturation   ) 
				     || IsUncalibrated(kConversionHiLoNotValid) 
				     || IsUncalibrated(kLoGainOscillating  )
				     || IsUncalibrated(kLoGainBlackout  ); }
    Bool_t IsHiGainBad() const { return IsUnsuitable  (kUnsuitableRun      )
  				     || IsUncalibrated(kHiGainOscillating  ) ; }

    Int_t  GetUnsuitableCalLevel() const   {
      if (!IsUnsuitable())                           return 0;
      if (IsUncalibrated( kChargeIsPedestal    ))    return 1;
      if (IsUncalibrated( kLoGainSaturation    ))    return 2;
//      if (IsUncalibrated( kMeanTimeInFirstBin  ))    return 3;
//      if (IsUncalibrated( kMeanTimeInLast2Bins ))    return 4;
      if (IsUncalibrated( kHiGainOverFlow      ))    return 3;
      if (IsUncalibrated( kLoGainOverFlow      ))    return 4;
      if (IsUncalibrated( kDeadPedestalRms     ))    return 5;
      if (IsUncalibrated( kDeviatingNumPhes    ))    return 6;
      //if (IsUncalibrated( kFluctuatingArrivalTimes)) return 7;
      if (IsUncalibrated( kDeviatingAbsTimeResolution)) return 7;
      if (IsUncalibrated( kDeviatingRelTimeResolution)) return 8;
      if (IsUncalibrated( kLoGainBlackout      ))    return 9;
      if (IsUncalibrated( kPreviouslyExcluded  ))    return 10;
      return 11;
    }

    static TString GetUnsuitableName(Int_t lvl)
    {
        switch (lvl)
        {
        case  1: return "Signal smaller 4.5 Pedestal RMS";
        case  2: return "Low Gain Saturation";
        case  3: return "High-Gain Histogram Overflow";
        case  4: return "Low-Gain Histogram Overflow";
        case  5: return "Presumably dead from Ped. Rms";
        case  6: return "Deviating Number of Photo-electrons";
        case  7: return "Deviation from median abs.arr-time rms";
        case  8: return "Deviation from median rel.arr-time rms";
        case  9: return "Too many Low-Gain Blackout Events";
        case 10: return "Previously Excluded";
        default: return "";
        }
    }

    static Int_t GetNumUnsuitable() { return 10; }

    Int_t  GetUnreliableCalLevel() const   {
      if (!IsUnreliable() || IsUnsuitable())      return 0;
      if (IsUncalibrated( kChargeSigmaNotValid )) return 1;
      if (IsUncalibrated( kHiGainNotFitted     )) return 2;
      if (IsUncalibrated( kLoGainNotFitted     )) return 3;
      if (IsUncalibrated( kRelTimeNotFitted    )) return 4;
      if (IsUncalibrated( kHiGainOscillating   )) return 5;
      if (IsUncalibrated( kLoGainOscillating   )) return 6;
      if (IsUncalibrated( kRelTimeOscillating  )) return 7;
      if (IsUncalibrated( kDeviatingFFactor    )) return 8;
      return 9;
    }

    static TString GetUnreliableName(Int_t lvl)
    {
        switch (lvl)
        {
        case 1: return "Signal Sigma smaller Pedestal RMS";
        case 2: return "High Gain Signals could not be fitted";
        case 3: return "Low  Gain Signals could not be fitted";
        case 4: return "Relative Arr. Times could not be fitted";
        case 5: return "High Gain Signals Oscillation";
        case 6: return "Low  Gain Signals Oscillation";
        case 7: return "Relative Arr. Times Oscillation";
        case 8: return "Deviating global F-Factor";
        default: return "";
        }
    }

    static Int_t GetNumUnreliable() { return 8; }

    void Merge(const MBadPixelsPix &pix);

    const TArrayI &GetInfo() const { return fInfo; }

    ClassDef(MBadPixelsPix, 1)	// Storage Container for bad pixel information of a single pixel
};

#endif

