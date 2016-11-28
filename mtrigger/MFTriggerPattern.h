#ifndef MARS_MFTriggerPattern
#define MARS_MFTriggerPattern

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

#ifndef MARS_MTriggerPatter
#include "MTriggerPattern.h"
#endif

class MTriggerPattern;

class MFTriggerPattern : public MFilter
{
public:
    enum Prescale_t {
        kUnPrescaled,
        kPrescaled
    };

private:
  MTriggerPattern *fPattern;        //!

  Byte_t fMaskRequiredPrescaled;    // Mask for filtering Trigger Pattern allowed
  Byte_t fMaskRequiredUnprescaled;  // Mask for filtering Trigger Pattern allowed
  Byte_t fMaskDeniedPrescaled;      // Mask for filtering Trigger Pattern denied
  Byte_t fMaskDeniedUnprescaled;    // Mask for filtering Trigger Pattern denied

  Bool_t fDefault;                  // Default which is used if trigger pattern has default value (p==0&&u==0)

  Bool_t fResult;                   //! Calculated result to be returned by IsExpressionTrue
  Int_t  fCounter[4];               //! Counter for results

  // MFTriggerPattern
  void Require(const Byte_t mask, Prescale_t prescaled=kUnPrescaled);
  void Deny(const Byte_t mask, Prescale_t prescaled=kUnPrescaled);
  void Allow(const Byte_t mask, Prescale_t prescaled=kUnPrescaled);

  Int_t Eval();

  // MTask
  Int_t PreProcess(MParList *pList);
  Int_t Process();
  Int_t PostProcess();

  // MFilter
  Bool_t IsExpressionTrue() const { return fResult; }

public:
  MFTriggerPattern(const char *name=NULL, const char *title=NULL);
  MFTriggerPattern(MFTriggerPattern &trigpatt);

  // Setter
  void RequireTriggerLvl1(Prescale_t prescaled=kUnPrescaled) { Require(MTriggerPattern::kTriggerLvl1, prescaled); }
  void RequireTriggerLvl2(Prescale_t prescaled=kUnPrescaled) { Require(MTriggerPattern::kTriggerLvl2, prescaled); }
  void RequireCalibration(Prescale_t prescaled=kUnPrescaled) { Require(MTriggerPattern::kCalibration, prescaled); }
  void RequirePedestal(Prescale_t prescaled=kUnPrescaled)    { Require(MTriggerPattern::kPedestal, prescaled);    }
  void RequirePinDiode(Prescale_t prescaled=kUnPrescaled)    { Require(MTriggerPattern::kPinDiode, prescaled);    }
  void RequireSumTrigger(Prescale_t prescaled=kUnPrescaled)  { Require(MTriggerPattern::kSumTrigger, prescaled);  }

  void DenyTriggerLvl1(Prescale_t prescaled=kUnPrescaled)    { Deny(MTriggerPattern::kTriggerLvl1, prescaled); }
  void DenyTriggerLvl2(Prescale_t prescaled=kUnPrescaled)    { Deny(MTriggerPattern::kTriggerLvl2, prescaled); }
  void DenyCalibration(Prescale_t prescaled=kUnPrescaled)    { Deny(MTriggerPattern::kCalibration, prescaled); }
  void DenyPedestal(Prescale_t prescaled=kUnPrescaled)       { Deny(MTriggerPattern::kPedestal, prescaled);    }
  void DenyPinDiode(Prescale_t prescaled=kUnPrescaled)       { Deny(MTriggerPattern::kPinDiode, prescaled);    }
  void DenySumTrigger(Prescale_t prescaled=kUnPrescaled)     { Deny(MTriggerPattern::kSumTrigger, prescaled);  }

  void AllowTriggerLvl1(Prescale_t prescaled=kUnPrescaled)   { Allow(MTriggerPattern::kTriggerLvl1, prescaled); }
  void AllowTriggerLvl2(Prescale_t prescaled=kUnPrescaled)   { Allow(MTriggerPattern::kTriggerLvl2, prescaled); }
  void AllowCalibration(Prescale_t prescaled=kUnPrescaled)   { Allow(MTriggerPattern::kCalibration, prescaled); }
  void AllowPedestal(Prescale_t prescaled=kUnPrescaled)      { Allow(MTriggerPattern::kPedestal, prescaled);    }
  void AllowPinDiode(Prescale_t prescaled=kUnPrescaled)      { Allow(MTriggerPattern::kPinDiode, prescaled);    }
  void AllowSumTrigger(Prescale_t prescaled=kUnPrescaled)    { Allow(MTriggerPattern::kSumTrigger, prescaled);  }

  void DenyAll(Prescale_t prescaled=kUnPrescaled);
  void AllowAll(Prescale_t prescaled=kUnPrescaled);

  void Require(TString patt, Prescale_t prescaled=kUnPrescaled);
  void Deny(TString patt, Prescale_t prescaled=kUnPrescaled);

  void SetDefault(Bool_t b) { fDefault=b; }

  // Low level settings. USE THESE ONLY IF YOU ARE AN EXPERT!
  void SetMaskRequired(const Byte_t mask, Prescale_t prescaled=kUnPrescaled);
  void SetMaskDenied(const Byte_t mask, Prescale_t prescaled=kUnPrescaled);

  ClassDef(MFTriggerPattern, 1) // A Filter for the Trigger Pattern
};

#endif
