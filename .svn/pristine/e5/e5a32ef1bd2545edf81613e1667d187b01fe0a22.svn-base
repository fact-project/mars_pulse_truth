#ifndef MARS_MHArray
#define MARS_MHArray

#ifndef MARS_MH
#include "MH.h"
#endif

class TList;
class TLegend;

class MMap;

class MHArray : public MH
{
private:
    Int_t  fIdx;              // current index for the fill function
    TList *fArray;            //-> Array storing the histograms

    const MParList *fParList; //! pointer to parameter list used for SetupFill when a new Hist is created
    TClass *fClass;           // pointer to class entry in root dictionary

    const MH *fTemplate;      //-> pointer to a template histogram
    TString fTemplateName;    // name of the template class

    MMap *fMapIdx;            //! Table to map keys to array indices

    Bool_t CreateH();
    void   AddLegendEntry(TLegend *leg, TObject *obj, Int_t idx) const;

    enum { kUseTemplate=BIT(14) };

    void Init(const char *name);

public:
    MHArray(const char *name=NULL, const char *title=NULL);
    MHArray(const TString hname, Bool_t istempl=kFALSE, const char *name=NULL, const char *title=NULL);
    MHArray(const MH *hist, const char *name=NULL, const char *title=NULL);
    ~MHArray();

    void Set(const TString hname, Bool_t istempl=kFALSE);
    void Set(const MH *hist);

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    Bool_t AddHistogram();

    MH &operator[](Int_t i);
    MH *At(Int_t i);

    MH *GetH();

    void SetIndexByKey(Double_t key);

    void SetIndex(Int_t i) { fIdx=i; }
    void IncIndex() { fIdx++; }
    void DecIndex() { fIdx--; }

    Int_t GetIndex() const { return fIdx; }

    void Print(Option_t *option="") const;

    void Draw(Option_t *opt="");
    TObject *DrawClone(Option_t *opt="") const;

    ClassDef(MHArray, 0) //A histogram class for an array of Mars histograms
};

#endif

