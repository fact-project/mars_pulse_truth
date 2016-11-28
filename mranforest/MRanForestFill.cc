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
!   Author(s): Thomas Hengstebeck 3/2003 <mailto:hengsteb@alwa02.physik.uni-siegen.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRanForestFill
//
//  Calculates the hadroness of an event. It calculates a mean value of all
//  classifications by the trees in a previously grown random forest.
//
//  To use only n trees for your calculation use:
//  MRanForestFill::SetUseNumTrees(n);
//
////////////////////////////////////////////////////////////////////////////
#include "MRanForestFill.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRanForest.h"

ClassImp(MRanForestFill);

using namespace std;

static const TString gsDefName  = "MRanForestFill";
static const TString gsDefTitle = "Tree Classification Loop";

// --------------------------------------------------------------------------
//
//
MRanForestFill::MRanForestFill(const char *name, const char *title):fNumTrees(-1)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Delete the data chains
//
MRanForestFill::~MRanForestFill()
{
    //    delete fData;
}

// --------------------------------------------------------------------------
Int_t MRanForestFill::PreProcess(MParList *plist)
{
    fRanTree = (MRanTree*)plist->FindObject("MRanTree");
    if (!fRanTree)
    {
        *fLog << err << dbginf << "MRanTree not found... aborting." << endl;
        return kFALSE;
    }

    fRanForest = (MRanForest*)plist->FindCreateObj("MRanForest");
    if (!fRanForest)
    {
        *fLog << err << dbginf << "MRanForest not found... aborting." << endl;
        return kFALSE;
    }

    fNum=0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MRanForestFill::Process()
{
    if (!(fRanForest->AddTree(fRanTree)))
        return kFALSE;

    fNum++;

    return fNumTrees<0 ? kTRUE : fNum<fNumTrees;
}

Int_t MRanForestFill::PostProcess()
{
    fRanForest->SetNumTrees(fNum);
    return kTRUE;
}

