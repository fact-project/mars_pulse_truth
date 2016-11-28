#ifndef MARS_MExtractPINDiode
#define MARS_MExtractPINDiode

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MExtractPINDiode                                                        //
//                                                                         //
// Integrates the time slices of the all pixels of a calibration event     //
// and substract the pedestal value                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class TH1F;
class MExtractedSignalPINDiode;

class MExtractPINDiode : public MExtractor
{
private:

  static const UInt_t fgPINDiodeIdx;  
  static const Byte_t fgHiGainFirst;     // First FADC slice Hi-Gain (now set to: 3) 
  static const Byte_t fgHiGainLast;      // Last  FADC slice Hi-Gain (now set to: 14) 
  static const Byte_t fgLoGainFirst;     // First FADC slice Lo-Gain (now set to: 3) 
  static const Byte_t fgLoGainLast;      // Last  FADC slice Lo-Gain (now set to: 14) 

  static const Byte_t fgLowerFitLimit;   // Default for fLowerFitLimit (now set to: 2)
  static const Byte_t fgUpperFitLimit;   // Default for fUpperFitLimit (now set to: 5)

  Byte_t fLowerFitLimit;                 // Number of FADC slices before maximum to start fit
  Byte_t fUpperFitLimit;                 // Number of FADC slices after maximum to end fit
  
  MExtractedSignalPINDiode  *fPINDiode;  // Extracted signal of the PIN Diode
  TH1F                      *fSlices;    // Histogram to fit the slices
  
  UInt_t  fPINDiodeIdx;                  // PIN Diode pixel ID
  
  MArrayF fPedMean;                      // The used pedestals (0: ped+AB, 1: ped-AB)
  
  Int_t  PreProcess( MParList *pList );
  Bool_t ReInit    ( MParList *pList );  
  Int_t  Process    ();
  Int_t  PostProcess();
  
public:

  MExtractPINDiode(const char *name=NULL, const char *title=NULL);
  ~MExtractPINDiode();  

  // Setters
  void SetRange(UShort_t hifirst=0, UShort_t hilast=0, Int_t lofirst=0, Byte_t lolast=0);
  void SetPINDiodeIdx  ( const UInt_t idx=fgPINDiodeIdx    ) { fPINDiodeIdx   = idx; }
  void SetLowerFitLimit( const Byte_t lim=fgLowerFitLimit  ) { fLowerFitLimit = lim; }
  void SetUpperFitLimit( const Byte_t lim=fgUpperFitLimit  ) { fUpperFitLimit = lim; }     

  ClassDef(MExtractPINDiode, 1) // Signal Extractor for the PIN Diode
};

#endif
