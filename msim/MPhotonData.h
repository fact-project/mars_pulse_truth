#ifndef MARS_MPhotonData
#define MARS_MPhotonData

#ifndef MARS_MMcEvtBasic
#include "MMcEvtBasic.h"
#endif

#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

#ifndef ROOT_TQuaternion
#include <math.h>
//#define sqrt ::sqrt
#include <TQuaternion.h>
//#undef sqrt
#endif

// gcc 3.2
//class ifstream;
#include <iosfwd>

class MCorsikaRunHeader;

class MPhotonData : public TObject
{
private:
    Float_t fPosX;                       // [cm] "+west"    "-east"  (both at observation level)
    Float_t fPosY;                       // [cm] "+south"   "-north" (north denotes the magnet north which is defined to be in the geografic north!)

    Float_t fCosU;                       // [cos x] U direction cosine to x-axis
    Float_t fCosV;                       // [cos y] V direction cosine to y-axis

    Float_t fTime;                       // [ns] Time since first interaction or entrance into atmosphere
    Short_t fWavelength;                 // [nm] Wavelength
//    UInt_t   fNumPhotons;                // Number of cherenkov photons ins bunch
    Float_t  fProductionHeight;          // [cm] Height of bunch production
    MMcEvtBasic::ParticleId_t fPrimary;  // Type of emitting particle

    Int_t   fTag;    //! A tag for external use
    Int_t   fMirrorTag;    // Tag for the mirror ID of the mirror which was hit by this photon
    Float_t fWeight; //! A weight for external use

public:
    MPhotonData(/*const char *name=NULL, const char *title=NULL*/);
    //MPhotonData(const MPhotonData &ph);

    // Getter 1D
    Float_t  GetPosX()  const { return fPosX; }
    Float_t  GetPosY()  const { return fPosY; }

    Float_t  GetCosU()  const { return fCosU; }
    Float_t  GetCosV()  const { return fCosV; }
    Double_t GetCosW()  const;
    Double_t GetSinW()  const;
    Double_t GetCosW2()  const;
    Double_t GetSinW2()  const;
    Double_t GetTheta() const;

    Double_t GetTime()  const { return fTime; }

    // Getter 2D
    TVector2 GetPos2()  const { return TVector2(fPosX, fPosY); }
    TVector2 GetDir2()  const { return TVector2(fCosU, fCosV);}
//    TVector2 GetDir2() const { return TVector2(fCosU, fCosV)/(1-TMath::Hypot(fCosU, fCosV)); }

    // Getter 3D
    TVector3 GetPos3()  const { return TVector3(fPosX, fPosY, 0); }
    TVector3 GetDir3()  const { return TVector3(fCosU, fCosV, -GetCosW()); }

    // Getter 4D
    TQuaternion GetPosQ() const;
    TQuaternion GetDirQ() const;

    // Getter Others
    UShort_t GetWavelength() const { return TMath::Abs(fWavelength); }
    Float_t GetProductionHeight() const { return fProductionHeight; }
    MMcEvtBasic::ParticleId_t GetPrimary() const { return fPrimary; }

    // Status of the simulation of photons
    Bool_t IncludesEfficiencies() const { return fWavelength<0; }

    //virtual Float_t GetWeight() const { return 1; }
    virtual Float_t GetWeight() const { return fWeight; }
    void SetWeight(Float_t w=1) { fWeight=w; }

    // Setter
    void SetPosition(Float_t x, Float_t y)  { fPosX=x; fPosY=y; }
    void SetDirection(Float_t u, Float_t v) { fCosU=u; fCosV=v; }

    void SetPosition(const TVector2 &p)     { fPosX=p.X(); fPosY=p.Y(); }
    void SetDirection(const TVector2 &d)    { fCosU=d.X(); fCosV=d.Y(); }

    void SetPosition(const TQuaternion &p)  { fPosX=p.fVectorPart.X(); fPosY=p.fVectorPart.Y(); fTime=p.fRealPart; }
    void SetDirection(const TQuaternion &d) { fCosU=d.fVectorPart.X(); fCosV=d.fVectorPart.Y(); }

    void SetPrimary(MMcEvtBasic::ParticleId_t p) { fPrimary=p; }
    void SetWavelength(Short_t wl) { fWavelength=wl; }

    void AddTime(Double_t dt) { fTime += dt; }
    void SetTime(Double_t t)  { fTime  = t; }

    void SimWavelength(Float_t wmin, Float_t wmax);

    void Copy(TObject &obj) const;

    void SetTag(Int_t tag) { fTag=tag; }
    Int_t GetTag() const { return fTag; }

    void SetMirrorTag(Int_t mirrorTag) { fMirrorTag=mirrorTag; }
    Int_t GetMirrorTag() const { return fMirrorTag; }

    // TObject
    //void Clear(Option_t * = NULL);
    void Print(Option_t * = NULL) const;
    //void Draw (Option_t * = NULL);
    Bool_t IsSortable() const { return kTRUE; }
    Int_t  Compare(const TObject *obj) const
    {
        const MPhotonData &d = *static_cast<const MPhotonData*>(obj);
        if (fTime<d.fTime)
            return -1;
        if (fTime>d.fTime)
            return 1;
        return 0;
    }

    // I/O
    //Int_t ReadCorsikaEvt(istream &fin);
    //Int_t ReadRflEvt(istream &fin);

    Int_t FillCorsika(Float_t f[7], Int_t i);
    Int_t FillEventIO(Short_t f[8]);
    Int_t FillEventIO(Float_t f[8]);
    Int_t FillRfl(Float_t f[8]);

    ClassDef(MPhotonData, 2) //Container to store a cherenkov photon bunch from a CORSUKA file
};

#endif
