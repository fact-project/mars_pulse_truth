#ifndef MARS_MMuonSearchPar
#define MARS_MMuonSearchPar

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MHillas;
class MGeomCam;
class MSignalCam;

class MMuonSearchPar : public MParContainer
{
private:
    Float_t fRadius;    // An estimated radius of the muon ring [mm]
    Float_t fDeviation; // The standard deviation from the estimated ring [mm]
    Float_t fCenterX;   // An estimated center position in X of the muon ring [mm]
    Float_t fCenterY;   // An estimated center position in Y of the muon ring [mm]
    Float_t fTime;      // Mean arrival time of core pixels
    Float_t fTimeRms;   // Rms of arrival time of core pixels

    MArrayF fSignal;    //! Temporary storage for signal
    MArrayF fX;         //! Temporary storage for pixels X-position
    MArrayF fY;         //! Temporary storage for pixels Y-position

    static void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    Double_t Fcn(Double_t *par) const;

public:
    MMuonSearchPar(const char *name=NULL, const char *title=NULL);

    // MParContainer
    void Reset();

    // Getter
    Float_t GetRadius()    const { return fRadius; }
    Float_t GetDeviation() const { return fDeviation; }
    Float_t GetCenterX()   const { return fCenterX; }
    Float_t GetCenterY()   const { return fCenterY; }
    Float_t GetDist() const;
    Float_t GetTime()      const { return fTime; }
    Float_t GetTimeRms()   const { return fTimeRms; }

    // MMuonSearchPar
    void   CalcMinimumDeviation(const MGeomCam &geom, const MSignalCam &evt,
                                Double_t &x, Double_t &y, Double_t &sigma, Double_t &rad);

    void   Calc(const MGeomCam &geom, const MSignalCam &evt,
		const MHillas &hillas);

    // TObject
    void   Paint(Option_t *opt="");
    void   Print(Option_t *opt=NULL) const;
    void   Print(const MGeomCam &geom, Option_t *opt=NULL) const;

    ClassDef(MMuonSearchPar, 2) // Container to hold muon search parameters
};

#endif
