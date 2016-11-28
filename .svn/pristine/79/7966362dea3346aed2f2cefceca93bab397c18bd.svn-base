#ifndef MARS_MSimReflector
#define MARS_MSimReflector

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MGeomCam;
class MPointingPos;
class MPhotonEvent;
class MCorsikaEvtHeader;

class MReflector;

class MSimReflector : public MTask
{
private:
    MPhotonEvent     *fEvt;        //! Event  storing the photons
    MPhotonEvent     *fMirror0;    //! Event  storing the photons in the mirror plane (w/o camera shadow)
    MPhotonEvent     *fMirror1;    //! Event  storing the photons in the mirror plane (w/  camera shadow)
    MPhotonEvent     *fMirror2;    //! Event  storing the photons in the mirror plane (w/  camera shadow)
    MPhotonEvent     *fMirror3;    //! Event  storing the photons in the mirror plane (w/  camera shadow)
    MPhotonEvent     *fMirror4;    //! Event  storing the photons in the mirror plane (w/  camera shadow)
    //MCorsikaRunHeader *fRunHeader;     //! Header storing event information
    MCorsikaEvtHeader *fEvtHeader;     //! Header storing event information

    MReflector        *fReflector;  //!
    MGeomCam          *fGeomCam;    //!
    MPointingPos      *fPointing;   //!

    TString fNameGeomCam;        // Name of the geometry container storing the APD gemeotry
    TString fNameReflector;      // Name of the container storing the reflector geometry 

    Double_t fDetectorFrame;     // A disk of radius DetectorFrame around the focal point absorbing photons
    Double_t fDetectorMargin;    // A margin around the detector (MGeomCam::HitCamera) in which photons are also stored

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MSimReflector(const char *name=NULL, const char *title=NULL);

    // MSimReflector
    void SetNameGeomCam(const char *name="MGeomCam")     { fNameGeomCam   = name; }
    void SetNameReflector(const char *name="MReflector") { fNameReflector = name; }

    void SetDetectorFrame(Double_t cm=0)  { fDetectorFrame  = cm; }
    void SetDetectorMargin(Double_t mm=0) { fDetectorMargin = mm; }

    ClassDef(MSimReflector, 0) // Task to calculate reflection on a mirror
};

#endif
