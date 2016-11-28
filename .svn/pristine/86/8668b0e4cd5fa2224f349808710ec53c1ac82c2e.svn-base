#ifndef MARS_MHCalibrationHiLoCam
#define MARS_MHCalibrationHiLoCam

#ifndef MARS_MHCalibrationCam
#include "MHCalibrationCam.h"
#endif

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

#ifndef ROOT_TH2F
#include <TH2F.h>
#endif

class MGeomCam;
class MArrivalTimeCam;
class MHCalibrationHiLoPix;

class MHCalibrationHiLoCam : public MHCalibrationCam
{
private:
   static const Int_t   fgNbins;                //! Default number of bins        (now set to: 900   )
   static const Axis_t  fgFirst;                //! Default lower histogram limit (now set to: -13.5 )
   static const Axis_t  fgLast;                 //! Default upper histogram limit (now set to:  13.5 )
   static const Float_t fgProbLimit;            //! The default for fProbLimit    (now set to: 0.0)

   static const Int_t   fgHivsLoNbins;          //! Default number of bins        (now set to: 900   )
   static const Axis_t  fgHivsLoFirst;          //! Default lower histogram limit (now set to: -13.5 )
   static const Axis_t  fgHivsLoLast;           //! Default upper histogram limit (now set to:  13.5 )

   static const Axis_t  fgLowerFitLimitProfile; //! Default for fLowerFitLimitProfile    (now set to: 480. )
   static const Axis_t  fgUpperFitLimitProfile; //! Default for fUpperFitLimitProfile    (now set to: 680. )

   static const TString gsHistName;          //! Default Histogram names
   static const TString gsHistTitle;         //! Default Histogram titles
   static const TString gsHistXTitle;        //! Default Histogram x-axis titles
   static const TString gsHistYTitle;        //! Default Histogram y-axis titles

   static const TString gsHivsLoHistName;    //! Default  names hi vs. lo profile
   static const TString gsHivsLoHistTitle;   //! Default  titles hi vs. lo profile
   static const TString gsHivsLoHistXTitle;  //! Default  x-axis titles hi vs. lo profile
   static const TString gsHivsLoHistYTitle;  //! Default  y-axis titles hi vs. lo profile

   TString fHivsLoHistName;                  //  names hi vs. lo profile
   TString fHivsLoHistTitle;                 //  titles hi vs. lo profile
   TString fHivsLoHistXTitle;                //  x-axis titles hi vs. lo profile
   TString fHivsLoHistYTitle;                //  y-axis titles hi vs. lo profile

   Int_t   fHivsLoNbins;                     // Number of HivsLo bins
   Axis_t  fHivsLoFirst;                     // Lower histogram limit low gain
   Axis_t  fHivsLoLast;                      // Upper histogram limit low gain

   Axis_t  fLowerFitLimitProfile;            // Lower limit for hi-vs. logain fit
   Axis_t  fUpperFitLimitProfile;            // Upper limit for hi-vs. logain fit

   MArrayD fSumareahi  ;                     //
   MArrayD fSumsectorhi;                     //
   MArrayI fNumareahi  ;                     //
   MArrayI fNumsectorhi;                     //
   MArrayD fSumarealo  ;                     //
   MArrayD fSumsectorlo;                     //
   MArrayI fNumarealo  ;                     //
   MArrayI fNumsectorlo;                     //

   MArrivalTimeCam *fArrTimes;               //! Pointer to arrival times

   TH2F    fHivsLoResults;                   // Results from pol1-fit

   Float_t fUsedLoGainSlices;                // Used low-gain FADC slices
   Bool_t ReInitHists(MParList *pList);
   Bool_t FillHists(const MParContainer *par, const Stat_t w=1);
   Bool_t FinalizeHists();
   void   FinalizeBadPixels();

   void   InitHiGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );
   void   InitLoGainArrays( const Int_t npix, const Int_t nareas, const Int_t nsectors );

   void   FinalizeHivsLo (MHCalibrationHiLoPix &hist);

   void   CheckOverflow( MHCalibrationPix &pix);

   Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:
  MHCalibrationHiLoCam(const char *name=NULL, const char *title=NULL);

  // Clone
  TObject *Clone(const char *name="") const;

  TH2F &GetHivsLoResults() { return fHivsLoResults; }

  void SetHivsLoHistName   ( const char *name )  { fHivsLoHistName   = name; }
  void SetHivsLoHistTitle  ( const char *name )  { fHivsLoHistTitle  = name; }
  void SetHivsLoHistXTitle ( const char *name )  { fHivsLoHistXTitle = name; }
  void SetHivsLoHistYTitle ( const char *name )  { fHivsLoHistYTitle = name; }

  void SetHivsLoNbins      ( const Int_t  i=fgHivsLoNbins   ) { fHivsLoNbins   = i; }
  void SetHivsLoFirst      ( const Axis_t f=fgHivsLoFirst   ) { fHivsLoFirst   = f; }
  void SetHivsLoLast       ( const Axis_t f=fgHivsLoLast    ) { fHivsLoLast    = f; }

  void SetLowerFitLimitProfile( const Axis_t f=fgLowerFitLimitProfile ) { fLowerFitLimitProfile = f; }
  void SetUpperFitLimitProfile( const Axis_t f=fgUpperFitLimitProfile ) { fUpperFitLimitProfile = f; }

  // Draw
  void   Draw(const Option_t *opt);       // *MENU*

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  ClassDef(MHCalibrationHiLoCam, 1)	// Histogram class for High Gain vs. Low Gain Calibration
};

#endif
