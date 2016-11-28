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
!   Author(s): Thomas Bretz  9/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMatrixLoop
//
// Use this task instead of reading from a file. It loops through the rows
// of a given MHMatrix. PreProcessing the task again starts from scratch.
//
/////////////////////////////////////////////////////////////////////////////
#include "MMatrixLoop.h"

#include "MHMatrix.h"
#include "MLog.h"

ClassImp(MMatrixLoop);

const TString MMatrixLoop::gsDefName  = "MRead";
const TString MMatrixLoop::gsDefTitle = "Loop through a MHMatrix (instead of reading from a file)";

// --------------------------------------------------------------------------
//
//  Default Constructor. Specify the MHMatrix object you want to loop
//  through.
//
MMatrixLoop::MMatrixLoop(MHMatrix *mat, const char *name, const char *title) : fMatrix(mat), fOperationMode(kDefault)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Return the number of events (rows) in the matrix
//
UInt_t MMatrixLoop::GetEntries()
{
    if (!fMatrix)
        return 0;

    return fOperationMode==kDefault ? fMatrix->GetNumRows() : (fMatrix->GetNumRows()+1)/2;
}

// --------------------------------------------------------------------------
//
// Return name of MHMatrix, <n/a> if not available
// The name of the matrix must nor contain a '/'
//
TString MMatrixLoop::GetFullFileName() const
{
    return fMatrix ? fMatrix->GetName() : "<n/a>";
}

// --------------------------------------------------------------------------
//
Int_t MMatrixLoop::PreProcess(MParList *plist)
{
    fNumRow = fOperationMode==kOdd ? 1 : 0;

    return fMatrix ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Set the present row of the Matrix, stops the loop when all rows have
// been accessed.
//
Int_t MMatrixLoop::Process()
{
    const Int_t rc = fMatrix->SetNumRow(fNumRow);
    fNumRow += fOperationMode==kDefault ? 1 : 2;
    return rc;
}
