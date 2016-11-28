/*====================================================================== *\
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
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MString
// =======
//
// If you are using root Form() command you must be aware of the fact that
// it uses a global buffer. This buffer is recreated depending on the
// length which is necessary to form the string. This recreation is not
// thread safe and it might result in crashes if used in multi-threaded
// environments.
//
// To get around this problem MString implements a Print() member function
// which form a string like Form does. This formation is done in a
// internal buffer. Because this buffer must be recreated and
// deleted each time Print() is called this might be slower than Form().
// The advantage is, that the buffer is not global and a call to Print()
// from different threads is safe. However accessing the same
// M/TString-object must still be locked with a mutex.
//
/////////////////////////////////////////////////////////////////////////////
#include "MString.h"

#include <stdarg.h>

ClassImp(MString);

// --------------------------------------------------------------------------
//
// Thread safe replacement for Form, use it like:
//
//  MString string;
//  string.Print("MyString has %d bytes", 128);
//
// As a special feature the function returns the reference to the MString
// so that you can directly work with it, eg.
//
//  string.Print("  MyString has %d bytes  ", 128).Strip(TString::kBoth);
//
MString &MString::Print(const char *fmt, va_list &ap)
{
    Int_t n=256;

    char *ret=0;

    while (1)
    {
        ret = new char[n+1];
        Int_t sz = vsnprintf(ret, n, fmt, ap);
        if (sz<=n)
            break;

        n *= 2;
        delete [] ret;
    };

    va_end(ap);

    *static_cast<TString*>(this) = ret;

    delete [] ret;

    return *this;
}

// --------------------------------------------------------------------------
//
// Thread safe replacement for Form, use it like:
//
//  MString string;
//  string.Print("MyString has %d bytes", 128);
//
// As a special feature the function returns the reference to the MString
// so that you can directly work with it, eg.
//
//  string.Print("  MyString has %d bytes  ", 128).Strip(TString::kBoth);
//
/*
MString &MString::Print(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    return Print(fmt, ap);
}
*/

// --------------------------------------------------------------------------
//
// Thread safe replacement for Form, use it like:
//
//  MString string;
//  string.Print("MyString has %d bytes", 128);
//
// As a special feature the function returns the reference to the MString
// so that you can directly work with it, eg.
//
//  string.Print("  MyString has %d bytes  ", 128).Strip(TString::kBoth);
//
// The static version of this function returns a copy(!) of the resulting
// M/TString.
//
#if ROOT_VERSION_CODE<ROOT_VERSION(5,18,00)
TString MString::Format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    MString ret;
    ret.Print(fmt, ap);
    return ret;
}
#endif

// --------------------------------------------------------------------------
//
// This is my implementation for root versions prior to 5.12/00 where
// TString::Form didn't exist.
//
/*
void MString::Form(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    Print(fmt, ap);
}
*/
