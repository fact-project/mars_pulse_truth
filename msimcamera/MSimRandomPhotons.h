#ifndef MARS_MSimRandomPhotons
#define MARS_MSimRandomPhotons

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MParList;
class MParSpline;
class MPhotonEvent;
class MPhotonStatistics;
class MCorsikaRunHeader;
class MPedestalCam;

class MSimRandomPhotons : public MTask
{
private:
    MGeomCam          *fGeom;    //! container with the geometry
    MPhotonEvent      *fEvt;     //! Event storing the photons
    MPhotonStatistics *fStat;    //! Container storing evenet statistics
//    MCorsikaEvtHeader *fEvtHeader;  //! Header storing event information
    MCorsikaRunHeader *fRunHeader;  //! Header storing run information
    MPedestalCam      *fRates;   // Random count rate per pixel

    // FIXME: Make this a single number per Pixel/APD
    Double_t fFreqFixed; // [1/ns]      A fixed frequency per pixel
    Double_t fFreqNSB;   // [1/ns/cm^2] A frequency depending on area

    Double_t fScale;

    Bool_t fSimulateWavelength;

    TString fNameGeomCam;
    TString fFileNameNSB;

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

public:
    MSimRandomPhotons(const char *name=NULL, const char *title=NULL);

    void SetFreq(Float_t fnsb, Float_t fdc) { fFreqNSB=fnsb; fFreqFixed=fdc; }

    void SetNameGeomCam(const char *name="MGeomCam") { fNameGeomCam = name; }

    ClassDef(MSimRandomPhotons, 0) // Simulate possonian photons (like NSB or dark current)
};

#endif
