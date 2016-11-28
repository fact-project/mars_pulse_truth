#ifndef MARS_MMcConfigRunHeader
#define MARS_MMcConfigRunHeader

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

#ifndef MARS_MGeomMirror
#include "MGeomMirror.h"
#endif
#ifndef MARS_MGeomPMT
#include "MGeomPMT.h"
#endif

class MGeomMirror;
class MGeomPMT;

class MMcConfigRunHeader : public MParContainer
{
private:
    // Mirror Information
    UShort_t      fNumMirrors;
    Float_t       fRadiusMirror;// [cm] Radius of a single mirror
    TClonesArray *fMirrors;     // FIXME: Change TClonesArray away from a pointer?

    Float_t fMirrorFraction;    // (adimensional, between 0 and 1) Fraction of mirror dish
                                //  which is really working.

    // Magic Def Parameters
    Float_t fFocalDist;         // [cm] Focal distance
    Float_t fParaboloidFocal;   // [cm] True focal of the paraboloid on which the mirror centers are placed
    Float_t fPointSpread;       // [cm] Point spread function, sigma in x and y on the camera as simualted in the Reflector
    Float_t fPointSpreadX;      // [cm] Point spread function, sigma in x on the camera adding gaussian in the Camera
    Float_t fPointSpreadY;      // [cm] Point spread function, sigma in y on the camera adding gaussian in the Camera
    Float_t fBlackSpot;         // [cm] Radius of black spot in the mirror center
    Float_t fCameraWidth;       // [cm] Radius on the camera plain 
                                // inside which the phe are kept.

    Float_t fMissPointingX;     // [deg] Misspointing in deg added in he Camera 
    Float_t fMissPointingY;     // [deg] simulation at rho (rotation FoV) = 0. 

    Float_t fPmtTimeJitter;     // [ns] PMT time jitter (sigma of gaussian), per phe-

    // QE Information
    UInt_t  fNumPMTs;
    TClonesArray  *fPMTs;

    // Light Collection Information (guides , plexiglas, 1st dynode) 

    TArrayF fIncidentTheta;    // [deg] Angle (0-180 deg) between light 
                               // direction and camera plane.
    TArrayF fLightCollectionFactor;//   Inner pixels
    TArrayF fLightCollectionFactorOuter;//  Outer pixels

public:
    MMcConfigRunHeader(const char *name=NULL, const char *title=NULL);
    ~MMcConfigRunHeader() { delete fMirrors; delete fPMTs; }

    void SetMagicDef(Float_t radius, Float_t focal, Float_t point,
                     Float_t spot, Float_t camwidth);
    void SetLightCollection(const TArrayF &theta, const TArrayF &factor, 
			    const TArrayF &factor_outer);

    UInt_t GetNumMirror() const { return fNumMirrors; }
    void   InitSizeMirror(UInt_t num) { fMirrors->Expand(num); }

    void   SetMirrorFraction(Float_t x) { fMirrorFraction = x; }

    UInt_t GetNumPMTs() const { return fNumPMTs; }
    void   InitSizePMTs(UInt_t num) { fPMTs->Expand(num); }

    Float_t GetPointSpread() const { return fPointSpread; }
    void    SetPointSpread(Float_t x) { fPointSpread = x; }

    Float_t GetPointSpreadX() const { return fPointSpreadX; }
    void    SetPointSpreadX(Float_t x) { fPointSpreadX = x; }
    Float_t GetPointSpreadY() const { return fPointSpreadY; }
    void    SetPointSpreadY(Float_t x) { fPointSpreadY = x; }

    Float_t GetMissPointingX() const {return fMissPointingX;}
    void    SetMissPointingX(Float_t x) {fMissPointingX=x;}

    Float_t GetMissPointingY() const {return fMissPointingY;}
    void    SetMissPointingY(Float_t x) {fMissPointingY=x;}

    const TArrayF &GetLightCollectionFactor() const {return fLightCollectionFactor; }
    const TArrayF &GetLightCollectionFactorOuter() const {return fLightCollectionFactorOuter; }

    Float_t GetMirrorFraction() const { return fMirrorFraction; }

    const TArrayF &GetIncidentTheta() const {return fIncidentTheta;}

    void   AddMirror(Int_t id)
    {
        new ((*fMirrors)[fNumMirrors++]) MGeomMirror(id);
    }

    void   AddPMT(Int_t id)
    {
        new ((*fPMTs)[fNumPMTs++]) MGeomPMT(id);
    }

    MGeomMirror &GetMirror(int i)  { return *(MGeomMirror*)(fMirrors->UncheckedAt(i)); }
    MGeomMirror &GetMirror(int i) const { return *(MGeomMirror*)(fMirrors->UncheckedAt(i)); }

    TClonesArray *GetMirrors() { return fMirrors; }

    MGeomPMT &GetPMT(int i)  { return *(MGeomPMT*)(fPMTs->UncheckedAt(i)); }
    MGeomPMT &GetPMT(int i) const { return *(MGeomPMT*)(fPMTs->UncheckedAt(i)); }
    void    SetPmtTimeJitter(Float_t x) { fPmtTimeJitter = x; }
    Float_t GetPmtTimeJitter() const { return fPmtTimeJitter; }

    ClassDef(MMcConfigRunHeader, 6)  // class for monte carlo configuration information
};

#endif

