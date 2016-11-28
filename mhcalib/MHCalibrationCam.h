#ifndef MARS_MHCalibrationCam
#define MARS_MHCalibrationCam

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif
#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif
#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class TText;
class TOrdCollection;

class MHCalibrationPix;
class MGeomCam;
class MRawRunHeader;
class MCalibrationCam;
class MCalibrationPix;
class MBadPixelsCam;
class MBadPixelsPix;

class MHCalibrationCam : public MH, public MCamEvent
{
  
private:
  static const Double_t fgLowerFitLimitHiGain; //! Default for fLowerFitLimitHiGain
  static const Double_t fgUpperFitLimitHiGain; //! Default for fUpperFitLimitHiGain
  static const Double_t fgLowerFitLimitLoGain; //! Default for fLowerFitLimitLoGain
  static const Double_t fgUpperFitLimitLoGain; //! Default for fUpperFitLimitLoGain

  static const Int_t   fgPulserFrequency;  //! Default for fPulserFrequency
  static const Float_t fgProbLimit;        //! Default for fProbLimit
  static const Float_t fgOverflowLimit;    //! Default for fOverflowLimit
  static const Int_t   fgMaxNumEvts;       //! Default for fMaxNumEvts
  
  static const TString gsHistName;         //! Default Histogram names
  static const TString gsHistTitle;        //! Default Histogram titles
  static const TString gsHistXTitle;       //! Default Histogram x-axis titles
  static const TString gsHistYTitle;       //! Default Histogram y-axis titles
  
protected:

  Int_t   fNbins;                         // Number of bins        
  Axis_t  fFirst;                         // Lower histogram limit 
  Axis_t  fLast;                          // Upper histogram limit 
 
  Float_t fProbLimit;                     // Limit for acceptance of probability of Gauss-Fit
  Float_t fOverflowLimit;                 // Part of maximum allowed overflow events

  Double_t fLowerFitLimitHiGain;          // Lower limit for the fit range for the hi-gain hist
  Double_t fUpperFitLimitHiGain;          // Upper limit for the fit range for the hi-gain hist
  Double_t fLowerFitLimitLoGain;          // Lower limit for the fit range for the lo-gain hist
  Double_t fUpperFitLimitLoGain;          // Upper limit for the fit range for the lo-gain hist

  Bool_t   fIsHiGainFitRanges;            // Are high-gain fit ranges defined?
  Bool_t   fIsLoGainFitRanges;            // Are low-gain fit ranges defined?
  
  TString fHistName;                      //! Histogram names
  TString fHistTitle;                     //! Histogram titles
  TString fHistXTitle;                    //! Histogram x-axis titles
  TString fHistYTitle;                    //! Histogram y-axis titles
  
  Float_t fNumHiGainSaturationLimit;      // Rel. amount sat. higain FADC slices until pixel is called saturated 
  Float_t fNumLoGainSaturationLimit;      // Rel. amount sat. logain FADC slices until pixel is called saturated

  Int_t   fMaxNumEvts;                    // Max Number of events
  Int_t   fCurrentNumEvts;                //! Current number of events
  
  MArrayI fRunNumbers;                    // Numbers of runs used

  MArrayF fAverageAreaRelSigma;           // Re-normalized relative sigmas in average pixels per area
  MArrayF fAverageAreaRelSigmaVar;        // Variance Re-normalized relative sigmas in average pixels per area 
  MArrayI fAverageAreaSat;                // Number of saturated slices in average pixels per area
  MArrayF fAverageAreaSigma;              // Re-normalized sigmas in average pixels per area
  MArrayF fAverageAreaSigmaVar;           // Variance Re-normalized sigmas in average pixels per area
  MArrayI fAverageAreaNum;                // Number of pixels in average pixels per area
  MArrayI fAverageSectorNum;              // Number of pixels in average pixels per sector

  TOrdCollection *fAverageHiGainAreas;    // Array of calibration pixels, one per pixel area
  TOrdCollection *fAverageHiGainSectors;  // Array of calibration pixels, one per camera sector
  TOrdCollection *fAverageLoGainAreas;    // Array of calibration pixels, one per pixel area
  TOrdCollection *fAverageLoGainSectors;  // Array of calibration pixels, one per camera sector

  MCalibrationCam::PulserColor_t fColor;  // Colour of the pulsed LEDs

