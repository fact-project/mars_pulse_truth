#ifndef MARS_MPointingPos
#define MARS_MPointingPos

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

// FIXME: Should not be here... (ZdAz)
#ifndef MARS_MPointing
#include "MPointing.h"
#endif

class fits;
class MTime;
class MObservatory;
class MPointingDev;

class MPointingPos : public MParContainer
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    Double_t fZd;  // [deg] Zenith distance (ZA) {fits: unit=deg}
    Double_t fAz;  // [deg] Azimuth              {fits: unit=deg}

    Double_t fRa;  // [h]   Right ascension      {fits: unit=h}
    Double_t fHa;  // [h]   Hour angle           {fits: unit=h}
    Double_t fDec; // [deg] Declination          {fits: unit=deg}

    // Hour angle!
    // Angle to magnetic field!

public:
    MPointingPos(const char *name=0, const char *title=0) : fZd(0), fAz(0), fRa(0), fHa(0), fDec(0)
    {
        fName  = name  ? (TString)name  : gsDefName;
        fTitle = title ? (TString)title : gsDefTitle;
    }
    MPointingPos(const MPointingPos &p) : MParContainer(p),
        fZd(p.fZd), fAz(p.fAz), fRa(p.fRa), fHa(p.fHa), fDec(p.fDec)
    {
    }

    Bool_t IsInitialized() const { return !(fZd==0 && fAz==0 && fRa==0 && fHa==0 && fDec==0); }

    void Print(Option_t *o="") const;

    TString GetString(Option_t *o="") const;

    void SetLocalPosition(Double_t zd, Double_t az) { fZd=zd; fAz=az; }
    void SetSkyPosition(Double_t ra, Double_t dec, Double_t ha=0) { fRa=ra; fDec=dec; fHa=ha; }

    Double_t GetZd() const  { return fZd; }
    Double_t GetAz() const  { return fAz; }

    Double_t GetZdRad() const;//  { return fZd*TMath::DegToRad(); }
    Double_t GetAzRad() const;//  { return fAz*TMath::DegToRad(); }

    ZdAz GetZdAz() const { return ZdAz(fZd, fAz); }

    Double_t GetRa() const  { return fRa; }
    Double_t GetHa() const  { return fHa; }
    Double_t GetDec() const { return fDec; }

    Double_t GetRaRad() const;//  { return fRa*TMath::DegToRad()*15; }
    Double_t GetDecRad() const;// { return fDec*TMath::DegToRad(); }

    Double_t RotationAngle(const MObservatory &o) const;
    Double_t RotationAngle(const MObservatory &o, const MTime &t, const MPointingDev *dev=0) const;
    Double_t RotationAngle(const MObservatory &o, const MTime *t) const
    {
        return t ? RotationAngle(o, *t) : RotationAngle(o);
    }

    Bool_t SetupFits(fits &fin);

    ClassDef(MPointingPos, 1) //Container storing the (corrected) telescope pointing position
};

#endif
