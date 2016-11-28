#ifndef MARS_MArgs
#define MARS_MArgs

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

#ifndef ROOT_TList
#include <TList.h>
#endif

class MArgsEntry : public TString, public TObject
{
public:
    MArgsEntry(const char *c) : TString(c), TObject()
    {
        ReplaceAll("\015", "");
        dynamic_cast<TString&>(*this) = Strip(TString::kBoth);
    }

    void Print(const Option_t *o) const;

    ClassDef(MArgsEntry, 0)
};

class MArgs : public TNamed
{
private:
    Int_t fArgc; //!
    TList fArgv; //!

    MArgsEntry *GetArgument(Int_t i) const;
    MArgsEntry *GeOption(Int_t i) const;

public:
    MArgs(int argc, char **argv);

    // TObject
    void Print(const Option_t *o="") const;

    // FIXME: Add max, min option
    // FIXME: Add default option

    // MArgs
    Int_t    RemoveRootArgs();

    Int_t    GetInt(const TString name) const;
    Double_t GetFloat(const TString name) const;
    TString  GetString(const TString name) const;

    Int_t    GetIntAndRemove(const TString name);
    Double_t GetFloatAndRemove(const TString name);
    TString  GetStringAndRemove(const TString name);

    Int_t    GetIntAndRemove(const TString name, Int_t def);
    Double_t GetFloatAndRemove(const TString name, Double_t def);
    TString  GetStringAndRemove(const TString name, const TString def);

    Bool_t   Has(const TString name) const;
    Bool_t   HasOnly(const TString name) const;
    Bool_t   HasOption(const TString name) const;
    Bool_t   HasOnlyAndRemove(const TString name);

    TString  GetArgumentStr(Int_t i) const;
    Int_t    GetArgumentInt(Int_t i) const;
    Float_t  GetArgumentFloat(Int_t i) const;
    Int_t    GetNumArguments() const;
    Int_t    GetNumOptions() const;
    Int_t    GetNumEntries() const;

    Bool_t   RemoveArgument(Int_t i);

    TString  GetCommandLine() const;

    static TString GetCommandLine(int argc, char **argv) { return MArgs(argc, argv).GetCommandLine(); }

    ClassDef(MArgs, 0)  //Class to parse command line arguments
};

#endif
