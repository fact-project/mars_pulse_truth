#ifndef MARS_MReportStarguider
#define MARS_MReportStarguider

#ifndef MARS_MReport
#include "MReport.h"
#endif

#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

class MReportStarguider : public MReport
{
public:
    enum { kError=0, kStandby=2, kMonitoring=4 };

private:
    Double_t fDevAz;              // [arcmin] azimuth mispointing
    Double_t fDevZd;              // [arcmin] zenith  mispointing

    Double_t fNominalZd;          // [deg] Nominal zenith distance
    Double_t fNominalAz;          // [deg] Nominal azimuth

    Float_t  fCameraCenterX;      // [CCD pix] PMT Camera center found
    Float_t  fCameraCenterY;      // [CCD pix] PMT Camera center found

    UInt_t   fNumIdentifiedStars; // Number of stars identified by starguider algorithm
    UInt_t   fNumCorrelatedStars; // Number of correlated stars identified by starguider algorithm

    Double_t fSkyBrightness;      // [au] Sky Brightness as calcualted from the CCD image
    Double_t fMjd;                // Modified Julian Date matching the nominal position

    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportStarguider();

    void Clear(Option_t *o="");

    Double_t GetDevAz() const { return fDevAz; }
    Double_t GetDevZd() const { return fDevZd; }
    Double_t GetDevAbs() const;
    TVector2 GetDev() const { return TVector2(fDevZd, fDevAz); }

    Double_t GetNominalZd() const { return fNominalZd; }
    Double_t GetNominalAz() const { return fNominalAz; }
    TVector2 GetNominalPos() const { return TVector2(fNominalZd, fNominalAz); }

    Float_t  GetCameraCenterX() const { return fCameraCenterX; }
    Float_t  GetCameraCenterY() const { return fCameraCenterY; }
    TVector2 GetCameraCenter() const { return TVector2(fCameraCenterX, fCameraCenterY); }

    UInt_t   GetNumIdentifiedStars() const { return fNumIdentifiedStars; }
    UInt_t   GetNumCorrelatedStars() const { return fNumCorrelatedStars; }

    Double_t GetSkyBrightness() const { return fSkyBrightness; }
    Double_t GetMjd() const { return fMjd; }

    Bool_t IsMonitoring() const { return GetState()==kMonitoring; }

    void Print(Option_t *o="") const;

    ClassDef(MReportStarguider, 3) // Class for STARG-REPORT information
};

#endif
