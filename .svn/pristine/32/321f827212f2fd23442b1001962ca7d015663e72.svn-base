#ifndef MARS_MRflEvtHeader
#define MARS_MRflEvtHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

class MRflEvtHeader : public MParContainer
{
    Int_t fEvtNumber;

    Float_t fEnergy;         // [GeV] of primary

    TVector3 fMomentum;      // versor of momentum of primary
    TVector2 fCorePosition;  // shower core position on ground

    Float_t fHeightFirstInt; // [cm] z coordinate (height) of first interaction

    Float_t fPhi;            // Phi of the telescope
    Float_t fTheta;          // Theta of the telescope [rad]

    Float_t fNmax;           // number of particles in shower maximum fitted by CORSIKA
    Float_t fT0;             // depth of first interaction point fitted by CORSIKA
    Float_t fTmax;           // depth of the shower maximum fitted by CORSIKA
    Float_t fChi2;           // quality of the fit to longit shower development

    /* Now follow the fraction of photons reaching the camera produced by  *
     * electrons, muons and other particles respectively:                  */
    Float_t fEFraction;      // elec_cph_fraction
    Float_t fMFraction;      // muon_cph_fraction
    Float_t fOFraction;      // other_cph_fraction

public:
    MRflEvtHeader(const char *name=NULL, const char *title=NULL);

    // Getter
    Int_t GetEvtNumber() const { return fEvtNumber; }

    Float_t GetEnergy() const { return fEnergy; }

    const TVector3 &GetMomentum() const { return fMomentum; }
    const TVector2 &GetCorePosition() const { return fCorePosition; }

    Float_t GetHeightFirstInt() const { return fHeightFirstInt; }

    Float_t GetPhi() const   { return fPhi; }
    Float_t GetTheta() const { return fTheta; }

    Float_t GetNmax() const  { return fNmax; }
    Float_t GetT0() const    { return fT0; }
    Float_t GetTmax() const  { return fTmax; }
    Float_t GetChi2() const  { return fChi2; }

    Float_t GetEFraction() const { return fEFraction; }
    Float_t GetMFraction() const { return fMFraction; }
    Float_t GetOFraction() const { return fOFraction; }

    // Setter
    void SetEvtNumber(Int_t n) { fEvtNumber = n; }
    void SetEnergy(Float_t x) { fEnergy = x; }
    void SetMomentum(const TVector3 &v) { fMomentum = v; }
    void SetCorePosition(const TVector2 &v) { fCorePosition = v; }

    void SetMomentum(Float_t px, Float_t py, Float_t pz) { fMomentum = TVector3(px, py, pz); }
    void SetCorePosition(Float_t cx, Float_t cy) { fCorePosition = TVector2(cx, cy); }
    void SetHeightFirstInt(Float_t x) { fHeightFirstInt = x; }

    void SetPhi(Float_t x)   { fPhi = x; }
    void SetTheta(Float_t x) { fTheta = x; }

    void SetNmax(Float_t x)  { fNmax = x ; }
    void SetT0(Float_t x)    { fT0 = x; }
    void SetTmax(Float_t x)  { fTmax = x ; }
    void SetChi2(Float_t x)  { fChi2 = x ; }

    void SetEFraction(Float_t x) { fEFraction = x ; }
    void SetMFraction(Float_t x) { fMFraction = x ; }
    void SetOFraction(Float_t x) { fOFraction = x ; }

    // TObject
    void Print(Option_t *o="") const;

    ClassDef(MRflEvtHeader, 1) // Header of an event from the reflector program
};

#endif
