/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MLut
//
// This is a simple and easy-to-use implementation of a look-up table with
// rows of different lengths.
//
//////////////////////////////////////////////////////////////////////////////
#include "MLut.h"

#include <errno.h>
#include <fstream>

#include <stdlib.h>  // atoi (Ubuntu 8.10)

#include <TRegexp.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayI.h"

ClassImp(MLut);

using namespace std;

// --------------------------------------------------------------------------
//
// Get the i-th entry in the routing table. Note, that i is not
// range checked, but for any valid i a valid MArrayI is returned.
//
const MArrayI &MLut::GetRow(UInt_t i) const
{
    return *static_cast<MArrayI*>(UncheckedAt(i));
}

void MLut::Delete(Option_t *option)
{
    TObjArray::Delete(option);

    fMaxEntries = 0;
    fMinEntries = 0;

    fMaxIndex = -1;
}

// --------------------------------------------------------------------------
//
// Check if it is a default lut which would just map every entry to itself
// An empty Lut is a default column
//
Bool_t MLut::IsDefaultCol() const
{
    if (IsEmpty())
        return kTRUE;

    if (!HasConstantLength() || fMaxEntries!=1)
        return kFALSE;

    // Loop over all rows
    for (Int_t y=0; y<GetEntriesFast(); y++)
        if (GetRow(y)[0]!=y)
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Setup a default lut which just maps n-entris to themself
//
void MLut::SetDefaultCol(UInt_t n)
{
    Delete();

    for (UInt_t y=0; y<n; y++)
    {

        MArrayI &idx = *new MArrayI(1);
        idx[0] = y;
        Add(&idx);
    }

    fMinEntries = 1;
    fMaxEntries = 1;

    fMaxIndex = n;
}

// --------------------------------------------------------------------------
//
// Check if it is a default lut which would just map all entries to one.
// An empty Lut is a default row
//
Bool_t MLut::IsDefaultRow() const
{
    if (IsEmpty())
        return kTRUE;

    if (GetEntriesFast()!=1)
        return kFALSE;

    const MArrayI &idx = GetRow(0);

    // Loop over all rows
    for (UInt_t x=0; x<idx.GetSize(); x++)
        if (UInt_t(idx[x])!=x)
            return kFALSE;

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Setup a default lut which maps all n-entris to one
//
void MLut::SetDefaultRow(UInt_t n)
{
    Delete();

    MArrayI &idx = *new MArrayI(n);

    for (UInt_t y=0; y<n; y++)
        idx[y] = y;

    Add(&idx);

    fMinEntries = n;
    fMaxEntries = n;

    fMaxIndex = n;
}

// --------------------------------------------------------------------------
//
// Return the inverse lut of the current lut. The user is responsible
// of deleting the allocated and returned MLut.
//
// For an example see Inverse()
//
MLut *MLut::GetInverse(Bool_t uniq) const
{
    MLut *lut = new MLut;
    lut->SetInverse(*this, uniq);
    return lut;
}

// --------------------------------------------------------------------------
//
// Set this to the inverse lut of the given argument.
//
// For an example see Inverse()
//
void MLut::SetInverse(const MLut &lut, Bool_t uniq)
{
    if (&lut==this)
    {
        Invert(uniq);
        return;
    }

    // Delete the current lut
    Delete();

    // Add as many MArrayI as needed
    for (Int_t i=0; i<=lut.fMaxIndex; i++)
        Add(new MArrayI);

    // Get the number of rows in the given lut
    const UInt_t num = lut.GetEntriesFast();

    // Loop over all rows
    for (UInt_t y=0; y<num; y++)
    {
        // Get the y-th row
        const MArrayI &arr = lut.GetRow(y);

        // Loop over all entries in this row
        for (UInt_t x=0; x<arr.GetSize(); x++)
        {
            // If the entry is valid convert it to the right position in
            // the new lut
            if (arr[x]<0)
                continue;

            MArrayI &row = *static_cast<MArrayI*>(UncheckedAt(arr[x]));
            if (uniq)
                row.AddUniq(y);
            else
                row.Add(y);
        }
    }

    // Reset the statistics
    fMaxEntries = 0;
    fMinEntries = 0;

    // Loop over all new MArrayI
    for (Int_t i=0; i<=lut.fMaxIndex; i++)
    {
        // Get i-th row
        MArrayI &row = *static_cast<MArrayI*>(UncheckedAt(i));

        // Sort the i-th row ascending
        row.ReSort();

        // Get the number of entries in the row
        UInt_t n = row.GetSize();

        // For convinience (to avoid empty lines in a file)
        // add -1 to empty rows
        if (n==0)
        {
            row.Add(-1);
            n=1;
        }

        // Get the new min and max
        if (n<fMinEntries || fMinEntries==0)
            fMinEntries = n;
        if (n>fMaxEntries)
            fMaxEntries = n;
    }

    // set the new highest index in the table
    fMaxIndex = num-1;
}

// --------------------------------------------------------------------------
//
// Inverts the current lut, for example
//
//                         0: --
//     0: 1 2     ---\     1: 0
//     1: 2 3     ---/     2: 0 1
//                         3: 1
//
void MLut::Invert(Bool_t uniq)
{
    MLut *lut = GetInverse(uniq);

    // Keep fMaxIndex
    TObjArray::Delete();

    for (Int_t i=0; i<=fMaxIndex; i++)
       Add(lut->Remove(lut->UncheckedAt(i)));

    fMaxEntries = lut->fMaxEntries;
    fMinEntries = lut->fMinEntries;
    fMaxIndex   = lut->fMaxIndex;

    delete lut;
}

// --------------------------------------------------------------------------
//
// Read a lut from a stream.
//
//  The row in the lut is just identical to the line in the stream.
//  The entries in one line must be seperated by a whitespace.
//  The lines can have a different number of entries.
//
//  Lines which first(!) character is a # are ignored (as they are not
//  existing in the file)
//
Int_t MLut::ReadStream(istream &fin)
{
    static const TRegexp reg("[^ 1234567890]");

    Delete();

    Int_t l = 0;

    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        l++;

        if (line[0]=='#')
            continue;

        if (line.Contains(reg))
        {
            gLog << warn << "WARNING - MLut::ReadStream: Line #" << l << " contains invalid character... ignored." << endl;
            continue;
        }

        // Split line by whitespaces
        TObjArray *tokens = line.Tokenize(' ');

        // Get number of entries in this row
        const UInt_t n = tokens->GetEntries();

        // Calculate minimum and maximum numbers of entries per row
        if (n<fMinEntries || fMinEntries==0)
            fMinEntries = n;
        if (n>fMaxEntries)
            fMaxEntries = n;

        // create new entry for this row and add it to the array
        MArrayI &idx = *new MArrayI(n);
        Add(&idx);

        // loop over all entries in this row
        for (UInt_t i=0; i<n; i++)
        {
            // Convert strings to numbers
            idx[i] = atoi((*tokens)[i]->GetName());

            // Calculate maximum existing index
            if (idx[i]>fMaxIndex)
                fMaxIndex = idx[i];
        }

        // delete allocated TObjArray
        delete tokens;
    }

    return GetEntries();
}

// --------------------------------------------------------------------------
//
// Write a lut to a stream.
//
Int_t MLut::WriteStream(ostream &out) const
{
    const Int_t n = GetEntriesFast();
    for (int y=0; y<n; y++)
    {
       const MArrayI &arr = GetRow(y);

       if (arr.GetSize()==0)
           out << -1 << endl;

       for (UInt_t x=0; x<arr.GetSize(); x++)
           out << arr[x] << " ";
       out << endl;
    }

    return n;
}

// --------------------------------------------------------------------------
//
// Read a lut from a file (see also ReadStream)
//
Int_t MLut::ReadFile(const char *fname)
{
    TString expname(fname);
    gSystem->ExpandPathName(expname);

    ifstream fin(expname);
    if (!fin)
    {
        gLog << err << "Cannot open file " << expname << ": ";
        gLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
        return -1;
    }
    return ReadStream(fin);
}

// --------------------------------------------------------------------------
//
// Write a lut to a file
//
Int_t MLut::WriteFile(const char *fname) const
{
    TString expname(fname);
    gSystem->ExpandPathName(expname);

    ofstream fout(expname);
    if (!fout)
    {
        gLog << err << "Cannot open file " << expname << ": ";
        gLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
        return -1;
    }

    return WriteStream(fout);
}

void MLut::Print(const Option_t *o) const
{
    gLog << all;
    WriteStream(gLog);
}
