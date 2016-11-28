#ifndef MARS_MMcEvtBasic
#define MARS_MMcEvtBasic

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MMcEvtBasic : public MParContainer
{
public:
    enum ParticleId_t
    {
        kUNDEFINED  =   -1,
        kGAMMA      =    1,
        kPOSITRON   =    2,
        kELECTRON   =    3,
        kANTIMUON   =    5,
        kMUON       =    6,
        kPI0        =    7,
        kNEUTRON    =   13,
        kPROTON     =   14,
        kHELIUM     =  402,
        kOXYGEN     = 1608,
        kIRON       = 5626,
        kArtificial = 9998,
        kNightSky   = 9999
    };

protected:
  ParticleId_t fPartId;  // Type of particle
  Float_t      fEnergy;  // [GeV] Energy
  Float_t      fImpact;  // [cm] impact parameter

  // Telescope orientation (see TDAS 02-11 regarding the 
  // precise meaning of these angles):
  Float_t      fTelescopePhi;    // [rad]
  Float_t      fTelescopeTheta;  // [rad]

  Float_t      fTheta;           // [rad] Theta angle of event
  Float_t      fPhi;             // [rad] Phi angle of event (see class description)

public:
  MMcEvtBasic();
  MMcEvtBasic(ParticleId_t, Float_t, Float_t, Float_t, Float_t);
  void operator=(const MMcEvtBasic &evt);

  // Getter
  ParticleId_t GetPartId() const { return fPartId; }

  Float_t GetEnergy()  const { return fEnergy; }
  Float_t GetImpact()  const { return fImpact; }

  Float_t GetTelescopePhi() const { return fTelescopePhi; }
  Float_t GetTelescopeTheta() const { return fTelescopeTheta; }

  Float_t GetParticlePhi() const { return fPhi; }
  Float_t GetParticleTheta() const { return fTheta; }

  //Float_t GetTheta() const { return fTheta; }
  //Float_t GetPhi() const { return fPhi ;  }

  static TString GetParticleName(Int_t id);
  static TString GetParticleSymbol(Int_t id);
  static TString GetEnergyStr(Float_t e);

  TString GetParticleSymbol() const
  {
      return GetParticleSymbol(fPartId);
  }

  TString GetParticleName() const
  {
      return GetParticleName(fPartId);
  }

  TString GetEnergyStr() const
  {
      return GetEnergyStr(fEnergy);
  }

  // Setter
  void SetPartId(ParticleId_t id) { fPartId = id; }
  void SetEnergy(Float_t Energy)  { fEnergy=Energy; }              //Set Energy
  void SetImpact(Float_t Impact)  { fImpact=Impact;}               //Set impact parameter

  void SetTelescopeTheta(Float_t Theta) { fTelescopeTheta=Theta; }
  void SetTelescopePhi  (Float_t Phi)   { fTelescopePhi=Phi; }

  void SetParticleTheta(Float_t Theta)  { fTheta=Theta; }
  void SetParticlePhi  (Float_t Phi)    { fPhi=Phi; }

  //void SetTheta(Float_t Theta) { fTheta=Theta; }                //Set Theta angle
  //void SetPhi(Float_t Phi)     { fPhi=Phi;  }                   //Set Phi angle

  void Fill(ParticleId_t, Float_t, Float_t, Float_t, Float_t);

  // TObject
  void Clear(Option_t *opt=NULL);
  void Print(Option_t *opt=NULL) const;

  // MParContainer
  Bool_t SetupFits(fits &fin);

  ClassDef(MMcEvtBasic, 3) //Stores Basic Montecarlo Information of one event

};

#endif
