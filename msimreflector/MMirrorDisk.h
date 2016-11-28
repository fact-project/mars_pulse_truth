#ifndef MARS_MMirrorDisk
#define MARS_MMirrorDisk

#ifndef MARS_MMirror
#include "MMirror.h"
#endif

class MMirrorDisk : public MMirror
{
private:
    Double_t fR; // Radius of the disk

public:
    MMirrorDisk() : fR(24.75) { }

    // MMirror
    Double_t GetMaxR() const { return fR; }
    Double_t GetA() const;

    Bool_t CanHit(const MQuaternion &p) const;
    Bool_t HasHit(const MQuaternion &p) const;

    Int_t ReadM(const TObjArray &tok);
    void  WriteM(std::ostream &out) const;

    //TObject
    void Paint(Option_t *);
    void Print(Option_t *) const;

    ClassDef(MMirrorDisk, 1)  // A spherical disk type mirror
};

#endif
