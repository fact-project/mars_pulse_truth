#ifndef MARS_MHPixVsTime
#define MARS_MHPixVsTime

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TGraphErrors
#include <TGraphErrors.h>
#endif

class MHCamera;
class MCamEvent;
class MGeomCam;

class MRawEvtHeader;
class MTime;

class MHPixVsTime : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    TGraphErrors *fGraph;
    Int_t fIndex;

    MCamEvent *fEvt; //! the current event
    MGeomCam  *fCam; //! the camera geometry

    MRawEvtHeader *fHeader; //!
    MTime         *fTime;   //!

    TString fNameEvt;
    TString fNameTime;

    Int_t fType;
    Int_t fTypeErr;

    Double_t fMinimum;      // User defined minimum
    Double_t fMaximum;      // User defined maximum

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

public:
    MHPixVsTime(Int_t idx=0, const char *name=NULL, const char *title=NULL);
    ~MHPixVsTime();

    void SetNameEvt(const TString name) { fNameEvt = name; }
    void SetNameTime(const TString name) { fNameTime = name; }
    void SetType(Int_t type, Int_t e=-1) { fType = type; fTypeErr=e; }

    void SetMinimum(Double_t min=-1111) { fMinimum = min; }
    void SetMaximum(Double_t max=-1111) { fMaximum = max; }

    void SetName(const char *name);
    void SetTitle(const char *title);

    TH1 *GetHistByName(const TString name="") const;
    TGraph *GetGraph() { return fGraph; }

    void Draw(Option_t *o=NULL);

    ClassDef(MHPixVsTime, 2) // Histogram to sum camera events
};

#endif
