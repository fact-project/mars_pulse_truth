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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCameraTH
//
// Class Version 2:
// ----------------
//   + Bool_t  fIsValid; // fTH contains valid information
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraTH.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MCameraTD.h"

ClassImp(MCameraTH);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MCameraTH::MCameraTH(Int_t size, const char *name, const char *title)
    : fTH(size), fIsValid(kTRUE)
{
    fName  = name  ? name  : "MCameraTH";
    fTitle = title ? title : "Storage container for the pixel discriminator threshold";
}

// --------------------------------------------------------------------------
//
// Interprete the TH (discriminator thresholds) part of the report
//
Bool_t MCameraTH::InterpreteTH(TString &str, Int_t ver, MCameraTD &td)
{
    // Skip the TH (discriminator thresholds) part of the report (for old
    // CC files with wrong or nonsense number of TH-Bytes)
    if (ver<200507190)
    {
        Ssiz_t pr = str.First(' ');
        if (pr<0)
        {
            *fLog << warn << "WARNING - No TH information found at all." << endl;
            return kFALSE;
        }
        if (pr!=1154)
        {
            td.Invalidate();

            str.Remove(0, pr);
            str=str.Strip(TString::kLeading);
            return kTRUE;
        }
    }

    const char *pos = str.Data();
    const char *end = str.Data()+577*2;

    Int_t i=0;
    while (pos<end)
    {
        const Char_t hex[3] = { pos[0], pos[1], 0 };
        pos += 2;

        const Int_t n=sscanf(hex, "%2hhx", &fTH[i++]);
        if (n==1)
            continue;

        *fLog << warn << "WARNING - Reading hexadecimal TH information." << endl;
        return kFALSE;
    }

    SetValid();

    str.Remove(0, end-str.Data()); // Remove TH
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print the discriminator thresholds
//
void MCameraTH::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    for (int i=0; i<fTH.GetSize(); i++)
        *fLog << " " << (int)fTH[i];
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Return the discriminator thresholds
//
Byte_t MCameraTH::GetMin() const
{
    Byte_t val = 0xff;
    for (int i=0; i<fTH.GetSize(); i++)
        val = TMath::Min(val, fTH[i]);
    return val;
}

// --------------------------------------------------------------------------
//
// Return the discriminator thresholds
//
Byte_t MCameraTH::GetMax() const
{
    Byte_t val = 0;
    for (int i=0; i<fTH.GetSize(); i++)
        val = TMath::Max(val, fTH[i]);
    return val;
}
