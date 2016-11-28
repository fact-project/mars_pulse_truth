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
!   Author(s): Thomas Bretz  5/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2008
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MObjLookup.h"

#include <TNamed.h>
#include <TObjString.h>

ClassImp(MObjLookup);

// --------------------------------------------------------------------------
//
// Delete all elements with kCenDelete set or all elements in case of
// kIsOwener is set
//
MObjLookup::~MObjLookup()
{
    TExMapIter iter(&fMap);

#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    Long_t key, value;
#else
    Long64_t key, value;
#endif

    while (iter.Next(key, value))
    {
        TObject *o = reinterpret_cast<TObject*>(value);
        if (o->TestBit(kCanDelete) || TestBit(kIsOwner))
            delete o;
    }
}

// --------------------------------------------------------------------------
//
// Add a TObjString to the table
//
void MObjLookup::Add(Long_t key, const char *txt)
{
    TObject *o=new TObjString(txt);

    o->SetBit(kCanDelete);

    Add(key,o);
}

// --------------------------------------------------------------------------
//
// Add a TNames to the table
//
void MObjLookup::Add(Long_t key, const char *name, const char *title)
{
    TObject *o=new TNamed(name, title);

    o->SetBit(kCanDelete);

    Add(key,o);
}

// --------------------------------------------------------------------------
//
// Search for the object corresponding to key. If no object is found,
// fDefault is returned instead.
//
TObject *MObjLookup::GetObj(Long_t key) const
{
    TObject *o = reinterpret_cast<TObject*>(const_cast<TExMap&>(fMap).GetValue(key));
    return o ? o : fDefault;
}
