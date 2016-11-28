#ifndef MARS_MHThetaSqN
#define MARS_MHThetaSqN

#ifndef MARS_MHAlpha
#include "MHAlpha.h"
#endif

class TVector2;

class MGeomCam;
class MTaskList;
class MSrcPosCam;

class MHThetaSqN : public MHAlpha
{
private:
    MGeomCam    *fGeom;      //! conversion mm to deg
    MParameterD *fDisp;      //!
    MSrcPosCam  *fSrcPosCam; //!

    Double_t fThetaSqCut;    //!
    Double_t fSignificanceCutLevel;

    UInt_t fNumBinsSignal;
    UInt_t fNumBinsTotal;
    UInt_t fNumOffSourcePos;

    Bool_t fDoOffCut;

    TArrayI fCounter;        //!

    // MHThetaSqN
    TVector2 GetVec(const TVector2 &v, Int_t n1) const;
    void     SetVal(Double_t val);

    // MHAlpha
    Bool_t   GetParameter(const MParList &pl) { return kTRUE; }
    Double_t GetVal() const;
    void SetOffData(const MHAlpha &h)
    {
        const MHThetaSqN &t = (MHThetaSqN&)h;

        fDoOffCut        = t.fDoOffCut;
        fNumOffSourcePos = t.fNumOffSourcePos;

        MHAlpha::SetOffData(h);
    }

    // MH
    Bool_t SetupFill(const MParList *pl);
    Int_t  Fill(const MParContainer *par, const Stat_t weight);
    Bool_t Finalize();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

public:
    MHThetaSqN(const char *name=NULL, const char *title=NULL);
    ~MHThetaSqN();

    void InitMapping(MHMatrix *mat, Int_t type=0);

    void SetNumBinsSignal(UInt_t n)      { fNumBinsSignal  =TMath::Max(n, 1U); }
    void SetNumBinsTotal(UInt_t n)       { fNumBinsTotal   =TMath::Max(n, 1U); }
    void SetNumOffSourcePos(UInt_t n=3)  { fNumOffSourcePos=TMath::Max(n, 1U); }

    void SetSignificanceCutLevel(Double_t l=1.7) { fSignificanceCutLevel=l; }

    void SetDoOffCut(Bool_t b=kTRUE)     { fDoOffCut = b; }

    ClassDef(MHThetaSqN, 1) // Theta-Plot which is fitted online
};

#endif
