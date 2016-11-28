#ifndef MARS_MPedestalCam
#define MARS_MPedestalCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class TClonesArray;

class MGeomCam;
class MPedestalPix;
class MBadPixelsCam;
class TArrayF;
class MPedestalCam : public MParContainer, public MCamEvent
{
private:

  TClonesArray *fArray;           //-> FIXME: Change TClonesArray away from a pointer?
  TClonesArray *fAverageAreas;    //-> Array of MPedestalPix, one per pixel area
  TClonesArray *fAverageSectors;  //-> Array of MPedestalPix, one per camera sector

  UInt_t fNumSlices;  // Total number of slices
  UInt_t fNumEvents;  // Number of events used for pedestal calculation (be careful, it has no predefined meaning!)

  void PrintArr(const TCollection &list) const;

public:

  MPedestalCam(const char *name=NULL, const char *title=NULL);
  ~MPedestalCam();

  // TObject
  void Clear(Option_t *o="");
  void Copy(TObject &object) const;
  void Print(Option_t *o="") const; //*MENU*

  // Getters 
        MPedestalPix &GetAverageArea   ( UInt_t i );
  const MPedestalPix &GetAverageArea   ( UInt_t i )            const;
        Int_t         GetNumAverageArea()                      const;
        MPedestalPix &GetAverageSector ( UInt_t i );
  const MPedestalPix &GetAverageSector ( UInt_t i )            const;
        Int_t         GetNumAverageSector()                    const;
  //Float_t             GetPedestalMin   ( const MGeomCam *cam ) const;
  //Float_t             GetPedestalMax   ( const MGeomCam *cam ) const;
  Int_t               GetSize          ()                      const;
  ULong_t             GetNumSlices() const { return fNumSlices; }
  UInt_t              GetNumEvents() const { return fNumEvents; }

  TArrayF GetAveragedPedPerArea  ( const MGeomCam &geom, const UInt_t ai=0,  MBadPixelsCam *bad=NULL );
  TArrayF GetAveragedPedPerSector( const MGeomCam &geom, const UInt_t sec=0, MBadPixelsCam *bad=NULL );
  TArrayF GetAveragedRmsPerArea  ( const MGeomCam &geom, const UInt_t ai=0,  MBadPixelsCam *bad=NULL );
  TArrayF GetAveragedRmsPerSector( const MGeomCam &geom, const UInt_t sec=0, MBadPixelsCam *bad=NULL );

        MPedestalPix &operator[]       ( Int_t i             );
  const MPedestalPix &operator[]       ( Int_t i             ) const;

  void  Init                           ( const MGeomCam &geom);
  void  InitSize                       ( const UInt_t i      );
  void  InitAverageAreas               ( const UInt_t i      );
  void  InitAverageSectors             ( const UInt_t i      );

  // Setters
  void SetNumSlices(const ULong_t n) { fNumSlices = n; }
  void SetNumEvents(const UInt_t n)  { fNumEvents = n; }

  // MCamEvent
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t idx) const;

  ClassDef(MPedestalCam, 3)	// Storage Container for all pedestal information of the camera
};

#endif
