#ifndef MARS_MCorsikaRunHeader
#define MARS_MCorsikaRunHeader
///////////////////////////////////////////////////////////////////////
//                                                                   //
// MRunHeader                                                        //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#ifndef MARS_MTime
#include "MTime.h"
#endif

class MCorsikaFormat;

class MCorsikaRunHeader : public MParContainer
{
    friend class MCorsikaEvtHeader;
public:
    enum CerenkovFlag_t
    {
        kCerenkov   = BIT(0),
        kIact       = BIT(1),
        kCeffic     = BIT(2),
        kAtmext     = BIT(3),
        kRefraction = BIT(4),
        kVolumedet  = BIT(5),
        kCurved     = BIT(6),
        kSlant      = BIT(8)
    };

private:
    static const Double_t fgEarthRadius; // Take same Earth radius as in CORSIKA (cm)

    UInt_t  fRunNumber;               // Run number
    UInt_t  fNumReuse;                // Number of how many times the same shower is used
    UInt_t  fParticleID;              // Particle ID (see MMcEvtBasic or CORSIKA manual)
    UInt_t  fNumEvents;               // Number of events
    MTime   fRunStart;                // Date of begin (yymmdd)
    Float_t fProgramVersion;          // Version of program

    Byte_t  fNumObsLevel;             // Number of observation levels
    Float_t fObsLevel[10];            // Observation levels [cm]

    Float_t fImpactMax;               // [cm] Maximum simulated impact

    Float_t fSlopeSpectrum;           // Slope of energy spectrum
    Float_t fEnergyMin;               // Lower limit of energy range
    Float_t fEnergyMax;               // Upper limit of energy range

    Float_t fZdMin;                   // [rad] Zenith distance
    Float_t fZdMax;                   // [rad] Zenith distance
    Float_t fAzMin;                   // [rad] Azimuth (north=0; east=90)
    Float_t fAzMax;                   // [rad] Azimuth (north=0; east=90) (north denotes the magnet north which is defined to be in the geografic north!)

    Float_t fMagneticFieldX;          // [muT] x-component of earth magnetic field (ceres coordinate system)
    Float_t fMagneticFieldZ;          // [muT] z-component of earth magnetic field (ceres coordinate system)
    Float_t fMagneticFieldAz;         // [rad] Azimuth angle of magnetic north expressed in telescope coordinates

    Float_t fWavelengthMin;           // [nm] Wavelength bandwidth lo edge
    Float_t fWavelengthMax;           // [nm] Wavelength bandwidth up edge

    Float_t fViewConeInnerAngle;      // [deg]
    Float_t fViewConeOuterAngle;      // [deg]

    Float_t fAtmosphericLayers[5];    // [cm]    ATMLAY (see Corsika Manual for details)
    Float_t fAtmosphericCoeffA[5];    // [g/cm²] AATM   (see Corsika Manual for details)
    Float_t fAtmosphericCoeffB[5];    // [g/cm²] BATM   (see Corsika Manual for details)
    Float_t fAtmosphericCoeffC[5];    // [cm]    CATM   (see Corsika Manual for details)

    UInt_t fCerenkovFlag;

public:
    MCorsikaRunHeader(const char *name=NULL, const char *title=NULL);

    // Getter
    UInt_t GetRunNumber() const { return fRunNumber; }
    UInt_t GetParticleID() const { return fParticleID; }
    UInt_t GetNumEvents() const { return fNumEvents; }
    UInt_t GetNumReuse() const { return fNumReuse; }

    const MTime &GetRunStart() const { return fRunStart; }

    Float_t GetProgramVersion() const { return fProgramVersion; }

    Float_t GetZdMin() const { return fZdMin; }
    Float_t GetZdMax() const { return fZdMax; }

    Float_t GetAzMin() const { return fAzMin; }
    Float_t GetAzMax() const { return fAzMax; }

    Float_t GetWavelengthMin() const { return fWavelengthMin; }
    Float_t GetWavelengthMax() const { return fWavelengthMax; }

    Float_t GetSlopeSpectrum() const { return fSlopeSpectrum; }
    Float_t GetEnergyMin() const { return fEnergyMin; }
    Float_t GetEnergyMax() const { return fEnergyMax; }

    Float_t GetImpactMax() const { return fImpactMax; }

    Float_t GetMagneticFieldX() const  { return fMagneticFieldX; }
    Float_t GetMagneticFieldZ() const  { return fMagneticFieldZ; }
    Float_t GetMagneticFieldAz() const { return fMagneticFieldAz; }

    Float_t GetViewConeInnerAngle() const { return fViewConeInnerAngle; }
    Float_t GetViewConeOuterAngle() const { return fViewConeOuterAngle; }
    Bool_t HasViewCone() const { return fViewConeOuterAngle>0; }

    Float_t GetObsLevel(UInt_t i=0) const { return i>9 ? -1 : fObsLevel[i]; }

    Bool_t Has(CerenkovFlag_t opt) const { return fCerenkovFlag&opt ? 1 : 0; }

    static Double_t EarthRadius() { return fgEarthRadius; }

    // Preliminary!
    Bool_t HasLayers() const { return fAtmosphericLayers[4]>0; }

    const Float_t *GetAtmosphericLayers() const { return fAtmosphericLayers; }
    const Float_t *GetAtmosphericCoeffA() const { return fAtmosphericCoeffA; }
    const Float_t *GetAtmosphericCoeffB() const { return fAtmosphericCoeffB; }
    const Float_t *GetAtmosphericCoeffC() const { return fAtmosphericCoeffC; }

    UInt_t GetNumAtmosphericModel() const { return (fCerenkovFlag>>10)&0x3ff; }

    // I/O
    Bool_t ReadEvt(MCorsikaFormat * fInFormat);
    Bool_t ReadEventHeader(Float_t * g);
    Bool_t ReadEvtEnd(MCorsikaFormat * fInFormat, Bool_t runNumberVerify);

    // TObject
    void Print(Option_t *t=NULL) const;

    ClassDef(MCorsikaRunHeader, 3)	// storage container for general info
};
#endif
