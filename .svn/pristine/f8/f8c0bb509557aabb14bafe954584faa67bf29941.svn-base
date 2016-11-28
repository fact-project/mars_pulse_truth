#ifndef MARS_MMuonCalibPar
#define MARS_MMuonCalibPar

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MSignalCam;
class MMuonSearchPar;

class MMuonCalibPar : public MParContainer
{
private:
//    Float_t fArcLength;     // An arc length of a muon along the arc [deg.]
    Float_t fArcPhi;        // A opening angle of a muon arc [deg.]
    Float_t fArcWidth;      // A width of a muon [deg.] (1 sigma of gaussian fit)
    Float_t fChiArcPhi;     // A chisquare value of the cosine fit for arc phi
    Float_t fChiArcWidth;   // A chisquare value of the cosine fit for arc wid
    Float_t fMuonSize;      // A SIZE of muon which is defined as a SIZE around the estimated circle
//    Float_t fEstImpact;     // An estimated impact parameter from the photon distribution along the arc image
    //Bool_t  fUseUnmap;      // This is a flag to know the Unmapped pixels are used. Refer to the class of MImgCleanStd
    Float_t fPeakPhi;       // The angle which indicates the peak position in the estimated circle

    Float_t fRelTimeMean;      // [ns] Result of the gaus fit to the arrival time
    Float_t fRelTimeSigma;     // [ns] Result of the gaus fit to the arrival time

public:
    MMuonCalibPar(const char *name=NULL, const char *title=NULL);
    //~MMuonCalibPar();

    void Reset();

//    Float_t GetArcLength()      const { return fArcLength; }
    Float_t GetArcPhi()         const { return fArcPhi; }
    Float_t GetArcWidth()       const { return fArcWidth; }
    Float_t GetChiArcPhi()      const { return fChiArcPhi; }
    Float_t GetChiArcWidth()    const { return fChiArcWidth; }
    Float_t GetMuonSize()       const { return fMuonSize; }
//    Float_t GetEstImpact()      const { return fEstImpact; }
    //Bool_t  IsUseUnmap()        const { return fUseUnmap; }
    Float_t GetPeakPhi()        const { return fPeakPhi; }
    Float_t GetRelTimeMean()    const { return fRelTimeMean; }
    Float_t GetRelTimeSigma()   const { return fRelTimeSigma; }

//    void    SetArcLength(Float_t len)       { fArcLength = len; }
    void    SetArcPhi(Float_t phi)          { fArcPhi = phi; }
    void    SetArcWidth(Float_t wid)        { fArcWidth = wid; }
    void    SetChiArcPhi(Float_t chi)       { fChiArcPhi = chi; }
    void    SetChiArcWidth(Float_t chi)     { fChiArcWidth = chi; }
    void    SetMuonSize(Float_t size)       { fMuonSize = size; }
//    void    SetEstImpact(Float_t impact)    { fEstImpact = impact; }
    //void    SetUseUnmap()                   { fUseUnmap = kTRUE; }
    void    SetPeakPhi(Float_t phi)         { fPeakPhi = phi; }
    void    SetTime(Float_t mean, Float_t sigma) { fRelTimeMean=mean; fRelTimeSigma=sigma; }

    void    Print(Option_t *opt=NULL) const;

    ClassDef(MMuonCalibPar, 2) // Container to hold muon calibration parameters
};
    
#endif
