#ifndef MARS_MHCamEventRot
#define MARS_MHCamEventRot

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

class TH2D;

class MGeomCam;
class MParList;
class MTime;
class MPointingPos;
class MObservatory;

class MHCamEventRot : public MH
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    MGeomCam      *fGeom;        //! container storing the camera geometry
    MTime         *fTime;        //! container to take the event time from
    MPointingPos  *fPointPos;    //! container to take pointing position from
    MObservatory  *fObservatory; //! conteiner to take observatory location from

    TH2D    fHist;               // Alpha vs. x and y

    Int_t fType;                 // Type to used for calling GetPixelContent

    Double_t fRa;
    Double_t fDec;

    TString fNameTime;

    Float_t fThreshold;          // Count pixel above/below this threshold
    Char_t fUseThreshold;        // Use a threshold? Which direction has it?

    TObject *GetCatalog();

public:
    enum { kIsLowerBound=1, kIsUpperBound=-1, kNoBound=0 };

    MHCamEventRot(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const { return const_cast<TH2D*>(&fHist); }

    void SetNameTime(const char *name) { fNameTime=name; }

    void Draw(Option_t *option="");

    void SetThreshold(Float_t f, Char_t direction=kIsLowerBound) { fThreshold = f; fUseThreshold=direction; }

    ClassDef(MHCamEventRot, 1) //2D-histogram in MCamEvent data (derotated)
};

#endif
