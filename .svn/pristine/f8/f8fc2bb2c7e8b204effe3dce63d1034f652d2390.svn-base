#ifndef MARS_MSignalPix
#define MARS_MSignalPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MMath
#include "MMath.h"
#endif

class MSignalPix : public MParContainer
{
private:
    Bool_t   fIsCore;        //! the pixel is a Core pixel -> kTRUE
    Short_t  fRing;          //  NT: number of analyzed rings around the core pixels, fRing>0 means: used, fRing= 0 means: unused, fRing= -1 means: unmapped (no possible to use in the calculation of the image parameters)
    Short_t  fIdxIsland;     //! the pixel is a Core pixel -> kTRUE

    Float_t  fPhot;          // The number of Cerenkov photons
    Float_t  fErrPhot;       // the error of fPhot
    Float_t  fArrivalTime;   // Calibrated Arrival Time
    Float_t  fTimeSlope;     // Time between half rise time and position of maximum

public:
    MSignalPix(Float_t phot=0, Float_t errphot=0);
    MSignalPix(const MSignalPix &pix)
        : fIsCore(pix.fIsCore), fRing(pix.fRing), fIdxIsland(pix.fIdxIsland),
        fPhot(pix.fPhot), fErrPhot(pix.fErrPhot), fArrivalTime(pix.fArrivalTime),
        fTimeSlope(pix.fTimeSlope)
    {
    }

    // TObject
    void Clear(Option_t *o=0);
    void Copy(TObject &obj) const
    {
        MSignalPix &pix = (MSignalPix&)obj;
        pix.fIsCore      = fIsCore;
        pix.fRing        = fRing;
        pix.fIdxIsland   = fIdxIsland;
        pix.fPhot        = fPhot;
        pix.fErrPhot     = fErrPhot;
        pix.fArrivalTime = fArrivalTime;
        pix.fTimeSlope   = fTimeSlope;
    }
    void    Print(Option_t *opt = NULL) const;

    // MSignalPix
    Float_t GetNumPhotons() const         { return fPhot;    }
    Float_t GetErrorPhot() const          { return fErrPhot; }
    Float_t GetArrivalTime() const        { return fArrivalTime; }
    Float_t GetTimeSlope() const          { return fTimeSlope; }

    Bool_t  IsPixelUsed() const           { return fRing>0; }
    Bool_t  IsPixelUnmapped() const       { return fRing==-1; }
    void    SetPixelUnused()              { fRing=0;  }
    void    SetPixelUsed()                { fRing=1;  }
    void    SetPixelUnmapped()            { fRing=-1; }
    void    SetIdxIsland(Short_t num)     { fIdxIsland=num; }
    Short_t GetIdxIsland() const          { return fIdxIsland; }

    void    SetRing(UShort_t r)           { fRing = r;   }
    Short_t GetRing() const               { return fRing;}

    void    SetPixelCore(Bool_t b=kTRUE)  { fIsCore = b; }
    Bool_t  IsPixelCore() const           { return fIsCore;  }

    void    SetNumPhotons(Float_t f)      { MMath::ReducePrecision(f); fPhot    = f; }
    void    SetErrorPhot(Float_t f)       { MMath::ReducePrecision(f); fErrPhot = f; }
    void    Set(Float_t np, Float_t ep)   { MMath::ReducePrecision(np); MMath::ReducePrecision(ep);  fPhot = np; fErrPhot = ep; }
    void    SetArrivalTime(Float_t tm)    { fArrivalTime = tm; }
    void    SetTimeSlope(Float_t ts)      { fTimeSlope = ts; }

    //void    AddNumPhotons(Float_t f)      { fPhot += f; }
    //void    Scale(Float_t f)              { fPhot /= f; }

    ClassDef(MSignalPix, 8)  // class containing information about the Cerenkov Photons in a pixel
};

#endif
