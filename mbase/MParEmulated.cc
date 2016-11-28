/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz 07/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MParEmulated
//
// Storage Container for emulated branches
//
// Thanks to roots streaming mechanism simple branches can be recreated
// from a file. To read these kind of foreign branches the root system
// allocates memory. We can get a pointer to this memory and the
// offsets to the data, thus allowing to use this data and ecapsulate
// it into the MARS environment. This is done using MParEmulated.
//
/////////////////////////////////////////////////////////////////////////////
#include "MParEmulated.h"

#include <TClass.h>   // root >=5.20/00
#include <TPRegexp.h>
#include <TMethodCall.h>
#include <TStreamerElement.h>
#include <TVirtualCollectionProxy.h>
#if ROOT_VERSION_CODE>ROOT_VERSION(5,12,00)
#include <TVirtualStreamerInfo.h>
#endif

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

ClassImp(MParEmulated);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MParEmulated::MParEmulated(const char *name, const char *title)
    : fPtr(0)
{
    fName  = name  ? name  : "MParEmulated";
    fTitle = title ? title : "Parameter container emulating a class";
}

// --------------------------------------------------------------------------
//
// The default is to print all emulated data members. If an option is
// given it is interpreted as TPRegexp (regular expression) and only
// data members matching this regex are printed.
//
void MParEmulated::Print(Option_t *o) const
{
    TString opt(o);
    if (opt.IsNull())
        opt = ".*";

    TPRegexp regex(opt);
    Print(regex, fClassName, "", 0);
}

int MParEmulated::GetPInt(TVirtualCollectionProxy *proxy, Int_t offset) const
{
    const TVirtualCollectionProxy::TPushPop pp(proxy, fPtr);
    return *reinterpret_cast<int*>((char*)proxy->At(0)+offset);
}
Int_t MParEmulated::GetPInt_t(TVirtualCollectionProxy *proxy, Int_t offset) const
{
    const TVirtualCollectionProxy::TPushPop pp(proxy, fPtr);
    return *reinterpret_cast<Int_t*>((char*)proxy->At(0)+offset);
}
double MParEmulated::GetPDouble(TVirtualCollectionProxy *proxy, Int_t offset) const
{
    const TVirtualCollectionProxy::TPushPop pp(proxy, fPtr);
    return *reinterpret_cast<double*>((char*)proxy->At(0)+offset);
}
unsigned long long MParEmulated::GetPULongLong(TVirtualCollectionProxy *proxy, Int_t offset) const
{
    const TVirtualCollectionProxy::TPushPop pp(proxy, fPtr);
    return *reinterpret_cast<unsigned long long*>((char*)proxy->At(0)+offset);
}

// --------------------------------------------------------------------------
//
// Get the class with name clsname and its corresponding streamer info
//
#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
TStreamerInfo *MParEmulated::GetStreamerInfo(const TString &clsname, TVirtualCollectionProxy * &proxy, Int_t &offset) const
#else
TVirtualStreamerInfo *MParEmulated::GetStreamerInfo(const TString &clsname, TVirtualCollectionProxy * &proxy, Int_t &offset) const
#endif
{
    TClass *cls = gROOT->GetClass(clsname);
    if (!cls)
    {
        *fLog << err << dbginf << "ERROR - Class " << clsname << " not in dictionary." << endl;
        return 0;
    }

    proxy = cls->GetCollectionProxy();
    if (proxy)
    {
        cls = proxy->GetValueClass();

        proxy->PushProxy(fPtr);
        // proxy->GetSize()  // Number of elements in array
        if (proxy->At(0))
            offset = (char*)proxy->At(0)-(char*)fPtr;
        proxy->PopProxy();

    }

#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
    TStreamerInfo *info = cls->GetStreamerInfo();
#else
    TVirtualStreamerInfo *info = cls->GetStreamerInfo();
#endif
    if (!info)
    {
        *fLog << err << dbginf << "ERROR - No TStreamerInfo for class " << clsname << "." << endl;
        return 0;
    }

    return info;
}

