#ifndef MARS_MEnv
#define MARS_MEnv

#ifndef ROOT_TEnv
#include <TEnv.h>
#endif

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

class TAttLine;
class TAttMarker;
class TAttText;
class TAttFill;
class TPave;

class MArgs;

class MEnv : public TEnv
{
private:
    TOrdCollection fChecked; //!

    TString fName;

    TString Compile(TString str, const char *post) const;
    Int_t   ReadInclude();
    Bool_t  IsTEnvCompliant(const char *fname);

public:
    MEnv(const char *name="", const char *defname=0);

    Bool_t      IsValid() const { return !TString(GetRcName()).IsNull(); }

    TObject    *Clone(const char *newname="") const;

    Bool_t      Touch(const char *name) { return !TString(GetValue(name, "")).IsNull(); }

    Int_t       GetEntries() const;

    Int_t       GetValue(const char *name, Int_t dflt);
    Double_t    GetValue(const char *name, Double_t dflt);
    const char *GetValue(const char *name, const char *dflt="");

    // GetValue: regexp

    const char *GetName() const;
    void        SetName(const char *name=0) { fName = name; }

    Int_t       GetColor(const char *name, Int_t dftl);
    Int_t       GetAlign(const char *name, Int_t dftl);
    Int_t       GetFillStyle(const char *name, Int_t dftl);
    Int_t       GetLineStyle(const char *name, Int_t dftl);

    void        GetAttributes(const char *name, TObject *obj, TObject *dftl=0);
    void        GetAttLine(const char *name, TAttLine &line, TAttLine *dftl=0);
    void        GetAttText(const char *name, TAttText &text, TAttText *dftl=0);
    void        GetAttFill(const char *name, TAttFill &fill, TAttFill *dftl=0);
    void        GetAttMarker(const char *name, TAttMarker &marker, TAttMarker *dftl=0);
    void        GetAttPave(const char *name, TPave &pave, TPave *dftl=0);

    void        SetColor(const char *name, Int_t col);
    void        SetAlign(const char *name, Int_t align);
    void        SetFillStyle(const char *name, Int_t style);
    void        SetLineStyle(const char *name, Int_t style);
    void        SetMarkerStyle(const char *name, Int_t style);

    void        SetAttributes(const char *name, const TObject *obj);
    void        SetAttLine(const char *name, const TAttLine &line);
    void        SetAttText(const char *name, const TAttText &text);
    void        SetAttFill(const char *name, const TAttFill &fill);
    void        SetAttMarker(const char *name, const TAttMarker &marker);
    void        SetAttPave(const char *name, const TPave &pave);

    void        AddEnv(const TEnv &env, Bool_t overwrite=kTRUE);
    Bool_t      TakeEnv(MArgs &args, Bool_t print=kFALSE, Bool_t overwrite=kTRUE);

    Int_t       Read(const char *name=0);
    Int_t       ReadFile(const char *fname, EEnvLevel level);

    Int_t       WriteFile(const char *filename, EEnvLevel level) { return TEnv::WriteFile(filename, level); }
    Int_t       WriteFile(const char *filename) { return WriteFile(filename, kEnvLocal); } //*MENU*

    void        PrintEnv(EEnvLevel level = kEnvAll) const;
    void        Print(Option_t *option) const { TEnv::Print(option); }
    void        Print() const { PrintEnv(kEnvLocal); } //*MENU*

    void PrintUntouched() const;
    Int_t GetNumUntouched() const;

    ClassDef(MEnv, 1) // A slightly more advanced version of TEnv
};
    
#endif

