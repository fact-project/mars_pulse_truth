#ifndef MARS_MExtractTimeAndCharge
#define MARS_MExtractTimeAndCharge

#ifndef MARS_MExtractTime
#include "MExtractTime.h"
#endif

class MPedestalPix;

class MExtractTimeAndCharge : public MExtractTime
{
private:
  static const Float_t fgLoGainStartShift; //! Default for fLoGainStartShift (now set to: -2.8)
  static const UInt_t  fgLoGainSwitch;     //! Default for fLoGainSwitch     (now set to: 100)
  
  Float_t fLoGainStartShift;      // Shift to start searching the low-gain signal obtained from the high-gain times.
  UInt_t  fLoGainSwitch;          // Limit for max. bin content before the low-gain gets extracted

protected:
  Int_t  fWindowSizeHiGain;       //  Window Size High-Gain
  Int_t  fWindowSizeLoGain;       //  Window Size Low-Gain

  Int_t  PreProcess(MParList *pList);
  Int_t  Process();
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

  Double_t GetSaturationTime(Int_t sat0, const Float_t *sig, Int_t maxcont) const;

public:
  MExtractTimeAndCharge(const char *name=NULL, const char *title=NULL);
  
  Int_t   GetWindowSizeHiGain  () const { return fWindowSizeHiGain; }
  Int_t   GetWindowSizeLoGain  () const { return fWindowSizeLoGain; }
  Float_t GetLoGainStartShift  () const { return fLoGainStartShift; }
  UInt_t  GetLoGainSwitch      () const { return fLoGainSwitch;     }

  void SetLoGainStartShift( const Float_t f=fgLoGainStartShift ) { fLoGainStartShift = f;  }
  void SetLoGainSwitch    ( const UInt_t  i=fgLoGainSwitch     ) { fLoGainSwitch     = i; }

  virtual void SetWindowSize(Int_t windowh, Int_t windowl) { fWindowSizeHiGain = windowh; fWindowSizeLoGain = windowl;  }

  virtual Bool_t InitArrays(Int_t) { return kTRUE; }

  virtual void FindTimeAndChargeHiGain2(const Float_t */*firstused*/, Int_t /*num*/, Float_t &/*sum*/, Float_t &/*dsum*/,
                                       Float_t &/*time*/, Float_t &/*dtime*/,
                                       Byte_t /*sat*/, Int_t /*maxpos*/) const { }

  virtual void FindTimeAndChargeLoGain2(const Float_t */*firstused*/, Int_t /*num*/, Float_t &/*sum*/, Float_t &/*dsum*/,
                                       Float_t &/*time*/, Float_t &/*dtime*/,
                                       Byte_t /*sat*/, Int_t /*maxpos*/) const { }
  
  // For MExtractPedestal
  Bool_t ReInit(MParList *pList);

  // TObject
  void Print(Option_t *o="") const; //*MENU*

  ClassDef(MExtractTimeAndCharge, 4)   // Time And Charge Extractor Base Class
};

#endif
