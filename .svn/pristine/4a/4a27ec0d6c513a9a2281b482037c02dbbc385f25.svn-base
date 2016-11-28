#ifndef MARS_MFSoftwareTrigger
#define MARS_MFSoftwareTrigger

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcEvt;
class MGeomCam;
class MSignalCam;
class MSignalPix;
class MArrivalTime;

class MFSoftwareTrigger : public MFilter
{
public:
    enum TriggerType_t {
        kSinglePixelNeighbors,
        kAnyPattern/*,
        kMagicLvl1*/
    };

private:
    const MGeomCam     *fCam; // Camera Geometry
    const MSignalCam   *fEvt; // Cerenkov Photon Event
    const MArrivalTime *fTme;

    Float_t     fThreshold;     // nuber of minimum required photons
    Float_t     fTimeWindow;    // Window for time coincidence
    Byte_t      fNumNeighbors;  // number of required neighbours

    Int_t       fCut[2];

    Bool_t      fResult;

    TriggerType_t fType;

    enum {
        kWasChecked     = BIT(21),
        kAboveThreshold = BIT(22),
        kIsCoincident   = BIT(23)
    };

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    void ResetBits(Int_t bits) const;
    const MSignalPix *CheckPixel(Int_t i) const;
    Bool_t SwTrigger() const;
    Bool_t SwCoincidence() const;
    //Int_t CheckCoincidence(Int_t idx, Float_t tm0) const;
    //Bool_t MagicLvl1Trigger() const;
    Int_t  CountPixels(Int_t idx, Float_t tm) const;
    Bool_t ClusterTrigger() const;

    Bool_t IsExpressionTrue() const { return fResult; }

public:
    MFSoftwareTrigger(const char *name=NULL, const char *title=NULL);

    void SetThreshold(Float_t min)          { fThreshold    = min; }
    void SetNumNeighbors(Byte_t num)        { fNumNeighbors = num; }
    void SetTimeWindow(Float_t win=-1)      { fTimeWindow   = win; }
    void SetTriggerType(TriggerType_t type) { fType = type; }

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MFSoftwareTrigger, 0) // Filter for software trigger
};

#endif
