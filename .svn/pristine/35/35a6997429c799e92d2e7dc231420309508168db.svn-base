#ifndef MARS_MPedPhotCam
#define MARS_MPedPhotCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class TClonesArray;

class MGeomCam;
class MPedPhotPix;
class MBadPixelsCam;
class MPedestalCam;

class MPedPhotCam : public MParContainer, public MCamEvent
{
private:
    TClonesArray *fArray;    // FIXME: Change TClonesArray away from a pointer?
    TClonesArray *fAreas;    //-> Array of MPedPhotPix, one per pixel area
    TClonesArray *fSectors;  //-> Array of MPedPhotPix, one per camera sector

    void InitArrays(const char *name=NULL, const char *title=NULL);

    //  void InitSize(const UInt_t i);
    void InitAreas(const UInt_t i);
    void InitSectors(const UInt_t i);

public:
    MPedPhotCam(const char *name=NULL, const char *title=NULL);
    MPedPhotCam(const MPedestalCam &pcam);
    ~MPedPhotCam();

    void Clear(Option_t *o="");

    void Init(const MGeomCam &geom);
    void InitSize(const UInt_t i); // HB
    Int_t GetSize() const;

    MPedPhotPix &operator[](Int_t i);
    const MPedPhotPix &operator[](Int_t i) const;

          MPedPhotPix &GetArea(UInt_t i);
    const MPedPhotPix &GetArea(UInt_t i) const;

    Int_t GetNumAreas() const;

          MPedPhotPix &GetSector(UInt_t i);
    const MPedPhotPix &GetSector(UInt_t i) const;

    Int_t GetNumSectors() const;

    void Print(Option_t *o="") const;

    void ReCalc(const MGeomCam &geom, MBadPixelsCam *bad=NULL);

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MPedPhotCam, 2)	// Storage Container for all pedestal information of the camera (in units of photons)
};

#endif

