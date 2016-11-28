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
!   Author(s): Thomas Bretz 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRawCrateArray
//
//  This class exists to make it possible to read in the crate data
//  TClones Array. In principal we can directly write the TClonesArray
//  to the root file, but when we read in again the root file we cannot
//  put the TClonesArray into our parameter list, becaus it isn't derived
//  from MParContainer. This class is derived from MParContainer and can be
//  put in the list. The TClones Array containes conatiners which store
//  the information about one crate (MRawCrateData).
//
/////////////////////////////////////////////////////////////////////////////
#include "MRawCrateArray.h"

#include <TClonesArray.h>

#include "MLog.h"
#include "MRawCrateData.h"

ClassImp(MRawCrateArray);

// --------------------------------------------------------------------------
//
//  Default Constructor. It creates the TClonesArray which is used to store
//  the crate data.
//
MRawCrateArray::MRawCrateArray(const char *name, const char *title)
{
    fName  = name  ? name  : "MRawCrateArray";
    fTitle = title ? title : "Array of MRawCrateData Information";

    //
    // craete an (almost) empty array. The size is easily determined
    // while filling the array
    //
    fArray = new TClonesArray("MRawCrateData", 0);
}

// --------------------------------------------------------------------------
//
//  Destructor. Deletes the TClones Array which stores the crate information
//
MRawCrateArray::~MRawCrateArray()
{
    //  FIXME: Is the contained data deleted, too?
    delete fArray;
}

// --------------------------------------------------------------------------
//
// clear the entries in the TClonesArray
//
void MRawCrateArray::Clear(Option_t *opt)
{
    fArray->Clear();
}

void MRawCrateArray::Print(Option_t *t) const
{
    fArray->Print();
}

void MRawCrateArray::SetSize(Int_t i)
{
    if (fArray->GetEntriesFast() == i)
        return;

    fArray->ExpandCreateFast(i);
}

Int_t MRawCrateArray::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Return a pointer the i-th entry in the array, without range check
//
MRawCrateData *MRawCrateArray::GetEntry(Int_t i)
{
    return (MRawCrateData*)fArray->UncheckedAt(i); // AddrAt would be with rcheck
}

// --------------------------------------------------------------------------
//
// Return the i-th entry in the array, with range check
//
MRawCrateData* &MRawCrateArray::operator[](Int_t i)
{
    return (MRawCrateData*&)(*fArray)[i];
}

// --------------------------------------------------------------------------
//
// return a pointer to the pointer of the array
// (actually not used. You may need it if you want to write
//  the TClonesArray directly)
//
TClonesArray **MRawCrateArray::GetArray()
{
    return &fArray;
}
