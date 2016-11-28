#ifndef MARS_MPointingPositionCalc
#define MARS_MPointingPositionCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MPointing;
class MPointingDev;
class MRawRunHeader;
class MReportStarguider;

class MPointingDevCalc : public MTask
{
private:
    static const TString fgFilePrefix; //! default file name of pointing model

    MReportStarguider *fReport;    //! MReportStarguider to get mispointing
    MPointingDev      *fDeviation; //! Output container to store pointing deviation
    MPointing         *fPointing;  //! MPointing, pointing model for the calibration

    UShort_t fRunType;             //! Run Type to decide where to get pointing position from

    Double_t fNsbSum;              //! Sum of Nsb from Starguider
    Double_t fNsbSq;               //! Sum of Sq of Nsb from Starguider
    Int_t    fNsbCount;            //! Counter of Nsb entries from Starguider

    TArrayI  fSkip;                //! Counter for execution statistics
    Double_t fLastMjd;             //! Time of last processed report

    TString fFilePrefix;           // File name of pointing model
    TArrayI fPointingModels;       // List with pointing models

    UInt_t  fNumMinStars;          // [num] Minimum number of identified stars
    Float_t fNsbLevel;             // Minimum deviation from mean in sigma
    Float_t fNsbMin;               // [au] Minimum NSB to calc mean and rms
    Float_t fNsbMax;               // [au] Maximum NSB to calc mean and rms
    Float_t fMaxAbsDev;            // [arcmin] Maximum considered absolute deviation
    Float_t fMaxAge;               // [min] Maximum age of reports to be used without an update

    Float_t fDx;                   // [deg] Starguider calibration dx
    Float_t fDy;                   // [deg] Starguider calibration dy

    // MPointingDevCalc
    void DoCalibration(Double_t devzd, Double_t devaz) const;

    Bool_t ReadPointingModel(const MRawRunHeader &run);
    UInt_t FindPointingModel(UInt_t num);
    void   SortPointingModels();

    Int_t ProcessStarguiderReport();
    void  Skip(Int_t i);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Bool_t ReInit(MParList *plist);
    Int_t  PreProcess(MParList *plist);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MPointingDevCalc() : fReport(0), fDeviation(0), fPointing(0),
        fSkip(7), fFilePrefix(fgFilePrefix), fNumMinStars(8),
        fNsbLevel(3), fNsbMin(30), fNsbMax(60), fMaxAbsDev(15),
        fMaxAge(1), fDx(0), fDy(0)
    {
        fName  = "MPointingDevCalc";
        fTitle = "Task calculating the starguider correction";

        AddToBranchList("MReportStarguider.*");
    }
    ~MPointingDevCalc();

    // Tobject
    void Clear(Option_t *o="");

    // Setter
    void SetNumMinStars(UInt_t n)  { fNumMinStars=n; }
    void SetNsbLevel(Float_t lvl)  { fNsbLevel=lvl;  }
    void SetNsbMin(Float_t nsb)    { fNsbMin=nsb;    }
    void SetNsbMax(Float_t nsb)    { fNsbMax=nsb;    }
    void SetMaxAbsDev(Float_t max) { fMaxAbsDev=max; }
    void SetDx(Float_t dx)         { fDx=dx; }
    void SetDy(Float_t dy)         { fDy=dy; }
    void SetMaxAge(Float_t age)    { fMaxAge=age; }

    void SetFilePrefix(const char *n) { fFilePrefix=n; }

    // Handle pointing models
    void    SetPointingModels(const TString &models);
    void    AddPointingModel(UInt_t runnum);
    TString GetPointingModels() const;

    ClassDef(MPointingDevCalc, 0) //Task calculating the starguider correction
};

#endif
