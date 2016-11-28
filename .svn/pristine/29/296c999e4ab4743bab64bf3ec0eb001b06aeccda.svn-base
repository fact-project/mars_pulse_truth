#ifndef MARS_MReportDrive
#define MARS_MReportDrive

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportDrive : public MReport
{
private:
    Double_t fMjd;        // Modified Julian Date send by the drive system

    Double_t fRa;         // [h]   Right ascension
    Double_t fDec;        // [deg] Declination
    Double_t fHa;         // [h]   Hour angle

    Double_t fNominalZd;  // [deg] Nominal zenith distance
    Double_t fNominalAz;  // [deg] Nominal azimuth
    Double_t fCurrentZd;  // [deg] current zenith distance
    Double_t fCurrentAz;  // [deg] current azimuth

    Double_t fErrorZd;    // [deg] system error in the zenith angle axis
    Double_t fErrorAz;    // [deg] sistem error in the azimuth angle axis

    Bool_t SetupReadingFits(fits &fits);
    Int_t  InterpreteFits(const fits &fits);
    Int_t  InterpreteBody(TString &str, Int_t ver);

public:
    MReportDrive();

    Double_t GetMjd() const       { return fMjd;       }

    Double_t GetRa() const        { return fRa;        }
    Double_t GetDec() const       { return fDec;       }
    Double_t GetHa() const        { return fHa;        }

    Double_t GetNominalZd() const { return fNominalZd; }
    Double_t GetNominalAz() const { return fNominalAz; }
    Double_t GetCurrentZd() const { return fCurrentZd; }
    Double_t GetCurrentAz() const { return fCurrentAz; }

    Double_t GetErrorZd() const   { return fErrorZd;   }
    Double_t GetErrorAz() const   { return fErrorAz;   }

    Double_t GetAbsError() const;

    void Print(Option_t *o="") const;

    ClassDef(MReportDrive, 1) // Class for DRIVE-REPORT information
};

#endif
