#ifndef MARS_MRanForest
#define MARS_MRanForest

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

class TObjArray;

class MArrayI;
class MArrayF;

class MRanTree;
class MDataArray;
class MHMatrix;
class MParList;

class MRanForest : public MParContainer
{
private:
    Bool_t fClassify;

    Int_t fNumTrees;       // Number of trees
    Int_t fNumTry;         // Number of tries
    Int_t fNdSize;         // Size of node

    Int_t fTreeNo;         //! Number of tree

    MRanTree   *fRanTree;  //! Pointer to some tree
    MDataArray *fRules;    //! Pointer to corresponding rules
    TObjArray  *fForest;   //  Array containing forest

    // training data
    TMatrix *fMatrix;      //!

    // true  and estimated hadronness
    TArrayI fClass;        //!
    TArrayD fGrid;         //!
    TArrayF fHadTrue;      //!
    TArrayF fHadEst;       //!

    // data sorted according to parameters
    TArrayI fDataSort;     //!
    TArrayI fDataRang;     //!
    TArrayI fClassPop;     //!

    // weights
    TArrayF fWeight;       //!
    TArrayI fNTimesOutBag; //!

    // estimates for classification error of growing forest
    TArrayD fTreeHad;      //! Hadronness values (buffer for MHRanForest)

    Double_t fUserVal;     // A user value describing this tree (eg E-mc)

    Double_t EstimateError(const MArrayI &jinbag, Bool_t calcResolution);

protected:
    // create and modify (->due to bagging) fDataSort
    Bool_t CreateDataSort();
    void ModifyDataSort(MArrayI &datsortinbag, const MArrayI &jinbag);

public:
    MRanForest(const char *name=NULL, const char *title=NULL);
    MRanForest(const MRanForest &rf);
    ~MRanForest();

    void Print(Option_t *o="") const; //*MENU*

    void SetGrid(const TArrayD &grid);
    void SetWeights(const TArrayF &weights);
    void SetNumTrees(Int_t n);

    void SetNumTry(Int_t n);
    void SetNdSize(Int_t n);

    void SetClassify(Bool_t n){ fClassify=n; }
    void PrepareClasses();

        // tree growing
    Bool_t SetupGrow(MHMatrix *mat,MParList *plist);
    Bool_t GrowForest();
    void   SetCurTree(MRanTree *rantree) { fRanTree=rantree; }
    Bool_t AddTree(MRanTree *rantree);
    void   SetUserVal(Double_t d) { fUserVal = d; }

    // getter methods
    TObjArray  *GetForest() const      { return fForest; }
    MRanTree   *GetCurTree() const     { return fRanTree; }
    MRanTree   *GetTree(Int_t i) const;
    MDataArray *GetRules() const       { return fRules; }


    Int_t      GetNumTrees() const { return fNumTrees; }
    Int_t      GetNumData()  const;
    Int_t      GetNumDim()   const;
    Int_t      GetNdSize() const { return fNdSize; }
    Int_t      GetNclass()   const;
    Double_t   GetTreeHad(Int_t i) const { return fTreeHad.At(i); }
    Double_t   GetUserVal() const { return fUserVal; }
    Bool_t     IsClassify() const { return fClassify; }

    // use forest to calculate hadronness of event
    Double_t CalcHadroness(const TVector &event);
    Double_t CalcHadroness();

    Bool_t AsciiWrite(std::ostream &out) const;

    ClassDef(MRanForest, 1) // Storage container for tree structure
};

#endif
