#ifndef MARS_MHMatrix
#define MARS_MHMatrix

#ifdef MARS_MLogManip
#error Please make ensure that MLogManip.h are included _after_ MHMatrix.h
#endif

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

class TArrayI;
class TArrayF;
class TH1F;

class MTask;
class MParList;
class MDataArray;
class MFilter;

class MHMatrix : public MH
{
private:
    static const TString gsDefName;  //! Default Name
    static const TString gsDefTitle; //! Default Title

    Int_t   fNumRows;   //! Number of rows filled into the matrix (this is NOT the number of rows of the matrix!)
    Int_t   fRow;       //! Present row
    TMatrix fM;         // Matrix to be filled

    TMatrix fM2;        //! Covariance Matrix

    MDataArray *fData;  // List of data members (columns)


    enum {
        kIsOwner  = BIT(14),
        kIsLocked = BIT(17)
    };

    void AddRow();
    Bool_t IsValid(const TMatrix &m) const
    {
        return m.GetNrows()>0 && m.IsValid();
    }

    Bool_t SetupFill(const MParList *pList);

    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void DrawDefRefInfo(const TH1 &hth, const TH1 &hthd, const TH1 &thsh, Int_t refcolumn);
    void GetRandomArrayI(TArrayI &ind) const;

    void StreamPrimitive(std::ostream &out) const;

public:
    MHMatrix(const char *name=NULL, const char *title=NULL);
    MHMatrix(MDataArray *mat, const char *name=NULL, const char *title=NULL);
    MHMatrix(const TMatrix &m, const char *name=NULL, const char *title=NULL);
    MHMatrix(const TMatrix &m, MDataArray *arr, const char *name=NULL, const char *title=NULL);
    ~MHMatrix();

    static void CopyCrop(TMatrix &target, const TMatrix &source, Int_t rows);

    void Lock()   { SetBit(kIsLocked); }
    void Unlock() { ResetBit(kIsLocked); }

    Int_t AddColumn(const char *name);
    void AddColumns(MDataArray *mat);
    void AddColumns(const TCollection &list);

    //MDataArray *GetColumns() { return fData; }
    const MDataArray *GetColumns() const { return fData; }
    MDataArray *GetColumns() { return fData; }

    const TMatrix &GetM() const { return fM; }

    Bool_t IsValid() const { return IsValid(fM); }
    Int_t  GetNumRows() const { return fM.GetNrows(); }

    //void Draw(Option_t *opt=NULL);
    //TObject *DrawClone(Option_t *opt=NULL) const;

    void Print(Option_t *) const;

    const TMatrix *InvertPosDef();

    Double_t CalcDist(const TMatrix &m, const TVector &v, Int_t num = 25) const;
    Double_t CalcDist(const TVector &v, Int_t num = 25);

    void SetOwner(Bool_t b=kTRUE) { b ? SetBit(kIsOwner) : ResetBit(kIsOwner); }

    void Reassign();

    const TArrayI GetIndexOfSortedColumn(Int_t ncol=0, Bool_t desc=kTRUE) const;
    void SortMatrixByColumn(Int_t ncol=0, Bool_t desc=kTRUE);

    Bool_t SetNumRow(Int_t row); 
 
    Int_t GetNumRow() const { return fRow; };
    Double_t operator[](Int_t col) { return fM(fRow, col); }

    void GetRow(TVector &v) const;
    void operator>>(TVector &v) const
    {
        GetRow(v);
    }

    Bool_t Fill(MParList *plist, MTask *read, MFilter *filter=NULL);

    TString GetDataMember() const;

    void ReduceNumberOfRows(UInt_t numrows, const TString opt);
    Bool_t RemoveInvalidRows();

    Int_t Read(const char *name);

    Bool_t DefRefMatrix(const UInt_t refcolumn, const TH1F &thsh,
                        Int_t nmaxevts=0, TMatrix *mrest=NULL);
    Bool_t DefRefMatrix(Int_t nmaxevts=0, TMatrix *mrest=NULL);

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    void ShuffleRows(UInt_t seed);
    void ReduceRows(UInt_t num);

    ClassDef(MHMatrix, 1) // Multidimensional Matrix (TMatrix) to store events
};

#endif


