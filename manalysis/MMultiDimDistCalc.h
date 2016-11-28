#ifndef MARS_MMultiDimDistCalc
#define MARS_MMultiDimDistCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MHMatrix;
class MParList;
class MDataArray;
class MParameterD;

class MMultiDimDistCalc : public MTask
{
private:
    Int_t  fNum;              // number of distances used for an avarage
    Bool_t fUseKernel;        // Flag whether kernel method should be used

    TString fHadronnessName;  // Name of container storing hadronness

    MHMatrix    *fMGammas;    //! Gammas describing matrix
    MHMatrix    *fMHadrons;   //! Hadrons (non gammas) describing matrix

    MParameterD *fHadronness; //! Output container for calculated hadroness

    MDataArray  *fData;       //! Used to store the MDataChains to get the event values

    void  StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *plist);
    Int_t Process();

public:
    MMultiDimDistCalc(const char *name=NULL, const char *title=NULL);

    void SetHadronnessName(const TString name) { fHadronnessName = name; }
    TString GetHadronnessName() const { return fHadronnessName; }

    void SetUseNumRows(UShort_t n=0) { fNum = n; }
    void SetUseKernelMethod(Bool_t k=kTRUE) { fUseKernel = k; }

    ClassDef(MMultiDimDistCalc, 1) // Task to calculate nearest neighbor-/kernel-hadronness
};

#endif



