#ifndef MARS_MRanTree
#define MARS_MRanTree

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

class TMatrixRow;
class TRandom;

class MArrayI;
class MArrayF;

class MRanTree : public MParContainer
{
private:
    Bool_t fClassify;

    Int_t fNdSize;
    Int_t fNumTry;

    Int_t fNumNodes;
    Int_t fNumEndNodes;
    Float_t fError;

    TArrayI fBestVar;
    TArrayI fTreeMap1;
    TArrayI fTreeMap2;
    TArrayF fBestSplit;
    TArrayF fGiniDec;

    int (MRanTree::*FindBestSplit)
        (const MArrayI &, const MArrayI &, const MArrayF &, const MArrayI &,
         Int_t, Int_t , const MArrayF &, const Float_t &, const Float_t &, Int_t &, Float_t &,
         Int_t &, const MArrayF &, const int); //!

    Double_t TreeHad(const Float_t *evt);

    int FindBestSplitGini(const MArrayI &datasort, const MArrayI &datarang,
                          const MArrayF &hadtrue, const MArrayI &idclass,
                          Int_t ndstart, Int_t ndend, const MArrayF &tclasspop,
                          const Float_t &mean, const Float_t &square, Int_t &msplit,
                          Float_t &decsplit, Int_t &nbest, const MArrayF &winbag,
                          const int nclass);

    int FindBestSplitSigma(const MArrayI &datasort, const MArrayI &datarang,
                           const MArrayF &hadtrue, const MArrayI &idclass,
                           Int_t ndstart, Int_t ndend, const MArrayF &tclasspop,
                           const Float_t &mean, const Float_t &square, Int_t &msplit,
                           Float_t &decsplit, Int_t &nbest, const MArrayF &winbag,
                           const int nclass);

    void MoveData(MArrayI &datasort, Int_t ndstart, Int_t ndend,
                  MArrayI &idmove, MArrayI &ncase, Int_t msplit,
                  Int_t nbest, Int_t &ndendl);

    void BuildTree(MArrayI &datasort, const MArrayI &datarang, const MArrayF &hadtrue,
                   const MArrayI &idclass,MArrayI &bestsplit,MArrayI &bestsplitnext,
                   const MArrayF &tclasspop, const Float_t &tmean, const Float_t &tsquare, const MArrayF &winbag,
                   Int_t ninbag, const int nclass);

public:
    MRanTree(const char *name=NULL, const char *title=NULL);
    MRanTree(const MRanTree &tree);

    void SetNdSize(Int_t n);
    void SetNumTry(Int_t n);
    void SetError(Float_t f) { fError = f; }

    Int_t GetNdSize() const { return fNdSize; }
    Int_t GetNumTry() const { return fNumTry; }
    Int_t GetNumNodes()          const { return fNumNodes; }
    Int_t GetNumEndNodes()       const { return fNumEndNodes; }
    Int_t IsClassify() const { return fClassify; }
    Float_t GetError() const { return fError; }

    Int_t GetBestVar(Int_t i)    const { return fBestVar.At(i); }
    Int_t GetTreeMap1(Int_t i)   const { return fTreeMap1.At(i); }
    Int_t GetTreeMap2(Int_t i)   const { return fTreeMap2.At(i); }
    Int_t GetNodeClass(Int_t i)  const { return fBestVar.At(i)+2; }
    Int_t GetNodeStatus(Int_t i) const { return TMath::Sign(1,fBestVar.At(i));}
    Float_t GetBestSplit(Int_t i)const { return fBestSplit.At(i); }

    Float_t GetGiniDec(Int_t i)  const { return fGiniDec.At(i); }

    void SetClassify(Bool_t n){ fClassify=n; }

    // functions used in tree growing process
    void GrowTree(TMatrix *mat, const MArrayF &hadtrue, const MArrayI &idclass,
                  MArrayI &datasort, const MArrayI &datarang,const MArrayF &tclasspop,
                  const Float_t &mean, const Float_t &square, const MArrayI &jinbag, const MArrayF &winbag,
                  const int nclass);

    Double_t TreeHad(const TVector &event);
    Double_t TreeHad(const TMatrixFRow_const &event);
    Double_t TreeHad(const TMatrix &m, Int_t ievt);

    Bool_t AsciiWrite(std::ostream &out) const;

    ClassDef(MRanTree, 1) // Storage container for tree structure
};

#endif
