#ifndef MARS_MSimAPD
#define MARS_MSimAPD

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MGeomCam;
class MParList;
class MPhotonEvent;
class MPhotonStatistics;
class MPedestalCam;
class MParameterD;

class MSimAPD : public MTask
{
private:
    MGeomCam          *fGeom;    //! APD geometry (used to know how many pixels we have)
    MPhotonEvent      *fEvt;     //! Event storing the photon information
    MPhotonStatistics *fStat;    //! Storing event statistics (needed for the start-time)
    MPedestalCam      *fRates;   //! Accidental Photon Rates for all pixels

    MParameterD       *fCrosstalkCoeffParam;

    TObjArray fAPDs;             //! Array keeping the necessary number of APDs

    TString fNameGeomCam;        // Name of the geometry container storing the APD gemeotry

    Double_t fFreq;              // Frequency of random phtons which hit the APDs

    Int_t fType;

    Int_t   fNumCells;           // Number of cells along one side
    Float_t fCrosstalkCoeff;     // Crosstalk coefficient (P form the formula)
    Float_t fDeadTime;           // Single cell dead time in nano-seconds
    Float_t fRecoveryTime;       // Single cell recovery time in nano-seconds
    Float_t fAfterpulseProb1;    // Early afterpulse probability
    Float_t fAfterpulseProb2;    // Late afterpulse probability

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimAPD(const char *name=NULL, const char *title=NULL);

    void SetNameGeomCam(const char *name="MGeomCam") { fNameGeomCam = name; }
    void SetFreq(Float_t f) { fFreq=f; }

    ClassDef(MSimAPD, 0) // Task to simulate the detection behaviour of APDs
};
#endif
