#ifndef MARS_MHillasExt
#define MARS_MHillasExt

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TArrayF;

class MHillas;
class MGeomCam;
class MSignalCam;

class MHillasExt : public MParContainer
{
private:
    // for description see MExtHillas.cc
    Float_t fAsym;    // [mm] fDist minus dist: center of ellipse, highest pixel
    Float_t fM3Long;  // [mm] 3rd moment (e-weighted) along major axis
    Float_t fM3Trans; // [mm] 3rd moment (e-weighted) along minor axis

    Float_t fSlopeLong;   // [ns/mm] Slope of the arrival time along the major axis
    Float_t fSlopeTrans;  // [ns/mm] Slope of the arrival time along the minor axis

    // FIXME: We could try to use the Medians!
    Float_t fTimeSpread;          // [ns] Spread (RMS) of the arrival times around the mean arrival time
    Float_t fTimeSpreadWeighted;  // [ns] Weighted spread (RMS) of the arrival times around the weighted mean arrival time

    Float_t fSlopeSpread;         // [ns] Spread (RMS) of the arrival time around the slope along the major axis
    Float_t fSlopeSpreadWeighted; // [ns] Weighted spread (RMS) of the arrival time around the slope along the major axis

public:
    MHillasExt(const char *name=NULL, const char *title=NULL);

    void Reset();

    Float_t GetAsym() const                { return fAsym; }
    Float_t GetM3Long() const              { return fM3Long; }
    Float_t GetM3Trans() const             { return fM3Trans; }
    Float_t GetSlopeLong() const           { return fSlopeLong; }
    Float_t GetSlopeTrans() const          { return fSlopeTrans; }
    Float_t GetTimeSpread() const          { return fTimeSpread; }
    Float_t GetTimeSpreadWeighted() const  { return fTimeSpreadWeighted; }
    Float_t GetSlopeSpread() const         { return fSlopeSpread; }
    Float_t GetSlopeSpreadWeighted() const { return fSlopeSpreadWeighted; }

    Int_t Calc(const MGeomCam &geom, const MSignalCam &pix,
               const MHillas &hil, Int_t island=-1);

    void Print(Option_t *opt=NULL) const;
    void Print(const MGeomCam &geom) const;

    void Paint(Option_t *o="");

    void Set(const TArrayF &arr);

    ClassDef(MHillasExt, 5) // Storage Container for extended Hillas Parameter
};
#endif
