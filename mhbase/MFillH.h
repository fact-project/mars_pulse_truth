#ifndef MARS_MFillH
#define MARS_MFillH

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MH;
class MMap;
class MData;
class MParameterD;
class MParList;

class TCanvas;

class MFillH : public MTask
{
public:
    enum {
        kDoNotDisplay = BIT(17),
        kCanSkip      = BIT(18)
    };

private:
    MParContainer *fParContainer; //! Pointer to the data container storing
    TString fParContainerName;    // Name to a data container

    MH* fH;                       // Pointer to the MH container to get filled
    TString fHName;               // Name to a MH container to get filled
    TString fNameTab;
    TString fTitleTab;

    MParameterD *fWeight;         //! Pointer to the container storing a weight
    TString fWeightName;          // Name of a container storing a weight

    MData *fIndex;                // MData object describing the 'key' to an automatic index for an MHArray
    MMap  *fMapIdx;               //! Map to map key-index-pair for an MHArray (MMap see MFillH.cc)

    TCanvas *fCanvas;             //! Canvas used to update a MStatusDisplay at the end of a loop

    TString fDrawOption;          // Draw option for status display

    TString ExtractName(const char *name) const;
    TString ExtractClass(const char *name) const;

    void Init(const char *name, const char *title);

    void StreamPrimitive(std::ostream &out) const;

    Bool_t DrawToDisplay();

public:
    MFillH();
    MFillH(const char *hist, const char *par=NULL, const char *name=NULL, const char *title=NULL);
    MFillH(const char *hist, MParContainer *par,   const char *name=NULL, const char *title=NULL);
    MFillH(MH *hist,         const char *par=NULL, const char *name=NULL, const char *title=NULL);
    MFillH(MH *hist,         MParContainer *par,   const char *name=NULL, const char *title=NULL);
    ~MFillH();

    void SetNameTab(const char *n="", const char *t="") { fNameTab = n; fTitleTab = t; }
    void SetTitleTab(const char *t="") { fTitleTab = t; }

    void SetRuleForIdx(const TString rule);
    void SetRuleForIdx(MData *rule);

    void ResetWeight() { fWeight=0; fWeightName=""; }
    void SetWeight(MParameterD *w)   { fWeight = w; }
    void SetWeight(const char *name="MWeight") { fWeightName = name; }

    void      SetDrawOption(Option_t *option="");
    Option_t *GetDrawOption() const { return fDrawOption; }

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

    TCanvas *GetCanvas() { return fCanvas; }

    ClassDef(MFillH, 3) // Task to fill a histogram with data from a parameter container
};
    
#endif

