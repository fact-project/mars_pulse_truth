#ifndef MARS_MVector3
#define MARS_MVector3

#ifndef ROOT_TVector3
#include <TVector3.h>
#endif

class MVector3 : public TVector3
{
private:
    enum VectorType_t
    {
        kIsInvalid,
        kIsRaDec,
        kIsZdAz,
        kIsAltAz,
        kIsArbitrary
    };

    VectorType_t fType;

    TString fName;

    void SetThetaPhiMag(Double_t theta, Double_t phi, Double_t mag); // SetMagThetaPhi(TMath::Power(10, -mag/2.5), theta, phi);

public:
    MVector3() { fType=kIsInvalid; }
    MVector3(Double_t theta, Double_t phi, Double_t mag=0)
    {
        SetThetaPhiMag(theta, phi, mag);
        fType=kIsArbitrary;
    }
    MVector3(const TVector3 &v3) : TVector3(v3) { fType=kIsArbitrary; }
    Double_t Magnitude() const;// { return -2.5*TMath::Log10(Mag()); }

    Bool_t IsValid() const { return fType!=kIsInvalid; }

    void SetRaDec(Double_t ra, Double_t dec, Double_t mag=0); // SetThetaPhiMag(TMath::Pi()/2-dec, ra, mag);
    void SetAltAz(Double_t alt, Double_t az, Double_t mag=0); // SetThetaPhiMag(TMath::Pi()/2-alt, az, mag);
    void SetZdAz(Double_t zd, Double_t az, Double_t mag=0)
    {
        fType = kIsZdAz;
        SetThetaPhiMag(zd, az, mag);
    }

    void SetName(const TString &str) { fName = str.Strip(TString::kBoth); }
    const char *GetName() const { return fName; }

    void WriteBinary(std::ostream &out) const;
    void ReadBinary(std::istream &in);

    ClassDef(MVector3, 1) // A specialized TVector3 storing a star-name
};

#endif
