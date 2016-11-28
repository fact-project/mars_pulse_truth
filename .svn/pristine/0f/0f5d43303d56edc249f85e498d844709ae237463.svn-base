#ifndef MARS_MCalibConstCam
#define MARS_MCalibConstCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

class MGeomCam;
class MCalibConstPix;
class MBadPixelsCam;

class MCalibConstCam : public MParContainer, public MCamEvent
{
private:

  TClonesArray *fArray;           //-> Array of MCalibConstPix, one per pixel
  TClonesArray *fAverageAreas;    //-> Array of MCalibConstPix, one per pixel area
  TClonesArray *fAverageSectors;  //-> Array of MCalibConstPix, one per camera sector

  //Int_t fRunNumber;               // Run number

  MBadPixelsCam *fBadPixels;      //!

public:

  MCalibConstCam(const char *name=NULL, const char *title=NULL);
  ~MCalibConstCam();
  
  void Clear(Option_t *o="");
  void Copy(TObject &object) const;
  
  // Getters 
        MCalibConstPix &GetAverageArea     ( UInt_t i );
  const MCalibConstPix &GetAverageArea     ( UInt_t i )  const;
  const Int_t           GetNumAverageArea  ()            const;
        MCalibConstPix &GetAverageSector   ( UInt_t i );
  const MCalibConstPix &GetAverageSector   ( UInt_t i )  const;
  const Int_t           GetNumAverageSector()            const;
  Int_t                 GetSize            ()            const;

        MCalibConstPix &operator[]         ( Int_t i  );
  const MCalibConstPix &operator[]         ( Int_t i  )  const;

  void  Init                           ( const MGeomCam &geom);
  void  InitSize                       ( const UInt_t i ) { fArray->ExpandCreate(i);          }
  void  InitAverageAreas               ( const UInt_t i ) { fAverageAreas->ExpandCreate(i);   }
  void  InitAverageSectors             ( const UInt_t i ) { fAverageSectors->ExpandCreate(i); }

  void Print(Option_t *o="") const;

  // Setters
  //void SetRunNumber(const Int_t n)    { fRunNumber = n; }
  void SetBadPixels(MBadPixelsCam *b) { fBadPixels = b; }
  
  Bool_t GetPixelContent (Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void   DrawPixelContent(Int_t idx) const;

  ClassDef(MCalibConstCam, 2)	// Temporary Storage for calibration constants
};

#endif
