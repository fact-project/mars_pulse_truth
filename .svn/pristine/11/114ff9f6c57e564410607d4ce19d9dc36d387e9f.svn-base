#ifndef MARS_MStatusArray
#define MARS_MStatusArray

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class TClass;
class TCanvas;
class TVirtualPad;

class MStatusDisplay;

class MStatusArray : public TObjArray
{
private:
    enum {
        kMyCanDelete = BIT(30)
    };

    void     SetCleanup(TObject *obj) const;
    Bool_t   RecursiveDelete(TVirtualPad *p, const char id=0) const;

    void     SetCanDelete(const TCollection *list) const;
    void     SetMyCanDelete(const TCollection *list) const;
    void     ResetMyCanDelete(const TCollection *list) const;
    void     PrintObjectsInPad(const TCollection *list, const TString &name, Int_t lvl=0) const;
    TObject *FindObjectInPad(TVirtualPad *pad, const char *object, TClass *base) const;
    void     RecursiveRemove(TCollection *c, TObject *o);

public:
    MStatusArray() : TObjArray() { }
    MStatusArray(const MStatusDisplay &d);
    ~MStatusArray();

    TObject *DisplayIn(Option_t *o=0) const;         // *MENU*
    void     DisplayIn(MStatusDisplay &d, const char *tab=0) const;
    TObject *Display() const { return DisplayIn(); } // *MENU*

    TCanvas *FindCanvas(const char *name) const;

    TObject *FindObjectInCanvas(const char *object, const char *base, const char *canvas) const;
    TObject *FindObjectInCanvas(const char *object, const char *canvas) const;

    void Print(Option_t *o="") const;
    void Print(const Option_t *o, Option_t *) const         { Print(o); }
    void Print(const Option_t *o, Int_t) const              { Print(o); }
    void Print(const Option_t *o, const char*, Int_t) const { Print(o); }
    void Print(const Option_t *o, TPRegexp&, Int_t) const   { Print(o); }

    TObject *FindObject(const char *object, const char *base) const;
    TObject *FindObject(const char *object) const;
    TObject *FindObject(const TObject *) const { return 0; }

    Int_t Read(const char *name=NULL);
    Int_t Write(const char *name=0, Int_t option=0, Int_t bufsize=0) const;
    Int_t Write(const char *name=0, Int_t option=0, Int_t bufsize=0)
    {
        return const_cast<const MStatusArray*>(this)->Write(name, option, bufsize);
    }

    void EnableTH1Workaround(const TCollection *list=0) const;
    void Delete(Option_t *option="");

    void RecursiveRemove(TObject *o);

    ClassDef(MStatusArray, 0) // Helper class for status display
};

#endif
