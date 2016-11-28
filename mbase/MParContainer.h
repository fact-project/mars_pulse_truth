#ifndef MARS_MParContainer
#define MARS_MParContainer

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MParContainer                                                        //
//                                                                      //
// The basis for all parameter containers                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif
#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif

// gcc 3.2
#include <iosfwd>
//class ofstream;
//class ifstream;

class fits;

class TEnv;
class TArrayD;
class TDataMember;
class TMethodCall;

class MLog;
class MStatusDisplay;

class MParContainer : public TObject
{
    friend class MParEmulated;
private:
    static TObjArray fgListMethodCall; //!

protected:
    TString fName;        // parameter container identifier (name)
    TString fTitle;       // parameter container title

    MLog   *fLog;         // The general log facility for this object, initialized with the global object

    // This data member was added later, because for calculating the
    // Checksum root (3.02/07) ignores ClassDef=0 all data members
    // which are not persistent (//!) are used. To make the two
    // class versions CheckSum-compatible (only getting rid of a
    // warning) this member is persistent.
    MStatusDisplay *fDisplay; //!

private:
    enum {
        kIsSavedAsPrimitive = BIT(15)
    };

    Bool_t  fReadyToSave; // should be set to true if the contents of the container is changed somehow

    TClass *GetConstructor(const char *name) const;

public:
    enum {
        kEnableGraphicalOutput = BIT(16)
    };

    MParContainer(const char *name="", const char *title="");
    MParContainer(const TString &name, const TString &title);
    MParContainer(const MParContainer &named);
    MParContainer& operator=(const MParContainer& rhs);
    virtual ~MParContainer();

    virtual TObject    *Clone(const char *newname="") const;
    virtual Int_t       Compare(const TObject *obj) const;
    virtual void        Copy(TObject &named)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
const
#endif
        ;
    virtual void        FillBuffer(char *&buffer);

    static const TString GetDescriptor(const TObject &o);

    static Bool_t Overwrites(const TClass *base, const TObject &obj, const char *name, TClass *cls=NULL);

    virtual const TString GetDescriptor() const;
    virtual const TString GetUniqueName() const;
    virtual const char   *GetName() const       { return fName.Data(); }
    virtual const char   *GetTitle() const      { return fTitle.Data(); }
    virtual ULong_t       Hash() const          { return fName.Hash(); }
    virtual Bool_t        IsSortable() const    { return kTRUE; }

    virtual void   SetName(const char *name); // *MENU*
    virtual void   SetObject(const char *name, const char *title);
    virtual void   SetTitle(const char *title=""); // *MENU*
    virtual void   ls(Option_t *option="") const;
    virtual void   Print(Option_t *option="") const;
    virtual Int_t  Sizeof() const;
    virtual void   SavePrimitive(std::ostream &out, Option_t *o="");
    virtual void   SavePrimitive(std::ofstream &out, Option_t *o="");

    virtual MParContainer *New() const;
    virtual void   SetLogStream(MLog *lg) { fLog = lg; }
    virtual void   Reset();
    virtual Bool_t IsReadyToSave() const             { return fReadyToSave; }
    virtual void   SetReadyToSave(Bool_t flag=kTRUE) { fReadyToSave=flag; }
    virtual Bool_t IsSavedAsPrimitive() const        { return TestBit(kIsSavedAsPrimitive); }
    virtual void   EnableGraphicalOutput(Bool_t flag=kTRUE) { flag ? SetBit(kEnableGraphicalOutput) : ResetBit(kEnableGraphicalOutput);}
    virtual Bool_t IsGraphicalOutputEnabled() const  { return TestBit(kEnableGraphicalOutput); }
    virtual void   SetVariables(const TArrayD &)     { AbstractMethod("SetVariables(const TArrayD&)"); }

    virtual void SetDisplay(MStatusDisplay *d) { fDisplay = d; }

    virtual void StreamPrimitive(std::ostream &out) const;

    virtual TMethodCall *GetterMethod(const char *name) const;
    virtual void        *DataMember(const char *name);

