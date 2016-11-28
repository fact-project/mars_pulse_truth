#ifndef MARS_MHillasSrc
#define MARS_MHillasSrc

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TArrayF;

class MHillas;
class MGeomCam;
class MSrcPosCam;

class MHillasSrc : public MParContainer
{
private:
    const MSrcPosCam *fSrcPos; //! Source position in the camere

    Float_t fAlpha;         // [deg]  angle of major axis with vector to src
    Float_t fDist;          // [mm]   distance between src and center of ellipse
    Float_t fCosDeltaAlpha; // [1]    cosine of angle between d and a

    Float_t fDCA;           // [mm]   Distance to closest approach 'DCA'
    Float_t fDCADelta;      // [deg]  Angle of the shower axis with respect to the x-axis

public:
    MHillasSrc(const char *name=NULL, const char *title=NULL);
    MHillasSrc(const MHillasSrc &h) { h.Copy(*this); }

    void Copy(TObject &o) const
    {
        MHillasSrc &src = static_cast<MHillasSrc&>(o);
        src.fSrcPos = fSrcPos;
        src.fAlpha  = fAlpha;
        src.fDist   = fDist;
        src.fCosDeltaAlpha = fCosDeltaAlpha;
        src.fDCA = fDCA;
        src.fDCADelta = fDCADelta;
    }

    void SetSrcPos(const MSrcPosCam *pos) { fSrcPos = pos; }
    const MSrcPosCam *GetSrcPos() const   { return fSrcPos; }

    void Reset();

    Float_t GetAlpha()         const { return fAlpha; }
    Float_t GetDist()          const { return fDist; }
    Float_t GetCosDeltaAlpha() const { return fCosDeltaAlpha; }
    Float_t GetDCA()           const { return fDCA; }
    Float_t GetDCADelta()      const { return fDCADelta; }

    void Print(Option_t *opt=NULL) const;
    void Print(const MGeomCam &geom) const;
    void Paint(Option_t *opt=NULL);

    virtual Int_t Calc(const MHillas &hillas);

    void Set(const TArrayF &arr);

    ClassDef(MHillasSrc, 5) // Container to hold source position dependant parameters
};

#endif
