#ifndef MARS_MBadPixelsTreat
#define MARS_MBadPixelsTreat

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MSignalCam;
class MPedPhotCam;
class MBadPixelsCam;

class MBadPixelsTreat : public MTask
{
private:
    MGeomCam      *fGeomCam;       //! Camera geometry to get the area scaling factors
    MSignalCam    *fEvt;           //! Signal Event to be interpolated
    MBadPixelsCam *fBadPixels;     //! Bad Pixels to be interpolated

    MPedPhotCam   *fPedPhot1;      //! Pedestal from extractor used for "no-signal" in InterpolateTimes
    MPedPhotCam   *fPedPhot2;      //! Pedestal from extractor used for "no-signal" in InterpolateTimes

    TString fNameSignalCam;

    TList fPedPhotCams;            // List of PedPhotCams to be treated

    Byte_t  fFlags;                // flag for the method which is used
    Byte_t  fNumMinNeighbors;      // Number of neighbors required
    Float_t fMaxArrivalTimeDiff;   // Maximum allowed arrival time difference of neighboring pixels

    TList fNamePedPhotCams;

    enum
    {
        kUseInterpolation   = 1,
        kUseCentralPixel    = 2,
        kProcessPedestalRun = 3,
        kProcessPedestalEvt = 4,
        kProcessTimes       = 5,
        kHardTreatment      = 6
    };

    static Double_t Pow2(Double_t x) { return x*x; }

    void InterpolateTimes() const;
    void InterpolateSignal() const;
    void InterpolatePedestals(MPedPhotCam &pedphot) const;
    void InterpolatePedestals() const;

    void   Unmap() const;
    Bool_t IsPixelBad(Int_t idx) const;

    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    void   StreamPrimitive(std::ostream &out) const;

public:
    MBadPixelsTreat(const char *name=NULL, const char *title=NULL);

    void SetUseInterpolation(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kUseInterpolation) : CLRBIT(fFlags, kUseInterpolation);
    }
    void SetUseCentralPixel(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kUseCentralPixel) : CLRBIT(fFlags, kUseCentralPixel);
    }
    void SetProcessPedestalRun(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kProcessPedestalRun) : CLRBIT(fFlags, kProcessPedestalRun);
    }
    void SetProcessPedestalEvt(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kProcessPedestalEvt) : CLRBIT(fFlags, kProcessPedestalEvt);
    }
    void SetProcessPedestal(Bool_t b=kTRUE)
    {
        SetProcessPedestalRun(!b);
        SetProcessPedestalEvt(b);
    }
    void SetProcessTimes(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kProcessTimes) : CLRBIT(fFlags, kProcessTimes);
    }
    void SetHardTreatment(Bool_t b=kTRUE)
    {
        b ? SETBIT(fFlags, kHardTreatment) : CLRBIT(fFlags, kHardTreatment);
    }

    Bool_t IsHardTreatment() const      { return TESTBIT(fFlags, kHardTreatment); }
    Bool_t IsProcessPedestalRun() const { return TESTBIT(fFlags, kProcessPedestalRun); }
    Bool_t IsProcessPedestalEvt() const { return TESTBIT(fFlags, kProcessPedestalEvt); }
    Bool_t IsProcessTimes() const       { return TESTBIT(fFlags, kProcessTimes); }
    Bool_t IsUseCentralPixel() const    { return TESTBIT(fFlags, kUseCentralPixel); }
    Bool_t IsUseInterpolation() const   { return TESTBIT(fFlags, kUseInterpolation); }

    void SetNumMinNeighbors(Byte_t num)   { fNumMinNeighbors=num; }
    void SetMaxArrivalTimeDiff(Float_t d) { fMaxArrivalTimeDiff=d; }
    void SetNameSignalCam(const char *name="MSignalCam") { fNameSignalCam=name; }
    void AddNamePedPhotCam(const char *name="MPedPhotCam");
    void SetNamePedPhotCam(const char *name)
    {
        AddNamePedPhotCam(name);
    } // Deprecated! Use AddNamePedPhotCam instead (directly)

    ClassDef(MBadPixelsTreat, 1) // Task to treat bad pixels (interpolation, unmapping)
}; 

#endif

