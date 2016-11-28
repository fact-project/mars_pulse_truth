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
!   Author(s): Oscar Blanch 11/2002 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomPMT
//
// This is the base class of the PMT characteristics. 
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomPMT.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MGeomPMT);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes a Mirror geometry with 0 values, except for fMirrorID.
//
MGeomPMT::MGeomPMT(Int_t pmt, const char *name, const char *title)
    : fPMTId(pmt), fWavelength(0), fQE(0)
{
    fName  = name  ? name  : "MGeomPMT";
    fTitle = title ? title : "Storage container for  a PMT characteristics";
}

// --------------------------------------------------------------------------
//
// DESCRIPTION MISSING Please contact Oscar
//
void MGeomPMT::SetPMTContent(Int_t pmt, const TArrayF &wav, const TArrayF &qe)
{
    if (fWavelength.GetSize()!=wav.GetSize() ||
        fQE.GetSize()!=qe.GetSize())
    {
        *fLog << err << dbginf << " fWavelength or fQE do not have ";
        *fLog << "size of setting arrays" << endl;
        return;
    }

    fPMTId = pmt;

    fWavelength = wav;
    fQE = qe;
}
