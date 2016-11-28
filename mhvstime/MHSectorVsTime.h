/* ======================================================================== *\
!  $Name: not supported by cvs2svn $:$Id: MHSectorVsTime.h,v 1.9 2009-02-07 20:47:52 tbretz Exp $
\* ======================================================================== */
#ifndef MARS_MHSectorVsTime
#define MARS_MHSectorVsTime

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

class MHCamera;
class MCamEvent;
class MGeomCam;

class MRawEvtHeader;
class MTime;
class TGraph;

class MHSectorVsTime : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

private:
    TGraph *fGraph;         // The TGraph output to the display

    MCamEvent *fEvt;        //! pointer to the current event
    MGeomCam  *fCam;        //! pointer the camera geometry

    MRawEvtHeader *fHeader; //! pointer to the event header (DAQ Evt number instead of counter)
    MTime         *fTime;   //! pointer to event time (time instead of number)

    MHCamera fHCamera;      //! The camera to be used for calculation

    Double_t fMin;          //! Calculation of minimum
    Double_t fMax;          //! Calculation of maximum

    Double_t fMinimum;      // User defined minimum
    Double_t fMaximum;      // User defined maximum

    Bool_t fUseMedian;      // Whether to display median/dev instead of mean/rms

    TString fNameEvt;       // Name of MCamEvent to be displayed
    TString fNameTime;      // Name of MTime container to be used

    Int_t fType;            // Type for mean  used in GetPixelContent
    Int_t fTypeErr;         // Type for error used in GetPixelContent (-1 = no error)

    TArrayI fSectors;       // Which sectors shell be displayed
    TArrayI fAreaIndex;     // Which pixel sized should be displayed

    // MH
    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

public:
    MHSectorVsTime(const char *name=NULL, const char *title=NULL);
    ~MHSectorVsTime();

    // Setter
    void SetNameEvt(const TString name)  { fNameEvt = name; }
    void SetNameTime(const TString name) { fNameTime = name; }
    void SetType(Int_t type, Int_t e=-1) { fType = type; fTypeErr=e; }
    void SetSectors(const TArrayI &s)    { fSectors=s; }
    void SetAreaIndex(const TArrayI &a)  { fAreaIndex=a; }

    void SetMinimum(Double_t min=-1111) { fMinimum = min; }
    void SetMaximum(Double_t max=-1111) { fMaximum = max; }

    void SetUseMedian(Bool_t b=kTRUE) { fUseMedian=b; }

    // Getter
    TH1 *GetHistByName(const TString name="") const;
    TGraph *GetGraph() { return fGraph; }

    // TObject
    void SetName(const char *name);
    void SetTitle(const char *title);

    void Draw(Option_t *o=NULL);

    void RecursiveRemove(TObject *obj);

    ClassDef(MHSectorVsTime, 3) // Histogram to sum camera events
};

#endif
