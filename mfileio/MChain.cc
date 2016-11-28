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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MChain
//
// Helper class for MReadTree
//
/////////////////////////////////////////////////////////////////////////////
#include "MChain.h"

ClassImp(MChain);

using namespace std;

// --------------------------------------------------------------------------
//
// This is the code from TChain::LoadTree but skips the
// notification in LoadTree. If LoadTree raises the notification
// a flag is set and the notification is done by hand. This
// is done to be able to catch the return value from Notify. If
// it has not been successfull -15 is returned.
// This is to support return values from Notify()/Reinit().
// The status can be checked afterward by HasError/HasFatalError/GetError
//
Long64_t MChain::LoadTree(Long64_t entry)
{
    TObject *notify = GetNotify();

    SetNotify(this);

    Long64_t rc = TChain::LoadTree(entry);

    // <0: LoadTree failed
    // =0: LoadTree was ok
    // >0: Notify failed
    fLastError = rc;

    if (rc>=0 && fNotified && notify)
    {
        fLastError = notify->Notify() ? 0 : kFatalError;
        if (fLastError==kFatalError)
            rc = -15;
    }

    SetNotify(notify);

    return rc;
}