// --------------------------------------------------------------------------
//
// Get the method call for the given method and offset, add method to
// MParContainer::fgListmethodCall
//
TMethodCall *MParEmulated::GetMethodCall(const char *get, Int_t offset, TVirtualCollectionProxy *proxy) const
{
    TString name(get);
    if (proxy)
        name.Prepend("P");
    name.Prepend("Get");

    TMethodCall *call = new TMethodCall(MParEmulated::Class(), name, proxy?MString::Format("%p,%d", proxy, offset):MString::Format("%d", offset));
    fgListMethodCall.Add(call);
    return call;
}

// --------------------------------------------------------------------------
//
// Get the getter method for the given data member. Since we have no real
// getter methods and no real data members we have to fake the TMethodCall.
//
TMethodCall *MParEmulated::GetterMethod(const char *name, TString clsname, Int_t offset) const
{
    TVirtualCollectionProxy *proxy = 0;

    // Get the streamer info for the class and the offset to the
    // first element of a possible array
    Int_t arroff = 0;

#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
    TStreamerInfo *info = GetStreamerInfo(clsname, proxy, arroff);
#else
    TVirtualStreamerInfo *info = GetStreamerInfo(clsname, proxy, arroff);
#endif
    if (!info)
        return 0;

    const TString arg(name);

    const Ssiz_t p = arg.Last('.');

    const TString nam = p<0 ? arg : arg(0, p);

    // Get the streamer element to the data member and
    // and the offset from the base of the object
    Int_t off;
    TStreamerElement *el = info->GetStreamerElement(nam, off);
    if (!el)
    {
        *fLog << err << dbginf << "ERROR - No TStreamerElement for " << nam << " [" << clsname << "]" << endl;
        return 0;
    }

    const TString type = el->GetTypeNameBasic();

    if (type=="int")
        return GetMethodCall("Int", arroff+offset+off, proxy);
    if (type=="Int_t")
        return GetMethodCall("Int_t", arroff+offset+off, proxy);
    if (type=="unisgned long long")
        return GetMethodCall("ULongLong", arroff+offset+off, proxy);
    if (type=="double")
    {
        cout << name << ": " << arroff << " " << offset << " " << off << " " << fPtr << endl;

        return GetMethodCall("Double", arroff+offset+off, proxy);
    }

    if (p<0)
    {
        *fLog << err << dbginf << "ERROR - Variable name missing for " << nam << "." << type << " [" << clsname << "]" << endl;
        return 0;
    }

    const TString var = arg(p+1, arg.Length());
    return GetterMethod(var, type, arroff+offset+off);
}

// --------------------------------------------------------------------------
//
//  Print the requested data from our memory using the streamer info.
//
void MParEmulated::Print(TPRegexp &regex, TString clsname, TString prefix, Int_t offset) const
{
    Int_t arroff = 0;

#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
    TStreamerInfo *info = GetStreamerInfo(clsname, arroff);
#else
    TVirtualStreamerInfo *info = GetStreamerInfo(clsname, arroff);
#endif
    if (!info)
        return;

    TIter Next(info->GetElements());
    TStreamerElement *el = 0;
    while ((el=(TStreamerElement*)Next()))
    {
        const TString str = prefix+el->GetName();

        if (str(regex).IsNull())
            continue;

        if (el->InheritsFrom(TStreamerBasicType::Class()))
        {
            const TString type(el->GetTypeNameBasic());

            cout << fName << (arroff?"[0]":"") << "." << str << " [" << type << "]"  << " \t";
            if (type=="int")
                cout << GetInt(el->GetOffset()+arroff+offset);
            if (type=="Int_t")
                cout << GetInt_t(el->GetOffset()+arroff+offset);
            if (type=="double")
                cout << GetDouble(el->GetOffset()+arroff+offset);
            if (type=="unsigned long long")
                cout << GetULongLong(el->GetOffset()+arroff+offset);

            cout << endl;
            continue;
        }

        if (el->InheritsFrom(TStreamerObjectAny::Class()))
        {
            Print(regex, el->GetTypeNameBasic(), str+".",
                  el->GetOffset()+arroff+offset);
            continue;
        }
    }
}
