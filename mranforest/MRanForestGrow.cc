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
!   Author(s): Thomas Hengstebeck 3/2003 <mailto:hengsteb@physik.hu-berlin.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRanForestGrow
//
//  Grows a random forest.
//
/////////////////////////////////////////////////////////////////////////////
#include "MRanForestGrow.h"

#include "MHMatrix.h" // must be before MLogManip.h

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MRanForest.h"

ClassImp(MRanForestGrow);

using namespace std;

const TString MRanForestGrow::gsDefName  = "MRead";
const TString MRanForestGrow::gsDefTitle = "Task to train a random forst";

MRanForestGrow::MRanForestGrow(const char *name, const char *title)
{
    //   set the name and title of this object

    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    //     SetNumTrees();
    //     SetNumTry();
    //     SetNdSize();
}

Int_t MRanForestGrow::PreProcess(MParList *plist)
{
    fMatrix = (MHMatrix*)plist->FindObject("MatrixTrain", "MHMatrix");
    if (!fMatrix)
    {
        *fLog << err << dbginf << "MatrixTrain [MHMatrix] not found... aborting." << endl;
        return kFALSE;
    }

    fRanForest = (MRanForest*)plist->FindCreateObj("MRanForest");
    if (!fRanForest)
    {
        *fLog << err << dbginf << "MRanForest not found... aborting." << endl;
        return kFALSE;
    }

    //     fRanForest->SetNumTry(fNumTry);
    //     fRanForest->SetNdSize(fNdSize);
    //     fRanForest->SetNumTrees(fNumTrees);

    return fRanForest->SetupGrow(fMatrix,plist);
}

Int_t MRanForestGrow::Process()
{
    return fRanForest->GrowForest();
}

Int_t MRanForestGrow::PostProcess()
{
    fRanForest->SetReadyToSave();

    return kTRUE;
}