    Bool_t WriteDataMember(std::ostream &out, const char *member, Double_t scale=1) const;
    Bool_t WriteDataMember(std::ostream &out, const TDataMember *member, Double_t scale=1) const;
    Bool_t WriteDataMember(std::ostream &out, const TList *list) const;

    virtual void AsciiRead(std::istream &fin);
    virtual Bool_t AsciiWrite(std::ostream &out) const;

    virtual Bool_t SetupFits(fits &fin) { return kTRUE; }

    Int_t Read(const char *name=NULL);

    virtual void GetNames(TObjArray &arr) const;
    virtual void SetNames(TObjArray &arr);

    virtual Int_t  TestEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);
    virtual Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);
    virtual Bool_t WriteEnv(TEnv &env, TString prefix, Bool_t print=kFALSE) const;

    Bool_t ReadEnv(const TEnv &env, Bool_t print=kFALSE) { return ReadEnv(env, "", print); }
    Bool_t WriteEnv(TEnv &env, Bool_t print=kFALSE) const { return WriteEnv(env, "", print); }

    Bool_t IsEnvDefined(const TEnv &env, TString prefix, TString postfix, Bool_t print) const;
    Bool_t IsEnvDefined(const TEnv &env, TString name, Bool_t print) const;

    Int_t       GetEnvValue(const TEnv &env, TString prefix, TString postfix, Int_t dflt) const;
    Double_t    GetEnvValue(const TEnv &env, TString prefix, TString postfix, Double_t dflt) const;
    const char *GetEnvValue(const TEnv &env, TString prefix, TString postfix, const char *dflt) const;

    Int_t       GetEnvValue(const TEnv &env, TString prefix, Int_t dflt) const;
    Double_t    GetEnvValue(const TEnv &env, TString prefix, Double_t dflt) const;
    const char *GetEnvValue(const TEnv &env, TString prefix, const char *dflt) const;

    template <class T> T GetEnvValue2Imp(const TEnv &env, const TString &prefix, const TString &postfix, T dflt, Bool_t print=kFALSE) const;
    Int_t       GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, Int_t dflt, Bool_t print=kFALSE) const;
    Double_t    GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, Double_t dflt, Bool_t print=kFALSE) const;
    const char *GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, const char *dflt, Bool_t print=kFALSE) const;

    TString     GetEnvValue3(const TEnv &env, const TString &prefix, TString id, UInt_t num) const;

    MParContainer *GetNewObject(const char *name, const char *base) const;
    MParContainer *GetNewObject(const char *name, TClass *base=MParContainer::Class()) const;

    static TClass *GetClass(const char *classname, TString &msg);
    static TClass *GetClass(const char *classname, MLog *log=NULL);

    void RecursiveRemove(TObject *obj);

    ClassDef(MParContainer, 0)  //The basis for all parameter containers
};

//!
//! Maybe we can add a static parameter list to MEvtLoop
//! Also we can derive MEvtLoop from MTaskList to have a static tasklist, too
//!

// FIXME: Move as (persistent) static data member to MParContainer
R__EXTERN TList *gListOfPrimitives; // instantiation in MEvtLoop

/*
class MParContainer : public TNamed
{
protected:
    MLog    *fLog;         //! The general log facility for this object, initialized with the global object

private:
    Bool_t   fReadyToSave; //! should be set to true if the contents of the container is changed somehow

public:
    MParContainer(const char *name="", const char *title="") : TNamed(name, title), fLog(&gLog), fReadyToSave(kFALSE) { }
    MParContainer(const TString &name, const TString &title) : TNamed(name, title), fLog(&gLog), fReadyToSave(kFALSE) { }

    void SetLogStream(MLog *lg)                      { fLog = lg; }

    virtual void   Reset()                           {  }

    virtual Bool_t IsReadyToSave()                   { return fReadyToSave; }
    virtual void   SetReadyToSave(Bool_t flag=kTRUE) { fReadyToSave=flag; }

    virtual void AsciiRead(ifstream &fin);
    virtual void AsciiWrite(ofstream &fout) const;

    ClassDef(MParContainer, 0)  //The basis for all parameter containers
};
*/
#endif


