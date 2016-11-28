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
// MCameraRecTemp
//
// Class Version 2:
// ----------------
//   + Bool_t  fIsValid; // fTD contains valid information
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraRecTemp.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCameraRecTemp);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MCameraRecTemp::MCameraRecTemp(Int_t size, const char *name, const char *title)
    : fRecTemp(size), fIsValid(kTRUE)
{
    fName  = name  ? name  : "MCameraRecTemp";
    fTitle = title ? title : "Storage container for the receiver board temperatures";
}

// --------------------------------------------------------------------------
//
// Interprete the receiver board temperature part of the report
//
Bool_t MCameraRecTemp::InterpreteRecTemp(TString &str)
{
    Int_t len;
    for (Int_t i=0; i<76; i++)
    {
        const Int_t n=sscanf(str.Data(), "%f %n", &fRecTemp[i], &len);
        str.Remove(0, len);

        if (n==1)
            continue;

        if (n==0 && i==0)
        {
            *fLog << inf << "Receiver Board Temperatures empty." << endl;
            Invalidate();
            break;
        }

        *fLog << warn << "WARNING - Reading Receiver Board Temperature information." << endl;
        return kFALSE;
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Print the dc currents
//
void MCameraRecTemp::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    for (int i=0; i<fRecTemp.GetSize(); i++)
        *fLog << " " << fRecTemp[i];
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Return the minimum receiver board temperature
//
Float_t MCameraRecTemp::GetMin() const
{
    Float_t val = FLT_MAX;
    for (int i=0; i<fRecTemp.GetSize(); i++)
        val = TMath::Min(val, fRecTemp[i]);
    return val;
}

// --------------------------------------------------------------------------
//
// Return the maximum receiver board temperature
//
Float_t MCameraRecTemp::GetMax() const
{
    Float_t val = -FLT_MAX;
    for (int i=0; i<fRecTemp.GetSize(); i++)
        val = TMath::Max(val, fRecTemp[i]);
    return val;
}
