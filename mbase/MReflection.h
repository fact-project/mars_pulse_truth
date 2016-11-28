#ifndef MARS_MReflection
#define MARS_MReflection

#ifndef ROOT_TRotation
#include <TRotation.h>
#endif

class MReflection : public TRotation
{
private:
    MReflection(Double_t a, Double_t b, Double_t c,
                Double_t d, Double_t e, Double_t f,
                Double_t g, Double_t h, Double_t i)
        : TRotation(a,b,c,d,e,f,g,h,i) { }

    void Init(Double_t x, Double_t y, Double_t z)
    {
        // Normalize components of vector (including factor 2!)
        const Double_t n = -2./(x*x + y*y + z*z);

        // Expressed by the focal length
        //const Double_t n = -1./(2*F*F);

        // Construct rotation matrix to rotate the photon direction
        // around the axis defined by the normal vector. (This is
        // a simplified version of TVector3::Rotate(double, TVector3&)
        // The minus-sign does the relfection, i.e. flips the
        // direction of the vector.
        static_cast<TRotation&>(*this) =
            MReflection(
                        1+n*x*x,   n*y*x,   n*z*x,
                          n*x*y, 1+n*y*y,   n*z*y,
                          n*x*z,   n*y*z, 1+n*z*z
                       );

        //
        //  n = ( x / y (UInt_t)-1;/ z )  // Normalenvektor
        //  V = ( X / Y / Z )  // Photon to be reflected
        //
        //  nV := -2 * ( x*X + y*Y + z*Z ) / ( x*x + y*y + z*z )
        //
        //  V' = nV * vec(n) + vec(V)
        //

        // return TVector2(X/Z, Y/Z);
    }

public:
    // Normal vector of the reflecting surface
    MReflection(const TVector3 &v)
    {
        Init(v.X(), v.Y(), v.Z());
    }
    // Normal vector of the reflecting surface
    MReflection(Double_t x, Double_t y, Double_t z)
    {
        Init(x, y, z);
    }

    ClassDef(MReflection, 0) // A TRotation derivative describing the reflection by the normal vector of the surface
};

#endif
