#ifndef MARS_MCorsikaEvtHeader
#define MARS_MCorsikaEvtHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

// gcc 3.2
//class ifstream;
#include <iosfwd>

class MCorsikaFormat;

class MCorsikaEvtHeader : public MParContainer
{
private:
    UInt_t   fEvtNumber;              // Event number
    UInt_t   fNumReuse;               // Counter of the reuse of the same shower
    UInt_t   fTotReuse;               //! number of reuse of the same shower     
//    UInt_t   fParticleID;             // Particle ID (see MMcEvtBasic or CORSIKA manual)
    Float_t  fTotalEnergy;            // [GeV] 

    Float_t  fStartAltitude;          // [g/cm²]
    Float_t  fFirstTargetNum;         // Number of first target if fixed
    Float_t  fFirstInteractionHeight; // [cm] z coordinate, first intercation height

    Float_t  fMomentumX;              // [GeV/c] "+west"    "-east"
    Float_t  fMomentumY;              // [GeV/c] "+south"   "-north" (north denotes the magnet north which is defined to be in the geografic north!)
    Float_t  fMomentumZ;              // [GeV/c] "+upwards" "-downwards"

    Float_t  fZd;                     // [rad] Zenith distance
    Float_t  fAz;                     // [rad] Azimuth (north=0; east=90) (north denotes the magnet north which is defined to be in the geografic north!)

    Float_t  fX;                      // [cm] Position of telescope on ground x / - impact parameter x
    Float_t  fY;                      // [cm] Position of telescope on gorund y / - impact parameter y

    Float_t  fWeightedNumPhotons;     // weighted number of photons arriving at observation level

    Float_t fTempX[20];               //! Temporary storage for impact parameter
    Float_t fTempY[20];               //! Temporary storage for impact parameter

public:
    MCorsikaEvtHeader(const char *name=NULL, const char *title=NULL);

    //void Clear(Option_t * = NULL);
    void Print(Option_t * = NULL) const;

    UInt_t GetEvtNumber() const { return fEvtNumber; }
    UInt_t GetNumReuse() const { return fNumReuse; }
    UInt_t GetTotReuse() const { return fTotReuse; }
//    UInt_t GetParticleID() const { return fParticleID; }

    TVector3 GetMomentum() const { return TVector3(fMomentumX, fMomentumY, fMomentumZ); }
    TVector2 GetImpactPos() const { return TVector2(fX, fY); }

    Float_t GetTotalEnergy() const { return fTotalEnergy; }
    Float_t GetFirstInteractionHeight() const { return fFirstInteractionHeight; }

    Float_t GetZd() const { return fZd; }
    Float_t GetAz() const { return fAz; }

    Float_t GetX() const { return fX; }
    Float_t GetY() const { return fY; }

    Double_t GetImpact() const;

    void GetArrayOffset(Int_t arrayIdx, Float_t & xArrOff, Float_t & yArrOff)
                     {xArrOff = fTempY[arrayIdx]; yArrOff=-fTempX[arrayIdx]; }
    void SetTelescopeOffset(Int_t arrayIdx, Float_t xTelOff, Float_t yTelOff)
                     {fNumReuse = arrayIdx; fX = xTelOff; fY = yTelOff;}

    void IncNumReuse() { fNumReuse++; }
    void ResetNumReuse() { fNumReuse=0; }

    void InitXY() { fX=fTempY[fNumReuse]; fY=-fTempX[fNumReuse]; }
    void AddXY(Float_t x, Float_t y) { fX+=x; fY+=y; }

    Int_t ReadEvt(Float_t * f);                 // read in event header block
    Int_t ReadEvtEnd(MCorsikaFormat *informat); // read in event end block

    Bool_t SetupFits(fits &fin);

    ClassDef(MCorsikaEvtHeader, 3) // Parameter Conatiner for raw EVENT HEADER
}; 

#endif
