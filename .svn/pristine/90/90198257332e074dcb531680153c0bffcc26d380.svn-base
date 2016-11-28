#ifndef MARS_MHexagonFFT
#define MARS_MHexagonFFT

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_MArrayF
#include "MArrayF.h"
#endif
#ifndef ROOT_MArrayI
#include "MArrayI.h"
#endif

class MHexagonFFT : public TObject
{
private:
    static float P(int j, int n)
    {
        const Int_t a = (j+n)%34 ? 0 : 1;
        const Int_t b = j%34     ? 0 : 1;
        const Int_t c = n%34     ? 0 : 1;

        return 6./((1+a)*(1+b+c));
    }

    Int_t fNum;

    MArrayF fPsi;

    MArrayI fM;
    MArrayI fK;

    MArrayF fP;

    MArrayI fIdx;

public:
    MHexagonFFT();
    MHexagonFFT(Float_t scale, Int_t num)
    {
        Prepare(scale, num);
    }

    void Prepare(Float_t scale, Int_t num);

    void TransformFastFWD(const MArrayD &inre, MArrayD &outre, MArrayD &outim) const;
    void TransformFastBWD(const MArrayD &inre, const MArrayD &inim, MArrayD &outre) const;

    void TransformSlow(const MArrayD &inre,
                       const MArrayD &inim,
                       MArrayD &outre,
                       MArrayD &outim,
                       Float_t scale,
                       Bool_t fwd=kTRUE);
    /*
    void DGT2(const MArrayD &inre,
             const MArrayD &inim,
             MArrayD &outre,
             MArrayD &outim,
             Float_t scale,
             Bool_t fwd=kTRUE);
    void DGT3(const MArrayD &inre,
             const MArrayD &inim,
             MArrayD &outre,
             MArrayD &outim,
             Float_t scale,
             Bool_t fwd=kTRUE);
    void DGT4(const MArrayD &inre,
             const MArrayD &inim,
             MArrayD &outre,
             MArrayD &outim,
             Float_t scale,
             Bool_t fwd=kTRUE);
    */
    ClassDef(MHexagonFFT,0)
};
    
#endif
