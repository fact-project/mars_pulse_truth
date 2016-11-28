#ifndef MARS_MExtractFixedWindowPeakSearch
#define MARS_MExtractFixedWindowPeakSearch

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

class MExtractFixedWindowPeakSearch : public MExtractor
{
private:

  static const Byte_t fgHiGainFirst;          //! Default for fHiGainFirst          (now set to:  0)
  static const Byte_t fgHiGainLast;           //! Default for fHiGainLast 	    (now set to: 16)
  static const Byte_t fgLoGainFirst;          //! Default for fLoGainFirst	    (now set to:  3)
  static const Byte_t fgLoGainLast;           //! Default for fLoGainLast 	    (now set to: 14)
  static const Byte_t fgHiGainWindowSize;     //! Default for fWindowSizeHiGain     (now set to:  6)
  static const Byte_t fgLoGainWindowSize;     //! Default for fWindowSizeLoGain     (now set to:  6)
  static const Byte_t fgPeakSearchWindowSize; //! Default for fPeakSearchWindowSize (now set to:  4)
  static const Byte_t fgOffsetFromWindow;     //! Default for fOffsetFromWindow     (now set to:  1)
  static const Byte_t fgLoGainPeakShift;      //! Default for fLowGainPeakShift     (now set to:  1)

  Byte_t  fHiGainWindowSize;     // Number of Hi Gain slices in window
  Byte_t  fLoGainWindowSize;     // Number of Lo Gain slices in window
  Byte_t  fPeakSearchWindowSize; // Size of FADC window in the search for the highest peak of all pixels.
  Byte_t  fOffsetFromWindow;     // Number of slices to start extraction before search window
  Byte_t  fLoGainPeakShift;      // Shift of the low gain pulse with respect to the high gain pulse, in slices: it is 0 if the low gain is delayed with respect to HG by 15 slices.

  void   FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const;
  void   FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const;  

  void   FindPeak(Byte_t *ptr, Byte_t window, Byte_t &startslice, Int_t &signal, Int_t &sat, Byte_t &satpos) const;

  Bool_t ReInit(MParList *pList);
  Int_t  Process();
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:

  MExtractFixedWindowPeakSearch(const char *name=NULL, const char *title=NULL);
  
  Byte_t GetHiGainWindowSize() const { return fHiGainWindowSize; }
  Byte_t GetLoGainWindowSize() const { return fLoGainWindowSize; }

  void SetRange(Byte_t hifirst=0, Byte_t hilast=0, Byte_t lofirst=0, Byte_t lolast=0);    
  void SetWindows(Byte_t windowh=fgHiGainWindowSize, Byte_t windowl=fgLoGainWindowSize, 
                  Byte_t peaksearchwindow=fgPeakSearchWindowSize);
  void SetOffsetFromWindow(Byte_t offset=fgOffsetFromWindow)  {  fOffsetFromWindow = offset; }
  
  void SetLoGainPeakShift(Byte_t shift=fgLoGainPeakShift) { fLoGainPeakShift = shift; }
  
  void Print(Option_t *o="") const;

  ClassDef(MExtractFixedWindowPeakSearch, 1) // Signal Extractor for fixed size trigger-corrected extraction window
};

#endif