  MBadPixelsCam    *fBadPixels;           //! Bad Pixels storage container
  MCalibrationCam  *fCam;                 //! Calibration Cam with the results
  MGeomCam         *fGeom;                //! Camera geometry
  MRawRunHeader    *fRunHeader;           //! Run Header
  
  TOrdCollection *fHiGainArray;           // Array of calibration pixels, one per pixel
  TOrdCollection *fLoGainArray;           // Array of calibration pixels, one per pixel

  Int_t  fPulserFrequency;                // Light pulser frequency

  enum {
      kDebug,
      kLoGain,
      kAverageing,
      kOscillations,
      kSizeCheck,
      kIsReset
  };     // Possible global flags
   
  Byte_t  fFlags;                         // Bit-field to hold the global flags
  
  virtual Bool_t SetupHists ( const MParList *pList ) { return kTRUE; }
  virtual Bool_t ReInitHists(       MParList *pList ) { return kTRUE; }
  virtual Bool_t FillHists  ( const MParContainer *par, const Stat_t w=1) { return kTRUE; }
  virtual Bool_t FinalizeHists()                      { return kTRUE;  }
  
  virtual void   FinalizeBadPixels() { }
  
  virtual void   CalcAverageSigma();
  
  virtual void   InitHiGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );
  virtual void   InitLoGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );
  
  virtual void   ResetHistTitles();
  
  void DrawAverageSigma( Bool_t sat, Bool_t inner,
                         Float_t sigma, Float_t sigmaerr,
                         Float_t relsigma, Float_t relsigmaerr) const; 
  
  void FitHiGainArrays ( MCalibrationCam &calcam, MBadPixelsCam &badcam,
                         MBadPixelsPix::UncalibratedType_t fittyp,
                         MBadPixelsPix::UncalibratedType_t osctyp);
  
  void FitHiGainHists  ( MHCalibrationPix &hist, 
                         MCalibrationPix &pix, 
                         MBadPixelsPix &bad, 
                         MBadPixelsPix::UncalibratedType_t fittyp,
                         MBadPixelsPix::UncalibratedType_t osctyp);
  
  void FitLoGainArrays ( MCalibrationCam &calcam, MBadPixelsCam &badcam,
                         MBadPixelsPix::UncalibratedType_t fittyp,
                         MBadPixelsPix::UncalibratedType_t osctyp);
  
  void FitLoGainHists  ( MHCalibrationPix &hist, 
                         MCalibrationPix &pix, 
                         MBadPixelsPix &bad, 
                         MBadPixelsPix::UncalibratedType_t fittyp,
                         MBadPixelsPix::UncalibratedType_t osctyp);

  void   InitHists     ( MHCalibrationPix &hist, MBadPixelsPix &bad, const Int_t i);
  Bool_t InitCams      ( MParList *plist, const TString name );
  
  Bool_t IsLoGain() const;
  Bool_t IsAverageing  () const  { return TESTBIT(fFlags,kAverageing);   }
  Bool_t IsDebug       () const  { return TESTBIT(fFlags,kDebug);        }  
  Bool_t IsOscillations() const  { return TESTBIT(fFlags,kOscillations); }
  Bool_t IsSizeCheck   () const  { return TESTBIT(fFlags,kSizeCheck);    }
  Bool_t IsReset       () const  { return TESTBIT(fFlags,kIsReset);      }

  void ToggleFlag(Bool_t b, Byte_t flag) { b ? SETBIT(fFlags, flag) : CLRBIT(fFlags,flag); }

  void   Remove  ( TOrdCollection *col );
  
  Int_t  ReadEnv ( const TEnv &env, TString prefix, Bool_t print );

