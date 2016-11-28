#ifndef MARS_MSimPointingPos
#define MARS_MSimPointingPos

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MCorsikaEvtHeader;
class MCorsikaRunHeader;
class MPointingPos;

class MSimPointingPos : public MTask
{
private:
    MCorsikaRunHeader *fRunHeader;  //! Header storing event information
    MCorsikaEvtHeader *fEvtHeader;  //! Header storing event information
    MPointingPos      *fPointing;   //! Output storing telescope pointing position in local (telescope) coordinate system
    MPointingPos      *fSimSourcePosition;   //! Output storing simulated source pointing position in local (telescope) coordinate system

    Double_t fOffTargetDistance;    // [rad] Distance of the observed off-target position from the source
    Double_t fOffTargetPhi;         // [rad] Rotation angle of the off-target position (phi==0 means south, phi=90 west) [0;2pi], phi<0 means random

    // MSimPointingPos
    void GetDelta(Double_t &dtheta, Double_t &dphi) const;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimPointingPos(const char *name=NULL, const char *title=NULL);

    // Getter
    Double_t GetOffTargetDistance() const;
    Double_t GetOffTargetPhi() const;

    // Setter
    void SetOffTargetDistance(Double_t d=0);
    void SetOffTargetPhi(Double_t p=-1);

    // MSimPointingPos
    Bool_t IsOffTargetObservation() const { return fOffTargetDistance!=0; }

    // TObject

    ClassDef(MSimPointingPos, 0) // Task to simulate the pointing position (mirror orientation)
};
    
#endif

