#ifndef MARS_MHSingleMuon
#define MARS_MHSingleMuon

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TProfile
#include <TProfile.h>
#endif

class MSignalCam;
class MMuonSearchPar;
class MMuonCalibPar;
class MMuonSetup;
class MGeomCam;

class MHSingleMuon : public MH
{
private:
    MSignalCam     *fSignalCam;     //!
    MMuonSearchPar *fMuonSearchPar; //!
    MGeomCam       *fGeomCam;       //!

    Double_t fMargin;               //!

    TProfile fHistPhi;    // Histogram of photon distribution along the arc.
    TProfile fHistWidth;  // Histogram of radial photon distribution of the arc.
    TH1F     fHistTime;   // Histogram of arrival time distribution along the arc.

    Double_t fRelTimeMean;   // Result of the gaus fit to the arrival time
    Double_t fRelTimeSigma;  // Result of the gaus fit to the arrival time

    Bool_t FindRangeAboveThreshold(const TProfile &h, Float_t thres, Int_t &first, Int_t &last) const;

public:
    MHSingleMuon(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    Bool_t CalcPhi(Double_t, Double_t &, Double_t &) const;
    Bool_t CalcWidth(Double_t, Double_t &, Double_t &);

    const TProfile &GetHistPhi() const { return fHistPhi; }
    const TH1F     &GetHistTime() const { return fHistTime; }
    const TProfile &GetHistWidth() const { return fHistWidth; }

    Double_t GetRelTimeMean() const  { return fRelTimeMean; }
    Double_t GetRelTimeSigma() const { return fRelTimeSigma; }

    Float_t CalcSize() const;

    void Draw(Option_t *o="");
    void Paint(Option_t *o="");

    ClassDef(MHSingleMuon, 2)
};

#endif
