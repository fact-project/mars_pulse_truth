#ifndef MARS_MQuaternion
#define MARS_MQuaternion

#if 1

// We prefer to derive from TQuaternion instead of TLorantzVector
// because TQuaternion implements vector algebra with just the 3D vector

#ifndef ROOT_TQuaternion
#include <math.h>
#if (__GNUC__ < 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ < 8))
#define sqrt ::sqrt
#endif
#include <TQuaternion.h>
#undef sqrt
#endif

class MQuaternion : public TQuaternion
{
public:
    MQuaternion(const TQuaternion &q) : TQuaternion(q) { }
    MQuaternion(const TVector3 &v, Double_t t=0) : TQuaternion(v, t) { }
    void operator*=(const TRotation &r)
    {
        fVectorPart *= r;
    }
    Double_t X() const { return fVectorPart.X(); }
    Double_t Y() const { return fVectorPart.Y(); }
    Double_t Z() const { return fVectorPart.Z(); }
    Double_t T() const { return fRealPart; }

    // It seems to be a little bit faster than X*X+Y*Y
    Double_t R2() const { return XYvector().Mod2(); }
    Double_t R() const { return XYvector().Mod(); }

    void PropagateDz(const MQuaternion &w, const Double_t dz)
    {
        *this += dz/w.Z()*w;
    }

    // Propagates the particle by a distance f in z along
    // its trajectory w, if f is positive, in the opposite
    // direction otherwise.
    void PropagateZ(const MQuaternion &w, const Double_t z)
    {
        PropagateDz(w, z-Z());

        // z=3400, Z= 1700, t=0, c=1    -=  3400/-5*-5     -= 3400       Z=0,    c>0
        //                              +=  1700/-5*-5     += 1700       Z=1700, c>0
        // z=3400, Z=-1700, t=0, c=1    -= -3400/-5*-5     -= -1700      Z=0, c<0

        // z=3400, Z= 1700, t=0, c=1    -=  (3400-1700)/-5*-5     -= 3400       Z=0,    c>0
    }

    // Move the photon along its trajectory to the x/y plane
    // so that z=0. Therefor stretch the vector until
    // its z-component vanishes.
    //p -= p.Z()/u.Z()*u;
    void PropagateZ0(const MQuaternion &w)
    {
        // If z>0 we still have to move by a distance of z.
        // If z<0 we have to move in the opposite direction.
        //  --> z has the right sign for PropagateZ
        PropagateDz(w, -Z());

        // Z= 1700, t=0, c=1    -=  1700/-5*-5     -=  1700 +c     Z=0, c>0
        // Z=-1700, t=0, c=1    -= -1700/-5*-5     -= -1700 -c     Z=0, c<0


        // Z= 1700, t=0, c=1    -=  1700/ 5* 5     -=  1700 -c     Z=0, c<0
        // Z=-1700, t=0, c=1    -= -1700/ 5* 5     -= -1700 +c     Z=0, c>0

        //PropagateZ(w, Z());
    }

    TVector2 XYvector() const { return fVectorPart.XYvector(); }

    //void Normalize() { fVectorPart *= TMath::Sqrt(1 - R2())/Z(); }

    void NormalizeVector() { fVectorPart = fVectorPart.Unit(); }

    ClassDef(MQuaternion, 1)
};

#else

#ifndef ROOT_TLorentzVector
#include <TLorentzVector.h>
#endif

class MQuaternion : public TLorentzVector
{
public:
    //MQuaternion(const TLorentzVector &q) : TLorentzVector(q) { }
    MQuaternion(const TVector3 &v, Double_t t=0) : TLorentzVector(v, t) { }
    /*
    void operator*=(const TRotation &r)
    {
        fVectorPart *= r;
    }
    Double_t X() const { return fVectorPart.X(); }
    Double_t Y() const { return fVectorPart.Y(); }
    Double_t Z() const { return fVectorPart.Z(); }
    Double_t T() const { return fRealPart; }
    */

    // It seems to be a little bit faster than X*X+Y*Y
    Double_t R2() const { return Perp2(); }
    Double_t R() const { return Perp(); }

    // Propagates the particle by a distance f in z along
    // its trajectory w, if f is positive, in the opposite
    // direction otherwise.
    void PropagateZ(const MQuaternion &w, const Double_t f)
    {
        *this += f/TMath::Abs(w.Z())*w;
    }

    // Move the photon along its trajectory to the x/y plane
    // so that z=0. Therefor stretch the vector until
    // its z-component vanishes.
    //p -= p.Z()/u.Z()*u;
    void PropagateZ0(const MQuaternion &w)
    {
        // If z>0 we still have to move by a distance of z.
        // If z<0 we have to move in th eopposite direction.
        //  --> z has the right sign for PropagateZ
        PropagateZ(w, Z());
    }

    TVector2 XYvector() const { return Vect().XYvector(); }

    //void Normalize() { fVectorPart *= TMath::Sqrt(1 - R2())/Z(); }

    ClassDef(MQuaternion, 0)
};

#endif

#endif
