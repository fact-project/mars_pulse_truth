#ifndef MARS_MHVsTime
#define MARS_MHVsTime

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

class TGraph;
class MData;

class MHVsTime : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

protected:
    // Could be const but root < 3.02/06 doesn't like this...
    TGraph     *fGraph;     // Histogram to fill
    MData      *fData;      // Object from which the data is filled
    MData      *fError;     // Object from which the error is filled
    Double_t    fScale;     // Scale for axis (eg unit)
    Int_t       fMaxPts;    // Maximum number of data points

    Int_t       fNumEvents; // Number of events to average

    Double_t    fMean;      //! Mean value
    Double_t    fMeanErr;   //! Mean error
    Int_t       fN;         //! Number of entries in fMean
    MTime       fLast;      //! For checks

    Double_t fMinimum;      // User defined minimum
    Double_t fMaximum;      // User defined maximum

    Double_t fMin;          //! Calculation of minimum
    Double_t fMax;          //! Calculation of maximum

    enum {
        kIsLogy = BIT(18)
    };

    Bool_t fUseEventNumber;

    Bool_t Finalize();

public:
    MHVsTime(const char *rule=NULL, const char *ruleerr=NULL);
    ~MHVsTime();

    void SetScale(Double_t scale) { fScale = scale; }

    void SetMinimum(Double_t min=-1111) { fMinimum = min; }
    void SetMaximum(Double_t max=-1111) { fMaximum = max; }

    Int_t GetNbins() const;

    void SetName(const char *name);
    void SetTitle(const char *title);

    void SetLogy(Bool_t b=kTRUE) { b ? SetBit(kIsLogy) : ResetBit(kIsLogy); }

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TString GetDataMember() const;
    TString GetRule() const;

    /*
     const TH1 *GetHist() const;
     TH1 *GetHist();
     TH1 *GetHistByName(const TString name="");
     */
    const TGraph *GetGraph() const { return fGraph; }
    TGraph *GetGraph() { return fGraph; }

    void SetUseEventNumber(Bool_t use = kTRUE) { fUseEventNumber = use; }
    void SetNumEvents(Int_t n) { fNumEvents=n; }

    void Draw(Option_t *opt=NULL);

    MParContainer *New() const;

    void SetMaxPts(Int_t n) { fMaxPts=n; }

    ClassDef(MHVsTime, 3) // Generalized 1/2/3D-histogram for Mars variables
};

#endif
