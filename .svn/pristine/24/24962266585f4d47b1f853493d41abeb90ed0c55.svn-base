#ifndef MARS_MMovieData
#define MARS_MMovieData

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class TGraph;
class MSpline3;

class MMovieData : public MParContainer, public MCamEvent
{
private:
    TObjArray fSplines;

    Float_t fMax;
    Float_t fMedianPedestalRms;
    UInt_t  fNumSlices;

    //TSpline *operator[](int i) { return (TSpline*)fSplines[i]; }
    MSpline3 &operator[](int i) const { return *(MSpline3*)fSplines[i]; }

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const { return kFALSE; }
    void   DrawPixelContent(Int_t num) const { }

public:
    MMovieData(const char *name=NULL, const char *title=NULL);
    ~MMovieData() { }

    void Reset();
    void Add(const TGraph &g);

    void SetMax(Float_t mx) { fMax = mx; }
    Float_t GetMax() const { return fMax; }

    void SetMedianPedestalRms(Float_t m) { fMedianPedestalRms = m; }
    Float_t GetMedianPedestalRms() const { return fMedianPedestalRms; }

    void SetNumSlices(UInt_t n) { fNumSlices=n; }
    UInt_t GetNumSlices() const { return fNumSlices; }

    Bool_t CheckRange(Int_t idx, Double_t tm) const;
    Double_t Eval(Int_t idx, Double_t tm) const;
    Double_t CheckedEval(Int_t idx, Double_t tm) const { return CheckRange(idx, tm) ? Eval(idx, tm) : 0; }

    ClassDef(MMovieData, 1) // Storage container for movie data
};

#endif
