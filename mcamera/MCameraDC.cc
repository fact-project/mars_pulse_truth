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
!   Author(s): Thomas Bretz, 5/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCameraDC (PRELIMINARY)
//
//
// Version 2:
// ----------
//   + fStatus
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraDC.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCameraDC);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MCameraDC::MCameraDC(Int_t size, const char *name, const char *title)
    : fStatus(0), fArray(size)
{
    fName  = name  ? name  : "MCameraDC";
    fTitle = title ? title : "Storage container for the pixel currents";
}

// --------------------------------------------------------------------------
//
// Interprete a DC report context
//
Int_t MCameraDC::Interprete(TString &str, Int_t len)
{
    const char *pos = str.Data()+len;
    const char *end = pos+577*4;

    Int_t i=0;
    while (pos<end)
    {
        Int_t c;
        const Char_t hex[5] = { pos[0], pos[1], pos[2], pos[3], 0 };
        pos += 4;

        const Int_t nn=sscanf(hex, "%4x", &c);
        if (nn!=1)
        {
            *fLog << warn << "WARNING - Reading hexadecimal DC information." << endl;
            return kCONTINUE;
        }

        fArray[i++] = 0.001*c;
    }

    str.Remove(0, pos-str.Data()); // Remove DC currents
    str=str.Strip(TString::kLeading);

    return str.IsNull() ? kTRUE : kCONTINUE;
}

// --------------------------------------------------------------------------
//
// Print the dc currents
//
void MCameraDC::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    for (int i=0; i<fArray.GetSize(); i++)
        *fLog << " " << GetCurrent(i);
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Return the minimum dc current
//
Float_t MCameraDC::GetMin() const
{
    Float_t val = (UInt_t)-1;
    for (int i=0; i<fArray.GetSize(); i++)
        val = TMath::Min(val, GetCurrent(i));
    return val;
}

// --------------------------------------------------------------------------
//
// Return the maximum dc current
//
Float_t MCameraDC::GetMax() const
{
    Float_t val = 0;
    for (int i=0; i<fArray.GetSize(); i++)
        val = TMath::Max(val, GetCurrent(i));
    return val;
}
