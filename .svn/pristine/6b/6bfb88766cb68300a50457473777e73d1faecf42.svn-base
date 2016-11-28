#ifndef MARS_MMirror
#define MARS_MMirror

#ifndef ROOT_TRotation
#include <TRotation.h>
#endif

class MQuaternion;

class MMirror : public TObject
{
    friend void operator/=(MQuaternion &, const MMirror &);
    friend void operator*=(MQuaternion &, const MMirror &);
    friend void operator-=(MQuaternion &, const MMirror &);
    friend void operator+=(MQuaternion &, const MMirror &);

private:
    TVector3  fPos;
    TVector3  fNorm;  // faster without

    TRotation fTilt;

    // ----- Spherical mirror data members -----
    Double_t  fFocalLength;
    Double_t  fSigmaPSF;

    Int_t fShape; // Spherical=0, Parabolic=1

    //    MMirror *fNeighbors[964];

public:
    MMirror() : fSigmaPSF(-1), fShape(0)
    {
    }

    // ----- Mirror basic functions -----
    TVector2 operator-(const MQuaternion &q) const;// { return TVector2(X()-q.X(), Y()-q.Y()); }
    TVector2 operator-(const MMirror &m) const;// { return TVector2(X()-m.X(), Y()-m.Y()); }

    void SetSigmaPSF(Double_t psf) { fSigmaPSF = psf; }
    void SetFocalLength(Double_t f) { fFocalLength = f; }
    void SetPosition(const TVector3 &v) { fPos = v; }
    void SetNorm(const TVector3 &n) {
        fNorm = n;

        fTilt = TRotation();
        // Can be simplified??  rotate the mirror along
        // perpendicular to its normal projected to x/y and z
        // by its "zenith angle"
        fTilt.Rotate(-n.Theta(), TVector3(-n.Y(), n.X(), 0));
    }
    void SetShape(Char_t c);

    void SetZ(Double_t z) { fPos.SetZ(z); }

    Double_t X() const { return fPos.X(); }
    Double_t Y() const { return fPos.Y(); }
    Double_t Z() const { return fPos.Z(); }

    Double_t Nx() const { return fNorm.X(); }
    Double_t Ny() const { return fNorm.Y(); }
    Double_t Nz() const { return fNorm.Z(); }

    TVector2 GetPosXY() const { return fPos.XYvector(); }
    const TVector3 &GetPos() const { return fPos; }
    const TVector3 &GetNorm() const { return fNorm; }

    Double_t GetFocalLength() const { return fFocalLength; }
    Double_t GetSigmaPSF() const { return fSigmaPSF; }

    Double_t GetDist() const { return fPos.Perp(); }
    Int_t GetShape() const { return fShape; }

    virtual Double_t GetMaxR() const=0;// { return TMath::Max(fMaxRX, fMaxRY); }
    virtual Double_t GetA() const=0;// { return TMath::Max(fMaxRX, fMaxRY); }

    TVector3 SimPSF(const TVector3 &n, Double_t F, Double_t psf) const;
    TVector3 SimPSF(const TVector3 &n) const
    {
        return SimPSF(n, fFocalLength, fSigmaPSF/10); // Convert from mm to cm
    }

    Bool_t ExecuteMirror(MQuaternion &p, MQuaternion &u) const;

    // ----- Basic function for parabolic mirror -----
    Bool_t ExecuteReflection(MQuaternion &p, MQuaternion &u) const;

    // ----- Mirror specialized functions -----

    virtual Bool_t HasHit(const MQuaternion &p) const=0;
    virtual Bool_t CanHit(const MQuaternion &p) const=0;

    virtual Int_t ReadM(const TObjArray &tok)=0;
    virtual void  WriteM(std::ostream &out) const=0;

    // TObject
    void Print(Option_t *o) const;

    /*
    Bool_t IsSortable() const { return kTRUE; }
    Int_t Compare(const TObject *obj) const
    {
        MMirror &m = (MMirror&)*obj;
        return m.fPos.Mag2()<fPos.Mag2();
    }
    */
    ClassDef(MMirror, 2) // Base class to describe a mirror
};

#endif
