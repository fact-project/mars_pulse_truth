#ifndef MARS_MLut
#define MARS_MLut

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#include <iosfwd>

class MArrayI;

class MLut : public TObjArray
{
private:
    UInt_t fMinEntries;   // The minimum entries per row
    UInt_t fMaxEntries;   // The maximum entries per row

    Int_t  fMaxIndex;     // The maximum index fount in the lut

    MLut *GetInverse(Bool_t uniq=kTRUE) const;

public:
    MLut() : fMinEntries(0), fMaxEntries(0), fMaxIndex(0) { SetOwner(); }

    // TObjArry
    void Delete(Option_t *option="");

    // MLut Getter
    const MArrayI &GetRow(UInt_t i) const;

     Int_t GetNumRows() const { return GetEntriesFast(); }

    UInt_t GetMaxEntries() const { return fMaxEntries; }
     Int_t GetMaxIndex() const { return fMaxIndex; }

    Bool_t HasConstantLength() const { return fMinEntries==fMaxEntries; }
    Bool_t IsEmpty() const { return fMaxEntries==0; }
    Bool_t IsDefaultCol() const;
    Bool_t IsDefaultRow() const;

    // MLut conversions
    void SetInverse(const MLut &lut, Bool_t uniq=kTRUE);
    void Invert(Bool_t uniq=kTRUE);

    // Setter
    void SetDefaultCol(UInt_t n);
    void SetDefaultRow(UInt_t n);

    // MLut I/O
    Int_t ReadStream(std::istream &in);
    Int_t WriteStream(std::ostream &out) const;

    Int_t ReadFile(const char *fname);
    Int_t WriteFile(const char *fname) const;

    // TObject
    void Print(const Option_t *o="") const;
    void Print(const Option_t *o, Option_t *) const         { Print(o); }
    void Print(const Option_t *o, Int_t) const              { Print(o); }
    void Print(const Option_t *o, const char*, Int_t) const { Print(o); }
    void Print(const Option_t *o, TPRegexp&, Int_t) const   { Print(o); }

    ClassDef(MLut, 1) // A simple and fast easy-to-use look-up-table
};

#endif
