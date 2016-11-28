#ifndef MARS_MHexagonalFT
#define MARS_MHexagonalFT

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_MArrayF
#include "MArrayF.h"
#endif
#ifndef ROOT_MArrayI
#include "MArrayI.h"
#endif

class MArrayD;
class MGeomCam;

class MHexagonalFT : public TObject
{
private:
    
    int Delta(int i) const
    {
        return i==0 || i==(Int_t)fNumRows-1 ? 1 : 0;
    } // Delta(i) = kronecker(i%(fNumRows-1), 0)
    float P(int j, int n) const
    {
        const Int_t a = Delta(j+n);
        const Int_t b = Delta(j);
        const Int_t c = Delta(n);

        return 6./((1+a)*(1+b+c));
    } // Definition of P(i,j) see paper

    UInt_t fNumRows;

    MArrayF fPsi;

    MArrayI fM;
    MArrayI fK;

    MArrayF fP;

    MArrayI fIdx;

public:
    MHexagonalFT();
    MHexagonalFT(Int_t num);

    void Prepare(Int_t num);

    void TransformFastFWD(const MArrayD &inre, MArrayD &outre, MArrayD &outim) const;
    void TransformFastBWD(const MArrayD &inre, const MArrayD &inim, MArrayD &outre) const;

    void TransformSlow(const MArrayD &inre, const MArrayD &inim,
                       MArrayD &outre, MArrayD &outim, Bool_t fwd=kTRUE);

    UInt_t GetNumKnots() const { return fP.GetSize(); } // Return number of knots in triangle
    UInt_t GetNumRows() const  { return fNumRows; }     // return number of rows in triangle

    UInt_t M(UShort_t i) const { return fM[i]; } // return m-axis-index of absolute index i
    UInt_t K(UShort_t i) const { return fK[i]; } // return k-axis-index of absolute index i

    Int_t GetRow(UShort_t i) const { return fM[i]+fK[i]; } // return row-index of absolute index i
    Int_t GetCol(UShort_t i) const { return fM[i]-fK[i]; } // return col-index of absolute index i

    ClassDef(MHexagonalFT,0) // general hexagonal fourier transformation
};
    
#endif
