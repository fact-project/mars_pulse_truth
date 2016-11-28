#ifndef MARS_MParEmulated
#define MARS_MParEmulated

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MParEmulated : public MParContainer
{
private:
    Byte_t *fPtr;
    TString fClassName;

#ifndef __CINT__
#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
    TStreamerInfo *GetStreamerInfo(const TString &cls, TVirtualCollectionProxy* &proxy, Int_t &offset) const;
    TStreamerInfo *GetStreamerInfo(const TString &cls, Int_t &offset) const
    {
        TVirtualCollectionProxy *proxy = 0; return GetStreamerInfo(cls, proxy, offset);
    }
#else
    TVirtualStreamerInfo *GetStreamerInfo(const TString &cls, TVirtualCollectionProxy* &proxy, Int_t &offset) const;
    TVirtualStreamerInfo *GetStreamerInfo(const TString &cls, Int_t &offset) const
    {
        TVirtualCollectionProxy *proxy = 0; return GetStreamerInfo(cls, proxy, offset);
    }
#endif
#endif
    TMethodCall *GetMethodCall(const char *get, Int_t offset, TVirtualCollectionProxy *proxy=0) const;

public:
    MParEmulated(const char *name=0, const char *title=0);

    void SetClassName(const char *name) { fClassName=name; }

    Byte_t **GetPtr() { return &fPtr; }

    int      GetInt(Int_t offset) const { return *reinterpret_cast<int*>(fPtr+offset); }
    Int_t    GetInt_t(Int_t offset) const { return *reinterpret_cast<Int_t*>(fPtr+offset); }
    double   GetDouble(Int_t offset) const { return *reinterpret_cast<double*>(fPtr+offset); }
    unsigned long long GetULongLong(Int_t offset) const { return *reinterpret_cast<unsigned long long*>(fPtr+offset); }

    int      GetPInt(TVirtualCollectionProxy *proxy, Int_t offset) const;
    Int_t    GetPInt_t(TVirtualCollectionProxy *proxy, Int_t offset) const;
    double   GetPDouble(TVirtualCollectionProxy *proxy, Int_t offset) const;
    unsigned long long GetPULongLong(TVirtualCollectionProxy *proxy, Int_t offset) const;

    TMethodCall *GetterMethod(const char *name, TString cls, Int_t offset=0) const;
    TMethodCall *GetterMethod(const char *name) const { return GetterMethod(name, fClassName); }

    void Print(TPRegexp &regex, TString classname, TString prefix, Int_t offset) const;
    void Print(Option_t *o=0) const;

    ClassDef(MParEmulated, 0) // Storage container for emulated classes from files
};

#endif
