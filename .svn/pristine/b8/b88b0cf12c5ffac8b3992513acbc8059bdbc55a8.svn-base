#ifndef MARS_MCalibrationCam
#define MARS_MCalibrationCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif
#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class TOrdCollection;

class MCalibrationPix;
class MBadPixelsPix;
class MBadPixelsCam;
class MGeomCam;

class MCalibrationCam : public MParContainer, public MCamEvent
{
public:

  enum PulserColor_t { 
	kCT1  =0, 
	kGREEN=1, 
	kBLUE =2, 
	kUV   =3, 
	kNONE =4 
	};                                  //! Possible Pulser colours
  static const Int_t gkNumPulserColors;     //! Number of Pulser colours (now set to: 4)
  
protected:

  TArrayI fNumUnsuitable;            // Number uncalibrated Pixels per area index 
  TArrayI fNumUnreliable;            // Number unreliable   Pixels per area index 

  TArrayF fNumHiGainFADCSlices;      // Number High-Gain FADC slices used by extractor      
  TArrayF fNumLoGainFADCSlices;      // Number Low -Gain FADC slices used by extractor
  
  Int_t          fRunNumber;         // Store run number for interlaced calib. events
  PulserColor_t  fPulserColor;        // Colour of the pulsed LEDs

  TOrdCollection *fPixels;            //-> Array of MCalibrationPix, one per pixel
  TOrdCollection *fAverageAreas;      // Array of MCalibrationPix, one per pixel area
  TOrdCollection *fAverageSectors;    // Array of MCalibrationPix, one per camera sector
  TOrdCollection *fAverageBadAreas;   // Array of MBadPixelsPix,   one per pixel area
  TOrdCollection *fAverageBadSectors; // Array of MBadPixelsPix,   one per camera sector

  virtual void Add(const UInt_t a, const UInt_t b);
  virtual void AddArea(const UInt_t a, const UInt_t b);
  virtual void AddSector(const UInt_t a, const UInt_t b);
  
  void  InitAverageSectors     ( const UInt_t i               );
  virtual void  InitAverageAreas       ( const UInt_t i               );

public:

  MCalibrationCam(const char *name=NULL, const char *title=NULL);
  ~MCalibrationCam();
  
  void Clear           ( Option_t *o="");
  void Copy(TObject& object) const;
  
  void DrawPixelContent( Int_t num) const;

  // Getters
        Int_t            GetAverageAreas        ()                     const;
        MCalibrationPix &GetAverageArea         ( const UInt_t i     );
  const MCalibrationPix &GetAverageArea         ( const UInt_t i     ) const;
        MBadPixelsPix   &GetAverageBadArea      ( const UInt_t i     );
  const MBadPixelsPix   &GetAverageBadArea      ( const UInt_t i     ) const;
        Int_t            GetAverageSectors      ()                     const;
        MCalibrationPix &GetAverageSector       ( const UInt_t i     );
  const MCalibrationPix &GetAverageSector       ( const UInt_t i     ) const;  
        MBadPixelsPix   &GetAverageBadSector    ( const UInt_t i     );
  const MBadPixelsPix   &GetAverageBadSector    ( const UInt_t i     ) const;
        Float_t          GetNumHiGainFADCSlices ( const Int_t aidx=0 ) const;
        Float_t          GetNumLoGainFADCSlices ( const Int_t aidx=0 ) const;
        Int_t            GetNumUnsuitable       ( const Int_t aidx=-1) const;
        Int_t            GetNumUnreliable       ( const Int_t aidx=-1) const;

        Bool_t           GetPixelContent        ( Double_t &val, Int_t idx, const MGeomCam &cam, 
	                                          Int_t type=0       ) const;
        PulserColor_t    GetPulserColor         ()                     const { return fPulserColor; }
        Int_t            GetRunNumer            ()                     const { return fRunNumber;   }
        Int_t            GetSize                ()                     const;

        MCalibrationPix &operator[]             ( UInt_t i );
  const MCalibrationPix &operator[]             ( UInt_t i )           const;

  static TString GetPulserColorStr(PulserColor_t col);

  // Inits
  void  Init                   ( const MGeomCam &geom         );
  void  InitSize               ( const UInt_t i               );

  // Setters
  void  SetNumHiGainFADCSlices ( const Float_t f, const Int_t aidx=0 );
  void  SetNumLoGainFADCSlices ( const Float_t f, const Int_t aidx=0 );
  void  SetNumUnsuitable       ( const UInt_t i,  const Int_t aidx   );  
  void  SetNumUnreliable       ( const UInt_t i,  const Int_t aidx   ); 
  virtual void  SetPulserColor ( const PulserColor_t col=kCT1 )  { fPulserColor = col; }
  void  SetRunNumber           ( const Int_t run              )  { fRunNumber   = run; }
   
  ClassDef(MCalibrationCam, 6)	// Base class Container for Calibration Results Camera
};

#endif
