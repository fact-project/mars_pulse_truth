#ifndef MARS_MParameters
#define MARS_MParameters

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MParameterD : public MParContainer
{
private:
    Double_t fVal;

protected:
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MParameterD(const char *name=NULL, const char *title=NULL);

    void SetVal(Double_t v) { fVal = v; }
    Double_t GetVal() const { return fVal; }

    void Print(Option_t *o="") const;
    Bool_t SetupFits(fits &fin);

    Double_t *GetPtr() { return &fVal; }

    ClassDef(MParameterD, 1) // Container to hold a generalized parameters (double)
};

class MParameterDerr : public MParameterD
{
private:
    Double_t fErr;

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MParameterDerr(const char *name=NULL, const char *title=NULL);

    void SetVal(Double_t v)             { MParameterD::SetVal(v); }
    void SetVal(Double_t v, Double_t e) { SetVal(v); fErr = e; }
    Double_t GetErr() const { return fErr; }

    void Print(Option_t *o="") const;
    Bool_t SetupFits(fits &fin);

    ClassDef(MParameterDerr, 2) // Container to hold a generalized parameters (double) and its Error
};

class MParameterI : public MParContainer
{
private:
    Int_t fVal;

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MParameterI(const char *name=NULL, const char *title=NULL);

    void SetVal(Int_t v) { fVal = v; }
    Int_t GetVal() const { return fVal; }

    void Print(Option_t *o="") const;
    Bool_t SetupFits(fits &fin);

    Int_t *GetPtr() { return &fVal; }

    ClassDef(MParameterI, 1) // Container to hold a generalized parameters (integer)
};

/*
class MParameters : public MParContainer
{
private:
    TObjArray fList;
    TObjArray fNames;

public:
    MParameters(const char *name=NULL, const char *title=NULL)
    {
        fName  = name  ? name  : "MParameters";
        fTitle = title ? title : "Additional temporary parameters";

        SetReadyToSave();
    }

    MParamaterI &AddInteger(const TString name, const TString title, Int_t val=0)
    {
        MParameterI &p = *new MParameterI(name, title);
        p.SetValue(val);

        fList.Add(&p);

        TNamed &n = *new TNamed(name, title);
        fNames.Add(&n);

        return p;
    }

    MParameterD &AddDouble(const TString name, const TString title, Double_t val=0)
    {
        MParameterD &p = *new MParameterD(name, title);
        p.SetValue(val);

        fList.Add(&p);

        TNamed &n = *new TNamed(name, title);
        fNames.Add(&n);

        return p;
    }

    const TObjArray &GetList()
    {
        fList.SetNames(&fNames);
        return fList;
    }

    MParameterD *GetParameterD(const TString &name)
    {
        fList.SetNames(&fNames);
        return (MParamaterD*)fList.FindObject(name);
    }

    MParameterI *GetParameterI(const TString &name)
    {
        fList.SetNames(&fNames);
        return (MParameterI*)fList.FindObject(name);
    }

    ClassDef(MParameters, 1) // List to hold generalized parameters (MParameterD/I)
    }
    */
#endif
