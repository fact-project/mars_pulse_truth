#ifndef MARS_MHCamEvent
#define MARS_MHCamEvent

#ifndef MARS_MH
#include "MH.h"
#endif

class MHCamera;
class MCamEvent;

class MHCamEvent : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

protected:
    MHCamera  *fSum;       // storing the sum
    MHCamera  *fErr;       //! storing the err
    MCamEvent *fEvt;       //! the current event
    MCamEvent *fSub;       //! the event to be subtracted

    TString fNameEvt;      // Name of MCamEvent to fill into histogram
    TString fNameSub;      // Name of MCamEvent to fill into histogram
    TString fNameGeom;     // Name of geometry container

    Int_t fType;           // Type to used for calling GetPixelContent
    Bool_t fErrorSpread;   // kFALSE==mean, kTRUE==spread
    Bool_t fErrorRelative; // Display relative error?

    Float_t fThreshold;    // Count pixel above/below this threshold
    Char_t fUseThreshold;  // Use a threshold? Which direction has it?

    Bool_t fIsRatio;       // If NameSub is set: Use Ratio or difference

    Int_t fNumReInit;      //!

    TString Format(const char *ext) const;

    void   Init(const char *name, const char *title);
    Bool_t InitGeom(const MParList &plist);

    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

public:
    enum { kIsLowerBound=1, kIsUpperBound=-1, kNoBound=0, kCollectMin=-2, kCollectMax=2, kMedianShift=3, kMeanShift=4 };

    MHCamEvent(const char *name=NULL, const char *title=NULL);
    MHCamEvent(Int_t type, const char *name=NULL, const char *title=NULL);
    ~MHCamEvent();

    void SetNameEvt(const TString name)  { fNameEvt = name; }
    void SetNameSub(const TString name, Bool_t ratio=kFALSE)  { fNameSub = name; fIsRatio = ratio;  }
    void SetNameGeom(const TString name) { fNameGeom = name; }
    void SetType(Int_t type) { fType = type; }

    void SetHist(const MHCamera &cam);
    MHCamera *GetHist() { return fSum; }
    const MHCamera *GetHist() const { return fSum; }

    TH1 *GetHistByName(const TString name="") const;

    void Paint(Option_t *o="");
    void Draw(Option_t *o="");  //*MENU*

    void PrintOutliers(Float_t s) const;

    void SetThreshold(Float_t f=0, Char_t direction=kIsLowerBound) { fThreshold = f; fUseThreshold=direction; }
    void SetCollectMin() { fUseThreshold=kCollectMin; }
    void SetCollectMax() { fUseThreshold=kCollectMax; }
    void SetMedianShift() { fUseThreshold=kMedianShift; }
    void SetMeanShift() { fUseThreshold=kMeanShift; }
    void SetErrorSpread(Bool_t b=kTRUE) { fErrorSpread = b; }
    void SetErrorRelative(Bool_t b=kTRUE) { fErrorRelative = b; }

    void RecursiveRemove(TObject *obj);

    ClassDef(MHCamEvent, 3) // Histogram to sum camera events
};

#endif
