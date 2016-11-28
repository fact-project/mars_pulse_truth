#ifndef MARS_MCamEvent
#define MARS_MCamEvent

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MGeomCam;

class MCamEvent
{
public:
    virtual ~MCamEvent() { }

    virtual Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const = 0;
    virtual void   DrawPixelContent(Int_t num) const = 0;

    virtual void   Init(const MGeomCam &geom);
    virtual void   InitSize(const UInt_t) { } // Used by MGeomApply see Init()

    virtual Double_t GetCameraMean(const MGeomCam &cam, Int_t type=0) const;
    virtual Double_t GetCameraMedian(const MGeomCam &cam, Int_t type=0) const;
    virtual Double_t GetCameraRMS(const MGeomCam &cam, Int_t type=0) const;
    virtual TArrayD  GetCameraStat(const MGeomCam &cam, Int_t type=0) const;

    ClassDef(MCamEvent, 0) // A camera event
};

#endif

