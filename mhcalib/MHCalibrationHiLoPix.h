#ifndef MARS_MHCalibrationHiLoPix
#define MARS_MHCalibrationHiLoPix

#ifndef MARS_MHCalibrationPix
#include "MHCalibrationPix.h"
#endif

#ifndef ROOT_TProfile
#include <TProfile.h>
#endif

class MHCalibrationHiLoPix : public MHCalibrationPix
{

 private:

  TProfile fHivsLo;        // Profile containing the high-gain vs. low-gain value
                          
  Int_t    fHivsLoNbins;  // Number of  bins used for the fHHivsLo 
  Axis_t   fHivsLoFirst;  // Lower bound bin used for the fHHivsLo
  Axis_t   fHivsLoLast;   // Upper bound bin used for the fHHivsLo

public:

  MHCalibrationHiLoPix(const char *name=NULL, const char *title=NULL);

  void Reset();  
  void InitBins();
  
  // Setters 
  virtual void SetHivsLoNbins(const Int_t  bins )  { fHivsLoNbins = bins;  }
  virtual void SetHivsLoFirst(const Axis_t first)  { fHivsLoFirst = first; }
  virtual void SetHivsLoLast( const Axis_t last )  { fHivsLoLast  = last;  }

  // Getters
  TProfile       *GetHivsLo()            { return &fHivsLo;  }
  const TProfile *GetHivsLo()      const { return &fHivsLo;  }

  const Float_t  GetHivsLoP0()     const;
  const Float_t  GetHivsLoP1()     const;
  const Float_t  GetHivsLoProb()   const;

  // Fill histos
  Bool_t FillHivsLo(const Float_t t, const Float_t w);

  // Draws
  void Draw(Option_t *opt="");

  ClassDef(MHCalibrationHiLoPix, 1)     // Base Histogram class for HiLo Pixel Calibration
};

#endif /* MARS_MHCalibrationHiLoPix */
