#ifndef MARS_MObjLookup
#define MARS_MObjLookup

#ifndef ROOT_TExMap
#include <TExMap.h>
#endif

class MObjLookup : public TObject
{
private:
    TExMap   fMap;
    TObject *fDefault;

    enum {
        kIsOwner = BIT(14),
    };

public:
    MObjLookup() : fDefault(0) { }
    ~MObjLookup();

    // Add a new object
    void Add(Long_t key, TObject *obj) { fMap.Add(key, Long_t(obj)); }
    void Add(Long_t key, const char *txt);
    void Add(Long_t key, const char *name, const char *title);

    // Get an object
    TObject *GetObj(Long_t key) const;

    TObject *operator[](Long_t key) const { return GetObj(key); }

    const char *GetObjName(Long_t key) const { const TObject *o=GetObj(key); return o?o->GetName():NULL; }
    const char *GetObjTitle(Long_t key) const { const TObject *o=GetObj(key); return o?o->GetTitle():NULL; }

    // Number of entrues
    Int_t GetSize() const { return fMap.GetSize(); }

    // Owenership
    void SetOnwer(Bool_t b=kTRUE) { b ? SetBit(kIsOwner) : ResetBit(kIsOwner); }

    // Default returned if no obj found (deletion is user responsibility)
    void SetDefault(TObject *o) { fDefault = o; }
    TObject *GetDefault() { return fDefault; }

    // Direct access to the TExMap
    const TExMap &GetMap() const { return fMap; }
          TExMap &GetMap()       { return fMap; }

    ClassDef(MObjLookup, 1) // A class providing a fast lookup table key(int)->TObject
};

#endif
