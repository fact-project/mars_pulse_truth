#ifndef MARS_MMirrorSquare
#define MARS_MMirrorSquare

#ifndef MARS_MMirror
#include "MMirror.h"
#endif

class MMirrorSquare : public MMirror
{
private:
    Double_t fSideLength; // HALF of the side length!

public:
    MMirrorSquare() : fSideLength(24.75) { }

    // MMirror
    Double_t GetMaxR() const;
    Double_t GetA() const;

    Bool_t HasHit(const MQuaternion &p) const;
    Bool_t CanHit(const MQuaternion &p) const;

    Int_t ReadM(const TObjArray &tok);
    void  WriteM(std::ostream &out) const;

    // TObject
    void Paint(Option_t *);
    void Print(Option_t *) const;

    ClassDef(MMirrorSquare, 1) // A spherical square type mirror
};

#endif
