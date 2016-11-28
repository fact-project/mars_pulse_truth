#ifndef MARS_MCalibrationIntensityConstCam
#define MARS_MCalibrationIntensityConstCam

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCalibConstCam
#include "MCalibConstCam.h"
#endif

#ifndef MARS_MGeomCamMagic
#include "MGeomCamMagic.h"
#endif

class MCalibConstPix;
class MBadPixelsCam;
class TOrdCollection;
class TGraph;
class TGraphErrors;

class MCalibrationIntensityConstCam : public MParContainer, public MCamEvent
{
private:

  TOrdCollection *fCams;      // Array of MCalibConstCams, one per pulse colour and intensity
  MBadPixelsCam  *fBadPixels; //! Pointer to current MBadPixelsCam

  void Add(const UInt_t from, const UInt_t to);
  void InitSize( const UInt_t n );

public:

  MCalibrationIntensityConstCam(const char *name=NULL, const char *title=NULL);
  ~MCalibrationIntensityConstCam();

  void  Clear ( Option_t *o="" );
  
  void  AddToList( const char* name, const MGeomCam &geom);

  const Int_t GetSize() const;

        MCalibConstCam *GetCam              ( Int_t i=-1);
  const MCalibConstCam *GetCam              ( Int_t i=-1) const;

        MCalibConstPix &operator[]          ( UInt_t i );
  const MCalibConstPix &operator[]          ( UInt_t i )  const;

  // Inits
  void  Init   ( const MGeomCam &geom );
  
   // Prints
  void   Print(Option_t *o="")         const;

  // Setters
  void SetBadPixels( MBadPixelsCam *b )  { fBadPixels = b; }
  
  // MCamEvent
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent( Int_t num) const { return GetCam()->DrawPixelContent(num); }

  // Posterior displays
  TGraphErrors *GetConvFactorPerAreaVsTime( const Int_t aidx=0, const MGeomCam &geom=MGeomCamMagic());  
  TGraph       *GetConvFactorVsTime      ( const Int_t pixid );

  void DrawConvFactorPerAreaVsTime( const Int_t aidx=-1);          // *MENU*
  void DrawConvFactorVsTime    ( const Int_t idx );          // *MENU*
  
  ClassDef(MCalibrationIntensityConstCam, 1) // Container Calibration Constants Results Camera
};

#endif
