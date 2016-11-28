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
!   Author(s): Thomas Bretz  04/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MDataElement
//
//  This MData class is used for accessing a column of an MHMatrix object.
//    eg. MDataElement("Matrix", 5)
//  will return the 5th column of the MHMatrix object called Matrix.
//
//  The row which is accessed must be set before by using
//  MHMatrix::SetNumRow. If you want to loop through a matrix use
//  MMatrixLoop.
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataElement.h"

#include <stdlib.h>   // atoi (Ubuntu 8.10)

#include <fstream>

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MDataElement);

using namespace std;

// --------------------------------------------------------------------------
//
// Specify the name of the MHMatrix-object and the column you want to
// access.
//
MDataElement::MDataElement(const char *member, Int_t col)
    : fMatrixName(member), fNumCol(col), fMatrix(NULL)
{
    if (fNumCol>=0)
        return;

    const Int_t pos = fMatrixName.First('[');
    if (pos<=0)
        return;

    fNumCol     = atoi(fMatrixName.Data()+pos+1);
    fMatrixName = fMatrixName(0, pos);
}

// --------------------------------------------------------------------------
//
// Specify the pointer of the MHMatrix-object and the column you want to
// access.
//
MDataElement::MDataElement(MHMatrix *mat, Int_t col)
    : fMatrixName(mat->GetName()), fNumCol(col), fMatrix(mat)
{
}

// --------------------------------------------------------------------------
//
// returns the value you requested
//
Double_t MDataElement::GetValue() const
{
    return fMatrix ? (*fMatrix)[fNumCol] : 0;
}

// --------------------------------------------------------------------------
//
//  If the pointer to the MHMatrix isn't yet set search for it in the
// parameter list.
//
Bool_t MDataElement::PreProcess(const MParList *plist)
{
    if (fMatrix)
        return kTRUE;

    fMatrix = (MHMatrix*)plist->FindObject(fMatrixName, "MHMatrix");
    if (!fMatrix)
    {
        *fLog << err << "MHMatrix '" << fMatrixName << "' not in parameter list... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the ready-to-save flag of the data member container
//
Bool_t MDataElement::IsReadyToSave() const
{
    return IsValid() ? fMatrix->IsReadyToSave() : kFALSE;
}

// --------------------------------------------------------------------------
//
// Return the rule used to identify this object in a datachain
//
TString MDataElement::GetRule() const
{
    TString rule = fMatrixName + "[";
    rule += fNumCol;
    rule += "]";
    return rule;
}
