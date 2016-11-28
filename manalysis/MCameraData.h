#ifndef MARS_MCameraData
#define MARS_MCameraData

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif
#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class MGeomCam;
class MSignalCam;
class MPedPhotCam;

class MCameraData : public MParContainer, public MCamEvent
{
private:
    TArrayD fData;  //
    TArrayC fValidity;

public:
    MCameraData(const char *name=NULL, const char *title=NULL);
    ~MCameraData() { }

    UInt_t GetNumPixels() const { return fData.GetSize(); }

    void CalcCleaningLevel(const MSignalCam &evt, const MPedPhotCam &fCam,
                           const MGeomCam &geom);
    void CalcCleaningLevel(const MSignalCam &evt, Double_t noise,
                           const MGeomCam &geom);
    void CalcCleaningLevel2(const MSignalCam &evt, const MPedPhotCam &fCam,
                            const MGeomCam &geom);
    void CalcCleaningLevelDemocratic(const MSignalCam &evt, const MPedPhotCam &cam,
                                     const MGeomCam &geom);
    void CalcCleaningProbability(const MSignalCam &evt, const MPedPhotCam &pcam,
                                 const MGeomCam &geom);
    void CalcCleaningAbsolute(const MSignalCam &evt, const MGeomCam &geom);
    void CalcCleaningArrivalTime(const MSignalCam &evt, const MGeomCam &geom);

    const TArrayD &GetData() const { return fData; }
    const TArrayC &GetValidity() const  { return fValidity; }

    Double_t operator[](int i) { return fData[i]; }

    void Print(Option_t *o="") const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MCameraData, 1) // Generalized storage class for camera data
};

#endif
