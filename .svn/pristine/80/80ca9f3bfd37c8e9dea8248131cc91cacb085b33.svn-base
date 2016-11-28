#ifndef MARS_MHexagonFreqSpace
#define MARS_MHexagonFreqSpace

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef ROOT_MArrayD
#include "MArrayD.h"
#endif

class MGeomCam;

class MHexagonFreqSpace : public MParContainer, public MCamEvent
{
private:
    MArrayD fDataRe;  //
    MArrayD fDataIm;  //

public:
    MHexagonFreqSpace(const char *name=NULL, const char *title=NULL);
    ~MHexagonFreqSpace() { }

    void Set(const MArrayD &re, const MArrayD &im);

    Double_t GetAbs(UInt_t idx) const;

    static MGeomCam *NewGeomCam(UShort_t num);
    MGeomCam *NewGeomCam() const { return NewGeomCam(fDataRe.GetSize()); }

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MHexagonFreqSpace, 1) // Generalized storage class for camera data in hexagonal frequency space (triangle)
};

#endif
