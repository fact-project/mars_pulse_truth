#ifndef MARS_MDataChain
#define MARS_MDataChain

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MDataList                                                              //
//                                                                         //
//  List of several filters                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MData
#include "MData.h"
#endif

class MDataChain : public MData
{
private:
    MData *fMember; // Filter

    // PLEASE, always add new enums to the end of the enumeration,
    // otherwise you would break loading old data chains...
    typedef enum {
        kENoop,
        kELog, kELog2, kELog10,
        kEAbs,
        kESin, kECos, kETan, kESinH, kECosH, kETanH,
        kEASin, kEACos, kEATan, kEASinH, kEACosH, kEATanH,
        kESqrt, kESqr, kEPow10, kEExp,
        kESgn,
        kEPositive,
        kENegative,
        kEFloor, kECeil, kERound,
        kERad2Deg, kEDeg2Rad, kERandom,
        kERandomP, kERandomE, kERandomI, kERandomG, kERandomL,
        kEIsNaN, kEFinite
    } OperatorType_t;

    OperatorType_t fOperatorType;

    OperatorType_t ParseOperator(TString txt) const;

    Int_t IsAlNum(TString txt);
    Int_t GetBracket(TString txt, char open, char close);

    void   SimplifyString(TString &txt) const;
    void   AddParenthesis(TString &test, const TString op="^%*/+-") const;
    MData *ParseString(TString txt, Int_t level=0);
    MData *ParseDataMember(TString txt);

    MDataChain(const char *rule, OperatorType_t op);

public:
    MDataChain(const char *rule=NULL, const char *name=NULL, const char *title=NULL);
    ~MDataChain();

    // MData
    Double_t GetValue() const;

    Bool_t IsValid() const { return fMember ? kTRUE : kFALSE; }
    Bool_t PreProcess(const MParList *plist);

    TString GetRule() const;
    TString GetDataMember() const;
    //void    ReconstructElements() { if (fMember) fMember->ReconstructElements(); }

    // MParContainer
    Bool_t IsReadyToSave() const;
    void SetVariables(const TArrayD &arr);
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MDataChain, 1) // A chain/concatenation of MData objects
};

#endif
