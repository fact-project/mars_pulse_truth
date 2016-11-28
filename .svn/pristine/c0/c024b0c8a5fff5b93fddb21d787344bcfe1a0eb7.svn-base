#ifndef MARS_MMirrorHex
#define MARS_MMirrorHex

#ifndef MARS_MMirror
#include "MMirror.h"
#endif

class MMirrorHex : public MMirror
{
protected:
    const static Double_t fgCos30;
    const static Double_t fgCos60;
    const static Double_t fgSin60;

    Double_t fD;  // half diameter D or better distance between opposite sides
    Double_t fMaxR;

    void SetD(Double_t d) { fD=d/2; fMaxR=fD/fgCos30; }

public:
    MMirrorHex() { SetD(24.75); }

    // MMirror
    Double_t GetMaxR() const { return fMaxR; }
    Double_t GetA() const;
    Double_t GetD() const { return fD; }

    Bool_t CanHit(const MQuaternion &p) const;
    Bool_t HasHit(const MQuaternion &p) const;

    Int_t ReadM(const TObjArray &tok);
    void  WriteM(std::ostream &out) const;

    // TObject
    void Paint(Option_t *);
    void Print(Option_t *) const;

    ClassDef(MMirrorHex, 1) // A spherical hexagon type mirror
};

class MMirrorHex90 : public MMirrorHex
{
public:
    MMirrorHex90() : MMirrorHex() { }

    // MMirror
    Bool_t HasHit(const MQuaternion &p) const;

    // TObject
    void Paint(Option_t *);

    ClassDef(MMirrorHex90, 1) // A spherical hexagon type mirror (MMirrorHex rotated by 90deg)
};

class MMirrorHex15 : public MMirrorHex
{
public:
    MMirrorHex15() : MMirrorHex() { }

    // MMirror
    Bool_t HasHit(const MQuaternion &p) const;

    // TObject
    void Paint(Option_t *);

    ClassDef(MMirrorHex15, 1) // A spherical hexagon type mirror (MMirrorHex rotated by 15deg)
};

#endif
