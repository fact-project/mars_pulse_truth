#ifndef MARS_MString
#define MARS_MString

#ifndef ROOT_TString
#include <TString.h>
#endif

class MString : public TString
{
private:
    MString &Print(const char *fmt, va_list &ap);
//    MString &Print(const char *fmt, ...);

public:
    MString(const char *txt=0)  : TString(txt) { }
    MString(const TString &txt) : TString(txt) { }

#ifndef __CINT__
#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
    static TString Format(const char *fmt, ...);
#endif
#endif

    //void Form(const char *fmt, ...);

    ClassDef(MString, 1) // Tool to make Form() thread safe against other TStrings for root prior 5.18.00
};

#endif
