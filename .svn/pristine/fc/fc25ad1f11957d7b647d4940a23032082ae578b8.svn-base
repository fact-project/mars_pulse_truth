#ifndef MARS_MObservatory
#define MARS_MObservatory

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MTime;
class TArrayD;

class MObservatory : public MParContainer
{
public:
    enum LocationName_t
    {
        kMagic1,
        kWuerzburgCity,
        kTuorla
    };

private:
    LocationName_t fObservatoryKey;  //!

    TString  fObservatoryName;       //! Name of the observatory

    Double_t fLongitude;             //! [rad] Longitude of observatory (+ east)
    Double_t fLatitude;              //! [rad] Latitude of observatory (+ north)

    Double_t fSinLatitude;           //! Sin component for faster access
    Double_t fCosLatitude;           //! Cos component for faster access

    Double_t fHeight;                //! [m] height of observatory

    void Init(const char *name=NULL, const char *title=NULL);

public:
    MObservatory(const char *name=NULL, const char *title=NULL);
    MObservatory(LocationName_t key, const char *name=NULL, const char *title=NULL);
    MObservatory(Double_t lon, Double_t lat, const char *name="<n/a>");
    MObservatory(Double_t lon, Double_t lat, Double_t h, const char *name="<n/a>");

    void Copy(TObject &obj) const
    {
        MObservatory &obs = (MObservatory&)obj;
        obs.fObservatoryName = fObservatoryName;
        obs.fLongitude = fLongitude;
        obs.fLatitude = fLatitude;
        obs.fSinLatitude = fSinLatitude;
        obs.fCosLatitude = fCosLatitude;
        obs.fHeight = fHeight;
    }

    void SetLocation(LocationName_t name);
    void SetLocation(Double_t lon, Double_t lat, Double_t h=0, const char *name=NULL);

    void Print(Option_t *o=0) const;

    const TString &GetObservatoryName() const { return fObservatoryName; }

    Double_t GetLatitudeDeg() const     { return fLatitude*kRad2Deg; }  //[deg]
    Double_t GetLongitudeDeg() const    { return fLongitude*kRad2Deg; } //[deg]

    Double_t GetLatitudeRad() const     { return fLatitude; }           //[rad]
    Double_t GetLongitudeRad() const    { return fLongitude; }          //[rad]

    Double_t GetPhi() const             { return fLatitude; }           //[rad]
    Double_t GetElong() const           { return fLongitude; }          //[rad]

    Double_t GetSinPhi() const          { return fSinLatitude; }
    Double_t GetCosPhi() const          { return fCosLatitude; }

    Double_t GetHeight() const          { return fHeight; }

    TArrayD GetSunRiseSet(Double_t mjd, Double_t alt=0) const;

    void RotationAngle(Double_t theta, Double_t phi, Double_t &sin, Double_t &cos) const;
    Double_t RotationAngle(Double_t theta, Double_t phi) const;

    LocationName_t GetObservatoryKey() const { return fObservatoryKey; }

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MObservatory, 0) // class storing observatory locations
};

#endif