public:

  MHCalibrationCam(const char *name=NULL, const char *title=NULL);
  virtual ~MHCalibrationCam();

  Bool_t SetupFill(const MParList *pList);
  Bool_t ReInit   (      MParList *pList);
  Int_t  Fill     (const MParContainer *par, const Stat_t w=1);
  Bool_t Finalize ( );

  virtual void ResetHists();

  // Draw
  void   Draw(const Option_t *opt);       // *MENU*

  Bool_t GetPixelContent ( Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const { return kTRUE; }
  void   DrawPixelContent( Int_t num )  const  {}

  const MArrayI           &GetAverageAreaNum     ()          const { return fAverageAreaNum; }
  const Int_t              GetAverageAreas       ()          const;	 
        MHCalibrationPix  &GetAverageHiGainArea  (UInt_t i);
  const MHCalibrationPix  &GetAverageHiGainArea  (UInt_t i)  const;
        MHCalibrationPix  &GetAverageLoGainArea  (UInt_t i);
  const MHCalibrationPix  &GetAverageLoGainArea  (UInt_t i)  const;
        MHCalibrationPix  &GetAverageHiGainSector(UInt_t i);
  const MHCalibrationPix  &GetAverageHiGainSector(UInt_t i)  const;
        MHCalibrationPix  &GetAverageLoGainSector(UInt_t i);
  const MHCalibrationPix  &GetAverageLoGainSector(UInt_t i)  const;
  const MArrayI           &GetAverageSectorNum    ()         const { return fAverageSectorNum; }
  const Int_t              GetAverageSectors      ()         const;
  const MCalibrationCam::PulserColor_t GetColor   ()     const  { return fColor;                    }
  const Float_t        GetNumHiGainSaturationLimit()     const  { return fNumHiGainSaturationLimit; }
  const Float_t        GetNumLoGainSaturationLimit()     const  { return fNumLoGainSaturationLimit; }
  const MArrayI       &GetRunNumbers         ()          const { return fRunNumbers;            }
  const Int_t          GetSize               ()          const;

        MHCalibrationPix  &operator[]            (UInt_t i);
  const MHCalibrationPix  &operator[]            (UInt_t i)  const;
        MHCalibrationPix  &operator()            (UInt_t i);
  const MHCalibrationPix  &operator()            (UInt_t i)  const;
 
  void SetColor(const MCalibrationCam::PulserColor_t color) { fColor = color; }

  void SetAverageing(const Bool_t b=kTRUE)   { ToggleFlag(b,kAverageing); }
  void SetDebug(const Bool_t b=kTRUE)        { ToggleFlag(b,kDebug); }
  void SetLoGain(const Bool_t b=kTRUE)       { ToggleFlag(b,kLoGain); }
  void SetOscillations(const Bool_t b=kTRUE) { ToggleFlag(b,kOscillations); }
  void SetSizeCheck(const Bool_t b=kTRUE)    { ToggleFlag(b,kSizeCheck); }
  void SetIsReset(const Bool_t b=kTRUE)      { ToggleFlag(b,kIsReset); }

  void SetHistName  ( const char *name )  { fHistName  = name;  }
  void SetHistTitle ( const char *name )  { fHistTitle = name;  }
  void SetHistXTitle( const char *name )  { fHistXTitle = name; }
  void SetHistYTitle( const char *name )  { fHistYTitle = name; }

  void SetLowerFitLimitHiGain( const Double_t d=fgLowerFitLimitHiGain)  {  fLowerFitLimitHiGain = d;  }
  void SetUpperFitLimitHiGain( const Double_t d=fgUpperFitLimitHiGain)  {  fUpperFitLimitHiGain = d;  }
  void SetLowerFitLimitLoGain( const Double_t d=fgLowerFitLimitLoGain)  {  fLowerFitLimitLoGain = d;  }
  void SetUpperFitLimitLoGain( const Double_t d=fgUpperFitLimitLoGain)  {  fUpperFitLimitLoGain = d;  }
   
  void SetBinning(Int_t n, Axis_t lo, Axis_t up) { fNbins=n; fFirst=lo; fLast=up; }

  void SetMaxNumEvts              ( const Int_t  i=fgMaxNumEvts ) { fMaxNumEvts  = i; }

  void SetProbLimit               ( const Float_t f=fgProbLimit) { fProbLimit = f; }    

  void SetNumLoGainSaturationLimit( const Float_t lim )    { fNumLoGainSaturationLimit  = lim; }
  void SetNumHiGainSaturationLimit( const Float_t lim )    { fNumHiGainSaturationLimit  = lim; }
  void SetOverflowLimit        ( const Float_t f=fgOverflowLimit ) { fOverflowLimit = f; }
  void SetPulserFrequency      ( const Int_t   i=fgPulserFrequency )   { fPulserFrequency  = i; }
  
  ClassDef(MHCalibrationCam, 6)	// Base Histogram class for Calibration Camera
};

#endif
