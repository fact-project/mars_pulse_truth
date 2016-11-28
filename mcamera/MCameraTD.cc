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
!   Author(s): Florian Goebel 11/2005 <mailto:fgoebel@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCameraTD
//
// Class Version 2:
// ----------------
//   + Bool_t  fIsValid; // fTD contains valid information
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraTD.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCameraTD);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MCameraTD::MCameraTD(Int_t size, const char *name, const char *title)
    : fTD(size), fIsValid(kTRUE)
{
    fName  = name  ? name  : "MCameraTD";
    fTitle = title ? title : "Storage container for the pixel discriminator delays";
}

// --------------------------------------------------------------------------
//
// Interprete the TD (discriminator delays) part of the report
//
Bool_t MCameraTD::InterpreteTD(TString &str, Int_t ver)
{
    // Skip the TD (discriminator delays) part of the report (for old
    // CC files with wrong or nonsense number of TD-Bytes)
    if (ver<200412210)
    {
        Ssiz_t pr = str.First(' ');
        if (pr<0)
        {
            *fLog << warn << "WARNING - No TD information found at all." << endl;
            return kFALSE;
        }
        if (pr!=1000)
        {
            Invalidate();

            str.Remove(0, pr);
            str=str.Strip(TString::kLeading);
            return kTRUE;
        }
    }

    // Older files have less bytes (pixels) stored
    const Int_t numpix = ver<200510250 ? 500 : 577;

    const char *pos = str.Data();
    const char *end = str.Data()+numpix*2;

    Int_t i=0;
    while (pos<end)
    {
        const Char_t hex[3] = { pos[0], pos[1], 0 };
        pos += 2;

        const Int_t n=sscanf(hex, "%2hhx", &fTD[i++]);
        if (n==1)
            continue;

        *fLog << warn << "WARNING - Reading hexadecimal TD information." << endl;
        return kFALSE;
    }

    SetValid();

    str.Remove(0, end-str.Data()); // Remove TD
    str=str.Strip(TString::kLeading);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print the discrimintaor delays
//
void MCameraTD::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    for (int i=0; i<fTD.GetSize(); i++)
        *fLog << " " << (int)fTD[i];
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Return the discrimintaor delays
//
Byte_t MCameraTD::GetMin() const
{
    Byte_t val = 0xff;
    for (int i=0; i<fTD.GetSize(); i++)
        val = TMath::Min(val, fTD[i]);
    return val;
}

// --------------------------------------------------------------------------
//
// Return the discrimintaor delays
//
Byte_t MCameraTD::GetMax() const
{
    Byte_t val = 0;
    for (int i=0; i<fTD.GetSize(); i++)
        val = TMath::Max(val, fTD[i]);
    return val;
}
