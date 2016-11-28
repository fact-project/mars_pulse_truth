#ifndef MARS_MSimTrigger
#define MARS_MSimTrigger

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MLut
#include "MLut.h"
#endif

#include <fstream>

class MParList;
class MParameterD;
class MAnalogChannels;
class MRawEvtHeader;
class MRawRunHeader;
class MPedestalCam;

class MSimTrigger : public MTask
{
private:
    MAnalogChannels *fCamera;           //! The analog input channels
    MParameterD     *fPulsePos;         //! The intended pulse positon
    MParameterD     *fTrigger;          //! The trigger position w.r.t. the analog channels
    MRawRunHeader   *fRunHeader;        //! The run header storing infos about the digitization
    MRawEvtHeader   *fEvtHeader;        //! The event header storing the trigger information
    MPedestalCam    *fElectronicNoise;  //! Electronic noise (for baseline correction)
    MPedestalCam    *fGain;             //! Gain of the pulses

    MLut fRouteAC;                      // Combinination map for the AC channels
    MLut fCoincidenceMap;               // channels for which digital coincidence is checked

    TString fNameRouteAC;               // Name for the AC routing
    TString fNameCoincidenceMap;        // Name for the coincidence mape

    Float_t fDiscriminatorThreshold;    // Discriminator threshold
    Float_t fDigitalSignalLength;       // Length of the output of the discriminator
    Float_t fCoincidenceTime;           // Minimum coincidence time (gate) [slices]

    Bool_t  fShiftBaseline;             // Shift the baseline back to 0 for the threshold (needs ElectronicNoise [MPedestalCam])
    Bool_t  fUngainSignal;              // "Remove" the gain from the signal (needs Gain [MPedestalCam])
    Bool_t  fSimulateElectronics;       // If the electronics is not simulated the trigger is set artificially to the first photon arrived

    Int_t   fMinMultiplicity;           // N out of M

    Int_t   fCableDelay;                // signal runtime on the clipping cable in slices, e.g. 105cm cable = 2*105cm
                                        // (20cm/ns) = 10.5ns = 21slices @ 2GHz [slices]

    Float_t fCableDamping;              // the signal is damped a bit when reflecting at the end of the cable and is inverted as well.
                                        // Damping factor in [-1..0]. In short tests by Kai Schennetten it looked like -0.96.

    // debugging
    std::ofstream patch_file;
    std::ofstream clipped_file;
    std::ofstream digital_file;
    std::ofstream ratescan_file;


    // MSimTrigger
    TObjArray *CalcCoincidence(const TObjArray &arr1, const TObjArray &arr2/*, Float_t gate=0*/) const;
    TObjArray *CalcMinMultiplicity(const MArrayI &idx, const TObjArray &ttls, Int_t threshold) const;
    TObjArray *CalcCoincidences(const MArrayI &idx, const TObjArray &ttls) const;
    void SetTrigger(Double_t pos, Int_t idx);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MSimTrigger(const char *name=NULL, const char *title=NULL);

    void SetNameRouteAC(const char *name) { fNameRouteAC=name; }
    void SetNameCoincidenceMap(const char *name) { fNameCoincidenceMap=name; }

    void SetDiscriminatorThreshold(Float_t th) { fDiscriminatorThreshold=th; }
    void SetDigitalSignalLength(Float_t ln) { fDigitalSignalLength=ln; }
    void SetCoincidenceTime(Float_t tm) { fCoincidenceTime=tm; }

    ClassDef(MSimTrigger, 0) // Task to simulate trigger electronics
};

#endif
